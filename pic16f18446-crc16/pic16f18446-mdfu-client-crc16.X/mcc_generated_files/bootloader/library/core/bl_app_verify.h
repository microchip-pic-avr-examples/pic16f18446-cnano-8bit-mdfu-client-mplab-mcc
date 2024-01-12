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
 * @file        bl_app_verify.h
 * @ingroup     8bit_mdfu_client
 * 
 * @brief       This file contains API prototypes to perform
 *              application image verification.
 */

#ifndef BL_VERIFY_H
/* cppcheck-suppress misra-c2012-2.5; This is a false positive. */
#define	BL_VERIFY_H

#include "bl_result_type.h"

/**
 * @ingroup 8bit_mdfu_client
 * @brief Performs a verification sequence on the application image memory space.
 * @return @ref BL_PASS - Bootloader verified the application image with no errors \n
 * @return @ref BL_ERROR_VERIFICATION_FAIL - Bootloader image verification failed \n
 * @return @ref BL_ERROR_COMMAND_PROCESSING - Bootloader image verification failed due to incorrect processing data \n
 * @return @ref BL_ERROR_ADDRESS_OUT_OF_RANGE - Bootloader image verification failed due to incorrect addresses \n
 */
bl_result_t BL_VerifyImage(void);

#endif // BL_VERIFY_H