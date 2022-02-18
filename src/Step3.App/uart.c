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

#include <math.h>
#include <string.h>
#include "uart.h"

static void (*uartRXpt) (uint8_t ch);

/*!
* @brief Initializes the uart0
* @param[in] baudrate       the baudrate to be used
* @param[in] ptf            Callback function that will receive any read character from uart0
*
*/
void uart0_init(uint32_t baudrate, void (*ptf) (uint8_t ch))
{
    uartRXpt = ptf;                         /*Assign the call back function into local one*/

    /* Define P1.1 and P1.2 as uart Rx/Tx*/
    P1SEL = BIT1 + BIT2 ;
    P1SEL2 = BIT1 + BIT2;

    /* Select SMCLK as the clock for uart module */
    UCA0CTL1 |= UCSSEL_2;

    /***********************************************************************/
    /* Define baudrate and uart mode as 8N 1 Stop no parity no flow */
    /* Check SLAU144J (User Guide) Setting a Baud Rate chapter for details */
    /***********************************************************************/

    /* integer part of baudrate */
    uint16_t n = round(1000000/baudrate);

    /* Set oversampling if required */
    if(UCA0BR0 > 16)
        UCA0MCTL |= UCOS16;

    /* Set baudrate settings */
    UCA0BR0 = n & 0x00ff;
    UCA0BR1 = (n >> 8) & 0x00ff;

    /* Set modulation settings */
    UCA0MCTL = UCBRS_1;

    /* Reset module */
    UCA0CTL1 &= ~UCSWRST;

    /* Enable interrupts */
    IE2 |= UCA0RXIE;

}

/*!
* @brief Transmits one byte
* @param[in] byte       the byte to be transmitted
*
*/
void uart0_tx_byte(uint8_t byte)
{
    /* load byte into TX register */
    UCA0TXBUF = byte;

    /* wait until data is Tx'd */
    while (!(IFG2&UCA0TXIFG));
}

/*!
* @brief Transmits a '0' terminated string
* @param[in] *string        The char pointer that contains the string
*
*/
void uart0_tx_string(char *string)
{
    unsigned int i;
    for (i = 0; i < strlen(string); i++)
    {
        uart0_tx_byte((uint8_t) string[i]);
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    /* Send Rx'd byte to handler */
    uartRXpt(UCA0RXBUF);

    /* Clear flag */
    UC0IFG &= ~(UCA0RXIFG);

    /* Wake uo from sleep */
    __bic_SR_register_on_exit(LPM0_bits);
}
