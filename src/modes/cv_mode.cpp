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

#include "cv_mode.h"
#include "app.h"
#include "drivers/led.h"
#include "settings.h"
#include "system.h"

// Local functions
static void protect(CvMode_t *cvMode);
static void soft_start(CvMode_t *cvMode);
static void snub(CvMode_t *cvMode);
static void turn_on(CvMode_t *cvMode);
static void init_leds();
static void toggle_leds();

// Local CV mode struct
static CvMode_t cvModeLocal;

// Map CV mode setting voltages in mV
static const uint16_t voltageSettings[] = {
    TO_MILI(1.5),
    TO_MILI(3.0),
    TO_MILI(3.7),
    TO_MILI(5.0),
    TO_MILI(9.0),
    TO_MILI(12.0),
    TO_MILI(14.0)};

/// @brief Initialize CV mode
void CV_MODE_Init()
{
  gApp.duty_cycle = 0;
  cvModeLocal.voltage = CV_MODE_VoltageSettingToMv(gSettings.cv_mode.voltage);
  cvModeLocal.max_voltage_ripple = TO_MILI(2.20);
  cvModeLocal.soft_start_step_up_voltage = TO_MILI(0.001);
  cvModeLocal.soft_start_period_10ms = 5; // 5 -> 5*10ms=50ms
  if (gSettings.cv_mode.voltage == CV_MODE_VOLTAGE_5V)
  {
    // No snubbing for 5V mode - messes up USB phone charging
    cvModeLocal.snub_power = 0;
  }
  else
  {
    cvModeLocal.snub_power = 3;
  }
  cvModeLocal.internal_var.previous_voltage = MAX_OUTPUT_VOLTAGE;
  soft_start(&cvModeLocal);

  // clear leds
  LED_Clear();
  init_leds();
}

/// @brief Regulate output in CV mode
/// @param cvMode pointer to options struct
void CV_MODE_Regulate(CvMode_t *cvMode)
{
  // Run protection logic first
  protect(cvMode);

  // if output is turned off exit early
  if (!gSettings.output)
  {
    return;
  }

  // Get current time
  cvMode->internal_var.current_time_10ms = SYSTEM_10millis();

  // TODO: Likely can add MPPT like this:
  // if(gApp.input_voltage < 5000)
  // {
  //   gApp.duty_cycle -= 2;
  // }

  // If we are in snub state hold the duty cycle at 0
  if (cvMode->state == CV_MODE_STATE_SNUB)
  {
    // once the voltage drops to desired snub level
    if (gApp.output_voltage < ((cvMode->voltage * (100 - cvMode->snub_power)) / 100))
    {
      // special case when snub power is set to 0, we skip the soft start
      // this might cause oscilations of the output voltage, however might be useful for voltage insensitive loads like motors etc
      if (cvMode->snub_power == 0)
      {
        turn_on(cvMode);
      }
      else
      {
        // move onto soft start
        soft_start(cvMode);
      }
    }
    else
    {
      // otherwise stall the regulation loop, while keeping duty cycle at 0 so output voltage can drop
      return;
    }
  }

  // if voltage is below target voltage and duty cycle can be increased
  if ((gApp.output_voltage < cvMode->voltage) && (gApp.duty_cycle < MAX_DUTY_CYCLE))
  {
    // if in soft start state
    if (cvMode->state == CV_MODE_STATE_SOFT_START)
    {
      // if enough time passed since last soft start regulation
      if (cvMode->internal_var.current_time_10ms - cvMode->internal_var.last_soft_regulated_10ms >= cvMode->soft_start_period_10ms)
      {
        // save te time of last regulation
        cvMode->internal_var.last_soft_regulated_10ms = cvMode->internal_var.current_time_10ms;
        // during soft-start only increase duty cycle if we detect out voltage not increasing
        // this technique pumps up load capacitors slowly preventing rushing the duty cycle up, before load capacitors have been charged
        if (gApp.output_voltage <= cvMode->internal_var.previous_voltage + cvMode->soft_start_step_up_voltage)
          gApp.duty_cycle += 1;
      }
    }
    else
    {
      gApp.duty_cycle += 1;
    }
  }
  // if output voltage is too high, and duty cycle can be lowered
  else if ((gApp.output_voltage > cvMode->voltage) && (gApp.duty_cycle > MIN_DUTY_CYCLE))
  {
    gApp.duty_cycle -= 1;
    // Disable soft start and turn on
    if (cvMode->state == CV_MODE_STATE_SOFT_START)
    {
      turn_on(cvMode);
    }
  }

  if ((gApp.output_voltage >= cvMode->voltage) || (gApp.duty_cycle == MAX_DUTY_CYCLE))
  {
    // turn off soft start once the desired voltage or max duty cycle is reached
    if (cvMode->state == CV_MODE_STATE_SOFT_START)
    {
      turn_on(cvMode);
    }
  }
  // Save last output voltage for the next iteration
  cvMode->internal_var.previous_voltage = gApp.output_voltage;
}

void CV_MODE_Tick()
{
  CV_MODE_Regulate(&cvModeLocal);
}
void CV_MODE_TimeSlice10ms()
{
}
void CV_MODE_TimeSlice100ms()
{
}
void CV_MODE_TimeSlice500ms()
{
}
void CV_MODE_TimeSlice1000ms()
{
  if (gSettings.output)
  {
    // if output is on - blink leds
    toggle_leds();
  }
}

void CV_MODE_ModeBtnPressed()
{
  APP_OutputToggle();
#ifdef DEBUG_MODE
  Serial.println("cv mode: mode btn pressed");
#endif
}
void CV_MODE_ModeBtnHeld()
{
  APP_NextMode();
#ifdef DEBUG_MODE
  Serial.println("cv mode: mode btn held");
#endif
}
void CV_MODE_OutputBtnPressed()
{
  if (gSettings.cv_mode.voltage < CV_MODE_VOLTAGE_18V)
  {
    gSettings.cv_mode.voltage = (CvModeVoltage_t)(gSettings.cv_mode.voltage + 1);
  }
  else
  {
    gSettings.cv_mode.voltage = CV_MODE_VOLTAGE_1_5V;
  }
  // Re-initialize with new params
  CV_MODE_Init();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;

#ifdef DEBUG_MODE
  Serial.println("cv mode: output btn pressed");
#endif
}
void CV_MODE_OutputBtnHeld()
{
  if (gSettings.cv_mode.voltage > CV_MODE_VOLTAGE_1_5V)
  {
    gSettings.cv_mode.voltage = (CvModeVoltage_t)(gSettings.cv_mode.voltage - 1);
  }
  else
  {
    gSettings.cv_mode.voltage = CV_MODE_VOLTAGE_18V;
  }
  // Re-initialize with new params
  CV_MODE_Init();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;
#ifdef DEBUG_MODE
  Serial.println("cv mode: output btn held");
#endif
}

// CV mode specific protections
static void protect(CvMode_t *cvMode)
{
  // if the output voltage spikes beyond acceptable ouput voltage ripple (i.e load disconnected abruptly)
  // go to fail mode
  if (cvMode->state == CV_MODE_STATE_ON)
  {
    if (gApp.output_voltage > cvMode->voltage + cvMode->max_voltage_ripple)
    {
#ifdef DEBUG_MODE
      Serial.print("cv mode: output overvoltage protection triggered. voltage reached:  ");
      Serial.println(gApp.output_voltage);
#endif
      snub(cvMode);
      // turn on error LED - indicating that output voltage is not good yet
      gLed.error = 1;
      gLed.needs_update = 1;
    }
  }
}

// state machine soft start action
static void soft_start(CvMode_t *cvMode)
{
  gApp.duty_cycle = 0;
  cvMode->state = CV_MODE_STATE_SOFT_START;
#ifdef EXTRA_DEBUG_MODE
  Serial.println("enabling soft start");
#endif
}

// state machine snub action
static void snub(CvMode_t *cvMode)
{
  // turn duty cycle to 0
  gApp.duty_cycle = 0;
  cvMode->state = CV_MODE_STATE_SNUB;
#ifdef EXTRA_DEBUG_MODE
  Serial.println("enabling snub");
#endif
}

// state machine turn on action
static void turn_on(CvMode_t *cvMode)
{
  cvMode->state = CV_MODE_STATE_ON;
#ifdef EXTRA_DEBUG_MODE
  Serial.println("turn on");
#endif
  // turn off error LED - indicating power good
  gLed.error = 0;
  gLed.needs_update = 1;
}

/// @brief Convert CvModeVoltage_t setting to mV
/// @param voltage CvModeVoltage_t setting
/// @return voltage in mV
uint32_t CV_MODE_VoltageSettingToMv(CvModeVoltage_t voltage)
{
  return voltageSettings[voltage];
}

// Initialize LED to show status
static void init_leds()
{
  gLed.cv = 1;
  switch (gSettings.cv_mode.voltage)
  {
  case CV_MODE_VOLTAGE_1_5V:
    gLed.x1 = 1;
    break;
  case CV_MODE_VOLTAGE_3V:
    gLed.x2 = 1;
    break;
  case CV_MODE_VOLTAGE_3_7V:
    gLed.x3 = 1;
    break;
  case CV_MODE_VOLTAGE_5V:
    gLed.x4 = 1;
    break;
  case CV_MODE_VOLTAGE_9V:
    gLed.x5 = 1;
    break;
  case CV_MODE_VOLTAGE_12V:
    gLed.x6 = 1;
    break;
  case CV_MODE_VOLTAGE_18V:
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

  switch (gSettings.cv_mode.voltage)
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
