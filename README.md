# nc-sim

This is a very simple G-Code intepreter and CNC driver.

The package is divided in 2 parts:

* GUI G-Code inprepreter running on PC. It can understand G00, G01, G02, G03 and G17 commands.
* A microcontroller firmware for driving stepper motors. It has versions for AVR8 (tested on Atmega8) and STM32. It can drive 3x L298 H-bridges in half-steps.

Communication is sone with UART and a very basic, single byte, protocol:

```
PC->uC
0b00xxxxxx - steps
0b00xxxxx1 X+
0b00xxxx1x Y+
0b00xxx1xx Z+
0b00xx1xxx X-
0b00x1xxxx Y-
0b001xxxxx Z-

0b1xxxxxxx - commands
0b101000xx - power setup, 0b10100001 - steppers up, 0b10100000 - steppers and DC motor up

0b11000000 - velocity setup, timer value in next byte

uC->PC
0b01111111 - echo from PC
0b1xxxxxxx - additional data
```
