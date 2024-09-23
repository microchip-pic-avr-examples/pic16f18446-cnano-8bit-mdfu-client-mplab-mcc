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
 * @file        bl_core.c
 * @ingroup     mdfu_client_8bit
 *
 * @brief       This file contains APIs to support  file transfer-based
 *              bootloader operations, including  an FTP module and all bootloader
 *              core firmware.
 *
 * @see @ref    mdfu_client_8bit_ftp
 */

/**@misradeviation{@required, 11.3} Although this is a valid concern in most systems, we are
 * deviating from common practice here because we know how the compilers will behave for our
 * hardware. There is no inherent risk for the supported MCUs when casting from these two pointer types.
 * Be sure to take special care when porting this code to a new hardware system as the endianness of the
 * hardware does matter.
 */

#include "bl_core.h"
#include "bl_app_verify.h"
#include "bl_config.h"
#include "bl_memory.h"
#include "ftp/bl_ftp.h"
#include "../com_adapter/com_adapter.h"
#ifdef __XC
#include <xc.h>
#endif

typedef struct
{
    bl_block_header_t blockHeader;
    uint8_t imageVersionPatch;
    uint8_t imageVersionMinor;
    uint8_t imageVersionMajor;
    uint32_t deviceId;
    uint16_t maxPayloadSize;
    bl_command_header_t commandHeader;
} bl_unlock_boot_metadata_t;

static bool bootloaderCoreUnlocked = false;

#if defined(AVR_ARCH)
/**
 * AVR devices have vectored interrupts where the base address is configurable
 * through the IVSEL bit of the CPU Interrupt Controller's CTRLA register. Thus
 * there is no need for the bootloader to redirect the interrupt vectors.
 */
typedef void (*app_t)(void);
#elif defined(PIC_ARCH)
#if BL_VECTORED_INTERRUPTS_ENABLED == 1
#if defined(IVTBASE)
/**
 * Newer PIC18F devices have vectored interrupts with the base address specified
 * by the SFR IVTBASE. Thus there is no need for the bootloader to redirect the
 * interrupt vectors. The end application should specify IVTBASE as needed.
 */
#elif defined(_PIC18)
/**
 * The bootloader code does not use any interrupts.
 * However, the application code may use interrupts.
 * The interrupt vector on a PIC18F is located at
 * address 0x0008 (High) and 0x0018 (Low).
 * The following function will be located
 * at the interrupt vector and will contain a jump to
 * the new application interrupt vectors.
 */
asm("psect  intcode,global,reloc=2,class=CODE,delta=1");
asm("GOTO " ___mkstr(BL_APPLICATION_INTERRUPT_VECTOR_HIGH));

#if !(_18FXXQ10_FAMILY_ || _18FXXK40_FAMILY_)
asm("psect  intcodelo,global,reloc=2,class=CODE,delta=1");
#endif
asm("GOTO " ___mkstr(BL_APPLICATION_INTERRUPT_VECTOR_LOW));
#elif !defined(_PIC18)
/**
 * The bootloader code does not use any interrupts.
 * However, the application code may use interrupts.
 * The interrupt vector on a PIC16F is located at
 * address 0x0004.
 * The following function will be located
 * at the interrupt vector and will contain a jump to
 * the new application interrupt vector
 */
asm("psect  intentry,global,class=CODE,delta=2");
asm("pagesel " ___mkstr(BL_APPLICATION_INTERRUPT_VECTOR_LOW));
asm("GOTO " ___mkstr(BL_APPLICATION_INTERRUPT_VECTOR_LOW));
#endif
#endif
#endif

static bl_result_t BootloaderProcessorUnlock(uint8_t * bufferPtr);
static void DownloadAreaErase(uint32_t startAddress, uint16_t pageEraseUnlockKey);

/* cppcheck-suppress misra-c2012-2.7; This rule will not be followed to aid in new feature support in the future. */
bl_result_t BL_BootCommandProcess(uint8_t * bootDataPtr, uint16_t bufferLength)
{
    bl_result_t bootCommandStatus = BL_ERROR_UNKNOWN_COMMAND;

    // Copy out the data buffer address into a defined packet structure
    bl_command_header_t commandHeader;
    (void) memcpy(&commandHeader, (void *) & bootDataPtr[BL_BLOCK_HEADER_SIZE] , sizeof (bl_command_header_t));
    bl_block_header_t blockHeader;
    (void) memcpy(&blockHeader, (void *) bootDataPtr, BL_BLOCK_HEADER_SIZE);

#ifdef PIC_ARCH
    // keys declared at local scope
    key_structure_t userProvidedKeys = {
        .eraseUnlockKey = commandHeader.pageEraseUnlockKey,
        .byteWordWriteUnlockKey = commandHeader.byteWriteUnlockKey,
        .rowWriteUnlockKey = commandHeader.pageWriteUnlockKey,
        .readUnlockKey = commandHeader.pageReadUnlockKey,
    };
#endif
    // Switch on the bootloader command and execute the logic needed
    switch (blockHeader.blockType)
    {
    case UNLOCK_BOOTLOADER:
        bootCommandStatus = BootloaderProcessorUnlock(bootDataPtr);
        break;
    case WRITE_FLASH:
        if (bootloaderCoreUnlocked)
        {
#ifdef PIC_ARCH
            // Tell memory layer how to unlock memory
            BL_MemoryUnlockKeysInit(userProvidedKeys);
#endif
            if ((FLASH_PageOffsetGet((flash_address_t) commandHeader.startAddress) == (flash_address_t) 0)
                    /* cppcheck-suppress misra-c2012-7.2; This rule cannot be followed due to assembly syntax requirements. */
                    && (flash_address_t) commandHeader.startAddress >= (flash_address_t) BL_APPLICATION_START_ADDRESS)
            {
                // Call the abstracted write function
                bl_mem_result_t memoryStatus = BL_FlashWrite(
                                                             (flash_address_t) commandHeader.startAddress,
                                                             /* cppcheck-suppress misra-c2012-11.3 */
                                                             (flash_data_t *) & (bootDataPtr[BL_COMMAND_HEADER_SIZE + BL_BLOCK_HEADER_SIZE]),
                                                             PROGMEM_PAGE_SIZE
                                                             );

                bootCommandStatus = (memoryStatus == BL_MEM_PASS) ? BL_PASS : BL_ERROR_COMMAND_PROCESSING;
            }
            else
            {
                bootCommandStatus = BL_ERROR_ADDRESS_OUT_OF_RANGE;
            }
#ifdef PIC_ARCH
            // Erase keys before proceeding
            BL_MemoryUnlockKeysClear();
#endif
        }
        break;

#if defined(BL_EEPROM_WRITE_ENABLED)
    case WRITE_EEPROM:
        if (bootloaderCoreUnlocked)
        {
#ifdef PIC_ARCH
            // Tell memory layer how to unlock memory
            BL_MemoryUnlockKeysInit(userProvidedKeys);
#endif
            if (FLASH_PageOffsetGet((eeprom_address_t) commandHeader.startAddress) == (eeprom_address_t) 0)
            {
                bl_mem_result_t memoryStatus = BL_EEPROMWrite(
                                                              (eeprom_address_t) commandHeader.startAddress,
                                                              (eeprom_data_t *) & (bootDataPtr[BL_COMMAND_HEADER_SIZE + BL_BLOCK_HEADER_SIZE]),
                                                              (blockHeader.blockLength - ((BL_COMMAND_HEADER_SIZE + BL_BLOCK_HEADER_SIZE)  - 2U))
                                                              );

                bootCommandStatus = (memoryStatus == BL_MEM_PASS) ? BL_PASS : BL_ERROR_COMMAND_PROCESSING;
            }
            else
            {
                bootCommandStatus = BL_ERROR_ADDRESS_OUT_OF_RANGE;
            }
#ifdef PIC_ARCH
            // Erase keys before proceeding
            BL_MemoryUnlockKeysClear();
#endif
        }
        break;
#endif
    default:
        bootCommandStatus = BL_ERROR_UNKNOWN_COMMAND;
        break;
    }

    return bootCommandStatus;
}

void BL_ApplicationStart(void)
{
#if defined(AVR_ARCH)
    /* cppcheck-suppress misra-c2012-7.2;
    This rule cannot be followed due to assembly syntax requirements. */
    app_t app = (app_t) ((uint64_t) BL_APPLICATION_START_ADDRESS / sizeof (app_t));
    app();
#elif defined(_PIC18)
    STKPTR = 0x00U;
    BSR = 0x00U;
    asm("goto " ___mkstr(BL_APPLICATION_START_ADDRESS));
#elif defined(PIC_ARCH) && !defined(_PIC18)
    STKPTR = 0x1FU;
    BSR = 0U;
    asm("pagesel " ___mkstr(BL_APPLICATION_START_ADDRESS));
    asm("goto  " ___mkstr(BL_APPLICATION_START_ADDRESS));
#endif
}

bl_result_t BL_Initialize(void)
{
    bl_result_t initResult = BL_PASS;

    bootloaderCoreUnlocked = false;

    return initResult;
}

static bl_result_t BootloaderProcessorUnlock(uint8_t * bufferPtr)
{
    bl_result_t commandStatus = BL_FAIL;

    bl_unlock_boot_metadata_t metadataPacket;
    (void) memcpy(&metadataPacket, (void *) bufferPtr, sizeof (bl_unlock_boot_metadata_t));

    // Verify the major version 
    if((metadataPacket.imageVersionMajor) != (uint8_t) BL_IMAGE_FORMAT_MAJOR_VERSION)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    // Verify the minor version is acceptable
    if(metadataPacket.imageVersionMinor <  (uint8_t) BL_IMAGE_FORMAT_MINOR_VERSION)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }

    // Read device id from memory and compare against the expected id house in the file data.
#if defined(PIC_ARCH)
    device_id_data_t deviceId = DeviceID_Read(BL_DEVICE_ID_START_ADDRESS_U);
#elif defined(AVR_ARCH)
    device_id_data_t deviceId = SIGROW_DeviceIDRead(BL_DEVICE_ID_START_ADDRESS_U);
#endif
    if (deviceId != (device_id_data_t) metadataPacket.deviceId)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    // Read and compare write size
    if (metadataPacket.maxPayloadSize != BL_WRITE_BYTE_LENGTH)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    // Compare the given start of app to the APPLICATION_START_ADDRESS and handle
    /* cppcheck-suppress misra-c2012-7.2;
    This rule cannot be followed due to assembly syntax requirements. */
    if (metadataPacket.commandHeader.startAddress != (uint32_t) BL_APPLICATION_START_ADDRESS)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }

    // Compare each NVM key that is expected to be used for booting
#if defined(_18FXXQ10_FAMILY_)
    if (metadataPacket.commandHeader.pageEraseUnlockKey != (uint16_t) UNLOCK_KEY_PAGE_ERASE)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.pageWriteUnlockKey != (uint16_t) UNLOCK_KEY_ROW_WRITE)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.pageReadUnlockKey != (uint16_t) UNLOCK_KEY_ROW_READ)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.byteWriteUnlockKey != (uint16_t) UNLOCK_KEY_WORD_BYTE_WRITE)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
#elif defined(PIC_ARCH)
    if (metadataPacket.commandHeader.pageEraseUnlockKey != (uint16_t) UNLOCK_KEY)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.pageWriteUnlockKey != (uint16_t) UNLOCK_KEY)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.pageReadUnlockKey != 0x0000U)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.byteWriteUnlockKey != (uint16_t) UNLOCK_KEY)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
#elif defined(AVR_ARCH)
    if (metadataPacket.commandHeader.pageEraseUnlockKey != 0x0000U)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.pageWriteUnlockKey != 0x0000U)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.pageReadUnlockKey != 0x0000U)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
    if (metadataPacket.commandHeader.byteWriteUnlockKey != 0x0000U)
    {
        commandStatus = BL_ERROR_VERIFICATION_FAIL;
    }
#endif

    if (commandStatus != BL_ERROR_VERIFICATION_FAIL)
    {
        bootloaderCoreUnlocked = true;
        commandStatus = BL_PASS;
        DownloadAreaErase(
                        metadataPacket.commandHeader.startAddress,
                        metadataPacket.commandHeader.pageEraseUnlockKey
                        );
    }

    return commandStatus;
}

/* cppcheck-suppress misra-c2012-2.7 */
static void DownloadAreaErase(uint32_t startAddress, uint16_t pageEraseUnlockKey)
{
    nvm_status_t errorStatus;

    flash_address_t address;
    address = (flash_address_t) startAddress;

#ifdef PIC_ARCH
    uint16_t unlockKey = pageEraseUnlockKey;
#endif
    while (address < (PROGMEM_SIZE - 1U))
    {
#ifdef PIC_ARCH
        NVM_UnlockKeySet(unlockKey);
#endif
        errorStatus = FLASH_PageErase(address);
#ifdef PIC_ARCH
        NVM_UnlockKeyClear();
#endif
        if (errorStatus != NVM_OK)
        {
            break;
        }
        address += PROGMEM_PAGE_SIZE;
    }
}