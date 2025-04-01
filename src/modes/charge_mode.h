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

#ifndef CHARGE_MODE_H
#define CHARGE_MODE_H

#include "settings.h"
#include "cv_mode.h"
#include "cc_mode.h"

// charge mode state machine
enum ChargeModeState_t : uint8_t
{
    CHARGE_MODE_IDLE = 0,
    CHARGE_MODE_CHARGING,
    CHARGE_MODE_STANDBY,
    CHARGE_MODE_FINISHED
};
typedef enum ChargeModeState_t ChargeModeState_t;

// Internal variables
typedef struct
{
    CcMode_t cc_mode;
    unsigned long current_time_10ms;    // stores milis10ms() for current time
    unsigned long last_regulation_10ms; // stores milis10ms() at the time last regulation took place
} ChargeModeInternalVar_t;

// Main charge mode struct
typedef struct
{
    ChargeModeState_t state;
    uint16_t regulation_period_10ms; // defines how often (in 10ms) should the regulation take place (0 for fastest regulation)
    ChargeModeInternalVar_t internal_var;
} ChargeMode_t;

void CHARGE_MODE_Init();
void CHARGE_MODE_Tick();
void CHARGE_MODE_TimeSlice10ms();
void CHARGE_MODE_TimeSlice100ms();
void CHARGE_MODE_TimeSlice500ms();
void CHARGE_MODE_TimeSlice1000ms();
void CHARGE_MODE_ModeBtnPressed();
void CHARGE_MODE_ModeBtnHeld();
void CHARGE_MODE_OutputBtnPressed();
void CHARGE_MODE_OutputBtnHeld();
uint16_t CHARGE_MODE_MinimumVoltageToMv(CvModeVoltage_t voltage);
uint16_t CHARGE_MODE_MaximumVoltageToMv(CvModeVoltage_t voltage);
#endif