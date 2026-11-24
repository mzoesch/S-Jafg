// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/PreferenceCollection.h"
#include "Nodes/Node.h"
#include "Widgets/Input_ListForward.h"

namespace Jafg
{

struct LPreferenceValue;
struct LPreferenceValueChangeRequest;

//#
//# The base class for all preferences that are conceptually a value.
//#
//# When a preference user interface menu of any form is opened, there is a set of preferences where (when following
//# the jafg standard should be some specialization of #TPreference) each preference has a default value and a
//# value (the initial value).
//# A user may change preferences but these preferences are not applied until the user confirmed their changes.
//# A user may at any time discard their current preferences.
//#
struct LPreferenceValue: LPreference
{
    typedef TFunction2<LFactoryNode(LViewport& Viewport
        , TArray<LPreferenceValueChangeRequest>& Changes
        , LPreferenceValue& Preference
        )> LCreateEditNode;

    struct CreateInfo
    {
        LString Identifier;
        LString DisplayName;
        LString Tooltip;

        bool bSupported{ true };
        bool bRequiresRestart{};
        bool bRequiresImminentRestart{};

        LCreateEditNode CreateEditNode;
    };

    LPreferenceValue(CreateInfo Info) noexcept
        : LPreference{EPreference::Value, std::move(Info.Identifier), std::move(Info.DisplayName), std::move(Info.Tooltip), {}}
        , bSupported{Info.bSupported}
        , bRequiresRestart{Info.bRequiresRestart}
        , bRequiresImminentRestart{Info.bRequiresImminentRestart}
        , CreateEditNode{std::move(Info.CreateEditNode)}
    {
        check(!!this->CreateEditNode)
    }

    //# Create a node, to allow seeing/editing the preference.
    NODISCARD LFactoryNode CreateNode(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes)
    {
        check(!!this->CreateEditNode)
        return std::invoke(this->CreateEditNode, Viewport, Changes, *this);
    }

    //# Load the value from persistent storage.
    NODISCARD virtual LString GetValue() const = 0;
    //# Save to value to a persistent storage.
    virtual void SaveValue(LString Value) = 0;

    //# Optional short text that user interfaces show right after the display name.
    LString Annotation;
    //# The user cannot change this preference. User interfaces show it, but mark it as read-only.
    bool bReadOnly{};

    NODISCARD FORCEINLINE constexpr bool IsSupported() const noexcept { return this->bSupported; }
    NODISCARD FORCEINLINE constexpr bool RequiresRestart() const noexcept { return this->bRequiresRestart; }
    NODISCARD FORCEINLINE constexpr bool RequiresImminentRestart() const noexcept { return this->bRequiresImminentRestart; }

private:

    //#
    //# Whether this preference is currently supported.
    //# Supported does not mean not implemented.
    //# Supported means:
    //# - The platform allows (runtime only)
    //# - The configuration allows (might also be compile-time)
    //#
    bool bSupported{ true };
    //#
    //# Changing this preference requires a restart of the application to take effect
    //# or apply to already existing clients. New clients using this preference might
    //# use the new one.
    //#
    bool bRequiresRestart{};
    //#
    //# Whether this preference requires a restart of the application after applying.
    //# Failing to do a restart will cause undefined behavior.
    //#
    bool bRequiresImminentRestart{};

    mutable LCreateEditNode CreateEditNode;
};

struct LPreferenceValueChangeRequest final
{
    TReference<LPreferenceValue> Preference;
    LString Value;
};

//# Provides an easy but not optimal integration of the #LPreferenceValue interface.
struct LCommonPreferenceValue: LPreferenceValue
{
    //# Universal getter/setter functions.
    typedef TFunction2<LString()> LGetter;
    typedef TFunction2<void(LString)> LSetter;

    struct CreateInfo
    {
        LString Identifier;
        LString DisplayName;
        LString Tooltip;

        bool bSupported{ true };
        bool bRequiresRestart{};
        bool bRequiresImminentRestart{};

        LGetter Getter;
        LSetter Setter;

        LCreateEditNode CreateEditNode;
    };

    LCommonPreferenceValue(CreateInfo Info) noexcept
        : LPreferenceValue{{
            .Identifier = std::move(Info.Identifier),
            .DisplayName = std::move(Info.DisplayName),
            .Tooltip = std::move(Info.Tooltip),
            .bSupported = Info.bSupported,
            .bRequiresRestart = Info.bRequiresRestart,
            .bRequiresImminentRestart = Info.bRequiresImminentRestart,
            .CreateEditNode = std::move(Info.CreateEditNode),
            }}
        , Getter{std::move(Info.Getter)}
        , Setter{std::move(Info.Setter)}
    {
        check(!!this->Getter)
        check(!!this->Setter)
    }

    NODISCARD virtual LString GetValue() const override
    {
        check(!!this->Getter)
        return std::invoke(this->Getter);
    }

    virtual void SaveValue(LString Value) override
    {
        check(!!this->Setter)
        std::invoke(this->Setter, std::move(Value));
    }

private:

    mutable LGetter Getter;
    LSetter Setter;
};

FORCEINLINE constexpr LPreferenceValue& LPreference::AsValue() noexcept
{
    check(this->Type == EPreference::Value)
    return *static_cast<LPreferenceValue*>(this);
}
FORCEINLINE constexpr LPreferenceValue const& LPreference::AsValue() const noexcept
{
    check(this->Type == EPreference::Value)
    return *static_cast<LPreferenceValue const*>(this);
}

//# A preference that stores a boolean.
struct LPreferenceValue_Boolean: LCommonPreferenceValue
{
    typedef bool value_type;

    struct CreateInfo
    {
        LString Identifier;
        LString DisplayName;
        LString Tooltip;

        bool bSupported{ true };
        bool bRequiresRestart{};
        bool bRequiresImminentRestart{};

        LGetter Getter;
        LSetter Setter;

        TFunction2<LFactoryNode(LViewport& Viewport
            , TArray<LPreferenceValueChangeRequest>& Changes
            , LPreferenceValue_Boolean& Preference
            )> CreateEditNode;
    };

    LPreferenceValue_Boolean(CreateInfo Info) noexcept
        : LCommonPreferenceValue{{
            .Identifier = std::move(Info.Identifier),
            .DisplayName = std::move(Info.DisplayName),
            .Tooltip = std::move(Info.Tooltip),
            .bSupported = Info.bSupported,
            .bRequiresRestart = Info.bRequiresRestart,
            .bRequiresImminentRestart = Info.bRequiresImminentRestart,
            .Getter = std::move(Info.Getter),
            .Setter = std::move(Info.Setter),
            .CreateEditNode = [CreateEditNode=std::move(Info.CreateEditNode)](LViewport& Viewport
                , TArray<LPreferenceValueChangeRequest>& Changes
                , LPreferenceValue& Preference
                ) mutable
                {
                    return std::invoke(CreateEditNode, Viewport, Changes, static_cast<LPreferenceValue_Boolean&>(Preference));
                },
            }}
    {
    }

};

//# A preference that stores a scalar.
struct LPreferenceValue_Scalar: LCommonPreferenceValue
{
    typedef f64 value_type;

    struct CreateInfo final
    {
        LString Identifier;
        LString DisplayName;
        LString Tooltip;

        bool bSupported{ true };
        bool bRequiresRestart{};
        bool bRequiresImminentRestart{};

        f64 Step;
        std::optional<f64> Minimum;
        std::optional<f64> Maximum;

        LGetter Getter;
        LSetter Setter;
        TFunction2<LFactoryNode(LViewport& Viewport
            , TArray<LPreferenceValueChangeRequest>& Changes
            , LPreferenceValue_Scalar& Preference
            )> CreateEditNode;
    };

    LPreferenceValue_Scalar(CreateInfo Info) noexcept
        : LCommonPreferenceValue{{
            .Identifier = std::move(Info.Identifier),
            .DisplayName = std::move(Info.DisplayName),
            .Tooltip = std::move(Info.Tooltip),
            .bSupported = Info.bSupported,
            .bRequiresRestart = Info.bRequiresRestart,
            .bRequiresImminentRestart = Info.bRequiresImminentRestart,
            .Getter = std::move(Info.Getter),
            .Setter = std::move(Info.Setter),
            .CreateEditNode = [CreateEditNode=std::move(Info.CreateEditNode)](LViewport& Viewport
                , TArray<LPreferenceValueChangeRequest>& Changes
                , LPreferenceValue& Preference
                ) mutable
                {
                    return std::invoke(CreateEditNode, Viewport, Changes, static_cast<LPreferenceValue_Scalar&>(Preference));
                },
            }}
        , Step{Info.Step}
        , Minimum{Info.Minimum}
        , Maximum{Info.Maximum}
        {}

    NODISCARD FORCEINLINE constexpr f64 GetStep() const noexcept { return this->Step; }
    NODISCARD FORCEINLINE constexpr bool HasMinimum() const noexcept { return this->Minimum.has_value(); }
    NODISCARD FORCEINLINE constexpr f64 GetMinimum() const noexcept { check(this->HasMinimum()) return *this->Minimum; }
    NODISCARD FORCEINLINE constexpr f64 GetSafeMinimum() const noexcept { return this->Minimum.value_or(std::numeric_limits<f64>::lowest()); }
    NODISCARD FORCEINLINE constexpr bool HasMaximum() const noexcept { return this->Maximum.has_value(); }
    NODISCARD FORCEINLINE constexpr f64 GetMaximum() const noexcept { check(this->HasMaximum()) return *this->Maximum; }
    NODISCARD FORCEINLINE constexpr f64 GetSafeMaximum() const noexcept { return this->Maximum.value_or(std::numeric_limits<f64>::max()); }

private:

    f64 Step{};

    std::optional<value_type> Minimum;
    std::optional<value_type> Maximum;
};

struct LPreferenceValue_List: LCommonPreferenceValue
{
    typedef LInputListItem value_type;

    struct CreateInfo final
    {
        LString Identifier;
        LString DisplayName;
        LString Tooltip;

        bool bSupported{ true };
        bool bRequiresRestart{};
        bool bRequiresImminentRestart{};

        TFunction2<TArray<value_type>()> Items;

        LGetter Getter;
        LSetter Setter;
        TFunction2<LFactoryNode(LViewport& Viewport
            , TArray<LPreferenceValueChangeRequest>& Changes
            , LPreferenceValue_List& Preference
            )> CreateEditNode;
    };

    LPreferenceValue_List(CreateInfo Info) noexcept
        : LCommonPreferenceValue{{
            .Identifier = std::move(Info.Identifier),
            .DisplayName = std::move(Info.DisplayName),
            .Tooltip = std::move(Info.Tooltip),
            .bSupported = Info.bSupported,
            .bRequiresRestart = Info.bRequiresRestart,
            .bRequiresImminentRestart = Info.bRequiresImminentRestart,
            .Getter = std::move(Info.Getter),
            .Setter = std::move(Info.Setter),
            .CreateEditNode = [CreateEditNode=std::move(Info.CreateEditNode)](LViewport& Viewport
                , TArray<LPreferenceValueChangeRequest>& Changes
                , LPreferenceValue& Preference
                ) mutable
                {
                    return std::invoke(CreateEditNode, Viewport, Changes, static_cast<LPreferenceValue_List&>(Preference));
                },
            }}
        , Items{std::move(Info.Items)}
        {}

    NODISCARD FORCEINLINE constexpr TArray<value_type> GetItems() const noexcept
    {
        check(!!this->Items)
        return this->Items();
    }

private:

    mutable TFunction2<TArray<value_type>()> Items;
};

} /* ~Namespace Jafg */
