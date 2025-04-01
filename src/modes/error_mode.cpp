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
#include "settings.h"
#include "system.h"

void ERROR_MODE_Init()
{
  // Keep duty cycle at 0
  gApp.duty_cycle = 0;
  // Set global app mode to error mode
  gSettings.mode = APP_MODE_ERROR;
  // Abort any scheduled settings save, so we don't save the setting that caused the error mode
  gSettingsSaveIn1000ms = 0;
  // Clear LED
  LED_Clear();
}
void ERROR_MODE_Tick()
{
  // Keep duty cycle at 0
  gApp.duty_cycle = 0;
}
void ERROR_MODE_TimeSlice10ms()
{
}
void ERROR_MODE_TimeSlice100ms()
{
}
void ERROR_MODE_TimeSlice500ms()
{
  // toggle the led
  gLed.error = !gLed.error;
  // request updates
  gLed.needs_update = 1;
}
void ERROR_MODE_TimeSlice1000ms()
{
}

void ERROR_MODE_ModeBtnPressed()
{
#ifdef DEBUG_MODE
  Serial.println("error mode: mode btn pressed");
#endif
  SYSTEM_Reboot();
}
void ERROR_MODE_ModeBtnHeld()
{
#ifdef DEBUG_MODE
  APP_NextMode();
  Serial.println("error mode: mode btn held");
#endif
}
void ERROR_MODE_OutputBtnPressed()
{
#ifdef DEBUG_MODE
  Serial.println("error mode: output btn pressed");
#endif
  SYSTEM_Reboot();
}
void ERROR_MODE_OutputBtnHeld()
{
#ifdef DEBUG_MODE
  Serial.println("error mode: output btn held");
#endif
}