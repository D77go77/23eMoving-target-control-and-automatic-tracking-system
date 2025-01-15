#include "control.h"

u8 Step_Next_Flag_X=1;
u8 Step_Next_Flag_Y=1;
u8 Step_Lock_Flag=0;

extern int UART_printf(uint32_t ui32Base, const char *fmt, ...);

/**
 * 将直角坐标转换为球面度数
 * 
 * 该函数将给定的直角坐标转换为球面的度数表示。
 * 它使用了数学库中的反正切函数来计算角度，并将结果以度数形式返回。
 * 
 * @param P0S_X 直角坐标系中的X坐标
 * @param P0S_Y 直角坐标系中的Y坐标
 * @param DEG_X 指向存储计算后X方向球面度数的浮点数的指针
 * @param DEG_Y 指向存储计算后Y方向球面度数的浮点数的指针
 */

void Straight_2_Sphere_Degree(int16_t P0S_X, int16_t P0S_Y, float *DEG_X, float *DEG_Y) 
{
    *DEG_X = (atanf((float) Screen_Dis / (float) (Green_Dis - P0S_X)) - atanf((float) Screen_Dis / (float) Green_Dis))
             * 180.0f / (float) M_PI;
    *DEG_Y = atanf((float) P0S_Y / sqrtf((float) P0S_X * (float) P0S_X + (float) Screen_Dis * (float) Screen_Dis))
             * 180.0f / (float) M_PI;
}


void POS_2_Pulse(float DEG_X, float DEG_Y, int16_t *Pulse_X_Num, int16_t *Pulse_Y_Num) {
    *Pulse_X_Num = (int16_t) (DEG_X * 32.0f / 1.8f);
    *Pulse_Y_Num = (int16_t) (DEG_Y * 32.0f / 1.8f);
}

/**
 * 分割位置点
 * 
 * 该函数将四个输入位置点进行细分，生成一个更细致的点集。每个输入点和它相邻的点之间
 * 的线段被分为10个等分点，加上起始点和结束点，共计41个点。
 * 
 * @param Input_POS 一个二维数组，包含4个位置点的坐标。每个位置点由x和y坐标组成。
 * @param Output_POS 一个二维数组，用于存储细分后的坐标点。数组大小应为41行×2列，每行代表一个点的坐标。
 */
void Position_Subdivide(const int16_t Input_POS[4][2], int16_t Output_POS[41][2]) 
{
    // 遍历每一个输入点
    for (uint8_t i = 0; i < 4; i++) 
    {
        // 在每个点和它的下一个点之间进行10等分
        for (uint8_t j = 0; j < 10; j++) 
        {
            // 计算等分点的x坐标
            Output_POS[i * 10 + j][0] = (int16_t) ((float) Input_POS[i][0] +
                                                   (float) j / 10.0f * (float) (Input_POS[(i + 1) % 4][0] - Input_POS[i][0]));
            // 计算等分点的y坐标
            Output_POS[i * 10 + j][1] = (int16_t) ((float) Input_POS[i][1] +
                                                   (float) j / 10.0f * (float) (Input_POS[(i + 1) % 4][1] - Input_POS[i][1]));
        }
    }
    // 将第一个点复制到输出数组的最后一个位置，用于闭合路径
    Output_POS[40][0] = Input_POS[0][0];
    Output_POS[40][1] = Input_POS[0][1];
}
