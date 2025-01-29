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
// get/set IO_RB5 aliases
#define IO_RB5_TRIS                 TRISBbits.TRISB5
#define IO_RB5_LAT                  LATBbits.LATB5
#define IO_RB5_PORT                 PORTBbits.RB5
#define IO_RB5_WPU                  WPUBbits.WPUB5
#define IO_RB5_OD                   ODCONBbits.ODCB5
#define IO_RB5_ANS                  ANSELBbits.ANSB5
#define IO_RB5_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define IO_RB5_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define IO_RB5_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define IO_RB5_GetValue()           PORTBbits.RB5
#define IO_RB5_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define IO_RB5_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define IO_RB5_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define IO_RB5_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define IO_RB5_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define IO_RB5_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define IO_RB5_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define IO_RB5_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)
// get/set IO_RB7 aliases
#define IO_RB7_TRIS                 TRISBbits.TRISB7
#define IO_RB7_LAT                  LATBbits.LATB7
#define IO_RB7_PORT                 PORTBbits.RB7
#define IO_RB7_WPU                  WPUBbits.WPUB7
#define IO_RB7_OD                   ODCONBbits.ODCB7
#define IO_RB7_ANS                  ANSELBbits.ANSB7
#define IO_RB7_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define IO_RB7_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define IO_RB7_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define IO_RB7_GetValue()           PORTBbits.RB7
#define IO_RB7_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define IO_RB7_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define IO_RB7_SetPullup()          do { WPUBbits.WPUB7 = 1; } while(0)
#define IO_RB7_ResetPullup()        do { WPUBbits.WPUB7 = 0; } while(0)
#define IO_RB7_SetPushPull()        do { ODCONBbits.ODCB7 = 0; } while(0)
#define IO_RB7_SetOpenDrain()       do { ODCONBbits.ODCB7 = 1; } while(0)
#define IO_RB7_SetAnalogMode()      do { ANSELBbits.ANSB7 = 1; } while(0)
#define IO_RB7_SetDigitalMode()     do { ANSELBbits.ANSB7 = 0; } while(0)
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