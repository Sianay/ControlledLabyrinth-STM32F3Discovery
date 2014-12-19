#include "detection_chute.h"
#include "main.h"

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

	
	
	//initialisation du port D en sortie
	GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct1.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct1.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStruct1);

	//initialisation du port E en entree
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
 
 
  GPIO_InitStruct2.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStruct2.GPIO_Pin = GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
GPIO_InitStruct2.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_Init(GPIOB, &GPIO_InitStruct2);

  if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) != RESET)
    puts("High");
  else
    puts("Low");
	
	/*
	GPIO_InitStruct2.GPIO_Pin = GPIO_Pin_0 &  GPIO_Pin_1 & GPIO_Pin_2 & GPIO_Pin_3 & GPIO_Pin_4 & GPIO_Pin_5;
  GPIO_InitStruct2.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct2.GPIO_Speed = GPIO_Speed_50MHz; 
	
	GPIO_Init(GPIOE, &GPIO_InitStruct2);*/
	
	
	//initialisation du port E 11 en sortie pour allumer la led
/*	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	*/
		 nombreDeColonne = 7;
 nombreDeLigne = 6;
 nombreDeTrouTotal = 40;	
	
}

void startDetectionChute()
{ 
	//boucle sur chaque trou du jeu
	int i;
	int k =0;
	



	
	
		for (j=0; j<nombreDeColonne-1; j++)
		{
			 	for (i=0; i<=nombreDeLigne;i++)
				{
					portDeSortieAAllumer = 1<<i;
					//ALLUMER GPIO OUT pour la ligne i
					GPIOD->BSRR = portDeSortieAAllumer; //set pin to 1
	
							while(k<10000)
		{
			k++;
		}
		k=0;
		
					trouEnCours = (i*nombreDeLigne)+j+1;

					if(trouEnCours < nombreDeTrouTotal)
					{
							//si le port est a 1 sur la ligne j
							if(GPIO_ReadInputDataBit(GPIOB,pinLecture[j]) == 1)
							{
								trouEnCours=trouEnCours - 1;
								
								trouEnCours=trouEnCours + 1;
									//chute bille dans trouEnCours
										//GPIOE->BSRR = 0x0800; //set pin to 1
								printf("#%d#",trouEnCours);
							}
							else
							{
										//GPIOE->BRR = 0x0800; //set pin to 1

							}
					}
					
					GPIOD->BRR = portDeSortieAAllumer; //set pin to 0

				}
	}
	/*
	for (i=0; i<=nombreDeLigne;i++)
	{
		portDeSortieAAllumer = 1<<i;
		//ALLUMER GPIO OUT pour la ligne i
		GPIOD->BRR = portDeSortieAAllumer; //set pin to 0
	}*/
}
