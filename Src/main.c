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

#define outRF GPIO_Pin_0

#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	      /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */
#define PI                         (float)     3.14159265f

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

/* Private macro -------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x


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

float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};
uint8_t Xval, Yval = 0x00;

int var = 0;
int titi;
int valeur = 0;
GPIO_InitTypeDef  GPIO_InitStructure;


/* Private function prototypes -----------------------------------------------*/
static void TIM_Config(void);
void ButtonPush_Handle(uint16_t pin1,uint16_t pin2);
void Gyro_Handle(void);
void turn_Right();
void turn_Left();

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
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA	, ENABLE);

	/* Configure the GPIO_LED pin */   
	GPIO_InitStructure.GPIO_Pin = outRF;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB	, ENABLE);
	
	/* Gestion des boutons sur PORT B */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 & GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI); 
  /* Reset UserButton_Pressed variable */
  //UserButtonPressed = 0x00; 

  /* Infinite loop */
  while (1)
  {
		ButtonPush_Handle(GPIO_Pin_0,GPIO_Pin_1);		
		Gyro_Handle();	
  }	


}

/**
*  Control servomotor with push button impulsion
*
**/
void ButtonPush_Handle(uint16_t pin1,uint16_t pin2){
	
		if(GPIO_ReadInputDataBit(GPIOB,pin1) == 0 )   // bouton incrémenté (enfoncé)
		{
			 turn_Right();
		}
		if(GPIO_ReadInputDataBit(GPIOB,pin2) == 0)   // bouton décrémenté   (enfoncé)
		{
			 turn_Left();
		}
	
}

void turn_Right(){
		 valeur ++;
			if(valeur >180)
			{ 
				valeur =0;
			}	
		 setAngle(valeur);

}

void turn_Left(){
			valeur --;
			if(valeur <0 )
			{
				valeur = 180;
			}
		  setAngle(valeur);
}


void Gyro_Handle(void){
	
			/* Demo Gyroscope */
    Demo_GyroConfig();
		
		
	  /* Read Gyro Angular data */
    Demo_GyroReadAngRate(Buffer);
         
      /* Update autoreload and capture compare registers value*/
      Xval = ABS((int8_t)(Buffer[0]));
      Yval = ABS((int8_t)(Buffer[1])); 
      
      if ( Xval>Yval)
      {
        if ((int8_t)Buffer[0] > 5.0f)
        { 
          /* LD10 On */
          STM_EVAL_LEDOn(LED10);
					
					turn_Right();
					
					
        }
        if ((int8_t)Buffer[0] < -5.0f)
        { 
          /* LD3 On */
          STM_EVAL_LEDOn(LED3);
					
				  turn_Left();
      }
      else
      {
        if ((int8_t)Buffer[1] < -5.0f)
        {
          /* LD6 on */
          STM_EVAL_LEDOn(LED6);
        }
        if ((int8_t)Buffer[1] > 5.0f)
        {
          /* LD7 On */
          STM_EVAL_LEDOn(LED7);
        } 
      }
			
		/* LEDs Off */
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED10);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED5);
	
}

/**
  * @brief  Configure the Mems to gyroscope application.
  * @param  None
  * @retval None
  */
void Demo_GyroConfig(void)
{
  L3GD20_InitTypeDef L3GD20_InitStructure;
  L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
  
  /* Configure Mems L3GD20 */
  L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
  L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
  L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
  L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
  L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
  L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
  L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500; 
  L3GD20_Init(&L3GD20_InitStructure);
   
  L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
  L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
  L3GD20_FilterConfig(&L3GD20_FilterStructure) ;
  
  L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}

/**
  * @brief  Calculate the angular Data rate Gyroscope.
  * @param  pfData : Data out pointer
  * @retval None
  */
void Demo_GyroReadAngRate (float* pfData)
{
  uint8_t tmpbuffer[6] ={0};
  int16_t RawData[3] = {0};
  uint8_t tmpreg = 0;
  float sensitivity = 0;
  int i =0;

  L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);
  
  L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);
  
  /* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
  if(!(tmpreg & 0x40))
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i] << 8) + tmpbuffer[2*i+1]);
    }
  }
  
  /* Switch the sensitivity value set in the CRTL4 */
  switch(tmpreg & 0x30)
  {
  case 0x00:
    sensitivity=L3G_Sensitivity_250dps;
    break;
    
  case 0x10:
    sensitivity=L3G_Sensitivity_500dps;
    break;
    
  case 0x20:
    sensitivity=L3G_Sensitivity_2000dps;
    break;
  }
  /* divide by sensitivity */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)RawData[i]/sensitivity;
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
