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

#include "led.h"
#include "app.h"

// Global LED struct
Led_t gLed;

/// @brief Led setup
void LED_Setup()
{
  // init led type
  LED_Clear();
  // setup pins
  pinMode(LED_CV_MODE_PIN, OUTPUT);
  pinMode(LED_CC_MODE_PIN, OUTPUT);
  pinMode(LED_CHARGE_MODE_PIN, OUTPUT);
  pinMode(LED_ERROR_MODE_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X1_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X2_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X3_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X4_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X5_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X6_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X7_PIN, OUTPUT);
  pinMode(LED_OUTPUT_X8_PIN, OUTPUT);
}
void LED_TimeSlice10ms()
{
  // if LED pins need an update
  if (gLed.needs_update)
  {
    // update all the pins
    digitalWrite(LED_CV_MODE_PIN, gLed.cv);
    digitalWrite(LED_CC_MODE_PIN, gLed.cc);
    digitalWrite(LED_CHARGE_MODE_PIN, gLed.charge);
    digitalWrite(LED_ERROR_MODE_PIN, gLed.error);
    digitalWrite(LED_OUTPUT_X1_PIN, gLed.x1);
    digitalWrite(LED_OUTPUT_X2_PIN, gLed.x2);
    digitalWrite(LED_OUTPUT_X3_PIN, gLed.x3);
    digitalWrite(LED_OUTPUT_X4_PIN, gLed.x4);
    digitalWrite(LED_OUTPUT_X5_PIN, gLed.x5);
    digitalWrite(LED_OUTPUT_X6_PIN, gLed.x6);
    digitalWrite(LED_OUTPUT_X7_PIN, gLed.x7);
    digitalWrite(LED_OUTPUT_X8_PIN, gLed.x8);
    // reset the flag
    gLed.needs_update = 0;
  }
}
void LED_TimeSlice100ms()
{
}
void LED_TimeSlice500ms()
{
}
void LED_TimeSlice1000ms()
{
}

/// @brief Clear LED struct
void LED_Clear()
{
  gLed.needs_update = 0;
  gLed.cv = 0;
  gLed.cc = 0;
  gLed.charge = 0;
  gLed.error = 0;
  gLed.x1 = 0;
  gLed.x2 = 0;
  gLed.x3 = 0;
  gLed.x4 = 0;
  gLed.x5 = 0;
  gLed.x6 = 0;
  gLed.x7 = 0;
  gLed.x8 = 0;
  // tell to refresh
  gLed.needs_update = 1;
}

// /// @brief Set IN PROTECT LED state hack that uses internal pull-up to drive the LED
// /// DO NOT USE - it causes unstable MCU behavior
// /// @param state desired state
// static void inProtectLedWrite(bool state)
// {
//   if (state)
//   {
//     bitSet(PORTB, PORTB7);
//   }
//   else
//   {
//     bitClear(PORTB, PORTB7);
//   }
// }