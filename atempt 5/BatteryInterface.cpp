#include "BatteryInterface.h"
#include <Arduino.h>
#include <Wire.h>

// Adafruit fuel gauge
#include <Adafruit_MAX1704X.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// Default I2C addresses if the header didn't define them
#ifndef MAX17048_ADDR
#define MAX17048_ADDR 0x36
#endif

#ifndef IP5306_ADDR
#define IP5306_ADDR 0x75
#endif

// Optional compile-time overrides for ADC battery sensing
// #define BATTERY_ADC_PIN   35     // example: GPIO35
// #define BATTERY_DIVIDER_RATIO 2.0f   // Vbatt = ADC_voltage * ratio (e.g., 2:1 divider)
// #define BATTERY_EMPTY_MV 3000    // 3.0V = 0%
// #define BATTERY_FULL_MV  4200    // 4.2V = 100%

// Local (translation-unit) instance of the MAX17048 driver.
static Adafruit_MAX17048 s_fuelGauge;

// ------- Helpers -------
static bool i2cPing(uint8_t addr) {
  Wire.beginTransmission(addr);
  uint8_t err = Wire.endTransmission(true);
  return (err == 0);
}

// Try to read the battery voltage in millivolts via ADC if a pin is provided.
static int readBatteryVoltageMilliVolts() {
#ifdef BATTERY_ADC_PIN
  // Ensure ADC is initialized; Arduino core handles this lazily.
  // Read raw
  int raw = analogRead(BATTERY_ADC_PIN);
  if (raw < 0) raw = 0;

  // Convert raw to millivolts. On ESP32, analogRead range is 0..4095 by default (12-bit).
  // ADC reference/attenuation vary; if you have board specifics, adjust here.
  // Assume default 11dB attenuation (~1100 mV to ~3100 mV range at pin). Many boards differ,
  // so we keep a conservative scale using analogReadMilliVolts() when available.
  #if ARDUINO_USB_CDC_ON_BOOT || defined(analogReadMilliVolts)
    // Some cores support per-pin mv reading
    int pin_mv = analogReadMilliVolts(BATTERY_ADC_PIN);
  #else
    // Fallback estimate: map raw 0..4095 to 0..1100mV (very conservative).
    // You should calibrate for your board by replacing this scale.
    int pin_mv = (raw * 1100) / 4095;
  #endif

  // Apply external divider (if present). Default assume 2:1 divider if not specified.
  float ratio =
  #ifdef BATTERY_DIVIDER_RATIO
      BATTERY_DIVIDER_RATIO;
  #else
      2.0f;
  #endif

  int vbatt_mv = (int)(pin_mv * ratio + 0.5f);
  return vbatt_mv;  // may be ~3000..4200 for LiPo
#else
  return -1; // ADC not configured
#endif
}

static int8_t percentFromMilliVolts(int mv) {
  // Typical LiPo curve (simple linear clamp). You can swap this for a better curve.
  const int empty_mv =
  #ifdef BATTERY_EMPTY_MV
    BATTERY_EMPTY_MV;
  #else
    3000; // 3.0V => 0%
  #endif

  const int full_mv  =
  #ifdef BATTERY_FULL_MV
    BATTERY_FULL_MV;
  #else
    4200; // 4.2V => 100%
  #endif

  if (mv <= 0) return -1; // indicates invalid measurement
  if (mv <= empty_mv) return 0;
  if (mv >= full_mv) return 100;
  // Linear interpolation
  int num = (mv - empty_mv) * 100;
  int den = (full_mv - empty_mv);
  if (den <= 0) return -1;
  int pct = (num + den/2) / den; // rounded
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return (int8_t)pct;
}

// ------- BatteryInterface -------
BatteryInterface::BatteryInterface() {
  // Members assumed to have in-class default initializers in the header
}

void BatteryInterface::RunSetup() {
  // Some cores expose Wire.isEnabled(); others don't. Guard defensively.
  #if defined(ESP8266)
    if (!Wire.isEnabled()) { Wire.begin(); }
  #else
    Wire.begin(); // harmless if already begun
  #endif

  i2c_supported = true;  // If begin didn't throw, assume we can use I2C

  // Detect MAX17048 fuel gauge
  has_max17048 = i2cPing(MAX17048_ADDR);
  if (has_max17048) {
    if (!s_fuelGauge.begin(&Wire)) {  // FIX: pass pointer, not reference
      has_max17048 = false;
    } else {
      s_fuelGauge.quickStart(); // helps first reading converge sooner
    }
  }

  // Detect IP5306 (optional; many ESP32 battery boards use it)
  has_ip5306 = i2cPing(IP5306_ADDR);

  // Seed levels to a real value so we don't start at -1.
  int8_t lvl = getBatteryLevel();
  if (lvl < 0) lvl = 50; // neutral default if nothing readable
  battery_level = lvl;
  old_level = lvl;
}

void BatteryInterface::main(uint32_t currentTime) {
  // Poll every ~3 seconds
  static uint32_t last = 0;
  if (currentTime - last < 3000U) return;
  last = currentTime;

  int8_t lvl = getBatteryLevel();
  if (lvl < 0) {
    // keep last known level; do not regress to -1
    lvl = old_level;
  }
  if (lvl != old_level) {
    Serial.print(F("Battery level: "));
    Serial.println(lvl);
    old_level = lvl;
    battery_level = lvl;
  }
}

int8_t BatteryInterface::getBatteryLevel() {
  // Preferred: MAX17048 gauge
  if (has_max17048) {
    float pct = s_fuelGauge.cellPercent(); // 0..100 (float)
    if (!isnan(pct)) {
      if (pct < 0.0f) pct = 0.0f;
      if (pct > 100.0f) pct = 100.0f;
      return static_cast<int8_t>(pct + 0.5f);
    }
    // Fallthrough to ADC if gauge read failed
  }

  // Fallback: estimate from ADC
  int mv = readBatteryVoltageMilliVolts();
  int8_t est = percentFromMilliVolts(mv);
  if (est >= 0) return est;

  // Final fallback: keep last value or neutral 50 if none
  if (old_level >= 0 && old_level <= 100) return old_level;
  return 50;
}
