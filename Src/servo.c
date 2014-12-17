#include "servo.h"


/* Private variables ---------------------------------------------------------*/
volatile unsigned long count = 0;
volatile unsigned short testvar = 0;
unsigned short servo1CurrentAngle = 0;
unsigned short servo2CurrentAngle = 0;

int valeurServo1 = 0;
int valeurServo2 = 0;

/* Private function prototypes -----------------------------------------------*/
/*wait desired time in ticks (one tick is 0.564 ms)*/
void waitTicks(unsigned long time); 
void setToAngle(uint16_t gpio_servo,int angle);



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

void setGyroAngle(uint16_t gpio_servo,int angle)
{
	int newAngle = angle;
	if(newAngle<-90)
	{
			newAngle=-90;
	}
	else if(newAngle>90)
	{
			newAngle=90;
	}
	
	setAngle(gpio_servo,newAngle+90);
}

void setAngle(uint16_t gpio_servo,int angle)
{
	int differenceAngles = 0;
	float interm = 0;
	unsigned long dest = 0;
	
	// Ce code permet de d�terminer le nombre de signaux 
  // � �mettre pour atteindre l'angle souhait�	
	
	if (gpio_servo == SERVO_1){
		differenceAngles = servo1CurrentAngle-angle;
	}else if (gpio_servo == SERVO_2){
		differenceAngles = servo2CurrentAngle-angle;
	}
	

	if(differenceAngles<0) {
				differenceAngles = -differenceAngles;
	}
	
	interm = (3.5F*(float)differenceAngles)*1000;  // temps de d�placement total du moteur vers la nouvelle position (angle)
	interm = interm/10.0F;
	dest = count + (unsigned long) interm; 
	
		if (gpio_servo == SERVO_1){
		servo1CurrentAngle = angle;
	}else if (gpio_servo == SERVO_2){
		servo2CurrentAngle = angle;
	}
	
	while(count < dest) // envoi de l'ordre pendant interm tick  (renvoi de trame pendant temps de d�placement)
	{
		setToAngle(gpio_servo,angle);
	}
}

/* Indique au cervo de se positionner jusqu'� l'angle en entr�e */
void setToAngle(uint16_t gpio_servo,int angle)
{
	unsigned short total = 20000;

	if (gpio_servo == SERVO_1){
		GPIOA->BSRR = 0x0001; //set pin to 1
	}else if (gpio_servo == SERVO_2){
		GPIOC->BSRR = 0x0001; //set pin to 1
	}
	
	
	// soustraction des 600 �sec de base
	waitTicks((unsigned long)60); //600us
	total = total-600;
	
	// pause pour d�finir l'angle souhait� (allongement du temps � l'�tat haut)
	waitTicks((unsigned long)angle);
	total = total-(angle*10); //soustraction du temps du mouvement de l'angle
	
	if (gpio_servo == SERVO_1){
		GPIOA->BRR = 0x0001; //set pin to 0
	}else if (gpio_servo == SERVO_2){
		GPIOC->BRR = 0x0001; //set pin to 0
	}
	total = total/10;
	
	waitTicks((unsigned long)total);

}

void turn_Right(uint16_t gpio_servo){
	
	if (gpio_servo == SERVO_1){
		if(valeurServo1 <180)
		{ 
			valeurServo1++;
			setAngle(gpio_servo,valeurServo1);
		}
	}else if (gpio_servo == SERVO_2){
		if(valeurServo2 <180)
		{ 
			valeurServo2++;
			setAngle(gpio_servo,valeurServo2);
		}
	}
}

void turn_Left(uint16_t gpio_servo){

	if (gpio_servo == SERVO_1){
		if(valeurServo1 >0)
		{ 
			valeurServo1--;
			setAngle(gpio_servo,valeurServo1);
		}
	}else if (gpio_servo == SERVO_2){
		if(valeurServo2 >0)
		{ 
			valeurServo2--;
			setAngle(gpio_servo,valeurServo2);
		}
	}
}
