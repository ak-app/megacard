# Megacard Library

This library is developed for using with Microchip ATmega16. This  chip is mounted on the Megacard. Every library has a **main.c** that describes how to use it. Detailed setup can be done in the **.h** file. Normally it is not necessary to edit the **.c** files.

List of libraries:

* [LCD Display](#LCD-Display)
* [Analog/Digital processing](#Analog/Digital-processing)
* [UART Communication](#UART-Communication)
* [SPI Communication](#SPI-Communication)
* [TWI Communication](#TWI-Communication)


## [LCD Display](/library/lcd/)

The lcd example program writes during init different data types to first line. On second line patterns which have previously been written to ram are displayed. With push-button S0 and S1 the data can be shifted right/left.

---

## [Analog/Digital processing](/library/adc/)

The adc example program converts the voltage level from potentiometer R20 to a numerical value (0-255) and writes it to the LEDs on PORTC.

---

## [UART Communication](/library/uart/)

The uart example program sends data to PC and reads in a number that will be multiplied by 5. The Result will be returned to PC and records 10 characters.

**IMPORTANT**\
To activate the printf functionality for decimal numbers the linker arguments needs to be extended!

To extend the arguments open properties of the project:

![Project properties](/docs/image/as_project_properties.png "Atmel Studio project properties")

```
# General:
Use vprintf
```

![Enable vprintf](/docs/image/as_linker_general.png "Atmel Studio enable vprintf")

```
# Libraries:
# -> for printf floating point operations
libprintf_flt

# -> for scanf floating point operations
libscanf_flt
```

![Enable vprintf](/docs/image/as_linker_libraries.png "Atmel Studio enable vprintf")

```
# Miscellaneous:
# -> for printf floating point operations
-Wl,-u,vfprintf -lprintf_flt -lm

# -> for scanf floating point operations
-Wl,-u,vfscanf -lscanf_flt -lm

# -> for printf and scanf floating point operations
-Wl,-u,vfprintf -lprintf_flt -lm -Wl,-u,vfscanf -lscanf_flt -lm
```

![Enable printf](/docs/image/as_linker_misc.png "Atmel Studio enable printf with decimal numbers")

---

## [SPI Communication](/library/spi/)

The spi example program sends push-button status (S3:0, doubled) in loopback mode back to the LEDs (PORTC) of Megacard.

---

## [TWI Communication](/library/twi/)

The twi transmits (writes) data to an EEPROM (AT24C0x) and receives (reads) it back. Correct data will be signalised on LEDs (PORTC[7]) of Megacard.

---