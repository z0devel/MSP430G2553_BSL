/** MIT License
 * Copyright (c) 2021, Z0 Devel
 * All rights reserved.
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 **/
#include <msp430.h>
#include "flash.h"

#define LED_RED_PORT          P1OUT
#define LED_RED_BIT           BIT6

#define RESET_Address           0xFFFE

int main(void)
{
    /* Stop watchdog timer */
    WDTCTL = WDTPW | WDTHOLD;

    /* Define LED_RED as an output */
    P1DIR |= LED_RED_BIT;

    /* Enable LED_RED */
    LED_RED_PORT |= LED_RED_BIT;

    /* Initializes flash module */
    flash_init();

    /* Defines an array where flash data will be stored */
    uint8_t data[64];


    while (1)
    {
        /* Wait some time while flashing LED */
        uint16_t counter = 0x1f;
        while (counter-- > 0)
        {
            __delay_cycles(100000L);
            LED_RED_PORT ^= LED_RED_BIT;
        }


        /* Read flash memory */
        flash_read_info(INFO_D, data);

        /* Check if first byte in INFO_D is 'a' */
        if (data[0] == 'a')
        {
            /* RESET */
            ((void (*)()) RESET_Address)();
        }
    }
}
