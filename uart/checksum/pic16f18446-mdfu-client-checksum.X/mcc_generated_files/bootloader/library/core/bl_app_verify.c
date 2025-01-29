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
 * @file        bl_app_verify.c
 * @ingroup     mdfu_client_8bit
 *
 * @brief       This file contains APIs to support verification of the
 *              application image space. \n
 */

#include <stdint.h>
#include <stdbool.h>
#include "bl_app_verify.h"
#include "bl_config.h"
#include "../../../nvm/nvm.h"

#define HASH_DATA_SIZE      (uint32_t)2U
#define END_OF_APP          (((uint32_t)PROGMEM_SIZE - (uint32_t)1U) - HASH_DATA_SIZE)
#define HASH_STORE_ADDRESS  (END_OF_APP + (uint32_t)1U)
#define HASH_CALC_LENGTH    (HASH_STORE_ADDRESS - (uint32_t)BL_APPLICATION_START_ADDRESS)

static void ChecksumCalculate(flash_address_t startAddress, uint32_t length, uint16_t *checkSum);
static bl_result_t ChecksumValidate(flash_address_t startAddress, uint32_t length, flash_address_t checkAddress);

static void ChecksumCalculate(flash_address_t startAddress, uint32_t length, uint16_t *checkSum)
{
    flash_address_t readAddress = startAddress;
#if defined(_PIC18) || defined(AVR_ARCH)
    for (uint32_t i = 0; i < length; i += 2U)
    {
        *checkSum += (uint16_t) FLASH_Read(readAddress);
        readAddress++;
        *checkSum += ((uint16_t) FLASH_Read(readAddress)) << 8;
        readAddress++;
    }
#elif !defined(_PIC18) && defined(PIC_ARCH)
    for (uint32_t i = 0; i < length; i += 1U)
    {
        *checkSum += (uint16_t) FLASH_Read(readAddress);
        readAddress++;
    }
#endif
}

static bl_result_t ChecksumValidate(flash_address_t startAddress, uint32_t length, flash_address_t checkAddress)
{

    bl_result_t result = BL_FAIL;
    uint16_t check_sum = 0;

    bool refAddrInsideEvaluatedArea = (((checkAddress + 1U) >= startAddress) && (checkAddress < (startAddress + length)));
    bool refAddrOutsideFlash = ((checkAddress + 1U) >= PROGMEM_SIZE);

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
        ChecksumCalculate(startAddress, length, &check_sum);
#if defined(_PIC18) || defined(AVR_ARCH)
        uint16_t refChecksum = ((uint16_t) (FLASH_Read(checkAddress + 1U) << 8U) | FLASH_Read(checkAddress));
#elif !defined(_PIC18) && defined(PIC_ARCH)
        uint16_t refChecksum = (uint16_t) (
                ((FLASH_Read(checkAddress + 1U) & 0x00FFU) << 8U)
                | (FLASH_Read(checkAddress) & 0x00FFU)
                );
#endif
        if (refChecksum != check_sum)
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

bl_result_t BL_ImageVerify(void)
{
    bl_result_t result = BL_ERROR_VERIFICATION_FAIL;

    /* cppcheck-suppress misra-c2012-7.2; This rule cannot be followed due to assembly syntax requirements. */
    result = ChecksumValidate(BL_APPLICATION_START_ADDRESS, HASH_CALC_LENGTH, HASH_STORE_ADDRESS);

    return result;
}