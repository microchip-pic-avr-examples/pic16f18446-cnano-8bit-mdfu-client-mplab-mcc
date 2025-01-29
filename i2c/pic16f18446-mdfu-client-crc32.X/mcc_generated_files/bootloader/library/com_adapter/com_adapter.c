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
 * @brief   This file contains the implementation of the communication adapter layer using I2C.
 * @ingroup com_adapter
 */

#include "com_adapter.h"
#include "../../../../mcc_generated_files/system/system.h"
#include "../../../timer/delay.h"

/**
 * @ingroup com_adapter
 * @def LENGTH_PREFIX_SIZE
 * Contains the size of the length prefix field.
 */
#define LENGTH_PREFIX_SIZE      (1U)

/**
 * @ingroup com_adapter
 * @def RESPONSE_PREFIX_SIZE
 * Contains the size of the response prefix field.
 */
#define RESPONSE_PREFIX_SIZE    (1U)

/**
 * @ingroup com_adapter
 * @def LENGTH_FIELD_SIZE
 * Contains the size of the length data field.
 */
#define LENGTH_FIELD_SIZE (2U)

/**
 * @ingroup com_adapter
 * @def MAX_RESPONSE_DATA_FIELD
 * Contains the maximum size of a response.
 */
#define MAX_RESPONSE_DATA_FIELD (25U)

/**
 * @ingroup com_adapter
 * @def RESPONSE_OFFSET
 * Contains the offset of the response data buffer where actual client response starts after the length, checksum and prefix fields.
 */
#define RESPONSE_OFFSET (LENGTH_PREFIX_SIZE + LENGTH_FIELD_SIZE + FRAME_CHECK_SIZE + RESPONSE_PREFIX_SIZE)

/**
 * @ingroup com_adapter
 * @enum com_transfer_state_t
 * @brief Contains codes for the return values for the transfer state of the bootloader communication adapter layer.
 * @var com_adapter_result_t: NOTHING_TO_SEND
 * 0U - com_adapter has nothing to send
 * @var com_adapter_result_t: SENDING_LENGTH
 * 1U - com_adapter is ready to send the response length
 * @var com_adapter_result_t: SENDING_RESPONSE
 * 2U - com_adapter is ready to send the response data
 */
typedef enum {
    NOTHING_TO_SEND  = 0U, 
    SENDING_LENGTH   = 1U,
    SENDING_RESPONSE = 2U,
} com_transfer_state_t;

static volatile com_transfer_state_t comResponseTransferState = NOTHING_TO_SEND;
static volatile bool isCommandReadyToProcess = false;
static volatile bool areTooManyBytesInCommand = false;

static volatile uint16_t maxBufferLength = 0; 
static volatile uint16_t comResponseBufferIndex = 0U;
static uint8_t comResponseBuffer[MAX_RESPONSE_DATA_FIELD + RESPONSE_OFFSET];
static uint8_t *comReceiveBuffer = NULL;
static uint16_t *comReceiveBufferIndex = NULL;  
static volatile com_adapter_result_t comStatus;

static bool SERCOM_EventHandler(i2c_client_transfer_event_t event);

com_adapter_result_t COM_Initialize(uint16_t maximumBufferLength)
{
    com_adapter_result_t result = COM_FAIL;

    if (0U != maximumBufferLength)
    {
        maxBufferLength = maximumBufferLength;
        SERCOM_CallbackRegister(SERCOM_EventHandler);
        result = COM_PASS;
    }
    else
    {
        result = COM_INVALID_ARG;
    }

    return result;
}

static bool SERCOM_EventHandler(i2c_client_transfer_event_t event)
{
    bool result = false;
    uint8_t nextByte = 0;
    i2c_client_error_t errorState = I2C_CLIENT_ERROR_NONE;
    static volatile i2c_client_transfer_dir_t transferDirection;
    static volatile bool wasTransactionAcknowledged;    
    
    switch(event)
    {
    case I2C_CLIENT_TRANSFER_EVENT_ADDR_MATCH:
        
        transferDirection = SERCOM_TransferDirGet(); 
        
        // NAK all incoming transactions if the com_adapter is busy processing a previous command
        if (false == isCommandReadyToProcess) 
        {
            if (I2C_CLIENT_TRANSFER_DIR_WRITE == transferDirection) 
            {
                result = true;
                wasTransactionAcknowledged = true;
                *comReceiveBufferIndex = 0;
                areTooManyBytesInCommand = false;
            }
            else
            {
                if (NOTHING_TO_SEND == comResponseTransferState) 
                {
                    result = true;
                    wasTransactionAcknowledged = false; 
                }
                else 
                { 
                    result = true;
                    wasTransactionAcknowledged = true;
                }
            }
        }
        else
        {
            result = false;
            wasTransactionAcknowledged = false;
        }  
        break;

    case I2C_CLIENT_TRANSFER_EVENT_RX_READY:
        
        nextByte = SERCOM_ReadByte();
        result = true; 
 
        if (!isCommandReadyToProcess) 
        {
            // Add byte to the buffer if there is space in the buffer
            if (*comReceiveBufferIndex < maxBufferLength)
            {
                comReceiveBuffer[*comReceiveBufferIndex] = nextByte;
                (*comReceiveBufferIndex)++;   
            }
            else
            { 
                areTooManyBytesInCommand = true;  
            }
        }
        break;

    case I2C_CLIENT_TRANSFER_EVENT_TX_READY:
        result = true; 
        SERCOM_WriteByte(comResponseBuffer[comResponseBufferIndex++]);
        break;

    case I2C_CLIENT_TRANSFER_EVENT_STOP_BIT_RECEIVED:
        if(wasTransactionAcknowledged) 
        {
            // Set the com response state to nothing if the transfer direction is host write
            if (I2C_CLIENT_TRANSFER_DIR_WRITE == transferDirection)
            {              
                isCommandReadyToProcess = true;                
                comResponseTransferState = NOTHING_TO_SEND;
            }
            else 
            { 
                if (SENDING_LENGTH == comResponseTransferState)
                {
                    comResponseTransferState = SENDING_RESPONSE;
                }
                else if (SENDING_RESPONSE == comResponseTransferState)
                {
                    comResponseTransferState = NOTHING_TO_SEND; 
                    comStatus = COM_SEND_COMPLETE;
                }
                else
                {
                    //do nothing
                }
            } 
        }
        else
        {
            // do nothing
        }
        break;

    case I2C_CLIENT_TRANSFER_EVENT_ERROR:
        errorState = SERCOM_ErrorGet();
                
        if (I2C_CLIENT_ERROR_BUS_COLLISION == errorState)
        {
            comStatus = COM_TRANSPORT_FAILURE;
        }
        else if (I2C_CLIENT_ERROR_WRITE_COLLISION == errorState)
        {
            comStatus = COM_TRANSPORT_FAILURE;
        }
        else if (I2C_CLIENT_ERROR_RECEIVE_OVERFLOW == errorState)
        {
            comStatus = COM_BUFFER_ERROR;
        }
        else if (I2C_CLIENT_ERROR_TRANSMIT_UNDERFLOW == errorState)
        {
            comStatus = COM_TRANSPORT_FAILURE;
        }
        else if (I2C_CLIENT_ERROR_READ_UNDERFLOW == errorState)
        {
            comStatus = COM_BUFFER_ERROR;
        }
        else
        {
            //do nothing
        }
        break;
    default:
        break;
    }
    return result;
}

static uint16_t FrameChecksumCalculate(uint8_t * ftpData,uint16_t bufferLength)
{
    uint16_t numBytesChecksummed = 0;
    uint16_t checksum = 0;

    while (numBytesChecksummed < (bufferLength))
    {
        if ((numBytesChecksummed % 2U) == 0U)
        {
            checksum += ((uint16_t)(ftpData[numBytesChecksummed]));
        }
        else
        {
            checksum += (((uint16_t)(ftpData[numBytesChecksummed])) << 8);
        }
        numBytesChecksummed++;
    }

    return ~checksum;
}

com_adapter_result_t COM_FrameTransfer(uint8_t *ftpReceiveBufferPtr,uint16_t *ftpReceiveBufferIndexPtr)
{
    com_adapter_result_t result = COM_FAIL;
    comReceiveBuffer = ftpReceiveBufferPtr;
    comReceiveBufferIndex = ftpReceiveBufferIndexPtr;

    if ((NULL == ftpReceiveBufferPtr) || (NULL == ftpReceiveBufferIndexPtr))
    {
        result = COM_INVALID_ARG;
    }
    else
    {
        if (isCommandReadyToProcess == true)
        {
            // Set the status to buffer error when the received data length exceeds the max buffer size
            if (areTooManyBytesInCommand) 
            { 
                areTooManyBytesInCommand = false;
                result = COM_BUFFER_ERROR;
            }
            else 
            {

                // Calculate the frame check sequence on the received packet 
                uint16_t calcuatedFrameChecksum = FrameChecksumCalculate(ftpReceiveBufferPtr,*ftpReceiveBufferIndexPtr - FRAME_CHECK_SIZE); 

                // Extract the last two bytes from the packet
                uint8_t *startOfWord = &ftpReceiveBufferPtr[*ftpReceiveBufferIndexPtr - FRAME_CHECK_SIZE];
                uint16_t frameCheckSequence = 0x0000;

                // Form the received FCS in the right order
                if (NULL != startOfWord)
                {
                    uint8_t *workPtr = startOfWord;
                    uint8_t lowByte = *workPtr;
                    workPtr++;
                    uint8_t highByte = *workPtr;
                    frameCheckSequence = (uint16_t)((((uint16_t)highByte) << 8) | lowByte);
                }

                // Check if calculated checksum and received checksum are equal
                if (calcuatedFrameChecksum == frameCheckSequence)
                {
                    result = COM_PASS;
                }
                else
                {
                    // Set the status to transport failure if checksum mismatch occurs
                    result = COM_TRANSPORT_FAILURE;
                }
            } 
        } 
        else
        {
            result = comStatus;
        }
    }

    return result;
}

com_adapter_result_t COM_FrameSet(uint8_t *responseBufferPtr, uint16_t responseLength)
{
    com_adapter_result_t result = COM_FAIL;

    uint16_t sendingLength = responseLength + FRAME_CHECK_SIZE;
    uint16_t sendingLengthChecksum;
    uint16_t dataChecksum;

    if ((NULL == responseBufferPtr) || (0U == responseLength))
    {
        result = COM_INVALID_ARG;
    }
    else
    {
        sendingLengthChecksum = FrameChecksumCalculate((uint8_t *) & sendingLength, 2U);
        dataChecksum = FrameChecksumCalculate(&(responseBufferPtr[0]),responseLength);
        
        // Copy the length prefix
        comResponseBuffer[0] = 'L';

        // Copy sending length into the response buffer
        comResponseBuffer[1] = (uint8_t)sendingLength & 0x00FFU;

        // Copy the checksum on sending length into the response buffer
        comResponseBuffer[2] = (uint8_t)(sendingLength >> 8);

        // Copy the data into the response buffer
        comResponseBuffer[3] = (uint8_t)sendingLengthChecksum & 0x00FFU;

        // Copy the checksum on the data into the response buffer
        comResponseBuffer[4] = (uint8_t)(sendingLengthChecksum >> 8);

        // Copy the response prefix
        comResponseBuffer[5] = 'R';
        
        // Copy the response into the COM static buffer and set length
        for (uint16_t i = RESPONSE_OFFSET;i < (responseLength + RESPONSE_OFFSET);i++)
        {
            comResponseBuffer[i] = responseBufferPtr[i - RESPONSE_OFFSET];
        }

        comResponseBuffer[responseLength + RESPONSE_OFFSET] = (uint8_t)dataChecksum & 0x00FFU;
        comResponseBuffer[responseLength + RESPONSE_OFFSET + 1U] = (uint8_t)(dataChecksum >> 8);
        
        comResponseBufferIndex = 0U;
        result = COM_PASS;
    }
    // Set the state to sending length when the comResponseBuffer is ready
    comResponseTransferState = SENDING_LENGTH;
    isCommandReadyToProcess = false;
    
    return result;
}
