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
 * @file        bl_example.c
 * @ingroup     8bit_mdfu_client
 *
 * @brief       This file contains APIs for running a bootloader update process
 *              using the FTP Handler and bootloader APIs.
 */
#include "../library/core/bl_core.h"
#include "../library/core/bl_app_verify.h"
#include "../library/core/ftp/bl_ftp.h"
#include "../../system/pins.h"
#include "bl_example.h"

/**
 * @ingroup 8bit_mdfu_client
 * @def IO_PIN_ENTRY_GetInputValue
 * This is a macro to remap the get value function of the entry pin.
 */
#define IO_PIN_ENTRY_GetInputValue  BL_ENTRY_GetValue

/**
 * @ingroup 8bit_mdfu_client
 * @def IO_PIN_ENTRY_RUN_BL
 * This is a macro that represents the "run bootloader" signal based on the
 * entry pin activation setting.
 * This is the expected level required at the pin to enter into bootloader mode.
 */
#define IO_PIN_ENTRY_RUN_BL         (0)

/**
 * @ingroup 8bit_mdfu_client
 * @def BL_INDICATOR_ON
 * This is a macro for enabling the bootloader indicator pin.
 * When the pin is set to start high, the bootloader will assume
 * that it needs to drive the pin low to enable it.
 */
#define BL_INDICATOR_ON             BL_INDICATOR_SetLow

/**
 * @ingroup 8bit_mdfu_client
 * @def BL_INDICATOR_OFF
 * This is a macro for disabling the bootloader indicator pin.
 * When the pin is set to start high, the bootloader will assume
 * that it needs to drive the pin high to disable it.
 */
#define BL_INDICATOR_OFF            BL_INDICATOR_SetHigh

typedef enum
{
    BOOTLOADER,
    APPLICATION
} bootloader_state_t;

static bootloader_state_t BootState;

static bool BL_CheckForcedEntry(void);

void BL_Example(void)
{
#warning "Customize the process logic to handle the bootloader processes."

    bl_result_t result;

    switch (BootState)
    {
    case APPLICATION:
        BL_INDICATOR_OFF();
        BL_StartApplication();
        break;
    case BOOTLOADER:
        BL_INDICATOR_ON();
        result = FTP_ProcessTask();
        break;
        default:
        // Unknown State; Do nothing
        break;
    }

    if (result == BL_ERROR_VERIFICATION_FAIL)
    {
#warning "Handle a failed bootload image however the user sees fit."
    }
}

bl_example_result_t BL_ExampleInitialize(void)
{
#warning "Customize the Initialization steps for the startup process and set the BootState accordingly."
    // Make assumption: We always start in bootload state
    BootState = BOOTLOADER;

    // On startup we must figure out what the bootloader is being requested to do.
    // If forced entry is being requested, we can short cut the verification scheme.
    // Otherwise check for a valid application
    if ((BL_CheckForcedEntry() == false) && (BL_VerifyImage() == BL_PASS))
    {
        // Set the application state
        BootState = APPLICATION;
    }

    return EXAMPLE_OK;
}

static bool BL_CheckForcedEntry(void)
{
    bool result = false;
#warning "Users can write their own process startup logic and return true when a bootload is needed."
    for (uint8_t i = 0U; i != 0xFFU; i++)
    {
        asm("nop");
    }
    // Check for entry pin signal
    if (IO_PIN_ENTRY_GetInputValue() == IO_PIN_ENTRY_RUN_BL)
    {
        result = true;
    }
    return result;
}

