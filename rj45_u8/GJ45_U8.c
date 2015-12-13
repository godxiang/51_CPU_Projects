
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
sfr P_SW1   = 0xA2;
sbit P36 = P3^6;
#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define S3_S0 0x02              //P_SW2.1
sfr P_SW2   = 0xBA;             //外设功能切换寄存器2


bit busy;
bit busy0;

void SendData(BYTE dat);
void SendString(char *s);
void Uart1SendData(BYTE dat);
void Uart1SendString(char *s);
void Delay1000ms();
void Delay200ms();

void main()
{
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		SCON = 0x50;
		S3CON = 0x10;
	
/*    P_SW2 &= ~S3_S0;            //S3_S0=0 (P0.0/RxD3, P0.1/TxD3)
//  P_SW2 |= S3_S0;             //S3_S0=1 (P5.0/RxD3_2, P5.1/TxD3_2)
#if (PARITYBIT == NONE_PARITY)
		SCON = 0x50;     
    S3CON = 0x10;               //8位可变波特率

#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    S3CON = 0x9a;               //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
    S3CON = 0x92;               //9位可变波特率,校验位初始为0
#endif
*/
		AUXR = 0x40;               //定时器1为1T模式
    TMOD = 0x20;                //定时器1为模式2(8位自动重载)
    TL1 = (256 - (FOSC/32/BAUD));   //设置波特率重装值
    TH1 = (256 - (FOSC/32/BAUD));
    TR1 = 1;                   //定时器1开始工作
    ES = 1;                     //使能串口中断
    
		T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
		AUXR = 0x54;                //T2为1T模式, 并启动定时器2
    IE2 = 0x08;                 //使能串口3中断
    EA = 1;

    
    while(1)
		{
			SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			//Uart1SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			P36=1;
			Delay1000ms();
			P36=0;
			Delay200ms();
		}
}

/*----------------------------
UART3 中断服务程序
-----------------------------*/
void Uart3() interrupt 17 using 1
{
    if (S3CON & S3RI)
    {
        S3CON &= ~S3RI;         //清除S3RI位
				Uart1SendData(S3BUF);
        //Uart1SendData(S3BUF);             //P30显示串口数据
			  //SendString("interrupt !\r\n");
			
    }
    if (S3CON & S3TI)
    {
        S3CON &= ~S3TI;         //清除S3TI位
        busy = 0;               //清忙标志
    }
}

/*----------------------------
UART1 中断服务程序
-----------------------------*/
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                                        
    }
    if (TI)
    {
        TI = 0;              
        busy0 = 0;            
    }
}

/*----------------------------
发送串口数据
----------------------------*/

void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    busy = 1;
    S3BUF = dat;                //写数据到UART3数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/

void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}

void Uart1SendData(BYTE dat)
{
    while (busy0);               //???????????
    ACC = dat;                  //?????P (PSW.0)
    if (P)                      //??P??????
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //??????0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //??????1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //??????1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //??????0
#endif
    }
    busy0 = 1;
    SBUF = ACC;                 //????UART?????
}

void Uart1SendString(char *s)
{
    while (*s)                 
    {
         Uart1SendData(*s++);         
    }
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

void Delay200ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


