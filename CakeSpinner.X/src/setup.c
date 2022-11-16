/*
 * File:   setup.c
 * Author: Richard Morrison
 *
 * Created on 2 December 2018, 6:54 PM
 */

#include <xc.h>
#include "cakespinner.h" // Includes hardware definitions

#include "setup.h"

// Microchip doesn't define these for some reason
#define _PPS_PWM5 2
#define _PPS_PWM6 3

void setup_io ( void ) {
    
    // -------------------------------------------------------------------------
    // Setup
    
    // Clear the PPSLOCKED bit
    di();
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0;
    ei();
    
    // Unselect all analog pins
    ANSELA = 0;
    ANSELC = 0;
    
    // -------------------------------------------------------------------------
    // Port A
    
    // RA0 - ICSP_DAT
    
    // RA1 - ICSP_CLK
    
    // RA2 - GPIO_LED1 ( Output )
    TRISAbits.TRISA2 = 0;
    ODCONAbits.ODCA2 = 1;
    
    // RA3 - ICSP_MCLR
    
    // RA4 - AN_SPEED
    TRISAbits.TRISA4 = 1;
    ANSELAbits.ANSA4 = 1;
    
    // RA5 - GPIO_DIR ( Input )
    TRISAbits.TRISA5 = 1;
    INTPPS = 5;
    
    // Interrupt on falling edge
    INTCONbits.INTEDG = 0;
    INTF = 0;
    INTE = 1;
    
    // -------------------------------------------------------------------------
    // Port C
    
    // RC0 - NC
    
    // RC1 - PWM_F ( Output )
    TRISCbits.TRISC1 = 0;
    RC1PPS = _PPS_PWM5;
    
    // RC2 - PWM_R ( Output )
    TRISCbits.TRISC2 = 0;
    RC2PPS = _PPS_PWM6;
    
    // RC3 - SR_DAT ( Output )
    TRISCbits.TRISC3 = 0;
    
    // RC4 - SR_LAT ( Output )
    TRISCbits.TRISC4 = 0;
    
    // RC5 - SR_CLK ( Output )
    TRISCbits.TRISC5 = 0;
    
    
    // -------------------------------------------------------------------------
    // Cleanup
    
    // Set the PPSLOCKED bit
    di();
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 1;
    ei();
}
