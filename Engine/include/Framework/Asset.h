// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

namespace Detail
{

inline constexpr LString AssetPostfix{".jasset"};
inline constexpr u32 AssetHeaderMagic{ 0x4B744755 };
inline constexpr u32 MagicReserved32{ 0xFF11EE22 };
inline constexpr u64 MagicReserved64{ 0xDD33CC44CC55BB66 };

enum struct EAsset : u32
{
    Identity                = 0,
    StaticMesh              = 1,
    Material                = 2,
    MaterialInstance        = 3,
    Texture                 = 4,
    Font                    = 5,
};
constexpr LStringView LexToString(EAsset Asset) noexcept
{
    switch (Asset)
    {
    case EAsset::Identity: return "Identity";
    case EAsset::StaticMesh: return "StaticMesh";
    case EAsset::Material: return "Material";
    case EAsset::MaterialInstance: return "MaterialInstance";
    case EAsset::Texture: return "Texture";
    case EAsset::Font: return "Font";
    }
}

#pragma pack(push, 1)
struct LAssetHeader final
{
    u32 Magic{ AssetHeaderMagic };
    u32 Version;
    EAsset Type;
    u32 __reserved1{ MagicReserved32 };
    u64 __reserved2{ MagicReserved64 };
    u64 __reserved3{ MagicReserved64 };
    u64 __reserved4{ MagicReserved64 };
};
#pragma pack(pop)
static_assert(std::is_standard_layout_v<LAssetHeader> && std::is_trivially_copyable_v<LAssetHeader>);
SERDE_BIN_NON_INTRUSIVE(LAssetHeader, Magic, Version, Type, __reserved1, __reserved2, __reserved3, __reserved4)

struct LAsset final
{
    LPath Path;
    TArray<LString> Tags;

    struct StaticMesh final
    {
        LString Glb;
    };
    struct Material final
    {
        LString Value;
    };
    struct Texture final
    {
        LString Value;
    };

    struct Export
    {
        EAsset Asset;

    };
    TArray<Export> Exports;
};

}  /* namespace Detail */

} /* ~Namespace Jafg */

template<> struct serde::is_stable_enum<Jafg::Detail::EAsset> : std::true_type {};
