# Frame comparison optimization — isolated follow-up

Read-only sampling of the running nested firmware boot placed 17/64 outer PCs in
the GOP frame comparison. This is a sampled hotspot, not a Dell timing result.

The scalar cacheline comparison passed 232,705 ASan/UBSan oracle/boundary cases,
including all byte alignments, middle/final mismatches and protected-page tails.
The actual x86 UEFI presenter passed translated ARM writes and real GOP pixel
readback, unchanged-frame suppression, last-pixel changes, black reversion and
an injected failure followed by correct retry. It has no new SIMD-state dependency.

In a separate 512MiB OVMF microbenchmark, 32 comparisons of 1MiB took 11.97/11.91ms
for the scalar reference and 6.60/6.49ms for the unrolled code (1.81/1.84x). These
are isolated aggregate comparison timings, not frame times or a full-boot speedup.
The SSE2 alternative passed correctness but was slower (16.15/16.14ms); its
negative result is preserved and that alternative is not selected.

The next full-engine candidate compiles and its production presenter object is
byte-identical to the one exercised by the firmware fixture. Its EFI hash is
5ec8c671db4eb306f089c855b984e62dc11bf78357badb1597e4ae26d437b652.
It has not run the full admission/kernel test and is not the active boot. The
security-startup-v12 comparison was observed and stopped without including this
optimization; the following v13 display-bridge candidate also keeps the previous
presenter, so the presentation experiment stays isolated. No graphical installer is claimed.
