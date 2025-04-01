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

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define SUM(a, b) ((a) + (b))
#define POW(a, b) (pow(a,b))
#define V_DIV_FACTOR(r1, r2) ((uint16_t)((r1+r2)/r2))

// Convinience macro to convert float to uint32_t, V to mV, A to mA etc.
// This way we don't use floating numbers math in runtime, but can express voltages in volts during programming etc.
#define TO_MILI(a) ((uint32_t)(a*1000))
// Convinience macro to convert uint16_t to float, mV to V, mA to A etc.
// shouldn't that be a function?
// #define FROM_MILI(a) ((float)(a/1000))

#endif