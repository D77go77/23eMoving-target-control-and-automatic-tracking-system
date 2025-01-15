
//----------------------------------------------------------------
//GND    电源地
//VCC  接5V或3.3v电源
//D0   接PB13（SCL）
//D1   接PB15（SDA）
//RES  接PB3
//DC   接PA15
//CS   接PB12              
//----------------------------------------------------------------
//******************************************************************************/

#include "u8g2_oled.h"
#include "stdlib.h"
#include "u8g2_oled_font.h"
#include "spi.h"
#include "u8g2.h"
#include "u8x8.h"
uint8_t OLED_Refresh = 0;
uint8_t Buffer1[32], Buffer2[32];

void draw(u8g2_t *u8g2)
{
	char tmp[30];char tmp2[30];
	
    u8g2_DrawLine(u8g2, 0, 0, 127, 0);  //画直线
    u8g2_DrawLine(u8g2, 0, 0, 0, 63);
    u8g2_DrawLine(u8g2, 0, 63, 127, 63);    
    u8g2_DrawLine(u8g2, 127, 0, 127, 63);
    
//    u8g2_DrawLine(u8g2, 0, 0, 127, 63); //画直线
//    u8g2_DrawLine(u8g2, 0, 63, 127, 0);

//    u8g2_DrawPixel(u8g2, 115, 32); //画点
//    u8g2_DrawBox(u8g2,56,44,14,10);   //画实心矩形
//    u8g2_DrawCircle(u8g2,62,13,10,U8G2_DRAW_ALL); //画空心矩形
//    u8g2_DrawFrame(u8g2,5,24,18,16);  //画空心圆形

	u8g2_SetFont(u8g2, u8g2_font_6x12_tf);  //设定字体
	
	sprintf_(tmp, "Mode:%d", Mode);
	u8g2_DrawStr(u8g2, 10, 12,  tmp);
	sprintf_(tmp2, "f_trace:%d", f_trace);
	u8g2_DrawStr(u8g2, 10, 26, tmp2);
//	sprintf_((char *) Buffer1, "Buzzer:%d", Buzzer_Enable);
//	u8g2_DrawStr(u8g2, 0, 40, (char *) Buffer1);

}
/********************************* end_of_file **********************************/

uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_BYTE_SEND: /*通过SPI发送arg_int个字节数据*/
			HAL_SPI_Transmit(&hspi2,(uint8_t *)arg_ptr,arg_int,200);
            break;
        case U8X8_MSG_BYTE_INIT: /*初始化函数*/
            break;
        case U8X8_MSG_BYTE_SET_DC: /*设置DC引脚,表明发送的是数据还是命令*/
			HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER: 
            break;
        case U8X8_MSG_BYTE_END_TRANSFER: 
            break;
        default:
            return 0;
    }
    return 1;
}

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
    U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
    U8X8_UNUSED void *arg_ptr) 
{
    switch (msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: /*delay和GPIO的初始化，在main中已经初始化完成了*/
            break;
        case U8X8_MSG_DELAY_MILLI: /*延时函数*/
            HAL_Delay(arg_int);     //调用谁stm32系统延时函数
            break;
        case U8X8_MSG_GPIO_CS: /*片选信号*/ //由于只有一个SPI设备，所以片选信号在初始化时已经设置为为常有效
            break;
        case U8X8_MSG_GPIO_DC: /*设置DC引脚,表明发送的是数据还是命令*/
            HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:
            break;
    }
    return 1;
}

void u8g2Init(u8g2_t *u8g2)
{
	/********************************************     
	U8G2_R0     //不旋转，不镜像     
	U8G2_R1     //旋转90度
	U8G2_R2     //旋转180度   
	U8G2_R3     //旋转270度
	U8G2_MIRROR   //没有旋转，横向显示左右镜像
	U8G2_MIRROR_VERTICAL    //没有旋转，竖向显示镜像
	********************************************/
//    u8g2_Setup_sh1106_128x64_noname_2(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // 初始化1.3寸OLED u8g2 结构体
	MD_OLED_RST_Set(); //复位拉高
    u8g2_Setup_ssd1306_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // 初始化0.96寸OLED u8g2 结构体
	u8g2_InitDisplay(u8g2);     //初始化显示
	u8g2_SetPowerSave(u8g2, 0); //开启显示
}


