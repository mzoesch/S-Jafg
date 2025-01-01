// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"
#include "MyWorld/MyWorldStatics.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/VoxelKey.h"

TEST_CASE(ChunkKey, "Engine.Keys")
{
    using namespace Jafg;

    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(  0.0f,   0.0f,   0.0f)), LChunkKey( 0,  0,  0) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(  0.1f,   0.1f,   0.1f)), LChunkKey( 0,  0,  0) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(  1.0f,   1.0f,   1.0f)), LChunkKey( 0,  0,  0) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(  2.0f,   2.0f,   2.0f)), LChunkKey( 0,  0,  0) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 30.0f,  30.0f,  30.0f)), LChunkKey( 0,  0,  0) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 31.9f,  31.9f,  31.9f)), LChunkKey( 0,  0,  0) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 32.0f,  32.0f,  32.0f)), LChunkKey( 1,  1,  1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 32.1f,  32.1f,  32.1f)), LChunkKey( 1,  1,  1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 33.0f,  33.0f,  33.0f)), LChunkKey( 1,  1,  1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 63.9f,  63.9f,  63.9f)), LChunkKey( 1,  1,  1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector( 64.0f,  64.0f,  64.0f)), LChunkKey( 2,  2,  2) )

    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(- 0.1f, - 0.1f, - 0.1f)), LChunkKey(-1, -1, -1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(- 1.0f, - 1.0f, - 1.0f)), LChunkKey(-1, -1, -1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(-31.9f, -31.9f, -31.9f)), LChunkKey(-1, -1, -1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(-32.0f, -32.0f, -32.0f)), LChunkKey(-1, -1, -1) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(-32.1f, -32.1f, -32.1f)), LChunkKey(-2, -2, -2) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(-63.9f, -63.9f, -63.9f)), LChunkKey(-2, -2, -2) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(-64.0f, -64.0f, -64.0f)), LChunkKey(-2, -2, -2) )
    CHECK_EQUALS( "ChunkKey conv from world.", LChunkKey(LVector(-64.1f, -64.1f, -64.1f)), LChunkKey(-3, -3, -3) )

    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey(  0,  0,  0).ToWorldSpaceVector(), LVector(  0.0f,   0.0f,   0.0f) )
    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey(  1,  1,  1).ToWorldSpaceVector(), LVector( 32.0f,  32.0f,  32.0f) )
    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey(  2,  2,  2).ToWorldSpaceVector(), LVector( 64.0f,  64.0f,  64.0f) )
    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey(  3,  3,  3).ToWorldSpaceVector(), LVector( 96.0f,  96.0f,  96.0f) )
    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey( -1, -1, -1).ToWorldSpaceVector(), LVector(-32.0f, -32.0f, -32.0f) )
    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey( -2, -2, -2).ToWorldSpaceVector(), LVector(-64.0f, -64.0f, -64.0f) )
    CHECK_EQUALS( "ChunkKey conv to world.", LChunkKey( -3, -3, -3).ToWorldSpaceVector(), LVector(-96.0f, -96.0f, -96.0f) )

    return;
}

TEST_CASE(VoxelKey, "Engine.Keys")
{
    using namespace Jafg;

    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(  0.0f,  0.0f,  0.0f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(  0.1f,  0.1f,  0.1f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(  0.9f,  0.9f,  0.9f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(  1.0f,  1.0f,  1.0f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(  1.1f,  1.1f,  1.1f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(  1.9f,  1.9f,  1.9f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 31.9f, 31.9f, 31.9f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 32.0f, 32.0f, 32.0f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 32.1f, 32.1f, 32.1f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 32.9f, 32.9f, 32.9f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 33.0f, 33.0f, 33.0f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 33.1f, 33.1f, 33.1f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 63.9f, 63.9f, 63.9f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 64.0f, 64.0f, 64.0f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector( 64.1f, 64.1f, 64.1f)), LVoxelKey(  0,  0,  0) )

    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 0.1f,- 0.1f,- 0.1f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 0.9f,- 0.9f,- 0.9f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 1.0f,- 1.0f,- 1.0f)), LVoxelKey( 30, 30, 30) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 1.1f,- 1.1f,- 1.1f)), LVoxelKey( 30, 30, 30) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 1.9f,- 1.9f,- 1.9f)), LVoxelKey( 30, 30, 30) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 2.0f,- 2.0f,- 2.0f)), LVoxelKey( 29, 29, 29) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(- 2.1f,- 2.1f,- 2.1f)), LVoxelKey( 29, 29, 29) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-28.0f,-29.0f,-30.0f)), LVoxelKey(  3,  2,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-31.0f,-31.0f,-31.0f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-31.9f,-31.9f,-31.9f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-32.0f,-32.0f,-32.0f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-32.1f,-32.1f,-32.1f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-32.9f,-32.9f,-32.9f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-33.0f,-33.0f,-33.0f)), LVoxelKey( 30, 30, 30) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-33.1f,-33.1f,-33.1f)), LVoxelKey( 30, 30, 30) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-33.9f,-33.9f,-33.9f)), LVoxelKey( 30, 30, 30) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-34.0f,-34.0f,-34.0f)), LVoxelKey( 29, 29, 29) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-62.9f,-62.9f,-62.9f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-63.0f,-63.0f,-63.0f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-63.1f,-63.1f,-63.1f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-63.9f,-63.9f,-63.9f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocation(LVector(-64.0f,-64.0f,-64.0f)), LVoxelKey( 31, 31, 31) )

    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(  0.0f,  0.0f,  0.0f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(  0.1f,  0.1f,  0.1f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(  0.9f,  0.9f,  0.9f)), LVoxelKey(  0,  0,  0) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(  1.0f,  1.0f,  1.0f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(  1.1f,  1.1f,  1.1f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(  1.9f,  1.9f,  1.9f)), LVoxelKey(  1,  1,  1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 31.9f, 31.9f, 31.9f)), LVoxelKey( 31, 31, 31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 32.0f, 32.0f, 32.0f)), LVoxelKey( 32, 32, 32) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 32.1f, 32.1f, 32.1f)), LVoxelKey( 32, 32, 32) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 32.9f, 32.9f, 32.9f)), LVoxelKey( 32, 32, 32) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 33.0f, 33.0f, 33.0f)), LVoxelKey( 33, 33, 33) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 33.1f, 33.1f, 33.1f)), LVoxelKey( 33, 33, 33) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 63.9f, 63.9f, 63.9f)), LVoxelKey( 63, 63, 63) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 64.0f, 64.0f, 64.0f)), LVoxelKey( 64, 64, 64) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector( 64.1f, 64.1f, 64.1f)), LVoxelKey( 64, 64, 64) )

    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 0.1f,- 0.1f,- 0.1f)), LVoxelKey( -1, -1, -1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 0.9f,- 0.9f,- 0.9f)), LVoxelKey( -1, -1, -1) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 1.0f,- 1.0f,- 1.0f)), LVoxelKey( -2, -2, -2) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 1.1f,- 1.1f,- 1.1f)), LVoxelKey( -2, -2, -2) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 1.9f,- 1.9f,- 1.9f)), LVoxelKey( -2, -2, -2) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 2.0f,- 2.0f,- 2.0f)), LVoxelKey( -3, -3, -3) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(- 2.1f,- 2.1f,- 2.1f)), LVoxelKey( -3, -3, -3) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-28.0f,-29.0f,-30.0f)), LVoxelKey(-29,-30,-31) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-31.0f,-31.0f,-31.0f)), LVoxelKey(-32,-32,-32) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-31.9f,-31.9f,-31.9f)), LVoxelKey(-32,-32,-32) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-32.0f,-32.0f,-32.0f)), LVoxelKey(-33,-33,-33) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-32.1f,-32.1f,-32.1f)), LVoxelKey(-33,-33,-33) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-32.9f,-32.9f,-32.9f)), LVoxelKey(-33,-33,-33) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-33.0f,-33.0f,-33.0f)), LVoxelKey(-34,-34,-34) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-33.1f,-33.1f,-33.1f)), LVoxelKey(-34,-34,-34) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-33.9f,-33.9f,-33.9f)), LVoxelKey(-34,-34,-34) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-34.0f,-34.0f,-34.0f)), LVoxelKey(-35,-35,-35) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-62.9f,-62.9f,-62.9f)), LVoxelKey(-63,-63,-63) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-63.0f,-63.0f,-63.0f)), LVoxelKey(-64,-64,-64) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-63.1f,-63.1f,-63.1f)), LVoxelKey(-64,-64,-64) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-63.9f,-63.9f,-63.9f)), LVoxelKey(-64,-64,-64) )
    CHECK_EQUALS( "VoxelKey conv from world.", LVoxelKey::FromWorldLocationPreserveLocalSpace(LVector(-64.0f,-64.0f,-64.0f)), LVoxelKey(-65,-65,-65) )

    return;
}
