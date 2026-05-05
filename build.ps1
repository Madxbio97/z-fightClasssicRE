$ErrorActionPreference = 'Stop'

$zig = $env:ZIG
if (-not $zig) {
  $zig = 'D:\Tools\zig\zig-x86_64-windows-0.16.0\zig.exe'
}
if (-not (Test-Path -LiteralPath $zig)) {
  throw "Zig compiler not found. Set ZIG to zig.exe or edit build.ps1."
}

$src = Join-Path $PSScriptRoot 'src\re2_zfix.c'
$out = Join-Path $PSScriptRoot 're2_zfix.asi'
& $zig cc -target x86-windows-msvc -shared -O2 `
  -D_CRT_SECURE_NO_WARNINGS `
  -Wall -Wextra -Werror `
  -Wno-deprecated-declarations `
  $src -o $out `
  -lkernel32 -luser32
if ($LASTEXITCODE -ne 0) {
  throw "zig cc failed with exit code $LASTEXITCODE"
}
Get-Item $out
