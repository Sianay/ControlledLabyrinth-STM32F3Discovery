#include "fall_detector.h"
#include "main.h"
#include "servo.h"

/* Private variables ---------------------------------------------------------*/


int columnNumber = 7;
int lineNumber = 6;
int fallTotalNumber = 40;
uint16_t gpoToSwitchOn;

const uint16_t pinLecture[] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5};
volatile unsigned char j;
int currentFall;

/* Private functions ---------------------------------------------------------*/

void initGPIOFallDectector()
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

void startFallDetector()
{
    int i;
    
    for (j=0; j<columnNumber-1; j++)
    {
        for (i=0; i<=lineNumber;i++)
        {
            gpoToSwitchOn = 1<<i;
            GPIOD->BSRR = gpoToSwitchOn; //set pin to 1
            
            waitTicks(20);
            
            currentFall = (j*columnNumber)+i+1;
            
            if(currentFall <= fallTotalNumber)
            {
                if(GPIO_ReadInputDataBit(GPIOB,pinLecture[j]) == 1)
                {
                    printf("#%d#",currentFall); //print to send to UART
                    waitTicks(1);
                }
            }
            GPIOD->BRR = gpoToSwitchOn; //set pin to 0
        }
    }
    
}
