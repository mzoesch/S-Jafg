// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "World.h"
#include "Object.generated.h"

namespace Jafg
{

class LWorld;
class LApplicationInstance;

//#
//# An object that usually uses a world as its outer.
//# Additionally, defines utility functions to help with world and engine access.
//# If you want a simpler base class, use JObjectBase.
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JObject : public ::Jafg::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JObject)

public:

    auto GetEngine() const -> LEngine*;
    auto GetApplicationInstanceSubsystem() const -> LApplicationInstance*;

    FORCEINLINE auto IsWorldValid() const -> bool { return this->CastedOuter != nullptr; }
    FORCEINLINE auto GetWorld() const -> LWorld* { return this->CastedOuter; }

    virtual void BeginLife() override;

    template <typename T> FORCEINLINE auto IsA() const -> bool;
    template <typename T> FORCEINLINE auto IsA(const T*& OutObject) const -> bool;
                          FORCEINLINE auto IsA(const LObjectClass* InStaticClass) const -> bool;
    template <typename T> FORCEINLINE auto As() -> T* { return DynamicCast<T>(this); }
    template <typename T> FORCEINLINE auto As() const -> const T* { return DynamicCast<T>(this); }
    template <typename T> FORCEINLINE auto AsChecked() -> T* ;
    template <typename T> FORCEINLINE auto AsChecked() const -> const T* ;
    template <typename T> FORCEINLINE auto AsPanicked() -> T*;
    template <typename T> FORCEINLINE auto AsPanicked() const -> const T*;

private:

    LWorld* CastedOuter = nullptr;
};

template <typename T>
bool JObject::IsA() const
{
    static_assert(std::is_base_of_v<JObjectBase, T>, "T must derive from JObject");
    return this->GetVTable()->DerivesFrom(T::StaticClass());
}

template <typename T>
bool JObject::IsA(const T*& OutObject) const
{
    static_assert(std::is_base_of_v<JObjectBase, T>, "T must derive from JObject");
    if (this->IsA<T>())
    {
        OutObject = CheckedStaticCast<T>(this);
        return true;
    }

    return false;
}

bool JObject::IsA(const LObjectClass* InStaticClass) const
{
    return this->GetVTable()->DerivesFrom(InStaticClass);
}

template <typename T>
T* JObject::AsChecked()
{
    T* Out = this->As<T>();
    check( Out )
    return Out;
}

template <typename T>
const T* JObject::AsChecked() const
{
    const T* Out = this->As<T>();
    check( Out )
    return Out;
}

template <typename T>
T* JObject::AsPanicked()
{
    T* Out = this->As<T>();
    if (Out == nullptr)
    {
        panicMsgf( "Failed to cast object [{}] to [{}].", this->GetFullName(), T::StaticClass()->GetSpacedClassName() )
    }

    return Out;
}

template <typename T>
const T* JObject::AsPanicked() const
{
    const T* Out = this->As<T>();
    if (Out == nullptr)
    {
        panicMsgf( "Failed to cast object [{}] to [{}].", this->GetFullName(), T::StaticClass()->GetSpacedClassName() )
    }

    return Out;
}

} /* ~Namespace Jafg */
