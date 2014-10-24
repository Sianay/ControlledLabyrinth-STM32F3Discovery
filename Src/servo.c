#include "servo.h"

/* Private variables ---------------------------------------------------------*/
volatile unsigned long count=0;
volatile unsigned short testvar=0;
unsigned short servo1CurrentAngle=0;

unsigned short total = 20000;
int differenceAngles=0;
volatile unsigned long dest=0;
float interm=0;


/* Private function prototypes -----------------------------------------------*/
/*wait desired time in ticks (one tick is 0.564 ms)*/
void waitTicks(unsigned long time); 

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{	
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
			count++;
	}

}

/*wait desired time in ticks (one tick is 0.564 ms)*/
void waitTicks(unsigned long time)   
{	
	unsigned long max = count + time; 

	while(count < max);	 //the count is incremented by the timer handler 
}



void setAngle(int angle)
{
	// ce code permet de déterminer le nombre de signaux 
  // à émmettre pour atteindre l'angle souhaité	
	differenceAngles=servo1CurrentAngle-angle;
	if(differenceAngles<0)
		differenceAngles=-differenceAngles;
	
	interm = (3.5F*(float)differenceAngles)*1000;  // temps de déplacement total du moteur vers la nouvelle position (angle)
	interm = interm/10.0F;
	dest = count + (unsigned long) interm; 
	servo1CurrentAngle=angle;
	
	while(count < dest) // envoi de l'ordre pendant interm tick  (renvoi de trame pendant temps de déplacement)
	{
		setToAngle(angle);
	}
}
void setToAngle(int angle)
{
	total = 20000;

	GPIOA->BSRR = 0x0001;
	
	// soustraction des 600 µsec de base
	waitTicks((unsigned long)60); //600us
	total=total-600;
	
	// pause pour définir l'angle souhaité (allongement du temps à l'état haut)
	waitTicks((unsigned long)angle);
	total=total-(angle*10); //soustraction du temps du mouvement de l'angle
	
	GPIOA->BRR = 0x0001;
	total=total/10;
	
	waitTicks((unsigned long)total);

}
