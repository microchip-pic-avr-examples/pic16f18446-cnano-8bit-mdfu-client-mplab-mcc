/**
 * © 2023 Microchip Technology Inc. and its subsidiaries.
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
 * @file        bl_app_verify.c
 * @ingroup     8bit_mdfu_client
 * 
 * @brief       This file contains APIs to support verification of the
 *              application image space.
 */

#include <stdint.h>
#include <stdbool.h>
#include "bl_app_verify.h"
#include "bl_config.h"
#include "../../../nvm/nvm.h"


#define CRC_POLYNOMIAL    (0xEDB88320U)
#define CRC_SEED          (0xFFFFFFFFU)
#define CRC_XorOut        (0x00000000U)

#define HASH_DATA_SIZE      4U
#define END_OF_APP          ((flash_address_t)((PROGMEM_SIZE - 1U) - HASH_DATA_SIZE))
#define HASH_STORE_ADDRESS  (flash_address_t)(END_OF_APP + 1U)
#define HASH_CALC_LENGTH    (HASH_STORE_ADDRESS - BL_APPLICATION_START_ADDRESS)

static void BL_CalculateCRC32(flash_address_t startAddress, uint32_t length, uint32_t *crcSeed);
static bl_result_t BL_ValidateCRC32(flash_address_t startAddress, uint32_t length, flash_address_t crcAddress);

#if defined(_PIC18) || defined(AVR_ARCH)

static void BL_CalculateCRC32(flash_address_t startAddress, uint32_t length, uint32_t *crcSeed)
{
    uint32_t byteIndex;
    uint8_t readByte;

    for (byteIndex = 0U; byteIndex < length; byteIndex++)
    {
        readByte = FLASH_Read((flash_address_t) (startAddress + byteIndex));

        *crcSeed ^= (uint32_t) readByte;

        for (uint8_t bit = 8U; bit > 0U; --bit)
        {
            if ((*crcSeed & 0x01U) != 0U)
            {
                *crcSeed = (*crcSeed >> 1U) ^ CRC_POLYNOMIAL;
            }
            else
            {
                *crcSeed >>= 1U;
            }
        }
    }
    *crcSeed ^= CRC_XorOut;
}
#elif !defined(_PIC18) && defined(PIC_ARCH)

static void BL_CalculateCRC32(flash_address_t startAddress, uint32_t length, uint32_t *crcSeed)
{
    uint16_t wordIndex;
    uint16_t byteIndex;
    uint16_t readWord;
    uint8_t byteArr[2];

    for (wordIndex = 0U; wordIndex < length; wordIndex++)
    {
        readWord = FLASH_Read((flash_address_t) startAddress + wordIndex);
        byteArr[1] = (uint8_t) (readWord >> 8U);
        byteArr[0] = (uint8_t) readWord;
        /* Bring next byte into the checksum. */
        for (byteIndex = 0U; byteIndex < 2U; byteIndex++)
        {
            *crcSeed ^= byteArr[byteIndex];
            for (uint8_t bit = 8U; bit > 0U; --bit)
            {
                if ((*crcSeed & 0x01U) != 0U)
                {
                    *crcSeed = (*crcSeed >> 1U) ^ CRC_POLYNOMIAL;
                }
                else
                {
                    *crcSeed >>= 1U;
                }
            }
        }
    }
    *crcSeed ^= CRC_XorOut;
}
#endif

static bl_result_t BL_ValidateCRC32(flash_address_t startAddress, uint32_t length, flash_address_t refAddress)
{
    bl_result_t result = BL_FAIL;
    uint32_t crc = CRC_SEED;
    uint32_t refCRC = 0x00000000U;
    bool refAddrInsideEvaluatedArea = (((refAddress + 3U) >= startAddress) && (refAddress < (startAddress + length)));
    bool refAddrOutsideFlash = ((refAddress + 3U) >= PROGMEM_SIZE);

    if ((length == 0U) || ((startAddress + length) > PROGMEM_SIZE))
    {
        result = BL_ERROR_INVALID_ARGUMENTS;
    }
    else if (refAddrInsideEvaluatedArea || refAddrOutsideFlash)
    {
        result = BL_ERROR_ADDRESS_OUT_OF_RANGE;
    }
    else
    {
        BL_CalculateCRC32(startAddress, length, &crc);
#if defined(_PIC18) || defined(AVR_ARCH)
        refCRC = (uint32_t) (
                (((uint32_t) FLASH_Read(refAddress))) |
                (((uint32_t) FLASH_Read(refAddress + 1U)) << 8U) |
                (((uint32_t) FLASH_Read(refAddress + 2U)) << 16U) |
                (((uint32_t) FLASH_Read(refAddress + 3U)) << 24U)
                );
#elif !defined(_PIC18) && defined(PIC_ARCH)
        refCRC = (uint32_t) (
                (((uint32_t) FLASH_Read(refAddress) & 0x00FFU)) |
                (((uint32_t) FLASH_Read(refAddress + 1U) & 0x00FFU) << 8U) |
                (((uint32_t) FLASH_Read(refAddress + 2U) & 0x00FFU) << 16U) |
                (((uint32_t) FLASH_Read(refAddress + 3U) & 0x00FFU) << 24U)
                );
#endif
        if (refCRC != crc)
        {
            result = BL_ERROR_VERIFICATION_FAIL;
        }
        else
        {
            result = BL_PASS;
        }
    }
    return result;
}

bl_result_t BL_VerifyImage(void)
{
    bl_result_t result = BL_ERROR_VERIFICATION_FAIL;
    /* cppcheck-suppress misra-c2012-7.2; This rule cannot be followed due to assembly syntax requirements. */
    result = BL_ValidateCRC32((flash_address_t) BL_APPLICATION_START_ADDRESS, HASH_CALC_LENGTH, HASH_STORE_ADDRESS);
    return result;
}