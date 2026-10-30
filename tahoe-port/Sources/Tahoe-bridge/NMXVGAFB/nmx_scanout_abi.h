// NMXAccel -> NMXVGAFB: scan a presented surface out of its own memory (virtio-gpu blob), or go back to the
// framebuffer's own buffer. Called with callPlatformFunction on the framebuffer.
#pragma once

#define NMX_SCANOUT_ABI_VERSION 2u
#define NMX_SCANOUT_FN_SURFACE  "nmxScanoutSurface"   // p1 = struct NMXScanoutSurface *
#define NMX_SCANOUT_FN_HOME     "nmxScanoutHome"      // no arguments

struct NMXScanoutSurface {
    unsigned int version;
    unsigned int surfaceId;
    void        *md;            // the surface's IOMemoryDescriptor (NMXVGAFB retains and prepares it while it is cached)
    unsigned int width, height, pitch;
    unsigned int format;        // 'BGRA' (0x42475241); only that is accepted
    unsigned long long ns;      // out: how long the switch took
    unsigned int flags;         // version 2: NMX_SCANOUT_*
};
// the surface's pixels are final (the accelerator ordered the flip after them): one flush now, none on later VBLs -
// a re-flush would show whatever a later frame writes into the surface while it is still on the panel
#define NMX_SCANOUT_FINAL 0x1u
