// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"

namespace Jafg::Validation
{

void GetAllChunksFromCenterAsBox(const LChunkKey2& Center, const int32 Distance, TdhArray<LChunkKey2>& OutChunks)
{
    const int32 PredictedOutChunkCount = (Distance * 2 + 1) * (Distance * 2 + 1);
    OutChunks.Reserve(PredictedOutChunkCount);

    auto MoveCursorRight = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X + 1, CursorLocation.Y); };
    auto MoveCursorDown  = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X, CursorLocation.Y - 1); };
    auto MoveCursorLeft  = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X - 1, CursorLocation.Y); };
    auto MoveCursorUp    = [] (const LChunkKey2& CursorLocation) { return LChunkKey2(CursorLocation.X, CursorLocation.Y + 1); };
    const std::vector<LChunkKey2(*)(const LChunkKey2&)> Moves =
    {
        MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp
    };

    int32 Cursor = 1;
    int32 CurrentMoveIndex = 0;
    int32 TimesToMove = 1;
    LChunkKey2 TargetPoint = Center;

    OutChunks.Emplace(Center.X, Center.Y);

    while (true)
    {
        for (int32 _1 = 0; _1 < 2; ++_1)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % static_cast<int32>(Moves.size());
            for (int32 _2 = 0; _2 < TimesToMove; ++_2)
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
        check( static_cast<int32>(OutChunks.GetSize()) == PredictedOutChunkCount )

        for (int32 i = 0; i < OutChunks.GetSize(); ++i)
        {
            for (int32 j = i + 1; j < OutChunks.GetSize(); ++j)
            {
                check( OutChunks[i] != OutChunks[j] )
            }
        }
    )

    return;
}

} /* ~Namespace Jafg::Validation */
