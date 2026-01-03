# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-01-03

### Added

- **Thermal Compensation**: Implemented polynomial correction algorithm using DHT22 sensor readings (Temp/Hum) to stabilize MQ-135 readings against environmental changes.
- **New Quality Levels**: Refined air quality scale to 4 states: `OPTIMA (<600)`, `GOOD (<1000)`, `REGULAR (<1500)`, `BAD (>1500)`.
- **Recalibration Button**: Support for a physical button (Pin 4) that forces a manual One-Point recalibration without restarting the device.
- **Centralized Configuration**: `config.h` file for easy management of pins, constants, and thresholds.
- **DHT Dependency**: Integrated DHT library for environmental readings.

### Changed

- **Software Architecture**: Complete migration from linear structure to non-blocking **Finite State Machine** (`STATE_BOOT`, `WARMUP`, `CALIBRATE`, `RUN`, `ERROR`).
- **Calibration Strategy**:
  - Previous: Erroneous continuous calibration during warmup.
  - Now: Passive 15-minute "Blind Time" preheating followed by a precise "One-Point Inverse Calibration" to a target of **427.20 ppm**.
- **User Interface**:
  - Visual countdown timer during warmup.
  - Temperature display and PPM estimation on the main screen.

### Fixed

- Eliminated reading drift caused by calibrating the sensor while its internal resistance was changing due to temperature (Warm-up drift).
- Corrected PPM formula to use a fixed `TARGET_OUTDOOR_PPM` instead of generic clean air ratios.
- Adjusted Load Resistance (`RL_VALUE`) default to 1.0k to match common hardware revisions.

## [1.0.0] - Initial Version (Legacy)

- Basic MQ-135 reading.
- LCD Display.
- Simple voltage-based logic without compensation.
