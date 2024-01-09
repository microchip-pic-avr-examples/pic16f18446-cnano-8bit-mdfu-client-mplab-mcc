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
 * @file    com_adapter.c
 * @brief   This is the implementation file for the communication adapter layer.
 * @ingroup com_adapter
 */

#include "com_adapter.h"

/**
 @ingroup com_adapter
 @def STX
 This is a macro used to load the Sync character into the transmit FIFO
 */

com_adapter_result_t COM_Send(uint8_t *data, size_t length)
{
    com_adapter_result_t status;
    status = COM_PASS;

    if ((length == 0U) || (data == NULL))
    {
        status = COM_INVALID_ARG;
    }
    else
    {
        for (uint16_t byteIndex = 0; byteIndex != length; byteIndex++)
        {
            status = COM_PASS;

            while (!SERCOM.IsTxReady())
            {
                // Wait for TX to be ready
            };
            // Call to send the next byte
            SERCOM.Write(data[byteIndex]);

            if (SERCOM.ErrorGet() != 0)
            {
#warning "Handle Transmission Error."
            }
        }
    }

    while(!SERCOM.IsTxDone())
    {
        // Block until last byte shifts out
    }

    return status;
}

com_adapter_result_t COM_Receive(uint8_t *data, size_t length)
{
    com_adapter_result_t status;
    
    if ((length == 0U) || (data == NULL)) 
    {
        status = COM_INVALID_ARG;
    }
    else
    {
        for (uint16_t byteIndex = 0; byteIndex != length; byteIndex++)
        {
            status = COM_PASS;

            if (SERCOM.ErrorGet() == 0U)
            {
                if (SERCOM.IsRxReady()) 
                {
                    data[byteIndex] = SERCOM.Read();
                }
                else
                {
                    status = COM_FAIL;
                }
            }
            else
            {
                status = COM_FAIL;
                // Fully reset the UART
                SERCOM.Deinitialize();
                SERCOM.Initialize();
            }
        }
    }
    return status;
}