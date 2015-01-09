/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERVO_H
#define __SERVO_H

/**
********************************
*  SERVO PILOT 
********************************
**/


#include "stm32f30x.h"

#define SERVO_1 0
#define SERVO_2 1

void setAngleServos(int angleServo1, int angleServo2);
void initServos(void);

void turn_Right(uint16_t gpio_servo);
void turn_Left(uint16_t gpio_servo);
void waitTicks(unsigned long time);  

#endif
