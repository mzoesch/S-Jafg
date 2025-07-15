# Autoload Gdb to your project
To automatically load the Gdb customization script provided by Jafg for its custom data structures, you can:
- source it automatically in your `~/.gdbinit` file form your home directory by referencing the absolute path of the `.gdbinit` file inside the engine root directory or
- change your gdb configuration by adding the engine root's `.gdbinit` file as a trusted source, e.g. by appending the following line to your `~/.config/gdb/gdbinit` file: `add-auto-load-safe-path /absolute/path/to/the/jafg/root/.gdbinit`.


# Create own Gdb customization script
To create your own Gdb customization script, create a file named `GdbModuleEntry.py` in the `Debug` folder under the `Source` folder of your Jafg module. E.g. `Lal/Lal/Source/Debug/GdbModuleEntry.py`.
In this python module add a free function named `on_gdb_launch` and add your logic to it. This function will be called automatically when the Gdb customization script of Jafg is loaded.
The function does not receive any parameters and should return `None`.
