/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <xc.h>
#include <string.h>
#include <timer_1ms.h>

/* Definitions *****************************************************/
#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_SOURCE_EXTERNAL       0x0002
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000

#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030
#define TIMER_INTERRUPT_PRIORITY    0x0001
#define TIMER_INTERRUPT_PRIORITY_4  0x0004

#define UART_SIM_TRIS   TRISAbits.TRISA0 // RA0 direction state (input / output)
#define UART_SIM_LAT    LATAbits.LATA0 // RA0 output state (high / low)


char *start;
char *message;
char *message32 = "     32 - bytes, 32 - bytes     ";
char *message24 = " 24 - bytes, 24 - bytes ";
char *message16 = "   16 - bytes   ";
char *message9 = "9 - bytes";
int output_bit = 0;
int length;

/*********************************************************************
 * Function: void TIMER_SetConfiguration(void)
 *
 * Overview: Initializes the timer.
 *
 * PreCondition: None
 *
 * Input:  None
 *
 * Output: None
 *
 ********************************************************************/
void TIMER_SetConfiguration(void)
{
    length = strlen(message32);
    start = message32;
    UART_SIM_TRIS = 0; // RA0 as output (pin 58)
    UART_SIM_LAT = 1; // RA0 set high (pin 58)
    
    IPC2bits.T3IP = TIMER_INTERRUPT_PRIORITY;
    IFS0bits.T3IF = 0;

    TMR3 = 0;

    PR3 = 416; // 9600 baud (7us - 16.38ms range)

    T3CON = TIMER_ON |
            STOP_TIMER_IN_IDLE_MODE |
            TIMER_SOURCE_INTERNAL |
            GATED_TIME_DISABLED |
            TIMER_16BIT_MODE |
            TIMER_PRESCALER_1;

    IEC0bits.T3IE = 1;
}

/****************************************************************************
  Function:
    void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)

  Description:
    Timer ISR, used to update application state. If no transfers are pending
    new input request is scheduled.
  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
 ***************************************************************************/
void __attribute__ ( ( __interrupt__ , auto_psv ) ) _T3Interrupt ( void )
{
    bool high = ((*message32 >> output_bit) & 0x01) == 0x01;

    if(high)
        UART_SIM_LAT = 1;
    else
        UART_SIM_LAT = 0;
    
    if(++output_bit == 8)
    {
        output_bit = 0; // reset output_bit if required
        message32++; // increment message pointer
    }
    
    // reset start of message
    if(message32 == (start + length))
        message32 = start;
    
    IFS0bits.T3IF = 0;
}