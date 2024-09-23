/**
 * Generated Pins header File
 * 
 * @file pins.h
 * 
 * @defgroup  pinsdriver Pins Driver
 * 
 * @brief This is generated driver header for pins. 
 *        This header file provides APIs for all pins selected in the GUI.
 *
 * @version Driver Version  3.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

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

#ifndef PINS_H
#define PINS_H

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_RA2 aliases
#define BL_INDICATOR_TRIS                 TRISAbits.TRISA2
#define BL_INDICATOR_LAT                  LATAbits.LATA2
#define BL_INDICATOR_PORT                 PORTAbits.RA2
#define BL_INDICATOR_WPU                  WPUAbits.WPUA2
#define BL_INDICATOR_OD                   ODCONAbits.ODCA2
#define BL_INDICATOR_ANS                  ANSELAbits.ANSA2
#define BL_INDICATOR_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define BL_INDICATOR_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define BL_INDICATOR_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define BL_INDICATOR_GetValue()           PORTAbits.RA2
#define BL_INDICATOR_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define BL_INDICATOR_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define BL_INDICATOR_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define BL_INDICATOR_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define BL_INDICATOR_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define BL_INDICATOR_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define BL_INDICATOR_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define BL_INDICATOR_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)
// get/set IO_RC2 aliases
#define BL_ENTRY_TRIS                 TRISCbits.TRISC2
#define BL_ENTRY_LAT                  LATCbits.LATC2
#define BL_ENTRY_PORT                 PORTCbits.RC2
#define BL_ENTRY_WPU                  WPUCbits.WPUC2
#define BL_ENTRY_OD                   ODCONCbits.ODCC2
#define BL_ENTRY_ANS                  ANSELCbits.ANSC2
#define BL_ENTRY_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define BL_ENTRY_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define BL_ENTRY_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define BL_ENTRY_GetValue()           PORTCbits.RC2
#define BL_ENTRY_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define BL_ENTRY_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define BL_ENTRY_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define BL_ENTRY_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define BL_ENTRY_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define BL_ENTRY_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define BL_ENTRY_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define BL_ENTRY_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)
// get/set IO_RC4 aliases
#define IO_RC4_TRIS                 TRISCbits.TRISC4
#define IO_RC4_LAT                  LATCbits.LATC4
#define IO_RC4_PORT                 PORTCbits.RC4
#define IO_RC4_WPU                  WPUCbits.WPUC4
#define IO_RC4_OD                   ODCONCbits.ODCC4
#define IO_RC4_ANS                  ANSELCbits.ANSC4
#define IO_RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define IO_RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define IO_RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define IO_RC4_GetValue()           PORTCbits.RC4
#define IO_RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define IO_RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define IO_RC4_SetPullup()          do { WPUCbits.WPUC4 = 1; } while(0)
#define IO_RC4_ResetPullup()        do { WPUCbits.WPUC4 = 0; } while(0)
#define IO_RC4_SetPushPull()        do { ODCONCbits.ODCC4 = 0; } while(0)
#define IO_RC4_SetOpenDrain()       do { ODCONCbits.ODCC4 = 1; } while(0)
#define IO_RC4_SetAnalogMode()      do { ANSELCbits.ANSC4 = 1; } while(0)
#define IO_RC4_SetDigitalMode()     do { ANSELCbits.ANSC4 = 0; } while(0)
// get/set IO_RC5 aliases
#define IO_RC5_TRIS                 TRISCbits.TRISC5
#define IO_RC5_LAT                  LATCbits.LATC5
#define IO_RC5_PORT                 PORTCbits.RC5
#define IO_RC5_WPU                  WPUCbits.WPUC5
#define IO_RC5_OD                   ODCONCbits.ODCC5
#define IO_RC5_ANS                  ANSELCbits.ANSC5
#define IO_RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define IO_RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define IO_RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define IO_RC5_GetValue()           PORTCbits.RC5
#define IO_RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define IO_RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define IO_RC5_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define IO_RC5_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define IO_RC5_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define IO_RC5_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define IO_RC5_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define IO_RC5_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)
// get/set IO_RC6 aliases
#define IO_RC6_TRIS                 TRISCbits.TRISC6
#define IO_RC6_LAT                  LATCbits.LATC6
#define IO_RC6_PORT                 PORTCbits.RC6
#define IO_RC6_WPU                  WPUCbits.WPUC6
#define IO_RC6_OD                   ODCONCbits.ODCC6
#define IO_RC6_ANS                  ANSELCbits.ANSC6
#define IO_RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define IO_RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define IO_RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define IO_RC6_GetValue()           PORTCbits.RC6
#define IO_RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define IO_RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define IO_RC6_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define IO_RC6_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define IO_RC6_SetPushPull()        do { ODCONCbits.ODCC6 = 0; } while(0)
#define IO_RC6_SetOpenDrain()       do { ODCONCbits.ODCC6 = 1; } while(0)
#define IO_RC6_SetAnalogMode()      do { ANSELCbits.ANSC6 = 1; } while(0)
#define IO_RC6_SetDigitalMode()     do { ANSELCbits.ANSC6 = 0; } while(0)
// get/set IO_RC7 aliases
#define CHIP_SELECT_TRIS                 TRISCbits.TRISC7
#define CHIP_SELECT_LAT                  LATCbits.LATC7
#define CHIP_SELECT_PORT                 PORTCbits.RC7
#define CHIP_SELECT_WPU                  WPUCbits.WPUC7
#define CHIP_SELECT_OD                   ODCONCbits.ODCC7
#define CHIP_SELECT_ANS                  ANSELCbits.ANSC7
#define CHIP_SELECT_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define CHIP_SELECT_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define CHIP_SELECT_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define CHIP_SELECT_GetValue()           PORTCbits.RC7
#define CHIP_SELECT_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define CHIP_SELECT_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define CHIP_SELECT_SetPullup()          do { WPUCbits.WPUC7 = 1; } while(0)
#define CHIP_SELECT_ResetPullup()        do { WPUCbits.WPUC7 = 0; } while(0)
#define CHIP_SELECT_SetPushPull()        do { ODCONCbits.ODCC7 = 0; } while(0)
#define CHIP_SELECT_SetOpenDrain()       do { ODCONCbits.ODCC7 = 1; } while(0)
#define CHIP_SELECT_SetAnalogMode()      do { ANSELCbits.ANSC7 = 1; } while(0)
#define CHIP_SELECT_SetDigitalMode()     do { ANSELCbits.ANSC7 = 0; } while(0)
/**
 * @ingroup  pinsdriver
 * @brief GPIO and peripheral I/O initialization
 * @param none
 * @return none
 */
void PIN_MANAGER_Initialize (void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handling routine
 * @param none
 * @return none
 */
void PIN_MANAGER_IOC(void);


#endif // PINS_H
/**
 End of File
*/