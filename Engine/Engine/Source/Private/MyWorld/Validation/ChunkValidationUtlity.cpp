// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"

namespace Jafg::Validation
{

void GetAllChunksFromCenterAsBox(const LChunkKey2& Center, const i32 Distance, TdhArray<LChunkKey2>& OutChunks)
{
    const i32 PredictedOutChunkCount = (Distance * 2 + 1) * (Distance * 2 + 1);
    OutChunks.Reserve(PredictedOutChunkCount);

    auto MoveCursorRight = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X + 1, CursorLocation.Y); };
    auto MoveCursorDown  = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X, CursorLocation.Y - 1); };
    auto MoveCursorLeft  = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X - 1, CursorLocation.Y); };
    auto MoveCursorUp    = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X, CursorLocation.Y + 1); };
    const std::vector<LChunkKey2(*)(const LChunkKey2&)> Moves =
    {
        MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp
    };

    i32 Cursor = 1;
    i32 CurrentMoveIndex = 0;
    i32 TimesToMove = 1;
    LChunkKey2 TargetPoint = Center;

    OutChunks.Emplace(Center.X, Center.Y);

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

                OutChunks.Emplace(TargetPoint.X, TargetPoint.Y);

                continue;
            }

            continue;
        }

        ++TimesToMove;
        continue;
    }

FunctionEnd:

    checkCode(
        check( static_cast<i32>(OutChunks.GetSize()) == PredictedOutChunkCount )

        for (i32 i = 0; i < OutChunks.GetSize(); ++i)
        {
            for (i32 j = i + 1; j < OutChunks.GetSize(); ++j)
            {
                check( OutChunks[i] != OutChunks[j] )
            }
        }
    )

    return;
}

} /* ~Namespace Jafg::Validation */
