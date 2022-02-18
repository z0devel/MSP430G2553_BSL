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
#ifndef FLASH_H_
#define FLASH_H_

#include <msp430.h>
#include <stdint.h>

#define INFO_A             ((uint8_t *)  0x10C0)
#define INFO_B             ((uint8_t *)  0x1080)
#define INFO_C             ((uint8_t *)  0x1040)
#define INFO_D             ((uint8_t *)  0x1000)

#define INVALID_SEGMENT     0x01
#define INVALID_DATA        0x02
#define FLASH_READ_OK       0x00
#define FLASH_WRITE_OK      0x00

/**
 * Public API functions provided by the flash library
 */
void flash_init();
uint8_t flash_erase_info(uint8_t *flash_addr);
uint8_t flash_read_info(uint8_t *flash_addr, uint8_t *data);
uint8_t flash_write_info(uint8_t* flash_addr, uint8_t *data);


#endif /* UART_H_ */
