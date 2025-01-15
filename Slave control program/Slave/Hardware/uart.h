#ifndef __UART_H
#define __UART_H

#include "common_inc.h"


extern u8 RX2_Data[50];

extern DMA_HandleTypeDef hdma_usart2_rx;

extern u16 data[4];

void uart_proc(void) ;
extern u8 uart_f;
extern u8 uart_f2;

extern u8 uart_cnt;
extern u8 uart2_cnt;

extern u8 rx1_index;
extern u8 rx_index;
#endif
