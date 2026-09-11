# Tejasvini UART Protocol Specification

This document defines the serial wire protocol between the **Tejasvini CrowPanel UI Terminal** and the **Tejasvini Machine Controller**.

---

## 1. Physical Layer & Wiring

* **Baud Rate**: `115200` baud
* **Data Bits**: `8`
* **Parity**: `None`
* **Stop Bits**: `1`
* **Flow Control**: `None`
* **Signal Voltage**: `3.3V LVCMOS` (Do NOT use RS-232 $\pm 12\text{V}$ signals!)

### Physical Soldered Wiring Arrangement

```text
CrowPanel Terminal (RP2040)                 Machine Controller (RP2350B)
┌─────────────────────────┐                 ┌──────────────────────────┐
│  GP0 (UART0 TX)         ├────────────────>│  GP33 (UART RX)          │
│  GP1 (UART0 RX)         │<────────────────┤  GP32 (UART TX)          │
│  GND                    ├─────────────────┤  GND                     │
└─────────────────────────┘                 └──────────────────────────┘
```

> [!IMPORTANT]
> The connections are soldered point-to-point. CrowPanel TX connects to Controller RX; CrowPanel RX connects to Controller TX. Common ground is strictly required. No external level shifters are needed as both devices operate on 3.3V logic.

---

## 2. Frame Format & Conventions

1. **Delimiter**: Each frame is newline-terminated (`\n` or `\r\n`).
2. **Maximum Length**: Maximum single line length is `160` bytes. Lines exceeding this limit are discarded to prevent buffer overflow.
3. **Encoding**: Standard ASCII text.
4. **Sequence Tracking**: Commands sent from UI to Controller may include a numeric sequence identifier (`<seq_id>`) which is mirrored in the corresponding `RESP <seq_id> ACK` or `RESP <seq_id> NACK`.

---

## 3. UI to Controller Commands

### Command Grammar
```text
CMD [<seq_id>] <COMMAND_NAME> [<ARG1>] [<ARG2>] ...\n
```

### Supported Commands

| Command | Arguments | Description | Example |
| :--- | :--- | :--- | :--- |
| `PING` | None | Heartbeat connection test | `CMD 1 PING` |
| `GET_STATUS` | None | Instant request for status report | `CMD 2 GET_STATUS` |
| `START_PROFILE` | `<profile_id>`<br>(`0`=MANUAL, `1`=LEAD_FREE, `2`=LEADED, `3`=LOW_TEMP) | Initiates reflow profile or manual heating | `CMD 3 START_PROFILE 1` |
| `STOP` | None | Immediately aborts heating and shuts off SSR | `CMD 4 STOP` |
| `PAUSE` | None | Freezes stage timing during reflow | `CMD 5 PAUSE` |
| `RESUME` | None | Resumes reflow stage timing | `CMD 6 RESUME` |
| `CLEAR_FAULT` | None | Clears latched safety fault (if sensors OK) | `CMD 7 CLEAR_FAULT` |
| `SET_TARGET` | `<temp_c>` (0 to 270) | Updates target setpoint temperature | `CMD 8 SET_TARGET 215` |
| `SET_FAN_MODE` | `<fan_id>` (1 or 2)<br>`<mode>` (`0`=AUTO, `1`=MANUAL, `2`=OFF)<br>`[<pwm_pct>]` (0 to 100) | Configures fan operation mode and speed | `CMD 9 SET_FAN_MODE 1 1 80` |

---

## 4. Controller to UI Responses & Messages

### 4.1 Response Frame (`RESP`)
```text
RESP <seq_id> ACK [<message>]\n
RESP <seq_id> NACK <reason>\n
PONG\n
```
* `ACK`: The controller accepted and executed the command.
* `NACK`: The command was rejected (e.g. invalid parameter, illegal state transition).

### 4.2 Status Frame (`STATUS`)
Broadcast periodically at 2 Hz (`COMM_STATUS_PERIOD_MS 500`) and immediately after any state change or `GET_STATUS` query:
```text
STATUS ver=<ver> mcu=<mcu_str> state=<state> prof=<prof> stage=<stage> prog=<prog> elap=<elap> rem=<rem> t_tgt=<t_tgt> t1=<t1> t2=<t2> t3=<t3> heat=<0|1> duty=<duty> f1_pwm=<f1> f1_rpm=<rpm1> f2_pwm=<f2> f2_rpm=<rpm2> fault=<code> sev=<sev> uptime=<uptime>\n
```

#### Status Field Definitions
| Key | Type | Description |
| :--- | :--- | :--- |
| `ver` | Integer | Protocol specification version (currently `1`) |
| `mcu` | String | Firmware version string (e.g. `v1.1.0`) |
| `state` | Enum String | Current machine state (`BOOTING`, `IDLE`, `HEATING`, `SOAKING`, `REFLOWING`, `COOLING`, `PAUSED`, `COMPLETE`, `FAULT`) |
| `prof` | Enum String | Selected profile (`MANUAL`, `LEAD_FREE`, `LEADED`, `LOW_TEMP`) |
| `stage` | Enum String | Reflow sub-stage (`NONE`, `PREHEAT`, `SOAK`, `REFLOW`, `COOL`, `DONE`) |
| `prog` | Integer | Progress value (`0` to `100` for MANUAL, `0` to `1000` for Reflow profiles) |
| `elap` | Integer | Elapsed seconds in active stage |
| `rem` | Integer | Target duration in seconds for active stage |
| `t_tgt` | Integer | Target temperature setpoint (°C) |
| `t1` | Float | Primary NTC temperature (°C) |
| `t2` | Float | Secondary NTC temperature (°C) |
| `t3` | Float | Auxiliary NTC temperature (°C, or `-99.0` if not connected) |
| `heat` | Integer (`0`/`1`)| SSR output state (`1` = active, `0` = inactive) |
| `duty` | Float | Active time-proportioning duty cycle (`0.0%` to `40.0%`) |
| `f1_pwm`| Integer | Fan 1 PWM duty (`0` to `100%`) |
| `f1_rpm`| Integer | Fan 1 Tachometer measured speed (RPM) |
| `f2_pwm`| Integer | Fan 2 PWM duty (`0` to `100%`) |
| `f2_rpm`| Integer | Fan 2 Tachometer measured speed (RPM) |
| `fault` | Integer | Active FaultCode (`0` = `FAULT_NONE`) |
| `sev` | Integer | Active FaultSeverity (`0` = `SEVERITY_NONE`, `3` = `CRITICAL`) |
| `uptime`| Integer | Controller uptime in seconds |

### 4.3 Fault Frame (`FAULT`)
Broadcast whenever a safety fault is triggered:
```text
FAULT <FAULT_NAME> <SEVERITY> <DESCRIPTION>\n
```
Example:
```text
FAULT OVERTEMP CRITICAL T_meas > 280C
```

---

## 5. Fault Codes & Severities

### Fault Codes
* `0` = `NONE`
* `1` = `NTC1_OPEN`: Primary sensor disconnected ($ADC < 50$)
* `2` = `NTC1_SHORT`: Primary sensor shorted to 3.3V ($ADC > 4050$)
* `3` = `NTC2_OPEN`: Secondary sensor disconnected
* `4` = `NTC2_SHORT`: Secondary sensor shorted to 3.3V
* `5` = `NTC3_OPEN`: Auxiliary sensor disconnected
* `6` = `NTC3_SHORT`: Auxiliary sensor shorted to 3.3V
* `7` = `NTC_DIVERGENCE`: Sensor delta $>15^\circ\text{C}$ before heating or $>35^\circ\text{C}$ during heating
* `8` = `OVERTEMP`: Measured temperature exceeded emergency cutoff ($280^\circ\text{C}$)
* `9` = `THERMAL_RUNAWAY`: Temperature failed to rise by $2.0^\circ\text{C}$ within 18 seconds under $\ge 20\%$ duty
* `10` = `COMM_TIMEOUT`: UART communication lost while heating ($>5000\text{ms}$)
* `11` = `WATCHDOG_RESET`: Controller rebooted due to watchdog trip
* `12` = `HARDWARE_FAILURE`: Board power or peripheral fault

### Severities
* `0` = `NONE`
* `1` = `INFO`
* `2` = `WARNING`
* `3` = `CRITICAL` (Immediate latching heater shutdown)
* `4` = `FATAL`

---

## 6. Timeout Policy

* **Heartbeat**: The UI sends a `PING` or `GET_STATUS` command every 1000ms.
* **Safety Watchdog Timeout**: If the controller is in an active heating state (`HEATING`, `SOAKING`, `REFLOWING`) and receives **no valid packet within 5,000ms** (`COMM_TIMEOUT_MS`), the controller immediately:
  1. Sets SSR output to LOW (Heater OFF).
  2. Latches `FAULT_COMM_TIMEOUT` (Severity CRITICAL).
  3. Activates the buzzer alarm.
  4. Broadcasts `FAULT COMM_TIMEOUT CRITICAL UART timeout`.
* **Cooling Independence**: If communication is lost during `COOLING`, the cooling fan is **NOT** stopped, ensuring safe cool-down of the hot aluminum plate.

---

## 7. Example Exchanges

### 7.1 Liveness Probe
```text
UI  -> CMD 1 PING
CTL <- RESP 1 ACK PONG
```

### 7.2 Starting a Lead-Free Reflow Profile
```text
UI  -> CMD 2 START_PROFILE 1
CTL <- RESP 2 ACK STARTED
CTL <- STATUS ver=1 mcu=v1.1.0 state=HEATING prof=LEAD_FREE stage=PREHEAT prog=0 elap=0 rem=90 t_tgt=150 t1=24.8 t2=24.5 t3=-99.0 heat=1 duty=40.0 f1_pwm=0 f1_rpm=0 f2_pwm=0 f2_rpm=0 fault=0 sev=0 uptime=12
```

### 7.3 Rejecting an Out-of-Bounds Target
```text
UI  -> CMD 3 SET_TARGET 350
CTL <- RESP 3 NACK TARGET_OUT_OF_RANGE
```

### 7.4 Immediate Stop
```text
UI  -> CMD 4 STOP
CTL <- RESP 4 ACK STOPPED
CTL <- STATUS ver=1 mcu=v1.1.0 state=IDLE prof=LEAD_FREE stage=NONE prog=0 elap=0 rem=0 t_tgt=150 t1=182.4 t2=181.9 t3=-99.0 heat=0 duty=0.0 f1_pwm=0 f1_rpm=0 f2_pwm=0 f2_rpm=0 fault=0 sev=0 uptime=88
```

### 7.5 Sensor Failure Trip
```text
[NTC1 wire vibrates loose]
CTL <- FAULT NTC1_OPEN CRITICAL NTC1_OPEN
CTL <- STATUS ver=1 mcu=v1.1.0 state=FAULT prof=LEAD_FREE stage=NONE prog=0 elap=0 rem=0 t_tgt=0 t1=-20.0 t2=182.0 t3=-99.0 heat=0 duty=0.0 f1_pwm=100 f1_rpm=2400 f2_pwm=0 f2_rpm=0 fault=1 sev=3 uptime=92
```
