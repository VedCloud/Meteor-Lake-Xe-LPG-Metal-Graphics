// NMXVirtioGPU: the virtio-gpu side of QEMU's virtio-vga, enough to show a guest-memory frame on the host.
//
// virtio-vga starts in VGA mode (the firmware's linear framebuffer in BAR0). Once the guest sets a virtio scanout,
// QEMU shows that instead; with blob=true (and a memfd-backed guest RAM) a scanout can be a "blob" resource made of
// guest pages, which QEMU exports to the host as a dmabuf (udmabuf) and its GL display shows without copying. A device
// reset puts QEMU back in VGA mode.
//
// One control queue, one command in flight: commands are rare (setup, then a flush per frame). Completion comes by
// MSI-X interrupt (the submitter sleeps), or by polling when the interrupt can't be set up.
#pragma once
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/IOLocks.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOFilterInterruptEventSource.h>

class NMXVirtioGPUIrq;

class NMXVirtioGPU {
public:
    // the virtio transport, feature negotiation and the control queue; false leaves the device in VGA mode
    bool init(IOPCIDevice *pci);
    // back to VGA mode (device reset) and everything freed
    void reset();
    bool ready() const { return fReady; }
    bool hasBlob() const { return fBlob; }
    bool hasEdid() const { return fEdid; }

    // scanout 0's preferred size (the host window), 0 x 0 when the host reports none
    bool displayInfo(uint32_t *w, uint32_t *h, bool *enabled);
    // scanout 0's EDID (VIRTIO_GPU_F_EDID): up to cap bytes into buf, *len the size the host gave
    bool edid(uint8_t *buf, uint32_t cap, uint32_t *len);
    // a guest-memory blob resource over one physically contiguous range
    bool createBlob(uint32_t resourceId, uint64_t phys, uint64_t size);
    // the same over a scatter list (addr/len pairs, page-aligned); at most kMaxEntries
    struct Range { uint64_t addr; uint32_t len; };
    static constexpr uint32_t kMaxEntries = 16000;     // QEMU takes up to 16384 (and the host udmabuf list_limit)
    bool createBlobSG(uint32_t resourceId, const Range *r, uint32_t n, uint64_t size);
    bool unrefResource(uint32_t resourceId);
    // shows the blob on scanout 0 (XRGB8888, little-endian: memory order B G R X)
    bool setScanoutBlob(uint32_t resourceId, uint32_t w, uint32_t h, uint32_t stride);
    // disables scanout 0 (resource 0): QEMU keeps the last frame
    bool disableScanout();
    // tells the host the rectangle changed
    bool flush(uint32_t resourceId, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    // the same, fenced: QEMU (patch 0007) answers it once its display has drawn the frame and lets the guest go on,
    // which with vblank-paced release is once per host refresh, at a fixed phase; a host without it answers at once.
    // *shownAt: mach time of the answer (the interrupt's, when interrupts are on)
    bool flushShown(uint32_t resourceId, uint32_t w, uint32_t h, uint64_t *shownAt);
    // a flip to another blob: SET_SCANOUT_BLOB and the fenced flush go out together (one notify, one wait), so the
    // host takes both in one pass instead of the guest waking for the first answer (~1.2 ms, measured)
    bool flipShown(uint32_t resourceId, uint32_t w, uint32_t h, uint32_t stride, uint64_t *shownAt);
    bool interrupts() const { return fIrqOn; }
    // VIRTIO_GPU_EVENT_DISPLAY (the host's display configuration changed: QEMU's window was resized) calls cb from
    // the interrupt's work loop; config changes interrupt on vector 0 too
    void setDisplayEventHandler(void (*cb)(void *), void *ctx) { fEventCtx = ctx; fEventCb = cb; }
    // The cursor queue: the host draws scanout 0's cursor from a 64x64 ARGB32 resource (QEMU's GTK display makes it
    // the window's own pointer). Fire and forget, never sleeps (IOGraphics moves the cursor from contexts that can't);
    // a move finding the queue full is dropped, an image update is not (it waits for a slot).
    bool hasCursor() const { return fCursorQ != nullptr; }
    bool cursorUpdate(uint32_t resourceId, uint32_t hotX, uint32_t hotY, uint32_t x, uint32_t y);
    bool cursorMove(uint32_t x, uint32_t y);

    uint64_t commands() const { return fCommands; }
    uint64_t failures() const { return fFailures; }
    // round-trip time per command kind (0 flush, 1 set scanout, 2 other, 3 fenced flush): count, total and longest, ns
    static constexpr unsigned kTimings = 4;
    struct Timing { uint64_t n, totalNs, maxNs; };
    Timing timing(unsigned kind) const { return kind < kTimings ? fTiming[kind] : Timing{}; }
    uint64_t irqs() const { return fIrqs; }

    // from the interrupt event source (NMXVirtioGPUIrq)
    void irqFilter();
    void irqAction();

private:
    IOPCIDevice *fPci = nullptr;
    IOMemoryMap *fBarMap[6] = {};
    volatile uint8_t *fCommon = nullptr, *fNotifyBase = nullptr, *fIsr = nullptr, *fDevCfg = nullptr;
    uint32_t fNotifyMult = 0;
    volatile uint16_t *fNotify = nullptr;      // the control queue's notify register
    IOBufferMemoryDescriptor *fRing = nullptr; // descriptors, avail and used rings, then the response area
    uint64_t fRingPhys = 0;
    uint8_t *fRingKva = nullptr;
    IOBufferMemoryDescriptor *fCmd = nullptr;  // the command area (a blob's scatter list makes it large)
    // cursor queue: descriptors, rings, then one command slot per descriptor
    IOBufferMemoryDescriptor *fCursorQ = nullptr;
    uint8_t *fCursorKva = nullptr;
    uint64_t fCursorPhys = 0;
    volatile uint16_t *fCursorNotify = nullptr;
    uint16_t fCursorQSize = 0, fCursorAvail = 0;
    IOSimpleLock *fCursorLock = nullptr;
    uint64_t fCursorDropped = 0;
    uint64_t fCmdPhys = 0;
    uint8_t *fCmdKva = nullptr;
    uint16_t fQSize = 0, fAvailIdx = 0, fUsedSeen = 0;
    IOLock *fLock = nullptr;
    bool fInFlight = false;               // a batch owns the descriptors and buffers until it is answered
    unsigned fWaitLogs = 0;
    bool fReady = false, fBlob = false, fEdid = false;
    uint64_t fCommands = 0, fFailures = 0, fFenceId = 0;
    Timing fTiming[kTimings] = {};
    // MSI-X: vector 0 is the control queue's; the filter stamps the time, the action wakes the submitter
    NMXVirtioGPUIrq *fIrqOwner = nullptr;
    IOWorkLoop *fWl = nullptr;
    IOFilterInterruptEventSource *fIrq = nullptr;
    bool fIrqOn = false;
    void (*volatile fEventCb)(void *) = nullptr;
    void *fEventCtx = nullptr;
    volatile uint64_t fIrqAt = 0, fIrqs = 0;

    volatile uint8_t *mapCap(uint8_t bar, uint32_t offset, uint32_t length);
    bool setupIrq();
    void teardownIrq();
    bool setupQueue();
    bool setupCursorQueue();
    bool cursorSubmit(uint32_t type, uint32_t resourceId, uint32_t hotX, uint32_t hotY, uint32_t x, uint32_t y, bool mustSend);
    // sends cmd and waits for the response; returns the response type (0 on timeout); *doneAt: when it came
    uint32_t submit(const void *cmd, uint32_t cmdLen, void *resp, uint32_t respLen, uint64_t *doneAt = nullptr);
    // the same for up to two commands at once (the second small); the answers' types go to type[], the result is the
    // last one's (0 on timeout); *doneAt: when the last answer came
    struct Cmd { const void *cmd; uint32_t len; void *resp; uint32_t respLen; uint32_t type; };
    uint32_t submitBatch(Cmd *c, unsigned n, uint64_t *doneAt);
    bool submitNoData(const void *cmd, uint32_t cmdLen, const char *what);
};
