/**
 * I2C Driver Client API Interface File
 *
 * @file i2c_client_interface.h
 *
 * @defgroup i2c_client I2C_CLIENT
 *
 * @brief This header file provides API prototypes for the I2C module client implementation in Polling and Interrupt mode.
 *
 * @version I2C Client Interface Version 1.0.3
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

#ifndef I2C_CLIENT_INTERFACE_H
#define I2C_CLIENT_INTERFACE_H
/**
  Section: Included Files
*/
#include <stdbool.h>
#include <stdint.h>
#include "i2c_client_types.h"
    
/**
 Section: Data Type Definitions
 */

/**
 * @ingroup i2c_client
 * @struct i2c_client_interface_t
 * @brief I2C Client driver prototypes structure.
 */
typedef struct 
{
    void (*Initialize)(void);
    void (*Deinitialize)(void);
    void (*WriteByte)(uint8_t wrByte);
    uint8_t(*ReadByte)(void);
    i2c_client_error_t (*ErrorGet)(void);
    i2c_client_transfer_dir_t (*TransferDirGet)(void);
    i2c_client_ack_status_t (*LastByteAckStatusGet)(void);
    void (*CallbackRegister)(bool (*handler)(i2c_client_transfer_event_t clientEvent));
    void (*Tasks)(void);
}i2c_client_interface_t;

#endif /* I2C_CLIENT_INTERFACE_H */