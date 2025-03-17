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
* User has one AA 1.5V alkaline battery but wants to use 3V 2xAA battery device
* User has one AA 1.5V alkaline battery but wants to charge the phone via USB C cable
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

## MCU used
MCU used is:
* LGT8F328P
  - low cost Atmega clone with some improvements
  - the programable gain op-amp is especially appealing for this use case


## Contributions
The project is still in the design phase. If you have any ideas, suggestions let me know in the [Discussions](https://github.com/kamilsss655/vectatus/discussions) section.

If you'd like to move this project forward any donations towards prototyping and design are welcome.
