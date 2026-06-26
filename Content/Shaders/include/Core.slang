// Copyright mzoesch. All rights reserved.

///
/// Core header for shaders that are used by the Jafg::JShaderSubsystem and Jafg::JMaterialSubsystem.
///
#pragma once

///
/// Bridge for a struct to mark its equivalent in CXX.
/// Usage:
///   [[CxxName("Namespace::StructName")]]
///   struct MyStruct{ ... };
///
[[__AttributeUsage(_AttributeTargets::Struct)]]
struct CxxName
{
    string name;
};

///
/// Marks a binding *layout* as shared between multiple shaders.
/// Usage:
///   [[vk::binding(0, ?),Shared("Group")]] Struct[] A;
///   [[vk::binding(1, ?),Shared("Group")]] Struct[] B;
///
/// @note All bindings in the set have to be declared with the same shared name.
///
[[__AttributeUsage(_AttributeTargets::Var)]]
struct Shared
{
    string group;
};

enum EUpdateFrequency
{
    Rarely,
    PerFrame,
}
static const EUpdateFrequency Rarely = EUpdateFrequency::Rarely;
static const EUpdateFrequency PerFrame = EUpdateFrequency::PerFrame;

[[__AttributeUsage(_AttributeTargets::Var)]]
struct UpdateFrequency
{
    EUpdateFrequency value;
}
