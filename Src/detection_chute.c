#include "detection_chute.h"
#include "main.h"
#include "servo.h"

int nombreDeColonne = 7;
int nombreDeLigne = 6;
int nombreDeTrouTotal = 40;
uint16_t portDeSortieAAllumer;

const uint16_t pinLecture[] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5};
volatile unsigned char j;
int trouEnCours;

void initGPIODetectionChute()
{

	GPIO_InitTypeDef  GPIO_InitStruct1;
	GPIO_InitTypeDef  GPIO_InitStruct2;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD	, ENABLE);
	
	
	GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct1.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct1.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStruct1);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
 
  GPIO_InitStruct2.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct2.GPIO_Pin = GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStruct2.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStruct2);
	
}

void startDetectionChute()
{ 
	//boucle sur chaque trou du jeu
	int i;
	
	for (j=0; j<nombreDeColonne-1; j++)
	{
		for (i=0; i<=nombreDeLigne;i++)
		{
			portDeSortieAAllumer = 1<<i;
			//ALLUMER GPIO OUT pour la ligne i
			GPIOD->BSRR = portDeSortieAAllumer; //set pin to 1

			waitTicks(50);

			trouEnCours = (j*nombreDeColonne)+i+1;

			if(trouEnCours <= nombreDeTrouTotal)
			{

				//si le port est a 1 sur la ligne j
				if(GPIO_ReadInputDataBit(GPIOB,pinLecture[j]) == 1)
				{
					//chute bille dans trouEnCours
					printf("#%d#",trouEnCours);

					waitTicks(1);

				}

			}

			GPIOD->BRR = portDeSortieAAllumer; //set pin to 0
		}
	}

}
