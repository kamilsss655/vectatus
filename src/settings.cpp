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

#include <EEPROM.h>

#include "settings.h"
#include "system.h"

// Global eeprom variable
SettingsVal_t gSettings;
// Countdown timer
uint8_t gSettingsSaveIn1000ms = 0;

// Local functions
static void save_settings_scheduler_1000ms();

/// @brief Load settings
void SETTINGS_Load()
{
  lgt_eeprom_readSWM(EEPROM_ADDRESS, (uint32_t *)&gSettings, sizeof(gSettings) / EEPROM_ALIGNMENT);
  // Set default values if data is malformed
  gSettings.mode = (gSettings.mode < APP_MODE_MAX) ? gSettings.mode : APP_MODE_IDLE;
  gSettings.cv_mode.voltage = (gSettings.cv_mode.voltage < CV_MODE_VOLTAGE_MAX) ? gSettings.cv_mode.voltage : CV_MODE_VOLTAGE_1_5V;
  gSettings.cc_mode.current = (gSettings.cc_mode.current < CC_MODE_CURRENT_MAX) ? gSettings.cc_mode.current : CC_MODE_CURRENT_2MA;
  gSettings.cc_mode.voltage = (gSettings.cc_mode.voltage < CV_MODE_VOLTAGE_MAX) ? gSettings.cc_mode.voltage : CV_MODE_VOLTAGE_1_5V;
#ifdef DEBUG_MODE
  Serial.println("SETTINGS loaded");
#endif
}

void SETTINGS_Save()
{
  // Split saving into smaller chunks and perform SYSTEM_Tick() in between
  // to prevent WDT reset
  uint8_t *ptr = (uint8_t *)&gSettings;

  for (uint8_t i = 0; i < (sizeof(gSettings) / EEPROM_ALIGNMENT); i++)
  {
    lgt_eeprom_writeSWM(EEPROM_ADDRESS + (i * EEPROM_ALIGNMENT), (uint32_t *)(ptr + (i * EEPROM_ALIGNMENT)), 1);
    SYSTEM_Tick();
  }
#ifdef DEBUG_MODE
  Serial.println("SETTINGS saved");
#endif
}

void SETTINGS_TimeSlice1000ms()
{
  save_settings_scheduler_1000ms();
}

// handler function for the save settings scheduler
static void save_settings_scheduler_1000ms()
{
  // check if there is save state scheduled
  if (gSettingsSaveIn1000ms > 0)
  {
    // if so decrement the scheduler counter
    gSettingsSaveIn1000ms--;
    // on last decrement save the state
    if (gSettingsSaveIn1000ms == 0)
    {
      SETTINGS_Save();
    }
  }
}