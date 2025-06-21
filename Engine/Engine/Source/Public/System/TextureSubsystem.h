// Copyright mzoesch. All rights reserved.

#pragma once

#include "EnginePath.h"
#include "Subsystems/EngineSubsystem.h"
#include "System/Path.h"
#include "TextureSubsystem.generated.h"
#include "Rhi/TextureCore.h"

namespace Jafg
{

class LTexture2;

//#
//# A texture that was loaded by the program and may be used across many different widgets.
//# @remark This is not intended for textures that are very specific.
//#
DECLARE_JAFG_CLASS(EClassFlags::Singleton)
class JTextureSubsystem : public JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JTextureSubsystem)

public:

    void PurgeTextures();

    std::shared_ptr<LTexture2> GetTexture(const LPath& InPath, const ERawImageFormat::Type InFormat) const;
    std::shared_ptr<LTexture2> GetTexture(const LEnginePath& InPath, const ERawImageFormat::Type InFormat) const;

    FORCEINLINE i32  GetLoadedTextureCount() const { return this->Textures.size(); }
    FORCEINLINE auto GetTextures() const -> const std::map<std::string, std::shared_ptr<LTexture2>>& { return this->Textures; }

private:

    mutable std::map<std::string, std::shared_ptr<LTexture2>> Textures;
};

} /* ~Namespace Jafg */
