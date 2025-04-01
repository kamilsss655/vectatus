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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

// Default amount of seconds of delay before EEPROM gets saved - for wear leveling, and fault recovery
#define SETTINGS_SAVE_DELAY_SECONDS 10
// EEPROM starting address
#define EEPROM_ADDRESS 0
// EEPROM alignment
#define EEPROM_ALIGNMENT 4

// Calibration settings
typedef struct
{
    int8_t input_current;         // input current calibration
    uint16_t input_current_idle;  // idle input current calibration
    int8_t output_current;        // output current calibration
    uint16_t output_current_idle; // idle output current calibration
    int8_t input_voltage;         // input voltage calibration
    int8_t output_voltage;        // output voltage calibration
} CalibrationSettings_t;

// Main application mode settings
enum AppMode_t : uint8_t
{
    APP_MODE_IDLE = 0,    // no voltage is being output
    APP_MODE_CV,          // constant voltage mode
    APP_MODE_CC,          // constant current mode
    APP_MODE_CHARGE,      // charge mode
    APP_MODE_MPPT,        // MPPT mode
    APP_MODE_ERROR,       // error mode
    APP_MODE_CALIBRATION, // calibration mode
    APP_MODE_MAX          // not used, needed for wraparound
};
typedef enum AppMode_t AppMode_t;

// CV mode output voltage settings
enum CvModeVoltage_t : uint8_t
{
    CV_MODE_VOLTAGE_1_5V = 0, // 1.5V
    CV_MODE_VOLTAGE_3V,       // 3V
    CV_MODE_VOLTAGE_3_7V,     // 3.7V
    CV_MODE_VOLTAGE_5V,       // 5V
    CV_MODE_VOLTAGE_9V,       // 9V
    CV_MODE_VOLTAGE_12V,      // 12V
    CV_MODE_VOLTAGE_18V,      // 18V
    CV_MODE_VOLTAGE_MAX       // not used
};
typedef enum CvModeVoltage_t CvModeVoltage_t;

// CC mode output current settings
enum CcModeCurrent_t : uint8_t
{
    CC_MODE_CURRENT_2MA = 0, // 2mA
    CC_MODE_CURRENT_100MA,   // 100mA
    CC_MODE_CURRENT_250MA,   // 250mA
    CC_MODE_CURRENT_500MA,   // 500mA
    CC_MODE_CURRENT_750MA,   // 750mA
    CC_MODE_CURRENT_1000MA,  // 1000mA
    CC_MODE_CURRENT_1500MA,  // 1500mA
    CC_MODE_CURRENT_MAX      // not used
};
typedef enum CcModeCurrent_t CcModeCurrent_t;

// CV MODE settings
typedef struct
{
    CvModeVoltage_t voltage; // CV mode voltage mode
} CvModeSettings_t;

// CC MODE settings
typedef struct
{
    CcModeCurrent_t current; // max current
    CvModeVoltage_t voltage; // max voltage
} CcModeSettings_t;

// CHARGE MODE settings
typedef struct
{
    CcModeCurrent_t current; // max current
    CvModeVoltage_t voltage; // max voltage
} ChargeModeSettings_t;

// SETTINGS values
typedef struct
{
    AppMode_t mode;                                         // stores information about operating mode
    bool output;                                            // global flag that determines whether output is on or off
    CalibrationSettings_t calibration;                      // stores device calibration
    CvModeSettings_t cv_mode;                               // stores CV mode settings
    CcModeSettings_t cc_mode;                               // stores CC mode settings
    ChargeModeSettings_t charge_mode;                       // stores CHARGE mode settings
} __attribute__((aligned(EEPROM_ALIGNMENT))) SettingsVal_t; // auto-align

// Global settings variable
extern SettingsVal_t gSettings;
// Global settings save flag
extern uint8_t gSettingsSaveIn1000ms;

void SETTINGS_Load();
void SETTINGS_Save();
void SETTINGS_TimeSlice1000ms();
#endif