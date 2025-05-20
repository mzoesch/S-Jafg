# For foreign plugins

When declaring a foreign plugin, you have to:
 - Link against the `~Engine/Engine` by adding it to your `Module.public_dependencies`.
 - Create a `.cpp` file in your plugin, include the `Freign/ForeignInclude.h` file and add the `DEFINE_PLUGIN('YourName')`.

You can see an example at `~/Plugins/JafgPlugin/Source/Private/JafgPlugin.cpp`.
