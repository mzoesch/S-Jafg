// Copyright mzoesch. All rights reserved.

#pragma once

#include "Physics/PhysicCompontent.h"
#include "MyWorld/Chunk/Chunk.h"

namespace Jafg
{

class LChunkPhysicsComponent final : public LPhysicsComponent
{
public:

    LChunkPhysicsComponent() = delete;
    explicit LChunkPhysicsComponent(AChunk* InChunk) : Owner(InChunk) { }
    DEFAULT_REALLOC_OF_ANY_FORM(LChunkPhysicsComponent)
    ~LChunkPhysicsComponent() override { LPhysicsComponent::~LPhysicsComponent(); }

    bool IsInTheoreticalMaxBounds(const LVector& Point) const;

    TOptional<LVector> GetNormalAtLocation(const LVector& InLocation, const LVector& InTraceNormal) const;

    // LPhysicsComponent implementation
    bool Overlaps(const LVector& Point) const override;
    bool Overlaps(const LVector& Point, const float Radius) const override { return false; } // Currently not used. Implement this if needed.
    bool Overlaps(const LPhysicsComponent& Other) const override { return false; } // Currently not used. Implement this if needed.
    bool Sweep(const LVector& Start, const LVector& End, LHitResult& OutHit) const override;
    // ~LPhysicsComponent implementation

private:

    AChunk* Owner = nullptr;
    //# The step interval used for walking along the ray.
    static constexpr float TraceStep { 0.001f };
};

} /* ~Namespace Jafg */
