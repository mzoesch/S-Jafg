# For foreign plugins

When declaring a foreign plugin, you have to:
- Link against the `~Engine/Engine` with `jafg_add_dependency("Engine/Engine" "public")` or any other publicly derived plugin.
- Create a `.cpp` file in your plugin, include the `Freign/ForeignInclude.h` file and ...
  - ... inherit from `Jafg::LPluginLifetime` (and optionally use the provided virtual functions to add important functionally during lifetime milestones).  
  - ... add the `DEFINE_PLUGIN(YourApi, YourLifetime, YourName)` below.

You can see an example at `~/Plugins/Jgc/Source/Private/Plugin.cpp`.
