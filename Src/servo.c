#include "servo.h"

#define MIN_ANGLE -35
#define MAX_ANGLE 35

#define ETAPE_1_SERVO 1 //envoi des 600us
#define ETAPE_2_SERVO 2	//envoi du signal
#define ETAPE_3_SERVO 3 //envoi du reste des 20ms
#define ETAPE_4_SERVO 4 //nothing to do

/* Private variables ---------------------------------------------------------*/
volatile unsigned long count = 0;
volatile unsigned long timeTogoAngle1 = 0;
volatile unsigned long timeTogoAngle2 = 0;

long nombreDeTickAAttendreServo1 = 0;
long nombreDeTickAAttendreServo2 = 0;

volatile int angleDeDestinationServo1 = 0;
volatile int angleDeDestinationServo2 = 0;

volatile int currentAngleServo1 = 0;
volatile int currentAngleServo2 = 0;

int etapeSignalServo1 = 1;
int etapeSignalServo2 = 0;

uint8_t Xphoneval, Yphoneval = 0x00;

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
			
			if(nombreDeTickAAttendreServo1>0)
				nombreDeTickAAttendreServo1--;
						
			if(nombreDeTickAAttendreServo1==0)
			{
				if(etapeSignalServo1==ETAPE_1_SERVO)
				{
					GPIOA->BSRR = 0x0001; //set pin to 1
					etapeSignalServo1 = ETAPE_2_SERVO;
					nombreDeTickAAttendreServo1=60; //600us
				}
				else if(etapeSignalServo1 == ETAPE_2_SERVO)
				{
					etapeSignalServo1 = ETAPE_3_SERVO;
					nombreDeTickAAttendreServo1 = angleDeDestinationServo1; //pause correspondante à l'angle (entre 0 et 180)
				}
				else if(etapeSignalServo1 == ETAPE_3_SERVO)
				{
					GPIOA->BRR = 0x0001; //set pin to 0
					etapeSignalServo1 = ETAPE_4_SERVO;
					nombreDeTickAAttendreServo1 = (20000-600-angleDeDestinationServo1*10)/10; //pause correspondante au 20ms - 600u - temps d'aller jusqu'a l'angle
				}
				else if(etapeSignalServo1 == ETAPE_4_SERVO)
				{
					if(currentAngleServo1 != angleDeDestinationServo1 || timeTogoAngle1 > count)
					{
						etapeSignalServo1 = ETAPE_1_SERVO;
					}
				}
			}
			
			if(nombreDeTickAAttendreServo2>0)
				nombreDeTickAAttendreServo2--;
						
			if(nombreDeTickAAttendreServo2==0)
			{
				if(etapeSignalServo2==ETAPE_1_SERVO)
				{
					GPIOC->BSRR = 0x0001; //set pin to 1
					etapeSignalServo2 = ETAPE_2_SERVO;
					nombreDeTickAAttendreServo2=60; //600us
				}
				else if(etapeSignalServo2 == ETAPE_2_SERVO)
				{
					etapeSignalServo2 = ETAPE_3_SERVO;
					nombreDeTickAAttendreServo2 = angleDeDestinationServo2; //pause correspondante à l'angle (entre 0 et 180)
				}
				else if(etapeSignalServo2 == ETAPE_3_SERVO)
				{
					GPIOC->BRR = 0x0001; //set pin to 0
					etapeSignalServo2 = ETAPE_4_SERVO;
					nombreDeTickAAttendreServo2 = (20000-600-angleDeDestinationServo2*10)/10; //pause correspondante au 20ms - 600u - temps d'aller jusqu'a l'angle
				}
				else if(etapeSignalServo2 == ETAPE_4_SERVO)
				{
					if(currentAngleServo2 != angleDeDestinationServo2 || timeTogoAngle2 > count)
					{
						etapeSignalServo2 = ETAPE_1_SERVO;
					}
				}
			}
			
	}

}

void initServos(void)
{
	etapeSignalServo1 = ETAPE_1_SERVO;
	nombreDeTickAAttendreServo1 = 0;
	
	etapeSignalServo2 = ETAPE_1_SERVO;
	nombreDeTickAAttendreServo2 = 0;
	
}

int getAngleBetweenMinAndMax(int angle)
{
	int newAngle = angle;
	if(newAngle<MIN_ANGLE)
	{
			newAngle=MIN_ANGLE;
	}
	else if(newAngle>MAX_ANGLE)
	{
			newAngle=MAX_ANGLE;
	}
	
	return newAngle;
}

void setAngleServo1(int angle)
{	

	angleDeDestinationServo1 = getAngleBetweenMinAndMax(angle)+90;
	
	if(currentAngleServo1 != angleDeDestinationServo1)
	{
		currentAngleServo1 = angleDeDestinationServo1;
		timeTogoAngle1 = count + 100000;
	}
	
}

void setAngleServo2(int angle)
{
	angleDeDestinationServo2 = getAngleBetweenMinAndMax(angle)+90;
	
	if(currentAngleServo2 != angleDeDestinationServo2)
	{
		currentAngleServo2 = angleDeDestinationServo2;
		timeTogoAngle2 = count + 100000;
	}
	
}

/* angle entre -90 et 90 */
void setAngleServos(int angleServo1, int angleServo2)
{
	setAngleServo1(angleServo1);
	setAngleServo2(angleServo2);
}

/*wait desired time in ticks (one tick is 0.564 ms)*/
void waitTicks(unsigned long time)   
{	
	unsigned long max = count + time; 
	while(count < max);	 //the count is incremented by the timer handler 
}


void turn_Right(uint16_t gpio_servo){
	
	if (gpio_servo == SERVO_1){		
		setAngleServo1(angleDeDestinationServo1-90+1);		
	}else if (gpio_servo == SERVO_2){	
		setAngleServo2(angleDeDestinationServo2-90+1);
	}
}

void turn_Left(uint16_t gpio_servo){

	if (gpio_servo == SERVO_1){		
		setAngleServo1(angleDeDestinationServo1-90-1);		
	}else if (gpio_servo == SERVO_2){	
		setAngleServo2(angleDeDestinationServo2-90-1);
	}
}
