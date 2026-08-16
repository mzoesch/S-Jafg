// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/ResourceReference.h"
#include "Rhi/DeviceBuffers.h"

namespace Jafg
{

//#
//# Represents a generic six-layered two-dimensional texture that is directly uploaded to a device.
//#
//# A texture of a cube map is always loaded with:
//#   - R8G8B8A8Srgb
//#   - e1 samples
//#   - 1 mip level
//# Therefore, you should not import your cube-map textures into the jasset format unless they are used elsewhere.
//#
//# It is implied to know that all faces of the resulting have to be in the same dimension and format.
//#
struct LTextureCube2 final
{
    //# Parent directory of a cube with PX, NX, PY, NY, PZ, NZ.
    struct CubeMap final
    {
        LPath Path;
    };
    //# Custom names for all faces; should be in order of PX, NX, PY, NY, PZ, NZ.
    struct Custom final
    {
        std::array<LPath, 6> Paths;
    };
    typedef std::variant<CubeMap, Custom> LSource;

    enum struct EResult
    {
        Success,
        FileNotFound,
        LoadingError,
    };
    static LStringView ResultToString(EResult Result) noexcept
    {
        switch (Result)
        {
        case EResult::Success: return "Success";
        case EResult::FileNotFound: return "FileNotFound";
        case EResult::LoadingError: return "LoadingError";
        }
        std::unreachable();
    }

    LTextureCube2() noexcept = default;
    NODISCARD static std::shared_ptr<LTextureCube2> MakeSharedTextureCube2(LSource Source) noexcept
    {
        return std::make_shared<LTextureCube2>(std::move(Source));
    }
    explicit LTextureCube2(LSource Source) noexcept: Source{std::move(Source)}
    {
        this->LoadToDevice();
    }
    PROHIBIT_COPY(LTextureCube2)
    DEFAULT_MOVE(LTextureCube2)
    ~LTextureCube2() = default;

    FORCEINLINE bool IsOnDevice() const noexcept { return !!*this->Handle; }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept
    {
        this->View.clear();
        this->Handle.free();
    }

#if !JAFG_IN_SHIPPING
    NODISCARD FORCEINLINE constexpr LPath const& GetDebugName() const noexcept
    {
        if (std::holds_alternative<CubeMap>(this->Source))
        {
            return std::get<CubeMap>(this->Source).Path;
        }
        return std::get<Custom>(this->Source).Paths.front();
    }
#endif /* !JAFG_IN_SHIPPING */

    FORCEINLINE constexpr rhi::device_image const& GetDeviceHandle() const noexcept { return this->Handle; }
    FORCEINLINE bool HasImageView() const noexcept { return static_cast<bool>(*this->View); }
    FORCEINLINE constexpr auto const& GetImageView() const noexcept { return this->View; }

private:

    LSource Source;
    rhi::device_image Handle;
    vk::raii::ImageView View{ nullptr };
};

} /* ~Namespace Jafg */
