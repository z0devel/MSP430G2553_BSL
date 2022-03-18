# PART 1



# Step (I)


## App code

### Goals

Create a regular application
* It uses uart0  and acts as a sort of simple server, accepting a limited command set
  * Enables the Green LED flashing
  * Disables the Green LED flashing
* Make sure it works as expected.

### Process

* Import project Step1.App
* Build and execute 

Open your preferred Serial com application with:
* 9600 bauds
* 8 Data bits
* 1 Stop bits
* No Parity
* No Flow control

What follows is a capture of some commands sent to Step1.App

![part1_1.png](/doc/part1_1.png)

### Results

The green led flashes by default.

Through the serial interface it is possible to  disable the LED, and enable it back.

## BSL code

### Goals

* Create a regular application, that will become the custom our BSL. At this point is just a simple application that runs from the default Flash location
  * It just flashed red LED
* Make sure it works as expected.


### Process

* Import project Step1.BSL
* Build and execute the project


### Results

The red LED flashes quickly.

Note that at this point, App and BSL applications override each other, they are written into the same Flash location (0xC000)



# Step (II)

In this second stage we will be doing the following changes:
1. Change the linker command of App to store the code in a different location: 0xD000
2. Change CCS settings so we prevent overriding the memory area where the BSL code is.
3. Change App code to add a new command to jump to 0xC000 address, where BSL application is.
4. Define a specific flashing process to avoid one flash operation to override the written data by the other process.
5. Verify we can Jump from App to BSL (later on we will follow the opposite path, at this point this is convenient to easily debug App code from within eclipse)


Regarding point 4, it seems the best approach would be:
1. Create the two binary (ELF) files
2. Merge them to TI-TXT using hex430
3. Flash the resulting into the MCU

However, we will use an alternative way:
1. Flash BSL code
2. Flash App code with the changes in the linker command and CCS


## App code

### Goals

* Change linker command file
* Change CCS settings
* Add new command

### Process

* Import project Step2.App. Have a look at the changes in code
* Change changes in linker command
  * FLASH area is split in two sections
    * BSL_FLASH: to store the BSL application
    * APP_FLASH: to store the App application
  * SECTIONS is updated to use the APP_FLASH

**Changes in bold**

**THIS IS NOT THE  COMPLETE FILE!!**

    MEMORY
    {
    SFR                     : origin = 0x0000, length = 0x0010
    PERIPHERALS_8BIT        : origin = 0x0010, length = 0x00F0
    PERIPHERALS_16BIT       : origin = 0x0100, length = 0x0100
    RAM                     : origin = 0x0200, length = 0x0200
    INFOA                   : origin = 0x10C0, length = 0x0040
    INFOB                   : origin = 0x1080, length = 0x0040
    INFOC                   : origin = 0x1040, length = 0x0040
    INFOD                   : origin = 0x1000, length = 0x0040
    **BSL_FLASH               : origin = 0xC000, length = 0x1000**
    **APP_FLASH               : origin = 0xD000, length = 0x2FDE**
    BSLSIGNATURE            : origin = 0xFFDE, length = 0x0002, fill = 0xFFFF
    INT00                   : origin = 0xFFE0, length = 0x0002
    INT01                   : origin = 0xFFE2, length = 0x0002
    INT02                   : origin = 0xFFE4, length = 0x0002
    INT03                   : origin = 0xFFE6, length = 0x0002
    INT04                   : origin = 0xFFE8, length = 0x0002
    INT05                   : origin = 0xFFEA, length = 0x0002
    INT06                   : origin = 0xFFEC, length = 0x0002
    INT07                   : origin = 0xFFEE, length = 0x0002
    INT08                   : origin = 0xFFF0, length = 0x0002
    INT09                   : origin = 0xFFF2, length = 0x0002
    INT10                   : origin = 0xFFF4, length = 0x0002
    INT11                   : origin = 0xFFF6, length = 0x0002
    INT12                   : origin = 0xFFF8, length = 0x0002
    INT13                   : origin = 0xFFFA, length = 0x0002
    INT14                   : origin = 0xFFFC, length = 0x0002
    RESET                   : origin = 0xFFFE, length = 0x0002
    }
    SECTIONS
    {
    .bss        : {} > RAM                  /* Global & static vars              */
    .data       : {} > RAM                  /* Global & static vars              */
    .TI.noinit  : {} > RAM                  /* For #pragma noinit                */
    .sysmem     : {} > RAM                  /* Dynamic memory allocation area    */
    .stack      : {} > RAM (HIGH)           /* Software system stack             */
    .text       : {} > **APP_FLASH**                /* Code                              */
    .cinit      : {} > **APP_FLASH**                /* Initialization tables             */
    .const      : {} > **APP_FLASH**                /* Constant data                     */
    .bslsignature  : {} > BSLSIGNATURE      /* BSL Signature                     */
    .cio        : {} > RAM                  /* C I/O Buffer                      */

    .pinit      : {} > **APP_FLASH**                /* C++ Constructor tables            */
    .binit      : {} > **APP_FLASH**                /* Boot-time Initialization tables   */
    .init_array : {} > **APP_FLASH**                /* C++ Constructor tables            */
    .mspabi.exidx : {} > **APP_FLASH**              /* C++ Constructor tables            */
    .mspabi.extab : {} > **APP_FLASH**              /* C++ Constructor tables            */
    #ifdef __TI_COMPILER_VERSION__
      #if __TI_COMPILER_VERSION__ >= 15009000
        #ifndef __LARGE_CODE_MODEL__
        .TI.ramfunc : {} load=**APP_FLASH**, run=RAM, table(BINIT)
        #else
        .TI.ramfunc : {} load=**APP_FLASH** | **APP_FLASH2**, run=RAM, table(BINIT)
        #endif
      #endif
    #endif

    .infoA     : {} > INFOA              /* MSP430 INFO FLASH Memory segments */
    .infoB     : {} > INFOB
    .infoC     : {} > INFOC
    .infoD     : {} > INFOD

    /* MSP430 Interrupt vectors          */
    TRAPINT      : { * ( .int00 ) } > INT00 type = VECT_INIT
    .int01       : {}               > INT01
    PORT1        : { * ( .int02 ) } > INT02 type = VECT_INIT
    PORT2        : { * ( .int03 ) } > INT03 type = VECT_INIT
    .int04       : {}               > INT04
    ADC10        : { * ( .int05 ) } > INT05 type = VECT_INIT
    USCIAB0TX    : { * ( .int06 ) } > INT06 type = VECT_INIT
    USCIAB0RX    : { * ( .int07 ) } > INT07 type = VECT_INIT
    TIMER0_A1    : { * ( .int08 ) } > INT08 type = VECT_INIT
    TIMER0_A0    : { * ( .int09 ) } > INT09 type = VECT_INIT
    WDT          : { * ( .int10 ) } > INT10 type = VECT_INIT
    COMPARATORA   : { * ( .int11 ) } > INT11 type = VECT_INIT
    TIMER1_A1    : { * ( .int12 ) } > INT12 type = VECT_INIT
    TIMER1_A0    : { * ( .int13 ) } > INT13 type = VECT_INIT
    NMI          : { * ( .int14 ) } > INT14 type = VECT_INIT
    .reset       : {}               > RESET  /* MSP430 Reset vector         */
    }

* Change CCS settings

In order to guarantee flashing App code does not erases BSL cose at 0xC000, change the following

![part1_2.png](/doc/part1_2.png)


## BSL code

There isn't any change in the BSL code


## Results

To see the effect of the changes, follow the steps:
* Execute the BSL applications, this removes all the Memory and stores the BSL as a complete application
* Execute (debug) the App application. The changes apply keep the BSL application at 0xC000 code
* Connect through the serial interface and make sure the command set is now expanded


![part1_3.png](/doc/part1_3.png)

* Add a breakpoint just before the jump

![part1_4.png](/doc/part1_4.png)

* At the serial interface select command 3 (jump)
* At CCS open the registers and Disassembly views
   * The PC register is logically in the 0xDxxx area
   * The Disassembly shows clearly the Jump we are about to do


![part1_5.png](/doc/part1_5.png)

* Step in, the debugging ability is lost. The red LED flashes, and that means we are executing BSL code
* Pause, or suspend, the execution

![part1_6.png](/doc/part1_6.png)

* The PC and Disassembly views show code being executed around 0xC000. This is the confirmation BSL is being executed.

![part1_7.png](/doc/part1_7.png)

In case you power cycle the board you will notice App is the executed code. This is the expected outcome, as App was flashed as the second stage flashing, and that means the App is flashed as the main application on the memory.

# Step (III)

## Overview

After this step we will have a conditional custom BSL code, that jumps to the main application only if certain criteria is met.

The criteria will be whether some content from INFO flash memory has a certain value. We will use INFO_D segment as sort of shared memory between the application App and the BSL code.

Note, that in order not to lose data in INFO_D we will be reading all the segment as a previous step to writing it. As any write requires a complete segment erase. That would also be an interesting exercise with flash memory INFO segments.

In this third stage we will be doing the following changes:
* Change the main application App
  * We will add an option to read INFO_D segment from the flash
  * We will add an option to write the first byte in INFO_D segment.
* Change the BSL code
  * Change the Linked command, so that we can easily debug the BSL and see how the change to App code happens.
  * Check periodically the contents of INFO_D segment
  * In case the first byte is the character 'a', do a reset, by jumping to the address of the RESET interrupt.


## App code

Import project Step3.App. Have a look at the changes in code
* flash.h and flash.c files
* New options in the application menu

The image below show how the new options work

![part1_8.png](/doc/part1_8.png)

## BSL code

Import project Step3.BSL. Have a look at the changes in code
* The same flash.h and flash.c files have been added
* Periodically, the BSL checks for byte0 in INFO_D segment. In case the content equals 'a' a system RESET is performed, by jumping to the address of the RESET interrupt
The BSL linker command file has been changed to account for the space used by the App code.
Also the CCS Flash options have been adjusted, so that it is possible to:
* Flash the App code
* Flash de BSL code
* Debug the BSL code in C, instead of Assembler.

Changes in linker command file

**Changes in bold**

**THIS IS NOT THE  COMPLETE FILE!!**

    MEMORY
    {
    SFR                     : origin = 0x0000, length = 0x0010
    PERIPHERALS_8BIT        : origin = 0x0010, length = 0x00F0
    PERIPHERALS_16BIT       : origin = 0x0100, length = 0x0100
    RAM                     : origin = 0x0200, length = 0x0200
    INFOA                   : origin = 0x10C0, length = 0x0040
    INFOB                   : origin = 0x1080, length = 0x0040
    INFOC                   : origin = 0x1040, length = 0x0040
    INFOD                   : origin = 0x1000, length = 0x0040
    BSL_FLASH		    : origin = 0xC000, length = 0x1000
    APP_FLASH               : origin = 0xD000, length = 0x2FDE
    BSLSIGNATURE            : origin = 0xFFDE, length = 0x0002, fill = 0xFFFF
    INT00                   : origin = 0xFFE0, length = 0x0002
    INT01                   : origin = 0xFFE2, length = 0x0002
    INT02                   : origin = 0xFFE4, length = 0x0002
    INT03                   : origin = 0xFFE6, length = 0x0002
    INT04                   : origin = 0xFFE8, length = 0x0002
    INT05                   : origin = 0xFFEA, length = 0x0002
    INT06                   : origin = 0xFFEC, length = 0x0002
    INT07                   : origin = 0xFFEE, length = 0x0002
    INT08                   : origin = 0xFFF0, length = 0x0002
    INT09                   : origin = 0xFFF2, length = 0x0002
    INT10                   : origin = 0xFFF4, length = 0x0002
    INT11                   : origin = 0xFFF6, length = 0x0002
    INT12                   : origin = 0xFFF8, length = 0x0002
    INT13                   : origin = 0xFFFA, length = 0x0002
    INT14                   : origin = 0xFFFC, length = 0x0002
    RESET                   : origin = 0xFFFE, length = 0x0002
    }

    /****************************************************************************/
    /* Specify the sections allocation into memory                              */
    /****************************************************************************/

    SECTIONS
    {
    .bss        : {} > RAM                  /* Global & static vars              */
    .data       : {} > RAM                  /* Global & static vars              */
    .TI.noinit  : {} > RAM                  /* For #pragma noinit                */
    .sysmem     : {} > RAM                  /* Dynamic memory allocation area    */
    .stack      : {} > RAM (HIGH)           /* Software system stack             */

    .text       : {} > BSL_FLASH                /* Code                              */
    .cinit      : {} > BSL_FLASH                /* Initialization tables             */
    .const      : {} > BSL_FLASH                /* Constant data                     */
    .bslsignature  : {} > BSLSIGNATURE      /* BSL Signature                     */
    .cio        : {} > RAM                  /* C I/O Buffer                      */

    .pinit      : {} > BSL_FLASH                /* C++ Constructor tables            */
    .binit      : {} > BSL_FLASH                /* Boot-time Initialization tables   */
    .init_array : {} > BSL_FLASH                /* C++ Constructor tables            */
    .mspabi.exidx : {} > BSL_FLASH              /* C++ Constructor tables            */
    .mspabi.extab : {} > BSL_FLASH              /* C++ Constructor tables            */
    #ifdef __TI_COMPILER_VERSION__
      #if __TI_COMPILER_VERSION__ >= 15009000
        #ifndef __LARGE_CODE_MODEL__
        .TI.ramfunc : {} load=BSL_FLASH, run=RAM, table(BINIT)
        #else
        .TI.ramfunc : {} load=BSL_FLASH | BSL_FLASH2, run=RAM, table(BINIT)
        #endif
      #endif
    #endif

    .infoA     : {} > INFOA              /* MSP430 INFO FLASH Memory segments */
    .infoB     : {} > INFOB
    .infoC     : {} > INFOC
    .infoD     : {} > INFOD

    /* MSP430 Interrupt vectors          */
    TRAPINT      : { * ( .int00 ) } > INT00 type = VECT_INIT
    .int01       : {}               > INT01
    PORT1        : { * ( .int02 ) } > INT02 type = VECT_INIT
    PORT2        : { * ( .int03 ) } > INT03 type = VECT_INIT
    .int04       : {}               > INT04
    ADC10        : { * ( .int05 ) } > INT05 type = VECT_INIT
    USCIAB0TX    : { * ( .int06 ) } > INT06 type = VECT_INIT
    USCIAB0RX    : { * ( .int07 ) } > INT07 type = VECT_INIT
    TIMER0_A1    : { * ( .int08 ) } > INT08 type = VECT_INIT
    TIMER0_A0    : { * ( .int09 ) } > INT09 type = VECT_INIT
    WDT          : { * ( .int10 ) } > INT10 type = VECT_INIT
    COMPARATORA   : { * ( .int11 ) } > INT11 type = VECT_INIT
    TIMER1_A1    : { * ( .int12 ) } > INT12 type = VECT_INIT
    TIMER1_A0    : { * ( .int13 ) } > INT13 type = VECT_INIT
    NMI          : { * ( .int14 ) } > INT14 type = VECT_INIT
    .reset       : {}               > RESET  /* MSP430 Reset vector         */
    }

* CCS settings
![part1_9.png](/doc/part1_9.png)

## Results

To see the effect of the changes, follow the steps:
* Execute the BSL application.
* Execute (debug) the App application. 
* Connect through the serial interface and make sure the command set is now expanded.

The green LED flashes, App code is running. Select erase flash and jump to BSL 

![part1_10.png](/doc/part1_10.png)

The red LED flashes non-stop.

As the INFO_D byte0 has been erased, the BSL failed to validate the boot criteria and execution stay in BSL code


Restart the board and make sure the green LED flashes again. This time write 'a' into INFO_D and jump to BSL code

![part1_11.png](/doc/part1_11.png)

The red LED flashes for some seconds, after INFO_D byte0 is validated as 'a' the execution jumps again to App code, the green LED flashes back.
