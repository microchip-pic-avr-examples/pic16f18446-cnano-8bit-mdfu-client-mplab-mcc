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
 * @file        bl_interrupt.c
 * @ingroup     mdfu_client_8bit
 *
 * @brief       This file includes assembly code to implement interrupt support for the MDFU Client application.
 */
#include "bl_config.h"
#include "bl_interrupt.h"
#ifdef __XC8__
#include <xc.h>
#endif

volatile __persistent BL_INTERRUPT_FLAG_QUAL __bit bootloaderIsRunning __at(BL_INTERRUPT_FLAG_BIT_ADDRESS);

__asm("psect	intentry,class=CODE,delta=2");
__asm("org 0x0"); //this is relative to the intentry psect address
__asm("asmopt push");
__asm("asmopt off");    //don't permit assembler optimizations to touch the following
__asm("pagesel $");
//these devices save BSR on interrupt, so we can safely change bank
__asm("BANKSEL(_bootloaderIsRunning/8)");
//the ljmp below may expand to multiple instructions and therefore it cannot 
//follow the btfsc skip instruction. Instead we use a single goto instruction to
//jump over it. If the user can ensure the application's interrupt is in 
//reach of a goto instruction/in the same program memory page, then the
//double jump would not be required and this would be slightly more optimal
__asm("btfsc " ___mkstr(BANKMASK(_bootloaderIsRunning/8))",_bootloaderIsRunning&7");
__asm("goto jmp_boothivec");
__asm("ljmp " ___mkstr(BL_APPLICATION_INTERRUPT_VECTOR_LOW));
__asm("jmp_boothivec:");
__asm("asmopt pop");
//bootloader interrupt code will automatically be linked here
