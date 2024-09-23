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
 * @brief   This is the implementation file for the communication adapter layer using SPI.
 * @ingroup com_adapter
 */
/**@misradeviation{@advisory, 2.3} This will not be followed due to Microchip coding
 * standards.
 */
/**@misradeviation{@advisory, 2.4} This will not be followed due to Microchip coding
 * standards.
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
 * @ingroup com_adapter
 * @def LENGTH_FIELD_SIZE
 * Holds the size of the length data field.
 */
#define LENGTH_FIELD_SIZE (2U)

/**
 * @ingroup com_adapter
 * @def SOP_SEQUENCE_LENGTH
 * Holds the number of bytes that need to be held in the data buffer for each start of frame sequence.
 */
#define SOP_SEQUENCE_LENGTH (4U)

/**
 * @ingroup com_adapter
 * @def MAX_RESPONSE_DATA_FIELD
 * Holds the maximum size of a response.
 */
#define MAX_RESPONSE_DATA_FIELD (25U)

/**
 * @ingroup com_adapter
 * @def RESPONSE_PACKET_SIZE
 * Holds the size of the response packet.
 */
#define RESPONSE_PACKET_SIZE (SOP_SEQUENCE_LENGTH + MAX_RESPONSE_DATA_FIELD + FRAME_CHECK_SIZE)

/**
 * @ingroup com_adapter
 * @def LENGTH_PACKET_SIZE
 * Holds the size of the length packet.
 */
#define LENGTH_PACKET_SIZE (SOP_SEQUENCE_LENGTH + LENGTH_FIELD_SIZE + FRAME_CHECK_SIZE)

/* cppcheck-suppress misra-c2012-2.3 */
typedef enum
{ /* cppcheck-suppress misra-c2012-2.4 */
    HOST_READ_CODE = 0x55U,
    HOST_WRITE_CODE = 0x11U,
} host_command_code_t;

typedef enum
{
    NO_ACTION = 0x00,
    SEND_LENGTH,
    SEND_RESPONSE,
    READ_COMMAND
} com_adapter_state_t;

static uint8_t sendBuffer[LENGTH_PACKET_SIZE + RESPONSE_PACKET_SIZE];
static uint16_t maxBufferLength = 0U;
static uint16_t calculatedFrameCheck = 0x0000U;
static uint16_t sendLength = 0U;
/* cppcheck-suppress misra-c2012-8.9 */
static uint16_t bytesSent = 0U;
static com_adapter_state_t comState = NO_ACTION;

static uint16_t FrameCheckCalculate(uint8_t * ftpData, uint16_t bufferLength);

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

com_adapter_result_t COM_FrameTransfer(uint8_t *receiveBufferPtr, uint16_t * receiveIndexPtr)
{
    uint8_t nextByte = 0U;
    uint16_t readByteCount = 0U;

    com_adapter_result_t processResult = COM_FAIL;

    if ((receiveBufferPtr == NULL) || (receiveIndexPtr == NULL))
    {
        processResult = COM_INVALID_ARG;
    }
    else
    {
        processResult = COM_BUSY;

        // Wait for chip select to transition from low to high
        while (CHIP_SELECT_GetValue() == 0U)
        {
            // Waiting
        }

        // Clears the isReady condition to prep for the next transfer
        if (SERCOM_IsRxReady())
        {
            (void) SERCOM_ByteRead();
        }

        // Wait for chip select to transition from high to low
        while (CHIP_SELECT_GetValue() != 0U)
        {
            // Waiting
        }

        // Blocking read on the first byte
        nextByte = SERCOM_ByteExchange(sendBuffer[bytesSent]);
        bytesSent += 1U;

        // Waits for the peripheral to finish shifting
        while (SERCOM_IsRxReady())
        {
            // Waiting
        }

        // Sets the state of the read command if needed
        if (nextByte == (uint8_t) HOST_WRITE_CODE)
        {
            comState = READ_COMMAND;
        }

        switch (comState)
        {
        case SEND_LENGTH:
            while (0U == CHIP_SELECT_GetValue())
            {
                // Performs an exchange on the length packet bytes
                if (bytesSent < LENGTH_PACKET_SIZE)
                {
                    (void) SERCOM_ByteExchange(sendBuffer[bytesSent]);
                    while (SERCOM_IsRxReady())
                    {
                        // Wait
                    }
                    bytesSent += 1U;
                }
            }
            // Resets the rx buffer
            while (SERCOM_IsRxReady())
            {
                (void) SERCOM_ByteRead();
            }
            // Sets the current state to expect a response send on the next cycle
            comState = SEND_RESPONSE;
            break;
        case SEND_RESPONSE:
            while (0U == CHIP_SELECT_GetValue())
            {
                // Performs an exchange on the response packet bytes
                if (bytesSent != sendLength)
                {
                    (void) SERCOM_ByteExchange(sendBuffer[bytesSent]);
                    while (SERCOM_IsRxReady())
                    {
                        // Wait
                    }
                    bytesSent += 1U;
                }
            }
            // Resets the rx buffer
            while (SERCOM_IsRxReady())
            {
                (void) SERCOM_ByteRead();
            }

            // Sets the state to expect the length packet again
            comState = SEND_LENGTH;
            bytesSent = 0U;

            // Sets the completed status to notify the FTP code.
            processResult = COM_SEND_COMPLETE;
            break;
        case READ_COMMAND:
            while (0U == CHIP_SELECT_GetValue())
            {
                // Puts a byte into the receive buffer if there is space left
                if (SERCOM_IsRxReady())
                {
                    nextByte = SERCOM_ByteRead();

                    if (readByteCount < maxBufferLength)
                    {
                        receiveBufferPtr[readByteCount] = nextByte;
                        readByteCount += 1U;
                    }
                    else
                    {
                        processResult = COM_BUFFER_ERROR;
                        comState = NO_ACTION;
                    }
                }
            }
            if (SERCOM_IsRxReady())
            {
                nextByte = SERCOM_ByteRead();

                if (readByteCount < maxBufferLength)
                {
                    receiveBufferPtr[readByteCount] = nextByte;
                    readByteCount += 1U;
                }
                else
                {
                    processResult = COM_BUFFER_ERROR;
                    comState = NO_ACTION;
                }
            }

            // Resets the sending logic
            sendLength = 0U;
            bytesSent = 0U;
            break;
        default:
            //Do Nothing
            break;
        }

        // Sets the new read index for the FTP code.
        *receiveIndexPtr = readByteCount;

        // Validates a received command
        if (READ_COMMAND == comState)
        {
            calculatedFrameCheck = FrameCheckCalculate(&(receiveBufferPtr[0U]), (readByteCount - FRAME_CHECK_SIZE));

            // Reads FCS from the receive buffer
            uint8_t *startOfWord = &receiveBufferPtr[readByteCount - FRAME_CHECK_SIZE];
            uint16_t frameCheckSequence = 0x0000;
            if (startOfWord != NULL)
            {
                uint8_t * workPtr = startOfWord;
                uint8_t lowByte = *workPtr;
                workPtr++;
                uint8_t highByte = *workPtr;
                frameCheckSequence = (uint16_t) ((((uint16_t) highByte) << 8) | lowByte);
            }

            if (calculatedFrameCheck == frameCheckSequence)
            {
                // Sets the status to execute the command
                processResult = COM_PASS;
            }
            else
            {
                // Sets the error status to craft a retry response
                processResult = COM_TRANSPORT_FAILURE;
            }
            comState = NO_ACTION;
        }
    }

    return processResult;
}

com_adapter_result_t COM_FrameSet(uint8_t *responseBufferPtr, uint16_t responseLength)
{
    com_adapter_result_t status = COM_FAIL;
    uint8_t responseStartSequence[SOP_SEQUENCE_LENGTH] = {0x00, 'R', 'S', 'P'};
    uint8_t lengthStartSequence[SOP_SEQUENCE_LENGTH] = {0x00, 'L', 'E', 'N'};

    if ((responseBufferPtr == NULL) || (responseLength == 0U))
    {
        status = COM_INVALID_ARG;
    }
    else
    {
        // Initializes the transfer logic
        sendLength = 0U;

        // Calculates the data size that the host is required to read
        uint16_t hostReadLength = responseLength + FRAME_CHECK_SIZE;
        calculatedFrameCheck = FrameCheckCalculate((uint8_t *) & hostReadLength, 2U);

        // Loads up the buffer with the length packet data
        for (uint8_t i = 0U; i < SOP_SEQUENCE_LENGTH; i++)
        {
            sendBuffer[sendLength] = lengthStartSequence[i];
            sendLength += 1U;
        }
        sendBuffer[sendLength] = (uint8_t) hostReadLength & 0x00FFU;
        sendLength += 1U;
        sendBuffer[sendLength] = (uint8_t) (hostReadLength >> 8);
        sendLength += 1U;
        sendBuffer[sendLength] = (uint8_t) calculatedFrameCheck & 0x00FFU;
        sendLength += 1U;
        sendBuffer[sendLength] = (uint8_t) (calculatedFrameCheck >> 8);
        sendLength += 1U;

        // Loads up the buffer with the response packet data
        for (uint8_t i = 0U; i < SOP_SEQUENCE_LENGTH; i++)
        {
            sendBuffer[sendLength] = responseStartSequence[i];
            sendLength += 1U;
        }
        // Copies the response data into the COM static buffer starting at the length response offset
        for (uint8_t i = 0U; i < responseLength; i++)
        {
            sendBuffer[sendLength] = responseBufferPtr[i];
            sendLength += 1U;
        }

        // Calculates and sets the FCS of the response data
        calculatedFrameCheck = FrameCheckCalculate(&responseBufferPtr[0], responseLength);
        sendBuffer[sendLength] = (uint8_t) calculatedFrameCheck & 0x00FFU;
        sendLength += 1U;
        sendBuffer[sendLength] = (uint8_t) (calculatedFrameCheck >> 8);
        sendLength += 1U;

        status = COM_PASS;
        comState = SEND_LENGTH;
    }
    return status;
}

com_adapter_result_t COM_Initialize(uint16_t maximumBufferLength)
{
    com_adapter_result_t result = COM_FAIL;
    comState = NO_ACTION;

    if (maximumBufferLength != 0U)
    {
        // Enables SPI
        bool isValid = SERCOM_Open(BOOT_CONFIG);

        if (true == isValid)
        {
            // Sets the static max size of the communication logic
            maxBufferLength = maximumBufferLength;

            result = COM_PASS;
        }
        else
        {
            maxBufferLength = 0U;

            result = COM_FAIL;
        }
    }
    else
    {
        result = COM_INVALID_ARG;
    }

    return result;
}