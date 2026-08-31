// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Jxx.h"
#include "User/UserPreferencesForward.h"
#include "Rhi/Material.h"
#include "Nodes/ButtonBase.h"
#include "Nodes/Box.h"
#include "Nodes/TextBox.h"
#include "Framework/SupremePolicies.h"
#include "Framework/PhysicsCore.h"
#include "User/PreferenceCollection.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

class WEditor;

DECLARE_JAFG_CLASS(EJxxClassBits::Config, EJxxClassBits::Singleton)
class JUserPreferences final : public JCxxClass
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JUserPreferences)

public:

    virtual void BeginLife() override;

#if JAFG_WITH_LOCAL_LAYER
    //#
    //# The default collection of user preferences.
    //# Jafg will add its preferences here. A plugin should add their own preferences to this collection.
    //# They will then be easily editable by the user from centralized user preferences widgets.
    //#
    TArray<LPreferenceCollection> TopLevelCollections;
#endif /* JAFG_WITH_LOCAL_LAYER */

    enum { UnlimitedTps = 0 };

    ///////////////////////////////////////////////////////////////////////////////
    // Audio
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TClampedPreference<f32> MasterVolume{ 1.0f, 0.0f, 1.0f };
    CLASS_FIELD(Config)
    TClampedPreference<f32> MusicVolume{ 1.0f, 0.0f, 1.0f };
    CLASS_FIELD(Config)
    TClampedPreference<f32> MiscVolume{ 1.0f, 0.0f, 1.0f };
    CLASS_FIELD(Config)
    TClampedPreference<f32> VoiceVolume{ 1.0f, 0.0f, 1.0f };

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

    //# System dependent. But should generally stay consistent.
    CLASS_FIELD(Config)
    TPreference<LString> PreferredPhysicalDevice;
    //# System dependent. But should generally stay consistent.
    CLASS_FIELD(Config)
    TPreference<LString> PreferredMonitor;
    //# Preferred bits per pixel. Zero means default. This is a legacy feature as most compositors straight up ignore this.
    CLASS_FIELD(Config)
    TPreference<LVec3i32> PreferredBpp{ maths::zero_vector<LVec3i32> };
    //# Preferred resolution in pixel. Zero means default. This is a legacy feature as most compositors straight up ignore this.
    CLASS_FIELD(Config)
    TPreference<LVec2i32> PreferredResolutionPx{ maths::zero_vector<LVec2u32> };
    //# Preferred refresh rate in hz. Zero means default. This is a legacy feature as most compositors straight up ignore this.
    CLASS_FIELD(Config)
    TPreference<i32> PreferredRefreshRateHz{ 0 };

    CLASS_FIELD(Config)
    TPreference<rhi::present_mode> DesiredPresentMode{ rhi::present_mode::Fifo };
    //#
    //# The cap for ticks per second.
    //# If Jafg is running with the local layer then this preference only has an effect if the present mode
    //# is not blocking.
    //#
    CLASS_FIELD(Config)
    TPreference<i32> MaxTps{ UnlimitedTps };

    CLASS_FIELD(Config)
    TPreference<EPolygonMode> PolygonMode{ EPolygonMode::Fill };

    CLASS_FIELD(Config)
    TPreference<bool> EditorPerspectiveDepthTestHint{ true };
    //# If set, this material should be preferred by mesh renders to use.
    std::optional<LMaterialInstanceRef> EditorMeshMaterialPreference;
    //# Might not be supported in all configurations.
    bool bHighlightFontRects{};
    LColor FontRectHighlightColor{ 0xFF000030_color };

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TPreference<f32> InterfaceGamma{ 2.4f };

    CLASS_FIELD(Config)
    TPreference<EApplicationScale> ApplicationScaleMode{ EApplicationScale::Double };

    CLASS_FIELD(Config)
    TPreference<bool> bInvertVerticalScrollWheel{ true };
    CLASS_FIELD(Config)
    TPreference<bool> bInvertHorizontalScrollWheel{ true };
    CLASS_FIELD(Config)
    TPreference<f32> MouseWheelScrollSpeed{ 35.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeSingle    { 15.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeSingle { 14.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeSingle      { 11.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeSingle   { 10.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeSingle     {  9.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeDouble    { 19.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeDouble { 17.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeDouble      { 12.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeDouble   { 11.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeDouble     { 10.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeTriple    { 21.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeTriple { 19.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeTriple      { 13.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeTriple   { 12.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeTriple     { 10.0f };

    CLASS_FIELD(Config)
    TPreference<LColor> BackgroundColor         { 0x15 };
    CLASS_FIELD(Config)
    TPreference<LColor> AccentColor             { 0x1C };
    CLASS_FIELD(Config)
    TPreference<LColor> ForegroundColor         { 0x24 };
    CLASS_FIELD(Config)
    TPreference<LColor> ForegroundColorVariant  { 0x2F };
    CLASS_FIELD(Config)
    TPreference<LColor> InputColor              { 0x0F };
    CLASS_FIELD(Config)
    TPreference<LColor> OverlayColor            { 0x38 };
    CLASS_FIELD(Config)
    TPreference<LColor> DisabledColor           { 0x1B };
    CLASS_FIELD(Config)
    TPreference<LColor> TextColor               { 0xFF };

    CLASS_FIELD(Config)
    TPreference<LStylePalette> PrimaryPaletteSolid{{
        .Normal=  {.Tint=0x3C97D3FF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xE0E0E0FF_color},
        .Hover=   {.Tint=0x2980BAFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Press=   {.Tint=0x2D6C97FF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Selected={.Tint=0x337AB7FF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Disabled={.Tint=0x8F8F8FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0x404040FF_color},
        }};
    CLASS_FIELD(Config)
    TPreference<LStylePalette> PrimaryPaletteLine{{
        .Normal=  {.Tint=Colors::Transparent, .Outline=0x3C97D3FF_color, .OutlineThickness=1u, .TextTint=0x3C97D3FF_color},
        .Hover=   {.Tint=0x2980BA30_color,    .Outline=0x2980BAFF_color, .OutlineThickness=1u, .TextTint=0x2980BAFF_color},
        .Press=   {.Tint=0x2D6C9730_color,    .Outline=0x2D6C97FF_color, .OutlineThickness=1u, .TextTint=0x2D6C97FF_color},
        .Selected={.Tint=0x337AB760_color,    .Outline=0x337AB7FF_color, .OutlineThickness=1u, .TextTint=0x337AB7FF_color},
        .Disabled={.Tint=0x14141460_color,    .Outline=0x141414FF_color, .OutlineThickness=1u, .TextTint=0x303030FF_color},
        }};
    CLASS_FIELD(Config)
    TPreference<LStylePalette> PalePrimaryPaletteSolid{{
        .Normal=  {.Tint=0x40576FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xE0E0E0FF_color},
        .Hover=   {.Tint=0x2C3234FF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Press=   {.Tint=0x2D6C97FF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Selected={.Tint=0x335c88FF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Disabled={.Tint=0x8F8F8FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0x404040FF_color},
        }};
    CLASS_FIELD(Config)
    TPreference<LStylePalette> PalePrimaryPaletteLine{{
        .Normal=  {.Tint=Colors::Transparent, .Outline=0x40576FFF_color, .OutlineThickness=1u, .TextTint=0x40576FFF_color},
        .Hover=   {.Tint=0x2C323430_color,    .Outline=0x464E51FF_color, .OutlineThickness=1u, .TextTint=0x2C3234FF_color},
        .Press=   {.Tint=0x2D6C9730_color,    .Outline=0x2D6C97FF_color, .OutlineThickness=1u, .TextTint=0x2D6C97FF_color},
        .Selected={.Tint=0x337AB760_color,    .Outline=0x335c88FF_color, .OutlineThickness=1u, .TextTint=0x335c88FF_color},
        .Disabled={.Tint=0x14141460_color,    .Outline=0x141414FF_color, .OutlineThickness=1u, .TextTint=0x303030FF_color},
        }};
    CLASS_FIELD(Config)
    TPreference<LStylePalette> SecondaryPaletteSolid{{
        .Normal=  {.Tint=0x2F2F2FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xC0C0C0FF_color},
        .Hover=   {.Tint=0x3F3F3FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Press=   {.Tint=0x4F4F4FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Selected={.Tint=0x4F4F4FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0xFFFFFFFF_color},
        .Disabled={.Tint=0x0F0F0FFF_color, .Outline=Colors::Black, .OutlineThickness=0u, .TextTint=0x808080FF_color},
        }};
    CLASS_FIELD(Config)
    TPreference<LStylePalette> SecondaryPaletteLine{{
        .Normal=  {.Tint=Colors::Transparent, .Outline=0x3E3E3EFF_color, .OutlineThickness=1u, .TextTint=0x3E3E3EFF_color},
        .Hover=   {.Tint=0x3F3F3F30_color,    .Outline=0x3F3F3FFF_color, .OutlineThickness=1u, .TextTint=0x3F3F3FFF_color},
        .Press=   {.Tint=0x4F4F4F30_color,    .Outline=0x4F4F4FFF_color, .OutlineThickness=1u, .TextTint=0x4F4F4FFF_color},
        .Selected={.Tint=0x4F4F4F60_color,    .Outline=0x4F4F4FFF_color, .OutlineThickness=1u, .TextTint=0x4F4F4FFF_color},
        .Disabled={.Tint=0x0F0F0F60_color,    .Outline=0x0F0F0FFF_color, .OutlineThickness=1u, .TextTint=0xE0E0E0FF_color},
        }};

    static constexpr LVec4F InputRadii{ 5.0f };
    static constexpr LPadding InputPadding{ 5_spt, 0};
    CLASS_FIELD(Config)
    TPreference<LStylePalette> InputPaletteSolid{{
        .Normal=  {.Tint=0x0F0F0FFF_color, .Outline=0x5F5F5FFF_color, .OutlineThickness=1u, .TextTint=0xFFFFFFFF_color},
        .Hover=   {.Tint=0x0F0F0FFF_color, .Outline=0x8F8F8FFF_color, .OutlineThickness=1u, .TextTint=0xFFFFFFFF_color},
        .Press=   {.Tint=0x0F0F0FFF_color, .Outline=0x8F8F8FFF_color, .OutlineThickness=1u, .TextTint=0xFFFFFFFF_color},
        .Selected={.Tint=0x0F0F0FFF_color, .Outline=0x8F8F8FFF_color, .OutlineThickness=1u, .TextTint=0xFFFFFFFF_color},
        .Disabled={.Tint=0x0F0F0FFF_color, .Outline=0x5F5F5FFF_color, .OutlineThickness=1u, .TextTint=0x808080FF_color},
        }};

    CLASS_FIELD(Config)
    TPreference<LColor> ProximityColorA         { 0x1A };
    CLASS_FIELD(Config)
    TPreference<LColor> ProximityColorB         { 0x15 };
    NODISCARD FORCEINLINE constexpr LColor GetProximityColor(std::integral auto Index) const noexcept
    {
        if (Index % 2)
        {
            return *this->ProximityColorA;
        }
        return *this->ProximityColorB;
    }
    NODISCARD LStylePalette ProximityPaletteSolid(std::integral auto Index, LStylePalette const& Template) const noexcept
    {
        LStylePalette Result{Template};
        Result.Normal.Tint = this->GetProximityColor(Index);
        return Result;
    }

    CLASS_FIELD(Config)
    TPreference<LColor> DangerColor             { LColor{0x94, 0x33, 0x33} };

    CLASS_FIELD(Config)
    TPreference<std::size_t> PreferredDragPadding{ 3 };
    CLASS_FIELD(Config)
    TPreference<std::size_t> PreferredDragOverlap{ 2 };
    CLASS_FIELD(Config)
    TPreference<LColor> ViewportBackgroundTint  { Colors::Black };

    ///////////////////////////////////////////////////////////////////////////////
    // Foreign plugins
    ///////////////////////////////////////////////////////////////////////////////

    //#
    //# Additional plugin search paths that are used to fetch plugin info metadata.
    //#
    CLASS_FIELD(Config)
    TPreference<TArray<LString>> AdditionalPluginsSearchPaths;

    //#
    //# The plugins that are loaded when the engine loads.
    //#
    CLASS_FIELD(Config)
    TPreference<TArray<LString>> EnabledEnginePlugins{ {"JafgGameplayCore"} };

    ///////////////////////////////////////////////////////////////////////////////
    // Storage
    ///////////////////////////////////////////////////////////////////////////////

    //#
    //# Additional saves search paths that are used to fetch saves info metadata.
    //# The default ist Saved/Saves.
    //#
    CLASS_FIELD(Config)
    TPreference<TArray<LString>> AdditionalSavesSearchPaths;

    ///////////////////////////////////////////////////////////////////////////////
    // Editor
    ///////////////////////////////////////////////////////////////////////////////

    //# The editor if active.
    WEditor* Editor{};

    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintX{ Colors::Crimson };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintY{ Colors::DeepSkyBlue };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintZ{ Colors::LimeGreen };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintHighlighted{ Colors::Yellow };

    CLASS_FIELD(Config)
    TPreference<bool> EditorSortDirectoriesFirst{ true };

    CLASS_FIELD(Config)
    TPreference<LString> EditorLastWorldName{ "Editor World" };
    CLASS_FIELD(Config)
    TPreference<TSubclassOf<ASupremePolicies>> EditorLastWorldSupremePolicies;
    CLASS_FIELD(Config)
    TPreference<bool> EditorAutoLaunchLastWorld;
    CLASS_FIELD(Config)
    TPreference<LString> EditorLastObjectCollection{ "Gym" };
    CLASS_FIELD(Config)
    TPreference<bool> EditorShowRate{ true };
    CLASS_FIELD(Config)
    TPreference<LPath> EditorLastLayout{ "Config/DefaultEditorLayout.json" };

    CLASS_FIELD(Config)
    TPreference<f32> EditorActorFocusProjectedForwardMultiplier{ 1.4f };
    CLASS_FIELD(Config)
    TPreference<f32> EditorActorFocusTransitionDuration{ 0.08f };

    CLASS_FIELD(Config)
    TPreference<bool> EditorShowEyeTranslation;

    CLASS_FIELD(Config)
    TPreference<LVec2u32> EditorPieDimensions{ LVec2u32{855, 475} };
    CLASS_FIELD(Config)
    TPreference<std::byte> EditorPawnStart{ std::byte{} };
    CLASS_FIELD(Config)
    TPreference<std::byte> EditorStartType{ std::byte{} };

    //# ---------- Core Visualization ----------

    CLASS_FIELD(Config)
    TPreference<f32> EditorVisualizationMaxDebugTextRenderDistance{ 50.0f };
    //# If the resulting font size is blow this limit it will be discarded.
    CLASS_FIELD(Config)
    TPreference<f32> EditorVisualizationDebugTextFontThreshold{ 2.5f };

    CLASS_FIELD(Config)
    TPreference<bool> EditorVisualizeAabbs;
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAabbVisualizationTint{ Colors::Green };
    CLASS_FIELD(Config)
    TPreference<bool> EditorVisualizeTransitiveAabbs;
    CLASS_FIELD(Config)
    TPreference<LColor> EditorTransitiveAabbVisualizationTint{ Colors::Blue };

    CLASS_FIELD(Config)
    TPreference<f32> EditorTraceLength{ 500.0f };
    CLASS_FIELD(Config)
    TPreference<bool> EditorVisualizeTraces;
    CLASS_FIELD(Config)
    TPreference<LColor> EditorTraceVisualizationTint{ Colors::Blue };
    CLASS_FIELD(Config)
    TPreference<f32> EditorTraceVisualizationDuration{ 30.0f };
    CLASS_FIELD(Config)
    TPreference<bool> EditorVisualizeTraceHits;
    CLASS_FIELD(Config)
    TPreference<LColor> EditorTraceHitVisualizationTint{ Colors::Red };
    CLASS_FIELD(Config)
    TPreference<f32> EditorTraceHitVisualizationDuration{ 30.0f };
    CLASS_FIELD(Config)
    TPreference<bool> EditorVisualizeGizmoInteractions;
    CLASS_FIELD(Config)
    TPreference<LColor> EditorGizmoVisualizationTint{ Colors::Red };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorGizmoVisualizationHitTint{ Colors::Green };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorGizmoVisualizationAabbTint{ Colors::Crimson };
    CLASS_FIELD(Config)
    TPreference<f32> EditorGizmoVisualizationLength{ 500.0f };

    //# ---------- Physics Visualization ----------

    CLASS_FIELD(Config)
    //# Draw the GetSupport() function, used for convex collision detection.
    TPreference<bool> EditorVisualizeGetSupportFunction;
    CLASS_FIELD(Config)
    //# When drawing the support function, also draw which direction mapped to a specific support point.
    TPreference<bool> EditorVisualizeSupportDirection;
    CLASS_FIELD(Config)
    //# Draw the faces that were found colliding during collision detection.
    TPreference<bool> EditorVisualizeGetSupportingFace;
    CLASS_FIELD(Config)
    //# Draw the shapes of all bodies
    TPreference<bool> EditorVisualizeShape;
    CLASS_FIELD(Config)
    //# When #EditorVisualizeShape is true and this is true, the shapes will be drawn in wireframe instead of solid.
    TPreference<bool> EditorVisualizeShapeWireframe;
    //# Coloring scheme to use for shapes.
    CLASS_FIELD(Config)
    TPreference<Physx::EShapeColor> EditorVisualizeShapeColor{Physx::EShapeColor::MotionType};
    //# Draw a bounding box per body.
    CLASS_FIELD(Config)
    TPreference<bool> EditorVisualizeBoundingBox;
    CLASS_FIELD(Config)
    //# Draw the center of mass for each body.
    TPreference<bool> EditorVisualizeCenterOfMassTransform;
    CLASS_FIELD(Config)
    //# Draw the world transform (which can be different than the center of mass) for each body.
    TPreference<bool> EditorVisualizeWorldTransform;
    CLASS_FIELD(Config)
    //# Draw the velocity vector for each body.
    TPreference<bool> EditorVisualizeVelocity;
    CLASS_FIELD(Config)
    //# Draw the mass and inertia (as the box equivalent) for each body.
    TPreference<bool> EditorVisualizeMassAndInertia;
    CLASS_FIELD(Config)
    //# Draw stats regarding the sleeping algorithm of each body.
    TPreference<bool> EditorVisualizeSleepStats;
    CLASS_FIELD(Config)
    //# Draw the vertices of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyVertices;
    CLASS_FIELD(Config)
    //# Draw the velocities of the vertices of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyVertexVelocities;
    CLASS_FIELD(Config)
    //# Draw the edge constraints of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyEdgeConstraints;
    CLASS_FIELD(Config)
    //# Draw the bend constraints of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyBendConstraints;
    CLASS_FIELD(Config)
    //# Draw the volume constraints of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyVolumeConstraints;
    CLASS_FIELD(Config)
    //# Draw the skin constraints of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodySkinConstraints;
    CLASS_FIELD(Config)
    //# Draw the LRA constraints of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyLRAConstraints;
    CLASS_FIELD(Config)
    //# Draw the rods of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyRods;
    CLASS_FIELD(Config)
    //# Draw the rod states (orientation and angular velocity) of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyRodStates;
    CLASS_FIELD(Config)
    //# Draw the rod bend twist constraints of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyRodBendTwistConstraints;
    CLASS_FIELD(Config)
    //# Draw the predicted bounds of soft bodies.
    TPreference<bool> EditorVisualizeSoftBodyPredictedBounds;
    //# Coloring scheme to use for soft body constraints.
    CLASS_FIELD(Config)
    TPreference<Physx::ESoftBodyConstraintColor> EditorVisualizeSoftBodyConstraintColor{ Physx::ESoftBodyConstraintColor::ConstraintType };
};

} /* ~Namespace Jafg */
