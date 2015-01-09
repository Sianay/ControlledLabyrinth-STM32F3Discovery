#include "servo.h"

#define MIN_ANGLE -36
#define MAX_ANGLE 36

#define STEP_1_SERVO 1 //send 600us
#define STEP_2_SERVO 2	//send signal
#define STEP_3_SERVO 3 //send more 20ms 
#define STEP_4_SERVO 4 //nothing to do

/* Private variables ---------------------------------------------------------*/
volatile unsigned long count = 0;
volatile unsigned long timeTogoAngle1 = 0;
volatile unsigned long timeTogoAngle2 = 0;

long numberOfTickToWaitServo1 = 0;
long numberOfTickToWaitServo2 = 0;

volatile int destinationAngleServo1 = 0;
volatile int destinationAngleServo2 = 0;

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
			
			if(numberOfTickToWaitServo1>0)
				numberOfTickToWaitServo1--;
						
			if(numberOfTickToWaitServo1==0)
			{
				if(etapeSignalServo1==STEP_1_SERVO)
				{
					GPIOA->BSRR = 0x0001; //set pin to 1
					etapeSignalServo1 = STEP_2_SERVO;
					numberOfTickToWaitServo1=60; //600us
				}
				else if(etapeSignalServo1 == STEP_2_SERVO)
				{
					etapeSignalServo1 = STEP_3_SERVO;
					numberOfTickToWaitServo1 = destinationAngleServo1; //pause corresponding to the angle (between 0 and 180)
				}
				else if(etapeSignalServo1 == STEP_3_SERVO)
				{
					GPIOA->BRR = 0x0001; //set pin to 0
					etapeSignalServo1 = STEP_4_SERVO;
					numberOfTickToWaitServo1 = (20000-600-destinationAngleServo1*10)/10; //pause corresponding to 20ms - 600u - time to angle
				}
				else if(etapeSignalServo1 == STEP_4_SERVO)
				{
					if(currentAngleServo1 != destinationAngleServo1 || timeTogoAngle1 > count)
					{
						etapeSignalServo1 = STEP_1_SERVO;
					}
				}
			}
			
			if(numberOfTickToWaitServo2>0)
				numberOfTickToWaitServo2--;
						
			if(numberOfTickToWaitServo2==0)
			{
				if(etapeSignalServo2==STEP_1_SERVO)
				{
					GPIOC->BSRR = 0x0001; //set pin to 1
					etapeSignalServo2 = STEP_2_SERVO;
					numberOfTickToWaitServo2=60; //600us
				}
				else if(etapeSignalServo2 == STEP_2_SERVO)
				{
					etapeSignalServo2 = STEP_3_SERVO;
					numberOfTickToWaitServo2 = destinationAngleServo2; //pause corresponding to the angle (between 0 and 180)
				}
				else if(etapeSignalServo2 == STEP_3_SERVO)
				{
					GPIOC->BRR = 0x0001; //set pin to 0
					etapeSignalServo2 = STEP_4_SERVO;
					numberOfTickToWaitServo2 = (20000-600-destinationAngleServo2*10)/10; //pause corresponding to 20ms - 600u - time to angle
				}
				else if(etapeSignalServo2 == STEP_4_SERVO)
				{
					if(currentAngleServo2 != destinationAngleServo2 || timeTogoAngle2 > count)
					{
						etapeSignalServo2 = STEP_1_SERVO;
					}
				}
			}
			
	}

}

void initServos(void)
{
	etapeSignalServo1 = STEP_1_SERVO;
	numberOfTickToWaitServo2 = 0;
	
	etapeSignalServo2 = STEP_1_SERVO;
	numberOfTickToWaitServo2 = 0;
	
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

	destinationAngleServo1 = getAngleBetweenMinAndMax(angle)+90;
	
	if(currentAngleServo1 != destinationAngleServo1)
	{
		currentAngleServo1 = destinationAngleServo1;
		timeTogoAngle1 = count + 100000;
	}
	
}

void setAngleServo2(int angle)
{
	destinationAngleServo2 = getAngleBetweenMinAndMax(angle)+90;
	
	if(currentAngleServo2 != destinationAngleServo2)
	{
		currentAngleServo2 = destinationAngleServo2;
		timeTogoAngle2 = count + 100000;
	}
	
}

/* angle between -90 and 90 */
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
		setAngleServo1(destinationAngleServo1-90+1);		
	}else if (gpio_servo == SERVO_2){	
		setAngleServo2(destinationAngleServo2-90+1);
	}
}

void turn_Left(uint16_t gpio_servo){

	if (gpio_servo == SERVO_1){		
		setAngleServo1(destinationAngleServo1-90-1);		
	}else if (gpio_servo == SERVO_2){	
		setAngleServo2(destinationAngleServo2-90-1);
	}
}
