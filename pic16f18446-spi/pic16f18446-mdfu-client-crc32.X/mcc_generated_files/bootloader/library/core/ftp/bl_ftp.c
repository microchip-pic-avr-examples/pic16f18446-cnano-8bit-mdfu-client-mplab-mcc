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
 * @file        bl_ftp.c
 * @ingroup     mdfu_client_8bit_ftp
 *
 * @brief       This file contains APIs for the Microchip
 *              Device Firmware Upgrade Transfer Protocol (MDFUTP).
 */

/**@misradeviation{@advisory, 2.4} This rule will not be followed for code clarity.
 */
/**@misradeviation{@advisory, 2.3} This rule will not be followed for code clarity.
 */

#include "bl_ftp.h"
#include "../bl_core.h"
#include "../../com_adapter/com_adapter.h"
#include "../bl_app_verify.h"
#include "../../../../timer/delay.h"

/**
 * @ingroup mdfu_client_8bit_ftp
 * @def COMMAND_DATA_SIZE
 * Length of the command data field in bytes.
 */
#define COMMAND_DATA_SIZE       (1U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def SEQUENCE_DATA_SIZE
 * Length of the sequence data field in bytes.
 */
#define SEQUENCE_DATA_SIZE      (1U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def MAX_RESPONSE_SIZE
 * Length of the largest possible response in bytes.
 */
#define MAX_RESPONSE_SIZE       (25U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def TLV_HEADER_SIZE
 * Length of a Type-Length-Value object header.
 */
#define TLV_HEADER_SIZE         (2U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def MAX_TRANSFER_SIZE
 * Length of the largest possible block transfer in bytes.
 */
#define MAX_TRANSFER_SIZE       (BL_MAX_BUFFER_SIZE + SEQUENCE_DATA_SIZE + COMMAND_DATA_SIZE + COM_FRAME_BYTE_COUNT)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def PACKET_BUFFER_COUNT
 * Number of buffers supported for reception.
 */
#define PACKET_BUFFER_COUNT     (1U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def RETRY_TRANSFER_bm
 * Mask of the Retry bit.
 */
#define RETRY_TRANSFER_bm       (0x40U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def SYNC_TRANSFER_bm
 * Mask of the Sync bit.
 */
#define SYNC_TRANSFER_bm        (0x80U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def SEQUENCE_NUMBER_bm
 * Mask of the sequence number.
 */
#define SEQUENCE_NUMBER_bm      (0x3FU)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def MAX_SEQUENCE_VALUE
 * Maximum value of the sequence field.
 */
#define MAX_SEQUENCE_VALUE      (31U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def FTP_BYTE_INDEX
 * Index of the status or command byte of the FTP.
 * @note This index is valid for both the command byte of the receive buffer and the status byte of the response buffer.
 */
#define FTP_BYTE_INDEX          (1U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def SEQUENCE_BYTE_INDEX
 * Index of the sequence byte of the FTP.
 */
#define SEQUENCE_BYTE_INDEX     (0U)
/**
 * @ingroup mdfu_client_8bit_ftp
 * @def FILE_DATA_INDEX
 * Index of the file transfer data located inside of the transfer buffer.
 */
#define FILE_DATA_INDEX         (COMMAND_DATA_SIZE + SEQUENCE_DATA_SIZE)

/* cppcheck-suppress misra-c2012-2.3 */
typedef enum
{/* cppcheck-suppress misra-c2012-2.4 */
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
    FTP_GENERIC_ERROR = 0x00U,
    FTP_INVALID_FILE_ERROR = 0x01U,
    FTP_INVALID_DEVICE_ID_ERROR = 0x02U,
    FTP_ADDRESS_ERROR = 0x03U,
    FTP_ERASE_ERROR = 0x04U,
    FTP_WRITE_ERROR = 0x05U,
    FTP_READ_ERROR = 0x06U,
    FTP_APP_VERSION_ERROR = 0x07U,
} ftp_abort_code_t;

typedef enum
{
    FTP_INTEGRITY_CHECK_ERROR = 0x00U,
    FTP_COMMAND_TOO_LONG_ERROR = 0x01U,
} ftp_transport_failure_code_t;

typedef enum
{
    FTP_IMAGE_VALID = 0x01U,
    FTP_IMAGE_INVALID = 0x02U
} ftp_image_state_t;

/* cppcheck-suppress misra-c2012-2.3 */
typedef enum
{ /* cppcheck-suppress misra-c2012-2.4 */
    FTP_PROTOCOL_VERSION = 0x01U,
    FTP_TRANSFER_PARAMETERS = 0x02U,
    FTP_TIMEOUT_INFO = 0x03U,
    FTP_MIN_INTER_MESSAGE_DELAY_INFO = 0x04U,
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
} ftp_parser_helper_t;

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

static bool resetPending = false;
static bool isComBusy = false;
static uint32_t minimumInterMessageDelayData = (uint32_t) 0x000F4240U; // 1,000,000 nanoseconds => 1 milliseconds

/* cppcheck-suppress misra-c2012-8.9 */
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
};

static ftp_discovery_data_t discoveryData = {
    .numberOfPacketBuffers = PACKET_BUFFER_COUNT,
    .maxPayloadSize = (BL_MAX_BUFFER_SIZE)
};

static ftp_version_data_t ftpVersionData = {
    .major = 0x01,
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
static ftp_tlv_t ftpMinInterMessageDelayTLVData = {
    .dataType = FTP_MIN_INTER_MESSAGE_DELAY_INFO,
    .dataLength = 0x04U,
    .valueBuffer = (uint8_t *) & minimumInterMessageDelayData
};

static void DeviceResetCheck(void);
static void ParserDataReset(void);
static bool SequenceNumberValidate(void);
static bl_result_t OperationalBlockExecute(void);
static void ResponseSet(
                        ftp_process_frame_t * bufferPtr,
                        uint8_t * responsePayload,
                        ftp_response_status_t responseStatus,
                        uint8_t sequenceByte,
                        uint16_t responsePayloadLength
                        );
static void ClientInfoResponseSet(ftp_process_frame_t * responseData);
static uint8_t TLVAppend(uint8_t * dataBufferStart, ftp_tlv_t tlvData);
static ftp_abort_code_t AbortCodeGet(bl_result_t targetStatus);
static ftp_transport_failure_code_t TransportFailureCodeGet(bl_result_t targetStatus);

bl_result_t FTP_Task(void)
{
    if (!isComBusy)
    {
        DeviceResetCheck();
    }
    bl_result_t processResult = BL_FAIL;
    com_adapter_result_t comResult = COM_FAIL;

    // Call the command to load the buffer up with the current receive count
    comResult = COM_FrameTransfer((uint8_t *)&ftpCommandReceiver.bufferPtr[0], &(ftpCommandReceiver.receiveIndex));

    if (comResult == COM_BUFFER_ERROR)
    {
        processResult = BL_ERROR_BUFFER_OVERLOAD;
        ftpHelper.resendRequired = true;
        ParserDataReset();
    }
    else if (comResult == COM_PASS)
    {
        if (SequenceNumberValidate())
        {
            // Call execution to handle the rest of the command processes
            processResult = OperationalBlockExecute();
            ftpHelper.responseRequired = true;
        }
        else
        {
            processResult = BL_ERROR_FRAME_VALIDATION_FAIL;
        }
        ParserDataReset();
    }
    else if (comResult == COM_TRANSPORT_FAILURE)
    {
        processResult = BL_ERROR_FRAME_VALIDATION_FAIL;
        // Frame check failed. Retry the transmission on the sequence we should be expecting
        ftpHelper.resendRequired = true;
    }
    else if (comResult == COM_BUSY)
    {
        // Still Loading
        processResult = BL_BUSY;
    }
#ifdef MULTI_STAGE_RESPONSE
    else if (comResult == COM_SEND_COMPLETE)
    {
        // flip the busy flag to allow resets
        isComBusy = false;
        processResult = BL_BUSY;
    }
#endif
    else
    {
        processResult = BL_ERROR_COMMUNICATION_FAIL;
    }

    if (ftpHelper.resendRequired)
    {
        ftp_transport_failure_code_t transportCode = TransportFailureCodeGet(processResult);
        ResponseSet(&ftpRetrySender, (uint8_t *) & transportCode, FTP_TRANSPORT_FAILURE, ftpHelper.nextSequenceNumber ^ RETRY_TRANSFER_bm, 1U);

        comResult = COM_FrameSet((uint8_t *) & ftpRetrySender.bufferPtr[0], ftpRetrySender.receiveIndex);

        if (COM_PASS != comResult)
        {
            processResult = BL_ERROR_COMMUNICATION_FAIL;
        }
        ftpHelper.resendRequired = false;
    }
    else if (ftpHelper.responseRequired)
    {
        comResult = COM_FrameSet((uint8_t *) & ftpResponseSender.bufferPtr[0], ftpResponseSender.receiveIndex);

        if (COM_PASS != comResult)
        {
            processResult = BL_ERROR_COMMUNICATION_FAIL;
        }
        ftpHelper.responseRequired = false;
    }
    else
    {
        // Do nothing
    }

    return processResult;
}

static bool SequenceNumberValidate(void)
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

static bl_result_t OperationalBlockExecute(void)
{
    bl_result_t processResult = BL_BUSY;

    switch (ftpCommandReceiver.bufferPtr[FTP_BYTE_INDEX])
    {
    case FTP_GET_CLIENT_INFO:
        ClientInfoResponseSet(&ftpResponseSender);
        processResult = BL_PASS;
        break;

    case FTP_GET_IMAGE_STATE:
        processResult = BL_ImageVerify();
        ftp_image_state_t isImageValid = (processResult == BL_PASS) ? FTP_IMAGE_VALID : FTP_IMAGE_INVALID;
        ResponseSet(&ftpResponseSender, (uint8_t *) & isImageValid, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 1U);
        break;

    case FTP_START_TRANSFER:
        processResult = BL_PASS;
        (void) BL_Initialize();
        ResponseSet(&ftpResponseSender, NULL, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 0U);
        break;

    case FTP_WRITE_CHUNK:
        processResult = BL_BootCommandProcess(&(ftpCommandReceiver.bufferPtr[FILE_DATA_INDEX]), ftpCommandReceiver.receiveIndex);
        if (processResult == BL_PASS)
        {
            ResponseSet(&ftpResponseSender, NULL, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 0U);
        }
        else
        {
            ftp_abort_code_t abortCode = AbortCodeGet(processResult);
            ResponseSet(&ftpResponseSender, (uint8_t *)&abortCode, FTP_ABORT_TRANSFER, ftpHelper.currentSequenceNumber, 1U);
        }
        break;

    case FTP_END_TRANSFER:
        ResponseSet(&ftpResponseSender, NULL, FTP_COMMAND_SUCCESS, ftpHelper.currentSequenceNumber, 0U);
        resetPending = true;
#ifdef MULTI_STAGE_RESPONSE
        // Prevent any reset from occurring until the communication layer is working.
        isComBusy = true;
#endif
        processResult = BL_PASS;
        break;

    default:
        processResult = BL_ERROR_UNKNOWN_COMMAND;
        ResponseSet(&ftpResponseSender, NULL, FTP_COMMAND_NOT_SUPPORTED, ftpHelper.currentSequenceNumber, 0U);
        break;
    }

    return processResult;
}

static ftp_abort_code_t AbortCodeGet(bl_result_t targetStatus)
{
    ftp_abort_code_t abortCode = FTP_GENERIC_ERROR;
    switch (targetStatus)
    {
    case BL_ERROR_VERIFICATION_FAIL:
        abortCode = FTP_INVALID_FILE_ERROR;
        break;
    case BL_ERROR_ADDRESS_OUT_OF_RANGE:
        abortCode = FTP_ADDRESS_ERROR;
        break;
    case BL_ERROR_COMMAND_PROCESSING:
        abortCode = FTP_WRITE_ERROR;
        break;
    case BL_ERROR_UNKNOWN_COMMAND:
        abortCode = FTP_INVALID_FILE_ERROR;
        break;
    default:
        break;
    }
    return abortCode;
}

static ftp_transport_failure_code_t TransportFailureCodeGet(bl_result_t targetStatus)
{
    ftp_transport_failure_code_t transportCode = FTP_INTEGRITY_CHECK_ERROR;

    switch (targetStatus)
    {
    case BL_ERROR_FRAME_VALIDATION_FAIL:
        transportCode = FTP_INTEGRITY_CHECK_ERROR;
        break;
    case BL_ERROR_BUFFER_OVERLOAD:
        transportCode = FTP_COMMAND_TOO_LONG_ERROR;
        break;
    default:
        break;
    }
    return transportCode;
}

static void ParserDataReset(void)
{
    uint16_t i = 0;
    ftpCommandReceiver.receiveIndex = 0;

    // Clear the transfer buffer
    for (i = 0; i != MAX_TRANSFER_SIZE; i++)
    {
        ftpCommandReceiver.bufferPtr[i] = 0x00U;
    }
}

static void DeviceResetCheck(void)
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

static void ResponseSet(ftp_process_frame_t * responseData, uint8_t * responsePayload, ftp_response_status_t responseStatus, uint8_t sequenceByte, uint16_t responsePayloadLength)
{
    // Calculate the length of the response
    responseData->receiveIndex = (responsePayloadLength + SEQUENCE_DATA_SIZE + COMMAND_DATA_SIZE);

    // Update The Sequence Value
    responseData->bufferPtr[SEQUENCE_BYTE_INDEX] = sequenceByte;

    // Status
    responseData->bufferPtr[FTP_BYTE_INDEX] = responseStatus;

    // Data Bytes
    for (uint8_t i = 0; i < responsePayloadLength; i++)
    {
        responseData->bufferPtr[(FILE_DATA_INDEX + i)] = responsePayload[i];
    }
}

static uint8_t TLVAppend(uint8_t * dataBufferStart, ftp_tlv_t tlvData)
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

static void ClientInfoResponseSet(ftp_process_frame_t * responseData)
{
    // Calculate and set the response length
    responseData->receiveIndex = (
            ftpVersionTLVData.dataLength + TLV_HEADER_SIZE +
            ftpTransferParametersTLVData.dataLength + TLV_HEADER_SIZE +
            ftpTimeoutTLVData.dataLength + TLV_HEADER_SIZE +
            ftpMinInterMessageDelayTLVData.dataLength + TLV_HEADER_SIZE +
            SEQUENCE_DATA_SIZE +
            COMMAND_DATA_SIZE
            );

    // Update The Sequence Value
    responseData->bufferPtr[SEQUENCE_BYTE_INDEX] = ftpHelper.currentSequenceNumber;

    // Update Status
    responseData->bufferPtr[FTP_BYTE_INDEX] = FTP_COMMAND_SUCCESS;

    uint16_t fileDataOffset = FILE_DATA_INDEX;

    // push each TLV Byte Stream to the buffer
    fileDataOffset += TLVAppend(&(responseData->bufferPtr[fileDataOffset]), ftpVersionTLVData);
    fileDataOffset += TLVAppend(&(responseData->bufferPtr[fileDataOffset]), ftpTransferParametersTLVData);
    fileDataOffset += TLVAppend(&(responseData->bufferPtr[fileDataOffset]), ftpTimeoutTLVData);

    // drop the length of the last TLV append command because it is not needed
    (void) TLVAppend(&(responseData->bufferPtr[fileDataOffset]), ftpMinInterMessageDelayTLVData);
}

bl_result_t FTP_Initialize(void)
{
    // Tell com layer the max size of the buffer it can use
    com_adapter_result_t comInitStatus = COM_Initialize(MAX_TRANSFER_SIZE);
    return (comInitStatus == COM_PASS) ? BL_PASS : BL_FAIL;
}