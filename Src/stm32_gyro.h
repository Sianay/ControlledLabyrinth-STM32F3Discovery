#ifndef __STM32_GYRO_H
#define __STM32_GYRO_H

/**
**************************************
*  STM32 GYROSCOPE HANDLE 
**************************************
**/


/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported functions ------------------------------------------------------- */

void Gyro_Handle(void);
void Demo_GyroConfig(void);
void Demo_GyroReadAngRate (float* pfData);


#endif
