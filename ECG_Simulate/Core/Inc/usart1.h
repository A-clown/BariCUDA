#ifndef __USART1_H
#define __USART1_H

#include "main.h"

extern UART_HandleTypeDef huart1; /* 串口1句柄 */

void USART1_Init(void);
void USART1_Process(void);

#endif
