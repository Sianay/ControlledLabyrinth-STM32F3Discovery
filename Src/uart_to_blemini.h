#ifndef __UART_TO_BLEMINI_H
#define __UART_TO_BLEMINI_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "stm32f30x.h"
#include "stm32f30x_it.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x.h"
#include "servo.h"
#include "main.h"


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define RX_BUFFER_LENGTH    (100)

void initUART(void);

extern PUTCHAR_PROTOTYPE;
extern uint8_t USART2_ReadChar();

#endif
