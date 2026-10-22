# Startup result123840

Same12:33:28 boot as motion report123544; plugin0.3.8 active, actual Intel(R) Graphics(MTL)/4096MB/Metal3 identity retained.

Filtered live log1767lines/233462bytes vs previous199637lines/24546446bytes. Different boots/interactions, not a controlled CPU benchmark. Still43 CoreAnimation Render timeout lines vs previous47; this does not establish a meaningful reduction or resolution. No GPU HANG/wedged/device-lost strings in captured live log.

Eight successful-readback timing windows total72readbacks. Weighted mean milliseconds: preparation/allocation24.126,recording0.096,submit/wait3.888,CPUcopy3.712,cleanup2.055,total33.877. Preparation range22.312–29.494ms. This phase includes upload barrier plus staging allocation/map and memory-coherence validation, so is not a pure allocation benchmark. Motion stacks separately show repeated staging mapping through IOKit. Reported maxima reach46.123ms. A60Hz frame period is16.67ms, but these samples do not measure achieved FPS or account for all composition passes.

Prior fresh command-pool workaround remains necessary. Dedicated host-coherent staging buffer lifetime is distinct from command-pool lifetime. A bounded staging retention candidate may remove repeated allocation/map cost if leases remain exclusive until both successful fence completion and CPU copy, and uncertain submissions quarantine buffers. Do not reuse command pools or relax ordering to obtain this improvement.
