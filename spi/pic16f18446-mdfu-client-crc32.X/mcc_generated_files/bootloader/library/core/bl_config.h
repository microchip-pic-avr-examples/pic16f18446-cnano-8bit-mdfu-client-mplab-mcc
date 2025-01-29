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
 * @file        bl_config.h
 * @ingroup     mdfu_client_8bit
 *
 * @brief       This file contains macros and type definitions related to the
 *              bootloader client device configuration and bootloader settings.
 */

#ifndef BL_BOOT_CONFIG_H
/* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
#define BL_BOOT_CONFIG_H

/**
 * @ingroup mdfu_client_8bit
 * @def PIC_ARCH
 * Corresponds to the current device architecture.
 * This macro is used to conditionally compile parts of the code needed
 * for specific architectures inside the bootloader core firmware.
 */
#ifndef PIC_ARCH
#define PIC_ARCH /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
#endif
/**
 * @ingroup mdfu_client_8bit
 * @def BL_IMAGE_FORMAT_MAJOR_VERSION
 * Represents the major version of the image format that is
 * understood by the bootloader core. \n
 */
#define BL_IMAGE_FORMAT_MAJOR_VERSION (0x00) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
/**
 * @ingroup mdfu_client_8bit
 * @def BL_IMAGE_FORMAT_MINOR_VERSION
 * Represents the minor version of the image format that is
 * understood by the bootloader core. \n
 */
#define BL_IMAGE_FORMAT_MINOR_VERSION (0x03) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
/**
 * @ingroup mdfu_client_8bit
 * @def BL_IMAGE_FORMAT_PATCH_VERSION
 * Represents the patch version of the image format that is
 * understood by the bootloader core. \n
 */
#define BL_IMAGE_FORMAT_PATCH_VERSION (0x00) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
/**
 * @ingroup mdfu_client_8bit
 * @def BL_VECTORED_INTERRUPTS_ENABLED
 * Indicates that the bootloader supports vectored interrupts in the application.
 */
#define BL_VECTORED_INTERRUPTS_ENABLED (1) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
/**
 * @ingroup mdfu_client_8bit
 * @def BL_APPLICATION_START_ADDRESS
 * Start of the application memory space.
 */
#define BL_APPLICATION_START_ADDRESS (0x1000) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
/**
 * @ingroup mdfu_client_8bit
 * @def BL_APPLICATION_INTERRUPT_VECTOR_LOW
 * Start address of the low-priority interrupt vector.
 */
#define BL_APPLICATION_INTERRUPT_VECTOR_LOW (0x1004) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
/**
 * @ingroup mdfu_client_8bit
 * @def BL_DEVICE_ID_START_ADDRESS_U
 * Device ID address.
 */
#define BL_DEVICE_ID_START_ADDRESS_U (0x06U) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */

#endif // BL_BOOT_CONFIG_H
