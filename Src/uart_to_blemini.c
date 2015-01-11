#include "uart_to_blemini.h"

USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef  GPIO_InitStructure_UART;

uint8_t rx_counter = 0;
uint8_t rx_buffer[RX_BUFFER_LENGTH];

char* tokens;


/*
 Enable UART and RX/TX initialisation
 */
void initUART(void){
    
    
    /* Enable USART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
    
    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
    
    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure_UART.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure_UART.GPIO_Mode = GPIO_Mode_AF;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure_UART.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure_UART);
    
    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure_UART.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure_UART);
    
    
    /* USART configuration */
    USART_InitStructure.USART_BaudRate = 57600 ;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    
    /* Enable the USART2 Receive interrupt: this interrupt is generated when the
     USART2 receive data register is not empty */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable Interrupt USART */
    NVIC_EnableIRQ(USART2_IRQn);
    
    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
    
    
}


/**
 * @brief  This function handles USART2 global interrupt request.
 * @param  None
 * @retval None
 */
void USART2_IRQHandler(void)
{
    
    xGyroPhoneValue = 500;
    yGyroPhoneValue = 500;
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        if((USART_ReceiveData(USART2) & 0x7F) == '/')
        {
            tokens = strtok ((char*)rx_buffer,",");
            while (tokens != NULL)
            {
                if (xGyroPhoneValue==500)
                {
                    xGyroPhoneValue = atoi(tokens);
                }
                else
                {
                    yGyroPhoneValue = atoi(tokens);
                }
                tokens = strtok (NULL, ",");
            }
            
            memset(rx_buffer, 0, RX_BUFFER_LENGTH);
            rx_counter = 0;
            
            printf("go");
        }
        else
        {
            rx_buffer[rx_counter] = (USART_ReceiveData(USART2) & 0x7F);
            rx_counter++;
        }
        
    }
}



PUTCHAR_PROTOTYPE
{
    /* Put character on the serial line */
    USART2->TDR = (ch & (uint16_t)0x01FF);
    
    /* Loop until transmit data register is empty */
    while ((USART2->ISR & USART_FLAG_TXE) == (uint16_t) RESET);
    
    return ch;
}



uint8_t USART2_ReadChar()
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART2);
    
    
}

char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}


char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    
    /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
    count++;
    
    result = malloc(sizeof(char*) * count);
    
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    
    return result;
}

