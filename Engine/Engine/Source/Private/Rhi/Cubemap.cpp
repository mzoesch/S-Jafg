// Copyright mzoesch. All rights reserved.

#include "Rhi/Cubemap.h"
#include "Rhi/Texture2.h"
#include "User/UserPreferences.h"
#include "Rhi/RhiVendorInclude.h"
#include "Framework/Eye.h"
#include "glm/ext/scalar_uint_sized.hpp"
#include "Rhi/Shader.h"
#include "Widgets/Viewport.h"

Jafg::LCubemap::LCubemap(const TArray<LEnginePath>& InPaths)
{
    this->Load(InPaths);
    return;
}

void Jafg::LCubemap::Free()
{
    if (this->IsValid() == false)
    {
        return;
    }

    if (this->Paths.GetSize() > 0)
    {
        LOG_VERBOSE(LogTextureSubsystem, "Shredding cubemap [{}].", this->Paths[0].GetRelativeUnresolvedPath())
    }
    else
    {
        LOG_VERBOSE(LogTextureSubsystem, "Shredding cubemap [{}].", this->Handle.GetValue())
    }

    glDeleteTextures(1, &this->Handle.GetValue());
    this->Handle.Reset();

    return;
}

void Jafg::LCubemap::Reload()
{
    if (this->IsValid())
    {
        this->Free();
    }

    jassert( this->Paths.GetSize() == ECubemap::Size )
    check( this->IsValid() == false )

    this->LoadImpl();

    return;
}

void Jafg::LCubemap::Cache(const TArray<LEnginePath>& InPaths)
{
    check( this->IsValid() == false )

    TArray<LEnginePath> Cache = InPaths;
    if (InPaths.GetSize() == 1)
    {
        const LEnginePath Path { InPaths[0] };
        for (u32 i = 0; i < 5; ++i)
        {
            Cache.Emplace(Path);
            continue;
        }
    }

    this->Paths = std::move(Cache);

    return;
}

void Jafg::LCubemap::Load(const TArray<LEnginePath>& InPaths)
{
    TArray<LEnginePath> Cache = InPaths;
    if (InPaths.GetSize() == 1)
    {
        const LEnginePath Path { InPaths[0] };
        for (u32 i = 0; i < 5; ++i)
        {
            Cache.Emplace(Path);
            continue;
        }
    }

    jassert( Cache.GetSize() == ECubemap::Size )
    check( this->IsValid() == false )
    this->Paths = std::move(Cache);

    this->LoadImpl();

    return;
}

void Jafg::LCubemap::Draw() const
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *this);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    return;
}

void Jafg::LCubemap::LoadImpl()
{
    check( this->Paths.GetSize() == ECubemap::Size )
    check( this->IsValid() == false )

    LOG_VERBOSE(LogRhi, "Uploading cubemap [{}].", this->Paths[0].GetRelativeUnresolvedPath())

    const JUserPreferences* Prefs = GetDefault<JUserPreferences>();

    this->Handle.Emplace(0);

    glGenTextures(1, &this->Handle.GetValue());
    glBindTexture(GL_TEXTURE_CUBE_MAP, *this);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

     i32 W;
     i32 H;
     i32 C;

    /* Back */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Back].ResolveAbsolutePath(*Prefs).ToPtr(), &W, &H, &C, 0) };
        jassert( Data )
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        }
        stbi_image_free(Data);
    }

    /* Front */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Front].ResolveAbsolutePath(*Prefs).ToPtr(), &W, &H, &C, 0) };
        jassert( Data )
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        }
        stbi_image_free(Data);
    }

    /* Top */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Top].ResolveAbsolutePath(*Prefs).ToPtr(), &W, &H, &C, 0) };
        jassert( Data )
        const Smart::TUnique Rotated { Texture2::RotateCW(Data, W, H, C) };
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, H, W, 0, GL_RGBA, GL_UNSIGNED_BYTE, Rotated);

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, H, W, 0, GL_RGB, GL_UNSIGNED_BYTE, Rotated);
        }
        stbi_image_free(Data);
    }

    /* Bottom */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Bottom].ResolveAbsolutePath(*Prefs).ToPtr(), &W, &H, &C, 0) };
        jassert( Data )
        const Smart::TUnique Rotated { Texture2::RotateCCW(Data, W, H, C) };
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, H, W, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, H, W, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        }
        stbi_image_free(Data);
    }

    /* Right */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Right].ResolveAbsolutePath(*Prefs).ToPtr(), &W, &H, &C, 0) };
        jassert( Data )
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        }
        stbi_image_free(Data);
    }

    /* Left */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Left].ResolveAbsolutePath(*Prefs).ToPtr(), &W, &H, &C, 0) };
        jassert( Data )
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        }
        // check( C == 4 )
        // glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
        stbi_image_free(Data);
    }

    return;
}
