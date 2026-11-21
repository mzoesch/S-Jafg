// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"

#if JAFG_WITH_EDITOR /* Trace geometry is only built in editor builds. */
TRIVIAL_TEST_CASE("mesh", TraceTriangles)
{
    using namespace Jafg;
    constexpr f32 Epsilon{1.0e-4f};
    constexpr f32 Miss{-100.0f};

    /* A unit quad in the xy plane at z = 0. Two triangles. Pulled from memory instead of a file. */
    LStaticMesh Mesh{LStaticMeshSource{.Path = "<test-quad>", .MeshIdx = 0}, "<test-quad>"};
    auto const Result{Mesh.LoadToHost(false, [](TArray<LStaticMesh::HostMesh>& Out) -> LStaticMesh::EResult
    {
        LStaticMesh::HostPrimitive Primitive;
        for (LVec3F const& Position: {LVec3F{0.0f, 0.0f, 0.0f}, LVec3F{1.0f, 0.0f, 0.0f}, LVec3F{1.0f, 1.0f, 0.0f}, LVec3F{0.0f, 1.0f, 0.0f}})
        {
            LStaticMesh::Vertex Vertex{};
            Vertex.Position = Position;
            Primitive.Vertices.push_back(Vertex);
        }
        Primitive.Indices = {0, 1, 2, 0, 2, 3};

        LStaticMesh::HostMesh HostMesh;
        HostMesh.Name = "Quad";
        HostMesh.Primitives.push_back(std::move(Primitive));
        Out.push_back(std::move(HostMesh));
        return LStaticMesh::EResult::Success;
    })};
    TEST_TRUE(Result == LStaticMesh::EResult::Success)
    TEST_TRUE(Mesh.HasTraceGeometry())

    /* Kept after the host memory is gone. */
    Mesh.FreeFromHost();
    TEST_TRUE(Mesh.HasTraceGeometry())

    /* Front face. */
    TEST_LESS(maths::abs(Mesh.TraceTriangles({0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, -1.0f}, 10.0f).value_or(Miss) - 1.0f), Epsilon)
    /* Back face; the second triangle. */
    TEST_LESS(maths::abs(Mesh.TraceTriangles({0.25f, 0.75f, -2.0f}, {0.0f, 0.0f, 1.0f}, 10.0f).value_or(Miss) - 2.0f), Epsilon)
    /* The parameter is in multiples of the (not normalized) direction. */
    TEST_LESS(maths::abs(Mesh.TraceTriangles({0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, -2.0f}, 10.0f).value_or(Miss) - 0.5f), Epsilon)
    /* Beside the quad. */
    TEST_FALSE(Mesh.TraceTriangles({2.0f, 2.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, 10.0f).has_value())
    /* Pointing away. */
    TEST_FALSE(Mesh.TraceTriangles({0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, 10.0f).has_value())
    /* Too short. */
    TEST_FALSE(Mesh.TraceTriangles({0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, -1.0f}, 0.5f).has_value())
    /* Parallel to the quad. */
    TEST_FALSE(Mesh.TraceTriangles({-1.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, 10.0f).has_value())
}
#endif /* JAFG_WITH_EDITOR */
