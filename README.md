# Tejasvini (तेजस्विनी) — Open-Source Heatplate Controller

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform: RP2040 & RP2350](https://img.shields.io/badge/Platform-RP2040%20%28UI%29%20%26%20RP2350B%20%28Controller%29-blue.svg)](https://www.raspberrypi.com/products/raspberry-pi-pico/)
[![Display: PicoDVI](https://img.shields.io/badge/Display-PicoDVI%20%28400x240%20%2F%20800x480%29-brightgreen.svg)](https://github.com/Wren6991/PicoDVI)
[![UI: LVGL 8.3](https://img.shields.io/badge/UI-LVGL%208.3%20%2F%20EEZ%20Studio-orange.svg)](https://lvgl.io/)
[![Build Status: Tests Passing](https://img.shields.io/badge/CTest-100%25%20Passing-brightgreen.svg)](test/)

**Tejasvini** is an open-source, precision heatplate controller firmware engineered for surface-mount (SMD/SMT) reflow soldering, PCB pre-heating, and rework.

The system features a **dual-target, decoupled architecture**:
1. **Tejasvini UI Firmware** (`firmware/Tejasvini_UI/`): Pure HMI terminal running on an **Elecrow CrowPanel 4.3" Pico DVI Display** (RP2040) with native LVGL 8.3 UI and capacitive touch.
2. **Tejasvini Controller Firmware** (`firmware/Tejasvini_Controller/`): Authoritative machine controller running on a custom **RP2350B control board** that owns thermal sensing, PI regulation, SSR actuation, safety interlocks, cooling fans, and rotary encoder inputs.
3. **Shared Specification Core** (`firmware/shared/`): High-performance, versioned, framed UART protocol connecting both targets over a dedicated soldered serial link.

---

## Key Features

* **Decoupled Safety Architecture**: The machine controller is the single source of truth for all safety and thermal execution. If the UI locks up, disconnects, or crashes, the controller automatically forces the heater OFF and protects hardware.
* **Redundant NTC Thermistor Sensing**: Multi-channel 100K 3950 NTC thermistors with 32x oversampling, trimmed-mean outlier rejection, and real-time sensor divergence detection.
* **AC Zero-Cross SSR Drive**: 1000ms time-proportioning slow PWM tailored specifically for AC zero-crossing Solid State Relays (e.g. Fotek SSR-25DA) to avoid half-cycle conduction jitter.
* **Soft-Start Reference Ramping**: Temperature ramps smoothly towards setpoint at $1.5^\circ\text{C}/\text{s}$ to prevent thermal shock to ceramic SMD components and eliminate overshoot.
* **Automated 4-Stage Reflow State Machine**:
  * **Lead-Free (SAC305)**: 245°C peak reflow curve.
  * **Leaded (Sn63Pb37)**: 215°C peak reflow curve.
  * **Low-Temperature (Sn42Bi58)**: 150°C peak reflow curve for heat-sensitive components and plastic connectors.
  * **Manual Mode**: Direct setpoint adjustment (0°C to 270°C).
* **Dual Cooling Fan Support**: Dual 4-wire 25 kHz PWM fan control with tachometer pulse RPM capture.
* **High-Resolution PicoDVI Dashboard**: Renders a 240x400 portrait UI (400x240 hardware DVI upscaled 2x to 800x480) with real-time temperature dials, duty percentage, stage timer, and status.
* **Dual Input Interfaces**: Capacitive touchscreen (GT911) with full on-screen control + detented rotary encoder and debounced push-button.
* **Comprehensive Multi-Tier Safety Interlocks**: Fast-trip open/short circuit detection (<100ms), thermal runaway monitoring, 280°C emergency shutdown, pre-heat sensor discrepancy check, UART heartbeat watchdog, and clamped duty cycle (max 40%).

---

## Directory Structure

```text
Tejasvini/
├── CMakeLists.txt            # Root CMake configuration (builds host test suite)
├── LICENSE                   # MIT License
├── README.md                 # Project documentation (this file)
├── CONTRIBUTING.md           # Developer guidelines & safety standards
├── CODE_OF_CONDUCT.md        # Contributor Covenant Code of Conduct
├── AGENTS.md                 # Developer & AI architectural guide
├── ui.eez-project            # EEZ Studio LVGL project source file
│
├── firmware/
│   ├── shared/               # Canonical shared Arduino library (library.properties)
│   │   ├── library.properties# Standard Arduino 1.5 library specification
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
│   │   ├── UiVars.cpp        # EEZ Studio variable bridge bindings
│   │   ├── UiActions.cpp     # EEZ Studio action bridge callbacks
│   │   └── src/ui/           # EEZ Studio generated LVGL UI screens and styles
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

## Physical UART Interconnect

The CrowPanel terminal connects directly to the machine controller using soldered wires:

```text
CrowPanel GP0 / UART0 TX  ───(Soldered 3.3V wire)───>  Controller UART RX / GP33
CrowPanel GP1 / UART0 RX  <───(Soldered 3.3V wire)───  Controller UART TX / GP32
CrowPanel GND             ───────────────────────────  Controller GND
```

* **Logic Voltage**: 3.3V LVCMOS (Direct connection; no level shifter needed).
* **Baud Rate**: 115200 baud, 8N1.

---

## Building and Flashing

See [docs/build-instructions.md](docs/build-instructions.md) for full setup instructions:

```bash
# Compile UI firmware (RP2040 CrowPanel)
arduino-cli compile -b rp2040:rp2040:rpipico --library firmware/shared --build-property "build.extra_flags=-DLV_LVGL_H_INCLUDE_SIMPLE -DLV_USE_OBJ_NAME=1" firmware/Tejasvini_UI/

# Compile Controller firmware (RP2350B Board)
arduino-cli compile -b rp2040:rp2040:rpipico2 --library firmware/shared firmware/Tejasvini_Controller/

# Run host unit, simulation, and repository validation tests
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## Technical Documentation Suite

* [Firmware Architecture Specification](docs/firmware-architecture.md)
* [UART Wire Protocol Specification](docs/uart-protocol.md)
* [Safety Behavior & Interlock Specification](docs/safety-behavior.md)
* [Hardware Pin Mapping Specification](docs/hardware-pin-map.md)
* [Firmware Build & Upload Guide](docs/build-instructions.md)
* [Hardware Wiring & Physical Protections](docs/hardware_and_safety.md)
* [Calibration & Controller Tuning](docs/calibration_and_tuning.md)

---

## License

This project is open-source under the **MIT License**. See [LICENSE](LICENSE) for details.

Copyright (c) 2026 Sarath "Delta" Singh <br />
Made with ❤️ in Kolkata, India.