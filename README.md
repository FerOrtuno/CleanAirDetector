# CleanAirDetector Firmware

Smart firmware for MOS-based Air Quality Monitors (MQ-135), designed to correct thermal drift and provide stable readings using environmental compensation.

[🇪🇸 Versión en Español](README_es.md)

## Key Features

- **Finite State Architecture**: Robust management of the sensor lifecycle (Boot -> Warmup -> Calibration -> Run).
- **Controlled Warmup**: Mandatory 15-minute cycle with on-screen countdown to thermally stabilize the sensor heating element.
- **"Outdoor Reference" Calibration**: One-point inverse calibration algorithm that fixes the baseline to **427.20 ppm** (approximate global CO2/Clean Air average) upon completion of the warmup in outdoor air.
- **Thermal Compensation**: Integration with **DHT22** sensor to dynamically adjust the MQ-135 adjustment factor based on temperature and humidity, preventing false positives when moving the device from cold outdoors to warm indoors.
- **Qualitative Quality Indicator**: 4-level system for easy and safe air interpretation:
  - **OPTIMA (EXT)**: < 600 ppm (Pure/Outdoor Air)
  - **BUENA (INT)**: 600 - 1000 ppm (Healthy Indoor Air)
  - **REGULAR**: 1000 - 1500 ppm (Ventilation Recommended)
  - **MALA**: > 1500 ppm (Ventilation Required)

## Hardware Requirements

- **Microcontroller**: Arduino Uno/Nano or compatible.
- **Gas Sensor**: MQ-135 Module (Pin A2).
  - *Note*: Verify your module's Load Resistance (RL). Configurable in `config.h` (Default: 1.0k).
- **T/H Sensor**: DHT22 or DHT11 (Pin Digital 2).
- **Display**: LCD 1602 with I2C interface (Address 0x27).
- **Button (Optional)**: Momentary push button for manual recalibration (Pin Digital 4 to GND).

## Installation

1. **Required Libraries**:
   - `LiquidCrystal_I2C`
   - `DHT sensor library` (Adafruit)

2. **Wiring**:

   | Component  | Arduino Pin |
   |------------|-------------|
   | MQ-135 AOUT| A2          |
   | DHT22 Data | D2          |
   | LCD SDA    | A4 (SDA)    |
   | LCD SCL    | A5 (SCL)    |
   | Button     | D4 (to GND) |

3. **Configuration**:
   Edit `config.h` to adjust parameters such as:
   - `WARMUP_MINUTES`: Warmup time.
   - `RL_VALUE`: Load resistance of your MQ board (Set to 1.0 for many generic modules).
   - Air quality thresholds.

## Usage

1. **Power On**: Turn on the device outdoors or near an open window ("Clean Air").
2. **Warmup**: Wait for the **PREHEATING** cycle to complete (15 min).
3. **Calibration**: The system will display **"Calibrado!"** (Calibrated) and switch to operation mode.
4. **Operation**: You can now bring the device indoors.
5. **Recalibration**: If readings drift (e.g., due to burn-in of a sensor that has been off for a long time), keep the sensor in clean air and press the Recalibration Button (or restart).

## License

Open Source.
