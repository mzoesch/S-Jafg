// Copyright mzoesch. All rights reserved.

#include "Widgets/ClassInspector.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VParent.h"
#include "Nodes/TextBox.h"
#include "Nodes/GenericTabInfos.h"
#include "User/UserPreferences.h"

void Jafg::WClassInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WParent* Container;
    BeginStyling(*this).StaticRoot<WScrollRegion>()
        .Tint(*Prefs.ForegroundColor)
        [NewStaticNode(WVParent).SaveTo(&Container).Anchor(EAnchor::Fill)];

    Detail::LRegistryClassPackage const& Package{(*algo::find_if_checked(Detail::GetGlobalCxxRecordRegistry().GetRegisteredPackages(),
        [](auto const& Package) { return Package->IsClass() && Package->AsClass().StaticClass.IsRoot(); }))->AsClass()};
    auto const& Class{Package.StaticClass};
    check(Class.IsRoot())

    auto Idx{0uz};
    auto AddElement{[&](LJxxClass const& Child, std::size_t Indent)
    {
        Container->AddChild(NewStaticNode(WTextBox)
            .Anchor(EAnchor::HFill)
            .Tint((!!(Idx++ % 2)) ? *Prefs.ProximityColorA : *Prefs.ProximityColorB)
            .Content(SprintF("{}{}", LString(Indent*8, ' '), Child.GetFullyQualifiedName()))
            .Unique());
    }};
    auto AddChild{[AddElement](this const auto& Self, LJxxClass const& Element, std::size_t Indent) -> void
    {
        AddElement(Element, Indent);
        TArray Children{Element.GetChildren()};
        algo::sort(Children, [](auto const& A, auto const& B)
        {
            return A->GetFullyQualifiedName() < B->GetFullyQualifiedName();
        });
        for (auto const& Child : Children)
        {
            Self(*Child, Indent + 1);
        }
        return;
    }};

    AddChild(Class, 0);

    return;
}
