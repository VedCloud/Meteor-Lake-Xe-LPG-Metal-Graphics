# Native CoreText runtime connection

`libLatitudeLCDNative-x86_64.dylib` now intercepts ordinary CTLineDraw calls in
an explicitly launched process. A caller that knows nothing about the custom
renderer passes the host runtime test: LCD coverage, coordinate placement,
clipping, native fallback, graphics-state restoration and 200 context-release
callbacks. Both sanitized and release ARM tests passed; Intel code compiled and
was signed. Actual Intel execution is still pending.

Supported native route: tracked public CGBitmapContext creation, opaque 8-bit
RGBA sRGB destination, identity device transform, unscaled text, fill mode,
normal blend and alpha 1. The renderer copies the glyph bounds only, retains
three-channel coverage until the opaque background is available, and draws
through Quartz to preserve arbitrary clipping. Unsupported cases use native
CoreText without first changing the destination. Context release uses the
public release callback, including CFRelease lifetimes; no context is retained
solely for tracking. Tracking is bounded to 2048 contexts.

This is a real native-call connection, **not complete system-app integration**.
It does not intercept all CG glyph routes, GPU-backed contexts or transparent
CoreAnimation text surfaces. Unknown private graphics-state changes are outside
the validated contract. Only horizontal RGB is enabled in this adapter until
live display-orientation coordination is validated. The underlying renderer
already tests RGB/RGB_V/BGR/BGR_V independently.

No global DYLD environment, framework replacement or Apple-app injection has
been installed. The included validate-native-text.sh launches only our ordinary
CoreText test executable with this library. Existing system apps remain on their
normal text path. Their transparent-layer/compositor connection is still work
to implement; an R8 mask cannot recover three independent coverage channels.
