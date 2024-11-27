$TargetPath = ".\Saved\Solution\Jafg.sln"
$ShortcutPath = ".\Jafg.sln.lnk"

$WScriptShell = New-Object -ComObject WScript.Shell
$Shortcut = $WScriptShell.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = (Resolve-Path $TargetPath).Path
$Shortcut.Save()
