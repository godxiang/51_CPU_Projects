 //文件包含
#include "stc15w4k32s4.h"
#include "intrins.h"

#define FOSC 22118400L          //时钟频率
#define BAUD 115200             //波特率
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4_S0 0x04              //P_SW2.2
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

void init_Uart();
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);
void Uart4_Process(void);
unsigned char htd(unsigned char a);
void Delay1000ms();
void Delay200ms();

unsigned char ReceiveData_Buf[7]; //接收缓冲区

bit Uart4busy;
bit Uart1busy;


void main(void)
	{
		unsigned char n;
		init_Uart();
		while(1){
		Delay1000ms();
		for (n=0;n<10;n++){
			Uart4SendData(FIND_CARD[n]);  
		}

		Uart4_Process();		
		}
	}
/*****************************************/
/* 串口4初始化													 */
/* 程序功能：串口4初始化								 */
/* 入口参数:无												   */
/* 出口参数:无												   */
/*****************************************/
void init_Uart()
	{
		P_SW2 &= ~S4_S0;            //S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
		ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		SCON = 0x50;								//1010000
		S4CON = 0x10;             	//10000
    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;								//T2为1T模式, 并启动定时器2 10100
		AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
    ES = 1;                     //使能串口1中断
    IE2 = 0x10;             		//使能串口4中断
    EA = 1;
}

/*----------------------------
UART4 中断服务程序
-----------------------------*/
void Uart4_ISR() interrupt 18 using 1
{
		EA=0;
	    if (S4CON & S4RI)
    {
			if(i<300){
			re_data_buffer[i++]=S4BUF;
			}	
        S4CON &= ~S4RI;        												//串口接收中断标记需软件清零
    }
				
		
		if (S4CON & S4TI)
		{
			S4CON&= ~S4TI;
			Uart4busy=0;
		}
		EA=1;
}
/***********************************************
*函数名称：Uart4_Process
*功    能：串口命令处理函数
*入口参数：无
*返 回 值：无	
*备    注：无
************************************************/
void Uart4_Process(void)  
{

}



/*----------------------------
串口4发送数据
----------------------------*/
void Uart4SendData(unsigned char Udat)
{
	while(Uart4busy);
	ACC = Udat;
	Uart4busy=1;
  S4BUF = ACC;
}


void Uart4SendString(char *s)
{
    while (*s)                 
    {
         Uart4SendData(*s++);         
    }
}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart1_ISR() interrupt 4 using 1
{
	EA=0;
    if (RI)
    {
        RI = 0;                 //清除RI位
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        Uart1busy = 0;               //清忙标志
    }
	EA=1;
}

/*----------------------------
发送串口数据
----------------------------*/
void Uart1SendData(unsigned char dat)
{
    while (Uart1busy);               //等待前面的数据发送完成
    Uart1busy = 1;
    SBUF = dat;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void Uart1SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        Uart1SendData(*s++);         //发送当前字符
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
