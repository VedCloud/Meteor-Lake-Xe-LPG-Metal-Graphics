// NMXVGAFB: the display of a VM whose GPU is a headless SR-IOV virtual function (an Intel Panther Lake VF driven by
// NMXGPU). The VM's own display device (QEMU virtio-vga / std VGA) keeps the linear framebuffer the firmware lit; this
// framebuffer takes that mode over, unchanged, and offers it to NMXAccel as head 0. WindowServer then composites on
// the VF and NMXAccel's display pipe copies each finished frame into this framebuffer (the CPU-only scan-out path
// NMIntelFB uses for an Intel panel under the NVIDIA accelerator). Without NMXAccel it is a plain framebuffer.
//
// The IOFramebuffer plumbing (VBL timer, power and connection answers) follows NMIntelFB, which follows NVRMFB.

#include <IOKit/IOLib.h>
#include <IOKit/IOPlatformExpert.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/IODeviceMemory.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/IONVRAM.h>
#include <kern/thread_call.h>
#include <kern/sched_prim.h>
#include <mach/thread_policy.h>
#include <pexpert/pexpert.h>
#include "../NMIntelFB/nvrm_vram_abi.h"
#include "nmx_virtio_gpu.h"
#include "nmx_scanout_abi.h"
#include <mach/mach_time.h>
#ifdef LATITUDE_MTL_PANEL
#include "latitude_scanout.h"
#include "latitude_gpu_identity.h"
#include "latitude_backlight.inc"
#include "latitude_panel_edid.h"
#include "../NVRM/latitude_gamma.h"
#include <IOKit/ndrvsupport/IOMacOSVideo.h>
#include <IOKit/IOUserClient.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#endif
#include <sys/sysctl.h>
#include <sys/proc.h>

// IOLog and kprintf both: a hang during start leaves only the serial console
#define VFBLOG(fmt, ...) do { IOLog("NMXVGAFB: " fmt "\n", ##__VA_ARGS__); kprintf("NMXVGAFB: " fmt "\n", ##__VA_ARGS__); } while (0)

#include <mach/kmod.h>
extern "C" kern_return_t _start(kmod_info_t *ki, void *d); extern "C" kern_return_t _stop(kmod_info_t *ki, void *d);
#ifdef LATITUDE_MTL_PANEL
extern "C" { KMOD_EXPLICIT_DECL(com.nullmoth.LatitudeMTLFB, "0.2.8", _start, _stop) }
#else
extern "C" { KMOD_EXPLICIT_DECL(com.nullmoth.NMXVGAFB, "0.1", _start, _stop) }
#endif
extern "C" kern_return_t _start(kmod_info_t *ki, void *d) { return KERN_SUCCESS; }
extern "C" kern_return_t _stop(kmod_info_t *ki, void *d)  { return KERN_SUCCESS; }


class NMXVGAFB : public IOFramebuffer {
    OSDeclareDefaultStructors(NMXVGAFB)
#ifdef LATITUDE_MTL_PANEL
    LatitudeBacklightHandler *fBacklight=nullptr;
    IOLock *fGammaLock=nullptr;LatitudeGamma fGamma={};
    IOReturn latitudeSetGamma(UInt32 channels,UInt32 count,UInt32 width,void *data){
        LatitudeGamma next={};if(!latitudeGammaBuild(channels,count,width,data,&next))return kIOReturnBadArgument;
        if(!fGammaLock)return kIOReturnNotReady;
        IOLockLock(fGammaLock);bool changed=memcmp(next.rgb,fGamma.rgb,sizeof next.rgb)!=0;
        next.generation=fGamma.generation+(changed?1:0);fGamma=next;IOLockUnlock(fGammaLock);
        setProperty("LatitudeGammaActive",next.active?kOSBooleanTrue:kOSBooleanFalse);setProperty("LatitudeGammaGeneration",next.generation,64);
        setProperty("LatitudeGammaChannels",channels,32);setProperty("LatitudeGammaCount",count,32);setProperty("LatitudeGammaWidth",width,32);
        return kIOReturnSuccess;
    }
    bool fPanelTiming = false;
    bool fPanelMemoryValidated = false;
    bool adoptLatitudePanel(IOPCIDevice *pci);
#endif
    IOMemoryMap *fSurfMap = nullptr;   // write-combined CPU view of the scan-out (NMXAccel's frames land here)
    struct Irq { IOFBInterruptProc proc = nullptr; OSObject *target = nullptr; void *ref = nullptr; };
    Irq fVbl, fConnect;
    // The VBL timer: a kernel thread of its own (time-constraint policy) that sleeps until fVblNext. A thread call, or an IOTimerEventSource even with zero leeway, fired 0-3 ms late: the
    // kernel coalesces non-critical timers. Re-arming (callers hold fVblLock and have set fVblNext) wakes the thread,
    // which then waits for the new deadline.
    thread_t fVblThread = nullptr;
    bool fVblTimer = false;               // the thread runs
    volatile bool fVblThreadDone = false;
    void vblArm(uint64_t) { thread_wakeup((event_t)&fVblNext); }
    static void vblThreadMain(void *p0, wait_result_t);
    static constexpr uint64_t kVblNearNs = 20000;   // the VBL wait's last step: at most this far out (see vblThreadMain)
    uint64_t fVblNext = 0;
    // VBL phase: while frames flow, the host's answer to a flip's fenced flush is the vblank (QEMU releases the guest
    // at a fixed phase of its panel's refresh); the timer runs on from the last one. fVblLock orders the two callers.
    IOLock *fVblLock = nullptr;
    uint64_t fLastVbl = 0, fPeriodAbs = 0;
    uint64_t fLatch = 0;                  // a host latch point (mach time), 0 until the first answer
    void vblTick(uint64_t at);
    void hostLatch(uint64_t at);
    // a host that answers fenced flushes at once (no QEMU 0007) gives no vblank: its answers count only once
    // kPacedSeen of them took more than kPacedNs (with 0007 the answer for a frame comes about a refresh later)
    static constexpr unsigned kPacedSeen = 8;
    static constexpr uint64_t kPacedNs = 1000000;
    unsigned fDeferredSeen = 0;
    volatile bool fStopping = false;
    IOPhysicalAddress fBase = 0;
    uint32_t fWidth = 0, fHeight = 0, fRowBytes = 0;
    // virtio-gpu (virtio-vga with blob=true): the scan-out is a guest-RAM blob the host shows without a copy, flushed
    // every VBL; without it (or when it fails) the scan-out stays the VGA BAR the firmware lit
    NMXVirtioGPU fVirtio;
    IOBufferMemoryDescriptor *fScanBuf = nullptr;
    // The own buffer's blob (surfaces' blobs start at 16). It is allocated once, at start while physical memory is
    // still unfragmented, large enough for any mode taken later (kScanCapPx or the start mode): a 20 MB contiguous
    // allocation after login held the free-page lock until every CPU spun on it (the VM hung on a mode switch).
    static constexpr uint32_t fScanRes = 1;
    static constexpr uint64_t kScanCapPx = 3840ull * 2400;
    uint64_t fScanCap = 0;                // bytes
    bool fVirtioOn = false;
    bool startVirtio(IOPCIDevice *pci, IOPhysicalAddress vgaBase);
    // Hardware cursor over virtio-gpu's cursor queue: QEMU's GTK display makes the image the host pointer's shape, so
    // pointer motion needs no composite and moves at the host's input rate. Two 64x64 ARGB32 blobs: the image, and a
    // transparent one that stands for "hidden" (GTK ignores a hide with an absolute pointer).
    static constexpr uint32_t kCursorRes = 2, kCursorHiddenRes = 3, kCursorDim = 64;
    IOBufferMemoryDescriptor *fCursorBuf = nullptr;   // the image, then the transparent one
    bool fCursorOn = false, fCursorShown = false, fCursorHaveImage = false;
    uint32_t fCursorHotX = 0, fCursorHotY = 0;
    SInt32 fCursorX = 0, fCursorY = 0;
    uint64_t fCursorImages = 0, fCursorMoves = 0;
    void startCursor();
    void cursorShow(bool visible, SInt32 x, SInt32 y);
    // Zero-copy: NMXAccel hands over each presented surface and the scan-out is switched to a blob over the surface's
    // own pages. WindowServer cycles through a few surfaces, so each keeps its blob (its memory prepared) while cached.
    struct SurfBlob { IOMemoryDescriptor *md; uint32_t surfaceId, res, w, h, pitch; uint64_t lastUse; };
    static constexpr unsigned kSurfBlobs = 8;
    SurfBlob fSurf[kSurfBlobs] = {};
    uint32_t fNextRes = 16;
    volatile uint32_t fCurRes = 0;        // the resource on scanout 0 (fScanRes or a surface's)
    uint32_t fCurW = 0, fCurH = 0;
    bool fCurFinal = false;               // the surface on scanout 0 came with NMX_SCANOUT_FINAL: no VBL re-flush
    uint64_t fLastPresent = 0;            // mach time of the last scanoutSurface (a present)
public:
    // present pacing, read through debug.nmxfb_vtiming: intervals between presents (ms buckets), time in scanoutSurface
    static constexpr unsigned kHist = 8;
    static constexpr uint32_t kHistMs[kHist - 1] = { 12, 20, 28, 37, 45, 60, 100 };
    uint64_t fPresentHist[kHist] = {};
    uint64_t fSwitchN = 0, fSwitchNs = 0, fSwitchMaxNs = 0;
    // VBLs, host answers taken as latch points, answers that moved the latch grid earlier
    uint64_t fVblTimerN = 0, fLatchAnswers = 0, fLatchEarlier = 0;
    // debug.nmxfb_vtrace: the last kTrace display events, times in us of the mach clock (ns on x86)
    // V vbl (arg: us after its deadline), L latch grid after a host answer (arg its phase in the period, us), S flip of a surface (arg id),
    // A host answer to its flush (arg round trip, us)
    static constexpr unsigned kTrace = 512;
    struct TraceEv { uint64_t us; uint32_t arg; char kind; };
    TraceEv fTrace[kTrace] = {};
    volatile uint32_t fTraceN = 0;
    void trace(char kind, uint64_t at, uint32_t arg) {
        const uint32_t i = (uint32_t)OSIncrementAtomic((volatile SInt32 *)&fTraceN) % kTrace;
        fTrace[i] = { at / 1000, arg, kind };
    }
    NMXVirtioGPU &virtio() { return fVirtio; }
    // a copy of what scanout 0 shows now (the host reads the same pages): header + rows of B G R X, for debug.nmxfb_shot
    IOReturn snapshot(void *dst, size_t cap, size_t *len);
private:
    bool fFlushing = true;
    unsigned fIdleLogs = 0;
    static constexpr uint64_t kSurfFlushNs = 1000000000ull;
    IOLock *fScanLock = nullptr;
    unsigned fSwitchLogs = 0;
    IOReturn scanoutSurface(NMXScanoutSurface *r);
    IOReturn scanoutHome();
    void dropSurfBlob(SurfBlob &b);
    // The one mode: with virtio-gpu the host's scanout 0 size (the QEMU window), else the firmware's. Its timing is the
    // EDID's first detailed timing when that is the mode's (QEMU 0002: reduced blanking at the host's 120 Hz), else
    // a reduced-blanking one at fHz (120, the host panel's), and boot-arg nmxvgafb_hz=<30..240> forces the latter.
    // WindowServer needs a timing to see a rate (0.00 Hz without); the VBL timer runs at it.
    uint32_t fHz = 120;
    bool fHzForced = false;
    uint64_t fFrameNs = 8333333ull;
    uint32_t fRefreshMHz = 120000;
    uint32_t fHT = 0, fVT = 0, fHSO = 48, fHSW = 32, fVSO = 3, fVSW = 5;
    uint64_t fPclk = 0;
    struct Timing { uint32_t ht, vt, hso, hsw, vso, vsw, mhz; uint64_t pclk, frameNs; };
    // a mode's timing: the EDID's first detailed timing when it describes w x h, else reduced blanking at fHz
    Timing timingFor(uint32_t w, uint32_t h) const;
    static uint32_t pitchFor(uint32_t w) { return (w * 4 + 63) & ~63u; }
    void setTiming();                     // the active mode's (fWidth x fHeight) into the fields above
    void publishTiming();                 // NVRMTiming, the record NMXAGDC's link-config answers read
    IODisplayModeID fActID = 1, fAdvID = 1, fNextID = 2;
    uint32_t fAdvW = 0, fAdvH = 0;
    thread_call_t fHotplug = nullptr;
    static void displayEvent(void *p0);   // from the virtio interrupt: the host resized its window
    static void hotplugFire(thread_call_param_t p0, thread_call_param_t);
    bool switchMode();
    // WindowServer's active state for this head (the 'wsrv' attribute, kIOWSAA_*). CoreDisplay starts a display with
    // To_Accelerated and completes it (Accelerated) on a present only once setFramebufferActiveOnUpdate is set, which
    // Framebuffer::InitializeMPPriv leaves false here (measured: the IODisplay is attached by then) and a reconfiguring
    // connect change sets. Left in To_Accelerated, it rebuilt its colour pipe on every present (BuildTransactions ->
    // AddEOTF -> 256 CombineLUTs on 8 threads, contending on CurrentHeadroom): ~30% of WindowServer during a drag.
    static constexpr uint32_t kWsaaStateMask = 0xF, kWsaaToAccelerated = 3;
    volatile UInt32 fWsaa = 0, fWsaaKicked = 0, fWsaaKicks = 0;
    static constexpr uint32_t kWsaaKickMax = 5;
    thread_call_t fWsaaKick = nullptr;
    static void wsaaKickFire(thread_call_param_t p0, thread_call_param_t);
    bool hasIODisplay();
    // the host's EDID for scanout 0, handed to IOGraphics (the display shows up as what QEMU describes)
    uint8_t fEdid[256] = {};
    uint32_t fEdidLen = 0;
    // the display hold: the registry stays busy from start() until the accelerator has armed this head (phase 1), so
    // WindowServer, which waits for a quiet registry before it looks for displays, finds it ready to composite on
    volatile UInt32 fHolding = 0;
    thread_call_t fHoldCap = nullptr;
    IOService *fAccel = nullptr;          // the accelerator that armed this head (retained)
    volatile UInt32 fOpenedOnce = 0;
    static constexpr uint32_t kHoldCapSec = 45;
    void releaseHold(const char *why) {
        if (!OSCompareAndSwap(1, 0, &fHolding)) return;
        adjustBusy(-1);
        VFBLOG("display hold released: %s", why);
    }
    static void holdCapFire(thread_call_param_t p0, thread_call_param_t) {
        NMXVGAFB *me = (NMXVGAFB *)p0;
        if (me->fHolding)
            VFBLOG("no accelerator armed this display within %u s - WindowServer goes on and composites on the CPU",
                   kHoldCapSec);
        me->releaseHold("cap");
    }

    static void vblFire(void *p0) {
        NMXVGAFB *me = (NMXVGAFB *)p0;
        if (me->fStopping) return;
        // under the scan-out lock: QEMU refuses to flush a blob that is not on a scanout, which the one just
        // replaced is; only a lost device (a command timed out) ends the virtio scan-out. The own buffer is flushed
        // every VBL (its CPU writers give no notice); a presented surface only for kSurfFlushNs after its present,
        // which covers the plugin's copy-out into it after the GPU signals - an idle desktop costs no flushes
        // A flip holds the scan-out lock while the host shows its frame (up to a refresh) and needs no VBL flush:
        // the timer doesn't wait for it.
        uint64_t now = mach_absolute_time();
        if (me->fVirtioOn && me->fScanLock && IOLockTryLock(me->fScanLock)) {
            uint64_t sinceNs = 0;
            if (me->fCurRes != fScanRes) absolutetime_to_nanoseconds(now - me->fLastPresent, &sinceNs);
            const bool flush = sinceNs < kSurfFlushNs && !(me->fCurRes != fScanRes && me->fCurFinal);
            if (flush) me->fVirtio.flush(me->fCurRes, 0, 0, me->fCurW, me->fCurH);
            if (!flush && me->fFlushing && me->fIdleLogs < 3) {
                me->fIdleLogs++;
                VFBLOG("virtio-gpu: no present for 1 s - VBL flushes pause until the next one");
            }
            me->fFlushing = flush;
            IOLockUnlock(me->fScanLock);
            if (!me->fVirtio.ready()) {
                me->fVirtioOn = false;
                VFBLOG("virtio-gpu stopped answering: the host keeps the last frame (%llu commands, %llu failed)",
                       me->fVirtio.commands(), me->fVirtio.failures());
            }
        }
        me->vblTick(mach_absolute_time());
    }

public:
    IOService *probe(IOService *provider, SInt32 *score) override;
    bool start(IOService *provider) override;
#ifdef LATITUDE_MTL_PANEL
    IOReturn setProperties(OSObject *properties) override;
#endif
#ifdef LATITUDE_MTL_PANEL
    void free() override {
        if(fBacklight){fBacklight->disable();if(fBacklight->getProvider()==this)fBacklight->detach(this);OSSafeReleaseNULL(fBacklight);}
        if(fGammaLock){IOLockFree(fGammaLock);fGammaLock=nullptr;}
        IOFramebuffer::free();
    }
    IOReturn getAppleSense(IOIndex idx,UInt32 *sense,UInt32 *primary,UInt32 *extended,UInt32 *type) override {
        if(idx || !fBacklight)return kIOReturnUnsupported;
        if(sense)*sense=0;if(primary)*primary=0;if(extended)*extended=0;if(type)*type=kGenericLCD;return kIOReturnSuccess;
    }
#endif
    void stop(IOService *provider) override;

    IOReturn enableController() override { return kIOReturnSuccess; }
    // WindowServer opens the framebuffer (its own thread, once the registry is quiet). Only an opened framebuffer can be
    // registered with the accelerator's display machine (the mode change reads state open() sets up), so the
    // accelerator registers it here, before open returns: WindowServer then finds this head's display pipe.
    IOReturn open() override {
        IOReturn r = IOFramebuffer::open();
        if (r != kIOReturnSuccess || !opened || !OSCompareAndSwap(0, 1, &fOpenedOnce)) return r;
        setProperty("NMXVGAFBOpen", kOSBooleanTrue);
        if (IOService *a = fAccel) {
            const OSSymbol *fn = OSSymbol::withCStringNoCopy("nmxHeadOpened");
            IOReturn ar = fn ? a->callPlatformFunction(fn, true, this, nullptr, nullptr, nullptr) : kIOReturnNoMemory;
            OSSafeReleaseNULL(fn);
            VFBLOG("opened; the accelerator registered this head (0x%x)", ar);
        } else {
            VFBLOG("opened before an accelerator armed this head: it registers the head when it does");
        }
        return r;
    }
    // Not the kernel console: with true, IOGraphics moved the text console onto this framebuffer during boot and the
    // machine stopped (console and serial output both, right after "IOG flags"). WindowServer does not need it.
    bool isConsoleDevice() override { return false; }
    IODeviceMemory *getApertureRange(IOPixelAperture aperture) override {
        if (aperture != kIOFBSystemAperture) return nullptr;
        return IODeviceMemory::withRange(fBase, (IOPhysicalLength)fRowBytes * fHeight);
    }
    IODeviceMemory *getVRAMRange() override { return getApertureRange(kIOFBSystemAperture); }
    const char *getPixelFormats() override { return IO32BitDirectPixels "\0"; }
    // Modes: the active one (fActID, fWidth x fHeight) and the advertised one (fAdvID, the host's latest size). They
    // differ between a host resize (displayEvent) and WindowServer's switch to the new mode (setDisplayMode).
    IOItemCount getDisplayModeCount() override { return 1; }
    IOReturn getDisplayModes(IODisplayModeID *modes) override { modes[0] = fAdvID; return kIOReturnSuccess; }
    bool modeSize(IODisplayModeID mode, uint32_t *w, uint32_t *h) const {
        if (mode == fAdvID) { *w = fAdvW; *h = fAdvH; return true; }
        if (mode == fActID) { *w = fWidth; *h = fHeight; return true; }
        return false;
    }
    IOReturn getInformationForDisplayMode(IODisplayModeID mode, IODisplayModeInformation *info) override {
        uint32_t w, h;
        if (!info || !modeSize(mode, &w, &h)) return kIOReturnBadArgument;
        const Timing t = timingFor(w, h);
        bzero(info, sizeof(*info));
        info->maxDepthIndex = 0; info->nominalWidth = w; info->nominalHeight = h;
#ifdef LATITUDE_MTL_PANEL
        uint32_t mmW=0,mmH=0;
        if(latitudeEDIDGeometry(fEdid,fEdidLen,w,h,&mmW,&mmH)){info->imageWidth=mmW;info->imageHeight=mmH;}
#endif
        info->refreshRate = (IOFixed1616)(((uint64_t)t.mhz << 16) / 1000);
        info->flags = kDisplayModeValidFlag | kDisplayModeSafeFlag | (mode == fAdvID ? kDisplayModeDefaultFlag : 0);
        return kIOReturnSuccess;
    }
    UInt64 getPixelFormatsForDisplayMode(IODisplayModeID, IOIndex) override { return 0; }
    IOReturn getPixelInformation(IODisplayModeID mode, IOIndex depth, IOPixelAperture aperture, IOPixelInformation *pi) override {
        uint32_t w, h;
        if (!modeSize(mode, &w, &h) || depth != 0 || aperture != kIOFBSystemAperture || !pi) return kIOReturnUnsupportedMode;
        bzero(pi, sizeof(*pi));
        pi->bytesPerRow = mode == fActID ? fRowBytes : pitchFor(w); pi->bitsPerPixel = 32; pi->pixelType = kIORGBDirectPixels;
        pi->componentCount = 3; pi->bitsPerComponent = 8;
        pi->componentMasks[0] = 0x00FF0000; pi->componentMasks[1] = 0x0000FF00; pi->componentMasks[2] = 0x000000FF;
        strlcpy(pi->pixelFormat, IO32BitDirectPixels, sizeof(pi->pixelFormat));
        pi->activeWidth = w; pi->activeHeight = h;
        return kIOReturnSuccess;
    }
    IOReturn getCurrentDisplayMode(IODisplayModeID *mode, IOIndex *depth) override { *mode = fActID; *depth = 0; return kIOReturnSuccess; }
    IOReturn getStartupDisplayMode(IODisplayModeID *mode, IOIndex *depth) override { return getCurrentDisplayMode(mode, depth); }
    IOReturn getTimingInfoForDisplayMode(IODisplayModeID mode, IOTimingInformation *info) override {
        uint32_t w, h;
        if (!info || !modeSize(mode, &w, &h)) return kIOReturnUnsupportedMode;
        const Timing t = timingFor(w, h);
        bzero(info, sizeof(*info));
        info->appleTimingID = kIOTimingIDInvalid;
        IODetailedTimingInformationV2 &d = info->detailedInfo.v2;
        d.pixelClock = d.minPixelClock = d.maxPixelClock = t.pclk;
        d.horizontalActive = w; d.horizontalBlanking = t.ht - w;
        d.horizontalSyncOffset = t.hso; d.horizontalSyncPulseWidth = t.hsw;
        d.verticalActive = h; d.verticalBlanking = t.vt - h;
        d.verticalSyncOffset = t.vso; d.verticalSyncPulseWidth = t.vsw;
        info->flags = kIODetailedTimingValid;
        return kIOReturnSuccess;
    }
    IOReturn setDisplayMode(IODisplayModeID mode, IOIndex depth) override;
    IOItemCount getConnectionCount() override { return 1; }
    IOReturn callPlatformFunction(const OSSymbol *fn, bool wait, void *p1, void *p2, void *p3, void *p4) override;
    IOReturn registerForInterruptType(IOSelect type, IOFBInterruptProc proc, OSObject *target, void *ref, void **iref) override {
        if (type == kIOFBVBLInterruptType) { fVbl = { proc, target, ref }; if (iref) *iref = &fVbl; return kIOReturnSuccess; }
        if (type == kIOFBConnectInterruptType) { fConnect = { proc, target, ref }; if (iref) *iref = &fConnect; return kIOReturnSuccess; }
        return kIOReturnUnsupported;
    }
    IOReturn unregisterInterrupt(void *) override { return kIOReturnSuccess; }
    IOReturn setInterruptState(void *, UInt32) override { return kIOReturnSuccess; }
#ifdef LATITUDE_MTL_PANEL
    IOReturn setGammaTable(UInt32 c,UInt32 n,UInt32 b,void *d) override {return latitudeSetGamma(c,n,b,d);}
    IOReturn setGammaTable(UInt32 c,UInt32 n,UInt32 b,void *d,bool) override {return latitudeSetGamma(c,n,b,d);}
#else
    IOReturn setGammaTable(UInt32,UInt32,UInt32,void *) override {return kIOReturnSuccess;}
    IOReturn setGammaTable(UInt32,UInt32,UInt32,void *,bool) override {return kIOReturnSuccess;}
#endif
    IOReturn setCLUTWithEntries(IOColorEntry *, UInt32, UInt32, IOOptionBits) override { return kIOReturnSuccess; }
    IOReturn setApertureEnable(IOPixelAperture, IOOptionBits) override { return kIOReturnSuccess; }
    IOReturn setStartupDisplayMode(IODisplayModeID, IOIndex) override { return kIOReturnSuccess; }
    // acknowledged at once, as NMIntelFB/NVRMFB do: left to IOFramebuffer, a power change waits on the controller
    // thread and froze the displays on the first WindowServer restart
    IOReturn setPowerState(unsigned long ordinal, IOService *) override {
        handleEvent(ordinal ? kIOFBNotifyDidPowerOn : kIOFBNotifyWillPowerOff);
        return kIOPMAckImplied;
    }
    IOReturn setAttribute(IOSelect attribute, uintptr_t value) override {
        if (attribute == kIOWindowServerActiveAttribute) {
            const UInt32 was = fWsaa;
            fWsaa = (UInt32)value;
            if ((value & kWsaaStateMask) == kWsaaToAccelerated) { fWsaaKicked = 0; fWsaaKicks = 0; }
            if (was != (UInt32)value)
                VFBLOG("WindowServer active state 0x%x -> 0x%x (IODisplay %s)", was, (UInt32)value,
                       hasIODisplay() ? "attached" : "not attached");
            return kIOReturnSuccess;
        }
        if (attribute == kIOPowerAttribute) {
            if (value) { IOReturn r = IOFramebuffer::setAttribute(attribute, value); handleEvent(kIOFBNotifyDidPowerOn);
                         return r == kIOReturnUnsupported ? kIOReturnSuccess : r; }
            handleEvent(kIOFBNotifyWillPowerOff);
            IOReturn r = IOFramebuffer::setAttribute(attribute, value);
            return r == kIOReturnUnsupported ? kIOReturnSuccess : r;
        }
        return IOFramebuffer::setAttribute(attribute, value);
    }
    IOReturn setCursorImage(void *cursorImage) override;
    IOReturn setCursorState(SInt32 x, SInt32 y, bool visible) override;
    IOReturn getAttribute(IOSelect attribute, uintptr_t *value) override {
        if (attribute == kIOWindowServerActiveAttribute) { if (value) *value = 1; return kIOReturnSuccess; }
        if (attribute == kIOHardwareCursorAttribute) { if (value) *value = fCursorOn; return kIOReturnSuccess; }
        return IOFramebuffer::getAttribute(attribute, value);
    }
    IOReturn setAttributeForConnection(IOIndex idx, IOSelect attribute, uintptr_t value) override {
        switch (attribute) {
        case kConnectionPower: case kConnectionColorModesSupported: case kConnectionColorDepthsSupported:
        case kConnectionControllerColorDepth: case kConnectionControllerDepthsSupported: case kConnectionColorMode:
        case kConnectionDisplayFlags: case kConnectionFlags:
            return kIOReturnSuccess;
        default: return IOFramebuffer::setAttributeForConnection(idx, attribute, value);
        }
    }
    IOReturn getAttributeForConnection(IOIndex idx, IOSelect attr, uintptr_t *value) override {
        // a capability question, asked without a value pointer: the EDID is there to read
        if (idx == 0 && attr == kConnectionSupportsHLDDCSense) return fEdidLen >= 128 ? kIOReturnSuccess : kIOReturnUnsupported;
        if (idx != 0 || !value) return kIOReturnBadArgument;
        switch (attr) {
        case kConnectionEnable: case kConnectionCheckEnable: *value = 1; return kIOReturnSuccess;
        case kConnectionFlags: *value = kIOConnectionBuiltIn; return kIOReturnSuccess;
        case kConnectionColorModesSupported: case kConnectionColorMode: *value = 0x00000001; return kIOReturnSuccess;
        case kConnectionColorDepthsSupported: case kConnectionControllerColorDepth: case kConnectionControllerDepthsSupported:
            *value = 0x00000002; return kIOReturnSuccess;
        default: return IOFramebuffer::getAttributeForConnection(idx, attr, value);
        }
    }
    bool hasDDCConnect(IOIndex) override { return fEdidLen >= 128; }
    IOReturn getDDCBlock(IOIndex, UInt32 blockNumber, IOSelect blockType, IOOptionBits, UInt8 *data, IOByteCount *length) override {
        if (blockType != kIODDCBlockTypeEDID || blockNumber == 0 || blockNumber * 128 > fEdidLen) return kIOReturnUnsupported;
        if (!data || !length || *length < 128) return kIOReturnBadArgument;
        memcpy(data, fEdid + (blockNumber - 1) * 128, 128); *length = 128;
        return kIOReturnSuccess;
    }
    IOReturn connectFlags(IOIndex, IODisplayModeID mode, IOOptionBits *flags) override {
        if (flags) *flags = kDisplayModeValidFlag | kDisplayModeSafeFlag | (mode == fAdvID ? kDisplayModeDefaultFlag : 0);
        return kIOReturnSuccess;
    }
};

OSDefineMetaClassAndStructors(NMXVGAFB, IOFramebuffer)

/*
 * VBL phase. With QEMU 0007 the host answers a flip's fenced flush at the latch point of the refresh the frame made
 * (its vblank plus a fixed phase), or when the frame is drawn if that is later - so the latch grid is the lower
 * envelope of the answers. An answer before the predicted latch moves the grid to it at once; a later one moves it by
 * at most kCreepNs (the guest's and the host's clocks drift apart slowly). The VBL is the timer's alone, on the latch
 * points as on a real display: a flip after it goes to the next refresh (WindowServer composites to flip just before
 * its next VBL). debug.nmxfb_vbl_lead_us moves it earlier; with a lead, a flip submitted just after a VBL could still
 * make the latch that follows it, a refresh early (measured: 16% of 60 fps frames uneven with 1.5 ms).
 */
static uint32_t gVblLeadUs = 0;
SYSCTL_UINT(_debug, OID_AUTO, nmxfb_vbl_lead_us, CTLFLAG_RW | CTLFLAG_LOCKED, &gVblLeadUs, 0,
            "NMXVGAFB: VBL this long (us) before the host's latch point (QEMU 0007 fenced flush answers); 0: on it, as on a real display");

static uint32_t gVblStepped = 1;
SYSCTL_UINT(_debug, OID_AUTO, nmxfb_vbl_stepped, CTLFLAG_RW | CTLFLAG_LOCKED, &gVblStepped, 0,
            "NMXVGAFB: wait for the VBL in steps that timer coalescing cannot carry past it; 0: one wait (up to 1 ms late)");

void NMXVGAFB::hostLatch(uint64_t at)
{
    if (!fVblLock || !fVblTimer) return;
    static constexpr uint64_t kCreepNs = 2000;
    IOLockLock(fVblLock);
    const uint64_t p = fPeriodAbs;
    if (!fLatch) fLatch = at;
    else {
        // the latch nearest to the answer, and how far the answer is from it
        const int64_t d = (int64_t)(at - fLatch);
        const int64_t k = (d >= 0 ? d + (int64_t)p / 2 : d - (int64_t)p / 2) / (int64_t)p;
        const uint64_t pred = fLatch + (uint64_t)(k * (int64_t)p);
        const int64_t e = (int64_t)(at - pred);
        uint64_t creep; nanoseconds_to_absolutetime(kCreepNs, &creep);
        fLatch = e < 0 ? at : pred + ((uint64_t)e < creep ? (uint64_t)e : creep);
        if (e < 0) fLatchEarlier++;
    }
    fLatchAnswers++;
    // the first VBL on the grid that no VBL served yet (one less than half a period after the last did): it can be
    // due already when the answer beat the timer to this latch, and then the timer fires at once
    uint64_t lead; nanoseconds_to_absolutetime((uint64_t)gVblLeadUs * 1000, &lead);
    if (lead >= p) lead = p / 2;
    const uint64_t served = fLastVbl ? fLastVbl + p / 2 : mach_absolute_time() - p / 2;
    uint64_t next = fLatch - lead;
    if (next <= served) next += ((served - next) / p + 1) * p;
    else next -= ((next - served - 1) / p) * p;
    if (next != fVblNext) { fVblNext = next; if (!fStopping) vblArm(next); }
    trace('L', fLatch, (uint32_t)((fLatch % p) / 1000));
    IOLockUnlock(fVblLock);
}

// the VBL timer: one VBL per refresh on the phase hostLatch set (or the one it had), skipping ticks it missed
void NMXVGAFB::vblThreadMain(void *p0, wait_result_t)
{
    NMXVGAFB *me = (NMXVGAFB *)p0;
    IOLockLock(me->fVblLock);
    while (!me->fStopping) {
        /*
         * A kernel thread's timer is coalesced like any kernel_task timer, time-constraint policy or not: up to 1/8 of
         * the distance to it, at most 1 ms (kern.timer_coalesce_kt_scale / _kt_ns_max). Armed a refresh ahead, the VBL
         * fired 0.95 ms late at p50 (bimodal: on time or ~1 ms late), so WindowServer's frame clock jumped by a ms and
         * it composited late and dropped refreshes. (assert_wait_deadline_with_leeway with critical urgency isn't
         * exported to kexts.) So the wait goes in steps of 8/9 of what is left, which even the full leeway cannot
         * carry past the VBL; the last steps are microseconds away and get next to none (p50 ~40 us late).
         */
        const uint64_t due = me->fVblNext, now = mach_absolute_time();
        uint64_t near; nanoseconds_to_absolutetime(kVblNearNs, &near);
        const uint64_t wake = gVblStepped && due > now + near ? now + (due - now) / 9 * 8 : due;
        assert_wait_deadline((event_t)&me->fVblNext, THREAD_UNINT, wake);
        IOLockUnlock(me->fVblLock);
        const wait_result_t w = thread_block(THREAD_CONTINUE_NULL);
        if (w == THREAD_TIMED_OUT && !me->fStopping && wake == due) vblFire(me);
        IOLockLock(me->fVblLock);
    }
    me->fVblThreadDone = true;
    IOLockWakeup(me->fVblLock, (void *)&me->fVblThreadDone, false);
    IOLockUnlock(me->fVblLock);
    thread_terminate(current_thread());
}

void NMXVGAFB::vblTick(uint64_t at)
{
    if (!fVblLock || !fVblTimer) return;
    IOLockLock(fVblLock);
    const uint64_t p = fPeriodAbs;
    if (fVblNext && at + p / 4 < fVblNext) {
        // a host answer moved the VBL since this tick was armed
        vblArm(fVblNext);
        IOLockUnlock(fVblLock);
        return;
    }
    fVblTimerN++;
    const uint64_t due = fVblNext;
    uint64_t next = fVblNext ? fVblNext + p : at + p;
    if (next <= at) next += ((at - next) / p + 1) * p;
    fLastVbl = at;
    if (fVbl.proc) fVbl.proc(fVbl.target, fVbl.ref);
    uint64_t lateNs = 0; if (due && at > due) absolutetime_to_nanoseconds(at - due, &lateNs);
    trace('V', at, (uint32_t)(lateNs / 1000));       // arg: how late the timer fired, us
    fVblNext = next;
    if (!fStopping) vblArm(next);
    IOLockUnlock(fVblLock);
}

// debug.nmxfb_vtiming: present pacing and virtio-gpu round trips (see fPresentHist and NMXVirtioGPU::timing)
static NMXVGAFB *volatile gVtInst;
static int nmxVtimingSysctl(struct sysctl_oid *, void *, int, struct sysctl_req *req)
{
    char buf[896];
    NMXVGAFB *f = gVtInst;
    if (!f) { snprintf(buf, sizeof buf, "no virtio display"); return SYSCTL_OUT(req, buf, strlen(buf) + 1); }
    const uint64_t *h = f->fPresentHist;
    int n = snprintf(buf, sizeof buf, "present gaps ms <12:%llu <20:%llu <28:%llu <37:%llu <45:%llu <60:%llu <100:%llu >=100:%llu"
                     " | switch n %llu avg %llu us max %llu us",
                     h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7], f->fSwitchN,
                     f->fSwitchN ? f->fSwitchNs / f->fSwitchN / 1000 : 0, f->fSwitchMaxNs / 1000);
    static const char *const kName[NMXVirtioGPU::kTimings] = { "flush", "set_scanout", "other", "shown" };
    for (unsigned k = 0; k < NMXVirtioGPU::kTimings && n > 0 && n < (int)sizeof buf; k++) {
        NMXVirtioGPU::Timing t = f->virtio().timing(k);
        n += snprintf(buf + n, sizeof buf - (size_t)n, " | %s n %llu avg %llu us max %llu us", kName[k], t.n,
                      t.n ? t.totalNs / t.n / 1000 : 0, t.maxNs / 1000);
    }
    if (n > 0 && n < (int)sizeof buf)
        snprintf(buf + n, sizeof buf - (size_t)n, " | vbl %llu, latch answers %llu (moved earlier %llu) | irqs %llu (%s)",
                 f->fVblTimerN, f->fLatchAnswers, f->fLatchEarlier, f->virtio().irqs(),
                 f->virtio().interrupts() ? "MSI-X" : "polled");
    return SYSCTL_OUT(req, buf, strlen(buf) + 1);
}
static int nmxVtraceSysctl(struct sysctl_oid *, void *, int, struct sysctl_req *req)
{
    NMXVGAFB *f = gVtInst;
    if (!f) return ENODEV;
    const size_t cap = NMXVGAFB::kTrace * 32 + 1;
    char *buf = (char *)IOMalloc(cap);
    if (!buf) return ENOMEM;
    const uint32_t end = f->fTraceN, n = end < NMXVGAFB::kTrace ? end : NMXVGAFB::kTrace;
    size_t len = 0;
    for (uint32_t k = end - n; k != end && len < cap - 32; k++) {
        const NMXVGAFB::TraceEv &e = f->fTrace[k % NMXVGAFB::kTrace];
        len += (size_t)snprintf(buf + len, cap - len, "%llu %c %u\n", (unsigned long long)e.us, e.kind ? e.kind : '?', e.arg);
    }
    buf[len] = 0;
    int r = SYSCTL_OUT(req, buf, len + 1);
    IOFree(buf, cap);
    return r;
}
SYSCTL_PROC(_debug, OID_AUTO, nmxfb_vtrace, CTLTYPE_STRING | CTLFLAG_RD | CTLFLAG_LOCKED, nullptr, 0, nmxVtraceSysctl, "A",
            "NMXVGAFB: the last display events (us, V/L/S/A, argument)");

SYSCTL_PROC(_debug, OID_AUTO, nmxfb_vtiming, CTLTYPE_STRING | CTLFLAG_RD | CTLFLAG_LOCKED, nullptr, 0, nmxVtimingSysctl, "A",
            "NMXVGAFB: present gaps, scan-out switch and virtio-gpu command times");

// debug.nmxfb_shot (root): the scan-out's pixels as the host sees them - a 16-byte header ('NMXS', width, height, pitch)
// and the rows. A screenshot that needs neither a GUI session nor the host window (both fail on a blob scan-out).
static int nmxShotSysctl(struct sysctl_oid *, void *, int, struct sysctl_req *req)
{
    NMXVGAFB *f = gVtInst;
    if (!f) return ENODEV;
    if (proc_suser(req->p)) return EPERM;
    const size_t cap = 16 + (size_t)4096 * 4 * 2304;
    if (!req->oldptr) return SYSCTL_OUT(req, nullptr, 16 + (size_t)1920 * 4 * 1080 * 2);   // a size hint
    void *buf = IOMallocData(cap);
    if (!buf) return ENOMEM;
    size_t len = 0;
    IOReturn r = f->snapshot(buf, cap, &len);
    int err = r == kIOReturnSuccess ? SYSCTL_OUT(req, buf, len) : EIO;
    IOFreeData(buf, cap);
    return err;
}
SYSCTL_PROC(_debug, OID_AUTO, nmxfb_shot, CTLTYPE_OPAQUE | CTLFLAG_RD | CTLFLAG_LOCKED, nullptr, 0, nmxShotSysctl, "S",
            "NMXVGAFB: the scan-out's current pixels (header NMXS w h pitch, then rows)");

// Opt-in, as NMXGPU is: boot-arg nmxvgafb=1, or the one-shot NVRAM variable nullmoth-vgafb-once (consumed here, so a
// start that hangs is followed by one with the firmware framebuffer). Declining leaves the display to IONDRVFramebuffer.
static int g_opted = -1;      /* the decision, made once: the NVRAM variable is consumed by the first ask */
static bool opted_in_once();
static bool opted_in()
{
    if (g_opted < 0)
        g_opted = opted_in_once() ? 1 : 0;
    return g_opted == 1;
}
static bool opted_in_once()
{
    int v = 0;
    if (PE_parse_boot_argn("-nmxvgafboff", &v, sizeof(v))) return false;
#ifndef LATITUDE_MTL_PANEL
    if (PE_parse_boot_argn("nmxvgafb", &v, sizeof(v)) && v != 0) return true;
#endif
    bool once = false;
    if (IORegistryEntry *e = IORegistryEntry::fromPath("/options", gIODTPlane)) {
        if (IODTNVRAM *n = OSDynamicCast(IODTNVRAM, e)) {
            if (OSObject *o = n->copyProperty("nullmoth-panel-once")) {
                o->release();
                once = true;
                if (const OSSymbol *k = OSSymbol::withCString("nullmoth-panel-once")) { n->removeProperty(k); k->release(); }
                n->sync();
            }
        }
        e->release();
    }
    return once;
}

IOService *NMXVGAFB::probe(IOService *provider, SInt32 *score)
{
#ifdef LATITUDE_MTL_PANEL
    if (provider->getProperty("LatitudePanelOptedIn") != kOSBooleanTrue) return nullptr;
    IOPCIDevice *matchedPCI = OSDynamicCast(IOPCIDevice, provider->getProvider());
    g_opted = 1;
#else
    IOPCIDevice *matchedPCI = OSDynamicCast(IOPCIDevice, provider);
#endif
    if (!matchedPCI) return nullptr;
#ifdef LATITUDE_MTL_PANEL
    if (matchedPCI->configRead16(kIOPCIConfigVendorID) != 0x8086 ||
        matchedPCI->configRead16(kIOPCIConfigDeviceID) != 0x7d45 ||
        matchedPCI->configRead16(kIOPCIConfigSubSystemVendorID) != 0x1028 ||
        matchedPCI->configRead16(kIOPCIConfigSubSystemID) != 0x0cb6) return nullptr;
#endif
    if (!opted_in()) { VFBLOG("not opted in (nmxvgafb=1 or NVRAM nullmoth-vgafb-once) - the firmware framebuffer keeps the display"); return nullptr; }
    VFBLOG("probe: opted in");
    return IOFramebuffer::probe(provider, score);
}

static void nmxVramReserve(unsigned long long slotBytes);

#ifdef LATITUDE_MTL_PANEL
bool NMXVGAFB::adoptLatitudePanel(IOPCIDevice *pci)
{
    IOMemoryMap *mmio = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
    IODeviceMemory *ap = pci->getDeviceMemoryWithRegister(kIOPCIConfigBaseAddress2);
    if (!mmio || !ap || mmio->getLength() < 0x138918) {
        OSSafeReleaseNULL(mmio); VFBLOG("Latitude: BAR0/BAR2 unavailable; declining"); return false;
    }
    const volatile uint8_t *regs = (const volatile uint8_t *)mmio->getVirtualAddress();
    auto rd = [regs](uint32_t off) { return *(const volatile uint32_t *)(regs + off); };
    auto rd64 = [&rd](uint32_t off) { return (uint64_t)rd(off) | ((uint64_t)rd(off+4)<<32); };
    uint32_t permission=rd(0x138914),ggc=rd(0x108040);
    uint64_t gsm=rd64(0x108100)&~((1ull<<20)-1),dsm=rd64(0x1080c0)&~((1ull<<20)-1);
    unsigned ggms=(pci->configRead16(0x50)>>6)&3,gms=(ggc>>8)&255;
    uint64_t tableBytes=ggms?((1ull<<ggms)<<20):0;
    uint64_t dsmBytes=gms<=4?(uint64_t)gms*32*1024*1024:(gms>=0xf0&&gms<=0xfe?(uint64_t)(gms-0xf0+1)*4*1024*1024:0);
    VFBLOG("Latitude memory: GOP=0x%llx BAR2=0x%llx size=0x%llx permission=0x%x GSM=0x%llx DSM=0x%llx GGC=0x%x GGTTBytes=0x%llx DSMBytes=0x%llx",
           fBase,ap->getPhysicalAddress(),ap->getLength(),permission,gsm,dsm,ggc,tableBytes,dsmBytes);
    if(permission!=1||!gsm||gsm>UINT64_MAX-(8ull<<20)||dsm!=gsm+(8ull<<20)||!tableBytes||!dsmBytes) {
        VFBLOG("Latitude: direct stolen-memory permission or geometry invalid; declining");mmio->release();return false;
    }
    bool found = false;
    for (unsigned pipe = 0; pipe < 4; pipe++) {
        uint32_t step = pipe * 0x1000, ctl = rd(0x70180 + step), conf = rd(0x70008 + step);
        VFBLOG("Latitude pipe %c: conf=0x%x ctl=0x%x size=0x%x stride=0x%x surf=0x%x", 'A'+pipe,
               conf, ctl, rd(0x70190+step), rd(0x70188+step), rd(0x7019c+step));
        if (!(ctl & (1u<<31)) || !(conf & (1u<<31))) continue;
        uint32_t size = rd(0x70190+step), pitch = (rd(0x70188+step)&0x7ff)*64;
        uint64_t surf = rd(0x7019c+step)&~0xfffull;
        if (((ctl>>23)&31) != 8 || ((ctl>>10)&7) != 0 ||
            (size&0xffff)+1 != fWidth || (size>>16)+1 != fHeight || pitch != fRowBytes) continue;
        LatitudeScanout mapping={fBase,gsm,dsm,dsmBytes,ap->getPhysicalAddress(),ap->getLength(),surf,(uint64_t)pitch*fHeight,tableBytes};
        uint64_t pages=(mapping.bytes+4095)/4096,firstPTE=(surf/4096)*8;
        if(!mapping.bytes||mapping.bytes>(128ull<<20)||firstPTE>tableBytes||pages>(tableBytes-firstPTE)/8) {
            VFBLOG("Latitude: scanout PTE span outside GGTT; declining pipe");continue;
        }
        uint64_t tableOffset=firstPTE&~4095ull,inPage=firstPTE-tableOffset;
        uint64_t mapBytes=(inPage+pages*8+4095)&~4095ull;
        IODeviceMemory *ptMemory=IODeviceMemory::withRange(gsm+tableOffset,mapBytes);
        IOMemoryMap *ptMap=ptMemory?ptMemory->map(kIOMapInhibitCache):nullptr;
        OSSafeReleaseNULL(ptMemory);
        if(!ptMap){VFBLOG("Latitude: direct GSM page table mapping failed");continue;}
        const volatile uint64_t *pt=(const volatile uint64_t *)(ptMap->getVirtualAddress()+inPage);
        uint64_t cpu=0;LatitudeMapResult translated=latitudeTranslate(mapping,pt,(size_t)pages,&cpu);
        VFBLOG("Latitude translation: surf=0x%llx firstPTE=0x%llx lastPTE=0x%llx pages=%llu result=%u cpu=0x%llx",
               surf,pt[0],pt[pages-1],pages,(unsigned)translated,cpu);
        ptMap->release();
        if(translated!=LatitudeMapOK)continue;
        uint32_t ht = rd(0x60000+step), vt = rd(0x6000c+step);
        uint32_t hs = rd(0x60008+step), vs = rd(0x60014+step);
        uint32_t htotal=(ht>>16)+1, vtotal=(vt>>16)+1;
        uint32_t hstart=(hs&0xffff)+1, hend=(hs>>16)+1;
        uint32_t vstart=(vs&0xffff)+1, vend=(vs>>16)+1;
        if ((ht&0xffff)+1 != fWidth || (vt&0xffff)+1 != fHeight ||
            htotal<=fWidth || vtotal<=fHeight || hstart<fWidth || hend<=hstart || hend>htotal ||
            vstart<fHeight || vend<=vstart || vend>vtotal) continue;
        uint32_t frame0=rd(0x70040+step); uint64_t t0=mach_absolute_time(); IOSleep(250);
        uint32_t frames=rd(0x70040+step)-frame0; uint64_t ns=0;
        absolutetime_to_nanoseconds(mach_absolute_time()-t0,&ns);
        if (!ns || frames<5 || frames>100) { VFBLOG("Latitude: panel frame counter did not advance plausibly"); continue; }
        uint64_t mhz=(uint64_t)frames*1000000000000ull/ns;
        if (mhz<30000 || mhz>240000) continue;
        fHT=htotal; fVT=vtotal; fHSO=hstart-fWidth; fHSW=hend-hstart;
        fVSO=vstart-fHeight; fVSW=vend-vstart; fRefreshMHz=(uint32_t)mhz;
        fHz=(uint32_t)((mhz+500)/1000); fFrameNs=ns/frames;
        fPclk=(uint64_t)htotal*vtotal*mhz/1000; fPanelTiming=true;
        fBase=cpu;fPanelMemoryValidated=true;
        setProperty("LatitudePanelPipe",pipe,32); setProperty("LatitudePanelValidated",kOSBooleanTrue);
        setProperty("LatitudePanelScanout",fBase,64); setProperty("LatitudePanelRefreshMilliHz",mhz,64);
        VFBLOG("Latitude: validated firmware panel %ux%u pitch %u pipe %c at %u.%03u Hz",fWidth,fHeight,fRowBytes,
               'A'+pipe,fRefreshMHz/1000,fRefreshMHz%1000);
        uint32_t mmW=0,mmH=0;
        if(latitudeEDIDGeometry(latitudePanelEDID,sizeof latitudePanelEDID,fWidth,fHeight,&mmW,&mmH)){
            memcpy(fEdid,latitudePanelEDID,sizeof latitudePanelEDID);fEdidLen=sizeof latitudePanelEDID;
            setProperty("LatitudePanelWidthMM",mmW,32);setProperty("LatitudePanelHeightMM",mmH,32);
            setProperty("LatitudePanelEDIDSource","Dell Linux capture 20261012 eDP-1");
        }
        fBacklight=LatitudeBacklightHandler::create(mmio,pipe);
        found=true; break;
    }
    mmio->release();
    if (!found) VFBLOG("Latitude: no linear firmware plane matches GOP; declining");
    return found;
}
#endif

#include "latitude_gpu_identity_kernel.inc"

bool NMXVGAFB::start(IOService *provider)
{
#ifdef LATITUDE_MTL_PANEL
    fGammaLock=IOLockAlloc();if(!fGammaLock)return false;latitudeGammaIdentity(&fGamma);
    setProperty("LatitudeGammaBackend","CPU scanout LUT; identity uses bulk copy");
    setProperty(kIOFBGammaWidthKey,16ull,32);setProperty(kIOFBGammaCountKey,256ull,32);
#endif
    // the mode the firmware lit (GOP): base, size and pitch of the linear framebuffer
    VFBLOG("start: reading the firmware framebuffer");
    PE_Video v;
    bzero(&v, sizeof(v));
    IOPlatformExpert *pe = getPlatform();
    if (!pe || pe->getConsoleInfo(&v) != kIOReturnSuccess || !v.v_baseAddr || v.v_depth != 32 ||
        v.v_width < 640 || v.v_height < 480 || v.v_rowBytes < v.v_width * 4) {
        VFBLOG("no usable firmware framebuffer (base 0x%lx %lux%lu depth %lu pitch %lu) - leaving the display to IONDRV",
               (unsigned long)v.v_baseAddr, (unsigned long)v.v_width, (unsigned long)v.v_height,
               (unsigned long)v.v_depth, (unsigned long)v.v_rowBytes);
        return false;
    }
#ifdef LATITUDE_MTL_PANEL
    if(v.v_baseAddr&0xfff){VFBLOG("Latitude: GOP base is not page aligned; declining");return false;}
#endif
    fBase = (IOPhysicalAddress)(v.v_baseAddr & ~0xFFFul);
    fWidth = (uint32_t)v.v_width; fHeight = (uint32_t)v.v_height; fRowBytes = (uint32_t)v.v_rowBytes;
    {
        uint32_t hz = 0;
        if (PE_parse_boot_argn("nmxvgafb_hz", &hz, sizeof hz) && hz >= 30 && hz <= 240) { fHz = hz; fHzForced = true; }
    }
    // the framebuffer must lie in one of the device's memory BARs, or it is not this device's
#ifdef LATITUDE_MTL_PANEL
    IOPCIDevice *pci = OSDynamicCast(IOPCIDevice,provider->getProvider());
    if (!pci) return false;
#else
    IOPCIDevice *pci = (IOPCIDevice *)provider;
#endif
#ifdef LATITUDE_MTL_PANEL
    for(unsigned attempt=0;attempt<500&&pci->getProperty("nmxgpu-claimed")!=kOSBooleanTrue;attempt++)IOSleep(10);
    if(pci->getProperty("nmxgpu-claimed")!=kOSBooleanTrue){VFBLOG("Latitude: render driver not ready after 5 seconds; declining");return false;}
    if (!adoptLatitudePanel(pci)) return false;
#endif
#ifdef LATITUDE_MTL_PANEL
    bool inBar = fPanelMemoryValidated;
#else
    bool inBar = false;
#endif
    for (UInt32 i = 0; i < pci->getDeviceMemoryCount() && !inBar; i++) {
        IODeviceMemory *m = pci->getDeviceMemoryWithIndex(i);
        if (m && fBase >= m->getPhysicalAddress() &&
            fBase + (uint64_t)fRowBytes * fHeight <= m->getPhysicalAddress() + m->getLength()) inBar = true;
    }
    if (!inBar) { VFBLOG("firmware framebuffer 0x%llx is not in this device's BARs", (unsigned long long)fBase); return false; }
    // The firmware framebuffer (IONDRVFramebuffer, from the system kernel collection) must be gone before this one
    // starts: with both on one device the kernel froze (console and serial stopped at "IOG flags"), or panicked in a
    // power-change callout. Both are in the IOFramebuffer match category, which this driver now holds, so one that has
    // not attached by now never will: no waiting (the 10 s wait it once had held WindowServer's start for nothing).
    IOService *ndrv = nullptr;
    if (OSIterator *it = pci->getClientIterator()) {
        while (OSObject *o = it->getNextObject()) {
            IOService *c = OSDynamicCast(IOService, o);
            if (c && c != this && c->metaCast("IOFramebuffer") && !c->isInactive()) { ndrv = c; ndrv->retain(); break; }
        }
        it->release();
    }
    if (ndrv) {
#ifdef LATITUDE_MTL_PANEL
        VFBLOG("Latitude: existing framebuffer %s is active; declining takeover", ndrv->getName());
        ndrv->release(); return false;
#endif
        VFBLOG("start: terminating %s (%s) on this device", ndrv->getName(), ndrv->getMetaClass()->getClassName());
        bool ok = ndrv->terminate(kIOServiceRequired | kIOServiceSynchronous);
        VFBLOG("start: terminate %s", ok ? "done" : "FAILED - declining");
        ndrv->release();
        if (!ok) return false;
    }
    pci->setMemoryEnable(true);
    {
        int off = 0;
#ifdef LATITUDE_MTL_PANEL
        off = 1;
#endif
        if (off || PE_parse_boot_argn("-nmxvgafbnovirtio", &off, sizeof(off))) VFBLOG("start: virtio-gpu off (-nmxvgafbnovirtio)");
        else if (!startVirtio(pci, fBase)) VFBLOG("start: no virtio-gpu scan-out - the VGA BAR stays the scan-out");
    }
    setTiming();
    fAdvW = fWidth; fAdvH = fHeight;
    VFBLOG("start: %ux%u pitch %u at 0x%llx, %u.%03u Hz, calling IOFramebuffer::start", fWidth, fHeight, fRowBytes,
           (unsigned long long)fBase, fRefreshMHz / 1000, fRefreshMHz % 1000);
#ifdef LATITUDE_MTL_PANEL
    if(fBacklight){
        if(fBacklight->attach(this)){fBacklight->registerService(kIOServiceSynchronous);setProperty(kIODisplayHasBacklightKey,kOSBooleanTrue);}
        else {fBacklight->disable();OSSafeReleaseNULL(fBacklight);}
    }
#endif
    if (!IOFramebuffer::start(provider)) { VFBLOG("start: IOFramebuffer::start failed"); return false; }
    VFBLOG("start: IOFramebuffer::start returned");
    if (fVirtioOn) {
        fSurfMap = fScanBuf->map();         // ordinary cached RAM: the host reads it through the dmabuf
        // a display surface of the largest mode: its height padded (2880x1800 asks for 1808 rows), page-rounded
        nmxVramReserve((fScanCap + fScanCap / 64 + PAGE_SIZE - 1) & ~(unsigned long long)(PAGE_SIZE - 1));
    } else if (IODeviceMemory *sm = IODeviceMemory::withRange(fBase, (IOPhysicalLength)fRowBytes * fHeight)) {
        fSurfMap = sm->map(kIOMapWriteCombineCache); sm->release();
    }
    if (fSurfMap) {
        // head 0 of the GPU's accelerator (NMXAccel watches for this class)
        setProperty("IOFBDependentID", 0x4E4D58560000001ull, 64);
        setProperty("IOFBDependentIndex", 0ull, 32);
        // what WindowServer may place its flip surfaces in (NVRMFB reports its VRAM here); system memory backs them
#ifndef LATITUDE_MTL_PANEL
        setProperty("IOFBMemorySize", (unsigned long long)(1ull << 30), 64);
#endif
    } else VFBLOG("the framebuffer would not map write-combined: no accelerated frames for it");
    setProperty("built-in", kOSBooleanTrue);
    // the timing AGDC link-config answers read (NMXAGDC, from NVRMAGDC: NVRMFB publishes the same record); the same
    // numbers as getTimingInfoForDisplayMode: pixel clock in Hz, refresh in mHz, sync polarity positive
    publishTiming();
    if (fVirtioOn && (fHotplug = thread_call_allocate(hotplugFire, this)))
        fVirtio.setDisplayEventHandler(displayEvent, this);
    fWsaaKick = thread_call_allocate(wsaaKickFire, this);
    if (fVirtioOn && OSCompareAndSwapPtr(nullptr, this, (void * volatile *)&gVtInst)) {
        sysctl_register_oid(&sysctl__debug_nmxfb_vtiming);
        sysctl_register_oid(&sysctl__debug_nmxfb_vtrace);
        sysctl_register_oid(&sysctl__debug_nmxfb_vbl_lead_us);
        sysctl_register_oid(&sysctl__debug_nmxfb_vbl_stepped);
        sysctl_register_oid(&sysctl__debug_nmxfb_shot);
    }
    nanoseconds_to_absolutetime(fFrameNs, &fPeriodAbs);
    fVblLock = IOLockAlloc();
    if (fVblLock && kernel_thread_start(&NMXVGAFB::vblThreadMain, this, &fVblThread) == KERN_SUCCESS) {
        fVblTimer = true;
        // a display refresh: about 50 us of work each period, due within 1 ms
        uint64_t comp, cons; nanoseconds_to_absolutetime(50000, &comp); nanoseconds_to_absolutetime(1000000, &cons);
        thread_time_constraint_policy_data_t tc = { (uint32_t)fPeriodAbs, (uint32_t)comp, (uint32_t)cons, TRUE };
        kern_return_t pr = thread_policy_set(fVblThread, THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&tc,
                                             THREAD_TIME_CONSTRAINT_POLICY_COUNT);
        if (pr != KERN_SUCCESS) VFBLOG("VBL thread: time-constraint policy refused (0x%x)", pr);
        uint64_t d; clock_interval_to_deadline((uint32_t)(fFrameNs / 1000), kMicrosecondScale, &d);
        fVblNext = d; vblArm(d);
    }
    // No registerPowerDriver here (NMIntelFB has one): IOFramebuffer::start sets up power management with its own
    // power states, and a second registration with two states panicked in a power-change callout (read at 0x120).
    VFBLOG("%ux%u pitch %u at 0x%llx adopted from the firmware, %u Hz%s", fWidth, fHeight, fRowBytes,
           (unsigned long long)fBase, fHz, fVblTimer ? " (VBL thread, time-constraint, stepped wait)" : ", NO VBL TIMER");
    if (fSurfMap && (fHoldCap = thread_call_allocate(holdCapFire, this))) {
        fHolding = 1; adjustBusy(1);
        uint64_t d; clock_interval_to_deadline(kHoldCapSec, kSecondScale, &d);
        thread_call_enter_delayed(fHoldCap, d);
        VFBLOG("display hold: the registry stays busy until the accelerator arms this head (%u s at most)", kHoldCapSec);
        // IOFramebuffer::start registered this framebuffer, and NMXAccel arms on that publication: its nmxDisplayReady
        // can come before the hold above (measured: 13 ms earlier), and the hold then lasted the whole cap
        if (fAccel) releaseHold("the accelerator armed this head before the hold");
    }
#ifdef LATITUDE_MTL_PANEL
    if (const OSSymbol *fn=OSSymbol::withCStringNoCopy("latitudePanelFramebufferReady")) {
        provider->callPlatformFunction(fn,false,this,nullptr,nullptr,nullptr);fn->release();
    }
#endif
    return true;
}

void NMXVGAFB::stop(IOService *provider)
{
    fStopping = true;
#ifdef LATITUDE_MTL_PANEL
    if(fBacklight){fBacklight->disable();fBacklight->detach(this);OSSafeReleaseNULL(fBacklight);}
#endif
    if (OSCompareAndSwapPtr(this, nullptr, (void * volatile *)&gVtInst)) {
        sysctl_unregister_oid(&sysctl__debug_nmxfb_vtiming);
        sysctl_unregister_oid(&sysctl__debug_nmxfb_vtrace);
        sysctl_unregister_oid(&sysctl__debug_nmxfb_vbl_lead_us);
        sysctl_unregister_oid(&sysctl__debug_nmxfb_vbl_stepped);
        sysctl_unregister_oid(&sysctl__debug_nmxfb_shot);
    }
    if (fHoldCap) { thread_call_cancel(fHoldCap); while (!thread_call_free(fHoldCap)) { thread_call_cancel(fHoldCap); IOSleep(5); } fHoldCap = nullptr; }
    fVirtio.setDisplayEventHandler(nullptr, nullptr);
    if (fHotplug) { thread_call_cancel(fHotplug); while (!thread_call_free(fHotplug)) { thread_call_cancel(fHotplug); IOSleep(5); } fHotplug = nullptr; }
    if (fWsaaKick) { thread_call_cancel(fWsaaKick); while (!thread_call_free(fWsaaKick)) { thread_call_cancel(fWsaaKick); IOSleep(5); } fWsaaKick = nullptr; }
    releaseHold("stopping");
    OSSafeReleaseNULL(fAccel);
    if (fVblThread) {
        IOLockLock(fVblLock);
        thread_wakeup((event_t)&fVblNext);
        while (!fVblThreadDone) IOLockSleep(fVblLock, (void *)&fVblThreadDone, THREAD_UNINT);
        IOLockUnlock(fVblLock);
        thread_deallocate(fVblThread);
        fVblThread = nullptr; fVblTimer = false;
    }
    if (fVblLock) { IOLockFree(fVblLock); fVblLock = nullptr; }
    OSSafeReleaseNULL(fSurfMap);
    // QEMU back in VGA mode (the firmware's framebuffer) before the blobs' memory goes
    fVirtioOn = false;
    if (fVirtio.ready()) fVirtio.reset();
    for (auto &b : fSurf) if (b.md) { b.md->complete(); b.md->release(); b = {}; }
    OSSafeReleaseNULL(fScanBuf);
    fCursorOn = false;
    OSSafeReleaseNULL(fCursorBuf);
    if (fScanLock) { IOLockFree(fScanLock); fScanLock = nullptr; }
    IOFramebuffer::stop(provider);
}

/*
 * The scan-out moves from the VGA BAR into guest RAM: a physically contiguous buffer the size of the firmware's mode
 * (the same pitch, so nothing else changes), given to the host as a blob resource and shown on scanout 0. The VGA
 * contents are copied over first, so the switch is invisible. fBase becomes the buffer's address: the aperture,
 * IOFBMemorySize users and NMXAccel's "live scanout" all see the RAM. Any failure resets the device (VGA mode again).
 */
bool NMXVGAFB::startVirtio(IOPCIDevice *pci, IOPhysicalAddress vgaBase)
{
    if (!fVirtio.init(pci)) return false;
    uint32_t hw = 0, hh = 0; bool en = false;
    if (fVirtio.displayInfo(&hw, &hh, &en))
        VFBLOG("virtio-gpu: host scanout 0 is %ux%u (%s)", hw, hh, en ? "enabled" : "disabled");
    if (!fVirtio.hasBlob()) {
        VFBLOG("virtio-gpu: the device offers no blob resources (QEMU: virtio-vga blob=true, memfd guest RAM)");
        fVirtio.reset();
        return false;
    }
    if (fVirtio.edid(fEdid, sizeof fEdid, &fEdidLen))
        VFBLOG("virtio-gpu: EDID %u bytes, '%c%c%c' product 0x%04x", fEdidLen, '@' + ((fEdid[8] >> 2) & 31),
               '@' + (((fEdid[8] & 3) << 3) | (fEdid[9] >> 5)), '@' + (fEdid[9] & 31), fEdid[10] | fEdid[11] << 8);
    else fEdidLen = 0;
    // the mode is the host window's size: QEMU then shows the scan-out 1:1 (the firmware's mode was stretched)
    const uint32_t vgaW = fWidth, vgaH = fHeight, vgaPitch = fRowBytes;
    if (en && hw >= 1024 && hh >= 600 && hw <= 7680 && hh <= 4320) {
        fWidth = hw; fHeight = hh; fRowBytes = (hw * 4 + 63) & ~63u;
    } else VFBLOG("virtio-gpu: keeping the firmware's %ux%u", fWidth, fHeight);
    uint64_t bytes = (uint64_t)fRowBytes * fHeight;
    if (bytes < kScanCapPx * 4) bytes = kScanCapPx * 4;
    bytes = (bytes + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);
    fScanCap = bytes;
    fScanBuf = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
        kIODirectionInOut | kIOMemoryPhysicallyContiguous, bytes, 0x000000FFFFFFF000ull);
    if (!fScanBuf) { VFBLOG("virtio-gpu: no %llu bytes of contiguous memory for the scan-out", bytes); fVirtio.reset(); return false; }
    const uint64_t phys = fScanBuf->getPhysicalSegment(0, nullptr, kIOMemoryMapperNone);
    bzero(fScanBuf->getBytesNoCopy(), bytes);
    if (fWidth != vgaW || fHeight != vgaH || fRowBytes != vgaPitch) {
        // a different mode: the firmware's picture doesn't fit, the scan-out starts black
    } else if (IODeviceMemory *vm = IODeviceMemory::withRange(vgaBase, (IOPhysicalLength)fRowBytes * fHeight)) {
        if (IOMemoryMap *m = vm->map(kIOMapReadOnly)) {
            memcpy(fScanBuf->getBytesNoCopy(), (const void *)m->getVirtualAddress(), (size_t)fRowBytes * fHeight);
            m->release();
        }
        vm->release();
    }
    if (!fVirtio.createBlob(fScanRes, phys, bytes) ||
        !fVirtio.setScanoutBlob(fScanRes, fWidth, fHeight, fRowBytes) ||
        !fVirtio.flush(fScanRes, 0, 0, fWidth, fHeight)) {
        VFBLOG("virtio-gpu: the blob scan-out was refused - back to VGA");
        fVirtio.reset();
        OSSafeReleaseNULL(fScanBuf);
        return false;
    }
    fBase = (IOPhysicalAddress)phys;
    startCursor();
    fScanLock = IOLockAlloc();
    fCurRes = fScanRes; fCurW = fWidth; fCurH = fHeight;
    fVirtioOn = true;
    setProperty("NMXVirtioScanout", true);
    VFBLOG("virtio-gpu: scan-out is a %ux%u blob in guest RAM at 0x%llx (was VGA 0x%llx)", fWidth, fHeight,
           (unsigned long long)phys, (unsigned long long)vgaBase);
    return true;
}

/*
 * "VRAM" for NMXAccel's video resources (WindowServer's display surfaces): IOAcceleratorFamily2 wires each one into
 * video memory before a display-pipe transaction may present it, and NMXAccel asks the head's framebuffer for that
 * memory, as NVRMFB grants BAR1 VRAM. A VM display has no VRAM; physically contiguous system memory stands in (the
 * resource's aperture descriptor is a single physical range, and the pipe copies from the kernel address).
 * WindowServer reallocates its display surfaces on every mode change, and after an hour or two guest RAM no longer
 * has 20 MB contiguous runs (a fullscreen QEMU window then left the display black: every allocation refused, every
 * display-pipe transaction failed). So the framebuffer reserves slots for them at start, like a GPU's VRAM; only a
 * surface larger than a slot, or one more than the slots hold, is allocated on demand.
 */
static constexpr unsigned long long kVramBudget = 512ull << 20;
static volatile SInt64 gVramUsed = 0;
static volatile SInt64 gVramCookie = 0;
static unsigned gVramLogs = 0;
static constexpr unsigned kVramSlots = 4;            // WindowServer's three display surfaces and one spare
static IOBufferMemoryDescriptor *gVramSlot[kVramSlots];
static volatile UInt32 gVramSlotBusy = 0;            // bit per slot
static unsigned long long gVramSlotBytes = 0;
static IOLock *gVramLock = nullptr;

// These buffers are exported as raw physical spans, not IOMMU DMA addresses.
// PhysicallyContiguous alone permits scattered host pages with a mapper.
static const IOOptionBits kNmxRawPhysicalOptions = kIODirectionInOut |
    kIOMemoryKernelUserShared | kIOMemoryPhysicallyContiguous |
    kIOMemoryHostPhysicallyContiguous | kIOMemoryMapperNone;
static void nmxVramReleaseBuffer(IOBufferMemoryDescriptor *b)
{
    if (gVramLock) {
        IOLockLock(gVramLock);
        for (unsigned i=0;i<kVramSlots;i++)
            if (gVramSlot[i]==b) gVramSlotBusy &= ~(1u<<i);
        IOLockUnlock(gVramLock);
    }
    b->release();
}



static void nmxVramReserve(unsigned long long slotBytes)
{
    if (!gVramLock && !(gVramLock = IOLockAlloc())) return;
    IOLockLock(gVramLock);
    if (!gVramSlotBytes) {
        unsigned n = 0;
        for (; n < kVramSlots; n++)
            if (!(gVramSlot[n] = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
                      kNmxRawPhysicalOptions, slotBytes, 0x000000FFFFFFF000ull))) break;
        if (n) gVramSlotBytes = slotBytes;
        for (unsigned i = n; i < kVramSlots; i++) gVramSlotBusy |= 1u << i;
        VFBLOG("VRAM: %u of %u slots of %llu bytes reserved", n, kVramSlots, slotBytes);
    }
    IOLockUnlock(gVramLock);
}

static IOReturn nmxVramAlloc(NVRMVramRequest *r)
{
    if (!r->size || r->size > kVramBudget) return kIOReturnBadArgument;
    const unsigned long long want = (r->size + PAGE_SIZE - 1) & ~(unsigned long long)(PAGE_SIZE - 1);
    if (!want) return kIOReturnBadArgument;
    if ((unsigned long long)OSAddAtomic64((SInt64)want, &gVramUsed) + want > kVramBudget) {
        OSAddAtomic64(-(SInt64)want, &gVramUsed);
        VFBLOG("nvAllocVram(%llu): REFUSED, budget of %llu bytes spent", want, kVramBudget);
        return kIOReturnNoSpace;
    }
    IOBufferMemoryDescriptor *b = nullptr;
    if (gVramLock && want <= gVramSlotBytes) {
        IOLockLock(gVramLock);
        for (unsigned i = 0; i < kVramSlots && !b; i++)
            if (!(gVramSlotBusy & (1u << i))) { gVramSlotBusy |= 1u << i; b = gVramSlot[i]; b->retain(); }
        IOLockUnlock(gVramLock);
    }
    if (!b) b = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
        kNmxRawPhysicalOptions, want, 0x000000FFFFFFF000ull);
    if (!b) {
        OSAddAtomic64(-(SInt64)want, &gVramUsed);
        VFBLOG("nvAllocVram(%llu): no contiguous system memory", want);
        return kIOReturnNoMemory;
    }
    IOByteCount span=0;
    const IOPhysicalAddress physical=b->getPhysicalSegment(0,&span,kIOMemoryMapperNone);
    if (!physical || span<want || b->getLength()<want || !b->getBytesNoCopy() ||
        physical>~(IOPhysicalAddress)0-(want-1)) {
        VFBLOG("nvAllocVram(%llu): refusing noncontiguous/invalid raw span phys 0x%llx length %llu",want,(uint64_t)physical,(uint64_t)span);
        nmxVramReleaseBuffer(b);
        OSAddAtomic64(-(SInt64)want,&gVramUsed);
        return kIOReturnNoMemory;
    }
    bzero(b->getBytesNoCopy(), want);
    r->handle = b; r->kva = b->getBytesNoCopy();
    r->phys = physical;
    r->actualSize = want;
    r->mappedTotal = (unsigned long long)gVramUsed;
    r->allocationCookie = (unsigned long long)OSAddAtomic64(1, &gVramCookie) + 1;
    if (gVramLogs < 16) { gVramLogs++;
        VFBLOG("nvAllocVram(%llu): verified host-physical phys 0x%llx span %llu kva %p [%llu of %llu]", want, r->phys, (unsigned long long)span, r->kva,
               r->mappedTotal, kVramBudget); }
    return kIOReturnSuccess;
}

static IOReturn nmxVramFree(NVRMVramRequest *r)
{
    IOBufferMemoryDescriptor *b = OSDynamicCast(IOBufferMemoryDescriptor, (OSObject *)r->handle);
    if (!b) return kIOReturnBadArgument;
    OSAddAtomic64(-(SInt64)(r->actualSize ? r->actualSize : b->getLength()), &gVramUsed);
    nmxVramReleaseBuffer(b);
    r->handle = nullptr; r->kva = nullptr; r->phys = 0; r->actualSize = 0; r->allocationCookie = 0;
    return kIOReturnSuccess;
}

// NMXAccel's display pipe asks its heads for their scan-out: a CPU mapping and no GPU address, so frames arrive by copy
IOReturn NMXVGAFB::callPlatformFunction(const OSSymbol *fn, bool wait, void *p1, void *p2, void *p3, void *p4)
{
#ifdef LATITUDE_MTL_PANEL
    if(fn && fn->isEqualTo("latitudeSetTransactionGamma")){
        auto gamma=(const LatitudeGamma *)p1;if(!gamma||gamma->version!=LATITUDE_GAMMA_VERSION||fStopping)return kIOReturnBadArgument;
        IOReturn result=latitudeSetGamma(3,256,8,(void *)gamma->rgb);
        if(result==kIOReturnSuccess)setProperty("LatitudeGammaSource","Display transaction");return result;
    }
    if(fn && fn->isEqualTo(LATITUDE_GAMMA_FUNCTION)){
        auto out=(LatitudeGamma *)p1;if(!out||out->version!=LATITUDE_GAMMA_VERSION||!fGammaLock||fStopping)return kIOReturnBadArgument;
        IOLockLock(fGammaLock);*out=fGamma;IOLockUnlock(fGammaLock);return kIOReturnSuccess;
    }
#endif
    // NMXAccel armed this head (phase 1): remember it for open(), and let WindowServer go on
    if (fn && fn->isEqualTo("nmxDisplayReady")) {
        IOService *a = OSDynamicCast(IOService, (OSObject *)p1);
        if (!a) return kIOReturnBadArgument;
        if (!fAccel) { a->retain(); if (!OSCompareAndSwapPtr(nullptr, a, (void *volatile *)&fAccel)) a->release(); }
        releaseHold("the accelerator armed this head");
        return kIOReturnSuccess;
    }
    if (fn && (fn->isEqualTo(NVRM_VRAM_FN_ALLOC) || fn->isEqualTo(NVRM_VRAM_FN_FREE))) {
        NVRMVramRequest *r = (NVRMVramRequest *)p1;
        if (!r || r->version != NVRM_VRAM_ABI_VERSION) return kIOReturnBadArgument;
        return fn->isEqualTo(NVRM_VRAM_FN_ALLOC) ? nmxVramAlloc(r) : nmxVramFree(r);
    }
    if (fn && fn->isEqualTo(NMX_SCANOUT_FN_SURFACE)) {
        NMXScanoutSurface *r = (NMXScanoutSurface *)p1;
        if (!r || r->version != NMX_SCANOUT_ABI_VERSION) return kIOReturnBadArgument;
        return scanoutSurface(r);
    }
    if (fn && fn->isEqualTo(NMX_SCANOUT_FN_HOME)) return scanoutHome();
    if (fn && fn->isEqualTo(NVRM_VRAM_FN_SCANOUT)) {
        NVRMVramRequest *r = (NVRMVramRequest *)p1;
        if (!r || r->version != NVRM_VRAM_ABI_VERSION) return kIOReturnBadArgument;
        if (!fSurfMap) return kIOReturnNotReady;
        r->kva = (void *)fSurfMap->getVirtualAddress();
        // The address is real CPU-physical memory (the VM display's BAR), not another GPU's VRAM: reported, the
        // accelerator's display pipe gets a framebuffer resource over it (its "live scanout" path), which
        // WindowServer needs to set up page flipping. With phys 0 ("CPU-only, frames are copied") CoreDisplay
        // failed MPAccessSurfaceForDisplayDevice and WindowServer composited on the CPU.
        r->phys = fBase; r->actualSize = (unsigned long long)fRowBytes * fHeight;
        r->width = fWidth; r->height = fHeight; r->pitch = fRowBytes;
        return kIOReturnSuccess;
    }
    return IOFramebuffer::callPlatformFunction(fn, wait, p1, p2, p3, p4);
}

void NMXVGAFB::dropSurfBlob(SurfBlob &b)
{
    if (!b.md) return;
    fVirtio.unrefResource(b.res);
    b.md->complete(); b.md->release();
    b = {};
}

/*
 * The presented surface becomes the scan-out: its pages (prepared, so they stay put) as a blob, cached per surface.
 * The host reads the surface itself, so a later write into it (the Metal plugin copies a frame out after the GPU
 * signals) shows with the next VBL flush. Refused (and NMXAccel copies instead) without virtio, for a surface that
 * is not this mode's size and format, or when the host refuses the blob.
 */
IOReturn NMXVGAFB::scanoutSurface(NMXScanoutSurface *r)
{
    if (!fVirtioOn || !fScanLock) return kIOReturnNotReady;
    IOMemoryDescriptor *md = OSDynamicCast(IOMemoryDescriptor, (OSObject *)r->md);
    if (!md || r->format != 0x42475241u || r->width != fWidth || r->height != fHeight || r->pitch < 4 * r->width)
        return kIOReturnUnsupported;
    const uint64_t bytes = ((uint64_t)r->pitch * r->height + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);
    if (md->getLength() < bytes) return kIOReturnUnsupported;
    const uint64_t t0 = mach_absolute_time();
    IOReturn ret = kIOReturnSuccess;
    uint64_t shownAt = 0;
    bool host = false;
    IOLockLock(fScanLock);
    SurfBlob *hit = nullptr, *victim = nullptr;
    for (auto &b : fSurf) {
        if (b.md == md && b.surfaceId == r->surfaceId && b.w == r->width && b.h == r->height && b.pitch == r->pitch) { hit = &b; break; }
        if (b.res == fCurRes && b.md) continue;                 // never the one on screen
        if (!victim || !b.md || (victim->md && b.lastUse < victim->lastUse)) victim = &b;
    }
    if (!hit && victim) {
        dropSurfBlob(*victim);
        NMXVirtioGPU::Range *rg = (NMXVirtioGPU::Range *)IOMalloc(sizeof(NMXVirtioGPU::Range) * NMXVirtioGPU::kMaxEntries);
        uint32_t n = 0; bool fits = rg != nullptr, prepared = false;
        if (fits) { prepared = md->prepare() == kIOReturnSuccess; fits = prepared; }
        for (uint64_t off = 0; fits && off < bytes; ) {
            IOByteCount len = 0;
            addr64_t pa = md->getPhysicalSegment(off, &len, kIOMemoryMapperNone);
            if (!pa || !len) { fits = false; break; }
            if (len > bytes - off) len = (IOByteCount)(bytes - off);
            if (n && rg[n - 1].addr + rg[n - 1].len == pa && (uint64_t)rg[n - 1].len + len < 0x80000000ull) rg[n - 1].len += (uint32_t)len;
            else if (n < NMXVirtioGPU::kMaxEntries) rg[n++] = { pa, (uint32_t)len };
            else fits = false;
            off += len;
        }
        const uint32_t res = fNextRes++;
        if (fits && fVirtio.createBlobSG(res, rg, n, bytes)) {
            md->retain();
            *victim = { md, r->surfaceId, res, r->width, r->height, r->pitch, 0 };
            hit = victim;
            if (fSwitchLogs < 8) VFBLOG("virtio-gpu: surface 0x%x is blob %u (%u ranges, %llu bytes)", r->surfaceId, res, n, bytes);
        } else {
            if (prepared) md->complete();
            if (fSwitchLogs < 8) VFBLOG("virtio-gpu: surface 0x%x: no blob (%u ranges of at most %u%s)", r->surfaceId, n, NMXVirtioGPU::kMaxEntries,
                                     fits ? ", refused by the host" : ", not describable");
            ret = kIOReturnUnsupported;
        }
        if (rg) IOFree(rg, sizeof(NMXVirtioGPU::Range) * NMXVirtioGPU::kMaxEntries);
    } else if (!hit) ret = kIOReturnNoResources;
    if (hit) {
        hit->lastUse = t0;
        if (fLastPresent) {
            uint64_t gap = 0; absolutetime_to_nanoseconds(t0 - fLastPresent, &gap);
            unsigned k = 0; while (k < kHist - 1 && gap >= kHistMs[k] * 1000000ull) k++;
            fPresentHist[k]++;
        }
        fLastPresent = t0;
        const bool final = r->version >= 2 && (r->flags & NMX_SCANOUT_FINAL);
        if (final) {
            // A final frame's flush is fenced: the host answers once it has shown it, so the flip (and the
            // accelerator's transaction) completes then, and the answer is this display's vblank. Switching to
            // another blob sends SET_SCANOUT_BLOB with it, in one round trip.
            const uint64_t f0 = mach_absolute_time();
            trace('S', t0, r->surfaceId);
            const bool ok = hit->res != fCurRes ? fVirtio.flipShown(hit->res, hit->w, hit->h, hit->pitch, &shownAt)
                                                : fVirtio.flushShown(hit->res, hit->w, hit->h, &shownAt);
            if (!ok) ret = kIOReturnIOError;
            else {
                fCurRes = hit->res; fCurW = hit->w; fCurH = hit->h; fCurFinal = true;
                if (fVirtio.interrupts()) {
                    uint64_t rt = 0; absolutetime_to_nanoseconds(shownAt - f0, &rt);
                    trace('A', shownAt, (uint32_t)(rt / 1000));
                    if (fDeferredSeen < kPacedSeen && rt > kPacedNs && ++fDeferredSeen == kPacedSeen)
                        VFBLOG("virtio-gpu: the host answers fenced flushes when it has shown the frame - VBL follows its latch points");
                    host = fDeferredSeen >= kPacedSeen;
                }
            }
        } else if (hit->res != fCurRes && !fVirtio.setScanoutBlob(hit->res, hit->w, hit->h, hit->pitch)) ret = kIOReturnIOError;
        else {
            fCurRes = hit->res; fCurW = hit->w; fCurH = hit->h; fCurFinal = false;
            fVirtio.flush(fCurRes, 0, 0, fCurW, fCurH);
        }
    }
    IOLockUnlock(fScanLock);
    if (host) hostLatch(shownAt);
    // a frame was shown while WindowServer still has this head in To_Accelerated: its first update did not complete it
    if (ret == kIOReturnSuccess && fCurFinal && fWsaaKick && (fWsaa & kWsaaStateMask) == kWsaaToAccelerated &&
        OSCompareAndSwap(0, 1, &fWsaaKicked))
        thread_call_enter(fWsaaKick);
    uint64_t ns = 0; absolutetime_to_nanoseconds(mach_absolute_time() - t0, &ns);
    r->ns = ns;
    fSwitchN++; fSwitchNs += ns; if (ns > fSwitchMaxNs) fSwitchMaxNs = ns;
    if (fSwitchLogs < 8) { fSwitchLogs++; VFBLOG("virtio-gpu: scan-out -> surface 0x%x: 0x%x in %llu us", r->surfaceId, ret, ns / 1000); }
    return ret;
}

void NMXVGAFB::startCursor()
{
    if (!fVirtio.hasCursor()) return;
    const uint64_t one = (uint64_t)kCursorDim * kCursorDim * 4;
    fCursorBuf = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
        kIODirectionInOut | kIOMemoryPhysicallyContiguous, 2 * one, 0x000000FFFFFFF000ull);
    if (!fCursorBuf) return;
    bzero(fCursorBuf->getBytesNoCopy(), 2 * one);
    const uint64_t phys = fCursorBuf->getPhysicalSegment(0, nullptr, kIOMemoryMapperNone);
    if (!fVirtio.createBlob(kCursorRes, phys, one) || !fVirtio.createBlob(kCursorHiddenRes, phys + one, one)) {
        VFBLOG("virtio-gpu: no cursor resources - the cursor stays in software");
        OSSafeReleaseNULL(fCursorBuf);
        return;
    }
    fCursorOn = true;
    VFBLOG("virtio-gpu: hardware cursor %ux%u ARGB32 on the cursor queue", kCursorDim, kCursorDim);
}

// the host shows the image (hot spot at x + hot x, y + hot y) or the transparent one
void NMXVGAFB::cursorShow(bool visible, SInt32 x, SInt32 y)
{
    const uint32_t hx = (uint32_t)(x + (SInt32)fCursorHotX), hy = (uint32_t)(y + (SInt32)fCursorHotY);
    fVirtio.cursorUpdate(visible ? kCursorRes : kCursorHiddenRes, fCursorHotX, fCursorHotY, hx, hy);
    fCursorShown = visible;
}

IOReturn NMXVGAFB::setCursorImage(void *cursorImage)
{
    if (!fCursorOn) return kIOReturnUnsupported;
    UInt32 *px = (UInt32 *)fCursorBuf->getBytesNoCopy();
    IOHardwareCursorDescriptor d; bzero(&d, sizeof d);
    d.majorVersion = kHardwareCursorDescriptorMajorVersion;
    d.minorVersion = kHardwareCursorDescriptorMinorVersion;
    d.width = kCursorDim; d.height = kCursorDim;
    d.bitDepth = 32;
    d.supportedSpecialEncodings = kTransparentEncodedPixel;
    IOHardwareCursorInfo ci; bzero(&ci, sizeof ci);
    ci.majorVersion = kHardwareCursorInfoMajorVersion;
    ci.minorVersion = kHardwareCursorInfoMinorVersion;
    UInt32 *staging = (UInt32 *)IOMalloc(kCursorDim * kCursorDim * 4);
    if (!staging) return kIOReturnNoMemory;
    bzero(staging, kCursorDim * kCursorDim * 4);
    ci.hardwareCursorData = (UInt8 *)staging;
    if (!convertCursorImage(cursorImage, &d, &ci) || !ci.cursorWidth || ci.cursorWidth > kCursorDim ||
        !ci.cursorHeight || ci.cursorHeight > kCursorDim) {
        IOFree(staging, kCursorDim * kCursorDim * 4);
        // larger than the plane (an enlarged accessibility cursor): IOGraphics draws it in software
        if (fCursorHaveImage && fCursorShown) cursorShow(false, 0, 0);
        fCursorHaveImage = false;
        return kIOReturnUnsupported;
    }
    // QEMU's cursor is 64x64 ARGB32 (straight alpha), the image's rows padded out to it
    for (uint32_t y = 0; y < kCursorDim; y++)
        for (uint32_t x = 0; x < kCursorDim; x++)
            px[y * kCursorDim + x] = (y < ci.cursorHeight && x < ci.cursorWidth) ? staging[y * ci.cursorWidth + x] : 0;
    IOFree(staging, kCursorDim * kCursorDim * 4);
    __asm__ __volatile__("mfence" ::: "memory");
    fCursorHotX = ci.cursorHotSpotX; fCursorHotY = ci.cursorHotSpotY;
    fCursorHaveImage = true;
    if (++fCursorImages <= 4)
        VFBLOG("cursor image %llu: %ux%u hot %u,%u", fCursorImages, (unsigned)ci.cursorWidth,
               (unsigned)ci.cursorHeight, (unsigned)ci.cursorHotSpotX, (unsigned)ci.cursorHotSpotY);
    // the host copies the image when it takes the update: re-send it even when already shown
    if (fCursorShown) cursorShow(true, fCursorX, fCursorY);
    return kIOReturnSuccess;
}

IOReturn NMXVGAFB::setCursorState(SInt32 x, SInt32 y, bool visible)
{
    if (!fCursorOn) return kIOReturnUnsupported;
    visible = visible && fCursorHaveImage;
    fCursorX = x; fCursorY = y;
    if (visible != fCursorShown) cursorShow(visible, x, y);
    else if (visible) { fCursorMoves++; fVirtio.cursorMove((uint32_t)(x + (SInt32)fCursorHotX), (uint32_t)(y + (SInt32)fCursorHotY)); }
    return kIOReturnSuccess;
}

NMXVGAFB::Timing NMXVGAFB::timingFor(uint32_t w, uint32_t h) const
{
#ifdef LATITUDE_MTL_PANEL
    if (fPanelTiming && w==fWidth && h==fHeight)
        return { fHT,fVT,fHSO,fHSW,fVSO,fVSW,fRefreshMHz,fPclk,fFrameNs };
#endif
    Timing t;
    auto d = [this](unsigned i) -> uint32_t { return fEdid[54 + i]; };     // the first detailed timing descriptor
    const uint64_t pclk = (uint64_t)(d(0) | d(1) << 8) * 10000;
    const uint32_t ha = d(2) | (d(4) & 0xf0) << 4, hb = d(3) | (d(4) & 0x0f) << 8;
    const uint32_t va = d(5) | (d(7) & 0xf0) << 4, vb = d(6) | (d(7) & 0x0f) << 8;
    if (!fHzForced && fEdidLen >= 128 && pclk && ha == w && va == h && hb && vb) {
        t.ht = ha + hb; t.vt = va + vb; t.pclk = pclk;
        t.hso = d(8) | (d(11) & 0xc0) << 2; t.hsw = d(9) | (d(11) & 0x30) << 4;
        t.vso = d(10) >> 4 | (d(11) & 0x0c) << 2; t.vsw = (d(10) & 0x0f) | (d(11) & 0x03) << 4;
    } else {
        t.ht = w + 160; t.vt = h + 35; t.pclk = (uint64_t)t.ht * t.vt * fHz;
        t.hso = 48; t.hsw = 32; t.vso = 3; t.vsw = 5;
    }
    const uint64_t frame = (uint64_t)t.ht * t.vt;
    t.frameNs = frame * 1000000000ull / t.pclk;
    t.mhz = (uint32_t)((t.pclk * 1000 + frame / 2) / frame);
    return t;
}

void NMXVGAFB::setTiming()
{
    const Timing t = timingFor(fWidth, fHeight);
    fHT = t.ht; fVT = t.vt; fHSO = t.hso; fHSW = t.hsw; fVSO = t.vso; fVSW = t.vsw; fPclk = t.pclk;
    fFrameNs = t.frameNs; fRefreshMHz = t.mhz;
    if (!fHzForced) fHz = (fRefreshMHz + 500) / 1000;
    if (fVblLock) { IOLockLock(fVblLock); nanoseconds_to_absolutetime(fFrameNs, &fPeriodAbs); IOLockUnlock(fVblLock); }
}

void NMXVGAFB::publishTiming()
{
    UInt32 k5[13] = { 0x4b350001u, (UInt32)fPclk, fWidth, fWidth + fHSO, fWidth + fHSO + fHSW, fHT, fHeight,
                      fHeight + fVSO, fHeight + fVSO + fVSW, fVT, fRefreshMHz, 1u, 1u };
    if (OSData *k5d = OSData::withBytes(k5, sizeof k5)) { setProperty("NVRMTiming", k5d); k5d->release(); }
}

void NMXVGAFB::displayEvent(void *p0)
{
    NMXVGAFB *me = (NMXVGAFB *)p0;
    if (me->fStopping || !me->fHotplug) return;
    // a window being dragged larger sends a stream of these: act once it has settled
    uint64_t d; clock_interval_to_deadline(300, kMillisecondScale, &d);
    thread_call_enter_delayed(me->fHotplug, d);
}

/*
 * The host's display changed (QEMU: the window was resized). The new size becomes the advertised mode - a new mode ID,
 * so IOGraphics and WindowServer see a different mode, not one that changed under them - and the connection-change
 * interrupt makes IOGraphics probe the modes (and the EDID, which QEMU regenerated for the size) again. WindowServer
 * then sets it as the default, and setDisplayMode switches the scan-out.
 */
void NMXVGAFB::hotplugFire(thread_call_param_t p0, thread_call_param_t)
{
    NMXVGAFB *me = (NMXVGAFB *)p0;
    if (me->fStopping || !me->fVirtioOn) return;
    uint32_t w = 0, h = 0; bool en = false;
    if (!me->fVirtio.displayInfo(&w, &h, &en)) return;
    if (!en || w < 1024 || h < 600 || w > 7680 || h > 4320) {
        VFBLOG("display event: host scanout 0 is %ux%u (%s) - not a mode, keeping %ux%u", w, h, en ? "enabled" : "disabled",
               me->fAdvW, me->fAdvH);
        return;
    }
    if (w == me->fAdvW && h == me->fAdvH) return;
    uint8_t e[sizeof me->fEdid]; uint32_t elen = 0;
    if (me->fVirtio.edid(e, sizeof e, &elen)) { memcpy(me->fEdid, e, elen); me->fEdidLen = elen; }
    if (w == me->fWidth && h == me->fHeight) me->fAdvID = me->fActID;
    else me->fAdvID = me->fNextID++;
    me->fAdvW = w; me->fAdvH = h;
    VFBLOG("display event: the host shows %ux%u - mode %d advertised (active %d, %ux%u)", w, h, (int)me->fAdvID,
           (int)me->fActID, me->fWidth, me->fHeight);
    if (me->fConnect.proc) me->fConnect.proc(me->fConnect.target, me->fConnect.ref);
}

// the head's IODisplay (AppleDisplay under its IODisplayConnect): IOFBConnect's "online" bit, which WindowServer reads
bool NMXVGAFB::hasIODisplay()
{
    bool found = false;
    OSIterator *it = getChildIterator(gIOServicePlane);
    if (!it) return false;
    while (OSObject *o = it->getNextObject()) {
        IOService *connect = OSDynamicCast(IOService, o);
        if (!connect || !connect->metaCast("IODisplayConnect")) continue;
        IORegistryEntry *disp = connect->getChildEntry(gIOServicePlane);
        if (disp && disp->metaCast("IODisplay")) { found = true; break; }
    }
    it->release();
    return found;
}

void NMXVGAFB::wsaaKickFire(thread_call_param_t p0, thread_call_param_t)
{
    NMXVGAFB *me = (NMXVGAFB *)p0;
    if (me->fStopping || (me->fWsaa & kWsaaStateMask) != kWsaaToAccelerated) return;
    if (!me->hasIODisplay()) { me->fWsaaKicked = 0; return; }   // the next shown frame asks again
    const UInt32 k = ++me->fWsaaKicks;
    // a connect change alone leaves it there; with a new mode (the current size under a new ID, as a host resize
    // advertises one) CoreDisplay reconfigures the display and completes on the next present
    me->fAdvW = me->fWidth; me->fAdvH = me->fHeight; me->fAdvID = me->fNextID++;
    VFBLOG("WindowServer left this head in To_Accelerated (0x%x) after a shown frame - mode %d advertised, connect change %u",
           me->fWsaa, (int)me->fAdvID, k);
    if (me->fConnect.proc) me->fConnect.proc(me->fConnect.target, me->fConnect.ref);
    // one that comes while WindowServer is still configuring the display changes nothing: look again, later each time
    if (k < kWsaaKickMax) {
        uint64_t d; clock_interval_to_deadline(1u << (k - 1), kSecondScale, &d);
        thread_call_enter_delayed(me->fWsaaKick, d);
    }
}

IOReturn NMXVGAFB::setDisplayMode(IODisplayModeID mode, IOIndex depth)
{
    if (depth != 0) return kIOReturnUnsupported;
    if (mode == fActID) return kIOReturnSuccess;
    if (mode != fAdvID) return kIOReturnUnsupported;
    return switchMode() ? kIOReturnSuccess : kIOReturnNoMemory;
}

// the scan-out takes the advertised size within the same buffer and blob (a new SET_SCANOUT_BLOB geometry); surface
// blobs of the old size go
bool NMXVGAFB::switchMode()
{
    if (!fVirtioOn || !fScanLock) return false;
    const uint32_t w = fAdvW, h = fAdvH, pitch = pitchFor(w);
    if ((uint64_t)pitch * h > fScanCap) {
        VFBLOG("mode %d: %ux%u needs %llu bytes, the scan-out buffer has %llu", (int)fAdvID, w, h,
               (unsigned long long)pitch * h, (unsigned long long)fScanCap);
        return false;
    }
    IOLockLock(fScanLock);
    bzero(fScanBuf->getBytesNoCopy(), (size_t)pitch * h);
    const bool ok = fVirtio.setScanoutBlob(fScanRes, w, h, pitch) && fVirtio.flush(fScanRes, 0, 0, w, h);
    if (ok) {
        for (auto &b : fSurf) dropSurfBlob(b);
        fCurRes = fScanRes; fCurW = w; fCurH = h; fCurFinal = false;
        fWidth = w; fHeight = h; fRowBytes = pitch;
        fActID = fAdvID;
    }
    IOLockUnlock(fScanLock);
    if (!ok) { VFBLOG("mode %d: the host refused the %ux%u scan-out", (int)fAdvID, w, h); return false; }
    setTiming();
    publishTiming();
    VFBLOG("mode %d: scan-out %ux%u pitch %u, %u.%03u Hz", (int)fActID, w, h, pitch, fRefreshMHz / 1000, fRefreshMHz % 1000);
    return true;
}

IOReturn NMXVGAFB::snapshot(void *dst, size_t cap, size_t *len)
{
    if (!fVirtioOn || !fScanLock) return kIOReturnNotReady;
    IOLockLock(fScanLock);
    IOMemoryDescriptor *md = fScanBuf;
    uint32_t w = fWidth, h = fHeight, pitch = fRowBytes;
    for (auto &b : fSurf) if (b.md && b.res == fCurRes) { md = b.md; w = b.w; h = b.h; pitch = b.pitch; }
    const size_t bytes = (size_t)pitch * h;
    IOReturn r = kIOReturnNoSpace;
    if (md && 16 + bytes <= cap) {
        const uint32_t hdr[4] = { 0x53584d4eu, w, h, pitch };
        memcpy(dst, hdr, sizeof hdr);
        r = md->readBytes(0, (uint8_t *)dst + 16, bytes) == bytes ? kIOReturnSuccess : kIOReturnIOError;
        *len = 16 + bytes;
    }
    IOLockUnlock(fScanLock);
    return r;
}

// back to the framebuffer's own buffer (NMXAccel is about to copy a frame into it)
IOReturn NMXVGAFB::scanoutHome()
{
    if (!fVirtioOn || !fScanLock) return kIOReturnNotReady;
    IOLockLock(fScanLock);
    IOReturn ret = kIOReturnSuccess;
    if (fCurRes != fScanRes) {
        if (fVirtio.setScanoutBlob(fScanRes, fWidth, fHeight, fRowBytes)) { fCurRes = fScanRes; fCurW = fWidth; fCurH = fHeight; }
        else ret = kIOReturnIOError;
    }
    IOLockUnlock(fScanLock);
    return ret;
}

/*
 * CoreDisplay sorts every framebuffer into a GPU vendor by its class name ("Intel", "AMD", "AppleParavirt", "NVDA" -
 * NullMoth's NVIDIA framebuffer is NVRMNVDAFramebuffer for this reason) and uses IOPresentment, the display-pipe path
 * NMXAccel serves, only when every framebuffer's vendor allows it; any other name counts as unknown, which never
 * does. WindowServer then took the legacy page-flip path, failed it ("Failed to set up surface for page flip mode")
 * and composited on the CPU. The VM display is the Intel GPU's head, so its class says so.
 */
class NMXIntelVGAFB : public NMXVGAFB {
    OSDeclareDefaultStructors(NMXIntelVGAFB)
};
OSDefineMetaClassAndStructors(NMXIntelVGAFB, NMXVGAFB)

/*
 * The firmware framebuffer driver (IONDRVFramebuffer) also takes a display-class device with no outputs, an SR-IOV VF;
 * as an unknown-vendor framebuffer it keeps IOPresentment off (see above), and a framebuffer cannot be stopped once
 * started (IOFramebuffer::stop panicked). This service holds the IOFramebuffer match category on such an Intel device
 * instead, at a higher score, as NullMoth's NVRMFBClaim does on an NVIDIA card. Only with the VM display opted in,
 * and never on the device that holds the firmware's framebuffer (a real Intel display stays with its own driver).
 */
class NMXVFFramebufferClaim : public IOService {
    OSDeclareDefaultStructors(NMXVFFramebufferClaim)
public:
    IOService *probe(IOService *provider, SInt32 *score) override {
        IOPCIDevice *pci = OSDynamicCast(IOPCIDevice, provider);
        if (!pci || !opted_in()) return nullptr;
        PE_Video v; bzero(&v, sizeof(v));
        IOPlatformExpert *pe = getPlatform();
        if (pe && pe->getConsoleInfo(&v) == kIOReturnSuccess && v.v_baseAddr) {
            uint64_t base = v.v_baseAddr & ~0xFFFul;
            for (UInt32 i = 0; i < pci->getDeviceMemoryCount(); i++) {
                IODeviceMemory *m = pci->getDeviceMemoryWithIndex(i);
                if (m && base >= m->getPhysicalAddress() && base < m->getPhysicalAddress() + m->getLength()) return nullptr;
            }
        }
        return IOService::probe(provider, score);
    }
    bool start(IOService *provider) override {
        if (!IOService::start(provider)) return false;
        VFBLOG("holding the IOFramebuffer category on %s (an Intel display device without the firmware framebuffer)",
               provider->getName());
        return true;
    }
};
OSDefineMetaClassAndStructors(NMXVFFramebufferClaim, IOService)
