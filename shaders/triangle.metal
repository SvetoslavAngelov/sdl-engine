#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[attribute(0)]];
    float4 colour [[attribute(1)]];
};

struct VertexOutput {
    float4 position [[position]];
    float4 colour;
};

vertex VertexOutput vertex_main(VertexInput input [[stage_in]]) {
    return {float4(input.position, 1.0), input.colour};
}

fragment float4 fragment_main(VertexOutput input [[stage_in]]) {
    return input.colour;
}
