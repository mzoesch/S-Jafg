// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "InputTypes.h"

namespace Jafg
{

struct LRawInput
{
    explicit LRawInput() = default;
    explicit LRawInput(const LKey Key) : Key(Key), Value(1.0f) { }
    explicit LRawInput(const LKey Key, const float Value) : Key(Key), Value(Value)
    {
    }

    LKey  Key;
    float Value;

    FORCEINLINE bool operator <(const LRawInput& Other) const   { return this->Key < Other.Key;  }
    FORCEINLINE bool operator <(const LKey& Other) const        { return this->Key < Other;      }
    FORCEINLINE bool operator >(const LRawInput& Other) const   { return this->Key > Other.Key;  }
    FORCEINLINE bool operator >(const LKey& Other) const        { return this->Key > Other;      }
    FORCEINLINE bool operator==(const LRawInput& Other) const   { return this->Key == Other.Key; }
    FORCEINLINE bool operator==(const LKey& Other) const        { return this->Key == Other;     }
    FORCEINLINE bool operator!=(const LRawInput& Other) const   { return this->Key != Other.Key; }
    FORCEINLINE bool operator!=(const LKey& Other) const        { return this->Key != Other;     }
};

class JPlayerInput final : public JObject
{
public:

    JPlayerInput() = default;

    /** Called when a new frame is started. */
    void BeginNewFrame(void);

    /** Key frame, add all the keys that are down. */
    FORCEINLINE auto AddKeyDown(const LKey Key) -> void { this->DownKeys.emplace(LRawInput(Key)); }
    FORCEINLINE auto AddKeyDown(const LKey Key, const float Value) -> void { this->DownKeys.emplace(LRawInput(Key, Value)); }
    FORCEINLINE auto GetCurrentlyPressedKeys(void) const -> const std::set<LRawInput>& { return this->DownKeys; }
    /** Whether this key was just downed this frame. */
    bool IsNewDown(const LKey& Key) const;

    void ProcessInput(void);

private:

    /** The keys that are currently down. */
    std::set<LRawInput> DownKeys;
    /** The keys that were down last frame. */
    std::set<LRawInput> LastFrameDownKeys;
};

} /* Namespace Jafg. */
