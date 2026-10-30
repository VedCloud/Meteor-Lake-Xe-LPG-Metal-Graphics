// NMXVirtioGPU: see nmx_virtio_gpu.h. Layouts follow the virtio 1.2 specification (4.1 PCI transport, 2.7 split
// virtqueues, 5.7 GPU device).

#include "nmx_virtio_gpu.h"
#include <mach/mach_time.h>
#include <IOKit/IOLib.h>
#include <libkern/OSAtomic.h>

#define VGLOG(fmt, ...) do { IOLog("NMXVGAFB: virtio-gpu: " fmt "\n", ##__VA_ARGS__); kprintf("NMXVGAFB: virtio-gpu: " fmt "\n", ##__VA_ARGS__); } while (0)

// ---- PCI transport ------------------------------------------------------------------------------------------------
enum { kCapVendor = 0x09, kCfgCommon = 1, kCfgNotify = 2, kCfgIsr = 3, kCfgDevice = 4 };
// common configuration (4.1.4.3)
enum {
    kDevFeatureSel = 0x00, kDevFeature = 0x04, kDrvFeatureSel = 0x08, kDrvFeature = 0x0c, kMsixConfig = 0x10,
    kNumQueues = 0x12, kStatus = 0x14, kCfgGeneration = 0x15, kQueueSel = 0x16, kQueueSize = 0x18,
    kQueueMsix = 0x1a, kQueueEnable = 0x1c, kQueueNotifyOff = 0x1e, kQueueDesc = 0x20, kQueueDriver = 0x28,
    kQueueDevice = 0x30,
};
enum { kStAck = 1, kStDriver = 2, kStDriverOk = 4, kStFeaturesOk = 8, kStNeedsReset = 0x40, kStFailed = 0x80 };
static constexpr uint64_t kFVersion1 = 1ull << 32, kFAccessPlatform = 1ull << 33;
static constexpr uint64_t kFGpuEdid = 1ull << 1, kFGpuBlob = 1ull << 3;

// ---- split virtqueue (2.7) ----------------------------------------------------------------------------------------
struct VqDesc { uint64_t addr; uint32_t len; uint16_t flags; uint16_t next; };
enum { kDescNext = 1, kDescWrite = 2 };
static constexpr uint16_t kMaxQ = 64;
// offsets in the ring buffer: descriptors, avail ring, used ring, then the response buffer; commands have their own
enum : uint32_t { kOffDesc = 0, kOffAvail = 1024, kOffUsed = 2048, kOffResp = 4096, kRingBytes = 8192 };
static constexpr uint32_t kCmdMax = 262144, kRespMax = 4096;    // a 16000-entry blob scatter list fits

// ---- GPU device (5.7.6) -------------------------------------------------------------------------------------------
enum : uint32_t {
    kCmdUpdateCursor = 0x0300, kCmdMoveCursor = 0x0301,
    kCmdGetDisplayInfo = 0x0100, kCmdResourceUnref = 0x0102, kCmdSetScanout = 0x0103, kCmdResourceFlush = 0x0104,
    kCmdGetEdid = 0x010a, kCmdResourceCreateBlob = 0x010c, kCmdSetScanoutBlob = 0x010d,
    kRespOkNoData = 0x1100, kRespOkDisplayInfo = 0x1101, kRespOkEdid = 0x1104,
};
enum : uint32_t { kBlobMemGuest = 1, kFormatB8G8R8X8 = 2, kFlagFence = 1 };
static constexpr uint16_t kNoVector = 0xffff;
#pragma pack(push, 1)
struct GpuHdr { uint32_t type, flags; uint64_t fenceId; uint32_t ctxId; uint8_t ringIdx, pad[3]; };
struct GpuRect { uint32_t x, y, w, h; };
struct GpuDisplayInfo { GpuHdr hdr; struct { GpuRect r; uint32_t enabled, flags; } pmodes[16]; };
struct GpuResourceUnref { GpuHdr hdr; uint32_t resourceId, pad; };
struct GpuGetEdid { GpuHdr hdr; uint32_t scanout, pad; };
struct GpuRespEdid { GpuHdr hdr; uint32_t size, pad; uint8_t edid[1024]; };
struct GpuSetScanout { GpuHdr hdr; GpuRect r; uint32_t scanoutId, resourceId; };
struct GpuResourceFlush { GpuHdr hdr; GpuRect r; uint32_t resourceId, pad; };
struct GpuMemEntry { uint64_t addr; uint32_t length, pad; };
struct GpuCreateBlob {
    GpuHdr hdr; uint32_t resourceId, blobMem, blobFlags, nrEntries; uint64_t blobId, size;
    GpuMemEntry entry[1];       // nrEntries of them
};
struct GpuSetScanoutBlob {
    GpuHdr hdr; GpuRect r; uint32_t scanoutId, resourceId, width, height, format, pad, strides[4], offsets[4];
};
struct GpuUpdateCursor {
    GpuHdr hdr; struct { uint32_t scanoutId, x, y, pad; } pos; uint32_t resourceId, hotX, hotY, pad;
};
#pragma pack(pop)
static_assert(sizeof(GpuHdr) == 24, "virtio_gpu_ctrl_hdr");
static_assert(sizeof(GpuDisplayInfo) == 24 + 16 * 24, "virtio_gpu_resp_display_info");
static_assert(sizeof(GpuCreateBlob) == 24 + 32 + 16, "virtio_gpu_resource_create_blob + one entry");
static_assert(sizeof(GpuSetScanoutBlob) == 24 + 16 + 24 + 32, "virtio_gpu_set_scanout_blob");
static_assert(sizeof(GpuUpdateCursor) == 56, "virtio_gpu_update_cursor");
// cursor queue layout in its buffer: descriptors, avail ring, used ring, command slots
enum : uint32_t { kCOffDesc = 0, kCOffAvail = 512, kCOffUsed = 1024, kCOffCmd = 2048, kCQMax = 32, kCRingBytes = 4096 };
static_assert(kCOffCmd + kCQMax * sizeof(GpuUpdateCursor) <= kCRingBytes, "cursor slots fit");

static inline void mb() { __asm__ __volatile__("mfence" ::: "memory"); }
static inline uint8_t  rd8 (volatile uint8_t *b, uint32_t o) { return *(volatile uint8_t *)(b + o); }
static inline uint16_t rd16(volatile uint8_t *b, uint32_t o) { return *(volatile uint16_t *)(b + o); }
static inline uint32_t rd32(volatile uint8_t *b, uint32_t o) { return *(volatile uint32_t *)(b + o); }
static inline void wr8 (volatile uint8_t *b, uint32_t o, uint8_t v)  { *(volatile uint8_t *)(b + o) = v; }
static inline void wr16(volatile uint8_t *b, uint32_t o, uint16_t v) { *(volatile uint16_t *)(b + o) = v; }
static inline void wr32(volatile uint8_t *b, uint32_t o, uint32_t v) { *(volatile uint32_t *)(b + o) = v; }
static inline void wr64(volatile uint8_t *b, uint32_t o, uint64_t v) { wr32(b, o, (uint32_t)v); wr32(b, o + 4, (uint32_t)(v >> 32)); }

// ---- interrupt -----------------------------------------------------------------------------------------------------
// the event source's owner: IOKit calls back an OSObject, this one forwards to the transport
class NMXVirtioGPUIrq : public OSObject {
    OSDeclareDefaultStructors(NMXVirtioGPUIrq)
public:
    NMXVirtioGPU *gpu = nullptr;
    bool init() override { return OSObject::init(); }
    static bool filter(OSObject *o, IOFilterInterruptEventSource *) { ((NMXVirtioGPUIrq *)o)->gpu->irqFilter(); return true; }
    static void action(OSObject *o, IOInterruptEventSource *, int) { ((NMXVirtioGPUIrq *)o)->gpu->irqAction(); }
};
OSDefineMetaClassAndStructors(NMXVirtioGPUIrq, OSObject)

// primary interrupt context: only the time (a message-signalled interrupt needs no ISR read or acknowledge)
void NMXVirtioGPU::irqFilter()
{
    fIrqAt = mach_absolute_time();
    fIrqs++;
}

// the work loop: wake the submitter, which sleeps on fLock until the used ring moves; pass on display events
void NMXVirtioGPU::irqAction()
{
    if (!fLock) return;
    // virtio_gpu_config: events_read, events_clear (5.7.4)
    if (fDevCfg && fReady) {
        const uint32_t ev = rd32(fDevCfg, 0);
        if (ev) {
            wr32(fDevCfg, 4, ev);
            if ((ev & 1) && fEventCb) fEventCb(fEventCtx);
        }
    }
    IOLockLock(fLock);
    IOLockWakeup(fLock, this, false);
    IOLockUnlock(fLock);
}

// one MSI-X vector on a work loop of our own; false leaves the transport polling
bool NMXVirtioGPU::setupIrq()
{
    if (!fPci->findPCICapability(kIOPCIMSIXCapability)) { VGLOG("no MSI-X capability - polling"); return false; }
    int index = -1;
    for (int pass = 0; pass < 2 && index < 0; pass++) {
        if (pass) {
            IOReturn r = fPci->configureInterrupts(kIOInterruptTypePCIMessagedX, 1, 1);
            if (r != kIOReturnSuccess) { VGLOG("MSI-X: configureInterrupts 0x%x - polling", r); return false; }
        }
        for (int i = 0; ; i++) {
            int type = 0;
            if (fPci->getInterruptType(i, &type) != kIOReturnSuccess) break;
            if (type & (kIOInterruptTypePCIMessaged | kIOInterruptTypePCIMessagedX)) { index = i; break; }
        }
    }
    if (index < 0) { VGLOG("MSI-X: no message-signalled interrupt source - polling"); return false; }
    fIrqOwner = OSTypeAlloc(NMXVirtioGPUIrq);
    if (!fIrqOwner || !fIrqOwner->init()) { OSSafeReleaseNULL(fIrqOwner); return false; }
    fIrqOwner->gpu = this;
    fWl = IOWorkLoop::workLoop();
    fIrq = fWl ? IOFilterInterruptEventSource::filterInterruptEventSource(fIrqOwner, &NMXVirtioGPUIrq::action,
                                                                          &NMXVirtioGPUIrq::filter, fPci, index) : nullptr;
    if (!fIrq || fWl->addEventSource(fIrq) != kIOReturnSuccess) {
        VGLOG("MSI-X: no interrupt event source - polling");
        teardownIrq();
        return false;
    }
    fIrq->enable();
    return true;
}

void NMXVirtioGPU::teardownIrq()
{
    fIrqOn = false;
    if (fIrq) {
        fIrq->disable();
        if (fWl) fWl->removeEventSource(fIrq);
        OSSafeReleaseNULL(fIrq);
    }
    OSSafeReleaseNULL(fWl);
    OSSafeReleaseNULL(fIrqOwner);
}

volatile uint8_t *NMXVirtioGPU::mapCap(uint8_t bar, uint32_t offset, uint32_t length)
{
    if (bar > 5) return nullptr;
    if (!fBarMap[bar]) {
        fBarMap[bar] = fPci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0 + bar * 4, kIOMapInhibitCache);
        if (!fBarMap[bar]) { VGLOG("BAR%u would not map", bar); return nullptr; }
    }
    if ((uint64_t)offset + length > fBarMap[bar]->getLength()) {
        VGLOG("capability [0x%x + 0x%x] lies outside BAR%u (0x%llx)", offset, length, bar,
              (unsigned long long)fBarMap[bar]->getLength());
        return nullptr;
    }
    return (volatile uint8_t *)(fBarMap[bar]->getVirtualAddress() + offset);
}

bool NMXVirtioGPU::init(IOPCIDevice *pci)
{
    fPci = pci;
    fLock = IOLockAlloc();
    if (!fLock) return false;
    // the vendor capabilities: the first of each type is the one to use (4.1.4)
    uint8_t off = pci->configRead8(kIOPCIConfigCapabilitiesPtr) & ~3;
    for (int guard = 0; off && guard < 48; guard++) {
        uint8_t id = pci->configRead8(off), next = pci->configRead8(off + 1) & ~3;
        if (id == kCapVendor) {
            uint8_t type = pci->configRead8(off + 3), bar = pci->configRead8(off + 4);
            uint32_t o = pci->configRead32(off + 8), len = pci->configRead32(off + 12);
            if (type == kCfgCommon && !fCommon) fCommon = mapCap(bar, o, len);
            else if (type == kCfgNotify && !fNotifyBase) { fNotifyBase = mapCap(bar, o, len); fNotifyMult = pci->configRead32(off + 16); }
            else if (type == kCfgIsr && !fIsr) fIsr = mapCap(bar, o, len);
            else if (type == kCfgDevice && !fDevCfg) fDevCfg = mapCap(bar, o, len);
        }
        off = next;
    }
    if (!fCommon || !fNotifyBase) { VGLOG("no modern virtio capabilities (common %p notify %p)", fCommon, fNotifyBase); reset(); return false; }
    pci->setMemoryEnable(true);
    pci->setBusLeadEnable(true);

    // reset, then ACKNOWLEDGE | DRIVER (3.1.1)
    wr8(fCommon, kStatus, 0);
    for (int i = 0; i < 1000 && rd8(fCommon, kStatus) != 0; i++) IODelay(100);
    wr8(fCommon, kStatus, kStAck);
    wr8(fCommon, kStatus, kStAck | kStDriver);
    wr32(fCommon, kDevFeatureSel, 0); uint64_t have = rd32(fCommon, kDevFeature);
    wr32(fCommon, kDevFeatureSel, 1); have |= (uint64_t)rd32(fCommon, kDevFeature) << 32;
    if (!(have & kFVersion1)) { VGLOG("device features 0x%llx lack VERSION_1", (unsigned long long)have); reset(); return false; }
    uint64_t want = kFVersion1 | (have & (kFGpuBlob | kFGpuEdid | kFAccessPlatform));
    wr32(fCommon, kDrvFeatureSel, 0); wr32(fCommon, kDrvFeature, (uint32_t)want);
    wr32(fCommon, kDrvFeatureSel, 1); wr32(fCommon, kDrvFeature, (uint32_t)(want >> 32));
    wr8(fCommon, kStatus, kStAck | kStDriver | kStFeaturesOk);
    if (!(rd8(fCommon, kStatus) & kStFeaturesOk)) { VGLOG("device refused features 0x%llx", (unsigned long long)want); reset(); return false; }
    fBlob = want & kFGpuBlob; fEdid = want & kFGpuEdid;
    const bool irq = setupIrq();
    if (!setupQueue()) { reset(); return false; }
    if (irq && !fIrqOn) { VGLOG("MSI-X: the device took no vector for the control queue - polling"); teardownIrq(); }
    if (!setupCursorQueue()) VGLOG("no cursor queue - the cursor stays in software");
    wr8(fCommon, kStatus, kStAck | kStDriver | kStFeaturesOk | kStDriverOk);
    fReady = true;
    VGLOG("up: device features 0x%llx, using 0x%llx (blob %s, edid %s), control queue %u entries, %s, cursor queue %u",
          (unsigned long long)have, (unsigned long long)want, fBlob ? "yes" : "NO", fEdid ? "yes" : "no", fQSize,
          fIrqOn ? "MSI-X" : "polled", fCursorQSize);
    return true;
}

bool NMXVirtioGPU::setupQueue()
{
    wr16(fCommon, kQueueSel, 0);
    uint16_t max = rd16(fCommon, kQueueSize);
    if (!max) { VGLOG("control queue missing"); return false; }
    fQSize = max < kMaxQ ? max : kMaxQ;
    wr16(fCommon, kQueueSize, fQSize);
    fRing = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
        kIODirectionInOut | kIOMemoryPhysicallyContiguous, kRingBytes, 0x000000FFFFFFF000ull);
    if (!fRing) { VGLOG("no memory for the control queue"); return false; }
    fRingKva = (uint8_t *)fRing->getBytesNoCopy();
    bzero(fRingKva, kRingBytes);
    fRingPhys = fRing->getPhysicalSegment(0, nullptr, kIOMemoryMapperNone);
    fCmd = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
        kIODirectionInOut | kIOMemoryPhysicallyContiguous, kCmdMax, 0x000000FFFFFFF000ull);
    if (!fCmd) { VGLOG("no memory for the command area"); return false; }
    fCmdKva = (uint8_t *)fCmd->getBytesNoCopy();
    fCmdPhys = fCmd->getPhysicalSegment(0, nullptr, kIOMemoryMapperNone);
    // the control queue and config changes (display events) interrupt on vector 0 when there is one
    wr16(fCommon, kMsixConfig, fIrq ? 0 : kNoVector);
    wr16(fCommon, kQueueMsix, fIrq ? 0 : kNoVector);
    fIrqOn = fIrq && rd16(fCommon, kQueueMsix) == 0;
    wr64(fCommon, kQueueDesc, fRingPhys + kOffDesc);
    wr64(fCommon, kQueueDriver, fRingPhys + kOffAvail);
    wr64(fCommon, kQueueDevice, fRingPhys + kOffUsed);
    uint16_t noff = rd16(fCommon, kQueueNotifyOff);
    fNotify = (volatile uint16_t *)(fNotifyBase + (uint32_t)noff * fNotifyMult);
    wr16(fCommon, kQueueEnable, 1);
    fAvailIdx = fUsedSeen = 0;
    return true;
}

// queue 1, no interrupt: a slot is free again once the used ring has moved past it
bool NMXVirtioGPU::setupCursorQueue()
{
    wr16(fCommon, kQueueSel, 1);
    uint16_t max = rd16(fCommon, kQueueSize);
    if (!max) return false;
    uint16_t qs = max < kCQMax ? max : (uint16_t)kCQMax;
    while (qs & (qs - 1)) qs &= (uint16_t)(qs - 1);          // a power of two, as a split ring needs
    fCursorLock = IOSimpleLockAlloc();
    fCursorQ = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
        kIODirectionInOut | kIOMemoryPhysicallyContiguous, kCRingBytes, 0x000000FFFFFFF000ull);
    if (!fCursorLock || !fCursorQ) {
        OSSafeReleaseNULL(fCursorQ);
        if (fCursorLock) { IOSimpleLockFree(fCursorLock); fCursorLock = nullptr; }
        return false;
    }
    fCursorKva = (uint8_t *)fCursorQ->getBytesNoCopy();
    bzero(fCursorKva, kCRingBytes);
    fCursorPhys = fCursorQ->getPhysicalSegment(0, nullptr, kIOMemoryMapperNone);
    VqDesc *d = (VqDesc *)(fCursorKva + kCOffDesc);
    for (uint16_t i = 0; i < qs; i++)
        d[i] = { fCursorPhys + kCOffCmd + i * sizeof(GpuUpdateCursor), (uint32_t)sizeof(GpuUpdateCursor), 0, 0 };
    fCursorQSize = qs;
    fCursorAvail = 0;
    wr16(fCommon, kQueueSize, qs);
    wr16(fCommon, kQueueMsix, kNoVector);
    wr64(fCommon, kQueueDesc, fCursorPhys + kCOffDesc);
    wr64(fCommon, kQueueDriver, fCursorPhys + kCOffAvail);
    wr64(fCommon, kQueueDevice, fCursorPhys + kCOffUsed);
    fCursorNotify = (volatile uint16_t *)(fNotifyBase + (uint32_t)rd16(fCommon, kQueueNotifyOff) * fNotifyMult);
    wr16(fCommon, kQueueEnable, 1);
    return true;
}

bool NMXVirtioGPU::cursorSubmit(uint32_t type, uint32_t resourceId, uint32_t hotX, uint32_t hotY, uint32_t x,
                                uint32_t y, bool mustSend)
{
    if (!fReady || !fCursorQ) return false;
    volatile uint16_t *avail = (volatile uint16_t *)(fCursorKva + kCOffAvail);
    volatile uint16_t *used = (volatile uint16_t *)(fCursorKva + kCOffUsed);
    for (int spin = 0; ; spin++) {
        IOSimpleLockLock(fCursorLock);
        mb();
        if ((uint16_t)(fCursorAvail - used[1]) < fCursorQSize) break;
        IOSimpleLockUnlock(fCursorLock);
        // full: the host hasn't caught up; a move is superseded by the next one anyway
        if (!mustSend || spin >= 1000) { fCursorDropped++; return false; }
        IODelay(10);
    }
    const uint16_t slot = fCursorAvail % fCursorQSize;
    GpuUpdateCursor *c = (GpuUpdateCursor *)(fCursorKva + kCOffCmd + slot * sizeof(GpuUpdateCursor));
    bzero(c, sizeof *c);
    c->hdr.type = type; c->pos.x = x; c->pos.y = y; c->resourceId = resourceId; c->hotX = hotX; c->hotY = hotY;
    avail[2 + slot] = slot;
    mb();
    avail[1] = ++fCursorAvail;
    mb();
    *fCursorNotify = 1;
    IOSimpleLockUnlock(fCursorLock);
    return true;
}

bool NMXVirtioGPU::cursorUpdate(uint32_t resourceId, uint32_t hotX, uint32_t hotY, uint32_t x, uint32_t y)
{
    return cursorSubmit(kCmdUpdateCursor, resourceId, hotX, hotY, x, y, true);
}

bool NMXVirtioGPU::cursorMove(uint32_t x, uint32_t y)
{
    return cursorSubmit(kCmdMoveCursor, 0, 0, 0, x, y, false);
}

void NMXVirtioGPU::reset()
{
    if (fCommon) wr8(fCommon, kStatus, 0);    // QEMU: back to VGA mode
    fReady = false;
    teardownIrq();
    OSSafeReleaseNULL(fRing);
    fRingKva = nullptr;
    OSSafeReleaseNULL(fCmd);
    fCmdKva = nullptr;
    OSSafeReleaseNULL(fCursorQ);
    fCursorKva = nullptr; fCursorNotify = nullptr; fCursorQSize = 0;
    if (fCursorLock) { IOSimpleLockFree(fCursorLock); fCursorLock = nullptr; }
    for (auto &m : fBarMap) OSSafeReleaseNULL(m);
    fCommon = fNotifyBase = fIsr = fDevCfg = nullptr;
    fNotify = nullptr;
    if (fLock) { IOLockFree(fLock); fLock = nullptr; }
}

uint32_t NMXVirtioGPU::submit(const void *cmd, uint32_t cmdLen, void *resp, uint32_t respLen, uint64_t *doneAt)
{
    Cmd c = { cmd, cmdLen, resp, respLen, 0 };
    return submitBatch(&c, 1, doneAt);
}

// command i at fCmdKva + i * kCmd1Off, its response at kOffResp + i * kResp1Off; descriptors 2i and 2i + 1
static constexpr uint32_t kCmd1Off = kCmdMax - 4096, kResp1Off = 2048;

uint32_t NMXVirtioGPU::submitBatch(Cmd *c, unsigned n, uint64_t *doneAt)
{
    if (!fReady || n < 1 || n > 2) return 0;
    for (unsigned i = 0; i < n; i++)
        if (c[i].len > (n == 1 ? kCmdMax : i ? 4096 : kCmd1Off) || c[i].respLen > (n == 1 ? kRespMax : kResp1Off)) return 0;
    IOLockLock(fLock);
    // One batch at a time: the wait below drops fLock, and a second submitter (the display event's GET_DISPLAY_INFO
    // during a flip) rewrote command 0 while the host had yet to read it - the flip's SET_SCANOUT_BLOB never arrived,
    // the used index ran out of step and every later command waited for good.
    if (fInFlight && fReady && fWaitLogs < 4) {
        fWaitLogs++;
        VGLOG("command 0x%x waits for the batch in flight", ((const GpuHdr *)c[0].cmd)->type);
    }
    while (fInFlight && fReady) IOLockSleep(fLock, &fInFlight, THREAD_UNINT);
    if (!fReady) { IOLockUnlock(fLock); return 0; }
    fInFlight = true;
    VqDesc *d = (VqDesc *)(fRingKva + kOffDesc);
    volatile uint16_t *avail = (volatile uint16_t *)(fRingKva + kOffAvail);   // flags, idx, ring[]
    for (unsigned i = 0; i < n; i++) {
        uint8_t *ca = fCmdKva + i * kCmd1Off;
        if (c[i].cmd != ca) memcpy(ca, c[i].cmd, c[i].len);
        bzero(fRingKva + kOffResp + i * kResp1Off, c[i].respLen);
        d[2 * i] = { fCmdPhys + i * kCmd1Off, c[i].len, kDescNext, (uint16_t)(2 * i + 1) };
        d[2 * i + 1] = { fRingPhys + kOffResp + i * kResp1Off, c[i].respLen, kDescWrite, 0 };
        avail[2 + ((fAvailIdx + i) % fQSize)] = (uint16_t)(2 * i);
    }
    mb();
    fAvailIdx = (uint16_t)(fAvailIdx + n);
    avail[1] = fAvailIdx;
    mb();
    *fNotify = 0;
    volatile uint16_t *used = (volatile uint16_t *)(fRingKva + kOffUsed);     // flags, idx, ring[]
    const uint16_t want = (uint16_t)(fUsedSeen + n);
    const uint64_t t0 = mach_absolute_time();
    uint64_t done = 0;
    bool answered = false, slow = false;
    // Answers normally take under a refresh. A host busy elsewhere (QEMU's GTK display re-creating its window
    // surface on a resize) can hold commands longer: wait up to 30 s, telling after 1 s, before giving virtio up.
    uint64_t slowAt, deadline;
    clock_interval_to_deadline(1, kSecondScale, &slowAt);
    clock_interval_to_deadline(30, kSecondScale, &deadline);
    for (;;) {
        mb();
        if (used[1] == want) {
            fUsedSeen = want;
            for (unsigned i = 0; i < n; i++) {
                const uint8_t *ra = fRingKva + kOffResp + i * kResp1Off;
                if (c[i].resp) memcpy(c[i].resp, ra, c[i].respLen);
                c[i].type = ((const GpuHdr *)ra)->type;
            }
            // the interrupt's time when it is this answer's (it came after the commands went out)
            const uint64_t at = fIrqAt;
            done = fIrqOn && at >= t0 ? at : mach_absolute_time();
            answered = true;
            break;
        }
        const uint64_t now = mach_absolute_time();
        if (now >= deadline) break;
        if (!slow && now >= slowAt) {
            slow = true;
            VGLOG("command 0x%x: no answer for 1 s - waiting (the host is stalled)", ((const GpuHdr *)c[0].cmd)->type);
        }
        const uint64_t until = slow ? deadline : slowAt;
        if (fIrqOn) { AbsoluteTime dl; memcpy(&dl, &until, sizeof dl); IOLockSleepDeadline(fLock, this, dl, THREAD_UNINT); }
        else IODelay(10);
    }
    if (doneAt) *doneAt = done;
    fCommands += n;
    {
        const GpuHdr *h = (const GpuHdr *)(fCmdKva + (n - 1) * kCmd1Off);
        Timing &tm = fTiming[h->type == kCmdResourceFlush ? (h->flags & kFlagFence ? 3 : 0) : h->type == kCmdSetScanoutBlob ? 1 : 2];
        uint64_t ns = 0; absolutetime_to_nanoseconds((done ? done : mach_absolute_time()) - t0, &ns);
        tm.n++; tm.totalNs += ns; if (ns > tm.maxNs) tm.maxNs = ns;
    }
    if (answered && slow) {
        uint64_t ns = 0; absolutetime_to_nanoseconds(done - t0, &ns);
        VGLOG("command 0x%x answered after %llu ms", ((const GpuHdr *)c[0].cmd)->type, ns / 1000000);
    }
    const uint32_t type = answered ? c[n - 1].type : 0;
    if (!answered) {
        fFailures++;
        // the device still owns the descriptors: no further command is safe
        fReady = false;
        VGLOG("command 0x%x timed out; virtio-gpu disabled", ((const GpuHdr *)c[0].cmd)->type);
    }
    fInFlight = false;
    IOLockWakeup(fLock, &fInFlight, false);
    IOLockUnlock(fLock);
    return type;
}

bool NMXVirtioGPU::submitNoData(const void *cmd, uint32_t cmdLen, const char *what)
{
    GpuHdr r;
    uint32_t t = submit(cmd, cmdLen, &r, sizeof r);
    if (t == kRespOkNoData) return true;
    if (t) { fFailures++; VGLOG("%s: response 0x%x", what, t); }
    return false;
}

bool NMXVirtioGPU::displayInfo(uint32_t *w, uint32_t *h, bool *enabled)
{
    GpuHdr c = {}; c.type = kCmdGetDisplayInfo;
    GpuDisplayInfo r;
    if (submit(&c, sizeof c, &r, sizeof r) != kRespOkDisplayInfo) return false;
    *w = r.pmodes[0].r.w; *h = r.pmodes[0].r.h; *enabled = r.pmodes[0].enabled != 0;
    return true;
}

bool NMXVirtioGPU::edid(uint8_t *buf, uint32_t cap, uint32_t *len)
{
    if (!fEdid) return false;
    GpuGetEdid c = {}; c.hdr.type = kCmdGetEdid; c.scanout = 0;
    GpuRespEdid *r = (GpuRespEdid *)IOMalloc(sizeof(GpuRespEdid));
    if (!r) return false;
    bool ok = submit(&c, sizeof c, r, sizeof *r) == kRespOkEdid && r->size >= 128 && r->size <= sizeof r->edid;
    if (ok) { *len = r->size < cap ? r->size : cap; memcpy(buf, r->edid, *len); }
    IOFree(r, sizeof(GpuRespEdid));
    return ok;
}

bool NMXVirtioGPU::createBlob(uint32_t resourceId, uint64_t phys, uint64_t size)
{
    GpuCreateBlob c = {};
    c.hdr.type = kCmdResourceCreateBlob;
    c.resourceId = resourceId; c.blobMem = kBlobMemGuest; c.blobFlags = 0; c.nrEntries = 1; c.size = size;
    c.entry[0] = { phys, (uint32_t)size, 0 };
    return submitNoData(&c, sizeof c, "RESOURCE_CREATE_BLOB");
}

bool NMXVirtioGPU::createBlobSG(uint32_t resourceId, const Range *r, uint32_t n, uint64_t size)
{
    if (!n || n > kMaxEntries) return false;
    const uint32_t len = (uint32_t)(sizeof(GpuCreateBlob) - sizeof(GpuMemEntry) + n * sizeof(GpuMemEntry));
    if (len > kCmdMax) return false;
    // built in a stack header plus the entries, then copied: the command area is shared by every command
    GpuCreateBlob c = {};
    c.hdr.type = kCmdResourceCreateBlob;
    c.resourceId = resourceId; c.blobMem = kBlobMemGuest; c.blobFlags = 0; c.nrEntries = n; c.size = size;
    uint8_t *buf = (uint8_t *)IOMalloc(len);
    if (!buf) return false;
    memcpy(buf, &c, sizeof(GpuCreateBlob) - sizeof(GpuMemEntry));
    GpuMemEntry *e = (GpuMemEntry *)(buf + sizeof(GpuCreateBlob) - sizeof(GpuMemEntry));
    for (uint32_t i = 0; i < n; i++) e[i] = { r[i].addr, r[i].len, 0 };
    bool ok = submitNoData(buf, len, "RESOURCE_CREATE_BLOB (scatter)");
    IOFree(buf, len);
    return ok;
}

bool NMXVirtioGPU::unrefResource(uint32_t resourceId)
{
    GpuResourceUnref c = {}; c.hdr.type = kCmdResourceUnref; c.resourceId = resourceId;
    return submitNoData(&c, sizeof c, "RESOURCE_UNREF");
}

bool NMXVirtioGPU::flipShown(uint32_t resourceId, uint32_t w, uint32_t h, uint32_t stride, uint64_t *shownAt)
{
    GpuSetScanoutBlob ss = {};
    ss.hdr.type = kCmdSetScanoutBlob;
    ss.r = { 0, 0, w, h };
    ss.scanoutId = 0; ss.resourceId = resourceId; ss.width = w; ss.height = h; ss.format = kFormatB8G8R8X8;
    ss.strides[0] = stride;
    GpuResourceFlush fl = {};
    fl.hdr.type = kCmdResourceFlush; fl.hdr.flags = kFlagFence; fl.r = { 0, 0, w, h }; fl.resourceId = resourceId;
    IOLockLock(fLock); fl.hdr.fenceId = ++fFenceId; IOLockUnlock(fLock);
    GpuHdr r0, r1;
    Cmd c[2] = { { &ss, sizeof ss, &r0, sizeof r0, 0 }, { &fl, sizeof fl, &r1, sizeof r1, 0 } };
    if (!submitBatch(c, 2, shownAt)) return false;
    if (c[0].type != kRespOkNoData || c[1].type != kRespOkNoData) {
        fFailures++;
        VGLOG("flip: SET_SCANOUT_BLOB 0x%x, RESOURCE_FLUSH (fenced) 0x%x", c[0].type, c[1].type);
        return false;
    }
    return true;
}

bool NMXVirtioGPU::setScanoutBlob(uint32_t resourceId, uint32_t w, uint32_t h, uint32_t stride)
{
    GpuSetScanoutBlob c = {};
    c.hdr.type = kCmdSetScanoutBlob;
    c.r = { 0, 0, w, h };
    c.scanoutId = 0; c.resourceId = resourceId; c.width = w; c.height = h; c.format = kFormatB8G8R8X8;
    c.strides[0] = stride;
    return submitNoData(&c, sizeof c, "SET_SCANOUT_BLOB");
}

bool NMXVirtioGPU::disableScanout()
{
    GpuSetScanout c = {}; c.hdr.type = kCmdSetScanout;
    return submitNoData(&c, sizeof c, "SET_SCANOUT(0)");
}

bool NMXVirtioGPU::flush(uint32_t resourceId, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    GpuResourceFlush c = {};
    c.hdr.type = kCmdResourceFlush; c.r = { x, y, w, h }; c.resourceId = resourceId;
    return submitNoData(&c, sizeof c, "RESOURCE_FLUSH");
}

bool NMXVirtioGPU::flushShown(uint32_t resourceId, uint32_t w, uint32_t h, uint64_t *shownAt)
{
    GpuResourceFlush c = {};
    c.hdr.type = kCmdResourceFlush; c.hdr.flags = kFlagFence; c.r = { 0, 0, w, h }; c.resourceId = resourceId;
    GpuHdr r;
    IOLockLock(fLock); c.hdr.fenceId = ++fFenceId; IOLockUnlock(fLock);
    uint32_t t = submit(&c, sizeof c, &r, sizeof r, shownAt);
    if (t == kRespOkNoData) return true;
    if (t) { fFailures++; VGLOG("RESOURCE_FLUSH (fenced): response 0x%x", t); }
    return false;
}
