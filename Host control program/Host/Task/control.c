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
//    *DEG_X = atanf((float) P0S_X / sqrtf((float) P0S_Y * (float) P0S_Y + (float) Screen_Dis * (float) Screen_Dis))
//             * 180.0f / (float) M_PI;
//    *DEG_Y = atanf((float) P0S_Y / sqrtf((float) P0S_X * (float) P0S_X + (float) Screen_Dis * (float) Screen_Dis))
//             * 180.0f / (float) M_PI;
	*DEG_X  = atanf((float)P0S_X /(float) Screen_Dis)* 180.0f / (float) M_PI;
	*DEG_Y  = atanf((float)P0S_Y /(float) Screen_Dis)* 180.0f / (float) M_PI;
}

/**
 * 计算位置到脉冲数的转换
 * 
 * 该函数根据当前位置和目标位置，计算需要的脉冲数。
 * 它首先将坐标转换为球面度数，然后根据角度的变化计算脉冲数。
 * 脉冲数的计算考虑了角度变化与脉冲之间的比例关系。
 * 
 * @param P0S_X_Cur 当前位置的X坐标
 * @param P0S_Y_Cur 当前位置的Y坐标
 * @param P0S_X_TAR 目标位置的X坐标
 * @param P0S_Y_TAR 目标位置的Y坐标
 * @param Pulse_X_Num 指向存储计算后X方向脉冲数的整数的指针
 * @param Pulse_Y_Num 指向存储计算后Y方向脉冲数的整数的指针
 */
void POS_2_Pulse(int16_t P0S_X_Cur, int16_t P0S_Y_Cur, 
                 int16_t P0S_X_TAR, int16_t P0S_Y_TAR, 
                 int16_t *Pulse_X_Num, int16_t *Pulse_Y_Num) 
{
   float DEG_X_Cur, DEG_Y_Cur, DEG_X_Tar, DEG_Y_Tar;

    Straight_2_Sphere_Degree(P0S_X_Cur, P0S_Y_Cur, &DEG_X_Cur, &DEG_Y_Cur);
    Straight_2_Sphere_Degree(P0S_X_TAR, P0S_Y_TAR, &DEG_X_Tar, &DEG_Y_Tar);

   *Pulse_X_Num = (int16_t) ((DEG_X_Tar - DEG_X_Cur) * 32.0f / 1.8f);
   *Pulse_Y_Num = (int16_t) ((DEG_Y_Tar - DEG_Y_Cur) * 32.0f / 1.8f);
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
