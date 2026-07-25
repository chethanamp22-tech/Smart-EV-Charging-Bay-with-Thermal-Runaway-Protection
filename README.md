# Smart EV Charging Bay with Thermal Runaway Protection

An Arduino Uno-based safety monitoring system for EV charging bays. It continuously watches ambient gas levels, flame presence, and temperature near the charging battery, and automatically cuts power and raises an alert if signs of thermal runaway are detected.

## Overview

Thermal runaway is one of the biggest safety risks in EV battery charging — a small fault can escalate into gas venting, fire, or explosion within seconds. This project simulates a low-cost embedded safety layer that a charging bay controller could use to detect early warning signs and shut down power before the situation worsens.

The system runs entirely on an Arduino Uno and displays live status on an OLED screen, with a button used to start/stop charging and acknowledge alerts.

## Features

- **Gas monitoring** — MQ135 sensor tracks air quality/gas concentration near the battery
- **Flame detection** — digital flame sensor for open-flame/fire signs
- **Temperature monitoring** — DHT11 sensor reads ambient bay temperature every 2 seconds
- **Automatic protection** — if any threshold is breached while charging is active, the system immediately cuts power and locks into a critical alert state
- **Audible + visual alerts** — buzzer tone and OLED "CRITICAL" screen on a thermal event
- **Manual control** — single push-button toggles charging on/off, and clears a thermal event once resolved
- **Live status display** — 128x64 OLED shows charging status, temperature, gas level, and flame status in real time
- **Debounced button input** with edge detection for reliable start/stop

## How It Works

1. Press the button to start charging (`SYSTEM STANDBY` → `CHARGING...`).
2. While charging, the system polls the gas and flame sensors continuously and the temperature sensor every 2 seconds.
3. If gas level exceeds the threshold, flame is detected, or temperature exceeds the limit, a **thermal event** is triggered:
   - Charging power (LED/relay output) is cut
   - Buzzer sounds continuously
   - OLED displays a critical warning screen
4. Pressing the button again acknowledges and clears the thermal event, returning the system to standby.

## Hardware Required

| Component | Purpose |
|---|---|
| Arduino Uno | Main controller |
| SSD1306 OLED Display (128x64, I2C) | Status display |
| MQ135 Gas Sensor | Air quality / gas leak detection |
| Flame Sensor (digital) | Fire detection |
| DHT11 Temperature Sensor | Ambient temperature monitoring |
| Push Button | Start/stop charging, clear alerts |
| Buzzer | Audible alert |
| LED (charge indicator) | Visual charging status / power line simulation |

## Pin Connections

| Pin | Component |
|---|---|
| A0 | MQ135 Gas Sensor |
| D2 | Flame Sensor |
| D3 | DHT11 Data |
| D4 | Push Button (INPUT_PULLUP) |
| D5 | Buzzer |
| D6 | Charge LED |
| SDA/SCL | OLED Display (I2C) |

## Libraries Used

- [Wire](https://www.arduino.cc/en/reference/wire) — built-in
- [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) (Adafruit)

Install these via the Arduino IDE Library Manager before uploading.

## Configuration

Thresholds can be tuned in the code to match your sensor calibration and environment:

```cpp
const int GAS_THRESHOLD = 400;   // MQ135 analog reading
const float TEMP_LIMIT = 45.0;   // Degrees Celsius
```

## Setup & Usage

1. Wire the components as per the pin table above.
2. Install the required libraries.
3. Upload `sketch.ino` to the Arduino Uno.
4. Open Serial Monitor at 9600 baud to view live sensor readings (optional, for debugging).
5. Press the button to begin charging simulation and monitor the OLED for live status.

## Future Improvements

- RFID-based charging authorization
- Relay-based main power contactor for real load switching
- Wireless alerting (ESP8266/NodeMCU) for remote monitoring
- Data logging for post-event analysis

## Contributors

- Chethana
- Srivarshini
- Jananya

**M.S. Ramaiah University of Applied Sciences**
