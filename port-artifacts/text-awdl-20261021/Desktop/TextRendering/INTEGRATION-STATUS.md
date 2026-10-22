# FreeType integration — implementation and remaining work

The goal remains system-wide LCD text integration, not a standalone demo or a
claim that an atlas library already changes macOS apps. Current implementation:

- Private FreeType 2.14.3 for Intel target and ARM reference testing, with filtered
  LCD rendering enabled. Source archive SHA and configuration are recorded.
- Four physical stripe layouts: RGB, RGB_V, BGR and BGR_V. Rotating by 180/270
  reverses channel order as well as changing the sampling axis. Native panel
  stripe geometry still needs visual/hardware confirmation; EDID is insufficient.
- Bounded 4 MiB coverage atlas, font/glyph/size/phase/layout keys, eviction and
  orientation-generation invalidation. Coverage channels are linear, not sRGB.
- Per-channel blending in linear light over a known opaque background. A Metal
  fragment source specifies separate coverage/background textures and an sRGB
  output attachment. Target GPU compilation/runtime remain untested.
- CoreText-shaped-run frontend: keeps native CoreText glyph positioning and font
  fallback instead of replacing shaping with character-to-glyph guesses. Resolves
  the exact font collection face and variation axes. Unsupported/color fonts,
  transformed text, context-derived colors, transparent destinations and invalid
  input return unsupported without partially drawing a run.
- Real CoreText line input into FreeType rendering passed sanitizer tests in all
  four layouts. Intel libraries compile/sign successfully. These host tests do
  not establish rendering through the Dell GPU or integration into existing apps.

The critical remaining integration is above the current Metal driver. Native
CoreText/CoreGraphics, WebKit and other renderers must deliver glyph data and
component-alpha text layers to the compositor. Existing single-channel R8/A8
atlases have already discarded the information FreeType needs; arbitrary texture
imports in NVMTLDevice cannot recover glyph outlines, font choice or RGB coverage.
Standard premultiplied RGBA layers also cannot encode three independent coverage
values as one alpha channel. Transparent text therefore needs a component-alpha
layer/compositor contract or deferred text rendering over the resolved background.

Startup-loaded native CoreText and AppKit glyph interception now pass runtime
tests, including native San Francisco variable fonts. Transparent CPU text-image
coverage survives CGImage creation and late opaque composition. System-app
deployment, GPU-backed CoreAnimation transport and compositor dispatch remain
unimplemented. The exported legacy dyld late-interpose function is a no-op in
modern libdyld; the attempted late loader is explicitly disabled.
The working NVMTL driver has not been modified to misidentify R8 textures as text
or to apply a screen-space sharpening filter and call that subpixel rendering.
No existing app is advertised as switched to LCD text yet. This requested work
remains explicitly open in UNIFIED-CANDIDATE-WORK.md.

FreeType license notices accompany the backend. System fonts are read locally;
no Apple fonts are redistributed. The component uses FreeType and is based in
part on the work of the FreeType Team.

Primary reference: https://freetype.org/freetype2/docs/reference/ft2-lcd_rendering.html
CoreText API reference: installed Apple SDK CTFont.h and CTRun.h.
