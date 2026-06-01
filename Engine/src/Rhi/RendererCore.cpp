// Copyright mzoesch. All rights reserved.

#include "Rhi/RendererCore.h"

void vk::from_json(json const& j, PipelineInputAssemblyStateCreateInfo& Info)
{
    typedef PipelineInputAssemblyStateCreateInfo T;

    Info.topology = j.at("topology").get<PrimitiveTopology>();
    Info.primitiveRestartEnable = j.at("primitiveRestartEnable").get<bool>();

    return;
}

void vk::from_json(json const& j, PipelineDepthStencilStateCreateInfo& Info)
{
    typedef PipelineInputAssemblyStateCreateInfo T;

    Info.depthTestEnable = j.at("depthTestEnable").get<bool>();
    Info.depthWriteEnable = j.at("depthWriteEnable").get<bool>();
    Info.depthCompareOp = j.at("depthCompareOp").get<CompareOp>();
    Info.depthBoundsTestEnable = j.at("depthBoundsTestEnable").get<bool>();
    Info.stencilTestEnable = j.at("stencilTestEnable").get<bool>();
    Info.front = j.at("front").get<StencilOpState>();
    Info.back = j.at("back").get<StencilOpState>();
    Info.minDepthBounds = j.at("minDepthBounds").get<float>();
    Info.maxDepthBounds = j.at("maxDepthBounds").get<float>();

    return;
}

void vk::from_json(json const& j, StencilOpState& State)
{

    typedef StencilOpState T;
    State.failOp = j.at("failOp").get<StencilOp>();
    State.passOp = j.at("passOp").get<StencilOp>();
    State.depthFailOp = j.at("depthFailOp").get<StencilOp>();
    State.compareOp = j.at("compareOp").get<CompareOp>();
    State.compareMask = j.at("compareMask").get<uint32_t>();
    State.writeMask = j.at("writeMask").get<uint32_t>();
    State.reference = j.at("reference").get<uint32_t>();

     return;
}
