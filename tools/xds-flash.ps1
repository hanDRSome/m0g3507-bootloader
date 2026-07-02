#!pwsh
$ErrorActionPreference = 'Stop'

$DSLite = 'D:/Dev/CCS/ccs/ccs_base/DebugServer/bin/DSLite.exe'

$ProjectDir = Split-Path $PSScriptRoot
$TARGET = Get-ChildItem "$ProjectDir/build" -Recurse -Depth 3 -Filter '*.hex' -ea 0 | Select-Object -First 1
$CCXML = "$PSScriptRoot/MSPM0G3507.ccxml"


# & $DSLite flash --config $CCXML -e -f $TARGET.FullName
& $DSLite flash --config $CCXML -s "FlashEraseSelection=Do not erase Flash memory" -f $TARGET.FullName


if ($LASTEXITCODE -eq 0) {
    Write-Host "`n=== 烧录成功 ===" -ForegroundColor Green
}
else {
    Write-Host "`n=== 烧录失败 (错误码: $LASTEXITCODE) ===" -ForegroundColor Red
}
exit $LASTEXITCODE