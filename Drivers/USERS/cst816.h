#ifndef __CST816_H_
#define __CST816_H_

#include "main.h"

/*********************IO操作函数*********************/
#define TP_RST_Clr() HAL_GPIO_WritePin(TP_RST_GPIO_Port,TP_RST_Pin, GPIO_PIN_RESET)//RES
#define TP_RST_Set() HAL_GPIO_WritePin(TP_RST_GPIO_Port,TP_RST_Pin, GPIO_PIN_SET)

#define TP_ADDR 		0x2A 	//device addr
#define I2C_WR 			0x0		/* 写控制bit */
#define I2C_RD 			0x1		/* 读控制bit */
#define TP_PRES_DOWN 	0x81 	//触屏被按下
#define TP_COORD_UD 	0x40	//触摸坐标更新标记

// I2C读写命令
#define TP_WR_ADDR 		0X2A 	//写命令
#define TP_RD_ADDR 		0X2B 	//读命令
// CST816 部分寄存器定义
#define TP_DEVIDE_MODE 		0x00	// CST816模式控制寄存器
#define TP_REG_NUM_FINGER 	0x02 	//触摸状态寄存器

#define TP_1_REG 		0X03 //第一个触摸点数据地址
#define TP_2_REG 		0X09 //第二个触摸点数据地址
#define TP_3_REG 		0X0F //第三个触摸点数据地址
#define TP_4_REG 		0X15 //第四个触摸点数据地址
#define TP_5_REG 		0X1B //第五个触摸点数据地址

#define TP_ID_G_LIB_VERSION 	0xA1	//版本
#define TP_ID_G_MODE 			0xA4	// CST816中断模式控制寄存器
#define TP_ID_G_THGROUP 		0x80	//触摸有效值设置寄存器
#define TP_ID_G_PERIODACTIVE	0x88 	//激活状态周期设置寄存器
#define Chip_Vendor_ID 			0xA7	//芯片ID(0x36)
#define ID_G_CST816ID 			0xA8	// 0x11

//触摸点相关数据结构体定义
typedef struct
{
	u8 TouchSta; //触摸情况，b7:按下1/松开0; b6:0没有按键按下/1有按键按下;bit5:保留；bit4-bit0触摸点按下有效标志，有效为1，分别对应触摸点5-1；
	u16 x[5];	 //支持5点触摸，需要使用5组坐标存储触摸点数据
	u16 y[5];

} TouchPointRefTypeDef;
extern TouchPointRefTypeDef TPR_Structure;


void CST816_Init(void);
bool Get_Touch_Status(void);
u8 Get_Touch_Position(u8 num, int16_t * x, int16_t * y);

#endif
