// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

struct LRenderInfo;
struct LGraphicsDevicePipeline;
struct LActorRenderInfo;
struct LActorDrawInfo;

//# Concept for push constant structures. It must fulfill these requirements.
template<typename T>
concept CPushConstant = std::is_standard_layout_v<T> && requires
{
    { T::Flags() } -> std::same_as<vk::ShaderStageFlags>;
};

//# Register a push constant to be used in the data driven shader system.
template<typename T> requires CPushConstant<T>
struct LRegisterPushConstant;

namespace PC
{

//#
//# A generic push constant structure.
//# Inherit from this to create push constant structures for specific shader stages.
//#
template<typename T, vk::ShaderStageFlagBits... InFlags>
struct TPushConstant
{
    template<typename TRenderInfo, typename TGraphicsDevicePipeline> requires
        (std::is_base_of_v<LRenderInfo, TRenderInfo> && std::is_base_of_v<LGraphicsDevicePipeline, TGraphicsDevicePipeline>)
    inline void Push(TRenderInfo const& Info, TGraphicsDevicePipeline const& Pipeline, u32 Offset = 0) noexcept
    {
        Info.CommandBuffer.pushConstants2({
            .layout = *Pipeline.Layout,
            .stageFlags = T::Flags(),
            .offset = Offset,
            .size = sizeof(T),
            .pValues = this
            });

        return;
    }

    static constexpr vk::ShaderStageFlags Flags() noexcept { return (InFlags | ...); }
};

} /* ~Namespace PC */

namespace Detail
{

struct LPushConstantInfo
{
    vk::ShaderStageFlags StageFlags;
    u32 Size;
};
typedef LPushConstantInfo(*LPushConstantProviderSig)();
typedef void(*LPushConstantActorAutoPush)(LActorRenderInfo const&, LGraphicsDevicePipeline const&, LActorDrawInfo const&);
struct LPushConstantSigs
{
    LPushConstantProviderSig Provider;
    LPushConstantActorAutoPush ActorAutoPush;
};

template<typename T> requires CPushConstant<T>
struct TPushConstantProviderFn
{
    static LPushConstantInfo operator()() noexcept
    {
        return LPushConstantInfo{
            .StageFlags = T::Flags(),
            .Size = sizeof(T)
            };
    }
};
template<typename T> requires CPushConstant<T>
inline constexpr TPushConstantProviderFn<T> PushConstantProvider{};
extern void AddPushConstantProviderImpl(LString Name, LPushConstantSigs const& Sigs) noexcept;
template<typename TPushConstant> requires CPushConstant<TPushConstant>
inline void AddPushConstantProvider() noexcept
{
    if constexpr (requires {TPushConstant::AutoActorPush;})
    {
        AddPushConstantProviderImpl(LString{GetTypeName<TPushConstant>()}, LPushConstantSigs{
            .Provider = &PushConstantProvider<TPushConstant>.operator(),
            .ActorAutoPush = &TPushConstant::AutoActorPush
            });
    }
    else
    {
        AddPushConstantProviderImpl(LString{GetTypeName<TPushConstant>()}, LPushConstantSigs{
            .Provider = &PushConstantProvider<TPushConstant>.operator()
            });
    }

    return;
}

} /* ~Namespace Detail */

template<typename T> requires CPushConstant<T>
struct LRegisterPushConstant final
{
    inline LRegisterPushConstant() noexcept
    {
        Detail::AddPushConstantProvider<T>();
    }
};

} /* ~Namespace Jafg */
