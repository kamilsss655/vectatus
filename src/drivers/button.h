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

#ifndef BUTTON_H
#define BUTTON_H

// Mode button pin number
#define BUTTON_PIN_MODE 2
// Output button pin number
#define BUTTON_PIN_OUTPUT 3

// Button hold timeout in seconds (determines long press / hold button event)
#define BUTTON_TIMEOUT_SEC 2

// Helper to determine how many 100ms slices are in 1 second
#define _100MS_TO_SEC 10

void BUTTON_Setup();
void BUTTON_ModePressed();
void BUTTON_ModeHeld();
void BUTTON_OutputPressed();
void BUTTON_OutputHeld();
void BUTTON_TimeSlice10ms();
void BUTTON_TimeSlice100ms();
void BUTTON_TimeSlice500ms();
void BUTTON_TimeSlice1000ms();

#endif