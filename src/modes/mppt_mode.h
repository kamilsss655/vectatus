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

#ifndef MPPT_MODE_H
#define MPPT_MODE_H

void MPPT_MODE_Init();
void MPPT_MODE_Tick();
void MPPT_MODE_TimeSlice10ms();
void MPPT_MODE_TimeSlice100ms();
void MPPT_MODE_TimeSlice500ms();
void MPPT_MODE_TimeSlice1000ms();
void MPPT_MODE_ModeBtnPressed();
void MPPT_MODE_ModeBtnHeld();
void MPPT_MODE_OutputBtnPressed();
void MPPT_MODE_OutputBtnHeld();
#endif