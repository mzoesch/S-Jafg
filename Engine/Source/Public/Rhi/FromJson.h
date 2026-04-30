// Copyright mzoesch. All rights reserved.

#pragma once

#include "Serialization/Json.h"
#include "Rhi/FromString.h"

namespace Jafg
{

namespace Detail
{

[[noreturn]]
inline void Vk_DefaultFromJsonFailHandler(LStringView DisplayName, LStringView Key, Json::EError Error)
{
    Json::DefaultFail(DisplayName, Key, Error);
}

} /* ~Namespace Jafg */

typedef void(*Vk_FromJsonFailHandler)(LStringView DisplayName, LStringView Key, Json::EError ExpectedValueType);

//#
//# A function that does not care if the key exits. Returning optional. But if it exists, it must be correct.
//# If the syntax is wrong, then the #Handler will be invoked.
//#
template<typename T>
inline std::optional<T> Vk_FromJson(LStringView DisplayName, LStringView Key, json Json, Vk_FromJsonFailHandler Handler = Detail::Vk_DefaultFromJsonFailHandler) = delete;

template<typename T>
inline T Vk_FromJsonValue(LStringView DisplayName, LStringView Key, json Json, Vk_FromJsonFailHandler Handler = Detail::Vk_DefaultFromJsonFailHandler)
{
    if (std::optional<T> Result{Vk_FromJson<T>(DisplayName, Key, Json, Handler)}; Result.has_value())
    {
        return *Result;
    }
    LOG_FATAL(LogSerialization, "[{}]: Deserialization did not result in a value.", DisplayName)
}

template<>
inline std::optional<vk::StencilOpState> Vk_FromJson(LStringView DisplayName, LStringView Key, json Json, Vk_FromJsonFailHandler Handler)
{
    LString MissingKey;
    Json::EError Error;

    if (Json.contains(Key) == false)
    {
        return {};
    }

    auto& Member = Json[Key];
    if (Member.is_object() == false)
    {
        Handler(DisplayName, Key, Json::EError::InvalidType);
        return {};
    }

    if (Json::DoesObjectContainTypeCheckedKeys(Member, {
        {"failOp", Json::LKeyType::String},
        {"passOp", Json::LKeyType::String},
        {"depthFailOp", Json::LKeyType::String},
        {"compareOp", Json::LKeyType::String},
        {"compareMask", Json::LKeyType::UInteger},
        {"writeMask", Json::LKeyType::UInteger},
        {"reference", Json::LKeyType::UInteger},
        }, &MissingKey, &Error) == false)
    {
        Handler(DisplayName, Key, Json::EError::InvalidType);
        return {};
    }

    return vk::StencilOpState{
        .failOp = Vk_FromString<vk::StencilOp>(Member["failOp"].get<LString>()),
        .passOp = Vk_FromString<vk::StencilOp>(Member["passOp"].get<LString>()),
        .depthFailOp = Vk_FromString<vk::StencilOp>(Member["depthFailOp"].get<LString>()),
        .compareOp = Vk_FromString<vk::CompareOp>(Member["compareOp"].get<LString>()),
        .compareMask = Member["compareMask"].get<uint32_t>(),
        .writeMask = Member["writeMask"].get<uint32_t>(),
        .reference = Member["reference"].get<uint32_t>(),
        };
}

template<>
inline std::optional<vk::PipelineInputAssemblyStateCreateInfo> Vk_FromJson(LStringView DisplayName, LStringView Key, json Json, Vk_FromJsonFailHandler Handler)
{
    LString MissingKey;
    Json::EError Error;

    if (Json.contains(Key) == false)
    {
        return {};
    }

    auto& Member = Json[Key];
    if (Member.is_object() == false)
    {
        Handler(DisplayName, Key, Json::EError::InvalidType);
        return {};
    }

    if (Json::DoesObjectContainTypeCheckedKeys(Member, {
        {"topology", Json::LKeyType::String},
        {"primitiveRestartEnable", Json::LKeyType::Bool},
        }, &MissingKey, &Error) == false)
    {
        Handler(DisplayName, Key, Json::EError::InvalidType);
        return {};
    }

    return vk::PipelineInputAssemblyStateCreateInfo{
        .topology = Vk_FromString<vk::PrimitiveTopology>(Member["topology"].get<LString>()),
        .primitiveRestartEnable = Member["primitiveRestartEnable"].get<bool>(),
        };
}

template<>
inline std::optional<vk::PipelineDepthStencilStateCreateInfo> Vk_FromJson(LStringView DisplayName, LStringView Key, json Json, Vk_FromJsonFailHandler Handler)
{
    LString MissingKey;
    Json::EError Error;

    if (Json.contains(Key) == false)
    {
        return {};
    }

    auto& Member = Json[Key];
    if (Member.is_object() == false)
    {
        Handler(DisplayName, Key, Json::EError::InvalidType);
        return {};
    }

    if (Json::DoesObjectContainTypeCheckedKeys(Member, {
        {"depthTestEnable", Json::LKeyType::Bool},
        {"depthWriteEnable", Json::LKeyType::Bool},
        {"depthCompareOp", Json::LKeyType::String},
        {"depthBoundsTestEnable", Json::LKeyType::Bool},
        {"stencilTestEnable", Json::LKeyType::Bool},
        {"front", Json::LKeyType::Object},
        {"back", Json::LKeyType::Object},
        {"minDepthBounds", Json::LKeyType::Float},
        {"maxDepthBounds", Json::LKeyType::Float},
        }, &MissingKey, &Error) == false)
    {
        Handler(DisplayName, Key, Json::EError::InvalidType);
        return {};
    }

    return vk::PipelineDepthStencilStateCreateInfo{
        .depthTestEnable = Member["depthTestEnable"].get<bool>(),
        .depthWriteEnable = Member["depthWriteEnable"].get<bool>(),
        .depthCompareOp = Vk_FromString<vk::CompareOp>(Member["depthCompareOp"].get<LString>()),
        .stencilTestEnable = Member["stencilTestEnable"].get<bool>(),
        .front = Vk_FromJsonValue<vk::StencilOpState>(DisplayName, "front", Member, Handler),
        .back = Vk_FromJsonValue<vk::StencilOpState>(DisplayName, "back", Member, Handler),
        .minDepthBounds = Member["minDepthBounds"].get<float>(),
        .maxDepthBounds = Member["maxDepthBounds"].get<float>(),
        };
}

} /* ~Namespace jafg */
