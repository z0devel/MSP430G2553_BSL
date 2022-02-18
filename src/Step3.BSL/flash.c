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
#include <stdint.h>
#include "flash.h"

/*!
* @brief Initializes the flash module
*/
void flash_init()
{
    FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
}

/*!
* @brief Erases a complete flash INFO segment of 64 bytes
* @param[in] *flash_addr            Address of the flash INFO segment
*
* @return   INVALID_SEGMENT           If *flash_addr does not point to a valid INFO segment
*           FLASH_OK                  If the operation finishes successfully
*
*/
uint8_t flash_erase_info(uint8_t *flash_addr)
{

    if (flash_addr != INFO_A && flash_addr != INFO_B && flash_addr != INFO_C
            && flash_addr != INFO_D)
    {
        return (INVALID_SEGMENT);
    }

    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *flash_addr = 0;                       // Dummy write to erase Flash segment

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit

    return (FLASH_OK);
}

/*!
* @brief Reads a complete flash INFO segment of 64 bytes
* @param[in] *flash_addr            Address of the flash INFO segment
* @param[in] *data                  Address where data will be stored
*
* @return   INVALID_SEGMENT           If *flash_addr does not point to a valid INFO segment
*           INVALID_DATA              If *data is not a valid pointer
*           FLASH_OK                  If the operation finishes successfully
*
*/
uint8_t flash_read_info(uint8_t *flash_addr, uint8_t *data)
{

    if (flash_addr != INFO_A && flash_addr != INFO_B && flash_addr != INFO_C
            && flash_addr != INFO_D)
    {
        return (INVALID_SEGMENT);
    }

    if (!data)
    {
        return (INVALID_DATA);
    }

    unsigned int i;
    FCTL3 = FWKEY;                            // Clear Lock bit
    for (i = 0; i < 64; i++)
    {
        *data++ = *flash_addr++;
    }

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit

    return (FLASH_OK);
}

/*!
* @brief Writes completely a flash INFO segment of 64 bytes
* @param[in] *flash_addr            Address of the flash INFO segment
* @param[in] *data                  Address where data will be fetched
*
* @return   INVALID_SEGMENT           If *flash_addr does not point to a valid INFO segment
*           INVALID_DATA              If *data is not a valid pointer
*           FLASH_OK                  If the operation finishes successfully
*
*/
uint8_t flash_write_info(uint8_t* flash_addr, uint8_t *data)
{
    if (flash_addr != INFO_A && flash_addr != INFO_B && flash_addr != INFO_C
            && flash_addr != INFO_D)
    {
        return (INVALID_SEGMENT);
    }

    if (!data)
    {
        return (INVALID_DATA);
    }

    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *flash_addr = 0;                       // Dummy write to erase Flash segment

    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    unsigned int i;
    for (i = 0; i < 64; i++)
    {
        *flash_addr++ = *data++;                   // Write value to flash
    }

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit

    return (FLASH_OK);
}
