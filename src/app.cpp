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

#include <Arduino.h>

#include "app.h"
#include "drivers/adc.h"
#include "settings.h"
#include "drivers/pwm.h"
#include "modes/calibration_mode.h"
#include "modes/idle_mode.h"
#include "modes/cv_mode.h"
#include "modes/cc_mode.h"
#include "modes/mppt_mode.h"
#include "modes/error_mode.h"
#include "modes/charge_mode.h"
#include "drivers/button.h"
#include "drivers/led.h"

// Global app variable
Application_t gApp;

// Local functions
static void take_measurements();
static void protect();
static bool enter_calibration_mode();
#ifdef DEBUG_MODE
static void print_debug_info();
#endif

/// @brief Setup app
void APP_Setup()
{
  // Load settings
  SETTINGS_Load();
  gApp.duty_cycle = 0;
  gApp.input_voltage = 0;
  gApp.output_voltage = 0;
  gApp.input_current = 0;
  gApp.output_current = 0;

  if (enter_calibration_mode())
  {
    gSettings.mode = APP_MODE_CALIBRATION;
  }

  APP_InitCurrentApp();
}

/// @brief Core application tick - happens as often as possible
void APP_Tick()
{
  take_measurements();
  protect();

  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_Tick();
    break;
  case APP_MODE_CV:
    CV_MODE_Tick();
    break;
  case APP_MODE_CC:
    CC_MODE_Tick();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_Tick();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_Tick();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_Tick();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_Tick();
    break;
  default:
    ERROR_MODE_Tick();
    break;
  }

  // update hardware PWM output based on app values
  PWM_Tick();
}

void APP_TimeSlice10ms()
{
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_TimeSlice10ms();
    break;
  case APP_MODE_CV:
    CV_MODE_TimeSlice10ms();
    break;
  case APP_MODE_CC:
    CC_MODE_TimeSlice10ms();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_TimeSlice10ms();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_TimeSlice10ms();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_TimeSlice10ms();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_TimeSlice10ms();
    break;
  default:
    ERROR_MODE_TimeSlice10ms();
    break;
  }
}
void APP_TimeSlice100ms()
{
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_TimeSlice100ms();
    break;
  case APP_MODE_CV:
    CV_MODE_TimeSlice100ms();
    break;
  case APP_MODE_CC:
    CC_MODE_TimeSlice100ms();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_TimeSlice100ms();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_TimeSlice100ms();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_TimeSlice100ms();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_TimeSlice100ms();
    break;
  default:
    ERROR_MODE_TimeSlice100ms();
    break;
  }
}
void APP_TimeSlice500ms()
{
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_TimeSlice500ms();
    break;
  case APP_MODE_CV:
    CV_MODE_TimeSlice500ms();
    break;
  case APP_MODE_CC:
    CC_MODE_TimeSlice500ms();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_TimeSlice500ms();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_TimeSlice500ms();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_TimeSlice500ms();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_TimeSlice500ms();
    break;
  default:
    ERROR_MODE_TimeSlice500ms();
    break;
  }
}
void APP_TimeSlice1000ms()
{
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_TimeSlice1000ms();
    break;
  case APP_MODE_CV:
    CV_MODE_TimeSlice1000ms();
    break;
  case APP_MODE_CC:
    CC_MODE_TimeSlice1000ms();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_TimeSlice1000ms();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_TimeSlice1000ms();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_TimeSlice1000ms();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_TimeSlice1000ms();
    break;
  default:
    ERROR_MODE_TimeSlice1000ms();
    break;
  }

#ifdef DEBUG_MODE
  print_debug_info();
#endif
}

// Initialize current app
void APP_InitCurrentApp()
{
  // Clear any left-over LED state from previous app mode
  LED_Clear();

  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_Init();
    break;
  case APP_MODE_CV:
    CV_MODE_Init();
    break;
  case APP_MODE_CC:
    CC_MODE_Init();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_Init();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_Init();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_Init();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_Init();
    break;
  default:
    ERROR_MODE_Init();
    break;
  }
}

// Switch to the next app mode
void APP_NextMode()
{
  // prevent toggling to beyond charge mode by the user
  if (gSettings.mode < APP_MODE_MPPT)
  {
    gSettings.mode = (AppMode_t)(gSettings.mode + 1);
  }
  else
  {
    gSettings.mode = APP_MODE_IDLE;
  }
  // Turn off output just in case different mode could destroy output connected
  APP_OutputOff();
  // Initialize current app
  APP_InitCurrentApp();
  // Schedule settings save to EEPROM
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;
}

/// @brief  Toggle output
void APP_OutputToggle()
{
  if (gSettings.output)
  {
    APP_OutputOff();
  }
  else
  {
    APP_OutputOn();
  }
  APP_InitCurrentApp();
}
/// @brief Turn off output
void APP_OutputOff()
{
  gSettings.output = 0;
  gApp.duty_cycle = 0;
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;
  // TODO: Add a countdown so if there is no activity after like 30seconds:
  // turn off LEDs,
  // attach wakup interrupt to MODE btn
  // go to sleep
}

/// @brief Turn on output
void APP_OutputOn()
{
  gSettings.output = 1;
  gSettingsSaveIn1000ms = SETTINGS_SAVE_DELAY_SECONDS;
}

#ifdef DEBUG_MODE
static void print_debug_info()
{
  Serial.print("App mode: ");
  Serial.println(gSettings.mode);

  Serial.print("Duty cycle: ");
  Serial.println(gApp.duty_cycle);

  Serial.print("Input voltage [mV]: ");
  Serial.println(gApp.input_voltage);

  Serial.print("Input current [mA]: ");
  Serial.println(gApp.input_current);

  Serial.print("Output voltage [mV]: ");
  Serial.println(gApp.output_voltage);

  Serial.print("Output current [mA]: ");
  Serial.println(gApp.output_current);
}
#endif

// Apply general protection rules for all modes
static void protect()
{
  // guard clause
  if (gSettings.mode == APP_MODE_ERROR)
  {
    return;
  }

  // general input over-current, output over-current, output over-voltage protections
  if (gApp.input_current > MAX_INPUT_CURRENT || gApp.output_current > MAX_OUTPUT_CURRENT || gApp.output_voltage > MAX_OUTPUT_VOLTAGE)
  {
#ifdef DEBUG_MODE
    Serial.println(F("ERROR! Protection circuit triggered for below values:"));
    print_debug_info();
#endif
    ERROR_MODE_Init();
  }

  // detect broken mosfet or other physical fault when duty cycle is max, but there is no voltage output
  if (gApp.duty_cycle == MAX_DUTY_CYCLE && gApp.output_voltage < MIN_OUTPUT_VOLTAGE)
  {
#ifdef DEBUG_MODE
    Serial.println(F("ERROR! Max duty cycle reached, but output voltage is below minimum."));
    print_debug_info();
#endif
    ERROR_MODE_Init();
  }

  // detect reverse voltage excess across SEPIC diode to prevent the diode from failing short
  // TODO: In the future also prevent user from selecting such combination of voltages
  // note: user might get stuck in this error mode
  if (gApp.input_voltage + gApp.output_voltage >= MAX_VIN_PLUS_VOUT)
  {
#ifdef DEBUG_MODE
    Serial.println(F("ERROR! Vin+Vout is greater than diode max reverse voltage."));
    print_debug_info();
#endif
    ERROR_MODE_Init();
  }
}

// Take measurements of voltage and current
static void take_measurements()
{
  gApp.input_current = ADC_InputCurrentVal();
  gApp.output_current = ADC_OutputCurrentVal();
  gApp.input_voltage = ADC_InputVoltageVal();
  gApp.output_voltage = ADC_OutputVoltageVal();
}

// Determine if calibration mode should be entered
static bool enter_calibration_mode()
{
  // if both mode and output buttons are held
  return (!digitalRead(BUTTON_PIN_MODE) && !digitalRead(BUTTON_PIN_OUTPUT));
}