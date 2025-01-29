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
// get/set IO_RB4 aliases
#define IO_RB4_TRIS                 TRISBbits.TRISB4
#define IO_RB4_LAT                  LATBbits.LATB4
#define IO_RB4_PORT                 PORTBbits.RB4
#define IO_RB4_WPU                  WPUBbits.WPUB4
#define IO_RB4_OD                   ODCONBbits.ODCB4
#define IO_RB4_ANS                  ANSELBbits.ANSB4
#define IO_RB4_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define IO_RB4_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define IO_RB4_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define IO_RB4_GetValue()           PORTBbits.RB4
#define IO_RB4_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define IO_RB4_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define IO_RB4_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define IO_RB4_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define IO_RB4_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define IO_RB4_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define IO_RB4_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define IO_RB4_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)
// get/set IO_RB6 aliases
#define IO_RB6_TRIS                 TRISBbits.TRISB6
#define IO_RB6_LAT                  LATBbits.LATB6
#define IO_RB6_PORT                 PORTBbits.RB6
#define IO_RB6_WPU                  WPUBbits.WPUB6
#define IO_RB6_OD                   ODCONBbits.ODCB6
#define IO_RB6_ANS                  ANSELBbits.ANSB6
#define IO_RB6_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define IO_RB6_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define IO_RB6_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define IO_RB6_GetValue()           PORTBbits.RB6
#define IO_RB6_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define IO_RB6_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define IO_RB6_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define IO_RB6_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)
#define IO_RB6_SetPushPull()        do { ODCONBbits.ODCB6 = 0; } while(0)
#define IO_RB6_SetOpenDrain()       do { ODCONBbits.ODCB6 = 1; } while(0)
#define IO_RB6_SetAnalogMode()      do { ANSELBbits.ANSB6 = 1; } while(0)
#define IO_RB6_SetDigitalMode()     do { ANSELBbits.ANSB6 = 0; } while(0)
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