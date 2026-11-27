// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HParent.h"
#include "Widgets/InputRange.h"
#include "Input_Vector1.generated.h"

namespace Jafg
{

struct LFactoryInput_Vector1;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Vector1)
class ENGINE_API WInput_Vector1 : public WHParent
{
    GENERATED_CLASS_BODY()

    /* For initial setup. */
    friend LFactoryInput_Vector1;

public:

    typedef f64 FloatingVector;
    typedef u64 UnsignedVector;
    typedef i64 SignedVector;

    inline static constexpr auto Precision{6uz};

    template<typename T>
    inline static constexpr bool is_valid_primitive_v{
        std::is_same_v<T, FloatingVector> || std::is_same_v<T, UnsignedVector> || std::is_same_v<T, SignedVector>
        };

protected:

    explicit WInput_Vector1(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->Anchor = EAnchor::HFill;
    }

    template<typename TCxxClass, typename T> requires is_valid_primitive_v<T>
    explicit WInput_Vector1(TNodeStaticInit<TCxxClass> const& Init, T Initial, f32 Acceleration = 0.01f) noexcept : Super{Init}
    {
        this->Anchor = EAnchor::HFill;
        this->Acceleration = Acceleration;
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
    template<typename T> requires is_valid_primitive_v<T>
    inline void Set(T Value, bool bCallEvent = true) noexcept
    {
        if (std::holds_alternative<std::monostate>(this->Vector))
        {
            if constexpr (std::same_as<T, SignedVector>) { this->SetType(EType::Integer); }
            else if constexpr (std::same_as<T, UnsignedVector>) { this->SetType(EType::UInteger); }
            else if constexpr (std::same_as<T, FloatingVector>) { this->SetType(EType::Float); }
            else { std::unreachable(); }
        }
        else
        {
            if constexpr (std::same_as<T, SignedVector>)
            {
                check(std::holds_alternative<SignedVector>(this->Vector))
            }
            else if constexpr (std::same_as<T, UnsignedVector>)
            {
                check(std::holds_alternative<UnsignedVector>(this->Vector))
            }
            else if constexpr (std::same_as<T, FloatingVector>)
            {
                check(std::holds_alternative<FloatingVector>(this->Vector))
            }
            else
            {
                std::unreachable();
            }
        }
        this->Vector = this->Range.Clamp(Value);

        if (!this->GetChildren().empty())
        {
            this->OnVectorDataChanged(bCallEvent);
        }
    }

    template<typename T> requires is_valid_primitive_v<T>
    NODISCARD FORCEINLINE T Get() const noexcept
    {
        if constexpr (std::same_as<T, SignedVector>)
        {
            check(std::holds_alternative<SignedVector>(this->Vector))
            return std::get<SignedVector>(this->Vector);
        }
        else if constexpr (std::same_as<T, UnsignedVector>)
        {
            check(std::holds_alternative<UnsignedVector>(this->Vector))
            return std::get<UnsignedVector>(this->Vector);
        }
        else if constexpr (std::same_as<T, FloatingVector>)
        {
            check(std::holds_alternative<FloatingVector>(this->Vector))
            return std::get<FloatingVector>(this->Vector);
        }
        else
        {
            std::unreachable();
        }
    }

    void SetInputEnabled(bool bEnabled);

    EVENT_DECL(OnVectorChanged, void(WInput_Vector1& Self))
    EVENT_DECL(OnDestruct, void(WInput_Vector1& Self))

    FORCEINLINE constexpr void SetAcceleration(f32 InAcceleration) noexcept { check(InAcceleration > 0.0f) this->Acceleration = InAcceleration; }
    NODISCARD FORCEINLINE constexpr f32 GetAcceleration() const noexcept { return this->Acceleration; }

    //# Optional bounds. @see LInputRange
    void SetMin(std::optional<f64> InMin) noexcept { this->Range.Min = InMin; this->RefreshFill(); }
    void SetMax(std::optional<f64> InMax) noexcept { this->Range.Max = InMax; this->RefreshFill(); }
    NODISCARD FORCEINLINE constexpr std::optional<f64> const& GetMin() const noexcept { return this->Range.Min; }
    NODISCARD FORCEINLINE constexpr std::optional<f64> const& GetMax() const noexcept { return this->Range.Max; }
    //# Optional soft bounds, e.g. the typical values. @see LInputRange
    void SetSoftMin(std::optional<f64> InMin) noexcept { this->Range.SoftMin = InMin; this->RefreshFill(); }
    void SetSoftMax(std::optional<f64> InMax) noexcept { this->Range.SoftMax = InMax; this->RefreshFill(); }

private:

    enum struct EType{Integer, UInteger, Float,};
    inline void SetType(EType Type) noexcept
    {
        check(!this->_HasBegunLife())
        check(std::holds_alternative<std::monostate>(this->Vector))
        switch (Type)
        {
        case EType::Integer: { this->Vector.emplace<SignedVector>(SignedVector{}); break; }
        case EType::UInteger: { this->Vector.emplace<UnsignedVector>(UnsignedVector{}); break; }
        case EType::Float: { this->Vector.emplace<FloatingVector>(FloatingVector{}); break; }
        }
    }

    NODISCARD FORCEINLINE LString FormatAxis(auto Vec) const noexcept
    {
        if constexpr (std::is_integral_v<decltype(Vec)>)
        {
            return std::to_string(Vec);
        }

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(WInput_Vector1::Precision) << Vec;
        auto String{ss.str()};

        while (!String.empty() && String.back() == '0')
        {
            String.pop_back();
        }
        if (!String.empty() && String.back() == '.')
        {
            String.push_back('0');
        }

        return String;
    }

    void OnTextButtonContentChanged(LString const& NewValue);
    void OnVectorDataChanged(bool bCallEvent);
    //# Updates the bar of the axis. @see LInputRange
    void RefreshFill() noexcept;
    std::variant<std::monostate,SignedVector,UnsignedVector,FloatingVector> Vector;
    LInputRange Range;
    std::optional<bool> bInitialEnabledState;
    bool bIgnoreChangeEvents{};

    f32 Acceleration{0.01f};
};

struct LFactoryInput_Vector1: NODE_FACTORY_PARENT(WInput_Vector1)
{
    NODE_FACTORY_BODY(WInput_Vector1)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnVectorChanged, OnVectorChanged)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnDestruct, OnDestruct)

    FORCEINLINE decltype(auto) Enabled(this auto&& Self, bool bEnabled) noexcept
    {
        NODE_FACTORY_SELF().bInitialEnabledState = bEnabled;
        return Self;
    }
    FORCEINLINE decltype(auto) Acceleration(this auto&& Self, f32 InAcceleration) noexcept
    {
        NODE_FACTORY_SELF().Acceleration = InAcceleration;
        return Self;
    }
    //# @see LInputRange
    FORCEINLINE decltype(auto) Min(this auto&& Self, std::optional<f64> InMin) noexcept
    {
        NODE_FACTORY_SELF().Range.Min = InMin;
        return Self;
    }
    FORCEINLINE decltype(auto) Max(this auto&& Self, std::optional<f64> InMax) noexcept
    {
        NODE_FACTORY_SELF().Range.Max = InMax;
        return Self;
    }
    //# @see LInputRange
    FORCEINLINE decltype(auto) SoftMin(this auto&& Self, std::optional<f64> InMin) noexcept
    {
        NODE_FACTORY_SELF().Range.SoftMin = InMin;
        return Self;
    }
    FORCEINLINE decltype(auto) SoftMax(this auto&& Self, std::optional<f64> InMax) noexcept
    {
        NODE_FACTORY_SELF().Range.SoftMax = InMax;
        return Self;
    }
};

} /* ~Namespace Jafg */
