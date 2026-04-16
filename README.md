# RootMixer 

A modular, bare-metal USB MIDI controller designed from scratch. Built around the STM32F411 (ARM Cortex-M4) using pure C CMSIS registers, featuring an expandable I2C and Analog Multiplexing architecture.

##  Features
* **Bare-Metal Firmware:** Zero heavy libraries (No HAL, No Arduino). Direct register manipulation for ultra-low latency (<2ms scan rate).
* **Hardware Multiplexing:** Daisy-chained `CD74HC4067` multiplexers allowing up to 16 analog channels per ADC pin.
* **Modular Design:** 1 Motherboard supports up to 4 swappable Channel Strips.
* **Native USB MIDI:** Implemented custom USB Device stack to interface natively with DAWs (Ableton, FL Studio) without drivers.
* **Real-time OSless Scheduler:** Custom `SysTick` non-blocking architecture.

##  Project Structure
* `/firmware`: Pure C source code, Makefile, and linker scripts.
* `/hardware`: KiCad 7 projects, Schematics (PDF), and Gerber files ready for JLCPCB.
    * `/Motherboard`: Main STM32 brain, LDO power supply, and Expansion ports.
    * `/Daughterboard_Channel`: The swappable channel strip (1x SC6021N Fader, 4x WH148 Pots, Mute Button).

##  Build & Flash (Firmware)
Requires `arm-none-eabi-gcc`, `make`, and `openocd`.

```bash
cd firmware
make clean
make
make flash
```

## License
This project is licensed under the MIT License - see the LICENSE file for details.
