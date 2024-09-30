# STM32G0-bare-metal
This repo contains projects that we developed with the stm32g0 microcontroller.
## How to Use
- First, clone the project using `git clone https://github.com/UmutMErdem/STM32G0-bare-metal.git`
- Download and install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html). Select workspace on the root folder, then import existing projects to workspace. (File -> Import -> General -> Existing Projects into Workspace)
- Since the compiler and debugger come pre-installed, you only need to run it on STM32CubeIDE after making the necessary hardware connections by examining the desired project document in the lab reports.
## Contents
- `lab 1`
   * **Problem 2:** Contains assembly code to light up an LED.
   * **Problem 3:** Contains assembly code to light up 4 LEDs.
   * **Problem 4:** Contains assembly code to toggle a LED in roughly 1 second intervals.
- `lab 2`
   * **Problem 1:** Contains assembly code to toggle the on-board LED at a rate of 1 second.
   * **Problem 2:** Contains the assembly code to turn on the LED using the tach button.
   * **Problem 3:** Contains assembly code to toggle 8 LEDs at the same time at a rate of 1 second.
   * **Problem 4:** Contains assembly code to control 8 LEDs with shift pattern using the tach button.
   * **Problem 5:** Contains assembly code implementing a Knight Rider effect with 8 LEDs.
- `lab 3`
   * **Problem 1:** Contains register-level C code that implements a delay function using the SysTick timer.
   * **Problem 2:** Contains register-level C code that toggles the on-board LED using timer interrupts and changes the blinking speed using a tach button.
   * **Problem 3:** Contains register-level C code examples for driving a seven-segment display.
   * **Problem 4:** Contains register-level C code that implements of Independent Watchdog timer (IWDG).
- `lab 4`
   * **Problem 1:** Contains register-level C code examples for UART receive and transmit operations.
   * **Problem 2:** Contains register-level C code that implements a PWM signal and drive an external LED using varying duty cycles.
   * **Problem 3:** Contains register-level C code examples for interfacing with a keypad and combining with PWM and UART.
- `lab 5`
   * **Problem 1:** Contains register-level C code examples for using ADC to read potentiometer and control LEDs with PWM.
   * **Problem 2:** Contains register-level C code that implements of a knock counter using a sound sensor with a seven-segment display.
