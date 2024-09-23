/**
 * © 2024 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms
 * applicable to your use of third party software (including open
 * source software) that may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL,
 * PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED,
 * EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE
 * DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW,
 * MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO
 * THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU
 * HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * @file    bl_memory.h
 * @ingroup bl_memory
 * @brief   This file contains implementation for the helper functions used with the NVM peripheral driver
 *
 */

#include "bl_memory.h"

/**@misradeviation{@advisory, 17.8} - The parameter is passed to this wrapper API to decrement and
 * write or erase bytes/words from consecutive memory locations until its value is zero. 
 * Assigning the value to a different local variable for each API will increase redundancy.
 */
/**@misradeviation{@advisory, 8.9} The static buffer will not be declared at local scope
 * due to portability concerns between various architectures.
 */

static uint16_t erasePageKey;
static uint16_t readKey;
static uint16_t byteWordWriteKey;
static uint16_t rowWriteKey;

void BL_MemoryUnlockKeysInit(key_structure_t keys)
{
    erasePageKey = keys.eraseUnlockKey;
    readKey = keys.readUnlockKey;
    rowWriteKey = keys.rowWriteUnlockKey;
    byteWordWriteKey = keys.byteWordWriteUnlockKey;
}

void BL_MemoryUnlockKeysClear(void)
{
    erasePageKey = 0;
    readKey = 0;
    rowWriteKey = 0;
    byteWordWriteKey = 0;
}

#if defined (BL_EEPROM_READ_ENABLED)

bl_mem_result_t BL_EEPROMRead(eeprom_address_t address, eeprom_data_t * buffer, size_t length)
{
    bl_mem_result_t result = BL_MEM_FAIL;
    NVM_StatusClear();
    if (buffer == NULL) //Check the valid buffer
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if ((length <= (size_t) 0) || ((address + length) > (EEPROM_START_ADDRESS + EEPROM_SIZE))) //Check valid length
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if ((address < EEPROM_START_ADDRESS) || (address > (EEPROM_START_ADDRESS + EEPROM_SIZE)))//Check valid address
    {
        result = BL_MEM_INVALID_ARG;
    }
    else
    {
        for (uint16_t index = 0; index < length; index++)
        {
            buffer[index] = EEPROM_Read(address);

            /* cppcheck-suppress misra-c2012-17.8 */
            address++;
            while (NVM_IsBusy())
            {

            }
            if (NVM_OK == NVM_StatusGet())
            {
                result = BL_MEM_PASS;
            }
            else
            {
                result = BL_MEM_FAIL;
                break;
            }
        }
    }
    return result;
}
#endif

#if defined (BL_EEPROM_WRITE_ENABLED)
bl_mem_result_t BL_EEPROMWrite(eeprom_address_t address, eeprom_data_t * buffer, size_t length)
{
    bl_mem_result_t result = BL_MEM_FAIL;
    NVM_StatusClear();
    if (buffer == NULL)//Check the valid buffer
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if ((length <= (size_t) 0) || ((address + length) > (EEPROM_START_ADDRESS + EEPROM_SIZE)))//Check valid length
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if ((address < EEPROM_START_ADDRESS) || (address > (EEPROM_START_ADDRESS + EEPROM_SIZE)))//Check valid address
    {
        result = BL_MEM_INVALID_ARG;
    }
    else
    {
        for (uint16_t index = 0; index < length; index++)
        {
#if defined (_18FXXQ10_FAMILY_)
            NVM_UnlockKeySet(byteWordWriteKey);
#else
            NVM_UnlockKeySet(byteWordWriteKey);
#endif
            EEPROM_Write(address, buffer[index]);
            NVM_UnlockKeyClear();
            /* cppcheck-suppress misra-c2012-17.8 */
            address++;
            while (NVM_IsBusy())
            {

            } //Wait while NVM is busy
            if (NVM_OK == NVM_StatusGet())
            {
                result = BL_MEM_PASS;
            } //If NVM_ERROR break
            else
            {
                result = BL_MEM_FAIL;
                break;
            }
        }
    }
    return result;
}
#endif

bl_mem_result_t BL_FlashRead(flash_address_t address, flash_data_t * buffer, size_t length)
{
    bl_mem_result_t result = BL_MEM_FAIL;
    NVM_StatusClear();
    if (buffer == NULL)//Check the valid buffer
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if (address > PROGMEM_SIZE) //Check the valid address
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if ((length <= (size_t) 0) || ((address + length) > (PROGMEM_SIZE + (uint24_t) 1)))//Check valid length
    {
        result = BL_MEM_INVALID_ARG;
    }
    else
    {
        for (uint16_t index = 0; index <= length; index++)
        {
#if defined (_18FXXQ10_FAMILY_)
            NVM_UnlockKeySet(readKey);
#else
            NVM_UnlockKeySet(readKey);
#endif
            buffer[index] = FLASH_Read(address);

            /* cppcheck-suppress misra-c2012-17.8 */
            address++;
            NVM_UnlockKeyClear();
            while (NVM_IsBusy())
            {

            }
            if (NVM_OK == NVM_StatusGet())
            {
                result = BL_MEM_PASS;
            }
            else
            {
                result = BL_MEM_FAIL;
                break;
            }
        }
    }
    return result;
}

// Static Buffer Declared to Assist in writing blocks of any length upto 1 page
/* cppcheck-suppress misra-c2012-8.9 */
static flash_data_t writeBuffer[PROGMEM_PAGE_SIZE];

bl_mem_result_t BL_FlashWrite(flash_address_t address, flash_data_t * buffer, size_t length) // using precompiled directives for differences in PIC18 and PIC16 implementation
{

    bl_mem_result_t result = BL_MEM_FAIL;
    uint16_t offset;
    uint16_t index;
    uint16_t counter = 0;
    flash_address_t pageStartAddress;

    if (buffer == NULL)//Check the valid buffer
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if (address > PROGMEM_SIZE)//Check valid address
    {
        result = BL_MEM_INVALID_ARG;
    }
    else if ((length <= (size_t) 0) || ((address + length) > (PROGMEM_SIZE + (uint24_t) 1)))//Check valid length
    {
        result = BL_MEM_INVALID_ARG;
    }

    else
    {
        NVM_StatusClear();
        if (length < PROGMEM_PAGE_SIZE) //Check if length is less that program memory page size
        {
            offset = FLASH_PageOffsetGet(address); //Get offset from start of the page consisting the address
            pageStartAddress = FLASH_PageAddressGet(address); //Get start address of the page with the address

            //Read entire page from FLASH
            for (index = 0; index < PROGMEM_PAGE_SIZE; index++)
            {
                writeBuffer[index] = FLASH_Read(pageStartAddress); //Write byte read from flash in buffer at index
                pageStartAddress++;
                while (NVM_IsBusy())
                {

                }
                if (NVM_OK == NVM_StatusGet())
                {
                    result = BL_MEM_PASS;
                }
                else
                {
                    result = BL_MEM_FAIL;
                    break;
                }
            }
            if (result != BL_MEM_FAIL)
            {
                //Append buffer elements at/from offset to the read page buffer
                while (length != (size_t) 0)
                {
                    writeBuffer[offset] = buffer[counter]; //Start writing offset onwards
                    offset++;
                    counter++;
                    /* cppcheck-suppress misra-c2012-17.8 */
                    length--;
                }
                pageStartAddress = FLASH_PageAddressGet(address); //Get start address of the page with the address

#if defined (_18FXXQ10_FAMILY_)
                NVM_UnlockKeySet(erasePageKey);
#else
                NVM_UnlockKeySet(erasePageKey);
#endif
                result = (bl_mem_result_t) FLASH_PageErase(pageStartAddress); //Erase page in flash
                NVM_UnlockKeyClear(); //Clear unlock key
                if ((bl_mem_result_t) NVM_OK == result)
                {
                    //Write data to Flash row
#if defined (_18FXXQ10_FAMILY_)
                    NVM_UnlockKeySet(rowWriteKey);
#else
                    NVM_UnlockKeySet(rowWriteKey);
#endif
                    result = (bl_mem_result_t) FLASH_RowWrite(pageStartAddress, writeBuffer);
                    NVM_UnlockKeyClear();
                }

                if ((bl_mem_result_t) NVM_OK == result)
                {
                    result = BL_MEM_PASS;
                }
                else
                {
                    result = BL_MEM_FAIL;
                }
            }
        }
        if (length == PROGMEM_PAGE_SIZE)
        {
            pageStartAddress = FLASH_PageAddressGet(address); //Get start address of the page with the address

#if defined (_18FXXQ10_FAMILY_)
            NVM_UnlockKeySet(erasePageKey);
#else
            NVM_UnlockKeySet(erasePageKey);
#endif
            result = (bl_mem_result_t) FLASH_PageErase(pageStartAddress); //Erase page in flash
            NVM_UnlockKeyClear();
            if ((bl_mem_result_t) NVM_OK == result)
            {
                //Write data to Flash row
#if defined (_18FXXQ10_FAMILY_)
                NVM_UnlockKeySet(rowWriteKey);
#else
                NVM_UnlockKeySet(rowWriteKey);
#endif
                result = (bl_mem_result_t)FLASH_RowWrite(pageStartAddress,buffer);
                NVM_UnlockKeyClear();
            }
            else
            {
                result = BL_MEM_FAIL;
            }

            if ((bl_mem_result_t) NVM_OK == result)
            {
                result = BL_MEM_PASS;
            }
            else
            {
                result = BL_MEM_FAIL;
            }
        }
    }
    return result;
}
