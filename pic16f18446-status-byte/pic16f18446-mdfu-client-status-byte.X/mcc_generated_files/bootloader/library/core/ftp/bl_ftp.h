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
 * @file        bl_ftp.h
 * @defgroup    mdfu_client_8bit_ftp 8-bit Bootloader Client File Transfer Protocol (FTP) Handler
 * 
 * @brief       Transfer binary file blocks using a defined transfer protocol.
 */
#ifndef BL_FTP_H
/* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
#define BL_FTP_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#ifdef __XC
#include <xc.h>
#endif
#include "../bl_result_type.h"

/**
 * @ingroup mdfu_client_8bit_ftp
 * @brief Acts as the main task runner of the codebase. This function will be called in a
 * loop to receive bytes from the host and make calls to the responsible software layers.
 * @param None.
 * @return @ref BL_PASS - FTP process cycle finished successfully
 * @return @ref BL_FAIL - FTP process cycle failed unexpectedly
 * @return @ref BL_ERROR_COMMUNICATION_FAIL - FTP process cycle failed to communicate with the host
 * @return @ref BL_ERROR_FRAME_VALIDATION_FAIL - FTP process cycle failed at the frame check stage
 * @return @ref BL_ERROR_BUFFER_OVERLOAD - FTP process cycle failed due to a buffer overflow
 * @return @ref BL_ERROR_UNKNOWN_COMMAND - FTP process cycle encountered an unknown command
 * @return @ref BL_ERROR_VERIFICATION_FAIL - FTP process cycle failed to verify the application image
 * @return @ref BL_ERROR_COMMAND_PROCESSING - FTP process cycle failed due to a data or processing related issue
 * @return @ref BL_ERROR_ADDRESS_OUT_OF_RANGE - FTP process cycle failed due to an incorrect address
 */
bl_result_t FTP_Task(void);

/**
 * @ingroup mdfu_client_8bit_ftp
 * @brief Performs the initialization actions required to set up the FTP and dependant layers.
 * @param None.
 * @return @ref BL_PASS - FTP initialization finished successfully
 * @return @ref BL_FAIL - FTP initialization failed unexpectedly
 */
bl_result_t FTP_Initialize(void);

#endif // BL_FTP_H