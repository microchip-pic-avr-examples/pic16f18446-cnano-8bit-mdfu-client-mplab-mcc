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
 * @file        bl_ftp.c
 * @ingroup     8bit_mdfu_client_ftp
 *
 * @brief       This file contains APIs for the Microchip
 *              Device Firmware Upgrade Transfer Protocol(MDFUTP).
 */
#include "bl_ftp.h"
#include "../bl_core.h"
#include "../../com_adapter/com_adapter.h"
#include "../bl_app_verify.h"
#include "../../../../timer/delay.h"

/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def COMMAND_DATA_SIZE
 * Length of the command data field in bytes.
 */
#define COMMAND_DATA_SIZE       (1U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def SEQUENCE_DATA_SIZE
 * Length of the sequence data field in bytes.
 */
#define SEQUENCE_DATA_SIZE      (1U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def FRAME_CHECK_SIZE
 * Length of the FTP frame check field in bytes.
 */
#define FRAME_CHECK_SIZE        (2U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def MAX_RESPONSE_SIZE
 * Length of the largest possible response in bytes.
 */
#define MAX_RESPONSE_SIZE       (19U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def TLV_HEADER_SIZE
 * Length of a Type-Length-Value object header.
 */
#define TLV_HEADER_SIZE         (2U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def MAX_TRANSFER_SIZE
 * Length of the largest possible block transfer in bytes.
 */
#define MAX_TRANSFER_SIZE       (BL_MAX_BUFFER_SIZE + SEQUENCE_DATA_SIZE + COMMAND_DATA_SIZE + FRAME_CHECK_SIZE)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def PACKET_BUFFER_COUNT
 * Number of buffers supported for reception.
 */
#define PACKET_BUFFER_COUNT     (1U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def RETRY_TRANSFER_bm
 * Mask of the Retry bit.
 */
#define RETRY_TRANSFER_bm       (0x40U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def SYNC_TRANSFER_bm
 * Mask of the Sync bit.
 */
#define SYNC_TRANSFER_bm        (0x80U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def SEQUENCE_NUMBER_bm
 * Mask of the sequence number.
 */
#define SEQUENCE_NUMBER_bm      (0x3FU)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def MAX_SEQUENCE_VALUE
 * Maximum value of the sequence field.
 */
#define MAX_SEQUENCE_VALUE      (31U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def FTP_BYTE_INDEX
 * Index of the status or command byte of the FTP.
 * @note This index is valid for both the command byte of the receive buffer and the status byte of the response buffer.
 */
#define FTP_BYTE_INDEX          (1U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def SEQUENCE_BYTE_INDEX
 * Index of the sequence byte of the FTP.
 */
#define SEQUENCE_BYTE_INDEX     (0U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def FILE_DATA_INDEX
 * Index of the file transfer data located inside of the transfer buffer.
 */
#define FILE_DATA_INDEX         (COMMAND_DATA_SIZE + SEQUENCE_DATA_SIZE)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def START_OF_PACKET_BYTE
 * Special character for identifying the start of the frame.
 */
#define START_OF_PACKET_BYTE    (0x56U)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def END_OF_PACKET_BYTE
 * Special character for identifying the end of the frame.
 */
#define END_OF_PACKET_BYTE      (0x9EU)
/**
 * @ingroup 8bit_mdfu_client_ftp
 * @def ESCAPE_BYTE
 * Special character for identifying an escaped byte in the file data.
 */
#define ESCAPE_BYTE             (0xCCU)

typedef enum
{/* cppcheck-suppress misra-c2012-2.4; This rule will not be followed for code clarity. */
    FTP_GET_CLIENT_INFO = 0x01U,
    FTP_START_TRANSFER = 0x02U,
    FTP_WRITE_CHUNK = 0x03U,
    FTP_GET_IMAGE_STATE = 0x04U,
    FTP_END_TRANSFER = 0x05U
} ftp_command_t;

typedef enum
{
    FTP_COMMAND_SUCCESS = 0x01U,
    FTP_COMMAND_NOT_SUPPORTED = 0x02U,
    FTP_COMMAND_NOT_AUTHORIZED = 0x03U,
    FTP_TRANSPORT_FAILURE = 0x04U,
    FTP_ABORT_TRANSFER = 0x05U
} ftp_response_status_t;

typedef enum
{
    FTP_IMAGE_VALID = 0x01U,
    FTP_IMAGE_INVALID = 0x02U
} ftp_image_state_t;

typedef enum
{ /* cppcheck-suppress misra-c2012-2.4; This rule will not be followed for code clarity. */
    FTP_PROTOCOL_VERSION = 0x01U,
    FTP_TRANSFER_PARAMETERS = 0x02U,
    FTP_TIMEOUT_INFO = 0x03U
} tlv_type_code_t;

typedef struct
{
    uint16_t maxPayloadSize;
    uint8_t numberOfPacketBuffers;
} ftp_discovery_data_t;

typedef struct
{
    uint16_t receiveIndex;
    uint16_t bufferLength;
    uint8_t * bufferPtr;
} ftp_process_frame_t;

typedef struct
{
    uint8_t lastSequenceNumber;
    uint8_t currentSequenceNumber;
    uint8_t nextSequenceNumber;
    bool responseRequired;
    bool resendRequired;
    bool isReceiveWindowOpen;
} ftp_parser_helper_t;

typedef struct
{
    uint8_t EscapeCharacter;
    uint8_t StartOfPacketCharacter;
    uint8_t EndOfPacketCharacter;
} ftp_special_characters_t;

typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} ftp_version_data_t;

typedef struct
{
    uint8_t commandCode;
    uint16_t timeoutValue;
} ftp_command_timeout_info_t;

typedef struct
{
    uint8_t dataType;
    uint8_t dataLength;
    uint8_t * valueBuffer;
} ftp_tlv_t;

static uint8_t FTP_RECEIVE_BUFFER[MAX_TRANSFER_SIZE];
static uint8_t FTP_RESPONSE_BUFFER[MAX_RESPONSE_SIZE];
static uint8_t FTP_RETRY_BUFFER[MAX_RESPONSE_SIZE];

static bool isEscapedByte = false;
static bool resetPending = false;

static ftp_process_frame_t ftpRetrySender = {
    .receiveIndex = 0U,
    .bufferLength = MAX_RESPONSE_SIZE,
    .bufferPtr = (uint8_t *) & FTP_RETRY_BUFFER
};

static ftp_process_frame_t ftpCommandReceiver = {
    .receiveIndex = 0U,
    .bufferLength = MAX_TRANSFER_SIZE,
    .bufferPtr = (uint8_t *) & FTP_RECEIVE_BUFFER
};

static ftp_process_frame_t ftpResponseSender = {
    .receiveIndex = 0U,
    .bufferLength = MAX_RESPONSE_SIZE,
    .bufferPtr = (uint8_t *) & FTP_RESPONSE_BUFFER
};
static ftp_parser_helper_t ftpHelper = {
    .lastSequenceNumber = 0U,
    .currentSequenceNumber = 0U,
    .nextSequenceNumber = 1U,
    .resendRequired = false,
    .responseRequired = false,
    .isReceiveWindowOpen = false
};

/* cppcheck-suppress misra-c2012-8.9; This rule will not be followed for code clarity. */
static ftp_discovery_data_t discoveryData = {
    .numberOfPacketBuffers = PACKET_BUFFER_COUNT,
    .maxPayloadSize = (BL_MAX_BUFFER_SIZE)
};

/* cppcheck-suppress misra-c2012-8.9; This rule will not be followed for code clarity. */
static ftp_version_data_t ftpVersionData = {
    .major = 0x00,
    .minor = 0x01,
    .patch = 0x00,
};

static ftp_command_timeout_info_t generalCommandTimeoutData = {
    .commandCode = 0x00U, // General Command Timeout Code
    .timeoutValue = 0x0064U // 0x64 -> 100 dec -> 10 Seconds
};

/* cppcheck-suppress misra-c2012-8.9; This rule will not be followed for code clarity. */
static ftp_tlv_t ftpVersionTLVData = {
    .dataType = FTP_PROTOCOL_VERSION,
    .dataLength = 0x03U,
    .valueBuffer = (uint8_t *) & ftpVersionData
};

/* cppcheck-suppress misra-c2012-8.9; This rule will not be followed for code clarity. */
static ftp_tlv_t ftpTransferParametersTLVData = {
    .dataType = FTP_TRANSFER_PARAMETERS,
    .dataLength = 0x03U,
    .valueBuffer = (uint8_t *) & discoveryData
};

/* cppcheck-suppress misra-c2012-8.9; This rule will not be followed for code clarity. */
static ftp_tlv_t ftpTimeoutTLVData = {
    .dataType = FTP_TIMEOUT_INFO,
    .dataLength = 0x03U,
    .valueBuffer = (uint8_t *) & generalCommandTimeoutData
};

/* cppcheck-suppress misra-c2012-8.9; This rule will not be followed for code clarity. */
static ftp_special_characters_t ftpSpecialCharacters = {
    .EscapeCharacter = ESCAPE_BYTE,
    .StartOfPacketCharacter = START_OF_PACKET_BYTE,
    .EndOfPacketCharacter = END_OF_PACKET_BYTE
};

static void BL_CheckDeviceReset(void);
static void FTP_ResetParserData(void);
static uint16_t FTP_CalculateFrameCheck(ftp_process_frame_t * ftpDataPtr);
static bool FTP_ValidateCommandPacket(void);
static bool FTP_ValidateSequenceNumber(void);
static uint16_t FTP_PeekUint16(uint8_t * startOfWord);
static bl_result_t FTP_ExecuteOperationalBlock(void);
static void FTP_SendTransferFrame(ftp_process_frame_t * sendObject);
static void FTP_SetResponse(
                            ftp_process_frame_t * bufferPtr,
                            uint8_t * responsePayload,
                            ftp_response_status_t responseStatus,
                            uint8_t sequenceByte,
                            uint16_t responsePayloadLength
                            );
static void FTP_SetClientInfoResponse(ftp_process_frame_t * responseData);
static uint8_t FTP_AppendTLVToResponse(uint8_t * dataBufferStart, ftp_tlv_t tlvData);

bl_result_t FTP_ProcessTask(void)
{
    BL_CheckDeviceReset();

    bl_result_t processResult = BL_FAIL;
    uint8_t nextByte = 0U;
    com_adapter_result_t communicationStatus = COM_Receive(&nextByte, 1U);

    if (communicationStatus == COM_PASS)
    {
        processResult = FTP_ProcessNextByte(nextByte);
    }
    else
    {
        processResult = BL_ERROR_COMMUNICATION_FAIL;
    }

    return processResult;
}

/* cppcheck-suppress misra-c2012-8.7; This is being ignored to allow unit testing in the development repository. */
bl_result_t FTP_ProcessNextByte(uint8_t nextByte)
{
    bl_result_t processResult = BL_FAIL;
    uint8_t processedByte = nextByte;

    if (processedByte == START_OF_PACKET_BYTE)
    {
        // Open the buffer window
        ftpHelper.isReceiveWindowOpen = true;
        // Reset all parser data and get ready for the next block
        FTP_ResetParserData();
    }
    else if (ftpHelper.isReceiveWindowOpen)
    {
        if (processedByte == END_OF_PACKET_BYTE)
        {
            // Close the buffer window
            ftpHelper.isReceiveWindowOpen = false;
            if (FTP_ValidateCommandPacket())
            {
                // Call execution to handle the rest of the command processes
                processResult = FTP_ExecuteOperationalBlock();
                ftpHelper.responseRequired = true;
            }
            else
            {
                processResult = BL_ERROR_FRAME_VALIDATION_FAIL;
            }
        }
        else if (processedByte == ESCAPE_BYTE)
        {
            isEscapedByte = true;
        }
        else
        {
            // If escape was flagged perform the bit flip to correct the byte
            if (isEscapedByte)
            {
                processedByte = ~processedByte;
                isEscapedByte = false;
            }

            // Route the byte into the transfer buffer
            if (ftpCommandReceiver.receiveIndex < MAX_TRANSFER_SIZE)
            {
                ftpCommandReceiver.bufferPtr[ftpCommandReceiver.receiveIndex] = processedByte;
                ftpCommandReceiver.receiveIndex++;
            }
            else
            {
                // Close the buffer window
                ftpHelper.isReceiveWindowOpen = false;
                processResult = BL_ERROR_BUFFER_OVERLOAD;
                FTP_SetResponse(&ftpRetrySender, NULL, FTP_TRANSPORT_FAILURE, ftpHelper.nextSequenceNumber ^ RETRY_TRANSFER_bm, 0U);
                FTP_SendTransferFrame(&ftpRetrySender);
            }
        }

        if (ftpHelper.responseRequired)
        {
            FTP_SendTransferFrame(&ftpResponseSender);
            ftpHelper.responseRequired = false;
        }
        else
        {
            // Still Processing
        }
    }
    else
    {
        // Do nothing.
    }

    return processResult;
}

static bool FTP_ValidateCommandPacket(void)
{
    bool isValid = false;
    uint16_t receivedFrameCheck = FTP_PeekUint16((uint8_t *) &(ftpCommandReceiver.bufferPtr[ftpCommandReceiver.receiveIndex - FRAME_CHECK_SIZE]));

    // Verify integrity of data transfer before checking anything
    if (FTP_CalculateFrameCheck(&ftpCommandReceiver) == receivedFrameCheck)
    {
        //Validate the sequence number only after the integrity has been checked
        if (FTP_ValidateSequenceNumber())
        {
            isValid = true;
        }
    }
    else
    {
        ftpHelper.resendRequired = true;
    }

    if (ftpHelper.resendRequired)
    {
        // Frame check failed. Retry the transmission on the sequence we should be expecting
        FTP_SetResponse(&ftpRetrySender, NULL, FTP_TRANSPORT_FAILURE, ftpHelper.nextSequenceNumber ^ RETRY_TRANSFER_bm, 0U);
        FTP_SendTransferFrame(&ftpRetrySender);
        ftpHelper.resendRequired = false;
    }

    return isValid;
}

static bool FTP_ValidateSequenceNumber(void)
{
    bool isValidSequenceNum = false;

    // Get the sequence Number info
    ftpHelper.currentSequenceNumber = ftpCommandReceiver.bufferPtr[SEQUENCE_BYTE_INDEX] & SEQUENCE_NUMBER_bm;
    bool syncRequested = ftpCommandReceiver.bufferPtr[SEQUENCE_BYTE_INDEX] & SYNC_TRANSFER_bm;

    // Sequence Sync Check
    if (syncRequested)
    {
        // If the sync field is check synchronize the buffer and execute the command
        isValidSequenceNum = true;
        ftpHelper.lastSequenceNumber = ftpHelper.currentSequenceNumber;
        ftpHelper.nextSequenceNumber = (ftpHelper.currentSequenceNumber + 1U) & MAX_SEQUENCE_VALUE;
    }
        // Else if the packet is next packet expected, execute the packet
    else if (ftpHelper.currentSequenceNumber == ftpHelper.nextSequenceNumber)
    {
        isValidSequenceNum = true;
        ftpHelper.lastSequenceNumber = ftpHelper.currentSequenceNumber;
        ftpHelper.nextSequenceNumber = (ftpHelper.currentSequenceNumber + 1U) & MAX_SEQUENCE_VALUE;
    }
    else if (ftpHelper.currentSequenceNumber == ftpHelper.lastSequenceNumber)
    {
        // Don't execute the command but resend the response that is already in the buffer
        isValidSequenceNum = false;
        ftpHelper.responseRequired = true;
    }
        // Else send a resend request for the next packet sequence number
    else
    {
        isValidSequenceNum = false;
        // Frame check failed. Retry the transmission on the sequence we should be expecting
        ftpHelper.resendRequired = true;
    }
    return isValidSequenceNum;
}

static bl_result_t FTP_ExecuteOperationalBlock(void)
{
    bl_result_t processResult = BL_BUSY;

    switch (ftpCommandReceiver.bufferPtr[FTP_BYTE_INDEX])
    {
    case FTP_GET_CLIENT_INFO:
        FTP_SetClientInfoResponse(&ftpResponseSender);
        processResult = BL_PASS;
        break;

    case FTP_GET_IMAGE_STATE:
        processResult = BL_VerifyImage();
        ftp_image_state_t isImageValid = (processResult == BL_PASS) ? FTP_IMAGE_VALID : FTP_IMAGE_INVALID;
        FTP_SetResponse(&ftpResponseSender, (uint8_t *) & isImageValid, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 1U);
        break;

    case FTP_START_TRANSFER:
        processResult = BL_PASS;
        (void) BL_Initialize();
        FTP_SetResponse(&ftpResponseSender, NULL, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 0U);
        break;

    case FTP_WRITE_CHUNK:
        processResult = BL_ProcessBootBuffer(&(ftpCommandReceiver.bufferPtr[FILE_DATA_INDEX]), ftpCommandReceiver.receiveIndex);
        if (processResult == BL_PASS)
        {
            FTP_SetResponse(&ftpResponseSender, NULL, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 0U);
        }
        else
        {
            FTP_SetResponse(&ftpResponseSender, NULL, FTP_ABORT_TRANSFER, ftpHelper.currentSequenceNumber, 0U);
        }
        break;

    case FTP_END_TRANSFER:
        FTP_SetResponse(&ftpResponseSender, NULL, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 0U);
        resetPending = true;
        processResult = BL_PASS;
        break;

    default:
        processResult = BL_ERROR_UNKNOWN_COMMAND;
        FTP_SetResponse(&ftpResponseSender, NULL, FTP_COMMAND_NOT_SUPPORTED, ftpHelper.currentSequenceNumber, 0U);
        break;
    }

    return processResult;
}

static void FTP_ResetParserData(void)
{
    uint16_t i = 0;

    isEscapedByte = false;
    ftpCommandReceiver.receiveIndex = 0;

    // Clear the transfer buffer
    for (i = 0; i != MAX_TRANSFER_SIZE; i++)
    {
        ftpCommandReceiver.bufferPtr[i] = 0x00U;
    }
}

static void BL_CheckDeviceReset(void)
{
    if (resetPending == true)
    {
        DELAY_milliseconds(25U);
#if defined(PIC_ARCH)
        RESET();
#elif defined(AVR_ARCH)
        ccp_write_io((void *) &RSTCTRL.SWRR, RESET_BIT_MASK);
#endif
    }
}

static uint16_t FTP_PeekUint16(uint8_t * startOfWord)
{
    uint16_t wordValue = 0x0000;
    if (startOfWord != NULL)
    {
        uint8_t * workPtr = startOfWord;
        uint8_t lowByte = *workPtr;
        workPtr++;
        uint8_t highByte = *workPtr;
        wordValue = (uint16_t) ((((uint16_t) highByte) << 8) | lowByte);
    }

    return wordValue;
}

static uint16_t FTP_CalculateFrameCheck(ftp_process_frame_t * ftpData)
{
    uint16_t numBytesChecksummed = 0;
    uint16_t checksum = 0;

    while (numBytesChecksummed < (ftpData->receiveIndex - FRAME_CHECK_SIZE))
    {
        if ((numBytesChecksummed % 2U) == 0U)
        {
            checksum += ((uint16_t) (ftpData->bufferPtr[numBytesChecksummed]));
        }
        else
        {

            checksum += (((uint16_t) (ftpData->bufferPtr[numBytesChecksummed])) << 8);
        }
        numBytesChecksummed++;
    }
    return ~checksum;
}

static void FTP_SendTransferFrame(ftp_process_frame_t * sendObject)
{
    uint16_t sentByteCount = 0U;

    (void) COM_Send(&(ftpSpecialCharacters.StartOfPacketCharacter), 1U);

    while (sentByteCount < sendObject->receiveIndex)
    {
        uint8_t nextByte = sendObject->bufferPtr[sentByteCount];

        if ((nextByte == START_OF_PACKET_BYTE) || (nextByte == END_OF_PACKET_BYTE) || (nextByte == ESCAPE_BYTE))
        {

            (void) COM_Send(&(ftpSpecialCharacters.EscapeCharacter), 1U);
            nextByte = ~nextByte;
        }

        (void) COM_Send((uint8_t *) & nextByte, 1U);

        sentByteCount++;
    }

    (void) COM_Send((uint8_t *)& (ftpSpecialCharacters.EndOfPacketCharacter), 1U);
}

static void FTP_SetResponse(ftp_process_frame_t * responseData, uint8_t * responsePayload, ftp_response_status_t responseStatus, uint8_t sequenceByte, uint16_t responsePayloadLength)
{
    // Calculate the length of the response
    responseData->receiveIndex = (responsePayloadLength + SEQUENCE_DATA_SIZE + COMMAND_DATA_SIZE + FRAME_CHECK_SIZE);

    // Update The Sequence Value
    responseData->bufferPtr[SEQUENCE_BYTE_INDEX] = sequenceByte;

    // Status
    responseData->bufferPtr[FTP_BYTE_INDEX] = responseStatus;

    // Data Bytes
    for (uint8_t i = 0; i < responsePayloadLength; i++)
    {
        responseData->bufferPtr[(FILE_DATA_INDEX + i)] = responsePayload[i];
    }

    // Integrity Check
    uint16_t frameCheck = FTP_CalculateFrameCheck(responseData);

    // Set frame check data at the end of the buffer
    responseData->bufferPtr[responseData->receiveIndex - 2U] = (uint8_t) (frameCheck & (uint16_t) 0x00FFU);
    responseData->bufferPtr[responseData->receiveIndex - 1U] = (uint8_t) ((frameCheck & 0xFF00U) >> 8);
}

static uint8_t FTP_AppendTLVToResponse(uint8_t * dataBufferStart, ftp_tlv_t tlvData)
{
    dataBufferStart[0] = tlvData.dataType;
    dataBufferStart[1] = tlvData.dataLength;

    // Data Bytes
    for (uint8_t i = 0; i < (tlvData.dataLength + TLV_HEADER_SIZE); i++)
    {
        dataBufferStart[(TLV_HEADER_SIZE + i)] = tlvData.valueBuffer[i];
    }

    return tlvData.dataLength + TLV_HEADER_SIZE;
}

static void FTP_SetClientInfoResponse(ftp_process_frame_t * responseData)
{
    // Calculate and set the response length
    responseData->receiveIndex = (
            ftpVersionTLVData.dataLength + TLV_HEADER_SIZE +
            ftpTransferParametersTLVData.dataLength + TLV_HEADER_SIZE +
            ftpTimeoutTLVData.dataLength + TLV_HEADER_SIZE +
            SEQUENCE_DATA_SIZE +
            COMMAND_DATA_SIZE +
            FRAME_CHECK_SIZE
            );

    // Update The Sequence Value
    responseData->bufferPtr[SEQUENCE_BYTE_INDEX] = ftpHelper.currentSequenceNumber;

    // Update Status
    responseData->bufferPtr[FTP_BYTE_INDEX] = FTP_COMMAND_SUCCESS;

    uint16_t fileDataOffset = FILE_DATA_INDEX;

    // push each TLV Byte Stream to the buffer
    fileDataOffset += FTP_AppendTLVToResponse(&(responseData->bufferPtr[fileDataOffset]), ftpVersionTLVData);
    fileDataOffset += FTP_AppendTLVToResponse(&(responseData->bufferPtr[fileDataOffset]), ftpTransferParametersTLVData);
    fileDataOffset += FTP_AppendTLVToResponse(&(responseData->bufferPtr[fileDataOffset]), ftpTimeoutTLVData);

    // Integrity Check
    uint16_t frameCheck = FTP_CalculateFrameCheck(responseData);

    // Set frame check data at the end of the buffer
    responseData->bufferPtr[responseData->receiveIndex - 2U] = (uint8_t) (frameCheck & (uint16_t) 0x00FFU);
    responseData->bufferPtr[responseData->receiveIndex - 1U] = (uint8_t) ((frameCheck & 0xFF00U) >> 8);
}