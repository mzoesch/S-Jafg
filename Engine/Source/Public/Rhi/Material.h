// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/FetchedShader.h"
#include "Rhi/ResourceReference.h"
#include "Rhi/GraphicsPipeline.h"

namespace Jafg
{

struct LFetchedMaterial final
{
    enum Type
    {
        Solid,
    };

    struct Property
    {
        LString Key;
        LString Value;
    };

    LPath Path;
    LString Name;
    LFetchedShader const& FetchedShader;
    Type Type{Solid};
    TArray<Property> Properties;
};

struct LMaterial final
{
    LFetchedMaterial const& FetchedMaterial;
    LGraphicsDevicePipeline Pipeline;
};

typedef TSharedRef<LMaterial> LMaterialRef;

struct LMaterialInstance final
{
    LMaterialRef Material;

    TArray<std::pair<u32, vk::raii::DescriptorSet>> InfrequentDescriptorSets;
    std::array<TArray<std::pair<u32, vk::raii::DescriptorSet>>, Vk_DesiredMaxFramesInFlight> FrequentDescriptorSets;
};

typedef TSharedRef<LMaterialInstance> LMaterialInstanceRef;

} /* ~Namespace Jafg */
