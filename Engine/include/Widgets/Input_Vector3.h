// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HParent.h"
#include "Input_Vector3.generated.h"

namespace Jafg
{

struct LFactoryInput_Vector3;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Vector3)
class ENGINE_API WInput_Vector3 : public WHParent
{
    GENERATED_CLASS_BODY()

    /* For initial setup. */
    friend LFactoryInput_Vector3;

public:

    typedef LVec3D   FloatingVector;
    typedef LVec3u64 UnsignedVector;
    typedef LVec3i64 SignedVector;

    inline static constexpr auto Precision{6uz};

    template<typename T>
    inline static constexpr bool is_valid_primitive_v{
        std::is_same_v<T, FloatingVector::value_type> || std::is_same_v<T, UnsignedVector::value_type> || std::is_same_v<T, SignedVector::value_type>
        };

protected:

    explicit WInput_Vector3(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->Anchor = EAnchor::HFill;
    }

    template<typename TCxxClass, typename T> requires is_valid_primitive_v<T>
    explicit WInput_Vector3(TNodeStaticInit<TCxxClass> const& Init, TVec3<T,maths::defaultp> Initial) noexcept : Super{Init}
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
    inline void Set(TVec3<T,maths::defaultp> Value, bool bCallEvent = true) noexcept
    {
        if (std::holds_alternative<std::monostate>(this->Vector))
        {
            if constexpr (std::same_as<T, SignedVector::value_type>) { this->SetType(EType::Integer); }
            else if constexpr (std::same_as<T, UnsignedVector::value_type>) { this->SetType(EType::UInteger); }
            else if constexpr (std::same_as<T, FloatingVector::value_type>) { this->SetType(EType::Float); }
            else { std::unreachable(); }
        }
        else
        {
            if constexpr (std::same_as<T, SignedVector::value_type>)
            {
                check(std::holds_alternative<SignedVector>(this->Vector))
            }
            else if constexpr (std::same_as<T, UnsignedVector::value_type>)
            {
                check(std::holds_alternative<UnsignedVector>(this->Vector))
            }
            else if constexpr (std::same_as<T, FloatingVector::value_type>)
            {
                check(std::holds_alternative<FloatingVector>(this->Vector))
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

    template<typename T> requires is_valid_primitive_v<T>
    FORCEINLINE TVec3<T,maths::defaultp> Get() const noexcept
    {
        if constexpr (std::same_as<T, SignedVector::value_type>)
        {
            check(std::holds_alternative<SignedVector>(this->Vector))
            return std::get<SignedVector>(this->Vector);
        }
        else if constexpr (std::same_as<T, UnsignedVector::value_type>)
        {
            check(std::holds_alternative<UnsignedVector>(this->Vector))
            return std::get<UnsignedVector>(this->Vector);
        }
        else if constexpr (std::same_as<T, FloatingVector::value_type>)
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

    EVENT_DECL(OnVectorChanged, void(WInput_Vector3& Self))
    EVENT_DECL(OnDestruct, void(WInput_Vector3& Self))

private:

    enum struct EType{Integer, UInteger, Float,};
    inline void SetType(EType Type) noexcept
    {
        check(!this->_HasBegunLife())
        check(std::holds_alternative<std::monostate>(this->Vector))
        switch (Type)
        {
        case EType::Integer: { this->Vector.emplace<SignedVector>(maths::zero_vector<SignedVector>); break; }
        case EType::UInteger: { this->Vector.emplace<UnsignedVector>(maths::zero_vector<UnsignedVector>); break; }
        case EType::Float: { this->Vector.emplace<FloatingVector>(maths::zero_vector<FloatingVector>); break; }
        }
    }

    enum struct EPart{X,Y,Z,};
    NODISCARD FORCEINLINE decltype(auto) ProjVec(EPart Part, auto Vec) const noexcept
    {
        switch (Part)
        {
        case EPart::X: return Vec.x;
        case EPart::Y: return Vec.y;
        case EPart::Z: return Vec.z;
        default: std::unreachable();
        }
    }
    NODISCARD FORCEINLINE auto& ProjVecL(EPart Part, auto& Vec) noexcept
    {
        switch (Part)
        {
        case EPart::X: return Vec.x;
        case EPart::Y: return Vec.y;
        case EPart::Z: return Vec.z;
        default: std::unreachable();
        }
    }
    NODISCARD FORCEINLINE LString FormatAxis(EPart Part, auto Vec) const noexcept
    {
        if constexpr (std::is_integral_v<decltype(this->ProjVec(Part, Vec))>)
        {
            return std::to_string(this->ProjVec(Part, Vec));
        }

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(WInput_Vector3::Precision) << this->ProjVec(Part, Vec);
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
    std::variant<std::monostate,SignedVector,UnsignedVector,FloatingVector> Vector;
    std::optional<bool> bInitialEnabledState;
    bool bIgnoreChangeEvents{};
};

struct LFactoryInput_Vector3 : NODE_FACTORY_PARENT(WInput_Vector3)
{
    NODE_FACTORY_BODY(WInput_Vector3)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnVectorChanged, OnVectorChanged)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnDestruct, OnDestruct)

    FORCEINLINE decltype(auto) Enabled(this auto&& Self, bool bEnabled) noexcept
    {
        NODE_FACTORY_SELF().bInitialEnabledState = bEnabled;
        return Self;
    }
};

} /* ~Namespace Jafg */
