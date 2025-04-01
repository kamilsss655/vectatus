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

#ifndef CV_MODE_H
#define CV_MODE_H

#include <stdint.h>

#include "settings.h"

// CV mode state machine
enum CV_MODE_STATE_t : uint8_t
{
    CV_MODE_STATE_SOFT_START = 0, // soft start mode - slowly ramp up the voltage
    CV_MODE_STATE_ON,             // on mode - operating at full speed
    CV_MODE_STATE_SNUB            // snubs voltage spikes - duty cycle stays at 0
};
typedef enum CV_MODE_STATE_t CV_MODE_STATE_t;

// Internal variables
typedef struct
{
    uint32_t previous_voltage;              // stores previous reading of the output voltage in mV
    unsigned long current_time_10ms;        // stores milis10ms() for current time
    unsigned long last_soft_regulated_10ms; // stores milis10ms() at the time last soft regulation took place
} CvModeInternalVar_t;

// Main CV mode struct
typedef struct
{
    CV_MODE_STATE_t state;               // cv mode current state
    uint32_t voltage;                    // target output voltage in mV
    uint32_t max_voltage_ripple;         // max output voltage ripple in mV, if breached soft start is enabled and duty dropped to 0
    uint32_t soft_start_step_up_voltage; // define step up in mV during soft start, higher the value the more agressive will be the soft start ramp up
    uint8_t soft_start_period_10ms;      // defines delay in 10ms between soft start regulations, higher delay = slower soft start
    uint8_t snub_power;                  // defines snubbing power which is a target voltage drop percentage (0-100%)
    CvModeInternalVar_t internal_var;    // internal variables
} CvMode_t;

void CV_MODE_Init();
void CV_MODE_Tick();
void CV_MODE_Regulate(CvMode_t *cvMode);
void CV_MODE_TimeSlice10ms();
void CV_MODE_TimeSlice100ms();
void CV_MODE_TimeSlice500ms();
void CV_MODE_TimeSlice1000ms();
void CV_MODE_ModeBtnPressed();
void CV_MODE_ModeBtnHeld();
void CV_MODE_OutputBtnPressed();
void CV_MODE_OutputBtnHeld();
uint32_t CV_MODE_VoltageSettingToMv(CvModeVoltage_t voltage);
#endif