#include "reg51.h"
#include "intrins.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 22118400L          //系统频率
#define BAUD 115200             //串口波特率

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位

sfr AUXR  = 0x8e;               //辅助寄存器
sfr S3CON = 0xac;               //UART3 控制寄存器
sfr S3BUF = 0xad;               //UART3 数据寄存器
sfr T2H   = 0xd6;               //定时器2高8位
sfr T2L   = 0xd7;               //定时器2低8位
sfr IE2   = 0xaf;               //中断控制寄存器2
sbit P36 = P3^6;

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3

sfr P_SW2   = 0xBA;             //外设功能切换寄存器2
#define S3_S0 0x02              //P_SW2.1

bit busy;

//void SendData(BYTE dat);
//void SendString(char *s);
//void delay();
void UartInit(void);
void Delay1000ms();
void main()
{
	UartInit();
	while(1)
 {
	Delay1000ms();
 S3BUF=0x55;    //01010101
 while(!(S3CON&S3TI));		//00000010
 S3CON =S3CON & ~S3TI; 
 }
}
void UartInit(void)
{
	S3CON = 0x10;		
	S3CON &= 0xBF;		
	AUXR &= 0xFB;		
	T2L = 0xD0;		
	T2H = 0xFF;		
	AUXR |= 0x10;		
}
void Delay1000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 85;
	j = 12;
	k = 155;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}







