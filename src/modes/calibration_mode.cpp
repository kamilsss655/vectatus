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

#include "calibration_mode.h"
#include "app.h"
#include "drivers/led.h"
#include "drivers/adc.h"
#include "settings.h"
#include "cc_mode.h"
#include "cv_mode.h"
#include "system.h"

// Local functions
static void toggle_leds();
static void next_state();
static void init_current_mode();
static uint16_t input_current_idle_calibration();
static uint16_t output_current_idle_calibration();

CalibrationMode_t calibrationModeLocal;

void CALIBRATION_MODE_Init()
{
  Serial.println("calibration mode started");
  calibrationModeLocal.state = CALIBRATION_MODE_IDLE_CURRENT;

  // get idle current calibrations
  gSettings.calibration.input_current_idle = input_current_idle_calibration();
  gSettings.calibration.output_current_idle = output_current_idle_calibration();
  // request to save settings in 1 second
  gSettingsSaveIn1000ms = 1;

  // go to next state of calibration process
  next_state();
}
void CALIBRATION_MODE_Tick()
{
  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_OUTPUT_CURRENT:
  case CALIBRATION_MODE_INPUT_CURRENT:
    CC_MODE_Tick();
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
    CV_MODE_Tick();
    break;
  default:
    break;
  }
}
void CALIBRATION_MODE_TimeSlice10ms()
{
}
void CALIBRATION_MODE_TimeSlice100ms()
{
  toggle_leds();
}
void CALIBRATION_MODE_TimeSlice500ms()
{
  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_OUTPUT_CURRENT:
  case CALIBRATION_MODE_INPUT_CURRENT:
    CC_MODE_TimeSlice500ms();
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
    CV_MODE_TimeSlice500ms();
    break;
  default:
    break;
  }
}
void CALIBRATION_MODE_TimeSlice1000ms()
{
  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_OUTPUT_CURRENT:
  case CALIBRATION_MODE_INPUT_CURRENT:
    CC_MODE_TimeSlice1000ms();
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
    CV_MODE_TimeSlice1000ms();
    break;
  default:
    break;
  }
}
void CALIBRATION_MODE_ModeBtnPressed()
{
  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_INPUT_VOLTAGE:
    gSettings.calibration.input_voltage += 1;
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
    gSettings.calibration.output_voltage += 1;
    break;
  case CALIBRATION_MODE_OUTPUT_CURRENT:
    gSettings.calibration.output_current += 1;
    break;
  case CALIBRATION_MODE_INPUT_CURRENT:
    gSettings.calibration.input_current += 1;
    break;
  default:
    break;
  }

  gSettingsSaveIn1000ms = 5;
}
void CALIBRATION_MODE_ModeBtnHeld()
{
  next_state();
}
void CALIBRATION_MODE_OutputBtnPressed()
{
  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_INPUT_VOLTAGE:
    gSettings.calibration.input_voltage -= 1;
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
    gSettings.calibration.output_voltage -= 1;
    break;
  case CALIBRATION_MODE_OUTPUT_CURRENT:
    gSettings.calibration.output_current -= 1;
    break;
  case CALIBRATION_MODE_INPUT_CURRENT:
    gSettings.calibration.input_current -= 1;
    break;
  default:
    break;
  }

  gSettingsSaveIn1000ms = 5;
}
void CALIBRATION_MODE_OutputBtnHeld()
{
}

// Toggle LED state based on current status
static void toggle_leds()
{
  gLed.charge = !gLed.charge;

  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_OUTPUT_CURRENT:
    gLed.cc = !gLed.cc;
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
    gLed.cv = !gLed.cv;
    break;
  default:
    break;
  }

  // request updates
  gLed.needs_update = 1;
}

/// @brief Get input current idle calibration value
/// @return calibration value
uint16_t input_current_idle_calibration()
{
  uint16_t adcValue;

  for (int i = 0; i < CALIBRATION_IDLE_SAMPLES_COUNT; i++)
  {
    adcValue += ADC_analogDiffRead(
        INPUT_CURRENT_ADC_N,
        INPUT_CURRENT_ADC_P,
        INPUT_CURRENT_GAIN,
        INPUT_CURRENT_OVERSAMPLE_BITS);

    if (i != 0)
      adcValue /= 2;

    // Tick the system to prevent WDT reset
    SYSTEM_Tick();
  }

  return adcValue;
}

/// @brief Get output current idle calibration value
/// @return calibration value
uint16_t output_current_idle_calibration()
{
  uint16_t adcValue;

  for (int i = 0; i < CALIBRATION_IDLE_SAMPLES_COUNT; i++)
  {
    adcValue += ADC_analogDiffRead(
        OUTPUT_CURRENT_ADC_N,
        OUTPUT_CURRENT_ADC_P,
        OUTPUT_CURRENT_GAIN,
        OUTPUT_CURRENT_OVERSAMPLE_BITS);

    if (i != 0)
      adcValue /= 2;

    // Tick the system to prevent WDT reset
    SYSTEM_Tick();
  }

  return adcValue;
}

static void init_current_mode()
{
  switch (calibrationModeLocal.state)
  {
  case CALIBRATION_MODE_INPUT_VOLTAGE:
#ifdef DEBUG_MODE
    Serial.println(F("Calibrating input voltage"));
#endif
    break;
  case CALIBRATION_MODE_OUTPUT_VOLTAGE:
#ifdef DEBUG_MODE
    Serial.println(F("Calibrating output voltage"));
#endif
    // turn on output
    gSettings.output = true;
    // init CC mode
    CV_MODE_Init();
    break;
  case CALIBRATION_MODE_OUTPUT_CURRENT:
#ifdef DEBUG_MODE
    Serial.println(F("Calibrating output current"));
#endif
    // turn on output
    gSettings.output = true;
    // init CC mode
    CC_MODE_Init();
    break;
  case CALIBRATION_MODE_INPUT_CURRENT:
#ifdef DEBUG_MODE
    Serial.println(F("Calibrating input current"));
#endif
    // turn on output
    gSettings.output = true;
    // init CC mode
    CC_MODE_Init();
    break;
  default:
    break;
  }
}

// Go to next state of calibration process
static void next_state()
{
  // go to next state if not finished
  if (calibrationModeLocal.state < CALIBRATION_MODE_FINISH)
  {
    calibrationModeLocal.state = (CalibrationModeState_t)(calibrationModeLocal.state + 1);
    init_current_mode();
  }
  else
  {
    // if finished go to next application mode exiting the calibration mode
    APP_NextMode();
  }
}
