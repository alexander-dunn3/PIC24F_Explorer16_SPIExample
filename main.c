/*******************************************************************************
 Explorer 16 Demo Main File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    Explorer 16 Demo Main File.

  Description: 
    This is the main file for the Explorer 16 Demo. It contains the main demo
    flow and calls processor specific files for processor specific
    implementations.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "app.h"

#define ONE_VOLT 310
#define ONE_TENTH_VOLT 31
#define ONE_HUNDREDTH_VOLT 3

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables and Functions
// *****************************************************************************
// *****************************************************************************

void Respond_To_Button_Presses(void);
void SYS_Initialize(void);


APP_DATA appData = {
    .messageLine1 = "Explorer 16 Demo",
    .messageLine2 = "Press S3 to cont",
    .messageTime = "Time 00: 00: 00 ",
    .messageADC = " Pot = 0.00 Vdc "
};

bool _previous_button_pressed_state = false;

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main(void) {
    /* Call the System Initialize routine */
    SYS_Initialize();

    /*Initialize Timer*/
    TIMER_SetConfiguration(TIMER_CONFIGURATION_RTCC);
    
    /* Initialize UART*/
    //UART_Initialize();
    
    /* Initialise SPI */
    SPI_Initialize();

    /* Infinite Loop */
    while (1) {
        Respond_To_Button_Presses();
    };
}

/*******************************************************************************

  Function:
   void Respond_To_Button_Presses( void )

  Summary:
    Function to perform tasks on button presses

  Description:
    This function will perform tasks on button presses

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:

 */

/******************************************************************************/
void Respond_To_Button_Presses(void) {
    bool button_pressed = BUTTON_IsPressed(BUTTON_S6);
    
    if(!_previous_button_pressed_state && button_pressed)
    {
        SPI_Transmit();
    }
    
    _previous_button_pressed_state = button_pressed;
}
