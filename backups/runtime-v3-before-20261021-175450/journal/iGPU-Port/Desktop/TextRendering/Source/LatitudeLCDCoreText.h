#pragma once
#include <CoreText/CoreText.h>
#include <stdint.h>
// Positioned glyph runs retain CoreText shaping/fallback. This is the frontend
// contract to the LCD rasterizer; the platform compositor must supply the opaque
// destination and call this before discarding glyph metadata into an R8 atlas.
// Returns 1 drawn, 0 unsupported (destination unchanged). RGBA, sRGB, alpha=255.
#ifdef __cplusplus
extern "C" {
#endif
int LatitudeLCDDrawCTLine(CTLineRef line,uint8_t*rgba,unsigned width,unsigned height,size_t rowBytes,double baselineX,double baselineY,unsigned clockwiseQuarterTurns);
#ifdef __cplusplus
}
#endif
