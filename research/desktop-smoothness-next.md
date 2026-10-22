# Desktop smoothness follow-up

Evidence: failure-detail-20261020-120834, collected on the same 11:55:41 accelerated boot as the prior report. Live sample ~13 minutes after boot was mostly idle; no shader compiler frames observed. It cannot establish interaction FPS, sustained CPU cost, or prove the absence of stalls during motion.

Source audit: NVMTLTexture nvmtlSurfaceOut copies non-host-backed images into IOSurface pages after synchronous Vulkan image readback. Latitude WindowServer forces owned readback staging allocations, and fresh command pools remain mandatory. The sample includes allocation/map and CPU-copy work in this path, but too few active samples to establish dominance. Do not remove waits, reuse command pools, or change buffer lifetime on this evidence. commitAndWaitUntilSubmitted already uses waitUntilScheduled when the framebuffer advertises ordered flips; removing completion ordering is not an established fix.

Next measurement: collect-desktop-motion.sh performs an 8-second stack sample during user window dragging/Dock interaction, with 35-second watchdog and concurrent ps context. It makes no NVRAM/driver changes and runs no rendering suite. It requires the accelerated boot to characterize that path. The latest touchpad trial can independently run on fallback. ps percentages are lifetime averages, not instantaneous load; this tool does not measure FPS. Capture keeps normal desktop content out of screenshots by collecting stacks only.

Prepared locally because USB was disconnected before publication. No graphics binaries changed and no smoothness improvement is claimed yet.
