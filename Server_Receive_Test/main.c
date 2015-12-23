#include "main.h"


void main(void)
	{
		init_Uart();
		while(1){
		Delay2000ms();												//等待身份证模块上电稳定
		UART4_Proccess();
		}
}
/*****************************************/
/* 串口1、4初始化													 */
/* 程序功能：串口1、4初始化								 */
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
	unsigned char dat;
	EA=0;
	    if (S4CON & S4RI)
    {
			dat = S4BUF;
			if(Re_Cnt < 100)
			Re_Buffer[Re_Cnt++] = dat;
      S4CON &= ~S4RI;        																				//串口接收中断标记需软件清零
		}
				
		
		if (S4CON & S4TI)
		{
			S4CON&= ~S4TI;
			Uart4busy=0;
		}
	EA=1;
}
/*----------------------------
串口4处理函数
----------------------------*/
void UART4_Proccess(void){
	bit Header_State;
	unsigned char i,j = 0;
	unsigned char CS = 0x00;
		if(Re_Buffer[0] == 0x55 && Re_Buffer[1] == 0xaa)
			Header_State = 1;
		else
			Header_State = 0;
		if(Header_State == 1){																						//头没有出错
		length = Hex2Int (Re_Buffer[2])*256 + Hex2Int (Re_Buffer[3]);			//获得data的长度，以便计算校验码CS
		for(i=4;i<10;i++){
				if(LocalHostNum[i] == Re_Buffer[i])														//判断是否是本机编号
					j++;
			
		}
			if(j==6){																												//是本机编号
				for	(j = 11;j<length + 11;j++){																		//计算校验码CS
					CS+=Re_Buffer[j];
				}
				if(CS == Re_Buffer[11+length])															//校验码CS是否出错 不出错则进行数据解析
				{
					for(j=0;j<3;j++)
						{
						if(MessageType[j][0] == Re_Buffer[11] && MessageType[j][0] == Re_Buffer[12])
						MessType = j;
					}
				}
			}
		}
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
UART1 中断服务程序
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
void Delay2000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 169;
	j = 24;
	k = 59;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/***********************************************
函数名称：Delay_ms
功    能：STC15系列单片机1ms延时程序
入口参数：ms:延时的毫秒数
返 回 值：无	
备    注：示波器实测：0.997ms，内部时钟：11.0592MHz           
************************************************/
void Delay_ms(unsigned int ms)
{
  
  	unsigned int i;
  	while( (ms--) != 0)
   	{
    	for(i = 0; i < 580; i++); 
   	} 
	
}
void Delay200ms()			//@22.1184MHz
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
