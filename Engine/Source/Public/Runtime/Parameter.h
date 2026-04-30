// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LProgramParameter;
namespace Application::Detail
{
ENGINE_API extern TArray<LProgramParameter*> RegisteredProgramParameters;
} /* ~Namespace Application::Detail */

//# The allowed types for a given parameter
enum struct EProgramParameterBits
{
    //# Stores true. Not value or list allowed
    StoreTrue = 0x1 << 0,
    //# Requires a value.
    Value = 0x1 << 1,
    //# Requires a list.
    List = 0x1 << 2,
};
ENUM_STRUCT_FLAGS(EProgramParameterBits, EProgramParameterFlags)
inline LString LexToString(EProgramParameterFlags Flags) noexcept
{
    std::stringstream ss;
    if (Flags & EProgramParameterBits::StoreTrue) ss << "StoreTrue|";
    if (Flags & EProgramParameterBits::Value) ss << "Value|";
    if (Flags & EProgramParameterBits::List) ss << "List|";
    return ss.str();
}

struct LProgramParameterCreateInfo
{
    LString Identifier;
    LString Description;
    EProgramParameterFlags Flags{ EProgramParameterBits::StoreTrue };
    TArray<LString> Variations;
};

struct LProgramParameter
{
    inline explicit LProgramParameter(LProgramParameterCreateInfo CreateInfo) noexcept
         : Identifier{std::move(CreateInfo.Identifier)}
         , Description{std::move(CreateInfo.Description)}
         , Flags{CreateInfo.Flags}
         , Variations{std::move(CreateInfo.Variations)}
    {
        if (auto It{algo::find(Application::Detail::RegisteredProgramParameters, this)}; It != Application::Detail::RegisteredProgramParameters.end())
        {
            /* Can happen if shared objects do not tear down correctly. */
            LOG_WARNING(LogProgramArguments, "Program argument [{}] already registered.", this->Identifier)
            Application::Detail::RegisteredProgramParameters.erase(It);
        }
        if (auto It{algo::find_if(Application::Detail::RegisteredProgramParameters, [&Identifier = this->Identifier](LProgramParameter* Param)
        {
            return Param->Identifier == Identifier;
        })}; It != Application::Detail::RegisteredProgramParameters.end())
        {
            LOG_FATAL(LogProgramArguments, "Program argument [{}] already registered by another parameter.", this->Identifier)
        }
        Application::Detail::RegisteredProgramParameters.emplace_back(this);
    }
    ENGINE_API ~LProgramParameter();

    LString Identifier;
    LString Description;
    EProgramParameterFlags Flags;
    TArray<LString> Variations;
};

} /* ~Namespace Jafg */
