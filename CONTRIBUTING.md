# Contributing to Tejasvini

Thank you for your interest in contributing to **Tejasvini**! Tejasvini is an open-source heatplate controller designed for SMD/SMT soldering and reflow work, featuring a dual-MCU architecture: a custom RP2350B machine controller and an RP2040-powered Elecrow 4.3" Pico DVI display terminal (CrowPanel).

Because this firmware controls high-temperature physical heating hardware (400W PTC heatplate, mains voltage Solid State Relays), software quality and safety are of paramount importance. Please take a few minutes to read these guidelines before submitting code.

---

## 1. Safety-First Principle

> [!CAUTION]
> **Heater Safety is Non-Negotiable**:
> Firmware in this repository controls mains-powered heatplates capable of exceeding 280°C. A bug or deadlock in the control loop can cause fire, toxic fume generation, or destruction of hardware.
>
> Any contribution that modifies `firmware/Tejasvini_Controller/`, `firmware/shared/`, or sensor acquisition must adhere to the following principles:
> 1. **Controller is Sole Authority**: The UI (CrowPanel) is an unprivileged terminal. All safety checks, thermal regulation, and actuator control belong strictly to `Tejasvini_Controller`.
> 2. **Fail-Safe Heater OFF**: Any detected sensor anomaly, invalid ADC value, sensor divergence, communication timeout, or unhandled fault must immediately force the heater output LOW and latch into an error state.
> 3. **Latching Faults**: Fault states cannot automatically re-enable the heater. Clearing a fault must require an explicit user command (`CLEAR_FAULT`), and heating must remain idle until explicitly re-triggered.
> 4. **Never Remove Safety Bounds**: Do not remove `MAX_DUTY_CYCLE` (40%), `OVERTEMP_SHUTDOWN` (280°C), `NTC_MIN_VALID_TEMP` (-20°C), `NTC_MAX_VALID_TEMP` (300°C), or thermal runaway detection.
> 5. **Communication Watchdog**: During active heating, if no valid packet is received across UART within 5,000ms (`COMM_TIMEOUT_MS`), the controller must immediately shut off the heater.
> 6. **Hardware Safety Required**: Firmware safety checks do NOT replace physical hardware protection (thermal fuse, grounding, fuse).

---

## 2. Development Setup

### 2.1 Toolchain Requirements
* **Microcontroller Core:** Earle F. Philhower Raspberry Pi Pico / RP2040 / RP2350 Arduino core (`rp2040` version 3.x+ / 4.x+).
* **CLI Tool:** `arduino-cli` (version 1.x+) or Arduino IDE 2.x.
* **Compiler:** `arm-none-eabi-gcc` (bundled with Earle Philhower core or ARM GNU Toolchain 12+).
* **Host Testing:** CMake 3.16+ and GCC/Clang with CTest.

### 2.2 Required Arduino Libraries (for UI Firmware)
The following libraries must be installed in your Arduino libraries directory:
* `PicoDVI` (Adafruit fork for RP2040 DVI output)
* `Adafruit_GFX` & `Adafruit_BusIO`
* `lvgl` (version 8.3.x compatible with EEZ Studio UI)
* `TAMC_GT911` (for capacitive touchscreen support)

### 2.3 Compiling Microcontroller Firmware
You can verify each firmware target independently using `arduino-cli`:

**1. Machine Controller (RP2350B):**
```bash
arduino-cli compile \
  -b rp2040:rp2040:rpipico2 \
  firmware/Tejasvini_Controller/
```

**2. Display UI Terminal (RP2040):**
```bash
arduino-cli compile \
  -b rp2040:rp2040:rpipico \
  --build-property "build.extra_flags=-DLV_LVGL_H_INCLUDE_SIMPLE -DLV_USE_OBJ_NAME=1 -DLV_USE_TRANSLATION=1 -Wall -Wextra" \
  firmware/Tejasvini_UI/
```

### 2.4 Running Host Test Suite
```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## 3. Repository Architecture

```text
Tejasvini/
├── CMakeLists.txt            # Host CTest test suite build definition
├── LICENSE                   # MIT License
├── README.md                 # Main user & hardware guide
├── CONTRIBUTING.md           # Contributor guidelines (this file)
├── CODE_OF_CONDUCT.md        # Contributor Covenant Code of Conduct
├── AGENTS.md                 # Developer & AI agent architectural guide
├── ui.eez-project            # EEZ Studio LVGL project source file
│
├── firmware/
│   ├── shared/               # Shared protocol, types, errors, configuration
│   │   ├── Config.h          # Shared timing and safety bounds
│   │   ├── Types.h / .cpp    # State, profile, and stage definitions
│   │   ├── ErrorCodes.h/.cpp # Fault codes and severities
│   │   ├── ProtocolVersion.h # Semantic protocol version
│   │   ├── Protocol.h / .cpp # Packet structures and parser
│   │   └── Serialization.h/.cpp # Key-value serialization
│   │
│   ├── Tejasvini_Controller/ # Machine Controller firmware (RP2350B)
│   │   ├── Tejasvini_Controller.ino # Controller entry point
│   │   ├── ControllerConfig.h# Pinout, PI gains, safety limits
│   │   ├── ControllerApp.h/.cpp # Loop coordinator
│   │   ├── MachineStateMachine.h/.cpp # State machine
│   │   ├── ThermalManager.h/.cpp # PI control & safety checks
│   │   ├── HeaterController.h/.cpp # SSR time-proportioning (max 40%)
│   │   └── ...
│   │
│   └── Tejasvini_UI/         # UI Firmware Target (CrowPanel RP2040)
│       ├── Tejasvini_UI.ino  # UI entry point
│       ├── UiConfig.h        # Display, touch, and UART pin assignments
│       ├── UiApp.h / .cpp    # UI lifecycle manager
│       ├── DisplayManager.h/.cpp # PicoDVI & LVGL bridge
│       ├── TouchManager.h/.cpp # GT911 touch driver
│       ├── ProtocolClient.h/.cpp # UART client & telemetry monitor
│       ├── UiBridge.h / .cpp # C-linkage bridge to UI events and variables
│       ├── UiActions.cpp     # Handwritten EEZ action implementations
│       ├── UiVars.cpp        # Handwritten EEZ variable getters/setters
│       └── ui/               # Strictly EEZ Studio generated LVGL screens & styles
│
├── docs/                     # Technical documentation & specifications
└── test/                     # Host automated unit & simulation tests
```

---

## 4. Coding Standards & Expectations

1. **No Magic Numbers**: All hardware pins, timing intervals, sensor constants, PID gains, and thresholds MUST be declared as descriptive `constexpr` symbols in `ControllerConfig.h`, `UiConfig.h`, or `shared/Config.h`.
2. **Resource-Constrained RP2040 Hygiene (UI)**:
   * RP2040 has 264 KB SRAM. The PicoDVI framebuffer occupies ~153.6 KB contiguous dynamic allocation. Keep static `.bss` allocations minimal and avoid large stack allocations.
   * Avoid heavy C++ standard library abstractions (`std::string`, `std::vector`, heap churn).
   * Prefer fixed-size buffers with `snprintf` / `strncpy` and explicit null-termination.
3. **Pin Configuration**:
   * Input pins connected to switches or encoder contacts MUST use `pinMode(pin, INPUT_PULLUP)` to prevent floating inputs.
   * Output pins controlling physical actuators MUST initialize safe output states (e.g. `digitalWrite(PIN_SSR, LOW)`) before enabling `pinMode(OUTPUT)`.
4. **Non-Blocking Execution**:
   * Never call `delay()` in any control loop or message handler. All tasks must be cooperative and time-sliced via `millis()` or timer counters.
5. **Compiler Warnings**:
   * Code must compile cleanly with `-Wall -Wextra` without introducing new compiler warnings.
6. **EEZ Studio UI Separation**:
   * Generated files belong strictly in `firmware/Tejasvini_UI/ui/`.
   * Handwritten event actions and variable binding implementations belong in `firmware/Tejasvini_UI/UiActions.cpp` and `firmware/Tejasvini_UI/UiVars.cpp`. Never write handwritten code inside `ui/` files that get regenerated.

---

## 5. Testing & Validation Expectations

Before opening a pull request, perform the following validation steps:

1. **Host Automated Tests**:
   Run `ctest --test-dir build --output-on-failure`. All protocol, serialization, state machine, and simulation tests must pass.
2. **Firmware Compilation Check**:
   Confirm that both `Tejasvini_Controller` and `Tejasvini_UI` compile cleanly with 0 errors.
3. **Safety Interlock Testing**:
   If your change affects sensor acquisition or heating:
   * **Open Circuit**: Disconnected NTC must trip safety shutdown in $< 100\text{ms}$.
   * **Short Circuit**: Shorted NTC input must trip safety shutdown in $< 100\text{ms}$.
   * **Divergence**: Sensor delta $> 35^\circ\text{C}$ during heating must trip safety shutdown.
   * **Thermal Runaway**: Failure to rise under sustained power must trip safety shutdown within 18s.
   * **Comm Watchdog**: Loss of UART telemetry for $> 5\text{s}$ during heating must trip immediate heater shutdown.

---

## 6. How to Submit Changes

1. **Fork the Repository** on GitHub.
2. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/my-cool-improvement
   ```
3. **Commit with Clear Messages**:
   * Use concise, descriptive commit subjects (e.g. `thermal: harden raw adc bounds check`, `docs: update pinout mapping in hardware guide`).
4. **Push to Your Fork** and open a Pull Request against the `main` branch.
5. **Fill out the PR Template**: Detail the rationale, safety impact assessment, and testing performed.

---

## 7. Issue & Community Guidelines

* **Bug Reports**: Use the [Bug Report Template](.github/ISSUE_TEMPLATE/bug_report.md) and include hardware details, firmware commit, and serial telemetry logs.
* **Feature Requests**: Use the [Feature Request Template](.github/ISSUE_TEMPLATE/feature_request.md) and outline motivation and hardware resource impact.
* **Electrical/Hardware Questions**: Use the [Hardware Issue Template](.github/ISSUE_TEMPLATE/hardware_issue.md).

All interactions are governed by our [Code of Conduct](CODE_OF_CONDUCT.md). Thank you for helping build safe, reliable open-source hardware tools!
