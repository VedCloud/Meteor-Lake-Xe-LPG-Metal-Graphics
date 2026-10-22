#include <metal_stdlib>
using namespace metal;
struct LCDQuad {float4 position [[position]];float2 atlasUV;float2 backgroundUV;};
// Coverage atlas MUST be linear rgba8Unorm, not sRGB. Background is a distinct
// opaque, resolved texture: no reading/writing the same render target.
// Fonts/positions/coverage are supplied by the text-layer producer, not inferred
// from arbitrary Metal R8 textures. Output attachment uses sRGB encoding.
fragment float4 latitudeLCDFragment(LCDQuad in [[stage_in]],
 texture2d<float> coverage [[texture(0)]],texture2d<float> opaqueBackground [[texture(1)]],
 constant float4&linearForeground [[buffer(0)]]) {
 constexpr sampler nearest(coord::normalized,address::clamp_to_zero,filter::nearest);
 float3 a=coverage.sample(nearest,in.atlasUV).rgb;
 float4 bg=opaqueBackground.sample(nearest,in.backgroundUV);
 return float4(mix(bg.rgb,linearForeground.rgb,a),1.0);
}
