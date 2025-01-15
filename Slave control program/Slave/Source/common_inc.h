#ifndef __COMMON_INC_H_
#define __COMMON_INC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>


typedef   signed           char int8;
typedef unsigned           char u8;
typedef unsigned           char uint8;
typedef unsigned           char byte;
typedef   signed short     int int16;
typedef unsigned short     int uint16;
typedef unsigned short     int u16;
typedef unsigned long      int u32; 
typedef signed       	char 	s8;
typedef signed short 	int 	s16;
typedef signed       	int 	s32;

//**************************
//***LIBRARY****************
//**************************
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include "spi.h"

#include "interrupt.h"
#include "printf.h"
//**************************
//***SYSTEM ****************
//**************************
#include "ComKey.h"
#include "AT24.h"
#include "u8g2_oled.h"
#include "u8g2.h"
#include "u8x8.h"
#include "vofa.h"
//**************************
//***HARDWARE **************
//**************************
#include "Led.h"
#include "StreamParser.h"
#include "As5600.h"
#include "StepHelper.h"
#include "uart.h"
//**************************
//***TASK     **************
//**************************
#include "control.h"
#include "Mode.h"
#include "Pid.h"

void mainloop(void);
void task_mainloop(void);

#endif 
