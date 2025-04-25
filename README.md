# moto_chain_auto_oiler


# ESP32 Hall Sensor Speedometer

This project is designed to measure the rotational speed of a wheel using an analog Hall effect sensor (such as 49E) connected to an ESP32 board. The sensor detects the passing of a magnet attached to the wheel, and the speed is calculated and printed to the Serial Monitor in kilometers per hour (km/h).

## Features

- 📏 Calculates wheel speed in km/h based on magnetic pulses
- 💡 Built-in LED indicator turns on when the magnet is detected
- 🕒 Speed is updated every second
- 📟 Serial Monitor output for real-time monitoring

## Hardware Requirements

- ESP32 microcontroller
- Analog Hall effect sensor (e.g. 49E)
- Magnet (attached to the rotating wheel)
- Jumper wires
- (Optional) Pull-down resistor for the sensor
- LED (optional, if not using the built-in one on GPIO2)

## Wiring

| Component        | ESP32 Pin        |
|------------------|------------------|
| Hall Sensor OUT  | GPIO34 (analog)  |
| Hall Sensor VCC  | 3.3V             |
| Hall Sensor GND  | GND              |
| Built-in LED     | GPIO2 (default)  |

> 💡 The magnet should pass close to the Hall sensor once per rotation.

## Configuration

In `main.ino` (or the `.cpp` file if used in PlatformIO), you can configure:

```cpp
const int hallPin = 34;         // analog input pin
const int threshold = 3000;     // detection threshold (adjust depending on your sensor)
const float wheelDiameter = 0.6; // diameter of the wheel in meters
