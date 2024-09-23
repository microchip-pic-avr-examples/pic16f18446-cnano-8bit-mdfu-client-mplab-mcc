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
 * @file        bl_core.h
 * @defgroup    mdfu_client_8bit 8-bit MDFU Client Library
 * 
 * @brief       This file contains API prototypes to
 *              perform bootloader operations.
 */

#ifndef BL_CORE_H
#define BL_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "bl_result_type.h"
#include "bl_config.h"
#include "../../../system/pins.h"
#include "../../../nvm/nvm.h"

/**
 * @ingroup mdfu_client_8bit
 * @enum bl_block_type_t
 * @brief Contains codes corresponding to the various types
 * of data blocks that the bootloader supports.
 * @var bl_block_type_t:: UNLOCK_BOOTLOADER
 * 0x01U - Unlock Bootloader Block - Identifies an
 * operational block that holds precondition metadata to be checked and validated before
 * any memory-changing actions occur in the bootloader
 * @var bl_block_type_t:: WRITE_FLASH
 * 0x02U - Flash Data Block - Identifies operational blocks
 * that need to be written into the Flash section of memory
 * @var bl_block_type_t:: WRITE_EEPROM
 * 0x03U - EEPROM Data Block - Identifies operational blocks
 * that need to be written into the EEPROM section of memory
 */
typedef enum
{
    UNLOCK_BOOTLOADER   = 0x01U,
    WRITE_FLASH         = 0x02U,
#if defined(BL_EEPROM_WRITE_ENABLED)
    WRITE_EEPROM        = 0x03U,
#endif
} bl_block_type_t;

/**
 * @ingroup mdfu_client_8bit
 * @struct bl_command_header_t
 * @brief Operational data orientation for each operation.
 * @var bl_command_header_t:: startAddress
 * Member 'startAddress' contains the start address of the data payload.
 * @var bl_command_header_t:: pageEraseUnlockKey
 * Member 'pageEraseUnlockKey' contains the unlock key needed to erase a page of memory.
 * @var bl_command_header_t:: pageWriteUnlockKey
 * Member 'pageWriteUnlockKey' contains the unlock key needed to write a page to memory.
 * @var bl_command_header_t:: byteWriteUnlockKey
 * Member 'byteWriteUnlockKey' contains the unlock key needed to write a byte or a word of memory.
 * @var bl_command_header_t:: pageReadUnlockKey
 * Member 'pageReadUnlockKey' contains the unlock key needed to read a page of memory.
 */
typedef struct
{
    uint32_t        startAddress;
    uint16_t        pageEraseUnlockKey;
    uint16_t        pageWriteUnlockKey;
    uint16_t        byteWriteUnlockKey;
    uint16_t        pageReadUnlockKey;
 } bl_command_header_t;

/**
 * @ingroup mdfu_client_8bit
 * @def BL_COMMAND_HEADER_SIZE
 * Total size of operational block header part.
 */
#define BL_COMMAND_HEADER_SIZE ((uint16_t)sizeof (bl_command_header_t))

/**
 * @ingroup mdfu_client_8bit
 * @struct bl_block_header_t
 * @brief Header data orientation for each block.
 * @var bl_block_header_t:: blockLength
 * Member 'blockLength' contains the total length of data bytes in the block.
 * @var bl_block_header_t:: blockType
 * Member 'blockType' contains the code that corresponds to the type of data inside
 * of the payload buffer.
 */
typedef struct
{
    uint16_t        blockLength;
    bl_block_type_t blockType;
} bl_block_header_t;

/**
 * @ingroup mdfu_client_8bit
 * @def BL_BLOCK_HEADER_SIZE
 * Total size of basic block header part.
 */
#define BL_BLOCK_HEADER_SIZE ((uint16_t)sizeof (bl_block_header_t))

/**
 * @ingroup mdfu_client_8bit
 * @def BL_WRITE_BYTE_LENGTH
 * Maximum number of bytes that the bootloader can hold inside of its process buffer.
 */
#if (defined(PIC_ARCH) && !defined(_PIC18))
#define BL_WRITE_BYTE_LENGTH        ((uint16_t) PROGMEM_PAGE_SIZE * 2U)
#else
#define BL_WRITE_BYTE_LENGTH        ((uint16_t) PROGMEM_PAGE_SIZE)
#endif

/**
 * @ingroup mdfu_client_8bit
 * @def BL_MAX_BUFFER_SIZE
 * Maximum length of data in bytes that the bootloader can receive from the host in each operational block.
 */
#define BL_MAX_BUFFER_SIZE          (BL_BLOCK_HEADER_SIZE + BL_COMMAND_HEADER_SIZE + BL_WRITE_BYTE_LENGTH)

/**
 * @ingroup mdfu_client_8bit
 * @brief Performs the initialization steps required to configure the bootloader peripherals.
 * @param None.
 * @return @ref BL_PASS - Bootloader initialization was successful
 * @return @ref BL_ERROR_COMMAND_PROCESSING - Bootloader initialization has failed
 */
bl_result_t BL_Initialize(void);

/**
 * @ingroup mdfu_client_8bit
 * @brief Executes the required action based on the block type received in the bootloader data buffer
 * @param [in] commandBuffer - Pointer to the start of the bootloader operational data
 * @param [in] commandLength - Length of the new data received by the FTP
 * @return @ref BL_PASS - Process cycle finished successfully
 * @return @ref BL_FAIL - Process cycle failed unexpectedly
 * @return @ref BL_ERROR_UNKNOWN_COMMAND - Process cycle encountered an unknown command
 * @return @ref BL_ERROR_VERIFICATION_FAIL - Process cycle failed to verify the application image
 * @return @ref BL_ERROR_COMMAND_PROCESSING - Process cycle failed due to a data or processing related issue
 * @return @ref BL_ERROR_ADDRESS_OUT_OF_RANGE - Process cycle failed due to an incorrect address
 */
bl_result_t BL_BootCommandProcess(uint8_t * commandBuffer, uint16_t commandLength);

/**
 * @ingroup mdfu_client_8bit
 * @brief Performs actions to jump the MCU program counter to 
 * the application start address.
 */
void BL_ApplicationStart(void);

#endif // BL_CORE_H