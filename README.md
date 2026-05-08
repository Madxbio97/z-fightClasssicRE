# z-fightClassicRE

ASI plugins for classic PC releases of Resident Evil, Resident Evil 2,
Resident Evil 3, and Dino Crisis, focused on model depth precision and polygon
seam cleanup.

## Targets

### Resident Evil 2

- Upgrades requested z-buffer surfaces to 24/32-bit formats when possible.
- Forces stable z-test/z-write state around accepted 3D model draw calls.
- Runs an invisible model depth prepass, followed by a color pass with z-write
  disabled and profile-controlled z-test.
- Uses texture-aware UV correction for PS1-style model texture seams.
- Uses callsite profiles and an adaptive depth conflict resolver for flat or
  near-flat model batches, including the RE2 crow texture profile.
- Classifies alpha model draws as cutout or translucent: cutout geometry uses
  alpha test plus depth write, while translucent draws keep z-test without
  writing depth.
- Applies conservative vertex-color relighting and small cutout expansion.

### Resident Evil 1

- Builds as a separate `re1_zfix.asi` target based on the RE2 DirectDraw path.
- Uses the RE2 model depth prepass/color pass, UV correction, adaptive depth
  conflict resolver, alpha classification, and vertex-color relighting baseline.
- Does not require the RE2 model callsite range, so the same geometry filters
  can work against the different Resident Evil 1 executable.
- Also hooks the old Direct3D 1 HAL-device execute-buffer path used by some
  Resident Evil 1 builds, so translucent blob shadows keep z-test discipline.
- Writes a separate `re1_zfix.log`; tune any future RE1-specific profiles from
  that log instead of mixing them into the RE2 build.

### Resident Evil 3 Classic Rebirth

- Hooks the DirectDraw-to-D3D9 path used by Classic Rebirth.
- Creates and binds an owned D3D9 depth-stencil surface when the wrapper does
  not provide a usable one.
- Forces stable z-test/z-write state around accepted model draw calls.
- Clears the owned depth surface at the start of each scene.
- Runs the same model depth prepass/color pass strategy used by the current
  RE3 build.
- Uses texture-aware UV correction, alpha cutout/translucent classification,
  adaptive depth conflict resolution, and conservative vertex-color relighting.
- Removes old water, shadow, sampler-filter, screen-expansion, CSV tracing, and
  depth-normalization experiments from the active code path.

### Dino Crisis 1

- Builds as a separate `dc1_zfix.asi` target for the same Classic Rebirth
  Direct3D9 path used by the RE3 plugin.
- Uses the model depth prepass/color pass, UV correction, alpha
  cutout/translucent classification, adaptive depth conflict resolution, and
  vertex-color relighting baseline.
- Starts with a generic Dino model profile; tune per-callsite profiles from
  `dc1_zfix.log` after checking real rooms.

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Build a single target:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re2
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re1
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re3
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target dc1
```

By default the script uses `D:\Tools\zig\zig-x86_64-windows-0.16.0\zig.exe`.
Set `ZIG` to another `zig.exe` path if needed:

```powershell
$env:ZIG = 'C:\Tools\zig\zig.exe'
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Outputs:

- `re2_zfix.asi`
- `re1_zfix.asi`
- `re3_zfix.asi`
- `dc1_zfix.asi`

Copy the matching ASI next to the game executable and load it through the
existing ASI loader.

## Notes

Each plugin writes a compact runtime log next to the game executable:

- `re2_zfix.log`
- `re1_zfix.log`
- `re3_zfix.log`
- `dc1_zfix.log`

The RE3 plugin no longer writes `re3_zfix_diag.csv`.

An inventory-entry crash observed during testing produced minidumps whose frame
chain went through `msvcrt.dll -> bio3hd.asi -> ddraw.dll -> BIOHAZARD(R) 3 PC.exe`.
`re3_zfix.asi` was loaded, but was not present in that stack chain; the RE3 log
also showed `depthFail=0` and `ownedDepthFail=0`.
