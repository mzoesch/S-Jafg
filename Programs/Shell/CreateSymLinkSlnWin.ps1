$TargetPath = ".\Saved\SolutionWin\Jafg.sln"
$ShortcutPath = ".\JafgWin.sln.lnk"

$WScriptShell = New-Object -ComObject WScript.Shell
$Shortcut = $WScriptShell.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = (Resolve-Path $TargetPath).Path
$Shortcut.Save()
