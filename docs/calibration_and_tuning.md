# Calibration & Controller Tuning Guide

This guide details how to verify sensor calibration, tune the PI temperature regulation parameters, calibrate thermal runaway limits, and customize reflow soldering profiles in **Tejasvini**.

---

## 1. NTC Thermistor Calibration

Accurate temperature reading depends on knowing the exact resistance of the voltage divider resistors and the nominal thermistor parameters.

### 1.1 Voltage Divider Resistor Calibration
While nominal 100kΩ resistors are specified, standard 1% resistors can vary between 99.0kΩ and 101.0kΩ, resulting in a 1°C to 2°C temperature offset.

**To calibrate:**
1. Power off the system and disconnect the thermistor plug.
2. Measure the actual resistance between the ADC pin (GPIO 26 or GPIO 27) and GND using a calibrated 4.5-digit or 5.5-digit Digital Multimeter (DMM).
3. If your measured resistance is e.g. $99{,}400\,\Omega$, update `R_DIVIDER` in [`firmware/Tejasvini_Controller/ControllerConfig.h`](../firmware/Tejasvini_Controller/ControllerConfig.h):
   ```c
   #define R_DIVIDER 99400.0f // Measured divider resistance in Ohms
   ```

### 1.2 Verifying Sensor Readings at Ambient
1. Connect both thermistors and boot the machine controller.
2. Open the serial terminal at 115200 baud on the controller USB CDC or UART.
3. Verify that both NTC1 and NTC2 readings match the room ambient temperature measured by a reference thermometer within $\pm 1.0^\circ\text{C}$.

---

## 2. PI Controller Tuning

Tejasvini utilizes a proportional-integral (PI) regulator with soft-start reference ramping. The controller constants are defined in [`firmware/Tejasvini_Controller/ControllerConfig.h`](../firmware/Tejasvini_Controller/ControllerConfig.h):

```c
#define KP_GAIN 1.5f  // Proportional gain
#define KI_GAIN 0.02f // Integral gain
```

### 2.1 Understanding Controller Parameters
* **`RAMP_RATE_DEG_PER_S` ($1.5^\circ\text{C}/\text{s}$)**: Soft-start ramp speed. The setpoint gradually climbs toward the target rather than stepping instantly, preventing overshoot and thermal shock.
* **`KP_GAIN` ($1.5$)**: Reacts immediately to temperature error.
  * *Too high:* The plate will oscillate around the target temperature by $\pm 3^\circ\text{C}$ to $5^\circ\text{C}$.
  * *Too low:* The plate will respond sluggishly and take too long to reach the target.
* **`KI_GAIN` ($0.02$)**: Gradually eliminates steady-state droop caused by ambient cooling losses.
  * *Too high:* Causes phase lag, overshoot at the end of the ramp, and slow hunting oscillations.
  * *Too low:* Plate may settle 1°C to 2°C below the target temperature indefinitely.

### 2.2 Tuning Procedure for Custom Plates
If using a heavier aluminum plate (e.g. 100mm $\times$ 100mm $\times$ 10mm thick), the thermal mass is significantly greater:
1. Start with `KP_GAIN = 1.0` and `KI_GAIN = 0.01`.
2. Heat the plate to 150°C and observe the steady-state graph on the serial telemetry monitor.
3. If temperature settles below 150°C, increase `KI_GAIN` by `0.005` increments until zero steady-state error is achieved.
4. If temperature oscillates continually, decrease `KP_GAIN` by `0.2` increments.

---

## 3. Thermal Runaway Tuning

The thermal runaway monitor protects against detached thermistors or failing SSRs:

```c
#define THERMAL_RUNAWAY_PERIOD_MS 18000 // Check window in milliseconds (18 seconds)
#define THERMAL_RUNAWAY_MIN_RISE  2.0f  // Minimum required temperature rise (°C)
```

### Tuning Considerations:
* When driving $\ge 20\%$ duty with a deficit $> 10^\circ\text{C}$, temperature must rise by at least `2.0°C` every `18` seconds.
* If you operate an unusually heavy, uninsulated heatplate or operate in cold ambient environments ($< 10^\circ\text{C}$), the rise rate near 200°C might slow down.
* If a false thermal runaway trip occurs on a very heavy plate, increase `THERMAL_RUNAWAY_PERIOD_MS` in [`firmware/shared/Config.h`](../firmware/shared/Config.h). Never disable the check entirely.

---

## 4. Customizing Reflow Soldering Profiles

Reflow profile parameters reside in [`firmware/Tejasvini_Controller/ProfileEngine.cpp`](../firmware/Tejasvini_Controller/ProfileEngine.cpp) in `kProfileConfigs`:

```cpp
struct ProfileStageConfig {
  int preheat_temp;        // °C (preheat target)
  uint32_t preheat_time_s; // target duration in preheat (seconds)
  int soak_temp;           // °C (soak target)
  uint32_t soak_time_s;    // seconds in soak zone
  int peak_temp;           // °C (peak reflow target)
  uint32_t peak_dwell_s;   // seconds dwell at peak
  int cool_temp;           // °C (safe handling temperature)
  uint32_t cool_time_s;    // nominal cooling duration (seconds)
};
```

### Preconfigured Profiles:
1. **Lead-Free (SAC305 / Sn96.5Ag3.0Cu0.5)**:
   - Preheat: 150°C (90s)
   - Soak: 175°C (60s)
   - Peak: 245°C (30s dwell)
   - Cool: 50°C
2. **Leaded (Sn63Pb37)**:
   - Preheat: 130°C (80s)
   - Soak: 155°C (60s)
   - Peak: 215°C (30s dwell)
   - Cool: 50°C
3. **Low-Temperature (Sn42Bi58 Bismuth)**:
   - Preheat: 90°C (60s)
   - Soak: 115°C (45s)
   - Peak: 150°C (25s dwell)
   - Cool: 45°C
