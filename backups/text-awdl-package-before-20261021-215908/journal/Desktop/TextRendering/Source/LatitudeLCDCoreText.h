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
// Immutable deferred glyph layer. It retains independent RGB coverage, owns
// font rasterization output and can outlive the original CTLine/font objects.
// No pixel drawing occurs until Composite is given the resolved background.
typedef struct LatitudeLCDTextLayer LatitudeLCDTextLayer;
LatitudeLCDTextLayer* LatitudeLCDCreateTextLayer(CTLineRef line,double baselineX,double baselineY,unsigned clockwiseQuarterTurns);
void LatitudeLCDDestroyTextLayer(LatitudeLCDTextLayer*layer);
// Returns 0 without writing for transparent destinations, short buffers, or
// stale stripe orientation. Destroy/recreate a layer after display rotation.
int LatitudeLCDTextLayerBounds(const LatitudeLCDTextLayer*layer,int*x,int*y,unsigned*width,unsigned*height);
int LatitudeLCDCompositeTextLayerAt(const LatitudeLCDTextLayer*layer,uint8_t*rgba,size_t byteLength,unsigned width,unsigned height,size_t rowBytes,unsigned clockwiseQuarterTurns,int originX,int originY);
int LatitudeLCDCompositeTextLayer(const LatitudeLCDTextLayer*layer,uint8_t*rgba,size_t byteLength,unsigned width,unsigned height,size_t rowBytes,unsigned clockwiseQuarterTurns);
int LatitudeLCDDrawCTLine(CTLineRef line,uint8_t*rgba,unsigned width,unsigned height,size_t rowBytes,double baselineX,double baselineY,unsigned clockwiseQuarterTurns);
#ifdef __cplusplus
}
#endif
