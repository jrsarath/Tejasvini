# AGENTS.md — Developer & AI Agent Guide for Tejasvini

This document provides developer guidelines, architectural rules, hardware specifications, and codebase navigation for AI agents and human developers working on **Tejasvini**.

---

## 1. Project Overview

**Tejasvini** is an open-source heatplate controller designed for SMD/SMT soldering and reflow work. The firmware is structured into **two independently buildable targets** with **one shared protocol specification**:

1. **Tejasvini UI Firmware (`firmware/Tejasvini_UI/`)**:
   * **MCU:** Raspberry Pi Pico (RP2040 dual ARM Cortex-M0+)
   * **Display:** 400x240 RGB display output driven via PicoDVI (`DVIGFX16`, 2x scaled to 800x480 on Elecrow CrowPanel RTD2281, rendered in 240x400 Portrait Mode)
   * **Touch:** GT911 Capacitive Touchscreen (I2C: GP20 SDA, GP21 SCL)
   * **Role:** Pure HMI terminal; sends high-level commands, displays telemetry, has zero safety authority.
2. **Tejasvini Controller Firmware (`firmware/Tejasvini_Controller/`)**:
   * **MCU:** Custom RP2350B machine controller board
   * **Heater Element:** 400W PTC Heatplate driven via a 3.3V Logic Solid State Relay (SSR, GPIO 22)
   * **Temperature Sensors:** Redundant 100K NTC Thermistors (NTC1: ADC0/GP26, NTC2: ADC1/GP27, NTC3: ADC2/GP28)
   * **Cooling Fans:** Dual 25 kHz 4-wire PWM fan control with tachometer pulse RPM capture
   * **Role:** Sole machine authority and safety supervisor.

---

## 2. Directory Structure & File Map

```text
Tejasvini/
├── CMakeLists.txt            # Root CMake build definition (builds host test suite)
├── LICENSE                   # MIT License
├── README.md                 # Project documentation & hardware BOM
├── CONTRIBUTING.md           # Contribution & coding standards guide
├── CODE_OF_CONDUCT.md        # Contributor Covenant Code of Conduct
├── AGENTS.md                 # Developer & AI agent architectural guide (this file)
├── ui.eez-project            # EEZ Studio LVGL project source file
│
├── firmware/
│   ├── shared/               # Shared protocol, error codes, and configuration
│   │   ├── Config.h          # Shared timing, temperature bounds, baud rate
│   │   ├── Types.h / .cpp    # Machine states, reflow profiles, stage enums
│   │   ├── ErrorCodes.h/.cpp # Fault codes and severity levels
│   │   ├── ProtocolVersion.h # Protocol versioning constants
│   │   ├── Protocol.h / .cpp # Command, status, and response packet definitions
│   │   └── Serialization.h/.cpp # Wire formatting and key-value tokenizers
│   │
│   ├── Tejasvini_UI/         # UI Firmware Target (CrowPanel RP2040)
│   │   ├── Tejasvini_UI.ino  # UI entry point
│   │   ├── UiConfig.h        # Display, touch, and UART pin assignments
│   │   ├── UiApp.h / .cpp    # UI lifecycle manager
│   │   ├── DisplayManager.h/.cpp # PicoDVI & LVGL 8.3 display bridge
│   │   ├── TouchManager.h/.cpp   # GT911 touch coordinate driver
│   │   ├── StatusModel.h/.cpp    # Cached state model for LVGL variables
│   │   ├── ProtocolClient.h/.cpp # Non-blocking UART client & heartbeat monitor
│   │   ├── UiBridge.h / .cpp # C-linkage bridge to UI events and variables
│   │   └── ui/               # EEZ Studio generated LVGL UI screens and styles
│   │
│   └── Tejasvini_Controller/ # Machine Controller Target (RP2350B)
│       ├── Tejasvini_Controller.ino # Controller entry point
│       ├── ControllerConfig.h# RP2350B pinout, safety thresholds, PI gains
│       ├── ControllerApp.h/.cpp # Subsystem coordinator & loop scheduler
│       ├── MachineStateMachine.h/.cpp # State machine (BOOTING, IDLE, HEATING, etc.)
│       ├── ProfileEngine.h/.cpp # Profile parameters & reflow curves
│       ├── ThermalManager.h/.cpp# Closed-loop PI regulator & soft-start ramp
│       ├── TemperatureManager.h/.cpp # Multi-channel NTC acquisition & checks
│       ├── NtcSensor.h / .cpp# Steinhart-Hart / Beta conversion & filtering
│       ├── HeaterController.h/.cpp # Time-proportioning SSR actuation (max 40%)
│       ├── FanController.h/.cpp # Dual fan PWM drive and cooling management
│       ├── Tachometer.h / .cpp # Dual fan tachometer pulse capture / RPM
│       ├── EncoderManager.h/.cpp # Rotary encoder decoding & button debouncing
│       ├── BuzzerManager.h/.cpp # Audible alerts & alarm patterns
│       ├── ArgbManager.h / .cpp # Visual status LED lighting cues
│       ├── UartTransport.h/.cpp # Non-blocking UART reader & transmitter
│       ├── CommandParser.h/.cpp # Inbound command validation & dispatch
│       ├── StatusPublisher.h/.cpp # Periodic STATUS telemetry broadcaster
│       └── WatchdogManager.h/.cpp # Hardware watchdog supervisor
│
├── docs/                     # Technical specifications & documentation
│   ├── firmware-architecture.md # Two-target architecture & module flow
│   ├── uart-protocol.md      # Wire framing, commands, responses & examples
│   ├── safety-behavior.md    # Safety rules, interlocks & timeout matrix
│   ├── hardware-pin-map.md   # Pinout mapping for RP2040 and RP2350B
│   ├── build-instructions.md # Compilation, upload, and testing guide
│   ├── hardware_and_safety.md# Physical protections & wiring guide
│   └── calibration_and_tuning.md # PI loop tuning & sensor calibration
│
├── test/                     # Host automated unit & simulation tests
│   ├── CMakeLists.txt        # CTest build definition
│   ├── protocol/             # Protocol parsing and command validation tests
│   ├── serialization/        # Status packet round-trip serialization tests
│   ├── state-machine/        # State transition and guard tests
│   └── integration/          # End-to-end command/status simulation tests
│
└── assets/                   # Hardware 3D models & CAD reference
    └── 3d files/             # STL enclosure models & aluminum heatplate reference
```

---

## 3. Physical UART Interconnect & Pin Mapping

```text
CrowPanel GP0 / UART0 TX  ───(Soldered 3.3V wire)───>  Controller UART RX / GP33
CrowPanel GP1 / UART0 RX  <───(Soldered 3.3V wire)───  Controller UART TX / GP32
CrowPanel GND             ───────────────────────────  Controller GND
```

### 3.1 CrowPanel Terminal (RP2040)
* `GP0`: UART0 TX
* `GP1`: UART0 RX
* `GP12-19`: PicoDVI differential pairs
* `GP20`: Touch I2C SDA
* `GP21`: Touch I2C SCL
* `GP24`: Backlight Control (Active LOW)
* `GP25`: Touch INT
* `GP29`: Touch RST

### 3.2 Machine Controller (RP2350B)
* `GP32`: UART TX
* `GP33`: UART RX
* `GP22`: PIN_SSR (3.3V Logic time-proportioning)
* `GP23`: PIN_RELAY (Safety isolation relay)
* `GP26`: PIN_NTC1 (ADC0, 100kΩ divider)
* `GP27`: PIN_NTC2 (ADC1, 100kΩ divider)
* `GP28`: PIN_NTC3 (ADC2, 100kΩ divider)
* `GP2`: PIN_ENA (Rotary encoder Phase A)
* `GP3`: PIN_ENB (Rotary encoder Phase B)
* `GP4`: PIN_EBT (Rotary encoder Button)
* `GP6`: PIN_FAN1_PWM (25 kHz fan drive)
* `GP7`: PIN_FAN1_TACH (Tachometer pulse capture)
* `GP8`: PIN_FAN2_PWM (25 kHz fan drive)
* `GP9`: PIN_FAN2_TACH (Tachometer pulse capture)
* `GP10`: PIN_BUZZER (PWM tone output)
* `GP11`: PIN_ARGB (WS2812B visual status)

---

## 4. Software Architecture & Safety Rules

### 4.1 Cooperative Non-Blocking Loop
* Each firmware target runs a cooperative non-blocking loop in its `.ino` file. No blocking `delay()` calls are allowed.

### 4.2 Machine Authority & Safety Invariants (CRITICAL)
* **The Controller is the Sole Authority**: UI never decides whether heating is safe.
* **Immediate Raw Bounds Validation**: Raw ADC ($50 \le \text{ADC} \le 4050$) and raw temperatures ($-20^\circ\text{C}$ to $300^\circ\text{C}$) are checked before filtering.
* **Latching Fault Shutdown**: All safety trips route through `trigger_fault()`, which zeroes `duty_pct`, cuts SSR output immediately to LOW, and locks out the heater until explicit `CLEAR_FAULT`.
* **`MAX_DUTY` Clamping**: SSR duty cycle is strictly limited to 40% (`MAX_DUTY_CYCLE`) to protect the 400W heatplate and SSR.
* **Emergency Over-Temperature**: Immediate shutoff at 280°C (`OVERTEMP_SHUTDOWN`).
* **Thermal Runaway Detection**: Validates temperature rise under sustained power within `THERMAL_RUNAWAY_PERIOD_MS` (18s).
* **Communication Timeout Watchdog**: If heating and no valid packet is received within 5,000ms (`COMM_TIMEOUT_MS`), heater is forced OFF immediately. Cooling fans are never stopped if communication is lost during cooling.

---

## 5. Build & Environment Instructions

### Microcontroller Firmware (Arduino CLI / IDE)
* Board Package: Earle F. Philhower RP2040/RP2350 (`rp2040:rp2040:rpipico` for UI, `rp2040:rp2040:rpipico2` for Controller).
* Build commands:
  ```bash
  arduino-cli compile -b rp2040:rp2040:rpipico firmware/Tejasvini_UI/
  arduino-cli compile -b rp2040:rp2040:rpipico2 firmware/Tejasvini_Controller/
  ```

### Host Unit & Simulation Test Suite
```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```
