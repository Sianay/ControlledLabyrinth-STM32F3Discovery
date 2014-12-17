/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERVO_H
#define __SERVO_H


#include "stm32f30x.h"

#define SERVO_1 0
#define SERVO_2 1

void setAngle(uint16_t gpio_servo,int angle);
void setGyroAngle(uint16_t gpio_servo,int angle);
void turn_Right(uint16_t gpio_servo);
void turn_Left(uint16_t gpio_servo);
#endif
