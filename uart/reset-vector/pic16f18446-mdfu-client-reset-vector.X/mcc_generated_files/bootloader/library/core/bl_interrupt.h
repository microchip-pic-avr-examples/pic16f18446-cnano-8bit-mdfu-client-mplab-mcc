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
 * @file        bl_interrupt.h
 * @ingroup     mdfu_client_8bit
 * 
 * @brief      This file includes macros and variables that provide support for handling interrupts in the MDFU Client application.
 */
#ifndef BL_INTERRUPT_H
#define BL_INTERRUPT_H

#ifdef __XC8__
#include <xc.h>
#endif

/* 
"bootloaderIsRunning" is a single bit that is used to indicate when the bootloader is active.
When this is set to 1, interrupts will be handled by the bootloader's interrupt function(s).
When this is set to 0, interrupts will be handled by the application's interrupt function(s).
This file should be included into both the bootloader and the application so that memory 
associated with this flag is reserved in both parts.

The below addresses have been carefully chosen based on the memory configuration
of the specific architecture, and on resources the compiler or debuggers may 
reserve. 
PIC12IE, PIC14E and PIC14EX devices are more flexible with where the flag may 
be located. Anywhere in general purpose (banked) RAM will suffice. Unbanked RAM
should be avoided.
PIC14 and PIC18 devices differ. They must have the flag located in unbanked 
common/access RAM and the address must not conflict with, for example, memory
that may be reserved for compiler use or by debuggers. 

=> Only change if you understand all of the consequences.
*/
#if defined(_PIC14E) || defined(_PIC14EX)
#define BL_INTERRUPT_FLAG_ADDRESS   0x20      //located at byte address 0x20 in bank0
#define BL_INTERRUPT_FLAG_BIT_ADDRESS   (BL_INTERRUPT_FLAG_ADDRESS * 8)       //multiply by 8 to get the bit address
#define BL_INTERRUPT_FLAG_QUAL   
#endif

#if defined(_PIC14) 
#define BL_INTERRUPT_FLAG_ADDRESS   0x71      //located at byte address 0x71 in unbanked common memory
#define BL_INTERRUPT_FLAG_BIT_ADDRESS   (BL_INTERRUPT_FLAG_ADDRESS * 8)       //multiply by 8 to get the bit address
#define BL_INTERRUPT_FLAG_QUAL   __near
#endif

#if defined(_PIC18)
#define BL_INTERRUPT_FLAG_ADDRESS   0x5F      //located at byte address 0x5F in unbanked access RAM
#define BL_INTERRUPT_FLAG_BIT_ADDRESS   (BL_INTERRUPT_FLAG_ADDRESS * 8)       //multiply by 8 to get the bit address
#define BL_INTERRUPT_FLAG_QUAL   __near
#endif

extern volatile __persistent BL_INTERRUPT_FLAG_QUAL __bit bootloaderIsRunning;
#endif //BL_INTERRUPT_H