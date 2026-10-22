# Native CoreText runtime connection

## Latest update

The native AppKit glyph route now works through ordinary NSAttributedString
drawing, including San Francisco variable regular/bold fonts. Face matching
uses actual source font tables when native variable-instance PostScript names
differ from the base face; family-name guessing is not used.

Transparent CPU text-only bitmaps now retain immutable RGB glyph coverage
through CGBitmapContextCreateImage. CGContextDrawImage applies that coverage
over the final opaque destination, preserving destination clipping. Native
grayscale pixels remain valid for unknown consumers. Source clipping, untracked
pixel modifications, unsupported alpha/transforms and mixed-content sources
fall back. Deferred metadata is bounded to 64 MiB total / 8 MiB per snapshot;
image release reclaims ownership without relying on CGImageRelease alone.

Sanitized and release host tests pass both native CoreText and AppKit routes.
Intel binaries compile and sign; Dell execution remains pending.

Startup loading is required. Apple's modern libdyld exports the legacy
dyld_dynamic_interpose symbol but its implementation returns without patching.
The late-load activation experiment failed, and the API now explicitly returns
zero. No Metal-plugin late-loader or global injection is installed. GPU-backed
CoreAnimation surfaces and deployment into existing Apple apps remain open.

Primary source: https://github.com/apple-oss-distributions/dyld/blob/main/libdyld/libdyldGlue.cpp

The earlier implementation notes below describe the initial opaque route;
the update above supersedes the transparent-CPU-image limitation.

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
