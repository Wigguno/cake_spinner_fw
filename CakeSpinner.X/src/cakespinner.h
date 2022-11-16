/*
 * File:   cakespinner.h
 * Author: Richard Morrison
 *
 * Created on 2 December 2018, 6:29 PM
 */

#ifndef CAKESPINNER_H
#define	CAKESPINNER_H

#include <xc.h> 

#define GPIO_LED1 PORTAbits.RA2
#define GPIO_DIR PORTAbits.RA5

#define PWM_F_TRIS TRISCbits.TRISC1
#define PWM_R_TRIS TRISCbits.TRISC2

// Load this into ADCON0bits.CHS to select the channel
#define AN_SPEED 4 

#define SR_DAT PORTCbits.RC3
#define SR_LAT PORTCbits.RC4
#define SR_CLK PORTCbits.RC5

#endif	/* CAKESPINNER_H */

