/**
 * MSSP2 Generated Driver File
 *
 * @file mssp2.c
 *
 * @ingroup I2C_client
 *
 * @brief This file contains the API implementation for the I2C2 driver.
 *
 * @version I2C2 Driver Version 2.1.3
 * 
 * @version I2C2 Package Version 7.0.3
 */

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <xc.h>
#include "../mssp2.h"

/**
  Section: Driver Interface
 */
const i2c_client_interface_t SERCOM = { 
    .Initialize = I2C2_Initialize,                                   
    .Deinitialize = I2C2_Deinitialize,
    .WriteByte = I2C2_WriteByte,
    .ReadByte = I2C2_ReadByte,
    .TransferDirGet = I2C2_TransferDirGet,
    .LastByteAckStatusGet = I2C2_LastByteAckStatusGet,
    .ErrorGet = I2C2_ErrorGet,
    .CallbackRegister = I2C2_CallbackRegister,
    .Tasks = NULL
};

/**
 Section: Private functions declaration
 */
static void I2C2_EventHandler(void);
static void I2C2_ErrorEventHandler(void);

/**
 Section: Private Variable Definitions
 */
static volatile uint16_t i2c2Addr;
static volatile i2c_client_error_t i2c2ErrorState;
static bool(*I2C2_InterruptHandler)(i2c_client_transfer_event_t clientEvent);

/**
 Section: Public functions
 */
void I2C2_Initialize(void)
{
    /* CKE disabled; SMP Standard Speed;  */
    SSP2STAT = 0x80;
    /* SSPM 7 Bit Polling; CKP disabled; SSPEN disabled; SSPOV no_overflow; WCOL no_collision;  */
    SSP2CON1 |= 0x6;
    /* SEN enabled; RSEN disabled; PEN disabled; RCEN disabled; ACKEN disabled; ACKDT acknowledge; GCEN disabled;  */
    SSP2CON2 = 0x1;
    /* DHEN disabled; AHEN disabled; SBCDE disabled; SDAHT 100ns; BOEN disabled; SCIE disabled; PCIE disabled;  */
    SSP2CON3 = 0x0;
    /* SSPADD 64;  */
    SSP2ADD = 0x40;
    /* SSPMSK 254;  */
    SSP2MSK = 0xFE;
    /* Enable Interrupts */
    PIE3bits.SSP2IE = 1;
    PIE3bits.BCL2IE = 1;
    I2C2_InterruptHandler = NULL;
    SSP2CON1bits.SSPEN = 1;
    SSP2CON3bits.PCIE = 1;
    SSP2CON3bits.AHEN = 1;
    SSP2CON3bits.DHEN = 1;

}

void I2C2_Deinitialize(void)
{
    SSP2STAT = 0x00;
    SSP2CON1 |= 0x00;
    SSP2CON2 = 0x00;
    SSP2CON3 = 0x00;
    SSP2ADD = 0x08;
    SSP2MSK = 0xFE;
    /* Disable Interrupts */
    PIE3bits.SSP2IE = 0;
    PIE3bits.BCL2IE = 0;
    SSP2CON3bits.PCIE = 0;
}

void I2C2_WriteByte(uint8_t data)
{
    SSP2BUF = data;
}

uint8_t I2C2_ReadByte(void)
{
    return SSP2BUF;
}

uint16_t I2C2_ReadAddr(void)
{
    return (i2c2Addr >> 1);
}

i2c_client_error_t I2C2_ErrorGet(void)
{
    i2c_client_error_t error;
    error = i2c2ErrorState;
    i2c2ErrorState = I2C_CLIENT_ERROR_NONE;
    return error;
}

i2c_client_transfer_dir_t I2C2_TransferDirGet(void)
{
    return (SSP2STATbits.R_nW ? I2C_CLIENT_TRANSFER_DIR_READ : I2C_CLIENT_TRANSFER_DIR_WRITE);
}

i2c_client_ack_status_t I2C2_LastByteAckStatusGet(void)
{
    return (SSP2CON2bits.ACKDT ? I2C_CLIENT_ACK_STATUS_RECEIVED_NACK : I2C_CLIENT_ACK_STATUS_RECEIVED_ACK);
}

void I2C2_CallbackRegister(bool(*callback)(i2c_client_transfer_event_t clientEvent))
{
    if (callback != NULL)
    {
        I2C2_InterruptHandler = callback;
    }
}

void I2C2_ISR(void)
{
    I2C2_EventHandler();
}

void I2C2_ERROR_ISR(void)
{
    I2C2_ErrorEventHandler();
}

/**
 Section: Private functions
 */
static void I2C2_EventHandler(void)
{
    PIR3bits.SSP2IF = 0;
    if (0U != SSP2STATbits.P)
    {
        /* Notify that a stop event has occurred */
        I2C2_InterruptHandler(I2C_CLIENT_TRANSFER_EVENT_STOP_BIT_RECEIVED);
    }
    else if ((!SSP2STATbits.D_nA) && (0U != SSP2CON3bits.ACKTIM))
    {
        /* Received I2C address must be read out */
        i2c2Addr = I2C2_ReadByte();
        /* Clear Software Error State */
        i2c2ErrorState = I2C_CLIENT_ERROR_NONE;
        /* Notify that a address match event has occurred */
        if (I2C2_InterruptHandler(I2C_CLIENT_TRANSFER_EVENT_ADDR_MATCH) == true)
        {
            /* Send ACK */
            SSP2CON2bits.ACKDT = 0;
            SSP2CON2bits.ACKEN = 1;
        }
        else
        {
            /* Send NACK */
            SSP2CON2bits.ACKDT = 1;
            SSP2CON2bits.ACKEN = 1;
        }
    }
    else
    {
        /* Host reads from client, client transmits */
        if (0U != SSP2STATbits.R_nW)
        {
            if ((!SSP2STATbits.BF) && (!SSP2CON2bits.ACKSTAT))
            {
                /* I2C host wants to read. In the callback, client must write to transmit register */
                if (I2C2_InterruptHandler(I2C_CLIENT_TRANSFER_EVENT_TX_READY) == false)
                {
                    /* Send NACK */
                    SSP2CON2bits.ACKDT = 1;
                    SSP2CON2bits.ACKEN = 1;
                }
                else
                {
                    /* Send ACK */
                    SSP2CON2bits.ACKDT = 0;
                    SSP2CON2bits.ACKEN = 1;
                }
            }
        }
        else
        {
            if (0U != SSP2STATbits.BF)
            {
                /* I2C host wants to write. In the callback, client must read data by calling I2Cx_ReadByte()  */
                if (I2C2_InterruptHandler(I2C_CLIENT_TRANSFER_EVENT_RX_READY) == true)
                {
                    /* Send ACK */
                    SSP2CON2bits.ACKDT = 0;
                    SSP2CON2bits.ACKEN = 1;
                }
                else
                {
                    /* Send NACK */
                    SSP2CON2bits.ACKDT = 1;
                    SSP2CON2bits.ACKEN = 1;
                }
            }
        }
    }

    /* Data written by the application; release the clock stretch */
    SSP2CON1bits.CKP = 1;
}

static void I2C2_ErrorEventHandler(void)
{
    if (0U != PIR3bits.BCL2IF)
    {
        i2c2ErrorState = I2C_CLIENT_ERROR_BUS_COLLISION;
        I2C2_InterruptHandler(I2C_CLIENT_TRANSFER_EVENT_ERROR);
        PIR3bits.BCL2IF = 0; /* Clear the bus collision */
    }
    else if (0U != SSP2CON1bits.WCOL)
    {
        i2c2ErrorState = I2C_CLIENT_ERROR_WRITE_COLLISION;
        I2C2_InterruptHandler(I2C_CLIENT_TRANSFER_EVENT_ERROR);
        SSP2CON1bits.WCOL = 0; /* Clear the Write collision */
    }
    else
    {
        i2c2ErrorState = I2C_CLIENT_ERROR_NONE; 
    }
   
    /*Clear SSPOV bit if set*/
    if (0U != SSP2CON1bits.SSPOV)
    {
        SSP2CON1bits.SSPOV = 0; 
    }
     /* Data written by the application; release the clock stretch */
    SSP2CON1bits.CKP = 1;
}
