# z-fightClassicRE

ASI plugins for classic PC releases of Resident Evil, Resident Evil 2,
Resident Evil 3, and Dino Crisis. The active scope is intentionally narrow:
fix model z-fighting and keep the Resident Evil 1 startup disclaimer plugin.

Image smoothing, framebuffer scaling, UV seam cleanup, screen expansion,
contact-shadow polish, cutout-alpha polish, and other visual experiments are
not part of the active build.

## Targets

### Resident Evil 1

- Builds `re1_zfix.asi`.
- Upgrades requested z-buffer surfaces to higher precision formats when the
  wrapper/game path allows it.
- Forces stable z-test/z-write state around accepted model draw calls.
- Runs an invisible model depth prepass and a color pass with depth writes
  disabled.
- Uses model callsite profiles and adaptive depth conflict resolution for flat
  or near-flat model batches.
- Keeps the legacy Direct3D 1 execute-buffer hook needed by some RE1 builds for
  model/shadow depth discipline.
- Keeps D3D1 foreground mask and overlay texture pages out of the z-fight
  state patch, so background masks keep their original draw-order behavior.

### Resident Evil 2

- Builds `re2_zfix.asi`.
- Upgrades requested z-buffer surfaces to higher precision formats when
  possible.
- Forces stable z-test/z-write state around accepted model draw calls.
- Runs an invisible model depth prepass and a color pass with depth writes
  disabled.
- Uses model callsite profiles and adaptive depth conflict resolution for flat
  or near-flat model batches.
- Guards D3D2 mask-like overlay quads on classic or hires mask texture pages so
  they bypass the model z-fight path.

### Resident Evil 3 Classic Rebirth

- Builds `re3_zfix.asi`.
- Hooks the Classic Rebirth DirectDraw-to-D3D9 path.
- Creates and binds an owned D3D9 depth-stencil surface when the wrapper does
  not provide a usable one.
- Clears the owned depth surface at scene boundaries.
- Forces stable z-test/z-write state around accepted model draw calls.
- Runs the model depth prepass/color pass strategy.
- Uses adaptive depth conflict resolution for model batches.

### Dino Crisis 1

- Builds `dc1_zfix.asi`.
- Uses the same Classic Rebirth Direct3D9 depth path as the RE3 plugin.
- Creates and binds an owned D3D9 depth-stencil surface when needed.
- Forces stable z-test/z-write state around accepted model draw calls.
- Runs the model depth prepass/color pass strategy.
- Uses adaptive depth conflict resolution for model batches.

### Resident Evil 1 Startup Disclaimer

- Builds `re1_startup_disclaimer.asi`.
- This target is intentionally kept even though it is separate from model
  z-fighting.

## Build

Build all active targets:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Build a single target:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re1
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Target re1_disclaimer
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

- `re1_zfix.asi`
- `re1_startup_disclaimer.asi`
- `re2_zfix.asi`
- `re3_zfix.asi`
- `dc1_zfix.asi`

## Diagnostics

Logging is disabled by default to avoid disk I/O and extra diagnostic work in
the draw hooks. To enable logs for debugging, create an empty `enable_log.txt`
file next to the installed `.asi`, then launch the game again. Remove
`enable_log.txt` after debugging to return to the low-overhead path.

## Usage

Use only the plugin that matches the game you are running:

- Resident Evil 1: `re1_zfix.asi`
- Resident Evil 2: `re2_zfix.asi`
- Resident Evil 3: `re3_zfix.asi`
- Dino Crisis 1: `dc1_zfix.asi`

The startup disclaimer is separate: install `re1_startup_disclaimer.asi` only
when you want that RE1 startup screen behavior.

### Installation

1. Build the required plugin, or use a prebuilt ASI from a release package.
2. Open the game installation folder.
3. Copy the matching `.asi` file into the same folder as the game executable.
4. Make sure the game already has an ASI loader installed. Classic Rebirth and
   most RE-ENHANCE packages usually include one; otherwise install a compatible
   ASI loader for the game first.
5. Start the game normally.

Do not install multiple game-specific zfix plugins into one game folder. Each
game should only load its own matching ASI.

### Verifying That The Plugin Loaded

Logs are only created when `enable_log.txt` is present next to the plugin.
With logging enabled, after launching the game check the game folder for the
matching log file:

- `re1_zfix.log`
- `re2_zfix.log`
- `re3_zfix.log`
- `dc1_zfix.log`

If the log file appears and contains startup lines, the ASI was loaded. If no
log file is created while `enable_log.txt` exists, the ASI loader did not load
the plugin, the plugin is in the wrong folder, or the wrong plugin was copied
for that game.

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
that can be used to tune a safer per-game or per-model depth profile.
