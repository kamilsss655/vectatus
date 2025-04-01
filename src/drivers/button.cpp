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

#include "button.h"
#include "app.h"
#include "settings.h"
#include "modes/idle_mode.h"
#include "modes/cv_mode.h"
#include "modes/cc_mode.h"
#include "modes/mppt_mode.h"
#include "modes/error_mode.h"
#include "modes/charge_mode.h"
#include "modes/calibration_mode.h"

// Button interrupt flags
volatile bool modeButtonPressed, outputButtonPressed = 0;

// Counter for keeping track how long the button was pressed
uint8_t modeButtonPressed100ms, outputButtonPressed100ms = 0;

// Scheduler counter for attaching interrupt
uint8_t modeButtonScheduleIntAttach100ms, outputButtonScheduleIntAttach100ms = 0;

bool modeButtonWasLongPressed, outputButtonWasLongPressed = false;

// Local functions
static void mode_btn_falling_intr();
static void output_btn_falling_intr();
static void mode_btn_interrupt_scheduler_100ms();
static void output_btn_interrupt_scheduler_100ms();
static void mode_btn_handler_100ms();
static void output_btn_handler_100ms();

/// @brief Setup buttons
void BUTTON_Setup()
{
  // Enable internal pull-up
  pinMode(BUTTON_PIN_MODE, INPUT_PULLUP);
  pinMode(BUTTON_PIN_OUTPUT, INPUT_PULLUP);
  // Attach interrupt
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_MODE), mode_btn_falling_intr, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_OUTPUT), output_btn_falling_intr, FALLING);
}

void BUTTON_TimeSlice10ms()
{
}
void BUTTON_TimeSlice100ms()
{
  mode_btn_interrupt_scheduler_100ms();
  mode_btn_handler_100ms();

  output_btn_interrupt_scheduler_100ms();
  output_btn_handler_100ms();
}
void BUTTON_TimeSlice500ms()
{
}
void BUTTON_TimeSlice1000ms()
{
}

// Handle application level logic when mode button was pressed
void BUTTON_ModePressed()
{
  // send event to appropriate handler
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_ModeBtnPressed();
    break;
  case APP_MODE_CV:
    CV_MODE_ModeBtnPressed();
    break;
  case APP_MODE_CC:
    CC_MODE_ModeBtnPressed();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_ModeBtnPressed();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_ModeBtnPressed();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_ModeBtnPressed();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_ModeBtnPressed();
    break;
  default:
    break;
  }
}

// Handle application level logic when mode button was held
void BUTTON_ModeHeld()
{
  // send event to appropriate handler
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_ModeBtnHeld();
    break;
  case APP_MODE_CV:
    CV_MODE_ModeBtnHeld();
    break;
  case APP_MODE_CC:
    CC_MODE_ModeBtnHeld();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_ModeBtnHeld();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_ModeBtnHeld();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_ModeBtnHeld();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_ModeBtnHeld();
    break;
  default:
    break;
  }
}

// Handle application level logic when output button was pressed
void BUTTON_OutputPressed()
{
  // send event to appropriate handler
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_OutputBtnPressed();
    break;
  case APP_MODE_CV:
    CV_MODE_OutputBtnPressed();
    break;
  case APP_MODE_CC:
    CC_MODE_OutputBtnPressed();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_OutputBtnPressed();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_OutputBtnPressed();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_OutputBtnPressed();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_OutputBtnPressed();
    break;
  default:
    break;
  }
}

// Handle application level logic when output button was held
void BUTTON_OutputHeld()
{
  // send event to appropriate handler
  switch (gSettings.mode)
  {
  case APP_MODE_IDLE:
    IDLE_MODE_OutputBtnHeld();
    break;
  case APP_MODE_CV:
    CV_MODE_OutputBtnHeld();
    break;
  case APP_MODE_CC:
    CC_MODE_OutputBtnHeld();
    break;
  case APP_MODE_CHARGE:
    CHARGE_MODE_OutputBtnHeld();
    break;
  case APP_MODE_MPPT:
    MPPT_MODE_OutputBtnHeld();
    break;
  case APP_MODE_ERROR:
    ERROR_MODE_OutputBtnHeld();
    break;
  case APP_MODE_CALIBRATION:
    CALIBRATION_MODE_OutputBtnHeld();
    break;
  default:
    break;
  }
}

// Local interrupt function called on falling edge detection across mode button
static void mode_btn_falling_intr()
{
  modeButtonPressed = 1;
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN_MODE));
}
// Local interrupt function called on falling edge detection across output button
static void output_btn_falling_intr()
{
  outputButtonPressed = 1;
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN_OUTPUT));
}

// Local mode button interrupt scheduler
static void mode_btn_interrupt_scheduler_100ms()
{
  // check if there is interrupt re-attachment scheduled
  if (modeButtonScheduleIntAttach100ms > 0)
  {
    // if so decrement the scheduler counter
    modeButtonScheduleIntAttach100ms--;
    // on last decrement attach the interrupt
    if (modeButtonScheduleIntAttach100ms == 0)
    {
      attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_MODE), mode_btn_falling_intr, FALLING);
    }
  }
}

// Local output button interrupt scheduler
static void output_btn_interrupt_scheduler_100ms()
{
  // check if there is interrupt re-attachment scheduled
  if (outputButtonScheduleIntAttach100ms > 0)
  {
    // if so decrement the scheduler counter
    outputButtonScheduleIntAttach100ms--;
    // on last decrement attach the interrupt
    if (outputButtonScheduleIntAttach100ms == 0)
    {
      attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_OUTPUT), output_btn_falling_intr, FALLING);
    }
  }
}

// Local mode button handler
static void mode_btn_handler_100ms()
{
  // mode button falling edge detected
  if (modeButtonPressed)
  {
    // if button is pressed
    if (!digitalRead(BUTTON_PIN_MODE))
    {
      // button is still being held so count how many 100ms timeslices is being held for
      modeButtonPressed100ms += 1;

      // decide based on modeButtonPressed100ms counter how long the button was held for
      if (modeButtonPressed100ms > (BUTTON_TIMEOUT_SEC * _100MS_TO_SEC))
      {
        // long press detected
        BUTTON_ModeHeld();
        modeButtonPressed100ms = 0;
        modeButtonWasLongPressed = true;
      }
    }
    else
    // button was released so process action
    {
      // reset
      modeButtonPressed = 0;
      modeButtonPressed100ms = 0;
      // if it was long pressed - reset the flag
      if (modeButtonWasLongPressed)
      {
        modeButtonWasLongPressed = false;
      }
      // otherwise it was not held long enough so process it as a short press
      else
      {
        BUTTON_ModePressed();
      }
      // schedule interrupt re-attachment in 2x100ms = 200ms
      modeButtonScheduleIntAttach100ms = 2;
    }
  }
}

// Local output button handler
static void output_btn_handler_100ms()
{
  // output button falling edge detected
  if (outputButtonPressed)
  {
    // if button is pressed
    if (!digitalRead(BUTTON_PIN_OUTPUT))
    {
      // button is still being held so count how many 100ms timeslices is being held for
      outputButtonPressed100ms += 1;

      // decide based on outputButtonPressed100ms counter how long the button was held for
      if (outputButtonPressed100ms > (BUTTON_TIMEOUT_SEC * _100MS_TO_SEC))
      {
        // long press detected
        BUTTON_OutputHeld();
        outputButtonPressed100ms = 0;
        outputButtonWasLongPressed = true;
      }
    }
    else
    // button was released so process action
    {
      // reset
      outputButtonPressed = 0;
      outputButtonPressed100ms = 0;
      // if it was long pressed - reset the flag
      if (outputButtonWasLongPressed)
      {
        outputButtonWasLongPressed = false;
      }
      // otherwise it was not held long enough so process it as a short press
      else
      {
        BUTTON_OutputPressed();
      }
      // schedule interrupt re-attachment in 2x100ms = 200ms
      outputButtonScheduleIntAttach100ms = 2;
    }
  }
}