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
 * @file    com_adapter.c
 * @brief   This is the implementation file for the communication adapter layer using UART.
 * @ingroup com_adapter
 */

/**@misradeviation{@advisory, 8.9} This cannot be followed since the value of the variables
 * must be saved until the next time the function is called. Declaring at block scope will
 * make managing the variable reset more difficult.
 */
/**@misradeviation{@advisory, 15.4} This will not be followed because it would require removing
 * code that is helpful for debugging making the code more difficult to work with.
 */

#include "com_adapter.h"

/**
 * @brief Macro workaround for a macro issue in uart
 */
#ifndef SERCOM_IsRxReady
#define SERCOM_IsRxReady        SERCOM__IsRxReady
#endif

/**
 * @ingroup com_adapter
 * @def START_OF_PACKET_BYTE
 * Special character for identifying the start of the frame.
 */
#define START_OF_PACKET_BYTE    (0x56U)
/**
 * @ingroup com_adapter
 * @def END_OF_PACKET_BYTE
 * Special character for identifying the end of the frame.
 */
#define END_OF_PACKET_BYTE      (0x9EU)
/**
 * @ingroup com_adapter
 * @def ESCAPE_BYTE
 * Special character for identifying an escaped byte in the file data.
 */
#define ESCAPE_BYTE             (0xCCU)

typedef struct
{
    uint8_t EscapeCharacter;
    uint8_t StartOfPacketCharacter;
    uint8_t EndOfPacketCharacter;
} ftp_special_characters_t;

static ftp_special_characters_t ftpSpecialCharacters = {
    .EscapeCharacter = ESCAPE_BYTE,
    .StartOfPacketCharacter = START_OF_PACKET_BYTE,
    .EndOfPacketCharacter = END_OF_PACKET_BYTE
};

static uint16_t MaxBufferLength = 0U;
static bool isReceiveWindowOpen = false;
static bool isEscapedByte= false;

static com_adapter_result_t DataSend(uint8_t *data, size_t length);
static com_adapter_result_t DataReceive(uint8_t *data, size_t length);

static uint16_t FrameCheckCalculate(uint8_t * ftpData, uint16_t bufferLength)
{
    uint16_t numBytesChecksummed = 0;
    uint16_t checksum = 0;

    while (numBytesChecksummed < (bufferLength))
    {
        if ((numBytesChecksummed % 2U) == 0U)
        {
            checksum += ((uint16_t) (ftpData[numBytesChecksummed]));
        }
        else
        {
            checksum += (((uint16_t) (ftpData[numBytesChecksummed])) << 8);
        }
        numBytesChecksummed++;
    }

    return ~checksum;
}

static com_adapter_result_t DataSend(uint8_t *data, size_t length)
{
    com_adapter_result_t status = COM_PASS;

    if ((length == 0U) || (data == NULL))
    {
        status = COM_INVALID_ARG;
    }
    else
    {
        for (uint16_t byteIndex = 0U; byteIndex != length; byteIndex++)
        {
            status = COM_PASS;

            while (!SERCOM_IsTxReady())
            {
                // Wait for TX to be ready
            };
            // Call to send the next byte
            SERCOM_Write(data[byteIndex]);

            if (SERCOM_ErrorGet() != 0U)
            {
#warning "Handle Transmission Error."
            }
        }
    }

    while (!SERCOM_IsTxDone())
    {
        // Block until last byte shifts out
    }

    return status;
}

static com_adapter_result_t DataReceive(uint8_t *data, size_t length)
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

            if (SERCOM_ErrorGet() == 0U)
            {
                if (SERCOM_IsRxReady())
                {
                    data[byteIndex] = SERCOM_Read();
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
                SERCOM_Deinitialize();
                SERCOM_Initialize();
            }
        }
    }
    return status;
}

com_adapter_result_t COM_FrameTransfer(uint8_t *receiveBufferPtr, uint16_t *receiveIndexPtr)
{
    uint8_t nextByte = 0U;
    com_adapter_result_t processResult = COM_FAIL;

    if ((receiveBufferPtr == NULL) || (receiveIndexPtr == NULL))
    {
        processResult = COM_INVALID_ARG;
    }
    else
    {
        if (SERCOM_IsRxReady())
        {
            processResult = DataReceive(&nextByte, 1U);
        }
        if (processResult == COM_PASS)
        {
            if (nextByte == ftpSpecialCharacters.StartOfPacketCharacter)
            {
                // Open the buffer window
                isReceiveWindowOpen = true;
                isEscapedByte = false;
                // Reset the buffer index
                *receiveIndexPtr = 0U;

                processResult = COM_BUSY;
            }
            else if (isReceiveWindowOpen)
            {
                if (nextByte == ftpSpecialCharacters.EndOfPacketCharacter)
                {
                    // Close the buffer window
                    isReceiveWindowOpen = false;

                    // Calculate the frame check here
                    uint16_t fcs = FrameCheckCalculate(receiveBufferPtr, *receiveIndexPtr - FRAME_CHECK_SIZE);

                    // Read FCS from the transfer buffer
                    uint8_t *startOfWord = &receiveBufferPtr[*receiveIndexPtr - FRAME_CHECK_SIZE];
                    uint16_t frameCheckSequence = 0x0000;
                    if (startOfWord != NULL)
                    {
                        uint8_t * workPtr = startOfWord;
                        uint8_t lowByte = *workPtr;
                        workPtr++;
                        uint8_t highByte = *workPtr;
                        frameCheckSequence = (uint16_t) ((((uint16_t) highByte) << 8) | lowByte);
                    }

                    if (fcs == frameCheckSequence)
                    {
                        // Set the status to execute the command
                        processResult = COM_PASS;
                    }
                    else
                    {
                        // Set the status to execute the command
                        processResult = COM_TRANSPORT_FAILURE;
                    }
                }
                else if (nextByte == ftpSpecialCharacters.EscapeCharacter)
                {
                    isEscapedByte = true;
                    processResult = COM_BUSY;
                }
                else
                {
                    // If escape was flagged perform the bit flip to correct the byte
                    if (isEscapedByte)
                    {
                        nextByte = ~nextByte;
                        isEscapedByte = false;
                    }

                    // Route the byte into the transfer buffer
                    if (*receiveIndexPtr < MaxBufferLength)
                    {
                        receiveBufferPtr[*receiveIndexPtr] = nextByte;
                        (*receiveIndexPtr)++;
                        processResult = COM_BUSY;
                    }
                    else
                    {
                        // Close the buffer window
                        isReceiveWindowOpen = false;
                        processResult = COM_BUFFER_ERROR;
                    }
                }
            }
            else
            {
                processResult = COM_FAIL;
            }
        }
        else
        {
            processResult = COM_FAIL;
        }
    }
    return processResult;
}

com_adapter_result_t COM_FrameSet(uint8_t *responseBufferPtr, uint16_t responseLength)
{
    com_adapter_result_t processResult = COM_FAIL;

    if((responseBufferPtr == NULL) || (responseLength == 0U))
    {
        processResult = COM_INVALID_ARG;
    }
    else
    {
        // Integrity Check
        uint16_t frameCheck = FrameCheckCalculate(responseBufferPtr, responseLength);

        processResult = DataSend(&(ftpSpecialCharacters.StartOfPacketCharacter), 1U);

        if(processResult == COM_PASS)
        {
            uint8_t nextByte;
            uint16_t sentByteCount = 0x00U;

            while (sentByteCount < (responseLength + FRAME_CHECK_SIZE))
            {/* cppcheck-suppress misra-c2012-15.4 */
                if(sentByteCount == responseLength)
                {
                    // send the low byte first
                    nextByte = (uint8_t)(frameCheck & 0x00FFU);
                }
                else if(sentByteCount == (responseLength + 1U))
                {
                    // send the high byte first
                    nextByte = (uint8_t) (frameCheck >> 8);
                }
                else
                {
                    nextByte = responseBufferPtr[sentByteCount];
                }

                if ((nextByte == START_OF_PACKET_BYTE) || (nextByte == END_OF_PACKET_BYTE) || (nextByte == ESCAPE_BYTE))
                {
                    processResult = DataSend(&(ftpSpecialCharacters.EscapeCharacter), 1U);
                    if (processResult != COM_PASS)
                    {
                        // fail and stop sending
                        break;
                    }
                    nextByte = ~nextByte;
                }

                processResult = DataSend((uint8_t *) & nextByte, 1U);
                if (processResult != COM_PASS)
                {
                    // fail and stop sending
                    break;
                }

                sentByteCount++;
            }

            processResult = DataSend((uint8_t *)& (ftpSpecialCharacters.EndOfPacketCharacter), 1U);
        }
        else 
        {
            // fail with no other actions
        }
    }

    return processResult;
}

com_adapter_result_t COM_Initialize(uint16_t maximumBufferLength)
{
    com_adapter_result_t result = COM_FAIL;
    if (maximumBufferLength != 0U)
    {
        MaxBufferLength = maximumBufferLength;
        isReceiveWindowOpen = false;
        isEscapedByte = false;
        SERCOM_Initialize();
        result = COM_PASS;
    }
    else
    {
        result = COM_INVALID_ARG;
    }

    return result;
}