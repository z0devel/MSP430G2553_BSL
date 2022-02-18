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
#include <uart.h>

#define LED_GREEN_PORT          P1OUT
#define LED_GREEN_BIT           BIT0

#define COUNTER_VAL             60000


volatile static uint8_t rx_char;
void uart0_rx_byte(uint8_t ch)
{
    rx_char = ch;
}

int main(void)
{
    /* Stop watchdog timer */
    WDTCTL = WDTPW | WDTHOLD;

    /* Define P1.0 as an output */
    P1DIR |= 0x01;

    /* Enable CCR0 interrupt */
    CCTL0 = CCIE;

    /* Define CCR0 value */
    CCR0 = COUNTER_VAL;

    /* Configure SMCLK, mode and divider for CCR0 */
    TACTL = TASSEL_2 + MC_2 + ID_3;

    /* Initialize UART */
    uart0_init(9600, uart0_rx_byte);

    /* Enable interrupts */
    __enable_interrupt();

    while(1)
    {
        /*
         * send console command
         */
        uart0_tx_string("Command menu \n");
        uart0_tx_string("  1) Led ON \n");
        uart0_tx_string("  2) Led OFF \n");
        uart0_tx_string("CMD>");

        __bis_SR_register(LPM0_bits + GIE);    // Enter LPM0, interrupts enabled

        /*
         *  setting LED on command
         */
        if (rx_char == '1')
        {
            __disable_interrupt();
            CCTL0 = CCIE;
            __enable_interrupt();

            uart0_tx_string("Turning LED ON - OK!\n");
        }
        /*
         *  setting LED off command
         */
        else if (rx_char == '2')
        {
            __disable_interrupt();
            CCTL0 = 0;
            /* make sure the led goes off when disabling it */
            LED_GREEN_PORT &= ~LED_GREEN_BIT;
            __enable_interrupt();


            uart0_tx_string("Turning LED OFF - OK!\n");
        }
        /**
         * Invalid
         */
        else
        {
            uart0_tx_string("Invalid option\n");
        }
    }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    LED_GREEN_PORT ^= LED_GREEN_BIT;
    CCR0 += COUNTER_VAL;
}
