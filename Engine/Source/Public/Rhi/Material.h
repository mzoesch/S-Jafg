// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "ResourceReference.h"
#include "Rhi/VkAl.h"

namespace Jafg
{

struct LFetchedMaterial final
{
    struct Layout
    {
        enum Type
        {
            Unique,
            Shared,
        };

        struct Set
        {
            LString Identifier;
            vk::ShaderStageFlagBits Stage;
            vk::DescriptorType DescriptorType;
        };

        Type Type;
        TOptional<LString> Identifier;
        TOptional<TArray<Set>> Sets;
    };

    LPath Path;
    LString Name;
    LString Shader;
    TArray<Layout> Layouts;
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
