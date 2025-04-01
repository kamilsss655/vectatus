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

#ifndef LED_H
#define LED_H

// LED pin
#define LED_CV_MODE_PIN E3
#define LED_CC_MODE_PIN E6
#define LED_CHARGE_MODE_PIN E1
#define LED_ERROR_MODE_PIN E4
#define LED_OUTPUT_X1_PIN D9
#define LED_OUTPUT_X2_PIN D8
#define LED_OUTPUT_X3_PIN D7
#define LED_OUTPUT_X4_PIN D5
#define LED_OUTPUT_X5_PIN D10
#define LED_OUTPUT_X6_PIN D11
#define LED_OUTPUT_X7_PIN D12
#define LED_OUTPUT_X8_PIN D4

typedef struct
{
    bool needs_update; // flag indicating request to change LED values
    bool cv;           // CV mode led state
    bool cc;           // CC mode led state
    bool charge;       // CHARGE mode led state
    bool error;        // ERROR mode led state
    bool x1;           // X1 output led state (1.5V/0.02A)
    bool x2;           // X2 output led state (3V/0.1A)
    bool x3;           // X3 output led state (3.7V/0.25A)
    bool x4;           // X4 output led state (5V/0.5A)
    bool x5;           // X5 output led state (9V/0.75A)
    bool x6;           // X6 output led state (12V/1A)
    bool x7;           // X7 output led state (18V/1.5A)
    bool x8;           // X8 output led state (X1/Y1)
} Led_t;

// Global LED struct
extern Led_t gLed;

void LED_Setup();
void LED_TimeSlice10ms();
void LED_TimeSlice100ms();
void LED_TimeSlice500ms();
void LED_TimeSlice1000ms();
void LED_Clear();

#endif