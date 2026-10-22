# Dell report 20261020-172853

Loaded framebuffer0.2.7 UUID7B74B859-666F-3D61-AA7A-D6636F14CA87, accelerator0.2.5 UUID8096FC0D-5CBB-398A-A0C9-935AB3CCAF7E, GPU1.0.15, Metal0.3.12. Captured physical panel EDID is present in system_profiler (BOE09e5/product0bae). Brightness handler is active, parameters published, live period192000, startup duty153600, last duty192000/level65535. User confirms Control Centre brightness and About This Mac icon; Weather text still corrupted and movement slightly laggy.

67 early readbacks across 8 aggregates average 5.004537 ms (alloc/record/wait/copy/cleanup=[0.28185074626865675, 0.04886567164179106, 3.210731343283582, 1.4630895522388059, 0.0]). This measures readback stages, not whole frames, and is not a controlled comparison to previous startup reports.

11 CoreAnimation timed-out-fence messages appear in retained live log, versus four in the preceding retained log. Capture duration/workload differ; this does not alone quantify a regression rate. Slow-part publication reaches564.350ms and submission/wait reaches624.475ms. Submission/wait includes CPU queue preparation and cleanup, not isolated GPU execution; publication includes downstream readback/work, not only memcpy. Main WindowServer thread waits in commitAndWaitUntilSubmitted -> waitUntilScheduled in122/275 sample ticks (~44.4%). This is sampled blocked wall time, not CPU percentage. Weather main thread is mostly event-loop idle; no glyph-specific root cause is established.

Between17:29:03 and17:29:11, retry-copy counter increases481 and cumulative retry-copy wall time486.289866ms, averaging1.011ms/copy. Skipped timer counter remains29. Lifetime maximum80.421484ms is not a measured frame duration. WindowServer accumulated process CPU time rises7.10s in approximately8s; sustained CPU cost remains material. These observations do not isolate why a particular Weather glyph is wrong.

Frame observer skipped without requesting Screen Recording permission; total_render_latency_ms remainsnull. No actual frame pacing/FPS can be inferred from timer-copy counters. Full log GPU HANG/wedged/device-lost signatures found: 0. Collector copies historical crashes/panics from48hours: counts alone cannot be called new failures.

Next engineering focus: inspect submission scheduling and IOSurface publication/readback contention; isolate glyph texture sampling/format/blend path with targeted evidence. No fences/cache visibility are removed and no new candidate or full hardware test is run for this log review.
