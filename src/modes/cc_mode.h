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

#ifndef CC_MODE_H
#define CC_MODE_H

#include "cv_mode.h"
#include "settings.h"

// CC mode state machine
enum CcModeState_t : uint8_t
{
    CC_MODE_STATE_SOFT_START = 0, // soft start mode - slowly ramp up the current
    CC_MODE_STATE_ON,             // on mode - operating at full speed
    CC_MODE_STATE_SNUB            // snubs current spikes - duty cycle stays at 0
};
typedef enum CcModeState_t CcModeState_t;

// Internal variables
typedef struct
{
    uint32_t previous_current;              // stores previous reading of the output current in mA
    unsigned long current_time_10ms;        // stores milis10ms() for current time
    unsigned long last_soft_regulated_10ms; // stores milis10ms() at the time last soft regulation took place
    CvMode_t cv_mode;                       // CV mode struct
} CcModeInternalVar_t;

// Main CC mode struct
typedef struct
{
    CcModeState_t state;                 // cc mode current state
    uint32_t current;                    // target output current in mA
    uint32_t max_current_ripple;         // max output current ripple in mA, if breached soft start is enabled and duty dropped to 0
    uint32_t soft_start_step_up_current; // define step up in mA during soft start, higher the value the more agressive will be the soft start ramp up
    uint8_t soft_start_period_10ms;      // defines delay in 10ms between soft start regulations, higher delay = slower soft start
    uint8_t snub_power;                  // defines snubbing power which is a target current drop percentage (0-100%)
    uint16_t cv_mode_switch_hysteresis;  // defines hysteresis (in mV) for switching to CV mode if no load connected
    CcModeInternalVar_t internal_var;    // internal variables
} CcMode_t;

void CC_MODE_Init();
void CC_MODE_Tick();
void CC_MODE_TimeSlice10ms();
void CC_MODE_TimeSlice100ms();
void CC_MODE_TimeSlice500ms();
void CC_MODE_TimeSlice1000ms();
void CC_MODE_ModeBtnPressed();
void CC_MODE_ModeBtnHeld();
void CC_MODE_OutputBtnPressed();
void CC_MODE_OutputBtnHeld();
void CC_MODE_Regulate(CcMode_t *ccMode);
uint32_t CC_MODE_CurrentSettingToMa(CcModeCurrent_t current);
#endif