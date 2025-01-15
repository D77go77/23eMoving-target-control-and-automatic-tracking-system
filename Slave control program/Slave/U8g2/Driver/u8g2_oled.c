
//----------------------------------------------------------------
//GND    ��Դ��
//VCC  ��5V��3.3v��Դ
//D0   ��PB13��SCL��
//D1   ��PB15��SDA��
//RES  ��PB3
//DC   ��PA15
//CS   ��PB12              
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
	
    u8g2_DrawLine(u8g2, 0, 0, 127, 0);  //��ֱ��
    u8g2_DrawLine(u8g2, 0, 0, 0, 63);
    u8g2_DrawLine(u8g2, 0, 63, 127, 63);    
    u8g2_DrawLine(u8g2, 127, 0, 127, 63);
    
//    u8g2_DrawLine(u8g2, 0, 0, 127, 63); //��ֱ��
//    u8g2_DrawLine(u8g2, 0, 63, 127, 0);

//    u8g2_DrawPixel(u8g2, 115, 32); //����
//    u8g2_DrawBox(u8g2,56,44,14,10);   //��ʵ�ľ���
//    u8g2_DrawCircle(u8g2,62,13,10,U8G2_DRAW_ALL); //�����ľ���
//    u8g2_DrawFrame(u8g2,5,24,18,16);  //������Բ��

	u8g2_SetFont(u8g2, u8g2_font_6x12_tf);  //�趨����
	
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
        case U8X8_MSG_BYTE_SEND: /*ͨ��SPI����arg_int���ֽ�����*/
			HAL_SPI_Transmit(&hspi2,(uint8_t *)arg_ptr,arg_int,200);
            break;
        case U8X8_MSG_BYTE_INIT: /*��ʼ������*/
            break;
        case U8X8_MSG_BYTE_SET_DC: /*����DC����,�������͵������ݻ�������*/
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
        case U8X8_MSG_GPIO_AND_DELAY_INIT: /*delay��GPIO�ĳ�ʼ������main���Ѿ���ʼ�������*/
            break;
        case U8X8_MSG_DELAY_MILLI: /*��ʱ����*/
            HAL_Delay(arg_int);     //����˭stm32ϵͳ��ʱ����
            break;
        case U8X8_MSG_GPIO_CS: /*Ƭѡ�ź�*/ //����ֻ��һ��SPI�豸������Ƭѡ�ź��ڳ�ʼ��ʱ�Ѿ�����ΪΪ����Ч
            break;
        case U8X8_MSG_GPIO_DC: /*����DC����,�������͵������ݻ�������*/
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
	U8G2_R0     //����ת��������     
	U8G2_R1     //��ת90��
	U8G2_R2     //��ת180��   
	U8G2_R3     //��ת270��
	U8G2_MIRROR   //û����ת��������ʾ���Ҿ���
	U8G2_MIRROR_VERTICAL    //û����ת��������ʾ����
	********************************************/
//    u8g2_Setup_sh1106_128x64_noname_2(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // ��ʼ��1.3��OLED u8g2 �ṹ��
	MD_OLED_RST_Set(); //��λ����
    u8g2_Setup_ssd1306_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // ��ʼ��0.96��OLED u8g2 �ṹ��
	u8g2_InitDisplay(u8g2);     //��ʼ����ʾ
	u8g2_SetPowerSave(u8g2, 0); //������ʾ
}


