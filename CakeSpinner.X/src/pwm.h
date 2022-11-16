/* 
 * File:   pwm.h
 * Author: Richard Morrison
 *
 * Created on 2 December 2018, 10:46 PM
 */

#ifndef PWM_H
#define	PWM_H

#include <stdint.h>

void setup_pwm ( void );
void set_pwm_duty_cycle( uint8_t duty_cycle );
void enable_motor_forward( void );
void enable_motor_reverse( void );
void disable_motor( void );

#endif	/* PWM_H */

