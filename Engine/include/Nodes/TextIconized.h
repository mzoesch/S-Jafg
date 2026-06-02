// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Text.h"
#include "Nodes/Iconized.h"
#include "TextIconized.generated.h"

namespace Jafg
{

class WTextIconizedLeft;
class WTextIconizedRight;
typedef TFactoryIconized<WTextIconizedLeft> LFactoryTextIconizedLeft;
typedef TFactoryIconized<WTextIconizedRight> LFactoryTextIconizedRight;

//#
//# A text node that also has the ability to display an icon on the left hand side.
//# Unlike textures. An icon only has a width and does, therefore, not affect the height of this node.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextIconizedLeft)
class ENGINE_API WTextIconizedLeft : public WText, public LIconized
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTextIconizedLeft)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

private:

    NODISCARD LVec2F GetIconTopLeft(LVec2F Translation) const noexcept
    {
        return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
            + IconLocation.GetRelativeLeftIconTopLeft
                <&WTextIconizedLeft::Icon, &WTextIconizedLeft::IconBrush>(*this, this->TextBrush.Padding);
    }
};

//#
//# A text node that also has the ability to display an icon on the right hand side.
//# Unlike textures. An icon only has a width and does, therefore, not affect the height of this node.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextIconizedRight)
class ENGINE_API WTextIconizedRight : public WText, public LIconized
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTextIconizedRight)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

private:

    NODISCARD LVec2F GetIconTopLeft(LVec2F Translation) const noexcept
    {
        return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
            + IconLocation.GetRelativeRightIconTopLeft
                <&WTextIconizedRight::Icon, &WTextIconizedRight::IconBrush>(*this, this->TextBrush.Padding);
    }
};

} /* ~Namespace Jafg */
