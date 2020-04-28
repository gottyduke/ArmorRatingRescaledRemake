# ~args
$config = $args[0]

# ~null
if ($config.Equals("")) {
    Write-Host "        Invocation with Invalid Configuration Argument!"
    Exit
}

# ~curr
$ProjDir = Split-Path -Path $script:MyInvocation.MyCommand.Path
$Project = pwd | Select-Object | %{$_.ProviderPath.Split("\")[-1]}

# ~path
$MO2Dir = "D:\Program Files (x86)\Steam\steamapps\common\Skyrim Special Edition\MO2\mods"
$GitDir = Join-Path -Path ([Environment]::GetFolderPath("Desktop")) -ChildPath "Workspace"
$SKSE64 = "SKSE\Plugins"

# ~join
$MO2Path = Join-Path -Path "$MO2Dir\$Project" -ChildPath $SKSE64
$GitPath = Join-Path -Path "$GitDir\$Project" -ChildPath $SKSE64
$Product = "D:\WorkSpace\skse64\x64\$config"

Write-Host "        Current Project <<< $Project >>>"

# ~touch
New-Item -Type dir $MO2Path -Force | Out-Null
New-Item -Type dir $GitPath -Force | Out-Null

# ~json
if (Test-Path "$ProjDir\$Project.json" -PathType Leaf) {
    Write-Host "        Found project json file."
    Copy-Item "$ProjDir\$Project.json" "$MO2Path\$Project.json" -Force
    Copy-Item "$ProjDir\$Project.json" "$GitPath\$Project.json" -Force
}

# ~exec
Copy-Item "$Product\$Project.dll" "$MO2Path\$Project.dll" -Force
Copy-Item "$Product\$Project.dll" "$GitPath\$Project.dll" -Force

# ~dbg
if ($config.Equals("Debug")) {
    Copy-Item "$Product\$Project.dbg" "$MO2Path\$Project.dbg" -Force
    Copy-Item "$Product\$Project.dbg" "$GitPath\$Project.dbg" -Force
}

Write-Host "        Finished Updating Project."