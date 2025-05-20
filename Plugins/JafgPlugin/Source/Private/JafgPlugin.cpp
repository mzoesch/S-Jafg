// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"

namespace Jafgp
{

class LJafgPluginLifetime final : public Jafg::LPluginLifetime
{
};

DEFINE_PLUGIN(JAFGPLUGIN_API, LJafgPluginLifetime, JafgPlugin)

} /* ~Namespace Jafgp */
