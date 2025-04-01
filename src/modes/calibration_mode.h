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

#ifndef CALIBRATION_MODE_H
#define CALIBRATION_MODE_H

#include <stdint.h>

// Amount of samples to take for idle calibrations
#define CALIBRATION_IDLE_SAMPLES_COUNT 500

// CC mode state machine
enum CalibrationModeState_t : uint8_t
{
    CALIBRATION_MODE_IDLE_CURRENT = 0, // measure idle input and output current calibration values (no user intervention required)
    CALIBRATION_MODE_INPUT_VOLTAGE,
    CALIBRATION_MODE_OUTPUT_VOLTAGE,
    CALIBRATION_MODE_INPUT_CURRENT,
    CALIBRATION_MODE_OUTPUT_CURRENT,
    CALIBRATION_MODE_FINISH // once reached the calibration is finished
};

// Main calibration mode struct
typedef struct
{
    CalibrationModeState_t state; // calibration mode current state
} CalibrationMode_t;

void CALIBRATION_MODE_Init();
void CALIBRATION_MODE_Tick();
void CALIBRATION_MODE_TimeSlice10ms();
void CALIBRATION_MODE_TimeSlice100ms();
void CALIBRATION_MODE_TimeSlice500ms();
void CALIBRATION_MODE_TimeSlice1000ms();
void CALIBRATION_MODE_ModeBtnPressed();
void CALIBRATION_MODE_ModeBtnHeld();
void CALIBRATION_MODE_OutputBtnPressed();
void CALIBRATION_MODE_OutputBtnHeld();
#endif