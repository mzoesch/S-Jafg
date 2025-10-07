// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "MyWorld/Validation/ChunkValidationUtility.h"

namespace Jafg::Validation
{

TArray<LIntVector2> GetAllChunksFromCenterAsBox(const LIntVector2& Center, const i32 Distance)
{
    TArray<LIntVector2> Out;

    const i32 PredictedOutChunkCount = (Distance * 2 + 1) * (Distance * 2 + 1);
    Out.reserve(PredictedOutChunkCount);

    auto MoveCursorRight = [] (const LIntVector2& CursorLocation) { return LIntVector2(CursorLocation.X + 1, CursorLocation.Y); };
    auto MoveCursorDown  = [] (const LIntVector2& CursorLocation) { return LIntVector2(CursorLocation.X, CursorLocation.Y - 1); };
    auto MoveCursorLeft  = [] (const LIntVector2& CursorLocation) { return LIntVector2(CursorLocation.X - 1, CursorLocation.Y); };
    auto MoveCursorUp    = [] (const LIntVector2& CursorLocation) { return LIntVector2(CursorLocation.X, CursorLocation.Y + 1); };
    const std::vector<LIntVector2(*)(const LIntVector2&)> Moves =
    {
        MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp
    };

    i32 Cursor = 1;
    i32 CurrentMoveIndex = 0;
    i32 TimesToMove = 1;
    LIntVector2 TargetPoint = Center;

    Out.emplace_back(Center.X, Center.Y);

    while (true)
    {
        for (i32 _1 = 0; _1 < 2; ++_1)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % static_cast<i32>(Moves.size());
            for (i32 _2 = 0; _2 < TimesToMove; ++_2)
            {
                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                if (Cursor++ >= PredictedOutChunkCount)
                {
                    goto FunctionEnd;
                }

                Out.emplace_back(TargetPoint.X, TargetPoint.Y);

                continue;
            }

            continue;
        }

        ++TimesToMove;
        continue;
    }

FunctionEnd:

    checkCode
    (
        check( static_cast<i32>(Out.size()) == PredictedOutChunkCount )

        for (TArray<LIntVector2>::size_type i { 0 }; i < Out.size(); ++i)
        {
            for (TArray<LIntVector2>::size_type j { i + 1 }; j < Out.size(); ++j)
            {
                check( Out[i] != Out[j] )
            }
        }
    )

    return Out;
}

TArray<LChunkKey> GetAllChunksFromCenterAsBox(const LChunkKey& Center, const i32 Distance, const i32 Height, const i32 HeightOffset)
{
    TArray<LChunkKey> Out;

    /* Intermediate result */
    const TArray<LIntVector2> Ir = GetAllChunksFromCenterAsBox(Center.XY(), Distance);

    Out.reserve(Ir.size() * Height);

    for (const LIntVector2& Chunk : Ir)
    {
        for (i32 Z = 0; Z < Height; ++Z)
        {
            Out.emplace_back(Chunk.X, Chunk.Y, HeightOffset + Z);
        }
    }

    return Out;
}

} /* ~Namespace Jafg::Validation */
