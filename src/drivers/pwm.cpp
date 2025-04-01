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

#include <Arduino.h>

#include "pwm.h"
#include "app.h"

// Local functions
static void auto_adjust_mode();

static Pwm_t pwm;

/// @brief Setup PWM on Timer 0, pin D6
//  the higher the freq, the higher the achievable voltage on output,
//  hower it also requires increases to the ACCEPTABLE_OUTPUT_VOLTAGE_RIPPLE

// note: use Fast PWM mode for drive pin (datasheet recommends it  for dc converters due to better slope), also it seems I can enable 80mA output, which might make
// the slope even sharper
// I can even build some simple sepic circuit and drive it from breadboard
// phase correct pwm can be interesitng due to symmetric nature might work for electric motors / inductive loads
// For adjustments use this great tool: https://dbuezas.github.io/arduino-web-timers/#mcu=LGT8F328P&timer=0&CompareOutputModeA=set-on-match%2C+clear-at-max&OCR0A=255

// note: this also depends on the total charge of the power mosfet used
// the higher the total gate charge of the mosfet the slower it will switch
// ie. for IRF3205:
// t=C/I
// t(ns) = 146(nC)/0.08(A) = 1825 ns
// f = 1/(2 * t) ~ 273 kHz
// ie. for FQP30N06
// t=C/I
// t(ns) = 25(nC)/0.08(A) = 312.5 ns
// f = 1/(2 * t) ~ 1602 kHz
// as a rule of thumb the mosfet switching frequency should be 10 times greater than the PWM switching frequency
// above calculations show that IRF3205 might not be able to handle PWM frequencies above 31kHz, while FQP30N06
// should be able to handle 125kHz

void PWM_Setup()
{
  // Set PWM mode
  PWM_SetMode(PWM_MODE_DEFAULT);
  // Set duty cycle to 0
  PWM_SetDutyCycle(0);
  // Set PWM drive pin output current to 80mA - might improve MOSFET switching slope
  PWM_SetOutputCurrent(PWM_OUTPUT_CURRENT_80MA);
  // Enable TIMER0 overflow interrupt - required for ADC auto triggering sync
  PWM_EnableTimerOverflowInterrupt();
}

void PWM_Tick()
{
  PWM_SetDutyCycle(gApp.duty_cycle);
}

void PWM_TimeSlice1000ms()
{
  auto_adjust_mode();
}
/// @brief Set the duty cycle
/// @param duty_cycle - 0-255 value, 0 off, 255 fully on
void PWM_SetDutyCycle(int duty_cycle)
{
  noInterrupts();
  OCR0A = MAX_PWM_RESOLUTION - duty_cycle;
  interrupts();
}

/// @brief Set PWM mode
/// @param mode mode type
void PWM_SetMode(PWM_MODE_t mode)
{
  pwm.mode = mode;

  noInterrupts();

  switch (mode)
  {
  case PWM_MODE_FAST_PWM_15KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM01 |
        1 << WGM00;
    TCCR0B =
        1 << CS01;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_FAST_PWM_31KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM01 |
        1 << WGM00;
    TCCR0B =
        1 << CS01;
    TCKCSR =
        1 << F2XEN |
        1 << TC2XS0;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_FAST_PWM_125KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM01 |
        1 << WGM00;
    TCCR0B =
        1 << CS00;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_FAST_PWM_250KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM01 |
        1 << WGM00;
    TCCR0B =
        1 << CS00;
    TCKCSR =
        1 << F2XEN |
        1 << TC2XS0;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_PC_PWM_8KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM00;
    TCCR0B =
        1 << CS01;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_PC_PWM_15KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM00;
    TCCR0B =
        1 << CS01;
    TCKCSR =
        1 << F2XEN |
        1 << TC2XS0;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_PC_PWM_63KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM00;
    TCCR0B =
        1 << CS00;
    DDRD =
        1 << DDD6;
    break;
  case PWM_MODE_PC_PWM_125KHZ:
    TCCR0A =
        1 << COM0A1 |
        1 << COM0A0 |
        1 << WGM00;
    TCCR0B =
        1 << CS00;
    TCKCSR =
        1 << F2XEN |
        1 << TC2XS0;
    DDRD =
        1 << DDD6;
    break;
  }

  interrupts();
}

/// @brief Set PWM output pin drive current
/// @param current pwm output pin drive current (default is 12mA)
void PWM_SetOutputCurrent(PWM_OUTPUT_CURRENT_t current)
{
  noInterrupts();
  HDR = current << HDR1;
  interrupts();
}

/// @brief Enable TIMER0 overflow interrupt - required for ADC auto triggering sync
void PWM_EnableTimerOverflowInterrupt()
{
  noInterrupts();
  TIMSK0 =
      1 << TOIE0;
  interrupts();
}

// Auto adjust mode (switching frequency)
static void auto_adjust_mode()
{
  // if duty cycle is max and device is in step down mode - activate high load PWM mode
  // activating it in step-up mode doesn't make sense as the output ripple voltages are too high
  if (pwm.mode == PWM_MODE_DEFAULT && gApp.duty_cycle == MAX_DUTY_CYCLE  && (gApp.input_voltage > gApp.output_voltage + PWM_STEP_UP_STEP_DOWN_HYSTERESIS))
  {
    PWM_SetMode(PWM_STEP_DOWN_MODE_HIGH_LOAD);
#ifdef DEBUG_MODE
    Serial.println(F("PWM_STEP_DOWN_MODE_HIGH_LOAD"));
#endif
  }
  // if no high load detected - go back to default PWM mode
  if (pwm.mode == PWM_STEP_DOWN_MODE_HIGH_LOAD && gApp.duty_cycle <= PWM_HIGH_LOAD_DISABLE)
  {
    // set mode to default
    PWM_SetMode(PWM_MODE_DEFAULT);
#ifdef DEBUG_MODE
    Serial.println(F("PWM_MODE_DEFAULT"));
#endif
  }
}