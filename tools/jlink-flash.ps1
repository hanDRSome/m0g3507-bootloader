#!pwsh
$ErrorActionPreference = 'Stop'

$JLink = 'D:/Assist/SEGGER/JLink_V926/JLink.exe'

$ProjectDir = Split-Path $PSScriptRoot
$TARGET = Get-ChildItem "$ProjectDir/build" -Recurse -Depth 3 -Filter '*.hex' -ea 0 | Select-Object -First 1

@"
loadfile $($TARGET.FullName)
r
g
exit
"@ | & $JLink -NoGui -device MSPM0G3507 -if SWD -speed 4000 -autoconnect 1 -usb -ExitOnError 1

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n=== 烧录成功 ===" -ForegroundColor Green
} else {
    Write-Host "`n=== 烧录失败 (错误码: $LASTEXITCODE) ===" -ForegroundColor Red
}
exit $LASTEXITCODE
