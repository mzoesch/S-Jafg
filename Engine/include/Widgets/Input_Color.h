// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HParent.h"
#include "Input_Color.generated.h"

namespace Jafg
{

struct LFactoryInput_Color;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Color)
class ENGINE_API WInput_Color : public WHParent
{
    GENERATED_CLASS_BODY()

    /* For initial setup. */
    friend LFactoryInput_Color;

public:

    inline static constexpr auto Precision{6uz};

    template<typename T>
    inline static constexpr bool is_valid_primitive_v{
        std::is_same_v<T, LLinearColor> || std::is_same_v<T, LLinearColor3> || std::is_same_v<T, LColor>
        };

protected:

    explicit WInput_Color(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->Anchor = EAnchor::HFill;
    }

    template<typename TCxxClass, typename T> requires is_valid_primitive_v<T>
    explicit WInput_Color(TNodeStaticInit<TCxxClass> const& Init, T Initial) noexcept : Super{Init}
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
    template<typename T> requires is_valid_primitive_v<T>
    inline void Set(T Value, bool bCallEvent = true) noexcept
    {
        checkCode
        (
            if (!std::holds_alternative<std::monostate>(this->Vector))
            {
                check(std::holds_alternative<T>(this->Vector))
            }
        )
        this->Vector = Value;

        if (!this->GetChildren().empty())
        {
            this->OnVectorDataChanged(bCallEvent);
        }
    }

    template<typename T> requires is_valid_primitive_v<T>
    FORCEINLINE T const& Get() const noexcept
    {
        check(std::holds_alternative<T>(this->Vector))
        return std::get<T>(this->Vector);
    }

    void SetInputEnabled(bool bEnabled);

    EVENT_DECL(OnVectorChanged, void(WInput_Color& Self))
    EVENT_DECL(OnDestruct, void(WInput_Color& Self))

private:

    inline void SetType(auto&& Type) noexcept
    {
        check(!this->_HasBegunLife())
        check(std::holds_alternative<std::monostate>(this->Vector))
        this->Vector.emplace<std::remove_cvref_t<decltype(Type)>>(Type);
    }

    enum struct EPart{R,G,B,A,};
    NODISCARD FORCEINLINE auto ProjVec(EPart Part, auto Vec) const noexcept
    {
        if constexpr (requires { Vec.a; })
        {
            switch (Part)
            {
            case EPart::R: return Vec.r;
            case EPart::G: return Vec.g;
            case EPart::B: return Vec.b;
            case EPart::A: return Vec.a;
            }
        }
        else
        {
            check(Part != EPart::A)
            switch (Part)
            {
            case EPart::R: return Vec.r;
            case EPart::G: return Vec.g;
            case EPart::B: return Vec.b;
            default: std::unreachable();
            }
        }
    }
    NODISCARD FORCEINLINE auto& ProjVecL(EPart Part, auto& Vec) noexcept
    {
        if constexpr (requires { Vec.a; })
        {
            switch (Part)
            {
            case EPart::R: return Vec.r;
            case EPart::G: return Vec.g;
            case EPart::B: return Vec.b;
            case EPart::A: return Vec.a;
            }
        }
        else
        {
            check(Part != EPart::A)
            switch (Part)
            {
            case EPart::R: return Vec.r;
            case EPart::G: return Vec.g;
            case EPart::B: return Vec.b;
            default: std::unreachable();
            }
        }
    }
    NODISCARD FORCEINLINE LString FormatAxis(EPart Part, auto Vec) const noexcept
    {
        if constexpr (std::is_integral_v<decltype(this->ProjVec(Part, Vec))>)
        {
            return std::to_string(this->ProjVec(Part, Vec));
        }

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(WInput_Color::Precision) << this->ProjVec(Part, Vec);
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

    void OnTextButtonContentChanged(EPart Part, LString const& NewValue);
    void OnVectorDataChanged(bool bCallEvent);
    std::variant<std::monostate,LLinearColor, LLinearColor3, LColor> Vector;
    std::optional<bool> bInitialEnabledState;
    bool bIgnoreChangeEvents{};
};

struct LFactoryInput_Color : NODE_FACTORY_PARENT(WInput_Color)
{
    NODE_FACTORY_BODY(WInput_Color)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnVectorChanged, OnVectorChanged)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnDestruct, OnDestruct)

    FORCEINLINE decltype(auto) Enabled(this auto&& Self, bool bEnabled) noexcept
    {
        NODE_FACTORY_SELF().bInitialEnabledState = bEnabled;
        return Self;
    }
};

} /* ~Namespace Jafg */
