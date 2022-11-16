/* 
 * File:   pwm.c
 * Author: Richard Morrison
 *
 * Created on 2 December 2018, 10:46 PM
 */

#include <xc.h>
#include "cakespinner.h" // Includes hardware definitions

#include <stdint.h>

#include "pwm.h"

#define DUTY_FACTOR 0.2

void setup_pwm ( void )
{
    // 1. Disable the PWMx pin output driver(s) by setting the associated TRIS bit(s).
    PWM_F_TRIS = 1;
    PWM_R_TRIS = 1;
    
    // 2. Configure the PWM output polarity by configuring the PWMxPOL bit of the PWMxCON register.
    PWM5CONbits.PWM5POL = 0;
    PWM6CONbits.PWM6POL = 0;
    
    // 3. Load the PR2 register with the PWM period value, as determined by Equation 19-1.
    // PWM Period = [(PR2 + 1)] * 4 * TOSC * (TMR2 Prescale Value)
    // H Bridge Driver needs a period of between 10 and 50 us - aim for 20 us
    // 20 uS = [(PR2 + 1)] * 4 * 31.25 ns  
    // PR2 = 159
    PR2 = 159;
    
    // 4. Load the PWMxDCH register and bits <7:6> of the PWMxDCL register with the PWM duty cycle value, as determined by Equation 19-2.
    // Pulse Width = ( PWMxDC ) * TOSC * (TMR 2 Prescale Value)
    set_pwm_duty_cycle( 0 );
    
    // 5. Configure and start Timer2:
    TMR2IF = 0;
    T2CONbits.T2CKPS = 0;
    T2CONbits.TMR2ON = 1;
    
    // 6. Wait until the TMR2IF is set
    while ( TMR2IF == 0 );
    
    // 7. When the TMR2IF flag bit is set:
    PWM_F_TRIS = 0;
    PWM_R_TRIS = 0;
    
    // Let the state machine handle which PWM is enabled
}

// Pass a duty cycle, 0-255
void set_pwm_duty_cycle( uint8_t duty_cycle )
{
    //Duty Cycle Ratio = PWMxDC / ( 4 * (PR2 + 1) )
    uint16_t max_duty = (PR2 + 1) * 4;
    float duty_ratio = duty_cycle / 255.0;
    
    uint16_t duty_amount = duty_ratio * max_duty;
    
    uint8_t dch = ( duty_amount & 0x03FC ) >> 2;
    uint8_t dcl = ( duty_amount & 0x0003 ) << 6;
    
    PWM5DCH = dch;
    PWM5DCL = dcl;
    
    PWM6DCH = dch;
    PWM6DCL = dcl;
}

void enable_motor_forward( void )
{
    PWM6CONbits.PWM6EN = 0;
    PWM5CONbits.PWM5EN = 1;
}

void enable_motor_reverse( void )
{
    PWM5CONbits.PWM5EN = 0;
    PWM6CONbits.PWM6EN = 1;
}

void disable_motor( void )
{
    PWM5CONbits.PWM5EN = 0;
    PWM6CONbits.PWM6EN = 0;
}