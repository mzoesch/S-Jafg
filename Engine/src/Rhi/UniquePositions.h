// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"
#include "../Framework/PhysicsForeignCore.h"

namespace Jafg::Detail
{

//#
//# The positions of all vertices of all primitives of the meshes, without duplicates (bitwise equal positions, where
//# -0 equals +0). In order of their first occurrence.
//# Imported meshes split their vertices along normals and UVs, so most positions occur multiple times (e.g., only a
//# quarter of the positions of a mesh with 2.5 million vertices are unique). Building a convex hull from the unique
//# positions is a lot faster and yields the same hull.
//#
//# Compiled with optimizations in every configuration (see Engine/CMakeLists.txt).
//#
NODISCARD JPH::Array<JPH::Vec3> GatherUniquePositions(TArray<LStaticMesh::HostMesh> const& Meshes);

} /* ~Namespace Jafg::Detail */
