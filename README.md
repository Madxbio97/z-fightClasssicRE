# z-fightClassicRE

Resident Evil 2 Classic PC ASI plugin focused on model depth precision and
polygon seam cleanup for the DirectDraw/Direct3D 2 renderer.

## Current scope

- Upgrades requested z-buffer surfaces to 24/32-bit formats when possible.
- Forces stable z-test/z-write state around accepted 3D model draw calls.
- Runs an invisible model depth prepass before the visible color pass.
- Keeps the tuned UV coordinate correction for PS1-style model texture seams.
- Keeps the targeted crow-wing depth exception from the current reference build.

Removed from this clean baseline:

- DX9 replay/overlay experiments.
- PGXP experiment code.
- Bilinear filtering and mask/background filtering experiments.
- Texture CRC scanning, HD texture probing, surface dumps, and wireframe debug code.

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

By default the script uses `D:\Tools\zig\zig-x86_64-windows-0.16.0\zig.exe`.
Set `ZIG` to another `zig.exe` path if needed:

```powershell
$env:ZIG = 'C:\Tools\zig\zig.exe'
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

The output is `re2_zfix.asi` in the project root. Copy it to the game directory
next to the executable and load it through the existing ASI loader.
