#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Configuration ---
#define PIN_MQ A2      // Analog pin for MQ-135 (User specified)
#define PIN_BTN 4      // Optional button moved to D4 (was D2, now used by DHT)
#define LED_BUILTIN 13 // Status LED

// LCD I2C Configuration
#define LCD_ADDR 0x27 // Common I2C address for 1602 LCDs
#define LCD_COLS 16
#define LCD_ROWS 2

// --- Sensor Parameters ---
// RL: Load Resistance on the board (Verify your module! Often 1k or 10k)
// 10.0 = 10k kOhm
#define RL_VALUE 1.0

// Standard MQ-135 coefficients for CO2
// Curve: PPM = A * (Rs/R0)^B
#define COEF_A 110.47
#define COEF_B -2.862

// --- Calibration Settings ---
// Time to wait before first calibration (Warmup)
#define WARMUP_MINUTES 15
#define WARMUP_MS (WARMUP_MINUTES * 60 * 1000UL)

// DHT22 Sensor Configuration
#define PIN_DHT 2 // Digital Pin for DHT22 (User specified)
#define DHT_TYPE DHT22

// Thermal Compensation Factors
// Reference conditions (Datasheet standard)
#define TEMP_REF 20.0
#define HUM_REF 33.0 // Standard RH for MQ datasheets vary, typically 33% or 65%

// Target PPM for Outdoor Calibration (Clean Air Reference)
// Global average is ~420ppm. We use a precise float.
#define TARGET_OUTDOOR_PPM 427.20f

// --- Air Quality Thresholds (PPM) ---
// These are approximate values for indoor air quality context
#define THRESHOLD_CLEAN_MAX 600    // < 600: Outdoor / Very Pure
#define THRESHOLD_GOOD_MAX 1000    // 601-1000: Good Indoor (Ventilated)
#define THRESHOLD_REGULAR_MAX 1500 // 1001-1500: Regular (Monitor)
// > 1500: Bad (Ventilate Now)

// --- Filter Settings ---
#define SAMPLES_FOR_AVERAGE 10 // Number of samples for smoothing reading
#define SAMPLE_DELAY_MS 100    // Delay between samples

#endif // CONFIG_H
