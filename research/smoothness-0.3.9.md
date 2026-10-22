# Smoothness candidate0.3.9 — bounded staging leases

Latest evidence: failure-detail-20261020-123840, same boot as motion123544. Graphics reports Intel(R) Graphics(MTL),4096MBshared,Metal3. Plugin0.3.8 remained sluggish and logged43 CoreAnimation timeouts. The reduced log capture worked(1767 vs199637live lines), but this did not resolve presentation stalls.

Across72 sampled successful full-HD readbacks, weighted mean phase times were preparation/allocation24.126ms,record0.096ms,submit/wait3.888ms,CPUcopy3.712ms,cleanup2.055ms,total33.877ms. Preparation includes upload-barrier work, allocation/map and coherence checks; it is not an isolated allocation benchmark. Motion worker stacks independently show repeated staging mapping through IOKit. Main-thread scheduling/publication waits account for365/640 samples in the motion trace.

## Change

Plugin0.3.9 retains up to4 dedicated host-coherent readback staging allocations,16MiB requested payload per slot. A slot is exclusively leased until the successful GPU fence and CPU copy finish. Try-lock acquisition never waits for another lease; if all slots are busy, requests exceed16MiB, or NVMTL_READBACK_STAGE_FRESH is explicitly set, the previous fresh allocation path is used. No app/non-target staging policy changes.

Known never-submitted commands can return a slot idle. Uncertain submissions quarantine the slot and retain GPU memory; its buffer is never reused. Invalid maps/memory types discard known-idle storage. Growing a slot destroys its previous idle buffer before replacement. Normal retained requested payload is bounded to64MiB total plus driver allocation alignment/metadata. Transient fresh-fallback allocations remain possible; this is not a new global memory quota. Retained buffers are an intentional process-lifetime cache, not a per-frame leak. Process exit reclaims the process's resources.

Command pools are still fresh for every submission and retired using the existing completion rules. This is staging-memory reuse, not command-pool/command-buffer reuse. All host coherence checks, dependency barriers, layout restore, serial GPU submission and schedule-after-publication rules remain. No zero-copy or framebuffer kernel changes. See Vulkan's GPU-to-CPU synchronization example: https://docs.vulkan.org/guide/latest/synchronization_examples.html

The expected benefit is avoiding repeated full-HD buffer allocation/mapping and cleanup after warm-up. This is a hardware-unverified candidate; measured smoothness,CPU and timeout reduction are not claimed. First allocations,large buffers or contention can still take the old path. Shader startup cost and CPU copying remain separate work.

## Validation and trial

Three plugin variants compile; strict signature verifies; no external imports added or old exports missing. Actual lease helper passes8000 concurrent ASan/UBSan cycles and allocation/error/quarantine/fallback tests. Actual readback fixture performs1000 cached padded copies with one allocation, plus cached failure recovery/quarantine and the previous20000 forced-fresh allocation/reclaim checks. These are mocked CPU fixtures, not Dell GPU tests. Installer rollback,loader and predicates pass. Touchpad remains rolled back; EFI is preserved.

From the Dell fallback desktop:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit0, restart through USB once without ResetNVRAM. No extension-update alert is required for this plugin-only change. Let desktop settle and assess normal movement, then collect:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-motion.sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

Motion requests eight seconds of window dragging/Dock interaction after a five-second lead-in. Collect before rebooting to preserve current accelerated state. This is a short comparison of the changed display path, not the22-case rendering suite. If it fails, next USB boot returns fallback; collect retained failure logs before another install. Target installer retains prior plugin/loader/backend backups. No target install,arming or reboot has been performed from the build Mac.
