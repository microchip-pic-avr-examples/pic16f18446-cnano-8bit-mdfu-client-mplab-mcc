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
 * @defgroup bl_memory Memory Helper
 * @brief   This file contains prototypes and data types for the helper functions used with the Non-Volatile Memory (NVM) peripheral driver
 *
 */
#ifndef BL_MEMORY_H
/* cppcheck-suppress misra-c2012-2.5 */
#define BL_MEMORY_H

#include <stdint.h>
#include <xc.h>
#include "../../../nvm/nvm.h"
#include "bl_config.h"

/**
 * @ingroup bl_memory
 * @struct key_structure_t
 * @brief Contains variables for the keys received by the user from the bootloader memory extension APIs.
 * @var key_structure_t:: eraseUnlockKey
 * Member 'eraseUnlockKey' contains the unlock key needed to erase a page of memory.
 * @var key_structure_t:: readUnlockKey
 * Member 'readUnlockKey' contains the unlock key needed to read a page of memory.
 * @var key_structure_t:: byteWordWriteUnlockKey
 * Member 'byteWordWriteUnlockKey' contains the unlock key needed to write a word/byte to memory.
 * @var key_structure_t:: rowWriteUnlockKey
 * Member 'rowWriteUnlockKey' contains the unlock key needed to write a row to memory.
 */
typedef struct
{
    uint16_t eraseUnlockKey;
    uint16_t readUnlockKey;
    uint16_t byteWordWriteUnlockKey;
    uint16_t rowWriteUnlockKey;
} key_structure_t;

/**
 * @ingroup bl_memory
 * @enum bl_mem_result_t
 * @brief Contains codes for the return values of the bootloader memory extension APIs.
 * @var bl_mem_result_t:: BL_MEM_PASS
 * 0x00 - NVM operation succeeded
 * @var bl_mem_result_t:: BL_MEM_FAIL
 * 0x01 -  NVM operation failed
 * @var bl_mem_result_t:: BL_MEM_INVALID_ARG
 * 0x02 -  NVM operation failed due to invalid argument
 *
 */
typedef enum
{
    BL_MEM_PASS, 
    BL_MEM_FAIL, 
    BL_MEM_INVALID_ARG, 
} bl_mem_result_t;

/**
 * @ingroup bl_memory
 * @brief Wrapper to the NVM Set Key function which assigns user-defined key values for write operations in NVM.
 * @param [in] keys - Structure containing values of user-defined read, write and erase keys
 * @return None.
 */
void BL_MemoryUnlockKeysInit(key_structure_t keys);

/**
 * @ingroup bl_memory
 * @brief Clears all unlock keys once the bootloader core firmware completes the memory operations requiring them.
 * @return None.
 * @param None.
 */
void BL_MemoryUnlockKeysClear(void);

#if defined (BL_EEPROM_READ_ENABLED)
/**
 * @ingroup bl_memory
 * @brief Wrapper to the NVM Read EEPROM function which reads multiple bytes from the desired address in EEPROM.
 * @param [in] address - Starting address for the EEPROM read operation
 * @param [in] buffer - Storage buffer for the read EEPROM bytes 
 * @param [in] length - Total number of bytes to be read from EEPROM
 * @return @ref BL_MEM_PASS - EEPROM read succeeded \n
 * @return @ref BL_MEM_FAIL - EEPROM read failed \n
 * @return @ref BL_MEM_INVALID_ARG - An invalid argument is passed to the function \n
 */
bl_mem_result_t BL_EEPROMRead(eeprom_address_t address, eeprom_data_t * buffer, size_t length);
#endif

#if defined (BL_EEPROM_WRITE_ENABLED)
/**
 * @ingroup bl_memory
 * @brief Wrapper to the NVM Write EEPROM function which writes multiple bytes to the desired address in EEPROM.
 * @param [in] address - Starting address for the EEPROM write operation
 * @param [in] buffer - Storage buffer for the EEPROM bytes to be written
 * @param [in] length - Total number of bytes to be written to EEPROM
 * @return @ref BL_MEM_PASS - EEPROM write succeeded \n
 * @return @ref BL_MEM_FAIL - EEPROM write failed \n
 * @return @ref BL_MEM_INVALID_ARG - An invalid argument is passed to the function \n
 */
bl_mem_result_t BL_EEPROMWrite(eeprom_address_t address, eeprom_data_t * buffer, size_t length);
#endif

/**
 * @ingroup bl_memory
 * @brief Wrapper to the NVM Read Flash function which reads multiple bytes from the desired address in program Flash.
 * @param [in] address - Starting address for the Flash read operation
 * @param [in] buffer - Storage buffer for the read Flash bytes
 * @param [in] length - Total number of bytes to be read from Flash
 * @return @ref BL_MEM_PASS - Flash read succeeded \n
 * @return @ref BL_MEM_FAIL - Flash read failed \n
 * @return @ref BL_MEM_INVALID_ARG - An invalid argument is passed to the function \n
 */
bl_mem_result_t BL_FlashRead(flash_address_t address, flash_data_t * buffer, size_t length);


/**
 * @ingroup bl_memory
 * @brief Wrapper to the NVM Write Flash function which writes multiple bytes to the desired address in program Flash.
 * @param [in] address - Starting address for the Flash write operation
 * @param [in] buffer - Storage buffer for the Flash bytes to be written
 * @param [in] length - Total number of bytes to be written to Flash
 * @return @ref BL_MEM_PASS - Flash write succeeded \n
 * @return @ref BL_MEM_FAIL - Flash write failed \n
 * @return @ref BL_MEM_INVALID_ARG - An invalid argument is passed to the function \n
 */
bl_mem_result_t BL_FlashWrite(flash_address_t address, flash_data_t * buffer, size_t length);

#endif //BL_MEMORY_H