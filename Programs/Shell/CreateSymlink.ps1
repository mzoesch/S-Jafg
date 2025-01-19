# Copyright mzoesch. All rights reserved.

param (
    [string]$TargetPath = $(throw "TargetPath is required"),
    [string]$ShortcutPath = $(throw "ShortcutPath is required")
)

$WScriptShell = New-Object -ComObject WScript.Shell
$Shortcut = $WScriptShell.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = (Resolve-Path $TargetPath).Path
$Shortcut.Save()
