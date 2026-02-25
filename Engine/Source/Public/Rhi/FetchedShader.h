// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/VkAl.h"

namespace Jafg
{

struct LFetchedShader final
{

    LPath Path;
    LString Name;
    TArray<LString> Inherits;

    TArray<LString> IncludeDirectories;
    TArray<LShaderEntrypoint> Entrypoints;

    TOptional<LPath> Src;
    TOptional<LPath> SrcPrefix;
    TOptional<LPath> Dst;
    TOptional<LPath> DstPrefix;

    TOptional<LString> VertexInput;
    TArray<LString> PushConstants;

    inline LPath GetSrc() const
    {
        if (this->Src.has_value() == false)
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: No source path specified for this shader.", this->Path)
        }
        if (this->SrcPrefix.has_value())
        {
            return *this->SrcPrefix / *this->Src;
        }
        return *this->Src;
    }

    inline LPath GetDst() const
    {
        if (this->Dst.has_value() == false)
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: No destination path specified for this shader.", this->Path)
        }
        if (this->DstPrefix.has_value())
        {
            return *this->DstPrefix / *this->Dst;
        }
        return *this->Dst;
    }
};

} /* ~Namespace Jafg */
