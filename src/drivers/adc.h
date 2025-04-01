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

#ifndef ADC_H
#define ADC_H

#include <Arduino.h>
#include <differential_amplifier.h>

#include "lib/util.h"

// ADC reference voltage
#define ADC_REF_VOLTAGE INTERNAL1V024
// ADC reference in mV - used for calculations, must reflect ADC_REF_VOLTAGE used
#define ADC_REF_VOLTAGE_VALUE TO_MILI(1.024)

// ADC resolution
#define ADC_HARDWARE_RESOLUTION 12 // ADC hardware resolution = 10, 11 or 12 Bit

// ADC auto-trigger - if defined it will sync ADC measurements with the PWM mosfet drive output
// NOTE: Looks like when enabled it significantly reduces output voltage ripple in CV mode,
// however in CC mode it causes the measured output voltage to be higher than actual
// perhaps this should be dynamic based on mode used?
#define ADC_AUTO_TRIGGER

// Input current ADC channel +
#define INPUT_CURRENT_ADC_P A0
// Input current ADC channel -
#define INPUT_CURRENT_ADC_N A1
// Input current ADC gain
#define INPUT_CURRENT_GAIN GAIN_16
#define INPUT_CURRENT_GAIN_VALUE 16 // used for calculations, must reflect INPUT_CURRENT_GAIN used
// Value in [mohm] of the input current sense resistor used
#define INPUT_CURRENT_RESISTOR_VALUE TO_MILI(0.02)
// Oversample bits - by oversampling and decimation it adds extra bits of resolution to the ADC readings
#define INPUT_CURRENT_OVERSAMPLE_BITS 1
// Actual resolution is the sum of hardware ADC resolution and oversample bits
#define INPUT_CURRENT_ACTUAL_ADC_RESOLUTION SUM(ADC_HARDWARE_RESOLUTION, INPUT_CURRENT_OVERSAMPLE_BITS)
// Max actual value of the ADC
#define INPUT_CURRENT_ACTUAL_ADC_MAX_VAL POW(2, INPUT_CURRENT_ACTUAL_ADC_RESOLUTION)
// Resolution multiplier to increase calibration resolution
#define INPUT_CURRENT_RES_MULTIPLIER 1000
// Filter attenuation - the higher the value the lower the response time
#define INPUT_CURRENT_FILTER_ATT 10

// Output current ADC channel +
#define OUTPUT_CURRENT_ADC_P A2
// Output current ADC channel -
#define OUTPUT_CURRENT_ADC_N A3
// Output current ADC gain
#define OUTPUT_CURRENT_GAIN GAIN_16
#define OUTPUT_CURRENT_GAIN_VALUE 16
// Value in [mohm] if the output current sense resistor used
#define OUTPUT_CURRENT_RESISTOR_VALUE TO_MILI(0.02)
// Oversample bits - by oversampling and decimation it adds extra bits of resolution to the ADC readings
#define OUTPUT_CURRENT_OVERSAMPLE_BITS 1
// Actual resolution is the sum of hardware ADC resolution and oversample bits
#define OUTPUT_CURRENT_ACTUAL_ADC_RESOLUTION SUM(ADC_HARDWARE_RESOLUTION, OUTPUT_CURRENT_OVERSAMPLE_BITS)
// Max actual value of the ADC
#define OUTPUT_CURRENT_ACTUAL_ADC_MAX_VAL POW(2, OUTPUT_CURRENT_ACTUAL_ADC_RESOLUTION)
// Resolution multiplier to increase calibration resolution
#define OUTPUT_CURRENT_RES_MULTIPLIER 1000
// Filter attenuation - the higher the value the lower the response time
// #define OUTPUT_CURRENT_FILTER_ATT 2

// Input voltage pin
#define INPUT_VOLTAGE_PIN 18
// Input voltage R1 resistor value (in kohm)
#define INPUT_VOLTAGE_R1_VALUE 100.0
// Input voltage R2 resistor value (in kohm)
#define INPUT_VOLTAGE_R2_VALUE 6.2
// Voltage divider factor
#define INPUT_VOLTAGE_DIVIDER_FACTOR V_DIV_FACTOR(INPUT_VOLTAGE_R1_VALUE, INPUT_VOLTAGE_R2_VALUE)
// Resolution multiplier to increase calibration resolution
#define INPUT_VOLTAGE_RES_MULTIPLIER 10
// Oversample bits - by oversampling and decimation it adds extra bits of resolution to the ADC readings
#define INPUT_VOLTAGE_OVERSAMPLE_BITS 1
// Actual resolution is the sum of hardware ADC resolution and oversample bits
#define INPUT_VOLTAGE_ACTUAL_ADC_RESOLUTION SUM(ADC_HARDWARE_RESOLUTION, INPUT_VOLTAGE_OVERSAMPLE_BITS)
// Max actual value of the ADC
#define INPUT_VOLTAGE_ACTUAL_ADC_MAX_VAL POW(2, INPUT_VOLTAGE_ACTUAL_ADC_RESOLUTION)
// Filter attenuation - the higher the value the lower the response time
// #define INPUT_VOLTAGE_FILTER_ATT 2

// Output voltage pin
#define OUTPUT_VOLTAGE_PIN 19
// Output voltage R1 resistor value (in kohm)
#define OUTPUT_VOLTAGE_R1_VALUE 100.0
// Output voltage R2 resistor value (in kohm)
#define OUTPUT_VOLTAGE_R2_VALUE 6.2
// Voltage divider factor
#define OUTPUT_VOLTAGE_DIVIDER_FACTOR V_DIV_FACTOR(OUTPUT_VOLTAGE_R1_VALUE, OUTPUT_VOLTAGE_R2_VALUE)
// Resolution multiplier to increase calibration resolution
#define OUTPUT_VOLTAGE_RES_MULTIPLIER 10
// Oversample bits - by oversampling and decimation it adds extra bits of resolution to the ADC readings
#define OUTPUT_VOLTAGE_OVERSAMPLE_BITS 2
// Actual resolution is the sum of hardware ADC resolution and oversample bits
#define OUTPUT_VOLTAGE_ACTUAL_ADC_RESOLUTION SUM(ADC_HARDWARE_RESOLUTION, OUTPUT_VOLTAGE_OVERSAMPLE_BITS)
// Max actual value of the ADC
#define OUTPUT_VOLTAGE_ACTUAL_ADC_MAX_VAL POW(2, OUTPUT_VOLTAGE_ACTUAL_ADC_RESOLUTION)
// Filter attenuation - the higher the value the lower the response time
// TODO: Might need to lower it to 14 for other modes to prevent oscilation
// #define OUTPUT_VOLTAGE_FILTER_ATT 77

void ADC_Setup();
uint16_t ADC_analogDiffRead(uint8_t negativePin, uint8_t positivePin, uint8_t gain, uint8_t oversampleBits);
uint16_t ADC_analogRead(uint8_t pin, uint8_t oversampleBits);
uint16_t ADC_InputCurrentVal();
uint16_t ADC_OutputCurrentVal();
uint16_t ADC_InputVoltageVal();
uint16_t ADC_OutputVoltageVal();
void BOARD_Reset();

#endif