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
 * @file        bl_result_type.h
 * @ingroup     mdfu_client_8bit
 *
 * @brief       This file contains an enumeration of all the bootloader status codes used by the core.
 */

#ifndef BL_RESULT_TYPE_H
/* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
#define	BL_RESULT_TYPE_H

/**
 * @ingroup mdfu_client_8bit
 * @enum bl_result_t
 * @brief Enumeration of bootloader API return codes.
 *
 * This enumeration defines the various return codes used by the bootloader APIs.
 * Each code represents a specific status or error condition that can be returned
 * by the bootloader functions.
 */
typedef enum
{
    BL_PASS = 0x81U, /**< (0b10000001) (dec 129) Operation completed successfully. */
    BL_BUSY = 0x3CU, /**< (0b00111100) (dec 60)  Bootloader is busy processing another request. */
    BL_FAIL = 0xC3U, /**< (0b11000011) (dec 195) Operation failed. */
    BL_ERROR_COMMUNICATION_FAIL = 0x18U, /**< (0b00011000) (dec 24)  Communication failure occurred. */
    BL_ERROR_FRAME_VALIDATION_FAIL = 0xFFU, /**< (0b11111111) (dec 255) Frame validation failed. */
    BL_ERROR_BUFFER_OVERLOAD = 0xBDU, /**< (0b10111101) (dec 190) Buffer overload detected. */
    BL_ERROR_INVALID_ARGUMENTS = 0xE7U, /**< (0b11100111) (dec 231) Invalid arguments provided. */
    BL_ERROR_UNKNOWN_COMMAND = 0x42U, /**< (0b01000010) (dec 66) Unknown command received. */
    BL_ERROR_ADDRESS_OUT_OF_RANGE = 0x24U, /**< (0b00100100) (dec 36) Address out of range. */
    BL_ERROR_COMMAND_PROCESSING = 0x7EU, /**< (0b01111110) (dec 126) Error occurred during command processing. */
    BL_ERROR_VERIFICATION_FAIL = 0xDBU, /**< (0b11011011) (dec 219) Verification failed. */
    BL_ERROR_BUFFER_UNDERLOAD = 0xAAU, /**< (0b01010101) (dec 170) Buffer underload detected. */
} bl_result_t;

#endif	// BL_RESULT_TYPE_H