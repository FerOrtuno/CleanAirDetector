/**
 * CleanAirDetector.ino
 * Firmware for MQ-135 based Air Quality Monitor
 *
 * Features:
 * - 15 Minute Warmup State (Mandatory for MOS sensors)
 * - Outdoor Calibration (One-Point) to Target Reference
 * - Quality-based Display (Good/Regular/Poor)
 * - State Machine Architecture
 */

#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <DHT.h>

// --- Global Objects ---
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
DHT dht(PIN_DHT, DHT_TYPE);

// --- State Definitions ---
enum SystemState {
  STATE_BOOT,
  STATE_WARMUP,
  STATE_CALIBRATION,
  STATE_RUN,
  STATE_ERROR
};

SystemState currentState = STATE_BOOT;
unsigned long timeStartWarmup = 0;
float R0 = 0.0; // Calibrated Base Resistance

// --- Helper Functions ---

/**
 * Reads sensor resistance (Rs)
 * Returns resistance in kOhms or -1.0 if error
 */
float readResistance() {
  int adc = analogRead(PIN_MQ);

  // Hardware sanity check (Open/Short circuit detection)
  if (adc <= 5 || adc >= 1020)
    return -1.0;

  float voltage = adc * (5.0 / 1023.0);
  if (voltage == 0)
    return -1.0; // Avoid division by zero

  // Rs = RL * ( (Vc - Vout) / Vout )
  float rs = RL_VALUE * ((5.0 - voltage) / voltage);
  return rs;
}

/**
 * Reads an average resistance over N samples
 */
float readResistanceAverage() {
  float sum = 0;
  int validSamples = 0;
  for (int i = 0; i < SAMPLES_FOR_AVERAGE; i++) {
    float r = readResistance();
    if (r > 0) {
      sum += r;
      validSamples++;
    }
    delay(SAMPLE_DELAY_MS);
  }
  if (validSamples == 0)
    return -1.0;
  return sum / validSamples;
}

/**
 * Calculates Correction Factor based on Temperature and Humidity
 * Based on MQ-135 Datasheet sensitivity curve approximation
 * @param t Temperature in Celsius
 * @param h Rel. Humidity in %
 */
float getCorrectionFactor(float t, float h) {
  // Simple polynomial approximation typically used for MQ sensors
  // Cor = (0.00035 * t * t) - (0.02718 * t) + 1.39538 - (0.0018 * h)
  // Warning: This is a generic approximation.

  // Normalized Logic:
  // If T < 20 (Cold), Rs increases -> Factor > 1 to reduce it back
  // If T > 20 (Hot), Rs decreases -> Factor < 1 to increase it back

  // Using a simplified linear model for stability if no complex curve data:
  // -0.01 per degree over 20
  // -0.005 per % RH over 33

  // return 1.0 - (0.01 * (t - TEMP_REF)) - (0.005 * (h - HUM_REF));

  // Using the more robust formula often found in MQUnifiedSensor lib:
  if (t < -20)
    t = -20;
  if (t > 50)
    t = 50;
  // This formula models the "Rs/Ro vs T/RH" graph
  float factor = (0.000002118 * pow(t, 3)) - (0.00017463 * pow(t, 2)) +
                 (0.0064975 * t) + 0.9053;
  // Humidity impact is often simpler linear offset
  float hum_impact = (h - 33.0) * 0.003;
  return factor - hum_impact;
}

/**
 * Calculate PPM based on current Rs and calibrated R0
 * Applies Thermal Compensation if DHT is working
 */
float calculatePPM(float rs, float t, float h) {
  if (R0 <= 0)
    return 0;

  float correction = 1.0;
  // If sensor values are valid, calculate correction
  if (!isnan(t) && !isnan(h)) {
    correction = getCorrectionFactor(t, h);
  }

  // Rs_Corrected = Rs / CorrectionFactor
  float rs_corrected = rs / correction;

  float ratio = rs_corrected / R0;
  // PPM = A * (Rs/R0)^B
  return COEF_A * pow(ratio, COEF_B);
}

/**
 * Inverse Calibration: Find R0 given current Rs and known Target PPM
 * R0 = Rs / ( (Target / A)^(1/B) )
 */
float calculateR0(float rs_actual, float target_ppm) {
  float term = target_ppm / COEF_A;
  float exponent = 1.0 / COEF_B;
  float denominator = pow(term, exponent);
  return rs_actual / denominator;
}

void setup() {
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  pinMode(PIN_MQ, INPUT);
  pinMode(PIN_BTN, INPUT_PULLUP); // Button active LOW
  dht.begin();                    // Start DHT Sensor

  // Initial Message
  lcd.setCursor(0, 0);
  lcd.print("CleanAir Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Ver: 1.1");
  delay(2000);

  currentState = STATE_BOOT;
}

void loop() {
  switch (currentState) {

  // --- BOOT STATE ---
  case STATE_BOOT:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SYSTEM START");
    lcd.setCursor(0, 1);
    lcd.print("Outdoor Air OK?");
    delay(3000);

    timeStartWarmup = millis();
    currentState = STATE_WARMUP;
    break;

  // --- WARMUP STATE (0-15 min) ---
  case STATE_WARMUP: {
    unsigned long elapsed = millis() - timeStartWarmup;
    long remaining = WARMUP_MS - elapsed;

    if (remaining <= 0) {
      currentState = STATE_CALIBRATION;
      return;
    }

    // Display Warmup Timer
    lcd.setCursor(0, 0);
    lcd.print("WARMING UP...   ");

    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    // Show temp while warming up
    float t = dht.readTemperature();
    if (!isnan(t))
      lcd.print((int)t);
    else
      lcd.print("--");
    lcd.print("C ");

    int mins = remaining / 60000;
    int secs = (remaining % 60000) / 1000;

    if (mins < 10)
      lcd.print("0");
    lcd.print(mins);
    lcd.print(":");
    if (secs < 10)
      lcd.print("0");
    lcd.print(secs);

    // Blink internal LED to show activity
    digitalWrite(LED_BUILTIN, (millis() / 500) % 2);

    delay(500); // UI Refresh rate
  } break;

  // --- CALIBRATION STATE (Point at 15 min) ---
  case STATE_CALIBRATION:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CALIBRATING...");
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");

    {
      // Take a stable reading
      float rs_avg = readResistanceAverage();

      if (rs_avg < 0) {
        currentState = STATE_ERROR;
        return;
      }

      // For calibration we do NOT compensate T/H because we assume Outdoor
      // Standard conditions OR we could compensate to normalize R0 to 20C.
      // Better: Calculate R0 "as is" raw, but when reading later we correct Rs
      // back to this baseline condition. Actually, best practice is to
      // normalize R0 to standard conditions (20C/33%RH).

      float t = dht.readTemperature();
      float h = dht.readHumidity();
      float correction = 1.0;
      if (!isnan(t) && !isnan(h))
        correction = getCorrectionFactor(t, h);

      // Normalize Rs to standard conditions before calculating R0
      float rs_normalized = rs_avg / correction;

      // Calculate R0 forcing the current air to be TARGET_OUTDOOR_PPM
      R0 = calculateR0(rs_normalized, TARGET_OUTDOOR_PPM);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calibrated!");
      lcd.setCursor(0, 1);
      lcd.print("R0: ");
      lcd.print(R0);
      delay(2000); // Show R0 briefly

      currentState = STATE_RUN;
    }
    break;

  // --- RUN STATE (Normal Operation) ---
  case STATE_RUN: {
    float rs = readResistanceAverage();
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (rs < 0) {
      // Sensor disconnected or failed
      lcd.setCursor(15, 0);
      lcd.print("E");
    } else {
      // calculatePPM now takes T and H for compensation
      float ppm = calculatePPM(rs, t, h);

      // Determine Quality
      lcd.setCursor(0, 0);
      lcd.print("QUALITY: ");

      if (ppm <= THRESHOLD_CLEAN_MAX) {
        lcd.print("OPTIMAL(OUT) ");
      } else if (ppm <= THRESHOLD_GOOD_MAX) {
        lcd.print("GOOD (INDOOR)");
      } else if (ppm <= THRESHOLD_REGULAR_MAX) {
        lcd.print("MODERATE     ");
      } else {
        lcd.print("POOR         ");
      }

      // Optional: Show estimated PPM on second line for debug/reference
      lcd.setCursor(0, 1);
      if (!isnan(t)) {
        lcd.print((int)t);
        lcd.print("C ");
      }
      lcd.print("Est: ");
      lcd.print((int)ppm);
      lcd.print("    ");
    }

    delay(1000); // 1 Second refresh

    // Check for Manual Recalibration Button
    if (digitalRead(PIN_BTN) == LOW) { // Assuming Active LOW button
      lcd.clear();
      lcd.print("Recalibrating...");
      delay(1000);
      currentState = STATE_CALIBRATION;
    }
  } break;

  // --- ERROR STATE ---
  case STATE_ERROR:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SENSOR ERROR");
    lcd.setCursor(0, 1);
    lcd.print("Check Connect");

    delay(2000);
    // Try to recover by going back to run (or boot)
    // For safety, we stay in error or retry reading
    float retry = readResistance();
    if (retry > 0) {
      currentState = STATE_RUN; // Recovered
    }
    break;
  }
}
