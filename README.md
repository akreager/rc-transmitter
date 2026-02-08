# RC Transmitter
Open-source design and code for a 3D-printed pistol-grip RC transmitter.

![Front](docs/images/MVIMG_20200218_222148.jpg?raw=true)

## Overview
A general-purpose RC transmitter designed to control multiple vehicles by swapping configuration profiles. Currently used with an RC snowblower and an engineering test rover.

### Features
- **Wireless control** via nRF24L01+ with bidirectional telemetry (acknowledge payloads)
- **Touchscreen UI** on a 240x320 TFT with resistive touch input
- **Live telemetry** display: battery voltage, board temperatures, motor currents
- **Motor stop** (M-Stop) soft button on the touchscreen
- **Joystick input** for throttle and steering

### Planned
- Joystick calibration via touchscreen UI
- Physical button matrix (multiplexed)
- SD card configuration profiles for multi-vehicle support
- Settings/setup menu

## Hardware

### Transmitter
- MCU with dual SPI buses (currently Teensy 4.1)
- [Adafruit 2.8" TFT Touch Breakout](https://www.adafruit.com/product/1770) — ILI9341 display with resistive touchscreen (SPI1)
- nRF24L01+ radio module (SPI0)
- Analog joystick (steering + throttle)

### Receiver (test rover)
- Arduino Mega
- nRF24L01+ radio module
- Basicmicro RoboClaw motor controller

## Project Structure
```
RC Transmitter/
├── main/                 # Transmitter firmware (Arduino IDE)
│   ├── main.ino          # Setup, loop, radio, global state
│   ├── config.h          # Pin assignments, constants, shared types
│   ├── touchInput.ino    # Touch state machine
│   ├── screenManager.ino # Screen transitions and button management
│   └── drawScreens.ino   # Screen drawing and UI updates
├── test_sketches/        # Standalone test/reference sketches
├── stl/                  # 3D printed enclosure files
├── docs/                 # Documentation and images
├── LICENSE               # GPLv3
└── README.md
```

## Pin Assignments (Teensy 4.1)

Active pin mapping for the current build. Also maintained in [Google Sheets](https://docs.google.com/spreadsheets/d/1aVeEQQCFw7_oekkrW9wztb_qrHcS88uQesicZp7ygn0/edit?gid=1983508387#gid=1983508387).

### Display & Touch (SPI1)
| Function | Pin  | Notes                   |
|----------|------|-------------------------|
| CLK      | D27  | SPI1 clock              |
| MISO     | D1   | SPI1 data in            |
| MOSI     | D26  | SPI1 data out           |
| TFT CS   | D0   | Display chip select     |
| TFT DC   | D2   | Display data/command    |
| Backlight| D3   | PWM brightness control  |
| Touch Y+ | D24  | Analog (A10)            |
| Touch X- | D25  | Analog (A11)            |
| Touch X+ | D28  | Digital                 |
| Touch Y- | D29  | Digital                 |

### Radio (SPI0)
| Function | Pin  | Notes                   |
|----------|------|-------------------------|
| SCK      | D13  | SPI0 clock (default)    |
| MISO     | D12  | SPI0 data in (default)  |
| MOSI     | D11  | SPI0 data out (default) |
| CE       | D9   |                         |
| CSN      | D10  |                         |

### Inputs
| Function  | Pin  | Notes                              |
|-----------|------|------------------------------------|
| Steering  | D14  | Analog (A0) — wheel input          |
| Throttle  | D15  | Analog (A1) — trigger input        |
| Joystick X| D17  | Analog (A3) — e.g. chute aim L/R  |
| Joystick Y| D16  | Analog (A2) — e.g. chute aim U/D  |

### Button Matrix (planned)
| Function | Pin  | Notes                  |
|----------|------|------------------------|
| Row A    | D30  | Digital                |
| Row B    | D31  | Digital                |
| Row C    | D32  | Digital                |
| Row D    | D33  | Digital                |
| Col 1    | D34  | Digital                |
| Col 2    | D35  | Digital                |

## CAD Source
3D design for the transmitter enclosure is on [OnShape](https://cad.onshape.com/documents/a59ab4c30cdb2f94f953abb2/v/0076fda3a5d6eca29f873ec9/e/06b2695cd32d304990fa8f74).

## Dependencies
Install via Arduino IDE Library Manager:
- [RF24](https://github.com/tmrh20/RF24/) (TMRh20 fork)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit ILI9341](https://github.com/adafruit/Adafruit_ILI9341)
- [Adafruit TouchScreen](https://github.com/adafruit/Adafruit_TouchScreen)
- [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)

Receiver also requires:
- [RoboClaw Arduino Library](https://github.com/basicmicro/roboclaw_arduino_library)

## Development
This project is developed with assistance from Claude (Anthropic), used for code architecture, refactoring, and documentation.

## License
This project is licensed under the GNU General Public License v3.0 — see [LICENSE](LICENSE) for details.
