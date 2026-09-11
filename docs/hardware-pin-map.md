# Tejasvini Hardware Pin Mapping

This document provides the definitive GPIO assignments and electrical characteristics for both the **CrowPanel UI Display** (RP2040) and the **Machine Controller Board** (RP2350B).

---

## 1. CrowPanel 4.3" DVI UI Terminal (Raspberry Pi Pico RP2040)

| Signal Name | RP2040 GPIO | Function / Peripheral | Electrical Characteristics / Notes |
| :--- | :--- | :--- | :--- |
| `UART0_TX` | **GPIO 0** | Hardware UART0 Transmit | Connects directly to Controller `GP33 (UART_RX)` via soldered wire |
| `UART0_RX` | **GPIO 1** | Hardware UART0 Receive | Connects directly to Controller `GP32 (UART_TX)` via soldered wire |
| `GND` | **GND** | Digital Ground | Common ground reference between both boards |
| `DVI_DATA_D0+ / D0-`| **GPIO 12, 13** | PicoDVI Blue Differential Pair | Driven by PIO0; 400x240 RGB565 @ 60Hz to RTD2281 scaler |
| `DVI_DATA_D1+ / D1-`| **GPIO 14, 15** | PicoDVI Green Differential Pair| Driven by PIO0 |
| `DVI_DATA_D2+ / D2-`| **GPIO 16, 17** | PicoDVI Red Differential Pair | Driven by PIO0 |
| `DVI_CLK+ / CLK-` | **GPIO 18, 19** | PicoDVI Pixel Clock Pair | Driven by PIO0 |
| `PIN_BACKLIGHT` | **GPIO 24** | Display Backlight Control | Active LOW (driven LOW to illuminate display) |
| `TOUCH_SDA` | **GPIO 20** | GT911 Capacitive Touch I2C SDA | Requires 4.7kΩ pull-up resistor to 3.3V |
| `TOUCH_SCL` | **GPIO 21** | GT911 Capacitive Touch I2C SCL | Requires 4.7kΩ pull-up resistor to 3.3V |
| `TOUCH_INT` | **GPIO 25** | GT911 Touch Interrupt | Active LOW touch detection |
| `TOUCH_RST` | **GPIO 29** | GT911 Touch Hardware Reset | Active LOW hardware reset |

---

## 2. Machine Controller Board (Custom RP2350B)

| Signal Name | RP2350B GPIO | Function / Peripheral | Electrical Characteristics / Notes |
| :--- | :--- | :--- | :--- |
| `UART_TX` | **GPIO 32** | Hardware UART Transmit | Connects directly to CrowPanel `GP1 (UART0_RX)` via soldered wire |
| `UART_RX` | **GPIO 33** | Hardware UART Receive | Connects directly to CrowPanel `GP0 (UART0_TX)` via soldered wire |
| `GND` | **GND** | Digital Ground | Common ground reference |
| `PIN_SSR` | **GPIO 22** | Solid-State Relay Actuation | 3.3V Logic Drive (Zero-Cross Time Proportioning, max 40% duty) |
| `PIN_RELAY` | **GPIO 23** | Auxiliary Safety Power Relay | Active HIGH drive (Energizes live AC relay when heating) |
| `PIN_NTC1` | **GPIO 26** | Primary NTC Thermistor | RP2350 ADC0 (12-bit), 100kΩ precision series divider to GND |
| `PIN_NTC2` | **GPIO 27** | Secondary NTC Thermistor | RP2350 ADC1 (12-bit), 100kΩ precision series divider to GND |
| `PIN_NTC3` | **GPIO 28** | Auxiliary NTC Thermistor | RP2350 ADC2 (12-bit), 100kΩ precision series divider to GND |
| `PIN_ENA` | **GPIO 2** | Rotary Encoder Phase A | Internal `INPUT_PULLUP` enabled |
| `PIN_ENB` | **GPIO 3** | Rotary Encoder Phase B | Internal `INPUT_PULLUP` enabled |
| `PIN_EBT` | **GPIO 4** | Rotary Encoder Push-Button | Internal `INPUT_PULLUP` enabled (Active LOW) |
| `PIN_FAN1_PWM` | **GPIO 6** | Cooling Fan 1 PWM Speed Control| 25 kHz PWM output for standard 4-wire cooling fans |
| `PIN_FAN1_TACH`| **GPIO 7** | Cooling Fan 1 Tachometer Pulse | Internal `INPUT_PULLUP` enabled (Falling edge interrupt capture) |
| `PIN_FAN2_PWM` | **GPIO 8** | Cooling Fan 2 PWM Speed Control| 25 kHz PWM output for standard 4-wire cooling fans |
| `PIN_FAN2_TACH`| **GPIO 9** | Cooling Fan 2 Tachometer Pulse | Internal `INPUT_PULLUP` enabled (Falling edge interrupt capture) |
| `PIN_BUZZER` | **GPIO 10** | Audible Piezo Buzzer Drive | Hardware PWM tone frequency generation (click, tones, siren) |
| `PIN_ARGB` | **GPIO 11** | Visual Status ARGB Indicator | WS2812B / NeoPixel single-wire data stream |

---

## 3. Physical UART Interconnect

```text
CrowPanel GP0 / UART TX  ───(Soldered 3.3V wire)───>  Controller UART RX / GP33
CrowPanel GP1 / UART RX  <───(Soldered 3.3V wire)───  Controller UART TX / GP32
CrowPanel GND            ───────────────────────────  Controller GND
```
