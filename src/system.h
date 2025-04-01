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

#ifndef SYSTEM_H
#define SYSTEM_H

// Serial connection baud rate
#define SERIAL_BAUD_RATE 115200
// System watchdog timeout
#define SYSTEM_WATCHDOG_TIMEOUT WTOH_32MS
// System watchdog timeout in debug mode (longer to allow serial output debug messages)
#define SYSTEM_WATCHDOG_TIMEOUT_DEBUG_MODE WTOH_32MS
// System welcome message
#define SYSTEM_WELCOME_MESSAGE PROJECT_NAME " " VERSION " has booted" 

void SYSTEM_Setup();
void SYSTEM_Tick();
void SYSTEM_TimeSlice10ms();
void SYSTEM_TimeSlice100ms();
void SYSTEM_TimeSlice500ms();
void SYSTEM_TimeSlice1000ms();
void SYSTEM_Reboot();
unsigned long SYSTEM_10millis();

#endif