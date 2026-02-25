// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "ResourceReference.h"
#include "Rhi/VkAl.h"

namespace Jafg
{

struct LFetchedMaterial final
{
    struct Property
    {
        LString Key;
        LString Value;
    };

    LPath Path;
    LString Name;
    LString Shader;
    TArray<Property> Properties;
};

struct LMaterial final
{
    LString FetchedMaterial;
    LGraphicsDevicePipeline Pipeline;
};

typedef TSharedRef<LMaterial> LMaterialRef;

struct LMaterialInstance final
{
    LMaterialRef Material;
    TArray<vk::raii::DescriptorSet> _UniqueDescriptorSets;
};

typedef TSharedRef<LMaterialInstance> LMaterialInstanceRef;

} /* ~Namespace Jafg */
