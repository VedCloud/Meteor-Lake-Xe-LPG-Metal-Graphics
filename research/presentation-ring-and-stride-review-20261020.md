# Presentation ring and texture-stride review — 2026-10-20

Scope: private source/log review and primary documentation research. No driver, EFI, Mesa, installer or collector changes in this review. USB candidate remains 0.3.18; hardware results for that candidate are pending.

## Evidence and limits

The historical 0.3.14 report had 564.350 ms publication and 11 CoreAnimation timeout messages. The later 0.3.17 report at Desktop/reports/failure-detail-20261020-190817 had four current-boot timeout messages, a 66.434 ms publication outlier, and a 339.217 ms submission/wait outlier. Different observation windows prevent a controlled numerical comparison.

Eight early publication samples in the later report measured IOSurface lock acquisition at 0.013–0.026 ms and the per-surface monitor at 0.001–0.003 ms. They do not capture every later stall. Consequently, neither a dominant scanout lock nor its absence throughout the run is established. The submission/wait phase includes buffer import, CPU serialization and GPU submission/wait; it is not GPU execution timing. The approximately 5 ms readback is not end-to-end frame latency.

## Triple buffering

Apple's triple-buffering guidance concerns overlapping producer/consumer work while retaining ownership until completion. It explicitly discusses the memory and latency costs of excess buffering. It does not establish that an arbitrary private WindowServer bridge can replace destination surfaces or eliminate all fence timeouts.

Current nvmtl_surface_publication.h has three exclusive CPU staging slots, each capped at 16 MiB (48 MiB combined). This reduces time spent preparing pixels while holding IOSurfaceLock. It is not an asynchronous IOSurface presentation ring. The existing completion path still publishes into the destination surface before marking completion.

A safe asynchronous design needs a separate lifetime contract for each submitted destination surface and generation: retain destination and source resources; fence producer completion; preserve dependencies and per-surface ordering; publish only to the correct destination; signal completion only after its pixels are visible; retire resources only after consumers finish. Queue saturation must apply bounded backpressure rather than overwrite an in-flight slot. Resize, destruction, errors and shutdown must drain or cancel without early success signals. Dropping arbitrary layer transactions is unsafe even when dropping superseded final display frames might be valid.

Next investigation: separate import, submission mutex wait, queue submission, fence wait, staging acquisition/readback, destination monitor wait, IOSurface lock, pixel copy and unlock in a slow transaction. Correlate those phases to transaction/surface identity before selecting an asynchronous scheduling change. Candidate 0.3.18 already fixes the release logger's suppression of later bounded publication records; inspect its target report before changing the scheduling contract.

## Weather text and pitch

Recorded BGRA samples include width 44 at pitch 192 (64-byte aligned), width 1876 at pitch 7552 (128-byte aligned), and width 1920 at pitch 7680. These are WindowServer samples, not proof of Weather's font-atlas format or layout. A grayscale atlas and a pitch mismatch remain hypotheses.

nvmtl_vk_image_write validates the supplied row size, copies each padded source row into a tightly packed upload buffer, then uses VkBufferImageCopy with zero bufferRowLength/bufferImageHeight, which specifies tight packing. That combination is consistent with the Vulkan copy contract. The linear buffer-texture alias path validates the requested pitch and rejects a queried Vulkan layout that differs. Mesa ANV obtains image layout through Intel surface-layout calculations; it is not a dedicated macOS font-atlas allocator.

For uncompressed images, a padded Vulkan buffer copy can instead use bufferRowLength in texels, with matching allocation size and addressing. It must not pass byte stride as texel count. Compressed formats require block-aware calculations. Ordinary optimal-tiled images cannot be treated as a linear array of padded rows; host-access layout queries apply to supported linear/modifier images. Changing an advertised pitch without repacking both sides would itself introduce corruption.

Next investigation: identify the actual failing Weather texture's format, dimensions, plane, mip/slice, IOSurface row size, upload/copy extent and Vulkan layout; compare uploaded bytes or a controlled capture to rendered output. Check sampling/swizzle, alpha and texture-view behavior alongside pitch. Apply alignment changes only if that evidence identifies a mismatch. Existing padded BGRA samples cannot justify a global 64/128-byte allocator override.

## Primary references

- Apple, Metal Best Practices: Triple Buffering: https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/TripleBuffering.html
- Khronos, VkBufferImageCopy (buffer row length in texels; zero means tightly packed): https://docs.vulkan.org/refpages/latest/refpages/source/VkBufferImageCopy.html
- Khronos, vkGetImageSubresourceLayout (supported tilings and host-access layout): https://docs.vulkan.org/refpages/latest/refpages/source/vkGetImageSubresourceLayout.html

Decision: keep both proposals as investigated hypotheses, not guaranteed fixes. Preserve the pending 0.3.18 candidate and successful rendering/audio/brightness baseline. Do not repeat the full rendering checklist for this research review.
