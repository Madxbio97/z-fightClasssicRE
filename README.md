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

## Usage

These fixes are distributed as separate ASI plugins. Use only the plugin that
matches the game you are running:

- Resident Evil 1: `re1_zfix.asi`
- Resident Evil 2: `re2_zfix.asi`
- Resident Evil 3: `re3_zfix.asi`

### Installation

1. Build the required plugin, or use a prebuilt ASI from a release package.
2. Open the game installation folder.
3. Copy the matching `.asi` file into the same folder as the game executable.
4. Make sure the game already has an ASI loader installed. Classic Rebirth and
   most RE-ENHANCE packages usually include one; otherwise install a compatible
   ASI loader for the game first.
5. Start the game normally.

Do not install all three plugins into one game folder. Each game should only
load its own matching ASI.

### Per-Game Notes

For Resident Evil 1, install `re1_zfix.asi`. The plugin is tuned for the older
DirectDraw/Direct3D path and also handles the legacy Direct3D 1 execute-buffer
path used by some builds. It writes `re1_zfix.log` next to the executable.

For Resident Evil 2, install `re2_zfix.asi`. This build contains the most tuned
model-depth path, including the crow-oriented adaptive depth profile, UV seam
cleanup, alpha cutout handling, and model depth prepass. It writes
`re2_zfix.log`.

For Resident Evil 3 Classic Rebirth, install `re3_zfix.asi`. This build hooks
the Classic Rebirth DirectDraw-to-D3D9 path, creates a stable depth surface when
needed, and applies the RE3-specific model stabilization settings. It writes
`re3_zfix.log`.

### Verifying That The Plugin Loaded

After launching the game, check the game folder for the matching log file:

- `re1_zfix.log`
- `re2_zfix.log`
- `re3_zfix.log`

If the log file appears and contains startup lines, the ASI was loaded. If no
log file is created, the ASI loader did not load the plugin, the plugin is in
the wrong folder, or the wrong plugin was copied for that game.

### Updating Or Removing

To update a fix, close the game and replace the old `.asi` with the new one.
Keeping a backup such as `re2_zfix.asi.bak` is recommended.

To disable a fix, rename the plugin so it no longer ends with `.asi`, for
example:

```text
re2_zfix.asi.disabled
```

### Troubleshooting

If the game crashes on startup, first remove or rename the plugin and confirm
the game starts without it. Then check that the correct ASI was installed for
the correct game.

If models still flicker or show depth artifacts, keep the matching log file and
test the same scene again. The logs contain draw-call and profile diagnostics
that can be used to tune a safer per-game or per-model profile.

## Experimental Model Polish Branch

The `codex/experimental-model-polish` branch contains a more aggressive visual
experiment for Resident Evil 1, Resident Evil 2, and Resident Evil 3.

Resident Evil 1 and Resident Evil 2 get the full experimental stack:

- adaptive room-style relighting based on the current model luma;
- stronger screen-space directional/rim lighting;
- half-pixel geometry stabilization for small or unstable model batches;
- contact-shadow polishing for dark translucent blob-like draws;
- sharper cutout alpha handling for hair, grates, feathers, and similar edges.

Resident Evil 3 gets the safer subset: adaptive relighting and half-pixel model
stabilization. The aggressive transparent/cutout path is intentionally not
re-enabled there because earlier RE3 testing showed that transparent fixes can
reintroduce z-fighting in some scenes.

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
