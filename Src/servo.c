#include "servo.h"

#define MIN_ANGLE -35
#define MAX_ANGLE 35

#define ETAPE_1_SERVO 1 //envoi des 600us
#define ETAPE_2_SERVO 2	//envoi du signal
#define ETAPE_3_SERVO 3 //envoi du reste des 20ms
#define ETAPE_4_SERVO 4 //nothing to do


/* Private variables ---------------------------------------------------------*/
volatile unsigned long count = 0;
volatile unsigned short testvar = 0;
unsigned short servo1CurrentAngle = 0;
unsigned short servo2CurrentAngle = 0;
unsigned long dest = 0;

//int valeurServo1 = 0;
//int valeurServo2 = 0;

//int oldAngleX =500;
//int oldAngleY =500;


int tempAngleX =0;
int tempAngleY =0;

long nombreDeTickAAttendreServo1=0;
long nombreDeTickAAttendreServo2=0;

volatile int angleDeDestinationServo1=0;
volatile int angleDeDestinationServo2=0;

int etapeSignalServo1=1;
int etapeSignalServo2=0;


uint8_t Xphoneval, Yphoneval = 0x00;

/* Private function prototypes -----------------------------------------------*/
/*wait desired time in ticks (one tick is 0.564 ms)*/
void waitTicks(unsigned long time); 
void setToAngle(uint16_t gpio_servo,int angle);

#define ABS(x)         (x < 0) ? (-x) : x

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
					if(servo1CurrentAngle != angleDeDestinationServo1)
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
					if(servo2CurrentAngle != angleDeDestinationServo2)
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
	if(angle<-90 || angle >90)
	{
			tempAngleX=angle;
			tempAngleX=angle;
	}
	tempAngleX=angle;
	
	angleDeDestinationServo1 = getAngleBetweenMinAndMax(angle)+90;
}

void setAngleServo2(int angle)
{
	tempAngleY=angle;

	angleDeDestinationServo2 = getAngleBetweenMinAndMax(angle)+90;
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



int getAngleBetween90(int angle )
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




void setAngle(uint16_t gpio_servo,int angle)
{
	int differenceAngles = 0;
	float interm = 0;
	dest = 0;

	// Ce code permet de déterminer le nombre de signaux 
  // à émettre pour atteindre l'angle souhaité	
	
	if (gpio_servo == SERVO_1){
		differenceAngles = servo1CurrentAngle-angle;
	}else if (gpio_servo == SERVO_2){
		differenceAngles = servo2CurrentAngle-angle;
	}
	

	if(differenceAngles<0) {
				differenceAngles = -differenceAngles;
	}
	
	interm = (3.5F*(float)differenceAngles)*1000;  // temps de déplacement total du moteur vers la nouvelle position (angle)
	interm = interm/10.0F;
	dest = count + (unsigned long) interm; 
	
	if (gpio_servo == SERVO_1){
		servo1CurrentAngle = angle;
	}else if (gpio_servo == SERVO_2){
		servo2CurrentAngle = angle;
	}
	
	while(count < dest) // envoi de l'ordre pendant interm tick  (renvoi de trame pendant temps de déplacement)
	{
		setToAngle(gpio_servo,angle);
	}
}

/* Indique au cervo de se positionner jusqu'à l'angle en entrée */
void setToAngle(uint16_t gpio_servo,int angle)
{
	unsigned short total = 20000;

	if (gpio_servo == SERVO_1){
		GPIOA->BSRR = 0x0001; //set pin to 1
	}else if (gpio_servo == SERVO_2){
		GPIOC->BSRR = 0x0001; //set pin to 1
	}
	
	
	// soustraction des 600 µsec de base
	waitTicks((unsigned long)60); //600us
	total = total-600;
	
	// pause pour définir l'angle souhaité (allongement du temps à l'état haut)
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
		if(angleDeDestinationServo1 <180)
		{ 
			angleDeDestinationServo1++;
		}
	}else if (gpio_servo == SERVO_2){
		if(angleDeDestinationServo2 <180)
		{ 
			angleDeDestinationServo2++;
		}
	}
}

void turn_Left(uint16_t gpio_servo){

	if (gpio_servo == SERVO_1){
		if(angleDeDestinationServo1 >0)
		{ 
			angleDeDestinationServo1--;

		}
	}else if (gpio_servo == SERVO_2){
		if(angleDeDestinationServo2 >0)
		{ 
			angleDeDestinationServo2--;
		}
	}
}

/*
void handlePhoneGyroValue(int angleX,int angleY){
	
	   Xphoneval = ABS((int8_t)(angleX));
     Yphoneval = ABS((int8_t)(angleY)); 
	
	
	    if ( Xphoneval>Yphoneval)
    {
       if (angleX < 0 && angleX >=-90)  //South
       { 

					turn_Right(SERVO_1);	
       }
        
			if (angleX > 0 && angleX <= 90) //North
      { 

				 turn_Left(SERVO_1);
			}
		}
    else
    {
			if (angleY < 0 && angleY >=-90) //West
      {
				turn_Right(SERVO_2);	
      }
      if (angleY > 0 && angleY <= 90) //East
      {
				turn_Left(SERVO_2);	
      } 
    }
	
}
*/

/*
void setGyroAngle(uint16_t gpio_servo,int angle)
{
	int newAngle=0;
	
	if(gpio_servo == SERVO_1)
	{
		newAngle = getAngleBetween90(angle);
		if(oldAngleX!=newAngle)
		{
				oldAngleX = newAngle;
				setAngle(SERVO_1,newAngle+90);
		}
	}
	else if(gpio_servo == SERVO_2)
	{
		newAngle = getAngleBetween90(angle);
		if(oldAngleY!=newAngle)
		{
				oldAngleY = newAngle;
				setAngle(SERVO_2,newAngle+90);
		}
	}
}

void setGyroAngles(int angleX, int angleY)
{
	setGyroAngle(SERVO_1, angleX);
	setGyroAngle(SERVO_2, angleY);
}*/

