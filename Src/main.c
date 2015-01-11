/**
 ******************************************************************************
 * @file    TIM_TimeBase/main.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    20-September-2012
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/** @addtogroup STM32F3_Discovery_Peripheral_Examples
 * @{
 */

/** @addtogroup TIM_TimeBase
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

//#define ENABLE_GYROSCOPE
//#define ENABLE_BUTTON
#define ENABLE_UART



/* Private variables ---------------------------------------------------------*/


TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 40961;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;
__IO uint32_t UserButtonPressed = 0;
__IO uint32_t TimingDelay = 0;



GPIO_InitTypeDef  GPIO_InitStructure;
int xGyroPhoneValue;
int yGyroPhoneValue;


/* Private function prototypes -----------------------------------------------*/
static void TIM_Config(void);
void ButtonPush_Handle(uint16_t pin1,uint16_t pin2);



/* Private functions ---------------------------------------------------------*/


/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
     this is done through SystemInit() function which is called from startup
     file (startup_stm32f30x.s) before to branch to application main.
     To reconfigure the default setting of SystemInit() function, refer to
     system_stm32f30x.c file
     */
    
    /* TIM Configuration */
    TIM_Config();
    
    /* Compute the prescaler value */
    // PrescalerValue = (uint16_t) ((SystemCoreClock) / 72000000) - 1;
    
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 5;
    TIM_TimeBaseStructure.TIM_Prescaler = 120;
    TIM_TimeBaseStructure.TIM_ClockDivision = 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    /* Prescaler configuration */
    // TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
    
    
    /* TIM Interrupts enable */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);
    
    
    /* Configure the GPIO_SERVO 1 & 2 pin 0 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA	, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC	, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    
#ifdef  ENABLE_BUTTON
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF	, ENABLE);
    
    /* Gestion des boutons sur PORT B */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 & GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
#endif
    
    
#ifdef  ENABLE_UART
    /* UART configuration */
    initUART();
#endif
    
    /* Detection Chute configuration */
    initGPIOFallDectector();
    
    //STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
    /* Reset UserButton_Pressed variable */
    //UserButtonPressed = 0x00;
    
    /* Reset angle servo  */
    setAngleServos(0,0);
    initServos();
    
    /* Infinite loop */
    while (1)
    {
        
#ifdef  ENABLE_BUTTON
        ButtonPush_Handle(GPIO_Pin_9,GPIO_Pin_10);
#endif
        
#ifdef  ENABLE_GYROSCOPE
        Gyro_Handle();
#endif
        
#ifdef  ENABLE_UART
        setAngleServos(xGyroPhoneValue,yGyroPhoneValue);
#endif
        startFallDetector();
    }
    
    
}



/**
 *  Control Servo1 with push button impulsion
 *
 **/
void ButtonPush_Handle(uint16_t pin1,uint16_t pin2)
{
    
    if(GPIO_ReadInputDataBit(GPIOF,pin1) == 0 )   // bouton incrémenté (enfoncé)
    {
        turn_Right(SERVO_1);
    }
    if(GPIO_ReadInputDataBit(GPIOF,pin2) == 0)   // bouton décrémenté   (enfoncé)
    {
        turn_Left(SERVO_1);
    }
}



/**
 * @brief  Configure the TIM IRQ Handler.
 * @param  None
 * @retval None
 */
static void TIM_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    /* Enable the TIM3 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Initialize Leds mounted on STM32F3-Discovery EVAL board */
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);
    
}

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in 10 ms.
 * @retval None
 */
void Delay(__IO uint32_t nTime)
{
    TimingDelay = nTime;
    
    while(TimingDelay != 0);
}

/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
    return 0;
}

/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
    return 0;
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    /* Infinite loop */
    while (1)
    {
        
    }
}
#endif


/**
 * @}
 */ 

/**
 * @}
 */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
