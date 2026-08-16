# lfs
Jafg has a simple solution to handle large files in Git repositories.

By declaring a `.lfs` file in a Content dir of a plugin. You can store it automatically in a separate storage, and pull in when cmake runs your plugin's cmake script. By just:
```cmake
include_guard(_guard)
if(_guard)
    # <...>

    set(MY_PLUGIN_CONTENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Content" CACHE STRING "Content directory of MY_PLUGIN.")
    # As Jafg does it (@see Engine/CMakeLists.txt) it is recommended to query the version of your plugin and embedd it into the URL.
    set(MY_PLUGIN_CONTENT_SERVER "https://myserver.com" CACHE STRING "Remote content server URL for MY_PLUGIN content files.")
    HandleContentDependencies(
        "${MY_PLUGIN_CONTENT_DIR}"
        "${MY_PLUGIN_CONTENT_SERVER}"
        )
endif()
```

If a file does not exist but is specified in the `.lfs` file, it will be downloaded from the remote content server.
If a client specifies `JAFG_UPGRADE_CONTENT` as an environment variable, all missing or outdated content files will be downloaded.

By using the helper script `Programs/GenerateManifest.sh` on the server, the manifest/sha256's will automatically be generated. 

An `.lfs` file can have empty lines, or comments starting with `#`. Each line should contain a relative path to a file in the Content dir. See `~/Content/.lfs`. Each relative line must fully match `^[a-zA-Z\d\/\.\,\-\_]+$`.
