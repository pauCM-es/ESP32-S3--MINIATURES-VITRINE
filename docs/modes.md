# Modes & options management

This document describes how the firmware’s **mode menu** works (top-level modes, per-mode options, and how actions are triggered).

It intentionally focuses only on mode/menu logic. For broader firmware context, see:

- [Project context](esp_32_s_3_smart_vitrine_project_context.md)
- [Firmware architecture](esp_32_s_3_smart_vitrine_firmware_architecture_platform_io.md)
- [Implementation steps](implementation-steps.md)

## Concepts

There are two separate “indices” in the firmware:

- **Miniature index**: the currently focused miniature position (rotary encoder navigation).
- **Menu focus index**: the currently highlighted entry in a menu (modes list or options list).

These must not interfere with each other.

## Entry point

The mode menu is opened with the dedicated mode button:

- `BTN_MODE` press handling lives in [src/main.cpp](../src/main.cpp)
- It calls `ModeManager::selectMainMode(...)`

## Where modes are defined

Modes are currently defined as a static array:

- `struct Mode { name, options[], numOptions }`
- `const Mode MODES[] = { ... }`
- `NUM_MODES`

Defined in: [src/hardware/ModeManager.h](../src/hardware/ModeManager.h)

## Menu flow

High-level call flow:

1. `ModeManager::selectMainMode(...)` builds a temporary `modeNames[]` array from `MODES[i].name`
2. It forwards to `ModeManager::selectMode(modeNames, NUM_MODES, ...)`
3. After selection, `main.cpp` calls `ModeManager::handleModeOptions(modeIndex)`
4. `handleModeOptions(...)` shows `MODES[modeIndex].options[]` and triggers the chosen action

Implementation: [src/hardware/ModeManager.cpp](../src/hardware/ModeManager.cpp)

## Menu rendering (anti-flicker)

Rendering is done by:

- `TFTDisplayControl::showOptions(options, numOptions, focusIndex)` in
  [src/hardware/DisplayControl.cpp](../src/hardware/DisplayControl.cpp)

Important: menu loops **must not** redraw every tick, or the TFT will visibly flicker.

Current behavior: `ModeManager` only calls `showOptions(...)` when `focusIndex` changes.

## Encoder behavior while in menus

The rotary encoder normally wraps around `MAX_MINIATURES`. In a menu, it must wrap by the number of entries in that menu.

This is done via:

- `EncoderControl::checkMovementWithWrap(int wrapMax)`
- `EncoderControl::setCurrentIndex(int)`

See: [src/hardware/EncoderControl.h](../src/hardware/EncoderControl.h)

### Saving/restoring miniature index

When entering a menu, the firmware saves the current miniature index, resets the encoder index to `0` for menu focus, and restores the saved miniature index when leaving the menu.

This prevents a mode menu from changing which miniature is currently selected.

## Adding a new mode (current structure)

1. Add a new entry to `MODES[]` in [src/hardware/ModeManager.h](../src/hardware/ModeManager.h)
2. Extend `ModeManager::handleModeOptions(modeIndex)` in
   [src/hardware/ModeManager.cpp](../src/hardware/ModeManager.cpp) to perform actions for the new mode/options
3. Keep actions short and always redraw/restore to the main screen on exit

## Recommended refactor (when modes grow)

When mode actions expand, move toward:

- `src/modes/<ModeName>.cpp`: mode-specific option/action code
- `src/modes/modes_registry.cpp`: single place where mode list is registered
- Keep `ModeManager` as the menu/input orchestrator

This keeps `ModeManager` small and makes modes easier to extend and test.
