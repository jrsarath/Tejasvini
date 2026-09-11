# Hardware Wiring & Physical Safety Guide

This document outlines the electrical connections, sensor circuits, solid-state relay considerations, and mandatory physical safety mechanisms required for building and operating **Tejasvini**.

---

## 1. Safety Warnings & Physical Protections

> [!CAUTION]
> **DANGER: MAINS VOLTAGE AND HIGH TEMPERATURE HAZARDS**
> * Tejasvini controls **mains AC voltage** (110V / 230V AC) and heating elements capable of reaching **exceeding 280°C**.
> * Mains electricity can cause fatal electric shock. High temperatures cause severe burns and fire.
> * **Software is NOT a safety mechanism.** Solid-state relays fail in a short-circuit (ON) state in roughly 90% of failure modes. If a microcontroller crashes or an SSR semiconductor fails shorted, firmware cannot shut off power.
> * You MUST incorporate the physical hardware protections described below.

### Mandatory Physical Protections
1. **Thermal Cut-Off Fuse (TCO / Thermal Fuse)**:
   - Install a non-resettable thermal fuse (rated at **260°C to 280°C**) clamped directly to the underside of the aluminum heatplate in physical contact with the metal.
   - Wire the thermal fuse in **series** with the AC Live line feeding the heating element. If the plate overheats for any reason, the fuse physically melts and permanently breaks the circuit.
2. **Protective Earth (PE) Grounding**:
   - The aluminum heatplate and all exposed metal chassis parts MUST be securely bonded to the AC mains Earth wire with a dedicated screw, lock washer, and ring terminal.
   - Use a multimeter to verify $< 0.1\,\Omega$ resistance between the heatplate surface and the ground pin of the mains plug.
3. **Mains Over-Current Fuse**:
   - Install a properly rated fast-blow fuse in a fused AC inlet socket (e.g., 3.15A for 230V 400W plate, or 5A for 110V 400W plate).
4. **Solid-State Relay (SSR) Heatsinking**:
   - Solid-state relays exhibit an internal forward voltage drop of $\approx 1.2\,\text{V}$ to $1.6\,\text{V}$. At a 2A load, this dissipates $2.5\,\text{W}$ to $3.5\,\text{W}$ of heat inside the SSR.
   - Mount the SSR to a metal chassis or aluminum heatsink using thermal paste. Never operate the SSR in a sealed unventilated plastic enclosure.

---

## 2. Hardware Pinout Overview

Tejasvini employs a dual-board configuration:
1. **CrowPanel 4.3" DVI UI Display** (RP2040): Drives PicoDVI, capacitive touch (GT911), backlight, and communicates via UART0 (GP0 TX / GP1 RX).
2. **Custom Machine Controller Board** (RP2350B): Controls the SSR (GP22), safety relay (GP23), NTC thermistors (GP26/27/28), cooling fans (GP6/7/8/9), buzzer (GP10), ARGB (GP11), rotary encoder (GP2/3/4), and connects to CrowPanel via UART (GP32 TX / GP33 RX).

For complete pin-by-pin specifications and schematics, refer to:
👉 **[docs/hardware-pin-map.md](hardware-pin-map.md)**

---

## 3. Dual NTC Thermistor Circuit & Math

Tejasvini uses **dual 100K 3950 NTC thermistors** mounted at separate positions across the heating surface (e.g. center and edge) to provide redundancy and detect thermal gradients or detached sensors.

### 3.1 Voltage Divider Circuit
Each thermistor is wired in a high-side divider configuration:

```text
       3.3V (VCC)
         │
       ┌─┴─┐
       │   │  NTC Thermistor (100kΩ @ 25°C)
       │   │  R_NTC
       └─┬─┘
         ├──────────────> RP2040 ADC Pin (GPIO 26 or GPIO 27)
       ┌─┴─┐
       │   │  Precision Divider Resistor (100kΩ, 1% tolerance)
       │   │  R_DIVIDER
       └─┬─┘
         │
        GND
```

### 3.2 Mathematical Derivation
The voltage at the ADC input pin is:
$$V_{\text{out}} = V_{\text{cc}} \cdot \frac{R_{\text{divider}}}{R_{\text{ntc}} + R_{\text{divider}}}$$

With a 12-bit ADC full-scale count of $4095$ corresponding to $V_{\text{cc}}$:
$$\frac{\text{ADC}}{4095} = \frac{R_{\text{divider}}}{R_{\text{ntc}} + R_{\text{divider}}}$$

Inverting both sides:
$$\frac{4095}{\text{ADC}} = \frac{R_{\text{ntc}}}{R_{\text{divider}}} + 1 \implies R_{\text{ntc}} = R_{\text{divider}} \cdot \left(\frac{4095}{\text{ADC}} - 1\right)$$

### 3.3 Steinhart-Hart / Beta Temperature Conversion
From the calculated resistance $R_{\text{ntc}}$, temperature in Kelvin is determined via the Beta equation:
$$\frac{1}{T} = \frac{1}{T_0} + \frac{1}{\beta} \cdot \ln\left(\frac{R_{\text{ntc}}}{R_0}\right)$$

Where:
* $T_0 = 298.15\,\text{K}$ ($25^\circ\text{C}$)
* $R_0 = 100{,}000\,\Omega$ ($100\text{k}\Omega$)
* $\beta = 3950\,\text{K}$
* $T_{\text{Celsius}} = T_{\text{Kelvin}} - 273.15$

### 3.4 Electrical Fault Signatures
* **Broken / Disconnected NTC (Open Circuit)**: $R_{\text{ntc}} \to \infty$. The ADC pin is pulled to GND by $R_{\text{divider}}$, reading $\text{ADC} \approx 0$ counts.
* **Shorted NTC (Short to 3.3V)**: $R_{\text{ntc}} \to 0$. The ADC pin is pulled directly to 3.3V, reading $\text{ADC} \approx 4095$ counts.
* **Firmware Fast Bounds**: Tejasvini checks $\text{ADC} < 50$ or $\text{ADC} > 4050$ counts and shuts off immediately ($<100\text{ms}$).

---

## 4. Solid-State Relay (SSR) Wiring

### 4.1 AC Mains SSR Connection
Connect a 3.3V-logic compatible AC Zero-Cross Solid State Relay (e.g. Fotek SSR-25DA or Crydom D2425):

```text
[Raspberry Pi Pico]                      [AC Mains Solid-State Relay]
GPIO 22 ───────────────────────────────> Input (+) [Terminal 3]
GND     ───────────────────────────────> Input (-) [Terminal 4]

[AC Mains 230V/110V Live] ────[Mains Fuse]───[Thermal Cutoff 260°C]───> Output [Terminal 1]
                                                                        Output [Terminal 2] ───> [PTC Heatplate L]
[AC Mains Neutral] ────────────────────────────────────────────────────────────────────────────> [PTC Heatplate N]
[AC Mains Earth]   ────────────────────────────────────────────────────────────────────────────> [Heatplate Metal Body]
```

### 4.2 Why Time-Proportioning is Used Instead of High-Frequency PWM
Zero-crossing AC solid-state relays only switch on and off when the AC line voltage passes through 0V (every 10ms at 50Hz, or 8.33ms at 60Hz).
* Applying a 1000 Hz PWM signal to an AC zero-cross SSR results in random, irregular half-cycle conduction, extreme flicker, and unpredictable power delivery.
* Tejasvini uses **Time-Proportioning PWM** over a 1000ms window (`SSR_WINDOW_MS 1000`). At 25% duty, the relay conducts for 250ms (12.5 complete AC cycles at 50Hz) and remains OFF for 750ms.

---

## 5. Mechanical Enclosure & 3D Hardware Models

CAD & 3D models for Tejasvini are located in [`assets/3d files/`](../assets/3d%20files/).

### 5.1 3D Model Manifest

#### 3D Printable Parts
| File Name | Functional Description | Print Recommendation |
| :--- | :--- | :--- |
| [`Frame - Main.stl`](../assets/3d%20files/Frame%20-%20Main.stl) | Main structural chassis. Houses the Raspberry Pi Pico, solid-state relay, AC terminal block, and power distribution. | PETG / ABS / ASA, 35% infill, 4 perimeters |
| [`Frame - Display.stl`](../assets/3d%20files/Frame%20-%20Display.stl) | Angled front display bezel. Secures the Elecrow 4.3" CrowPanel RTD2281 DVI screen and GT911 touch panel. | PETG / ABS / PLA, 25% infill, 3 perimeters |
| [`Display Bottom Plate.stl`](../assets/3d%20files/Display%20Bottom%20Plate.stl) | Rear/bottom cover plate for the display module housing, protecting display PCB and encoder wiring. | PETG / ABS / PLA, 25% infill, 3 perimeters |
| [`Top Plate.stl`](../assets/3d%20files/Top%20Plate.stl) | Top cosmetic deck plate surrounding the heating zone, providing a smooth, flush exterior interface. | **ABS / ASA / PC**, 40% infill, 4 perimeters |

#### Physical Hardware Reference (Do NOT 3D Print)
| File Name | Functional Description | Material | Notes |
| :--- | :--- | :--- | :--- |
| [`Heating Plate.stl`](../assets/3d%20files/Heating%20Plate.stl) | Physical heating plate reference model. | **Aluminium** | **Visualization only.** Must be fabricated from aluminum (sheet / CNC machined plate). Never 3D print. |

### 5.2 Assembly & Thermal Isolation
Because the aluminum heatplate operates at temperatures up to 250°C, direct contact between the metal plate and thermoplastic components will cause warping or melting:

1. **Aluminium Plate Construction**:
   - The actual heating surface represented by `Heating Plate.stl` **must be made of aluminum** for optimal thermal conductivity and heat distribution across the PCB work area.
2. **Thermal Standoffs**:
   - Mount the aluminum heating plate to the chassis using M3 brass standoffs (minimum 10mm to 15mm height) to establish a convective air gap above the electronics.
   - Place heat-resistant silicone grommets or ceramic washers between the screw heads and the plate mounting holes to prevent conductive heat transfer into the printed `Top Plate.stl` or chassis.
3. **Thermal Fuse Mounting**:
   - The 260°C–280°C thermal cut-off fuse (TCO) should be mounted directly to the underside of the aluminum heatplate with a metal retaining clip or high-temperature silicone adhesive, ensuring intimate thermal contact.
4. **Internal Airflow**:
   - The main frame features ventilation slots to promote natural convection around the SSR heatsink and the Raspberry Pi Pico.

### 5.3 Filament Material Guide

| Filament Material | Glass Transition ($T_g$) | Heat Deflection (HDT @ 0.45MPa) | Suitability for Tejasvini |
| :--- | :--- | :--- | :--- |
| **PLA** | ~55°C – 60°C | ~55°C | **Only for Display Bezel**. Never use near the heater plate. |
| **PETG** | ~80°C – 85°C | ~70°C – 75°C | Excellent for Main Frame & Display housing. |
| **ABS / ASA** | ~105°C | ~95°C – 100°C | **Recommended** for Heating Plate mount and Top Plate. |
| **PC (Polycarbonate)** | ~145°C – 150°C | ~135°C – 140°C | **Best performance** for high-temperature continuous operation. |

