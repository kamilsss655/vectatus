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

#include "adc.h"
#include "settings.h"
#include "lib/filter.h"

#ifdef OUTPUT_VOLTAGE_FILTER_ATT
static FilterIrrLp_t outputVoltageFilter;
#endif
#ifdef INPUT_VOLTAGE_FILTER_ATT
static FilterIrrLp_t inputVoltageFilter;
#endif
#ifdef INPUT_CURRENT_FILTER_ATT
static FilterIrrLp_t inputCurrentFilter;
#endif
#ifdef OUTPUT_CURRENT_FILTER_ATT
static FilterIrrLp_t outputCurrentFilter;
#endif

// Local functions
#ifdef ADC_AUTO_TRIGGER
static void adc_setup_auto_trigger();
#endif
static void setup_filters();
static inline uint8_t samples_count(uint8_t oversampleBits);

/// @brief Setup board
void ADC_Setup()
{
#ifdef ADC_AUTO_TRIGGER
  // Setup ADC auto trigger
  adc_setup_auto_trigger();
#endif
  // setup ADC reference voltage
  analogReference(ADC_REF_VOLTAGE);
  // setup ADC resolution
  analogReadResolution(ADC_HARDWARE_RESOLUTION);
  // setup IIR filters for ADC readings
  setup_filters();
}

/// @brief Returns oversampled average ADC differential input value
/// @param negativePin analog pin with the lowest voltage
/// @param positivePin analog pin with the highest voltage
/// @param gain gain
/// @param oversampleBits amount of extra bits gathered by oversampling (0-2)
/// @return
uint16_t ADC_analogDiffRead(uint8_t negativePin, uint8_t positivePin, uint8_t gain, uint8_t oversampleBits)
{
  uint32_t adcValue = 0;
  uint8_t samplesCount = samples_count(oversampleBits);

  for (int i = 0; i < samplesCount; i++)
  {
    adcValue += analogDiffRead(negativePin, positivePin, gain); // add reading
  }

  // Decimate by amount of oversampled bits
  adcValue = adcValue >> oversampleBits;
  return adcValue;
}

/// @brief Returns oversampled average ADC input value
/// @param pin analog pin
/// @param oversampleBits amount of extra bits gathered by oversampling (0-2)
/// @return
uint16_t ADC_analogRead(uint8_t pin, uint8_t oversampleBits)
{
  uint32_t adcValue = 0;
  uint8_t samplesCount = samples_count(oversampleBits);

  for (int i = 0; i < samplesCount; i++)
  {
    adcValue += analogRead(pin); // add reading
  }

  // Decimate by amount of oversampled bits
  adcValue = adcValue >> oversampleBits;
  return adcValue;
}

/// @brief
/// @return Returns actual input curent value in mA
uint16_t ADC_InputCurrentVal()
{
  uint64_t adcValue = ADC_analogDiffRead(
      INPUT_CURRENT_ADC_N,
      INPUT_CURRENT_ADC_P,
      INPUT_CURRENT_GAIN,
      INPUT_CURRENT_OVERSAMPLE_BITS);

  // Apply no load calibration
  if (adcValue > gSettings.calibration.input_current_idle)
  {
    adcValue = adcValue - gSettings.calibration.input_current_idle;
  }
  else
  {
    adcValue = 0;
  }

#ifdef INPUT_CURRENT_FILTER_ATT
  adcValue = FILTER_Update(&inputCurrentFilter, adcValue); // Filter
#endif

  adcValue = (adcValue * ADC_REF_VOLTAGE_VALUE * INPUT_CURRENT_RES_MULTIPLIER);
  adcValue = (adcValue) / ((INPUT_CURRENT_RESISTOR_VALUE + gSettings.calibration.input_current) * INPUT_CURRENT_ACTUAL_ADC_MAX_VAL * INPUT_CURRENT_GAIN_VALUE);

  return adcValue;
}

/// @brief
/// @return Returns actual output curent value in mA
uint16_t ADC_OutputCurrentVal()
{
  uint64_t adcValue = ADC_analogDiffRead(
      OUTPUT_CURRENT_ADC_N,
      OUTPUT_CURRENT_ADC_P,
      OUTPUT_CURRENT_GAIN,
      OUTPUT_CURRENT_OVERSAMPLE_BITS);

  // Apply no load calibration
  if (adcValue > gSettings.calibration.output_current_idle)
  {
    adcValue = adcValue - gSettings.calibration.output_current_idle;
  }
  else
  {
    adcValue = 0;
  }

#ifdef OUTPUT_CURRENT_FILTER_ATT
  adcValue = FILTER_Update(&outputCurrentFilter, adcValue); // Filter
#endif

  adcValue = (adcValue * ADC_REF_VOLTAGE_VALUE * OUTPUT_CURRENT_RES_MULTIPLIER);
  adcValue = (adcValue) / ((OUTPUT_CURRENT_RESISTOR_VALUE + gSettings.calibration.output_current) * OUTPUT_CURRENT_ACTUAL_ADC_MAX_VAL * OUTPUT_CURRENT_GAIN_VALUE);

  return adcValue;
}

/// @brief
/// @return Returns actual input voltage value in mV
uint16_t ADC_InputVoltageVal()
{
  uint64_t adcValue = ADC_analogRead(INPUT_VOLTAGE_PIN, INPUT_VOLTAGE_OVERSAMPLE_BITS); // Raw value
#ifdef INPUT_VOLTAGE_FILTER_ATT
  adcValue = FILTER_Update(&inputVoltageFilter, adcValue); // Filter
#endif
  adcValue = (adcValue * ADC_REF_VOLTAGE_VALUE * ((INPUT_VOLTAGE_DIVIDER_FACTOR * INPUT_VOLTAGE_RES_MULTIPLIER) + gSettings.calibration.input_voltage)); // Apply calibration
  adcValue = adcValue / (INPUT_VOLTAGE_ACTUAL_ADC_MAX_VAL * INPUT_VOLTAGE_RES_MULTIPLIER);                                                               // Final value
  return adcValue;
}

/// @brief
/// @return Returns actual output voltage value in mV
uint16_t ADC_OutputVoltageVal()
{
  uint64_t adcValue = ADC_analogRead(OUTPUT_VOLTAGE_PIN, OUTPUT_VOLTAGE_OVERSAMPLE_BITS); // Raw value
#ifdef OUTPUT_VOLTAGE_FILTER_ATT
  adcValue = FILTER_Update(&outputVoltageFilter, adcValue); // Filter
#endif
  adcValue = (adcValue * ADC_REF_VOLTAGE_VALUE * ((OUTPUT_VOLTAGE_DIVIDER_FACTOR * OUTPUT_VOLTAGE_RES_MULTIPLIER) + gSettings.calibration.output_voltage)); // Apply calibration
  adcValue = adcValue / (OUTPUT_VOLTAGE_ACTUAL_ADC_MAX_VAL * OUTPUT_VOLTAGE_RES_MULTIPLIER);                                                                // Final value
  return adcValue;
}

#ifdef ADC_AUTO_TRIGGER
// This sync ADC reads with PWM output on TIMER0 preventing noise and glitches
static void adc_setup_auto_trigger()
{
  noInterrupts();
  // enable auto-triggering
  ADCSRA |= (1 << ADATE);
  // enable trigger source on timer 0 overflow interrupt
  ADCSRB |= (1 << ADTS2);
  interrupts();
}
#endif

// Setup IIRC filters
static void setup_filters()
{
#ifdef INPUT_VOLTAGE_FILTER_ATT
  FILTER_Init(&inputVoltageFilter, INPUT_VOLTAGE_FILTER_ATT);
#endif
#ifdef OUTPUT_VOLTAGE_FILTER_ATT
  FILTER_Init(&outputVoltageFilter, OUTPUT_VOLTAGE_FILTER_ATT);
#endif
#ifdef INPUT_CURRENT_FILTER_ATT
  FILTER_Init(&inputCurrentFilter, INPUT_CURRENT_FILTER_ATT);
#endif
#ifdef OUTPUT_CURRENT_FILTER_ATT
  FILTER_Init(&outputCurrentFilter, OUTPUT_CURRENT_FILTER_ATT);
#endif
}

/// @brief Get required samples count for desired oversample bits amount.
/// This basically is a fast 4 to the power of bits function limited to 0-2 input values
/// @param oversampleBits desired oversample bits 0-2
/// @return samples count is 4 to the power of oversampled bits required
static inline uint8_t samples_count(uint8_t oversampleBits)
{
  switch (oversampleBits)
  {
  case 1:
    return 4;
  case 2:
    return 16;
  default:
    return 1;
  }
}