/**
* PROJET LABYRINTHE CONNECTE / LABYRINTH CONNECTED PROJECT
******************************************
* ARLEM 2015 
******************************************
* DAHAN Avner 
* GRIMA Benjamin
* PAYET Anais
******************************************
* main.h
*
* created from TIM_TimeBase example @ STMicroelectronics
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>

#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_it.h"
#include "stm32f30x_gpio.h"
#include "servo.h"
#include "fall_detector.h"
#include "uart_to_blemini.h"
#include "stm32_gyro.h"
#include <stdint.h>
#include "stm32f30x_conf.h"



/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


void Delay(__IO uint32_t nTime);

extern int xGyroPhoneValue;
extern int yGyroPhoneValue;


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics & ARLEM *****END OF FILE****/
