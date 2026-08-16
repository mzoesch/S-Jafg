// Copyright mzoesch. All rights reserved.

#include "Rhi/ReflectedShader.h"

namespace
{

NODISCARD TArray<std::pair<LString,TArray<LString>>> from_json_user_attribs(json const& j)
{
    TArray<std::pair<LString,TArray<LString>>> Result;

    for (auto& Attribute: j)
    {
        auto& Ref{Result.emplace_back(Attribute.at("name").get<LString>(), TArray<LString>{}).second};
        for (auto& Argument: Attribute.at("arguments"))
        {
            if (Argument.is_string())
            {
                Ref.emplace_back(Argument.get<LString>());
            }
            else if (Argument.is_number_float())
            {
                Ref.emplace_back(algo::sprintf("{}", Argument.get<f64>()));
            }
            else if (Argument.is_number())
            {
                Ref.emplace_back(algo::sprintf("{}", Argument.get<i64>()));
            }
            else if (Argument.is_boolean())
            {
                if (auto b{Argument.get<bool>()}; b)
                {
                    Ref.emplace_back("true");
                }
                else
                {
                    Ref.emplace_back("false");
                }
            }
            else
            {
                LOG_FATAL(LogSerialization, "Unsupported user attribute value type [{}].", Argument.type_name())
            }
        }
    }

    return Result;
}

} /* ~Namespace <Anonymous> */

namespace rhi
{

void from_json(json const& j, reflected_shader::binding_definition& binding_definition)
{
    j.at("name").get_to(binding_definition.Name);
    {
        auto& b = j.at("binding");
        b.at("kind").get_to(binding_definition.Kind);
        if (b.contains("space"))
        {
            b.at("space").get_to(binding_definition.Space);
        }
        else
        {
            binding_definition.Space = 0;
        }
        b.at("index").get_to(binding_definition.Index);
    }

    auto resolve{[&](auto&& t, auto&& Type)
    {
        if (t.contains("elementType"))
        {
            if (auto& et = t.at("elementType"); et.contains("userAttribs"))
            {
                for (auto& [Key, value]: from_json_user_attribs(et.at("userAttribs")))
                {
                    if (Key == "CxxName")
                    {
                        if (value.size() != 1)
                        {
                            LOG_FATAL(LogSerialization, "Expected exactly one argument for user attribute [{}]. But got [{}]."
                                , Key, value.size())
                        }
                        binding_definition.CxxName = value.front();
                    }
                    binding_definition.UserAttributes[std::move(Key)] = std::move(value);
                }
            }
        }
        else if (t.contains("resultType"))
        {
            if (auto& rt = t.at("resultType"); rt.contains("elementType"))
            if (auto& et = rt.at("elementType"); et.contains("userAttribs"))
            {
                for (auto& [Key, value]: from_json_user_attribs(et.at("userAttribs")))
                {
                    if (Key == "CxxName")
                    {
                        if (value.size() != 1)
                        {
                            LOG_FATAL(LogSerialization, "Expected exactly one argument for user attribute [{}]. But got [{}]."
                                , Key, value.size())
                        }
                        binding_definition.CxxName = value.front();
                    }
                    binding_definition.UserAttributes[std::move(Key)] = std::move(value);
                }
            }
        }

        LString kind{t.at("kind").template get<LString>()};
        if (kind == "resource")
        {
            auto& ref{Type.template emplace<reflected_shader::binding_definition::resource>()};
            [&t](auto&& variant)
            {
                LString base_shape{t.at("baseShape").template get<LString>()};
                if (base_shape == "texture2D")
                {
                    variant = reflected_shader::binding_definition::resource::texture2D{};
                    return;
                }
                if (base_shape == "textureCube")
                {
                    variant = reflected_shader::binding_definition::resource::textureCube2D{};
                    return;
                }
                if (base_shape == "structuredBuffer")
                {
                    auto& result_type{t.at("resultType")};
                    reflected_shader::binding_definition::resource::structured_buffer result{
                        .Name = result_type.at("name").template get<LString>()
                        };
                    if (result_type.contains("userAttribs"))
                    {
                        for (auto& [Key, value]: from_json_user_attribs(result_type.at("userAttribs")))
                        {
                            if (Key == "CxxName")
                            {
                                if (value.size() != 1)
                                {
                                    LOG_FATAL(LogSerialization, "Expected exactly one argument for user attribute [{}]. But got [{}]."
                                        , Key, value.size())
                                }
                                result.CxxName = value.front();
                            }
                            result.UserAttributes[std::move(Key)] = std::move(value);
                        }
                    }
                    variant = std::move(result);
                    return;
                }
                LOG_FATAL(LogSerialization, "Unsupported binding definition resource base shape [{}].", base_shape)
            }(ref.base_shape);
        }
        else if (kind == "samplerState")
        {
            Type.template emplace<reflected_shader::binding_definition::sampler_state>();
        }
        else if (kind == "constantBuffer")
        {
            auto& ref{Type.template emplace<reflected_shader::binding_definition::constant_buffer>()};
            auto& b = t.at("elementVarLayout").at("binding");
            b.at("offset").get_to(ref.Offset);
            b.at("size").get_to(ref.Size);
            b.at("elementStride").get_to(ref.Stride);
        }
        else
        {
            return false;
        }
        return true;
    }};

    if (auto& t = j.at("type"); resolve(t, binding_definition.Type))
    {
    }
    else if (LString kind{t.at("kind").get<LString>()}; kind == "array")
    {
        auto& array_type{binding_definition.Type.emplace<reflected_shader::binding_definition::array>()};
        t.at("elementCount").get_to(array_type.Count);
        if (auto& e = t.at("elementType"); !resolve(e, array_type.Type))
        {
            LOG_FATAL(LogSerialization, "Unsupported binding definition array element type kind [{}].", e.at("kind").get<LString>())
        }
    }
    else
    {
        LOG_FATAL(LogSerialization, "Unsupported binding definition type kind [{}].", kind)
    }

    if (j.contains("userAttribs"))
    {
        for (auto& [Key, value]: from_json_user_attribs(j.at("userAttribs")))
        {
            if (Key == "CxxName")
            {
                if (value.size() != 1)
                {
                    LOG_FATAL(LogSerialization, "[{}]: Expected exactly one argument for user attribute [{}]. But got [{}]."
                        , binding_definition.Name, Key, value.size())
                }
                binding_definition.CxxName = value.front();
            }
            binding_definition.UserAttributes[std::move(Key)] = std::move(value);
        }
    }

    if (!binding_definition.CxxName)
    {
        if (std::holds_alternative<reflected_shader::binding_definition::resource>(binding_definition.Type))
        {
            auto& Resource{std::get<reflected_shader::binding_definition::resource>(binding_definition.Type)};
            if (std::holds_alternative<reflected_shader::binding_definition::resource::structured_buffer>(Resource.base_shape))
            {
                auto& StructuredBuffer{std::get<reflected_shader::binding_definition::resource::structured_buffer>(Resource.base_shape)};
                if (StructuredBuffer.CxxName)
                {
                    binding_definition.CxxName = StructuredBuffer.CxxName;
                }
            }
        }
    }
}

void from_json(json const& j, reflected_shader::binding& type)
{
    j.at("name").get_to(type.Name);
    auto& binding = j.at("binding");
    binding.at("kind").get_to(type.Kind);
    if (binding.contains("space"))
    {
        binding.at("space").get_to(type.Space);
    }
    else
    {
        type.Space = 0;
    }
    binding.at("index").get_to(type.Index);
}

void from_json(json const& j, reflected_shader::entry_point::custom_type& type)
{
    j.at("name").get_to(type.Name);
    if (j.contains("userAttribs"))
    {
        for (auto& [Key, value]: from_json_user_attribs(j.at("userAttribs")))
        {
            type.UserAttributes[std::move(Key)] = std::move(value);
        }
    }
}

void from_json(json const& j, reflected_shader::entry_point& entry)
{
    j.at("name").get_to(entry.Name);

    if (LString stage{j.at("stage").get<LString>()}; stage == "vertex"sv)
    {
        entry.Stage = vk::ShaderStageFlagBits::eVertex;
    }
    else if (stage == "fragment"sv)
    {
        entry.Stage = vk::ShaderStageFlagBits::eFragment;
    }
    else
    {
        LOG_FATAL(LogSerialization, "Unsupported entry point stage [{}].", stage)
    }

    if (j.at("parameters").is_array())
    {
        for (auto& parameter: j.at("parameters"))
        {
            auto& parameter_type{parameter.at("type")};
            LString parameter_type_kind{parameter_type.at("kind").get<LString>()};
            if (parameter_type_kind == "struct"sv)
            {
                if (std::holds_alternative<reflected_shader::entry_point::custom_type>(entry.Parameters))
                {
                    LOG_FATAL(LogSerialization, "Entry point parameter type kind [{}] is not supported multiple custom types."
                        , parameter_type_kind)
                }
                entry.Parameters.emplace<reflected_shader::entry_point::custom_type>();
                parameter_type.get_to(std::get<reflected_shader::entry_point::custom_type>(entry.Parameters));
            }
            else if (parameter_type_kind == "scalar"sv)
            {
                if (!std::holds_alternative<reflected_shader::entry_point::generated_type>(entry.Parameters))
                {
                    LOG_FATAL(LogSerialization, "Entry point parameter type kind [{}] is not supported with mixed custom types."
                        , parameter_type_kind)
                }
            }
            else
            {
                LOG_FATAL(LogSerialization, "Unsupported entry point parameter type kind [{}]."
                    , parameter_type_kind)
            }
        }
    }
    else
    {
        LOG_FATAL(LogSerialization, "Entry point parameters should be an array.")
    }

    if (j.at("bindings").is_array())
    {
        check(entry.Bindings.empty())
        j.at("bindings").get_to(entry.Bindings);
    }
    else
    {
        LOG_FATAL(LogSerialization, "Entry point bindings should be an array.")
    }
}

void from_json(json const& j, reflected_shader& shader)
{
    j.at("parameters").get_to(shader.Parameters);
    j.at("entryPoints").get_to(shader.EntryPoints);
    checkCode
    (
        vk::ShaderStageFlags Stages;
        for (auto& entry: shader.EntryPoints)
        {
            if (Stages & entry.Stage)
            {
                LOG_FATAL(LogSerialization, "Multiple entry points with stage [{}] are not supported."
                    , vk::to_string(entry.Stage))
            }
            Stages |= entry.Stage;
        }
    )
}

} /* ~Namespace rhi */
