param(
  [ValidateSet('all', 're1', 're2', 're3', 'dc1')]
  [string]$Target = 'all'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 3.0

$BuildTargets = @{
  re2 = 're2_zfix'
  re1 = 're1_zfix'
  re3 = 're3_zfix'
  dc1 = 'dc1_zfix'
}

$DefaultTargetOrder = @('re2', 're1', 're3', 'dc1')

$zig = $env:ZIG
if (-not $zig) {
  $zig = 'D:\Tools\zig\zig-x86_64-windows-0.16.0\zig.exe'
}
if (-not (Test-Path -LiteralPath $zig)) {
  throw "Zig compiler not found. Set ZIG to zig.exe or edit build.ps1."
}

function Build-Asi {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Name
  )

  $src = Join-Path $PSScriptRoot "src\$Name.c"
  $out = Join-Path $PSScriptRoot "$Name.asi"
  & $zig cc -target x86-windows-msvc -shared -O2 `
    -D_CRT_SECURE_NO_WARNINGS `
    -Wall -Wextra -Werror `
    -Wno-deprecated-declarations `
    $src -o $out `
    -lkernel32 -luser32
  if ($LASTEXITCODE -ne 0) {
    throw "zig cc failed for $Name with exit code $LASTEXITCODE"
  }
  Get-Item $out
}

$targetsToBuild = if ($Target -eq 'all') { $DefaultTargetOrder } else { @($Target) }
foreach ($targetName in $targetsToBuild) {
  Build-Asi $BuildTargets[$targetName]
}
