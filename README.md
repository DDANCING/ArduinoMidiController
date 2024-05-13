# Arduino MIDI Controller

This project is an Arduino-based MIDI controller that utilizes buttons and potentiometers to send MIDI commands. It is designed to work with multiple Arduino boards, including ATmega328 (Uno, Mega, Nano) and ATmega32U4 (Micro, Pro Micro, Leonardo). The controller can send both MIDI note and control change (CC) messages.

## Table of Contents

1. [Components](#components)
2. [Libraries](#libraries)
3. [Wiring](#wiring)
4. [Code Explanation](#code-explanation)
5. [Usage](#usage)
6. [License](#license)

## Components

- Arduino board (Uno, Mega, Nano, Micro, Pro Micro, Leonardo)
- Buttons (8)
- Potentiometers (2)
- LiquidCrystal_I2C display (optional)
- Servo (optional)
- Connecting wires
- Breadboard (optional)

## Libraries

Ensure you have the following libraries installed:

- `LiquidCrystal_I2C`
- `Servo`
- `ResponsiveAnalogRead` - [Download from GitHub](https://github.com/dxinteractive/ResponsiveAnalogRead)
- `MIDI` or `MIDIUSB` (depending on the board used)

## Wiring

### Buttons

Connect the buttons to the Arduino digital pins as follows:

| Button | Arduino Pin |
|--------|--------------|
| 1      | 1            |
| 2      | 2            |
| 3      | 3            |
| 4      | 4            |
| 5      | 5            |
| 6      | 6            |
| 7      | 7            |
| 8      | 8            |

### Potentiometers

Connect the potentiometers to the Arduino analog pins:

| Potentiometer | Arduino Pin |
|---------------|--------------|
| 1             | A0           |
| 2             | A1           |

### Note:

- Ensure each button is connected with a pull-up resistor or use the internal pull-up resistor.
- For the potentiometers, ensure the outer pins are connected to GND and 5V, and the middle pin is connected to the analog input.

## Code Explanation

### Preprocessor Directives

The code begins with defining constants and including necessary libraries. The type of Arduino board and the number of input devices (buttons and potentiometers) are specified.

### Button Handling

- Buttons are debounced using a debounce delay to avoid multiple reads due to mechanical bouncing.
- Button states are monitored to send MIDI note on/off messages based on button presses/releases.

### Potentiometer Handling

- Potentiometers are read using the `ResponsiveAnalogRead` library for smooth analog input.
- The readings are mapped to MIDI CC values (0-127) and sent as MIDI CC messages when the potentiometer values change.

### MIDI Handling

Depending on the board type, different MIDI handling libraries (`MIDI`, `MIDIUSB`) are used to send MIDI messages.

### Setup and Loop

- `setup()`: Initializes serial communication and sets up the input devices (buttons and potentiometers).
- `loop()`: Continuously checks the states of buttons and potentiometers and sends appropriate MIDI messages.

## Usage

1. Upload the code to your Arduino board.
2. Connect the MIDI output to your computer or MIDI device using appropriate software/hardware.
3. Press buttons to send MIDI note on/off messages.
4. Turn potentiometers to send MIDI CC messages.

### Serial Communication

- Baud rate is set to 115200 for use with Hairless MIDI.
- Adjust the baud rate to 31250 if connecting directly to MIDI-compliant devices.

### Debug Mode

- Enable debug mode by defining `DEBUG` to print button and potentiometer states to the serial monitor for troubleshooting.

## License

This project is open-source and available under the MIT License. Feel free to use, modify, and distribute the code with appropriate credit.

---

This README provides an overview of the Arduino MIDI controller project. Follow the wiring instructions, upload the code, and interact with buttons and potentiometers to send MIDI messages. Happy tinkering!

![image](https://github.com/DDANCING/ArduinoMidiController/assets/100043354/2f1c5ff5-10d7-4071-a213-6714312dabab)

![image](https://github.com/DDANCING/ArduinoMidiController/assets/100043354/a08c6a65-857d-4c94-bf10-af40af859288)

![image](https://github.com/DDANCING/ArduinoMidiController/assets/100043354/bff431a8-1135-45bf-b645-f883a1f104d7)


