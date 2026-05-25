// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HParent.h"
#include "Input_Vector2.generated.h"

namespace Jafg
{

struct LFactoryInput_Vector2;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Vector2)
class ENGINE_API WInput_Vector2 : public WHParent
{
    GENERATED_CLASS_BODY()

    /* For initial setup. */
    friend LFactoryInput_Vector2;

protected:

    explicit WInput_Vector2(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->Anchor = EAnchor::HFill;
    }

    template<typename TCxxClass, typename T> requires(std::same_as<T, i64> || std::same_as<T, u64> || std::same_as<T, f64>)
    explicit WInput_Vector2(TNodeStaticInit<TCxxClass> const& Init, TVec2<T,maths::defaultp> Initial) noexcept : Super{Init}
    {
        this->Anchor = EAnchor::HFill;
        this->Set(Initial);
    }

public:

    virtual void Construct() override;
    virtual void Destruct() override
    {
        this->OnDestruct.InvokeIfBound(*this);
        Super::Destruct();
        return;
    }

    //# The type of vector. Once set, it cannot be changed.
    template<typename T> requires(std::same_as<T, i64> || std::same_as<T, u64> || std::same_as<T, f64>)
    inline void Set(TVec2<T,maths::defaultp> Value, bool bCallEvent = true) noexcept
    {
        if (std::holds_alternative<std::monostate>(this->Vector))
        {
            if constexpr (std::same_as<T, i64>) { this->SetType(EType::Integer); }
            else if constexpr (std::same_as<T, u64>) { this->SetType(EType::UInteger); }
            else if constexpr (std::same_as<T, f64>) { this->SetType(EType::Float); }
            else { std::unreachable(); }
        }
        else
        {
            if constexpr (std::same_as<T, i64>)
            {
                check(std::holds_alternative<LVec2i64>(this->Vector))
            }
            else if constexpr (std::same_as<T, u64>)
            {
                check(std::holds_alternative<LVec2u64>(this->Vector))
            }
            else if constexpr (std::same_as<T, f64>)
            {
                check(std::holds_alternative<LVec2D>(this->Vector))
            }
            else
            {
                std::unreachable();
            }
        }
        this->Vector = Value;

        if (!this->GetChildren().empty())
        {
            this->OnVectorDataChanged(bCallEvent);
        }
        return;
    }

    template<typename T> requires(std::same_as<T, i64> || std::same_as<T, u64> || std::same_as<T, f64>)
    FORCEINLINE TVec2<T,maths::defaultp> Get() const noexcept
    {
        if constexpr (std::same_as<T, i64>)
        {
            check(std::holds_alternative<LVec2i64>(this->Vector))
            return std::get<LVec2i64>(this->Vector);
        }
        else if constexpr (std::same_as<T, u64>)
        {
            check(std::holds_alternative<LVec2u64>(this->Vector))
            return std::get<LVec2u64>(this->Vector);
        }
        else if constexpr (std::same_as<T, f64>)
        {
            check(std::holds_alternative<LVec2D>(this->Vector))
            return std::get<LVec2D>(this->Vector);
        }
        else
        {
            std::unreachable();
        }
    }

    void SetInputEnabled(bool bEnabled);

    EVENT_DECL(OnVectorChanged, void(WInput_Vector2& Self))
    EVENT_DECL(OnDestruct, void(WInput_Vector2& Self))

private:

    enum struct EType{Integer, UInteger, Float,};
    inline void SetType(EType Type) noexcept
    {
        check(!this->_HasBegunLife())
        check(std::holds_alternative<std::monostate>(this->Vector))
        switch (Type)
        {
        case EType::Integer: { this->Vector.emplace<LVec2i64>(maths::zero_vector<LVec2i64>); break; }
        case EType::UInteger: { this->Vector.emplace<LVec2u64>(maths::zero_vector<LVec2u64>); break; }
        case EType::Float: { this->Vector.emplace<LVec2D>(maths::zero_vector<LVec2D>); break; }
        }
    }

    enum struct EPart{X,Y,};
    void OnTextButtonContentChanged(EPart Part, LString const& NewValue);
    void OnVectorDataChanged(bool bCallEvent);
    std::variant<std::monostate,LVec2i64,LVec2u64,LVec2D> Vector;
    std::optional<bool> bInitialEnabledState;
    bool bIgnoreChangeEvents{};
};

struct LFactoryInput_Vector2 : NODE_FACTORY_PARENT(WInput_Vector2)
{
    NODE_FACTORY_BODY(WInput_Vector2)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnVectorChanged, OnVectorChanged)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnDestruct, OnDestruct)

    FORCEINLINE decltype(auto) Enabled(this auto&& Self, bool bEnabled) noexcept
    {
        NODE_FACTORY_SELF().bInitialEnabledState = bEnabled;
        return Self;
    }
};

} /* ~Namespace Jafg */
