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

#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

// IIR (Infinite Impulse Response) low pass filter
typedef struct
{
    uint32_t current_val; // holds current ADC value
    uint8_t attenuation;  // high frequency attenuation, higher the value = slower response
} FilterIrrLp_t;

void FILTER_Init(FilterIrrLp_t *filter, uint8_t attenuation);
uint16_t FILTER_Update(FilterIrrLp_t *filter, uint16_t newInput);
#endif