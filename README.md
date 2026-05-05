# z-fightClassicRE

ASI plugins for the classic PC releases of Resident Evil 2 and Resident Evil 3,
focused on model depth precision and polygon seam cleanup.

## Targets

### Resident Evil 2

- Upgrades requested z-buffer surfaces to 24/32-bit formats when possible.
- Forces stable z-test/z-write state around accepted 3D model draw calls.
- Runs the tuned invisible model depth prepass used by the current RE2 build.
- Keeps UV coordinate correction for PS1-style model texture seams.
- Keeps the targeted crow-wing depth exception from the current reference build.

### Resident Evil 3 Classic Rebirth

- Hooks the DirectDraw-to-D3D9 path used by Classic Rebirth.
- Creates and binds an owned D3D9 depth-stencil surface when the wrapper does
  not provide a usable one.
- Forces stable z-test/z-write state around accepted model draw calls.
- Clears the owned depth surface at the start of each scene.
- Keeps UV coordinate correction, while removing old CSV/callsite tracing,
  disabled depth-normalization experiments, and the disabled prepass path.
- Applies model-only subpixel stabilization and tiny per-draw depth bias to
  reduce shimmer without changing UI/background layers.
- Lets thin transparent model triangles use a softer z-tested path with very
  small screen expansion and a gentler depth bias, instead of rejecting every
  spike-like shape outright.

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Build a single target:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re2
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re3
```

By default the script uses `D:\Tools\zig\zig-x86_64-windows-0.16.0\zig.exe`.
Set `ZIG` to another `zig.exe` path if needed:

```powershell
$env:ZIG = 'C:\Tools\zig\zig.exe'
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Outputs:

- `re2_zfix.asi`
- `re3_zfix.asi`

Copy the matching ASI next to the game executable and load it through the
existing ASI loader.

## Notes

The RE3 plugin writes a compact `re3_zfix.log`. It no longer writes
`re3_zfix_diag.csv`.

An inventory-entry crash observed during testing produced minidumps whose frame
chain went through `msvcrt.dll -> bio3hd.asi -> ddraw.dll -> BIOHAZARD(R) 3 PC.exe`.
`re3_zfix.asi` was loaded, but was not present in that stack chain; the RE3 log
also showed `depthFail=0` and `ownedDepthFail=0`.
