// Copyright mzoesch. All rights reserved.

///
/// Core header for shaders that are used by the Jafg::JShaderSubsystem and Jafg::JMaterialSubsystem.
///
#pragma once

/// JAFG_WITH_EDITOR: Always automatically defined as a compile time constant when used in editor builds.
#ifndef JAFG_WITH_EDITOR
    #define JAFG_WITH_EDITOR        0
#endif /* !JAFG_WITH_EDITOR */

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
/// @note All bindings have to be declared as shared.
///       All bindings in the set have to be declared with the same shared name.
///       While in theory the group could be inferred from only one binding, Jafg intentionally requires all bindings
///       to be marked as shared to avoid accidental mismatches.
///
[[__AttributeUsage(_AttributeTargets::Var)]]
struct Shared
{
    string group;
};

/// A frequency hint for how often a resource is expected to be updated.
enum EUpdateFrequency
{
    /// Only one resouce is created.
    Rarely,
    /// There are as many resources created as there are frames in flight.
    /// Therefore, each frame gets its own resouce and doesn't stall the device when updating.
    PerFrame,
}
static const EUpdateFrequency Rarely = EUpdateFrequency::Rarely;
static const EUpdateFrequency PerFrame = EUpdateFrequency::PerFrame;
[[__AttributeUsage(_AttributeTargets::Var)]]
struct UpdateFrequency
{
    EUpdateFrequency value;
}
