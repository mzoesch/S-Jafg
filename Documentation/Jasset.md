# JAsset (.jasset)

This file type is used by Jafg for its automatic analysis on dependencies. It will find all jassets in the engine paths and clients can use it to quickly and type safely query for it and load said assets. 

The file format specification can be found at `Content/Asset.hexpat`. You may inspect / edit jassets outside of the Jafg editor with xxd or with the mentioned specification in ImHex.

# Update content
After a remote content update you should update all content assets with:
```bash
JAFG_UPGRADE_CONTENT=1 cmake -B build
```
Note, that content is not per build configuration, but project wide. Updating one build dir will affect all other builds too. If you want separate content you will have to use different jafg root dirs.

In case of an invalid local state you may also force an update for all files:
```bash
JAFG_FORCE_CONTENT_UPDATE=1 cmake -B build
```
