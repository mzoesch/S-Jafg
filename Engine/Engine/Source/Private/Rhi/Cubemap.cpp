// Copyright mzoesch. All rights reserved.

#include "Rhi/Cubemap.h"
#include "Rhi/Texture2.h"
#include "User/UserPreferences.h"
#include "Rhi/RhiVendorInclude.h"
#include "Framework/Eye.h"
#include "glm/ext/scalar_uint_sized.hpp"
#include "Rhi/Shader.h"
#include "Widgets/Viewport.h"

#if 0
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

    if (this->Paths.size() > 0)
    {
        LOG_VERBOSE(LogTextureSubsystem, "Shredding cubemap [{}].", this->Paths[0])
    }
    else
    {
        LOG_VERBOSE(LogTextureSubsystem, "Shredding cubemap [{}].", this->Handle.value())
    }

    glDeleteTextures(1, &this->Handle.value());
    this->Handle.reset();

    return;
}

void Jafg::LCubemap::Reload()
{
    if (this->IsValid())
    {
        this->Free();
    }

    jassert( this->Paths.size() == ECubemap::Size )
    check( this->IsValid() == false )

    this->LoadImpl();

    return;
}

void Jafg::LCubemap::Cache(const TArray<LEnginePath>& InPaths)
{
    check( this->IsValid() == false )

    TArray<LEnginePath> Cache = InPaths;
    if (InPaths.size() == 1)
    {
        const LEnginePath Path { InPaths[0] };
        for (u32 i = 0; i < 5; ++i)
        {
            Cache.emplace_back(Path);
            continue;
        }
    }

    this->Paths = std::move(Cache);

    return;
}

void Jafg::LCubemap::Load(const TArray<LEnginePath>& InPaths)
{
    TArray<LEnginePath> Cache = InPaths;
    if (InPaths.size() == 1)
    {
        const LEnginePath Path { InPaths[0] };
        for (u32 i = 0; i < 5; ++i)
        {
            Cache.emplace_back(Path);
            continue;
        }
    }

    jassert( Cache.size() == ECubemap::Size )
    check( this->IsValid() == false )
    this->Paths = std::move(Cache);

    this->LoadImpl();

    return;
}

void Jafg::LCubemap::LoadImpl()
{
    check( this->Paths.size() == ECubemap::Size )
    check( this->IsValid() == false )

    LOG_VERBOSE(LogRhi, "Uploading cubemap [{}].", this->Paths[0])

    const JUserPreferences* Prefs = GetDefault<JUserPreferences>();

    this->Handle.emplace(0);

    glGenTextures(1, &this->Handle.value());
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
        u8* Data { stbi_load(this->Paths[ECubemap::Back].ResolvePath().c_str(), &W, &H, &C, 0) };
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
        u8* Data { stbi_load(this->Paths[ECubemap::Front].ResolvePath().c_str(), &W, &H, &C, 0) };
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
        u8* Data { stbi_load(this->Paths[ECubemap::Top].ResolvePath().c_str(), &W, &H, &C, 0) };
        jassert( Data )
        const TUnique Rotated { Texture2::RotateCW(Data, W, H, C) };
        if (C == 4)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, H, W, 0, GL_RGBA, GL_UNSIGNED_BYTE, Rotated.get());

        }
        else
        {
            check( C == 3 )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, H, W, 0, GL_RGB, GL_UNSIGNED_BYTE, Rotated.get());
        }
        stbi_image_free(Data);
    }

    /* Bottom */
    {
        ::stbi_set_flip_vertically_on_load(false);
        u8* Data { stbi_load(this->Paths[ECubemap::Bottom].ResolvePath().c_str(), &W, &H, &C, 0) };
        jassert( Data )
        const TUnique Rotated { Texture2::RotateCCW(Data, W, H, C) };
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
        u8* Data { stbi_load(this->Paths[ECubemap::Right].ResolvePath().c_str(), &W, &H, &C, 0) };
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
        u8* Data { stbi_load(this->Paths[ECubemap::Left].ResolvePath().c_str(), &W, &H, &C, 0) };
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
#endif