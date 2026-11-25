// Copyright mzoesch. All rights reserved.

//#
//# Removing duplicate positions runs over every vertex of a mesh (millions for large meshes), so it is compiled with
//# optimizations in every configuration (see Engine/CMakeLists.txt). The loop works on plain integers, so that it is
//# reasonably fast even if it is not optimized (e.g., with another compiler).
//#

#include "UniquePositions.h"

namespace
{

struct LPositionBits final
{
    u32 X;
    u32 Y;
    u32 Z;

    FORCEINLINE bool operator==(LPositionBits const&) const noexcept = default;
};

//# Bitwise, so that it is exact and cheap. But -0 and +0 are the same position.
FORCEINLINE u32 ToBits(f32 const Value) noexcept
{
    return Value == 0.0f ? 0u : std::bit_cast<u32>(Value);
}

FORCEINLINE LPositionBits ToBits(LVec3F const& Position) noexcept
{
    return {ToBits(Position.x), ToBits(Position.y), ToBits(Position.z)};
}

FORCEINLINE u64 Hash(LPositionBits const& Bits) noexcept
{
    u64 H{  (static_cast<u64>(Bits.X) * 0x9E3779B97F4A7C15ull)
          ^ (static_cast<u64>(Bits.Y) * 0xC2B2AE3D27D4EB4Full)
          ^ (static_cast<u64>(Bits.Z) * 0x165667B19E3779F9ull)
          };
    H ^= H >> 29;
    return H;
}

} /* ~Anonymous namespace */

JPH::Array<JPH::Vec3> Jafg::Detail::GatherUniquePositions(TArray<LStaticMesh::HostMesh> const& Meshes)
{
    STAT_FUNCTION()

    std::size_t Count{0};
    for (auto const& Mesh: Meshes)
    {
        for (auto const& Primitive: Mesh.Primitives)
        {
            Count += Primitive.Vertices.size();
        }
    }
    check(Count < std::numeric_limits<u32>::max())

    //#
    //# Open addressing with linear probing. A slot holds the index of a unique position plus one (zero is empty).
    //# At least twice as many slots as positions, so that the table is at most half full even if all are unique.
    //#
    std::size_t const Capacity{std::bit_ceil(std::max<std::size_t>(Count * 2, 16))};
    std::size_t const Mask{Capacity - 1};
    TArray<u32> Slots(Capacity, 0);
    TArray<LPositionBits> UniqueBits;
    JPH::Array<JPH::Vec3> Unique;

    for (auto const& Mesh: Meshes)
    {
        for (auto const& Primitive: Mesh.Primitives)
        {
            for (auto const& Vertex: Primitive.Vertices)
            {
                LPositionBits const Bits{ToBits(Vertex.Position)};
                for (std::size_t Slot{Hash(Bits) & Mask};; Slot = (Slot + 1) & Mask)
                {
                    if (Slots[Slot] == 0)
                    {
                        UniqueBits.push_back(Bits);
                        Unique.emplace_back(Vertex.Position.x, Vertex.Position.y, Vertex.Position.z);
                        Slots[Slot] = static_cast<u32>(Unique.size());
                        break;
                    }
                    if (UniqueBits[Slots[Slot] - 1] == Bits)
                    {
                        break;
                    }
                }
            }
        }
    }

    return Unique;
}
