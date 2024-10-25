#include "cst816.h"

const u16 CST816_TPX_TBL[5] ={TP_1_REG,TP_2_REG,TP_3_REG,TP_4_REG,TP_5_REG};
TouchPointRefTypeDef TPR_Structure;

//向CST816写入一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:写数据长度
//返回值:0,成功;1,失败.
u8 CST816_WR_Reg(u8 reg, u8 *buf, u8 len)
{
	return (u8)HAL_I2C_Mem_Write(&tp_i2c, TP_ADDR, reg, 1, buf, len, 1);
}
//从CST816读出一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:读数据长度
u8 CST816_RD_Reg(u8 reg, u8 *buf, u8 len)
{
    return (u8)HAL_I2C_Mem_Read(&tp_i2c, TP_ADDR, reg, 1, buf, len, 1);
}

bool Get_Touch_Status(void)
{
    u8 finger_num = 0;
	CST816_RD_Reg(0x02, &finger_num, 1); //读取触摸点的状态

    if (finger_num > 0)
        return true;
    else
        return false;        
}

u8 Get_Touch_Position(u8 num, int16_t * x, int16_t * y)
{
    u8 buf[6] = {0};
    if (Get_Touch_Status()) //触摸有按下
    {
        CST816_RD_Reg(CST816_TPX_TBL[num], buf, 6); //读取XY坐标值
        U_LOG("%d %d %d %d %d %d",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
        *x = buf[1];
        *y = buf[3];
        return 1;
    }
    *x = 0;
    *y = 0;
    return 0;
}

/*
**函数名：CST816_Init
**传入参数：无
**返回值：无
**功能：初始化CST816引脚
*/
void CST816_Init(void)
{
	u8 temp=0;

	HAL_Delay(110);
	CST816_RD_Reg(Chip_Vendor_ID, &temp, 1);
    if (temp != 0xb6)
    {
        U_LOG("Touchpad init fail:id 0x%x", temp);
        return;
    }
    else
    {
        U_LOG("Touchpad init success :id 0x%x", temp);
    }
	temp = 0;
	CST816_WR_Reg(TP_DEVIDE_MODE, &temp, 1);  //进入正常操作模式
	temp = 1;								  //触摸有效值，22，越小越灵敏
	CST816_WR_Reg(TP_ID_G_THGROUP, &temp, 1); //设置触摸有效值
	temp = 12;								  //激活周期，不能小于12，最大14
	CST816_WR_Reg(TP_ID_G_PERIODACTIVE, &temp, 1);
}
