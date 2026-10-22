# Candidate0.3.10 — retained staging CPU cache visibility

0.3.9 target result125537 confirmed substantially faster recorded readbacks(33.877ms before,4.318ms now), and5 CoreAnimation timeout messages instead of43 in the respective startup streams. This is not an FPS measurement or controlled benchmark. User reports/photos also show new coloured cursor trails,partial background repaint,black menu bar instead of translucency,text artifacts and window-movement corruption. The candidate is not validated as visually correct.

## Targeted workaround

This candidate supplements the retained-staging path with explicit x86 CPU cache maintenance: flush/invalidate cache lines and MFENCE before a retained buffer is overwritten by the GPU, and again after the successful GPU fence before copying its bytes into an IOSurface. Pre-flushing prevents dirty CPU aliases being written back over later device data; post-flushing avoids reusing old CPU cache lines. Actual architecture support and cache-line size are queried through CPUID. If unsupported, retention is disabled and the prior fresh allocation path remains.

This is a hypothesis-driven workaround for possible stale retained IOKit mappings, not proof of a diagnosed kernel cache-coherency defect. Under the Vulkan HOST_COHERENT contract extra invalidation should not be necessary; the local ANV implementation skips vkInvalidateMappedMemoryRanges for coherent types, so simply adding that Vulkan call would be a no-op. Image content preservation,damage tracking and physical scanout remain alternative explanations if artifacts persist.

Flush addresses stay within the requested allocation range,including unaligned first/last bytes; zero,overflow,unsupported geometry and capacity violations are rejected. The Vulkan transfer-to-host barrier and successful fence are still mandatory. On timeout there is no CPU cache read/copy or premature release; quarantine remains. A bounded cache of4exclusive16MiB-payload slots remains; fresh command pools are unchanged. No kernel/framebuffer/backend/EFI/touchpad changes. CPU cache maintenance adds work and may reduce some of0.3.9's speed benefit; target timing is needed.

## Local checks

Three plugin variants compile; strict signature verifies; no new external imports or missing prior exports. x86_64 ASan/UBSan tests execute actual CLFLUSH/MFENCE under Rosetta and cover guard pages,unaligned ranges,overflow,capacity and geometry. They verify byte preservation and code order,not Dell GPU visibility. Actual readback fixtures assert pre-submit/post-fence ordering,1000cached padded copies with one allocation,timeout retention/no destination writes,invalid memory type discard,known unsubmitted failure recovery,and20000forced-fresh cleanup checks. Loader/installer guards and rollback checks pass. No Dell hardware rendering suite rerun.

Sources: [Vulkan coherent memory contract](https://docs.vulkan.org/spec/latest/chapters/memory.html),[GPU-to-CPU synchronization](https://docs.vulkan.org/guide/latest/synchronization_examples.html),[Intel instruction reference](https://cdrdv2-public.intel.com/812383/253666-sdm-vol-2a.pdf). Local ANV anv_device.c InvalidateMappedMemoryRanges reviewed; no Mesa changes in this candidate.

## Dell trial

From fallback:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit0,restart through USB once without ResetNVRAM. No extension-update alert is required for the plugin/loader-only update. Check background redraw at rest,cursor trails,menu transparency,text and window dragging,then collect before reboot:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

The collector should show candidate0.3.10/cache-visibility first-submit records and present-perf0.3.10 timing. No repeated22-case suite or motion script is required for this correctness trial. If still corrupt,report which symptoms remain; do not treat passing readbacks or lower timeout counts as proof of correct pixels. The target installer preserves prior plugin/loader backups and one-shot fallback recovery. No target install,arming or reboot from the build Mac. All observed artifact fixes remain hardware-unverified.
