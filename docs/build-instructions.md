# Tejasvini Firmware Build & Upload Guide

This guide details how to compile, test, and flash both **Tejasvini UI Firmware** and **Tejasvini Controller Firmware**.

---

## 1. Prerequisites & Toolchain Setup

### Microcontroller Board Support Package
Both firmware targets are built using the official **Earle F. Philhower RP2040/RP2350** core for Arduino:
* **Package URL**: `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`
* **Core Version**: `>= 4.0.0` (supports both RP2040 and RP2350/RP2350B architectures)

### Required Arduino Libraries
Install the following libraries via the Arduino IDE Library Manager or `arduino-cli`:

1. **For `Tejasvini_UI` (CrowPanel RP2040)**:
   - `PicoDVI` (Adafruit)
   - `Adafruit_GFX` & `Adafruit_BusIO`
   - `lvgl` (version `8.3.11`)
   - `TAMC_GT911` (Capacitive Touch)
2. **For `Tejasvini_Controller` (RP2350B)**:
   - Standard Philhower RP2350 core libraries (Hardware PWM, Timer Interrupts, Watchdog, Hardware Serial).

---

## 2. Building Tejasvini UI Firmware (`firmware/Tejasvini_UI/`)

### Target Specifications
* **Target Board**: Raspberry Pi Pico (`rp2040:rp2040:rpipico`)
* **Flash Size**: 2MB (Sketch: 2MB, FS: none)
* **CPU Speed**: 133 MHz (or 250 MHz for high-speed DVI)

### Arduino CLI Command
```bash
arduino-cli compile \
  -b rp2040:rp2040:rpipico \
  --build-property "build.extra_flags=-DLV_LVGL_H_INCLUDE_SIMPLE -DLV_USE_OBJ_NAME=1 -Wall -Wextra" \
  firmware/Tejasvini_UI/
```

### Arduino IDE Steps
1. Open Arduino IDE.
2. Open [`firmware/Tejasvini_UI/Tejasvini_UI.ino`](../firmware/Tejasvini_UI/Tejasvini_UI.ino).
3. Select Board: **Raspberry Pi Pico**.
4. Set **CPU Speed**: **133 MHz** (or **250 MHz**).
5. Click **Verify / Compile**.

---

## 3. Building Tejasvini Controller Firmware (`firmware/Tejasvini_Controller/`)

### Target Specifications
* **Target Board**: Raspberry Pi Pico 2 / RP2350 (`rp2040:rp2040:rpipico2`)
* **Flash Size**: 4MB
* **CPU Speed**: 150 MHz

### Arduino CLI Command
```bash
arduino-cli compile \
  -b rp2040:rp2040:rpipico2 \
  --build-property "build.extra_flags=-Wall -Wextra" \
  firmware/Tejasvini_Controller/
```

### Arduino IDE Steps
1. Open Arduino IDE.
2. Open [`firmware/Tejasvini_Controller/Tejasvini_Controller.ino`](../firmware/Tejasvini_Controller/Tejasvini_Controller.ino).
3. Select Board: **Raspberry Pi Pico 2** (RP2350 ARM Cortex-M33).
4. Click **Verify / Compile**.

---

## 4. Flashing Firmware to Hardware

### Method A: UF2 Bootloader (Drag-and-Drop)
1. Hold down the **BOOTSEL** button on the target board while connecting the USB cable to your computer.
2. A mass storage device named `RPI-RP2` (for RP2040) or `RP2350` will mount.
3. Drag and drop the compiled `.uf2` file onto the drive.
4. The microcontroller automatically flashes the firmware and reboots into normal execution.

### Method B: Serial / USB CDC Upload
If the board is already running firmware:
```bash
arduino-cli upload -p /dev/cu.usbmodemXXXXX -b rp2040:rp2040:rpipico firmware/Tejasvini_UI/
arduino-cli upload -p /dev/cu.usbmodemYYYYY -b rp2040:rp2040:rpipico2 firmware/Tejasvini_Controller/
```

---

## 5. Serial Debugging & Telemetry Inspection

Both targets mirror their diagnostic telemetry onto USB CDC (`Serial`) in addition to physical UART (`Serial1`):
* **Baud Rate**: `115200`
* **Connection**: Open any serial monitor (e.g. Arduino Serial Monitor, `minicom`, or `screen`):
  ```bash
  screen /dev/cu.usbmodem1101 115200
  ```
* **Interactive Commands**: You can type protocol commands directly into the terminal:
  - `CMD 1 PING` $\to$ Returns `RESP 1 ACK PONG`
  - `CMD 2 GET_STATUS` $\to$ Prints instantaneous `STATUS` frame
  - `CMD 3 START_PROFILE 1` $\to$ Starts Lead-Free reflow
  - `CMD 4 STOP` $\to$ Immediately cuts heater power

---

## 6. Running Host Automated Unit & Simulation Tests

Tejasvini includes a native host test suite built with CMake and CTest:

```bash
# 1. Configure test build
cmake -B build test/

# 2. Build test targets
cmake --build build

# 3. Execute test suite with detailed output
ctest --test-dir build --output-on-failure
```

### Test Coverage Summary
* `test_protocol`: Validates command tokenization, framing limits, argument range clamping, response encoding, and fault packet structures.
* `test_serialization`: Validates round-trip encoding and decoding of complete `STATUS` key-value telemetry records.
* `test_state_machine`: Validates machine state progression (`IDLE` $\to$ `HEATING` $\to$ `SOAK` $\to$ `REFLOW` $\to$ `COOL` $\to$ `DONE`), pauses, illegal state guards, and latching fault trips.
* `test_end_to_end_sim`: Simulates the complete interaction sequence: `PING` $\to$ `PONG`, `START_PROFILE` $\to$ `ACK`, `STOP` $\to$ heater OFF, invalid command $\to$ `NACK`, UART timeout during heating $\to$ heater OFF, and critical sensor trip $\to$ `FAULT` shutdown.
