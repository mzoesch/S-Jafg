// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Rhi/PhysicalRendering.h"
#include "PbrMaterialInspector.generated.h"

namespace Jafg
{

class WParent;
class WInput_LargeList;
class JMaterialSubsystem;

//#
//# Inspects a pbr material: Its parameters, textures, sampling and flags. Everything is editable and visible from the
//# next frame on to everything that is drawn with the material. Sliders span the typical values (the norm) of a
//# parameter, typed values may use its whole range. Pbr instances are shared per material (@see
//# JMaterialSubsystem::GetPbrInstance), so this edits the material for all of its users. Every field can be reset to
//# the value of the template of the material. Changes are not written back to the material file.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WPbrMaterialInspector final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WPbrMaterialInspector)
    {
        this->SetShouldTick(true);
    }

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Pbr Material Inspector", JafgInfo, "Icons/Sphere")

    virtual void Construct() override;
    virtual void Tick(f32 Dt) override;

    //# Shows this material. Null shows nothing.
    void Inspect(LPbrMaterialInstanceRef InInstance);
    NODISCARD FORCEINLINE constexpr LPbrMaterialInstanceRef const& GetInspected() const noexcept { return this->Instance; }

    //# What the rows edit. Everything but the textures, which are bound right away.
    typedef LPbrMaterialParameters LEditState;

private:

    void Rebuild();

    JMaterialSubsystem* Subsystem{};
    LPbrMaterialInstanceRef Instance;

    WInput_LargeList* Selector{};
    WParent* Container{};

    //# What the rows edit. Its address is stable while the rows exist. Null if nothing is inspected.
    std::unique_ptr<LEditState> Edit;
    struct LEntry final
    {
        LString Name;
        LPluginInfo Namespace{ nullptr };
        LString Identifier;
    };
    //# All pbr materials that can be selected.
    TArray<LEntry> Entries;
};
static_assert(CTabSelectorCandidate<WPbrMaterialInspector>);
static_assert(CTabCandidate<WPbrMaterialInspector>);

} /* ~Namespace Jafg */
