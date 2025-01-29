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
 *              application image space.
 */

#include <stdint.h>
#include "bl_app_verify.h"
#include "bl_config.h"
#include "../../../nvm/nvm.h"

#if defined(_PIC18) || defined(AVR_ARCH)
#define STATUS_ADDRESS (flash_address_t)(PROGMEM_SIZE - 2U)
#elif !defined(_PIC18) && defined(PIC_ARCH)
#define STATUS_ADDRESS (flash_address_t)(PROGMEM_SIZE - 1U)
#endif
#define APPLICATION_VALID 0x55U

bl_result_t BL_ImageVerify(void)
{
    bl_result_t result = BL_FAIL;

#if defined(_PIC18) || defined(AVR_ARCH)
    if (FLASH_Read(STATUS_ADDRESS) == APPLICATION_VALID)
    {
        result = BL_PASS;
    }
#elif !defined(_PIC18) && defined(PIC_ARCH)
    flash_data_t wordRead = FLASH_Read(STATUS_ADDRESS);

    if ((wordRead & (flash_data_t) 0x00FFU) == (flash_data_t) APPLICATION_VALID)
    {
        result = BL_PASS;
    }
#endif
    else
    {
        result = BL_ERROR_VERIFICATION_FAIL;
    }

    return result;
}