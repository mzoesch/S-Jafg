// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace rhi
{

struct vk_binding final
{
    vk::DescriptorType type;
    u32 space;
    u32 index;
};

struct reflected_shader final
{
    typedef algo::transparent_unordered_string_map<TArray<LString>> user_attributes_t;

    enum update_frequency: u8 { rarely, per_frame, }; /* Stable across slang. */
    enum binding_kind: u8 { resource, constant_buffer, };
    enum binding_type: u8 { push_constant_buffer, descriptor_table_slot, };

    NODISCARD static update_frequency from_string(LStringView Value) noexcept
    {
        if (Value == "Rarely" || /*slang annotation*/Value == "0") { return update_frequency::rarely; }
        if (Value == "PerFrame" || /*slang annotation*/Value == "1") { return update_frequency::per_frame; }
        LOG_FATAL(LogSerialization, "Invalid reflected shader update frequency string [{}].", Value)
    }

    struct binding_definition final
    {
        LString Name;
        std::optional<LString> CxxName;
        binding_type Kind;
        u32 Space;
        u32 Index;

        struct resource final
        {
            struct texture2D{};
            struct textureCube2D{};
            struct structured_buffer
            {
                LString Name;
                std::optional<LString> CxxName;
                user_attributes_t UserAttributes;
            };
            std::variant<texture2D, textureCube2D, structured_buffer> base_shape;
        };
        struct sampler_state final
        {
        };
        struct constant_buffer final
        {
            u32 Offset;
            u32 Size;
            u32 Stride;
        };
        typedef std::variant<std::monostate, resource, sampler_state, constant_buffer> type_inner;

        struct array final
        {
            type_inner Type;
            u32 Count;
        };
        typedef std::variant<std::monostate, resource, sampler_state, constant_buffer, array> type_outer;
        type_outer Type;

        user_attributes_t UserAttributes;

        NODISCARD bool operator==(binding_definition const& Rhs) const noexcept
        {
            bool bResult{this->Kind == Rhs.Kind
                && this->Space == Rhs.Space
                && this->Index == Rhs.Index};
            checkCode
            (
                if (bResult)
                {
                    check(this->Name == Rhs.Name)
                }
            )
            return bResult;
        }

        //# @return Nullptr if not found.
        NODISCARD TArray<LString> const* find_user_attribute(LStringView Name, std::optional<std::size_t> Arguments = {}) const noexcept
        {
            if (auto It{this->UserAttributes.find(Name)}; It != this->UserAttributes.end())
            {
                if (Arguments && It->second.size() != *Arguments)
                {
                    LOG_FATAL(LogSerialization, "[{}]: User attribute [{}] has invalid argument count [{}!={}]."
                        , this->Name, Name, It->second.size(), *Arguments)
                }
                return &It->second;
            }
            return nullptr;
        }

        NODISCARD vk::DescriptorType as_descriptor_type() const noexcept
        {
            auto ResourceToDescriptorType{[](auto&& r) -> vk::DescriptorType
            {
                auto& base_shape{std::get<resource>(r).base_shape};
                if (std::holds_alternative<resource::texture2D>(base_shape))
                {
                    return vk::DescriptorType::eSampledImage;
                }
                if (std::holds_alternative<resource::textureCube2D>(base_shape))
                {
                    return vk::DescriptorType::eSampledImage;
                }
                if (std::holds_alternative<resource::structured_buffer>(base_shape))
                {
                    return vk::DescriptorType::eStorageBuffer;
                }
                LOG_FATAL(LogSerialization, "Unsupported binding definition resource base shape kind [{}]."
                    , base_shape.index())
            }};

            auto InnerToDescriptorType{[&](auto&& t) -> std::optional<vk::DescriptorType>
            {
                if (std::holds_alternative<resource>(t))
                {
                    return ResourceToDescriptorType(t);
                }
                if (std::holds_alternative<sampler_state>(t))
                {
                    return vk::DescriptorType::eSampler;
                }
                if (std::holds_alternative<constant_buffer>(t))
                {
                    return vk::DescriptorType::eUniformBuffer;
                }
                return {};
            }};

            if (auto Result{InnerToDescriptorType(this->Type)})
            {
                return *Result;
            }
            if (std::holds_alternative<array>(this->Type))
            {
                auto& arr{std::get<array>(this->Type)};
                if (auto Result{InnerToDescriptorType(arr.Type)})
                {
                    return *Result;
                }
                LOG_FATAL(LogSerialization, "Unsupported binding definition array element type kind [{}]."
                    , arr.Type.index())
            }
            LOG_FATAL(LogSerialization, "Unsupported binding definition type kind [{}]."
                , this->Type.index())
        }

        NODISCARD u32 binding_count() const noexcept
        {
            if (std::holds_alternative<array>(this->Type))
            {
                auto Count{std::get<array>(this->Type).Count};
                if (Count > 0)
                {
                    return Count;
                }
                LOG_FATAL(LogSerialization, "Invalid binding definition array element count [{}]."
                    , Count)
            }
            return 1;
        }
    };

    struct binding final
    {
        LString Name;
        binding_type Kind;
        u32 Space;
        u32 Index;

        NODISCARD bool operator==(binding const& Rhs) const noexcept
        {
            bool bResult{this->Kind == Rhs.Kind
                && this->Space == Rhs.Space
                && this->Index == Rhs.Index};
            checkCode
            (
                if (bResult)
                {
                    check(this->Name == Rhs.Name)
                }
            )
            return bResult;
        }
        NODISCARD bool operator==(binding_definition const& Rhs) const noexcept
        {
            bool bResult{this->Kind == Rhs.Kind
                && this->Space == Rhs.Space
                && this->Index == Rhs.Index};
            checkCode
            (
                if (bResult)
                {
                    check(this->Name == Rhs.Name)
                }
            )
            return bResult;
        }
    };

    struct entry_point final
    {
        struct generated_type final
        {
        };
        struct custom_type final
        {
            LString Name;
            user_attributes_t UserAttributes;
        };

        LString Name;
        LPath Code;
        vk::ShaderStageFlagBits Stage;
        std::variant<generated_type,custom_type> Parameters;
        TArray<binding> Bindings;
    };

    LString Identifier;
    TArray<binding_definition> Parameters;
    std::inplace_vector<entry_point, 2> EntryPoints;

    template<typename TSelf>
    struct space_it final
    {
        constexpr space_it(TSelf& InSelf, std::size_t InIdx) noexcept
            : self{InSelf}
            , idx{InIdx}
        {
            while (this->idx < this->self.Parameters.size())
            {
                if (auto& Param{this->self.Parameters[this->idx]}; Param.Kind == descriptor_table_slot)
                {
                    this->visited.push_back(Param.Space);
                    return;
                }
                ++this->idx;
            }
        }

        TSelf& self;
        std::size_t idx;
        TArray<u32> visited;

        NODISCARD FORCEINLINE constexpr u32 operator*() const noexcept
        {
            check(algo::valid_index(this->self.Parameters, this->idx))
            return this->self.Parameters[this->idx].Space;
        }
        NODISCARD FORCEINLINE constexpr binding_definition const* operator->() const noexcept
        {
            check(algo::valid_index(this->self.Parameters, this->idx))
            return &this->self.Parameters[this->idx];
        }

        FORCEINLINE constexpr space_it& operator++() noexcept
        {
            while (++this->idx < this->self.Parameters.size())
            {
                if (auto& Param{this->self.Parameters[this->idx]}; Param.Kind == descriptor_table_slot)
                {
                    if (!algo::contains(this->visited, Param.Space))
                    {
                        this->visited.push_back(Param.Space);
                        break;
                    }
                }
            }
            return *this;
        }
        NODISCARD FORCEINLINE constexpr bool operator==(space_it const& Rhs) const noexcept { return this->idx == Rhs.idx; }
        NODISCARD FORCEINLINE constexpr auto operator<=>(space_it const& Rhs) const noexcept { return this->idx <=> Rhs.idx; }
    };
    NODISCARD decltype(auto) begin_space() const noexcept { return space_it{*this, 0uz}; }
    NODISCARD decltype(auto) end_space() const noexcept { return space_it{*this, this->Parameters.size()}; }

    NODISCARD decltype(auto) push_constant_iter(this auto&& Self) noexcept
    {
        return Self.Parameters
            | algo::views::filter([](auto const& Param) { return Param.Kind == push_constant_buffer; });
    }
    NODISCARD decltype(auto) descriptor_slot_iter(this auto&& Self) noexcept
    {
        return Self.Parameters
            | algo::views::filter([](auto const& Param) { return Param.Kind == descriptor_table_slot; });
    }

    NODISCARD binding_definition const* find_descriptor_slot(u32 Space, u32 Index) const noexcept
    {
        for (auto& Param: this->Parameters)
        {
            if (Param.Kind == descriptor_table_slot && Param.Space == Space && Param.Index == Index)
            {
                return &Param;
            }
        }
        return nullptr;
    }
    NODISCARD binding_definition const& get_descriptor_slot(u32 Space, u32 Index) const noexcept
    {
        if (auto* Param{this->find_descriptor_slot(Space, Index)}; Param)
        {
            return *Param;
        }
        LOG_FATAL(LogSerialization, "[{}]: No descriptor table slot found for space [{}] and index [{}]."
            , this->Identifier, Space, Index)
    }

    NODISCARD vk::ShaderStageFlags shader_stage_flags_for(auto&& Binding) const noexcept
        requires algo::is_weak_eq_v<binding, std::remove_cvref_t<decltype(Binding)>>
    {
        vk::ShaderStageFlags Result{};
        for (auto& EntryPoint: this->EntryPoints)
        {
            for (auto& B: EntryPoint.Bindings)
            {
                if (B == Binding)
                {
                    Result |= EntryPoint.Stage;
                }
            }
        }
        return Result;
    }
};

SERDE_JSON_ENUM_PAIR(reflected_shader::binding_kind, {
    {reflected_shader::binding_kind::resource, "resource"},
    {reflected_shader::binding_kind::constant_buffer, "constant_buffer"},
    })
SERDE_JSON_ENUM_PAIR(reflected_shader::binding_type, {
    {reflected_shader::binding_type::push_constant_buffer, "pushConstantBuffer"},
    {reflected_shader::binding_type::descriptor_table_slot, "descriptorTableSlot"},
    })

ENGINE_API void from_json(json const& j, reflected_shader::binding_definition& binding_definition);
ENGINE_API void from_json(json const& j, reflected_shader::binding& type);
ENGINE_API void from_json(json const& j, reflected_shader::entry_point::custom_type& type);
ENGINE_API void from_json(json const& j, reflected_shader::entry_point& entry);
ENGINE_API void from_json(json const& j, reflected_shader& shader);

} /* ~Namespace rhi */
