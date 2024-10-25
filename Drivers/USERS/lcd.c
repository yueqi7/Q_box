#include "lcd.h"
#include "gpio.h"
#include "main.h"

u16 BACK_COLOR; //背景色


/******************************************************************************
      函数说明：硬件SPI
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
u8 LCD_SPI_ReadWrite_Bus(u8 dat) 
{		
	u8 rx_byte = 0;
	HAL_SPI_TransmitReceive(&lcd_spi, &dat, &rx_byte,1,1);
	while(HAL_SPI_GetState(&lcd_spi) != HAL_SPI_STATE_READY);
	return rx_byte;
}

/******************************************************************************
      函数说明：硬件SPI
      入口数据：dat  要写入的串行数据序列
      返回值：  无
******************************************************************************/
void LCD_SPI_WriteBuffer_Bus(const u8 * tx, u32 size) 
{
	HAL_SPI_Transmit(&lcd_spi, tx, size, size/1000 + 1);
	while(HAL_SPI_GetState(&lcd_spi) != HAL_SPI_STATE_READY);
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Clr();//写命令
	LCD_SPI_ReadWrite_Bus(dat);
	
	LCD_CS_Set();
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Set();//写数据
	LCD_SPI_ReadWrite_Bus(dat);
	
	LCD_CS_Set();
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_CS_Clr();
	LCD_DC_Set();//写数据
	LCD_SPI_ReadWrite_Bus(dat>>8);
	LCD_SPI_ReadWrite_Bus(dat);
	LCD_CS_Set();
}

/******************************************************************************
	  函数说明：设置起始和结束地址
	  入口数据：x1,x2 设置列的起始和结束地址
				y1,y2 设置行的起始和结束地址
	  返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u8 x[4]={x1>>8,x1,x2>>8,x2};
	u8 y[4]={y1>>8,y1,y2>>8,y2};
	LCD_CS_Clr();
	LCD_DC_Clr();//写命令
	LCD_SPI_ReadWrite_Bus(0x2a);
	LCD_DC_Set();//写数据
	LCD_SPI_WriteBuffer_Bus(x, 4);
	LCD_DC_Clr();//写命令
	LCD_SPI_ReadWrite_Bus(0x2b);
	LCD_DC_Set();//写数据
	LCD_SPI_WriteBuffer_Bus(y, 4);
	LCD_DC_Clr();//写命令
	LCD_SPI_ReadWrite_Bus(0x2c);
	LCD_CS_Set();
}
/******************************************************************************
	  函数说明：LCD清屏函数
	  入口数据：无
	  返回值：  无
******************************************************************************/
void LCD_Clear(u16 Color)
{
	u16 i;
	u8 buffer[LCD_W*2];
	LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
	LCD_DC_Set();//写数据
	for (i = 0; i < sizeof(buffer); )
	{
			buffer[i]=Color>>8;
			buffer[i+1]=Color;
			i += 2;
	}
	
	for (i = 0; i < LCD_H; i++)
	{
		LCD_CS_Clr();
		LCD_SPI_WriteBuffer_Bus(buffer, sizeof(buffer));
		LCD_CS_Set();
	}
}
/******************************************************************************
	  函数说明：LCD显示汉字
	  入口数据：x,y   起始坐标
	  返回值：  无
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_Address_Set(x, y, x, y); //设置光标位置
	LCD_WR_DATA(color);
}

/******************************************************************************
	  函数说明：在指定区域填充颜色
	  入口数据：xsta,ysta   起始坐标
				xend,yend   终止坐标
	  返回值：  无
******************************************************************************/
void LCD_Fill_Color(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
	u16 i;
	u16 buffer_range = (xend - xsta + 1) * 2;
	u8 buffer[LCD_W*2];
	LCD_Address_Set(xsta, ysta, xend, yend); //设置光标位置
	LCD_DC_Set();//写数据
	
	for (i = 0; i <= buffer_range; i+=2)
	{
		buffer[i]=color>>8;
		buffer[i+1]=color;
	}

	LCD_CS_Clr();
	for (i = ysta; i <= yend; i++)
	{
		LCD_SPI_WriteBuffer_Bus(buffer, buffer_range);
	}
	LCD_CS_Set();
}
/******************************************************************************
	  函数说明：在指定区域填充颜色
	  入口数据：xsta,ysta   起始坐标
				xend,yend   终止坐标
	  返回值：  无
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u8 * buffer)
{
	u16 i;
	u16 buffer_range = (xend - xsta + 1) * 2;
	LCD_Address_Set(xsta, ysta, xend, yend); //设置光标位置
	LCD_DC_Set();//写数据

	LCD_CS_Clr();
	for (i = ysta; i <= yend; i++)
	{
		LCD_SPI_WriteBuffer_Bus(buffer, buffer_range);
	}
	LCD_CS_Set();
}
/******************************************************************************
	  函数说明：画线
	  入口数据：x1,y1   起始坐标
				x2,y2   终止坐标
	  返回值：  无
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; //画线起点坐标
	uCol = y1;
	if (delta_x > 0)
		incx = 1; //设置单步方向
	else if (delta_x == 0)
		incx = 0; //垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; //水平线
	else
	{
		incy = -1;
		delta_y = -delta_x;
	}
	if (delta_x > delta_y)
		distance = delta_x; //选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t < distance + 1; t++)
	{
		LCD_DrawPoint(uRow, uCol, color); //画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
/******************************************************************************
	  函数说明：画矩形
	  入口数据：x1,y1   起始坐标
				x2,y2   终止坐标
	  返回值：  无
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	LCD_DrawLine(x1, y1, x2, y1, color);
	LCD_DrawLine(x1, y1, x1, y2, color);
	LCD_DrawLine(x1, y2, x2, y2, color);
	LCD_DrawLine(x2, y1, x2, y2, color);
}
/******************************************************************************
	  函数说明：画圆
	  入口数据：x0,y0   圆心坐标
				r       半径
	  返回值：  无
******************************************************************************/
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
	int a, b;
	//int di;
	a = 0;
	b = r;
	while (a <= b)
	{
		LCD_DrawPoint(x0 - b, y0 - a, color); // 3
		LCD_DrawPoint(x0 + b, y0 - a, color); // 0
		LCD_DrawPoint(x0 - a, y0 + b, color); // 1
		LCD_DrawPoint(x0 - a, y0 - b, color); // 2
		LCD_DrawPoint(x0 + b, y0 + a, color); // 4
		LCD_DrawPoint(x0 + a, y0 - b, color); // 5
		LCD_DrawPoint(x0 + a, y0 + b, color); // 6
		LCD_DrawPoint(x0 - b, y0 + a, color); // 7
		a++;
		if ((a * a + b * b) > (r * r)) //判断要画的点是否过远
		{
			b--;
		}
	}
}
/******************************************************************************
	  函数说明：LCD初始化函数
	  入口数据：无
	  返回值：  无
******************************************************************************/
void Lcd_Init(void)
{
	LCD_SPI_ReadWrite_Bus(0);

	LCD_RST_Clr();
	HAL_Delay(20);
	LCD_RST_Set();
	HAL_Delay(20);
    
	LCD_WR_REG(0x11);     

	HAL_Delay(120);                

	LCD_WR_REG(0x36); //MX, MY, RGB mode 
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);  

	LCD_WR_REG(0x3A);     
	LCD_WR_DATA8(0x05);   

	LCD_WR_REG(0xB2);     
	LCD_WR_DATA8(0x1F);   
	LCD_WR_DATA8(0x1F);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x33);   
	LCD_WR_DATA8(0x33);   

	LCD_WR_REG(0xB7);     
	LCD_WR_DATA8(0x00);   

	LCD_WR_REG(0xBB);     
	LCD_WR_DATA8(0x36);   

	LCD_WR_REG(0xC2);     
	LCD_WR_DATA8(0x01);   

	LCD_WR_REG(0xC3);     
	LCD_WR_DATA8(0x13);  

	LCD_WR_REG(0xC4);     
	LCD_WR_DATA8(0x20);   //VDV, 0x20:0v

	LCD_WR_REG(0xC6);     
	LCD_WR_DATA8(0x13);   //0x0F:60Hz        

	LCD_WR_REG(0xD6);     
	LCD_WR_DATA8(0xA1);   	

	LCD_WR_REG(0xD0);     
	LCD_WR_DATA8(0xA4);   
	LCD_WR_DATA8(0xA1);  

	LCD_WR_REG(0x2A); //Column Address Set
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00); //0
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xEF); //239

	LCD_WR_REG(0x2B); //Row Address Set
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00); //0
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xEF); //239 

	LCD_WR_REG(0xE0);     
	LCD_WR_DATA8(0xF0);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x0E);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x2F);   
	LCD_WR_DATA8(0x33);   
	LCD_WR_DATA8(0x45);   
	LCD_WR_DATA8(0x36);   
	LCD_WR_DATA8(0x13);   
	LCD_WR_DATA8(0x12);   
	LCD_WR_DATA8(0x2A);   
	LCD_WR_DATA8(0x2D);   

	LCD_WR_REG(0xE1);     
	LCD_WR_DATA8(0xF0);   
	LCD_WR_DATA8(0x0E);   
	LCD_WR_DATA8(0x12);   
	LCD_WR_DATA8(0x0C);   
	LCD_WR_DATA8(0x0A);   
	LCD_WR_DATA8(0x15);   
	LCD_WR_DATA8(0x2E);   
	LCD_WR_DATA8(0x32);   
	LCD_WR_DATA8(0x44);   
	LCD_WR_DATA8(0x39);   
	LCD_WR_DATA8(0x17);   
	LCD_WR_DATA8(0x18);   
	LCD_WR_DATA8(0x2B);   
	LCD_WR_DATA8(0x2F);   

	LCD_WR_REG(0xE4);    
	LCD_WR_DATA8(0x1D);
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x00); 

	LCD_WR_REG(0x21);     
	LCD_WR_REG(0x29);  

	LCD_WR_REG(0xc7);    
	LCD_WR_DATA8(0x04);

	LCD_WR_REG(0xcc);    
	LCD_WR_DATA8(0x18);

	LCD_Clear(0XFF00);
	HAL_TIM_PWM_Start(&lcd_pwm,TIM_CHANNEL_4);
	LCD_BLK_Set(100);

    U_LOG("Lcd_Init Finished.");
}
