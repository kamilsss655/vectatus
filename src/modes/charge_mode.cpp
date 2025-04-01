/* Copyright 2025 kamilsss655
 * https://github.com/kamilsss655
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#ifdef DEBUG_MODE
#include <Arduino.h>
#endif

#include "charge_mode.h"
#include "cc_mode.h"
#include "cv_mode.h"
#include "app.h"
#include "drivers/led.h"
#include "settings.h"
#include "system.h"

// Local functions
static void init_leds();
static void toggle_config_leds();
static void toggle_output_status_led();
static void start_charging(ChargeMode_t *chargeMode);
static void standby(ChargeMode_t *chargeMode);
static void finish_charging(ChargeMode_t *chargeMode);

// Local charge mode struct
static ChargeMode_t chargeModeLocal;

// Map minimum voltages in mV - charging will not start if battery doesn't have such voltage
static const uint16_t minimumVoltage[] = {
    TO_MILI(1.1), // alkaline / nimh
    TO_MILI(2.0), // 3V coin battery?
    TO_MILI(3.0), // LIPO cell voltage
    TO_MILI(4.0),
    TO_MILI(6.0), // 2S LIPO ? 3V×2
    TO_MILI(10.0),  // 12V car battery
    TO_MILI(15.0)}; // 18V power tool battery, 3V*5 = 15V

// Map maximum (fully charged) voltages in mV
static const uint16_t maximumVoltage[] = {
    TO_MILI(1.48), // alkaline / nimh
    TO_MILI(3.1),  // 3V coin battery?
    TO_MILI(4.12), // LIPO cell
    TO_MILI(5.0),
    TO_MILI(8.24), // 2S LIPO ? 4,12V×2
    TO_MILI(13.8),  // 12V car battery
    TO_MILI(20.0)}; // 18V power tool battery, 4V*5 = 20V

/// @brief Initialize CC mode
void CHARGE_MODE_Init()
{
  gApp.duty_cycle = 0;

  // Setup regulation period
  // in order to have accurate output voltage reading during charging
  // the regulation loop needs to be slowed down
  chargeModeLocal.regulation_period_10ms = 7; // 7*10ms = 70ms

  // Setup CC mode
  chargeModeLocal.internal_var.cc_mode.current = CC_MODE_CurrentSettingToMa(gSettings.charge_mode.current);
  chargeModeLocal.internal_var.cc_mode.max_current_ripple = TO_MILI(1.0);
  chargeModeLocal.internal_var.cc_mode.soft_start_step_up_current = TO_MILI(0.001);
  chargeModeLocal.internal_var.cc_mode.soft_start_period_10ms = 5; // 5 -> 5*10ms=50ms
  chargeModeLocal.internal_var.cc_mode.snub_power = 0;
  chargeModeLocal.internal_var.cc_mode.cv_mode_switch_hysteresis = 20; // 20mV - determines CC->CV switch behavior when charge is near complete
  chargeModeLocal.internal_var.cc_mode.internal_var.previous_current = MAX_OUTPUT_CURRENT;
  chargeModeLocal.internal_var.cc_mode.state = CC_MODE_STATE_SOFT_START;

  // Setup CC-CV mode
  chargeModeLocal.internal_var.cc_mode.internal_var.cv_mode.voltage = CHARGE_MODE_MaximumVoltageToMv(gSettings.charge_mode.voltage);
  chargeModeLocal.internal_var.cc_mode.internal_var.cv_mode.max_voltage_ripple = TO_MILI(4.0);
  // disable CV snubbing (required for charging)
  chargeModeLocal.internal_var.cc_mode.internal_var.cv_mode.snub_power = 0;
  chargeModeLocal.internal_var.cc_mode.internal_var.cv_mode.soft_start_period_10ms = 5; // 5 -> 5*10ms=50ms
  chargeModeLocal.internal_var.cc_mode.internal_var.cv_mode.state = CV_MODE_STATE_ON;

  // clear leds
  LED_Clear();
  init_leds();
}

/// @brief Regulate output in charge mode
/// @param chargeMode pointer to options struct
void CHARGE_MODE_Regulate(ChargeMode_t *chargeMode)
{
  // Get current time
  chargeMode->internal_var.current_time_10ms = SYSTEM_10millis();

  // if not enough time passed since last regulation
  if (chargeMode->internal_var.current_time_10ms - chargeMode->internal_var.last_regulation_10ms < chargeMode->regulation_period_10ms)
  {
    // exit early
    return;
  }

  // if output is turned off or charging finished exit early
  if (!gSettings.output || chargeMode->state == CHARGE_MODE_FINISHED)
  {
    return;
  }

  if (chargeMode->state == CHARGE_MODE_IDLE)
  {
    // check if battery voltage is higher than safe threshold
    if (gApp.output_voltage >= CHARGE_MODE_MinimumVoltageToMv(gSettings.charge_mode.voltage))
    {
      start_charging(chargeMode);
    }
    else
    {
      // indicate that battery is deeply discharged and charging cannot be started for safety
      gLed.error = 1;
      gLed.needs_update = 1;
      return;
    }
  }

  // if battery is charged switch to standby mode (battery will still get trickle charged)
  // output current must be <= 10% of programmed current, and output voltage must be reached
  if ((chargeMode->state == CHARGE_MODE_CHARGING) && (gApp.output_current <= (chargeMode->internal_var.cc_mode.current / 20)) && (gApp.output_voltage >= chargeMode->internal_var.cc_mode.internal_var.cv_mode.voltage))
  {
    standby(chargeMode);
  }

  CC_MODE_Regulate(&chargeMode->internal_var.cc_mode);
}

void CHARGE_MODE_Tick()
{
  CHARGE_MODE_Regulate(&chargeModeLocal);
}
void CHARGE_MODE_TimeSlice10ms()
{
}
void CHARGE_MODE_TimeSlice100ms()
{
}
void CHARGE_MODE_TimeSlice500ms()
{
  toggle_config_leds();

  if (chargeModeLocal.state == CHARGE_MODE_CHARGING)
    toggle_output_status_led();
}
void CHARGE_MODE_TimeSlice1000ms()
{
  if (chargeModeLocal.state == CHARGE_MODE_STANDBY)
    toggle_output_status_led();
}

void CHARGE_MODE_ModeBtnPressed()
{
  APP_OutputToggle();
#ifdef DEBUG_MODE
  Serial.println("charge mode: mode btn pressed");
#endif
}
void CHARGE_MODE_ModeBtnHeld()
{
  APP_NextMode();
#ifdef DEBUG_MODE
  Serial.println("charge mode: mode btn held");
#endif
}
void CHARGE_MODE_OutputBtnPressed()
{
  if (gSettings.charge_mode.current < CC_MODE_CURRENT_1500MA)
  {
    gSettings.charge_mode.current = (CcModeCurrent_t)(gSettings.charge_mode.current + 1);
  }
  else
  {
    gSettings.charge_mode.current = CC_MODE_CURRENT_2MA;
  }
  // Re-initialize with new params
  CHARGE_MODE_Init();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;

#ifdef DEBUG_MODE
  Serial.println("charge mode: output btn pressed");
#endif
}
void CHARGE_MODE_OutputBtnHeld()
{
  if (gSettings.charge_mode.voltage < CV_MODE_VOLTAGE_18V)
  {
    gSettings.charge_mode.voltage = (CvModeVoltage_t)(gSettings.charge_mode.voltage + 1);
  }
  else
  {
    gSettings.charge_mode.voltage = CV_MODE_VOLTAGE_1_5V;
  }
  // Re-initialize with new params
  CHARGE_MODE_Init();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;
#ifdef DEBUG_MODE
  Serial.println("charge mode: output btn held");
#endif
}

// Initialize LED to show status
static void init_leds()
{
  gLed.cc = 1;
  gLed.charge = 1;
  switch (gSettings.charge_mode.current)
  {
  case CC_MODE_CURRENT_2MA:
    gLed.x1 = 1;
    break;
  case CC_MODE_CURRENT_100MA:
    gLed.x2 = 1;
    break;
  case CC_MODE_CURRENT_250MA:
    gLed.x3 = 1;
    break;
  case CC_MODE_CURRENT_500MA:
    gLed.x4 = 1;
    break;
  case CC_MODE_CURRENT_750MA:
    gLed.x5 = 1;
    break;
  case CC_MODE_CURRENT_1000MA:
    gLed.x6 = 1;
    break;
  case CC_MODE_CURRENT_1500MA:
    gLed.x7 = 1;
    break;
  default:
    break;
  }
  // request updates
  gLed.needs_update = 1;
}

// Toggle LED state based on current config
static void toggle_config_leds()
{
  // toggle the led
  gLed.cv = !gLed.cv;

  switch (gSettings.charge_mode.voltage)
  {
  case CV_MODE_VOLTAGE_1_5V:
    gLed.x1 = !gLed.x1;
    break;
  case CV_MODE_VOLTAGE_3V:
    gLed.x2 = !gLed.x2;
    break;
  case CV_MODE_VOLTAGE_3_7V:
    gLed.x3 = !gLed.x3;
    break;
  case CV_MODE_VOLTAGE_5V:
    gLed.x4 = !gLed.x4;
    break;
  case CV_MODE_VOLTAGE_9V:
    gLed.x5 = !gLed.x5;
    break;
  case CV_MODE_VOLTAGE_12V:
    gLed.x6 = !gLed.x6;
    break;
  case CV_MODE_VOLTAGE_18V:
    gLed.x7 = !gLed.x7;
    break;
  default:
    break;
  }

  // request updates
  gLed.needs_update = 1;
}

// Toggle LED state based output status
static void toggle_output_status_led()
{
  // blink the X8 led to indicate output on
  if (gSettings.output)
  {
    gLed.x8 = !gLed.x8;
  }
}

// state machine start CC charging action
static void start_charging(ChargeMode_t *chargeMode)
{
  chargeMode->state = CHARGE_MODE_CHARGING;
#ifdef EXTRA_DEBUG_MODE
  Serial.println(F("charge mode: starting"));
#endif
}

// state machine standby action
static void standby(ChargeMode_t *chargeMode)
{
  chargeMode->state = CHARGE_MODE_STANDBY;
#ifdef EXTRA_DEBUG_MODE
  Serial.println(F("charge mode: standby"));
#endif
}

// state machine finish charging action
static void finish_charging(ChargeMode_t *chargeMode)
{
  chargeMode->state = CHARGE_MODE_FINISHED;
  gApp.duty_cycle = 0;
  gSettings.output = false;
#ifdef EXTRA_DEBUG_MODE
  Serial.println(F("charge mode: finished"));
#endif
}

/// @brief Get minimum voltage required in mV from CvModeVoltage_t
/// @param voltage CV mode voltage type
/// @return voltage in mV
uint16_t CHARGE_MODE_MinimumVoltageToMv(CvModeVoltage_t voltage)
{
  return minimumVoltage[voltage];
}

/// @brief Get maximum voltage (fully charged) in mV from CvModeVoltage_t
/// @param voltage CV mode voltage type
/// @return voltage in mV
uint16_t CHARGE_MODE_MaximumVoltageToMv(CvModeVoltage_t voltage)
{
  return maximumVoltage[voltage];
}
