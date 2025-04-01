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

#include <WDT.h>
#include <lgt_LowPower.h>

#include "app.h"
#include "drivers/adc.h"
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pwm.h"
#include "system.h"
#include "settings.h"

// main timekeeping via timer
volatile bool tick10ms = 0;
volatile unsigned long timer2_10millis = 0;
// time slice tick counters
uint8_t slice10ms, slice100ms, slice500ms;

static void watchdog_enable();
static void system_clock_timer_enable();
static void send_welcome_message();

// Interrupt handler when TIMER2 overflows (happens every 10.24ms)
ISR(TIMER2_OVF_vect)
{
  // indicate that 10ms has passed
  tick10ms = 1;
  timer2_10millis += 1;
}

/// @brief Setup system
void SYSTEM_Setup()
{
  // setup ADC
  ADC_Setup();
  // setup LEDS
  LED_Setup();
  // setup buttons
  BUTTON_Setup();
  // setup PWM
  PWM_Setup();
  // setup serial connection
  Serial.begin(SERIAL_BAUD_RATE);
  // send welcome message
  send_welcome_message();
  // enable watchdog
  watchdog_enable();
  // setup TIMER2 to generate timer overflow interrupt every 10.24 ms
  system_clock_timer_enable();
  // setup app
  APP_Setup();
}

/// @brief Perform system logic
void SYSTEM_Tick()
{
  // Reset system watchdog timer
  wdt_reset();

  // Perform app logic every tick
  APP_Tick();

  // Detect if 10ms has passed
  if (tick10ms)
  {
    // reset the indicator
    tick10ms = 0;
    // runs every 10ms
    SYSTEM_TimeSlice10ms();
  }
}

void SYSTEM_TimeSlice10ms()
{
  // every 10ms, place logic here:
  APP_TimeSlice10ms();
  LED_TimeSlice10ms();
  BUTTON_TimeSlice10ms();

  // Propagate tick
  if (slice10ms < 10 - 1)
  {
    slice10ms += 1;
  }
  else
  {
    slice10ms = 0;
    SYSTEM_TimeSlice100ms();
  }
}

void SYSTEM_TimeSlice100ms()
{
  // every 100ms, place logic here:
  APP_TimeSlice100ms();
  LED_TimeSlice100ms();
  BUTTON_TimeSlice100ms();

  // Propagate tick
  if (slice100ms < 5 - 1)
  {
    slice100ms += 1;
  }
  else
  {
    slice100ms = 0;
    SYSTEM_TimeSlice500ms();
  }
}

void SYSTEM_TimeSlice500ms()
{
  // every 500ms, place logic here:
  APP_TimeSlice500ms();
  LED_TimeSlice500ms();
  BUTTON_TimeSlice500ms();

  // Propagate tick
  if (slice500ms < 2 - 1)
  {
    slice500ms += 1;
  }
  else
  {
    slice500ms = 0;
    SYSTEM_TimeSlice1000ms();
  }
}

void SYSTEM_TimeSlice1000ms()
{
  // every 1000ms, place logic here:
  APP_TimeSlice1000ms();
  SETTINGS_TimeSlice1000ms();
  LED_TimeSlice1000ms();
  BUTTON_TimeSlice1000ms();
  PWM_TimeSlice1000ms();
}

// Reboot the system
void SYSTEM_Reboot()
{
  // Set all pins to input
  DDRB = 0x00;
  DDRC = 0x00;
  DDRD = 0x00;
  DDRE = 0x00;
  // Enable watchdog
  wdt_enable(SYSTEM_WATCHDOG_TIMEOUT);
  // Enter infinite loop causing WDT reset
  while (1)
  {
  }
}

/// @brief This returns value of how many 10 miliseconds have passed since the system started.
/// This is a replacement for millis() function which depends on TIMER2 instead of TIMER0.
/// Since TIMER0 is used for PWM in this board, it could not be used for timekeeping.
/// @return how many 10ms passed since system started
unsigned long SYSTEM_10millis()
{
  return timer2_10millis;
}

// Enable watchog
static void watchdog_enable()
{
  // Use HFRC oscillator for watchdog
  Lgtwdt.begin(WTOH_32MHZ);
#ifdef DEBUG_MODE
  wdt_enable(SYSTEM_WATCHDOG_TIMEOUT_DEBUG_MODE);
#else
  wdt_enable(SYSTEM_WATCHDOG_TIMEOUT);
#endif
}

// Enable TIMER2 to generate timer overflow interrupt every 10.24 ms
static void system_clock_timer_enable()
{
  noInterrupts();
  TCCR2A =
      1 << WGM20;
  TCCR2B =
      1 << WGM22 |
      1 << CS22 |
      1 << CS21 |
      1 << CS20;
  TIMSK2 =
      1 << TOIE2;
  OCR2A = 160;
  interrupts();
}

// Send welcome message
static void send_welcome_message()
{
  Serial.println(F(SYSTEM_WELCOME_MESSAGE));
}
