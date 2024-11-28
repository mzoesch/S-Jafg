$TargetPath = ".\Saved\SolutionWasm\Jafg.sln"
$ShortcutPath = ".\JafgWasm.sln.lnk"

$WScriptShell = New-Object -ComObject WScript.Shell
$Shortcut = $WScriptShell.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = (Resolve-Path $TargetPath).Path
$Shortcut.Save()
