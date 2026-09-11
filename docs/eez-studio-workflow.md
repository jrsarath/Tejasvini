# EEZ Studio UI Workflow Guide

This document provides the standard workflow for editing, exporting, and integrating the LVGL graphical user interface for **Tejasvini UI Firmware**.

---

## 1. Project Location

The EEZ Studio project file is located at the root of the repository:
* **Project File**: [`ui.eez-project`](../ui.eez-project)
* **UI State File**: [`ui.eez-project-ui-state`](../ui.eez-project-ui-state)

---

## 2. Opening the Project in EEZ Studio

1. Download and install [EEZ Studio](https://www.envox.hr/eez/eez-studio/studio-introduction.html) (version `>= 0.10.x`).
2. Launch EEZ Studio.
3. Select **Open Project** and browse to the repository root.
4. Open [`ui.eez-project`](../ui.eez-project).
5. The workspace opens displaying the 240x400 portrait dashboard with components for temperature display, reflow profile progress, dials, and settings.

---

## 3. Export Configuration & Target Destination

The project is pre-configured to export generated C code directly into the CrowPanel UI firmware source tree:

* **Configured Destination**: `firmware/Tejasvini_UI/ui`
* **Target Firmware Binary**: `Tejasvini_UI` (`firmware/Tejasvini_UI/Tejasvini_UI.ino`)

The setting in `ui.eez-project` is:
```json
"destinationFolder": "firmware/Tejasvini_UI/ui"
```

---

## 4. Separation of Generated vs. Handwritten Files

To prevent generated code from overwriting application logic, the project enforces a strict boundary between automatically generated files and handwritten code:

### 4.1 Automatically Generated Files (`firmware/Tejasvini_UI/ui/`)
These files are **overwritten** whenever code is generated from EEZ Studio. **Do not edit these files manually**:
* `screens.c` / `screens.h`: Screen layouts, object widgets, and coordinate trees.
* `styles.c` / `styles.h`: Visual styles, padding, and theme colors.
* `images.c` / `images.h`: Bitmap image declarations and assets.
* `fonts.h` & `ui_font_fontawesome_*.c`: Font tables and glyph maps.
* `structs.h`: Struct definitions.
* `ui.c` / `ui.h`: LVGL initialization and tick dispatchers.
* `actions.h`: Generated C declarations for UI event callbacks.
* `vars.h`: Generated C declarations for UI variable getters and setters.

### 4.2 Handwritten Application & Bridge Files (`firmware/Tejasvini_UI/`)
These files reside **outside** the `ui/` directory and are **never touched by EEZ Studio**:
* [`UiActions.cpp`](../firmware/Tejasvini_UI/UiActions.cpp): Implements the event handlers declared in `ui/actions.h`. Delegates events to `UiBridge` and `ProtocolClient`.
* [`UiVars.cpp`](../firmware/Tejasvini_UI/UiVars.cpp): Implements all `get_var_...()` and `set_var_...()` declared in `ui/vars.h`. Reads state from `StatusModel`.
* [`UiBridge.h / .cpp`](../firmware/Tejasvini_UI/UiBridge.h): C-linkage bridge providing clean decoupling between LVGL and C++ application classes.
* [`StatusModel.h / .cpp`](../firmware/Tejasvini_UI/StatusModel.h): Cached telemetry state model populated by incoming UART `STATUS` packets.
* [`ProtocolClient.h / .cpp`](../firmware/Tejasvini_UI/ProtocolClient.h): Transmits high-level commands (`START_PROFILE`, `STOP`, `SET_TARGET`) to the RP2350B controller.
* [`DisplayManager.h / .cpp`](../firmware/Tejasvini_UI/DisplayManager.h): PicoDVI hardware driver and partial buffer engine.
* [`TouchManager.h / .cpp`](../firmware/Tejasvini_UI/TouchManager.h): GT911 touch coordinate driver.
* [`UiApp.h / .cpp`](../firmware/Tejasvini_UI/UiApp.h): Master UI lifecycle coordinator.
* [`Tejasvini_UI.ino`](../firmware/Tejasvini_UI/Tejasvini_UI.ino): Main Arduino sketch entry point.

---

## 5. Step-by-Step UI Modification & Export Procedure

Whenever you modify widgets, screens, fonts, or themes in EEZ Studio:

1. **Edit the UI**: Make visual changes in EEZ Studio's design editor.
2. **Build / Generate Code**:
   * Click the **Generate Code** button (or press `Ctrl+B` / `Cmd+B`).
   * Alternatively, select **Project** $\to$ **Build / Generate Code** from the menu.
3. **Verify Export Location**:
   * Inspect `git status` to verify modified files appear strictly in `firmware/Tejasvini_UI/ui/`:
     ```bash
     git status firmware/Tejasvini_UI/ui/
     ```
   * Confirm no files were written to obsolete directories or temporary folders.
4. **Hook Up New Actions or Variables (if added)**:
   * If you added a new Action in EEZ Studio, implement its callback in [`firmware/Tejasvini_UI/UiActions.cpp`](../firmware/Tejasvini_UI/UiActions.cpp).
   * If you added a new Variable in EEZ Studio, implement its getter/setter in [`firmware/Tejasvini_UI/UiVars.cpp`](../firmware/Tejasvini_UI/UiVars.cpp).
5. **Verify Compilation**:
   ```bash
   arduino-cli compile -b rp2040:rp2040:rpipico firmware/Tejasvini_UI/
   ```
