# z-fightClassicRE

ASI plugins for the classic PC releases of Resident Evil 2 and Resident Evil 3,
plus Dino Crisis 1, focused on model depth precision and polygon stability.

## Targets

### Resident Evil 2

- Upgrades requested z-buffer surfaces to 24/32-bit formats when possible.
- Forces stable z-test/z-write state around accepted 3D model draw calls.
- Runs the tuned invisible model depth prepass used by the current RE2 build.
- Keeps RE2 model depth adjustment focused on the active bias/prepass path;
  the disabled depth-normalization experiment has been removed for release.
- Uses a less brittle color pass after the RE2 depth prepass and only treats
  vertex alpha as transparent when alpha blending is actually enabled, which
  helps custom opaque player models that carry non-255 lighting alpha.
- Keeps UV coordinate correction for PS1-style model texture seams.
- Keeps the targeted crow-wing depth exception from the current reference build,
  but requires a confirmed texture trace so modded model textures do not
  accidentally match it by handle alone.

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

### Dino Crisis 1

- Uses the same DirectDraw-to-D3D9 hook family as the RE3 Classic Rebirth
  target, but ships as a separate `dc1_zfix.asi`.
- Treats the game as a polygonal 3D scene instead of a prerender-background
  game: the screen/rhw limits are wider so level geometry can enter the stable
  geometry path.
- Leaves the incoming TL vertices as 32-bit float pass-through data: no
  per-vertex snap, half-pixel offset, UV snap, or depth bias is applied.
- Does not upgrade, replace, clear, or bias the depth buffer in the DC1 build;
  it only forces linear texture filtering around accepted 3D draw calls.

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Build a single target:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re2
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
- `re3_zfix.asi`
- `dc1_zfix.asi`

Copy the matching ASI next to the game executable and load it through the
existing ASI loader.

## Notes

The RE3 plugin writes a compact `re3_zfix.log`. It no longer writes
`re3_zfix_diag.csv`.

An inventory-entry crash observed during testing produced minidumps whose frame
chain went through `msvcrt.dll -> bio3hd.asi -> ddraw.dll -> BIOHAZARD(R) 3 PC.exe`.
`re3_zfix.asi` was loaded, but was not present in that stack chain; the RE3 log
also showed `depthFail=0` and `ownedDepthFail=0`.
