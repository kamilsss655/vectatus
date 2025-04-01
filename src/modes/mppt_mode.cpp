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

#include "app.h"
#include "drivers/led.h"
#include <settings.h>

static uint16_t mppt_voltage = TO_MILI(4.0);

void MPPT_MODE_Init()
{
}

void MPPT_MODE_Tick()
{
  // if output is turned off exit early
  if (!gSettings.output)
  {
    return;
  }
  // if voltage is below target voltage and duty cycle can be increased
  if ((gApp.input_voltage > mppt_voltage) && (gApp.duty_cycle < MAX_DUTY_CYCLE))
  {
    gApp.duty_cycle += 1;
  }
  // if output voltage is too high, and duty cycle can be lowered
  else if ((gApp.input_voltage < mppt_voltage) && (gApp.duty_cycle > MIN_DUTY_CYCLE))
  {
    gApp.duty_cycle -= 1;
  }
}
void MPPT_MODE_TimeSlice10ms()
{
}
void MPPT_MODE_TimeSlice100ms()
{
}
void MPPT_MODE_TimeSlice500ms()
{
}
void MPPT_MODE_TimeSlice1000ms()
{
  // toggle the led
  gLed.charge = !gLed.charge;
  // request updates
  gLed.needs_update = 1;
}

void MPPT_MODE_ModeBtnPressed()
{
  APP_OutputToggle();
#ifdef DEBUG_MODE
  Serial.println("mppt mode: mode btn pressed");
#endif
}
void MPPT_MODE_ModeBtnHeld()
{
  APP_NextMode();
#ifdef DEBUG_MODE
  Serial.println("mppt mode: mode btn held");
#endif
}
void MPPT_MODE_OutputBtnPressed()
{
  mppt_voltage += 100;
#ifdef DEBUG_MODE
  Serial.println("mppt mode: output btn pressed");
#endif
}
void MPPT_MODE_OutputBtnHeld()
{
  mppt_voltage -= 100;
#ifdef DEBUG_MODE
  Serial.println("mppt mode: output btn held");
#endif
}