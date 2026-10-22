# Report review: 20261020-120834

Collection 12:08:34 BST, same 11:55:41 boot as prior report. Current WindowServer PID163 launched11:55:44; live sample12:08:35 succeeded (exit0).

System Information reports Intel(R) Graphics (MTL), PCI7d45,4096MB shared, Metal3,1920x1080 online internal display.
Live 3-second sample: main thread271/275 samples waiting in mach messages,4 in display update/submission. No shader compilation stack observed. Some sampled presentation work uses nvmtlSurfaceOut -> nvmtl_vk_readback, including buffer mapping and CPU memmove. This establishes the copy path remains active, not that it dominates animation time. Idle sample cannot measure animation FPS or input latency.
Captured log contains20 WindowServer CoreAnimation Render timed-out fence messages,11:56:12–11:57:02; broader timeout strings include other subsystems. No GPU HANG/wedged/device lost strings found. Copied crash/panic filenames contain no2026-10-20 reports; copied historical files are not new crashes.
Touchpad: I2C3 controller active,D0VerifiedYes; ELAN device nub remains unregistered/unmatched; precision touchpad driver output empty. This boot predates the inline ACPI correction published after prior report, so does not test that correction.

No binaries/configuration changed during this review. Full hardware rendering suite not rerun. Next useful evidence: boot staged USB ACPI correction and verify ELAN attachment/gestures; steady-state animation performance needs an active-motion capture rather than interpreting idle waits as a fault.
