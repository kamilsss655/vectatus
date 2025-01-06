# vectatus
Vectatus - universal portable DC/DC converter

## Introduction
Vectatus is meant to be a portable, universal, programmable DC/DC converter that resolves most of the common power interfacing issues like not having the right battery, or a correct charger at hand.

The device will provide a set of LEDs indicating device operating mode, input voltage, output voltage, warnings, errors etc.

User will be able to switch different modes via onboard buttons. This way no multimeter is required to operate the device, as it usually is the case with traditional DC/DC converters.

Selected modes should be stored in EEPROM, so they do not change in case of power failure. There also should be user lock option so, the mode cannot be accidentally changed possibly causing damage.

Device will provide connections for output and input via:
* pads for attaching wires or alligator clips
* USB type C port
* common lipo battery connector

## Operation
The idea is to have MCU (STM32 probably) run independendly from a wide range of input voltages 0.6V-20V. The MCU power circuit will choose boost or LDO power path automatically for the MCU based on the input voltage.

The DC/DC converion is done from the input source and to output. MCU has it's own power path which is pararell to the DC/DC converter part.

The DC/DC converter topology will either be:
* non-inverting buck-boost
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

## Operation modes
* CV (Constant Voltage) - provide stable voltage for powering different devices
* CC (Constant Current) - provide constant current which can be used as a LED driver or a charger
* Charger - dedicated charger mode that can consist of CC/CV based on the specific needs of the target battery

## Input sources
- 0.5-20V solar panels
- 1.2V NiMh battery - needs to be protected from overdischarge
- 1.5V alkaline - can drain batteries that are classified as "empty" due to joule thief design
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
...

## MCU power circuit
![mcu-power](https://github.com/user-attachments/assets/1c3ee9b1-6d10-4dd9-9490-e3fd14fc396e)

## DC/DC converter circuit
...


## Contributions
The project is still in the design phase. If you have any ideas, suggestions let me know in the [Discussions](https://github.com/kamilsss655/vectatus/discussions) section.

If you'd like to move this project forward any donations towards prototyping and design are welcome.
