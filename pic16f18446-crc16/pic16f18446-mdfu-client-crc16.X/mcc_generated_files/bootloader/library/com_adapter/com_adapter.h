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
 * @file    com_adapter.h
 * @brief   This file contains prototypes and other data types for communication adapter layer
 * @defgroup com_adapter Communication Adapter
 */

#ifndef COM_ADAPTER_H
/* cppcheck-suppress misra-c2012-2.5 */
#define COM_ADAPTER_H

#include <stdint.h>
#ifdef __XC
#include <xc.h>
#endif
#include "../../../system/system.h"

/**
 * @ingroup com_adapter
 * @def SERCOM
 * This macro is used to set SERCOM to the custom name used in the communication module UI.
 */
#define SERCOM (UART1) /* cppcheck-suppress misra-c2012-2.5; This is a false positive. */

/**
 * @ingroup com_adapter
 * @enum com_adapter_result_t
 * @brief Contains codes for the return values of the bootloader communication adapter layer APIs.
 * @var com_adapter_result_t: COM_PASS
 * 0xE7U - com_adapter operation succeeded
 * @var com_adapter_result_t: COM_FAIL
 * 0xC3U - com_adapter operation failed
 * @var com_adapter_result_t: COM_INVALID_ARG
 * 0x96U - com_adapter operation has an invalid argument
 */
typedef enum
{
    COM_PASS = 0xE7U, 
    COM_FAIL = 0xC3U, 
    COM_INVALID_ARG = 0x96U, 
} com_adapter_result_t;

/**
 @ingroup com_adapter
 @brief Sends the data to SERCOM. 
 @param [in] data - Pointer to the buffer  provided by the bootloader
 @param [in] length - Number of bytes to be sent to SERCOM
 @return @ref COM_PASS - Specified length of data is transmitted successfully \n
 @return @ref COM_INVALID_ARG - Data buffer is NULL and/or data length field is zero \n
 @return @ref COM_FAIL - An error occurred in SERCOM TX \n
 */
com_adapter_result_t COM_Send(uint8_t *data, size_t length);

/**
 @ingroup com_adapter
 @brief Receives the data from the RX and stores it in the software buffer provided by the bootloader. 
 @param [in] data - Pointer to the buffer provided by the bootloader
 @param [in] length - Number of bytes to be received from SERCOM
 @return @ref COM_PASS - Specified length of data is received successfully \n
 @return @ref COM_INVALID_ARG - Data buffer is NULL and/or data length field is zero \n
 @return @ref COM_FAIL - An error occurred in SERCOM RX \n
 */
com_adapter_result_t COM_Receive(uint8_t *data, size_t length);

#endif //COM_ADAPTER_H