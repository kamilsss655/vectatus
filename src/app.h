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

#ifndef APP_H
#define APP_H

#include "lib/util.h"

// Max input current in mA
#define MAX_INPUT_CURRENT TO_MILI(1.5)
// Max output current in mA
#define MAX_OUTPUT_CURRENT TO_MILI(2.0)
// Max output voltage in mV
// Note: CURRENT VOLTAGE DIVIDERS ALLOW ONLY MAX VOLTAGE TO BE: 17.6V
// this prevents protections triggering at 20V, since max voltage we read is 17V even if actual voltage is ie 40V
// I could increase voltage reference to 2.048V from 1.024V, but this increases noise and decreases stability
// so the voltage divider needs to be adjusted in future version, while for now max vout would be like 16V or so to be safe
#define MAX_OUTPUT_VOLTAGE TO_MILI(16.9)
// Minimum output voltage in mV when duty cycle is maximum (used to detect physical circuit faults)
#define MIN_OUTPUT_VOLTAGE TO_MILI(1.0)
// Define SEPIC output diode max reverse voltage in mV, 40V for SS54
#define DIODE_MAX_REVERSE_VOLTAGE TO_MILI(40.0)
// The sum of Vin+Vout must be < diode max reverse voltage rating, otherwise it will fail short.
// Take 68% of the manufacturer rating for safety.
#define MAX_VIN_PLUS_VOUT ((DIODE_MAX_REVERSE_VOLTAGE * 68) / 100)

// Duty cycle resolution
#define MAX_PWM_RESOLUTION 255
// Maximum duty cycle
#define MAX_DUTY_CYCLE 85
// Minimum duty cycle
#define MIN_DUTY_CYCLE 0

typedef struct
{
    uint8_t duty_cycle;      // current operating duty cycle of the converter
    uint32_t input_voltage;  // input voltage in mV
    uint32_t output_voltage; // output voltage in mV
    uint32_t input_current;  // input current in mA
    uint32_t output_current; // output current in mA
} Application_t;

// Global app variable
extern Application_t gApp;

void APP_Setup();
void APP_Tick();
void APP_TimeSlice10ms();
void APP_TimeSlice100ms();
void APP_TimeSlice500ms();
void APP_TimeSlice1000ms();
void APP_InitCurrentApp();
void APP_NextMode();
void APP_OutputToggle();
void APP_OutputOff();
void APP_OutputOn();
#endif