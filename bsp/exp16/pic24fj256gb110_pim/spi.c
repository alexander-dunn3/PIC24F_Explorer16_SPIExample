/*
 * File:   spi.c
 * Author: alexander.dunn
 *
 * Created on 29 October 2021, 15:18
 */

#include <xc.h>
#include <spi.h>

#define SS_TRIS     TRISAbits.TRISA0
#define SS_LAT      LATAbits.LATA0

#define INPUT  1
#define OUTPUT 0

bool tx_complete = false;

/*********************************************************************
* Function: SPI_Initialize(void);
*
* Overview: Initializes SPI
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
********************************************************************/
void SPI_Initialize(void)
{
    TRISFbits.TRISF8 = 0; // RF8 as output (SDO1)
    TRISBbits.TRISB1 = 0; // RB1 as output (SCK1OUT)
    SS_TRIS = 0; // RA2 as output (SS)
    SS_LAT = 1; // set SS idle high
    
    // Unlock Registers
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    
    RPOR7bits.RP15R = 7; // RP15 -> SDO1 (RF8)
    RPOR0bits.RP1R = 8; // RP1 -> SCK1OUT (RB1)
    
    // Lock Registers
    __builtin_write_OSCCONL(OSCCON | 0x40);
    
    IFS0bits.SPI1IF = 0; // Clear the SPIxIF bit in the respective IFS register
    IEC0bits.SPI1IE = 1; // Set the SPIxIE bit in the respective IEC register
    IPC2bits.SPI1IP = 0x7; // Write the SPIxIP bits in the respective IPC register to set the interrupt priority
    
    SPI1STATbits.SPIEN = 0; // SPIxCON1 and SPIxCON2 can not be written while the SPIx modules are enabled. 
                        // The SPIEN (SPIxSTAT<15>) bit must be clear before modifying either register.
    SPI1STATbits.SISEL = 5; // Interrupt when the last bit is shifted out of SPIxSR, now the transmit is complete
    
    SPI1CON1bits.DISSCK = 0; // Internal SPIx clock is enabled
    SPI1CON1bits.DISSDO = 0; // SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 = 0; // Communication is byte-wide (8 bits)
    SPI1CON1bits.SMP = 0; // Input data sampled at middle of data output time
    SPI1CON1bits.CKE = 1; // Serial output data changes on transition from Idle clock state to active clock state
    SPI1CON1bits.SSEN = 0; // SSx pin not used for Master
    SPI1CON1bits.CKP = 1; // Idle state for clock is a low level; active state is a high level
    SPI1CON1bits.MSTEN = 1; // Master mode
    SPI1CON1bits.SPRE = 0x0; // Secondary prescale 8:1
    SPI1CON1bits.PPRE = 0x0; // Primary prescale 64:1git
    
    SPI1CON2bits.FRMEN = 0; // Framed SPIx support disabled
    SPI1CON2bits.SPIFSD = 0; // Frame sync pulse output (master)
    SPI1CON2bits.SPIFPOL = 0; // Frame sync pulse is active-low
    SPI1CON2bits.SPIFE = 0; // Frame sync pulse precedes first bit clock
    SPI1CON2bits.SPIBEN = 0; // Enhanced Buffer disabled (Legacy mode)
    
    SPI1STATbits.SPIROV = 0; // Clear the SPIROV bit (SPIxSTAT<6>)
    
    SPI1STATbits.SPIEN = 1; // Enable SPI operation by setting the SPIEN bit (SPIxSTAT<15>)
}

void SPI_Transmit(void)
{
    int i = 0;
    
    // drop SS (active)
    SS_LAT = 0;
    
    // output message
    char message[15] = "congratulations";
    
    for (; i < 15; i++)
    {
        SPI1BUF = message[i];
        
        while(SPI1STATbits.SPITBF) {} // wait for SPIx Transmit Buffer Full Status bit to enter 
                                      // 'Transmit started, SPIxTXB is empty' state
    }
    
    tx_complete = true;
}

int counter = 0;

/*
 SPI1 transfer completed interrupt
 */
void __attribute__ ( ( __interrupt__ , auto_psv ) ) _SPI1Interrupt(void)
{    
    if(tx_complete)
    {
        SS_LAT = 1;
        tx_complete = false;
    }

    
    IFS0bits.SPI1IF = 0; // Clear the SPIxIF bit in the respective IFS register   
    SPI1STATbits.SPIROV = 0; // Clear the SPIROV bit (SPIxSTAT<6>)
}