/* 
 * File:   adc.h
 * Author: Richard Morrison
 *
 * Created on 3 December 2018, 1:02 PM
 */

#include <xc.h>
#include "cakespinner.h" // Includes hardware definitions

#include "adc.h"

void setup_adc ( void )
{
    // Set up the ADC to sample the speed potentiometer
    ADCON0bits.CHS = AN_SPEED;  // Connect the speed pot to the ADC
    
    ADCON1bits.ADFM = 0;        // Right Justified
    ADCON1bits.ADCS = 6;        // FOSC / 64 ( 2us per conversion )
    
    ADIF = 0;
    ADIE = 1;
    
    ADCON0bits.ADON = 1;        // Enable the ADC
}