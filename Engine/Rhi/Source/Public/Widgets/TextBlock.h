// Copyright mzoesch. All rights reserved.

#pragma once

#include "WidgetNode.h"
#include "TextBlock.generated.h"

namespace Jafg
{

class Shader;
class LViewport;

DECLARE_JAFG_CLASS()
class RHI_API WTextBlock final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBlock)

public:

    // WWidgetNode implementation
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LViewport* Context) const override;
    // ~WWidgetNode implementation

private:

    void DrawTextImpl() const;

    Shader* FontShaderProgram = nullptr;
};

} /* ~Namespace Jafg */
