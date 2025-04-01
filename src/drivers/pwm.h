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

#ifndef PWM_H
#define PWM_H

enum PWM_MODE_t : uint8_t
{
    PWM_MODE_FAST_PWM_15KHZ = 0, // FAST PWM mode, f=15kHz
    PWM_MODE_FAST_PWM_31KHZ,     // FAST PWM mode, f=31kHz
    PWM_MODE_FAST_PWM_125KHZ,    // FAST PWM mode, f=125kHz
    PWM_MODE_FAST_PWM_250KHZ,    // FAST PWM mode, f=250kHz
    PWM_MODE_PC_PWM_8KHZ,        // PHASE CORRECT PWM mode, f=8kHz
    PWM_MODE_PC_PWM_15KHZ,       // PHASE CORRECT PWM mode, f=15kHz
    PWM_MODE_PC_PWM_63KHZ,       // PHASE CORRECT PWM mode, f=63kHz
    PWM_MODE_PC_PWM_125KHZ,      // PHASE CORRECT PWM mode, f=125kHz
};
typedef enum PWM_MODE_t PWM_MODE_t;

// Define hysteresis voltage in mV between input and output voltage for determining whether device is stepping up or steeping down the voltage 
#define PWM_STEP_UP_STEP_DOWN_HYSTERESIS TO_MILI(2.5)
// PWM mode default
#define PWM_MODE_DEFAULT PWM_MODE_FAST_PWM_125KHZ
// PWM mode activated under high load in step-down mode
#define PWM_STEP_DOWN_MODE_HIGH_LOAD PWM_MODE_FAST_PWM_15KHZ
// Define duty cycle threshold for deactivating high load mode
#define PWM_HIGH_LOAD_DISABLE (MAX_DUTY_CYCLE / 10)

// PWM output pin drive current
enum PWM_OUTPUT_CURRENT_t
{
    PWM_OUTPUT_CURRENT_12MA = 0, // output 12mA at max (default)
    PWM_OUTPUT_CURRENT_80MA = 1  // output 80mA at max
};
typedef enum PWM_OUTPUT_CURRENT_t PWM_OUTPUT_CURRENT_t;

// PWM struct
typedef struct
{
    PWM_MODE_t mode; // current PWM mode
} Pwm_t;

void PWM_Setup();
void PWM_SetMode(PWM_MODE_t mode);
void PWM_SetOutputCurrent(PWM_OUTPUT_CURRENT_t current);
void PWM_Tick();
void PWM_TimeSlice1000ms();
void PWM_SetDutyCycle(int duty_cycle);
void PWM_EnableTimerOverflowInterrupt();
#endif