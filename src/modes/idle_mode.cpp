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

void IDLE_MODE_Init()
{
}
void IDLE_MODE_Tick()
{
  // Keep duty cycle at 0
  gApp.duty_cycle = 0;
}
void IDLE_MODE_TimeSlice10ms()
{
}
void IDLE_MODE_TimeSlice100ms()
{
}
void IDLE_MODE_TimeSlice500ms()
{
}
void IDLE_MODE_TimeSlice1000ms()
{
  // toggle the led
  gLed.x8 = !gLed.x8;
  // request updates
  gLed.needs_update = 1;
}

void IDLE_MODE_ModeBtnPressed()
{
#ifdef DEBUG_MODE
  Serial.println("idle mode: mode btn pressed");
#endif
}
void IDLE_MODE_ModeBtnHeld()
{
  APP_NextMode();
#ifdef DEBUG_MODE
  Serial.println("idle mode: mode btn held");
#endif
}
void IDLE_MODE_OutputBtnPressed()
{
#ifdef DEBUG_MODE
  Serial.println("idle mode: output btn pressed");
#endif
}
void IDLE_MODE_OutputBtnHeld()
{
#ifdef DEBUG_MODE
  Serial.println("idle mode: output btn held");
#endif
}