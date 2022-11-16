/*
 * File:   main.c
 * Author: Richard Morrison
 *
 * Created on 2 December 2018, 6:29 PM
 */

// Include the processor configuration bits first
#include "config.h"

#include <xc.h> // Microchip's stuff
#include "cakespinner.h" // Includes hardware definitions

#include <stdint.h> // Includes standard fixed-bit length types
#include <string.h> // Includes memory functions, eg. memset, memcpy

#include "setup.h" // Includes setup routines
#include "pwm.h" // PWM routines for driving motor
#include "adc.h" // PWM routines for driving motor

// Prototype Functions
void setup_timer4( void );

// States for the state machine
typedef enum {
    SPIN_STATE_STOPPED_F,
    SPIN_STATE_FORWARD,
    SPIN_STATE_STOPPED_R,
    SPIN_STATE_REVERSE,
} spin_state_t;
spin_state_t spin_state = SPIN_STATE_STOPPED_F;

// System flag structure
typedef struct { 
    unsigned f_button_pressed:1;
    unsigned f_ad_result_new:1;
} sys_flags_t;
sys_flags_t sys_flags;

uint8_t ad_result = 0;      // Record the last A->D Result, even if motor stopped
uint8_t motor_duty = 0;     // The currently set motor duty cycle
uint8_t motor_target = 0;   // The target motor duty cycle

// Global interrupt routine ( no interrupt vectors :( )
void __interrupt() isr ( void )
{
    // External Interrupt ( Direction Button Pressed )
    if ( INTE && INTF )
    {
        INTF = 0;
        
        // Set a flag saying the button has been pressed
        sys_flags.f_button_pressed = 1;
    }
    
    // ADC Interrupt
    if ( ADIE && ADIF )
    {
        ADIF = 0;
        
        // Record the A->D result even if we're in a stopped state
        ad_result = ADRESH;
        sys_flags.f_ad_result_new = 1;
        
        // If we're in a moving state, change the target duty
        if ( spin_state == SPIN_STATE_FORWARD || spin_state == SPIN_STATE_REVERSE )
        {
            // Set the motor target speed
            motor_target = ad_result; // The upper 8 bits of the AD result
        }
    }
    
    // Timer 4 Interrupt ( 10 ms periodic ))
    if ( TMR4IE && TMR4IF )
    {
        TMR4IF = 0;
        
        // Move the motor speed towards the motor target
        if ( motor_target > motor_duty )
            motor_duty++;
        if ( motor_target < motor_duty )
            motor_duty--;
        
        // Start a new ADC conversion if none are going at the moment
        if ( !ADCON0bits.ADGO )
            ADCON0bits.ADGO = 1;
    }
}

void main ( void ) {
    
    INTCONbits.GIE = 1;     // Enable Interrupts
    INTCONbits.PEIE = 1;    // Enable Peripheral Interrupts
    
    memset( &sys_flags, 0, sizeof( sys_flags_t ) );
    
    setup_io();
    
    GPIO_LED1 = 0;
    
    setup_pwm();
    setup_adc();
    setup_timer4();
    
    // Main Loop
    while ( 1 )
    {
        // Set the motor speed
        set_pwm_duty_cycle( motor_duty );
        
        // Disable the motor if a) in a stopped state, and b) duty cycle  == 0
        if ( motor_duty == 0 && ( spin_state == SPIN_STATE_STOPPED_F || spin_state == SPIN_STATE_STOPPED_R ) )
        {
            disable_motor();
        }
        
        // Advance the state machine
        if ( sys_flags.f_button_pressed )
        {
            // Clear the button pressed flag
            sys_flags.f_button_pressed = 0;
            GPIO_LED1 = !GPIO_LED1;
        
            switch ( spin_state )
            {
                case SPIN_STATE_STOPPED_F:
                    // Only engage forward gear if the motor has stopped
                    if ( motor_duty == 0 )
                    {
                        spin_state = SPIN_STATE_FORWARD;
                        enable_motor_forward();
                        motor_target = ad_result;
                    }
                    break;

                case SPIN_STATE_STOPPED_R:
                    // Only reverse forward gear if the motor has stopped
                    if ( motor_duty == 0 )
                    {
                        spin_state = SPIN_STATE_REVERSE;
                        enable_motor_reverse();
                        motor_target = ad_result; 
                    }
                    break;

                case SPIN_STATE_FORWARD:
                    // Increment the state, set the target speed to zero
                    spin_state = SPIN_STATE_STOPPED_R;
                    motor_target = 0;
                    break;

                case SPIN_STATE_REVERSE:
                    // Increment the state, set the target speed to zero
                    spin_state = SPIN_STATE_STOPPED_F;
                    motor_target = 0;
                    break;
                    
                default:
                    spin_state = SPIN_STATE_STOPPED_F;
                    disable_motor();
                    motor_target = 0;
                    break;
            }
        }
        
        //GPIO_LED1 = !GPIO_LED1;
        
        // Clear the watchdog timer each loop
        CLRWDT();
    }
}

// Set up timer 4 for 10 ms periodic interrupts
void setup_timer4( void )
{
    T4CONbits.T4CKPS = 3;   // 64x prescaler
    T4CONbits.T4OUTPS = 9;  // 10x postscaler
    PR4 = 124;              // 125 period
    
    TMR4IF = 0;
    TMR4IE = 1;
    
    T4CONbits.TMR4ON = 1;
}