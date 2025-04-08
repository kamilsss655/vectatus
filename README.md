# vectatus
Vectatus - universal portable programmable DC/DC converter

![7](https://github.com/user-attachments/assets/907becdf-10fd-4e0c-bb57-2495eac52631)

![vectatus-pcb](https://github.com/user-attachments/assets/19aba33c-e9f4-452a-86b7-c66815fba3c6)

![back](https://github.com/user-attachments/assets/fb1222c6-c984-4e3c-9004-d7b7184c00cf)

![high-level-vectatus](https://github.com/user-attachments/assets/da3f6433-87c1-4ef2-8a7e-b05aaf931699)

## Introduction
Vectatus is meant to be a portable, universal, programmable DC/DC converter that resolves most of the common power interfacing issues like not having the right battery, or a correct charger at hand.

The device will provide a set of LEDs indicating device operating mode, input voltage, output voltage, warnings, errors etc.

User will be able to switch different modes via onboard buttons. This way no multimeter is required to operate the device, as it usually is the case with traditional DC/DC converters.

Selected modes should be stored in EEPROM, so they do not change in case of power failure. There also should be user lock option so, the mode cannot be accidentally changed possibly causing damage.

Device will provide connections for output and input via:
* pads for attaching wires or alligator clips
* USB type C port
* common lipo battery connector

> [!WARNING]
> This custom-built evaluation kit is intended for use by professionals and is solely designated for research and development purposes at appropriate facilities. Any usage of the device outside of these parameters may result in fire, damage to property, or other hazards. Users are responsible for ensuring compliance with all local laws and regulations governing the use of such technology. The author of this kit shall not be held liable for any misuse or unlawful activities conducted by the user. By using this kit, you acknowledge and accept these terms and conditions.

> [!WARNING]
> USB C output is directly connected to the output of the converter. Thus if the user selects voltage output which is outside of the USB spec, like 9V, then 9V will appear on the USB C output possibly destroying the device connected.

## Table of contents
* [Operation](#operation)
* [Operation modes](#operation-modes)
* [Input sources](#input-sources)
* [Output sources](#output-sources)
* [Use cases](#use-cases)
* [Efficiency](#efficiency)
* [Parts used](#parts-used)
* [Schematic](#schematic)
* [Hardware connections](#hardware-connections)
* [Controls](#controls)
* [Calibration mode](#calibration-mode)
* [Wiki](#wiki)

## Operation
The idea is to have MCU run independendly from a wide range of input voltages 2V-20V.

The DC/DC converion is done from the input source and to output. MCU has it's own power path which is pararell to the DC/DC converter part.

The DC/DC converter topology is:
* SEPIC

SEPIC seems favorable for this kind of application due to decoupling capacitor acting as a short circuit protection.

The MCU will measure:
* input voltage
* input current
* output voltage
* output current

Above set of data is enough to dynamically measure efficiency, so the MCU can adjust conversion parameters like frequency etc. in order to achieve best efficiency.

Features:
* reverse polarity protection on input
* short-circuit protection on output
* overcurrent protection on output and input
* overdischarge protection
* soft start
  - slowly ramp-up the voltage at start
* overvoltage protection on output when load is disconnected
  - when voltage is raises 2-3V above preset value, immediately turn the duty cycle to 0 to prevent voltage spikes

## Operation modes
* Idle mode - no output present, input and output is isolated via SEPIC coupling capacitor
* CV (Constant Voltage) - provide stable voltage for powering different devices
* CC (Constant Current) - provide constant current which can be used as a LED driver or a charger
* Charger - dedicated charger mode that can consist of CC/CV based on the specific needs of the target battery
* MPPT - dedicated mode for use with solar panels
* Calibration mode - allows users to fine tune input and output voltages and currents via onboard buttons (NO PC NEEDED to calibrate)

## Input sources
- 1.8-20V solar panels
- 2 x 1.2V NiMh battery - needs to be protected from overdischarge
- 2 x 1.5V alkaline - can drain batteries that are classified as "empty" due to joule thief design
- 3.7V LiPo, Lithium-ion battery - needs to be protected from overdischarge
- 5V USB - needs overcurrent protection
- 9V battery
- 12V car battery - needs to be protected from overdischarge
- 18V power tool battery / laptop battery - needs to be protected from overdischarge

## Output sources
- 1.5V - emulates single AA, AAA battery
- 3V - emulates double AA, AAA battery
- 3.7V LiPo, Lithium-ion battery - acts as a charger or emulates a battery
- 5V USB - for powering USB devices
- 9V - emulates 9V battery
- 12V - emulates 12V battery or acts as a car battery charger
- 18V - emulates 18V battery (no support for charching multi-cell batteries due to design limitations)
- manual mode - CV 0-20V, CC 0-3A adjustable via buttons (user needs multimeter)

## Use cases
Here are some examples of use cases:
* User has 2x AA 1.5V alkaline batteries but wants to charge the phone via USB C cable
* User has 2X AA batteries, but wants to use 9V battery device
* User has a solar panel, but wants to charge the 3.7V LiPo battery
* User has a solar panel, but wants to charge the phone via USB C cable
* User has a solar panel, but wants to charge the 12V car battery
* User has a solar panel, but wants to charge the 18V power tool battery
* User has 5V USB C charger, but wants to charge 12V car battery
* User has 5V USB C charger, but wants to charge 3.7V LiPo battery
* User has 5V USB C charger, but wants to use 3V 2xAA battery device
* User has 5V USB C charger, but wants to drive power LED at 0.5/1A constant current
* User has 3.7V LiPo battery, but wants to power 5V USB C device
* User has 3.7V LiPo battery, but wants to power 9V battery device
* User has 18V power tool battery, but wants to charge the phone via USB C cable

## Efficiency
Efficiency is around **78%** depending on input and output conditions.

Efficiency could likely be raised to ~89% by increasing the gate drive current from 80mA to 500mA by using a totem pole driver.

More efficiency tests can be found here: https://github.com/kamilsss655/vectatus/wiki/Tests#efficiency-tests

## Parts used
* MCU used is:
  * LGT8F328P
    - low cost Atmega clone with some improvements
    - the programable gain op-amp is especially appealing for this use case
* N-channel MOSFET
  * FQP30N06L
    - fast, cheap, logic level MOSFET

## Schematic

V3.1:
![schematic-3-1](https://github.com/user-attachments/assets/2f25cf6b-0219-48fc-81a4-24aed2730ba3)

## Hardware connections

The LGT8F328P can be programmed either via:
* ISP programmer - ISP programmer can be made from Arduino board (i.e Arduino Uno) detailed instructions here: https://github.com/kamilsss655/LGTISP and here https://github.com/dbuezas/lgt8fx/blob/master/lgt8f/libraries/LarduinoISP/readme.md , this is likely needed only once to burn the Arduino bootloader onto the chip (unless it comes with bootloader burned in already)

| Nano      | LGT8F328P | 
|:---------:|:---------:| 
| D13       |  SWC    | 
| D12       |  SWD    | 
| D10       |  RST (optional) | 
| VCC       |  VCC    | 
| GND       |  GND    | 

      Note: A capacitor (e.g 10uF) between the RESET pin and VCC pin of ISP programmer is needed

* Serial connection (for example using FTDI cable or any other serial to USB cable):

| FTDI      | LGT8F328P|
|:---------:|:---------:|
| RX        |TX         |
| TX        |RX         |
| DTR       |RST (optional, must use 0.1uF capacitor in between these lines, otherwise the autoreset will not work)|
| VCC       |VCC        |
| GND       |GND        |

      for FTDI adapter either `3.3V` or `5V` mode can be used

## Controls
Control over the device is done via 2 buttons:
* `MODE`
  * `short press` - enable or disable the output
  * `hold` - go to next mode
* `OUTPUT`
  * `short press` - change output params
  * `hold` - change secondary output params (depending on mode)

## Calibration mode
To enter calibration mode hold OUTPUT and MODE buttons while device is being turned on, the LEDS will blink, then release all buttons.

The calibration point ( output voltage and current) will be loaded from the EEPROM as last used CC/CV mode settings, so user can calibrate different output points based on preference.

Most of the calibrations can be done by attaching voltmeter or ampere meter directly to the output terminals of Vectatus.

Calibration mode has following sub-modes (steps):
  * `CALIBRATION_MODE_IDLE_CURRENT` - measure idle input and output current calibration values (no user intervention required, goes to next step almost immediatelly)
  * `CALIBRATION_MODE_INPUT_VOLTAGE` - press MODE/OUTPUT to increase/decrease input voltage calibration value while measuring the input voltage with voltmeter and looking at serial output of Vectatus, once the values match hold MODE button to go to next step
  * `CALIBRATION_MODE_OUTPUT_VOLTAGE` - press MODE/OUTPUT to increase/decrease output voltage calibration value while measuring the output voltage with voltmeter, once the values match hold MODE button to go to next step
  * `CALIBRATION_MODE_INPUT_CURRENT` - press MODE/OUTPUT to increase/decrease input current calibration value while measuring the input current with ampere meter and looking at serial output of Vectatus, once the values match hold MODE button to go to next step
  * `CALIBRATION_MODE_OUTPUT_CURRENT` - press MODE/OUTPUT to increase/decrease output current calibration value while measuring the output current with ampere meter (ampere meter should be high range >5A, as depending on programmed current it can deliver upto 3A, you essentially will create a short with ampere meter connected to the Vectatus output which is fine), once the values match hold MODE button to go to next step

  Note: You can skip steps by holding MODE button (the values of these steps will not get changed).

  After calibration wait around 10-30 seconds so the calibration values will get saved to EEPROM (entering ERROR mode right after would prevent them from saving).


## Wiki
Please take a look at the [Wiki](https://github.com/kamilsss655/vectatus/wiki) section.
