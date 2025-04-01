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

#include "cc_mode.h"
#include "cv_mode.h"
#include "app.h"
#include "drivers/led.h"
#include "settings.h"
#include "system.h"

// Local functions
static void protect(CcMode_t *ccMode);
static void soft_start(CcMode_t *ccMode);
static void snub(CcMode_t *ccMode);
static void turn_on(CcMode_t *ccMode);
static void init_leds();
static void toggle_leds();

// Local CC mode struct
static CcMode_t ccModeLocal;

// Map CC mode setting currents in mA
static const uint16_t currentSettings[] = {
    TO_MILI(0.02),
    TO_MILI(0.1),
    TO_MILI(0.25),
    TO_MILI(0.5),
    TO_MILI(0.75),
    TO_MILI(1.0),
    TO_MILI(1.5)};

/// @brief Initialize CC mode
void CC_MODE_Init()
{
  // Setup CC mode
  gApp.duty_cycle = 0;
  ccModeLocal.current = CC_MODE_CurrentSettingToMa(gSettings.cc_mode.current);
  ccModeLocal.max_current_ripple = TO_MILI(1.0);
  ccModeLocal.soft_start_step_up_current = TO_MILI(0.001);
  ccModeLocal.soft_start_period_10ms = 5; // 5 -> 5*10ms=50ms
  ccModeLocal.snub_power = 3;
  ccModeLocal.internal_var.previous_current = MAX_OUTPUT_CURRENT;
  ccModeLocal.cv_mode_switch_hysteresis = 0;
  soft_start(&ccModeLocal);

  // Setup CV mode
  ccModeLocal.internal_var.cv_mode.voltage = CV_MODE_VoltageSettingToMv(gSettings.cc_mode.voltage);
  ccModeLocal.internal_var.cv_mode.max_voltage_ripple = TO_MILI(2.0);
  ccModeLocal.internal_var.cv_mode.snub_power = 3;
  ccModeLocal.internal_var.cv_mode.soft_start_period_10ms = 5; // 5 -> 5*10ms=50ms
  ccModeLocal.internal_var.cv_mode.state = CV_MODE_STATE_ON;

  // clear leds
  LED_Clear();
  init_leds();
}

/// @brief Regulate output in CC mode
/// @param ccMode pointer to options struct
void CC_MODE_Regulate(CcMode_t *ccMode)
{
  // Run protection logic first
  protect(ccMode);

  // if output is turned off exit early
  if (!gSettings.output)
  {
    return;
  }

  // if voltage is higher then desired limit or currently snubbing voltage spike (likely no load connected) do the CV mode loop
  // TODO: Refactor this so CC mode has separate hysteresis param
  if (gApp.output_voltage >= (ccMode->internal_var.cv_mode.voltage + ccMode->cv_mode_switch_hysteresis) || ccMode->internal_var.cv_mode.state == CV_MODE_STATE_SNUB)
  {
    CV_MODE_Regulate(&ccMode->internal_var.cv_mode);
    return;
  }
  // otherwise do the CC loop

  // Get current time
  ccMode->internal_var.current_time_10ms = SYSTEM_10millis();

  // If we are in snub state hold the duty cycle at 0
  if (ccMode->state == CC_MODE_STATE_SNUB)
  {
    // once the current drops to desired snub level
    if (gApp.output_current < ((ccMode->current * (100 - ccMode->snub_power)) / 100))
    {
      // special case when snub power is set to 0, we skip the soft start
      // this might cause oscilations of the output voltage, however might be useful for voltage insensitive loads like motors etc
      if (ccMode->snub_power == 0)
      {
        turn_on(ccMode);
      }
      else
      {
        // move onto soft start
        soft_start(ccMode);
      }
    }
    else
    {
      // otherwise stall the regulation loop, while keeping duty cycle at 0 so output current can drop
      return;
    }
  }

  // if current is below target current and duty cycle can be increased
  if ((gApp.output_current < ccMode->current) && (gApp.duty_cycle < MAX_DUTY_CYCLE))
  {
    // if in soft start state
    if (ccMode->state == CC_MODE_STATE_SOFT_START)
    {
      // if enough time passed since last soft start regulation
      if (ccMode->internal_var.current_time_10ms - ccMode->internal_var.last_soft_regulated_10ms >= ccMode->soft_start_period_10ms)
      {
        // save te time of last regulation
        ccMode->internal_var.last_soft_regulated_10ms = ccMode->internal_var.current_time_10ms;
        // during soft-start only increase duty cycle if we detect out current not increasing
        // this technique pumps up load capacitors slowly preventing rushing the duty cycle up, before load capacitors have been charged
        if (gApp.output_current <= ccMode->internal_var.previous_current + ccMode->soft_start_step_up_current)
          gApp.duty_cycle += 1;
      }
    }
    else
    {
      gApp.duty_cycle += 1;
    }
  }
  // if output current is too high, and duty cycle can be lowered
  else if ((gApp.output_current > ccMode->current) && (gApp.duty_cycle > MIN_DUTY_CYCLE))
  {
    gApp.duty_cycle -= 1;
    // Disable soft start and turn on
    if (ccMode->state == CC_MODE_STATE_SOFT_START)
    {
      turn_on(ccMode);
    }
  }

  if ((gApp.output_current >= ccMode->current) || (gApp.duty_cycle == MAX_DUTY_CYCLE))
  {
    // turn off soft start once the desired current or max duty cycle is reached
    if (ccMode->state == CC_MODE_STATE_SOFT_START)
    {
      turn_on(ccMode);
    }
  }
  // Save last output current for the next iteration
  ccMode->internal_var.previous_current = gApp.output_current;
}

void CC_MODE_Tick()
{
  CC_MODE_Regulate(&ccModeLocal);
}
void CC_MODE_TimeSlice10ms()
{
}
void CC_MODE_TimeSlice100ms()
{
}
void CC_MODE_TimeSlice500ms()
{
  toggle_leds();
}
void CC_MODE_TimeSlice1000ms()
{
}

void CC_MODE_ModeBtnPressed()
{
  APP_OutputToggle();
#ifdef DEBUG_MODE
  Serial.println("cc mode: mode btn pressed");
#endif
}
void CC_MODE_ModeBtnHeld()
{
  APP_NextMode();
#ifdef DEBUG_MODE
  Serial.println("cc mode: mode btn held");
#endif
}
void CC_MODE_OutputBtnPressed()
{
  if (gSettings.cc_mode.current < CC_MODE_CURRENT_1500MA)
  {
    gSettings.cc_mode.current = (CcModeCurrent_t)(gSettings.cc_mode.current + 1);
  }
  else
  {
    gSettings.cc_mode.current = CC_MODE_CURRENT_2MA;
  }
  // Re-initialize with new params
  CC_MODE_Init();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;

#ifdef DEBUG_MODE
  Serial.println("cc mode: output btn pressed");
#endif
}
void CC_MODE_OutputBtnHeld()
{
  if (gSettings.cc_mode.voltage < CV_MODE_VOLTAGE_18V)
  {
    gSettings.cc_mode.voltage = (CvModeVoltage_t)(gSettings.cc_mode.voltage + 1);
  }
  else
  {
    gSettings.cc_mode.voltage = CV_MODE_VOLTAGE_1_5V;
  }
  // Re-initialize with new params
  CC_MODE_Init();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;
#ifdef DEBUG_MODE
  Serial.println("cc mode: output btn held");
#endif
}

// CC mode specific protections
static void protect(CcMode_t *ccMode)
{
  // if the output current spikes beyond acceptable ouput current ripple (i.e load disconnected abruptly)
  // go to fail mode
  if (ccMode->state == CC_MODE_STATE_ON)
  {
    if (gApp.output_current > ccMode->current + ccMode->max_current_ripple)
    {
#ifdef DEBUG_MODE
      Serial.print("cc mode: output overcurrent protection triggered. current reached:  ");
      Serial.println(gApp.output_current);
#endif
      snub(ccMode);
      // turn on error LED - indicating that output current is not good yet
      gLed.error = 1;
      gLed.needs_update = 1;
    }
  }
}

// state machine soft start action
static void soft_start(CcMode_t *ccMode)
{
  gApp.duty_cycle = 0;
  ccMode->state = CC_MODE_STATE_SOFT_START;
#ifdef DEBUG_MODE
  Serial.println("cc: enabling soft start");
#endif
}

// state machine snub action
static void snub(CcMode_t *ccMode)
{
  // turn duty cycle to 0
  gApp.duty_cycle = 0;
  ccMode->state = CC_MODE_STATE_SNUB;
#ifdef DEBUG_MODE
  Serial.println("cc: enabling snub");
#endif
}

// state machine turn on action
static void turn_on(CcMode_t *ccMode)
{
  ccMode->state = CC_MODE_STATE_ON;
#ifdef DEBUG_MODE
  Serial.println("cc: turn on");
#endif
  // turn off error LED - indicating power good
  gLed.error = 0;
  gLed.needs_update = 1;
}

/// @brief Convert CcModeCurrent_t setting to mA
/// @param current CcModeCurrent_t setting
/// @return current in mA
uint32_t CC_MODE_CurrentSettingToMa(CcModeCurrent_t current)
{
  return currentSettings[current];
}

// Initialize LED to show status
static void init_leds()
{
  gLed.cc = 1;
  switch (gSettings.cc_mode.current)
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

// Toggle LED state based on current status
static void toggle_leds()
{
  // toggle the led
  gLed.cv = !gLed.cv;

  switch (gSettings.cc_mode.voltage)
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

  // blink the X8 led to indicate output on
  if (gSettings.output)
  {
    gLed.x8 = !gLed.x8;
  }

  // request updates
  gLed.needs_update = 1;
}
