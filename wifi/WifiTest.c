#include "stc15w4k32s4.h"
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

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4RB8 0x04              //S4CON.2
#define S4TB8 0x08              //S4CON.3
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define S3_S0 0x02              //P_SW2.1
#define S4_S0 0x04
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80             
sbit Transmit_DATA0 =P2^0;
sbit Transmit_DATA1 =P2^1;
sbit Transmit_DATA2 =P2^2;
sbit Transmit_DATA3 =P2^3;
sbit Transmit_U2STB =P2^6;/*当发送数据时，需要先使此位置高，默认为输出，低电平*/
sbit Transmit_U2REQ =P2^7;/*当接收到来自其他机器的高电平信号时，默认为输出*/
sbit Transmit_U1STB =P2^4;/*当其他机器向本机发送数据时，默认为输入*/
sbit Transmit_U1REQ =P2^5;/*当其他机器向本机发送数据时，默认为输入*/
sbit Transmit_U2BusACK =P1^3;/*默认为输出，忙信号*/
sbit Transmit_U1BusACK =P1^2;/*默认输入，忙信号检测*/



bit Uart1busy;
bit Uart3busy;
bit Uart4busy;

void Uart1SendData(BYTE dat);
void Uart1SendString(char *s);
void Uart3SendData(BYTE dat);
void Uart3SendString(char *s);
void Uart4SendData(BYTE dat);
void Uart4SendString(char *s);
unsigned char Sent_Byte(unsigned char Data0);
unsigned char Read_Byte(void);
void Transmit_init(void);
void Delay1000ms();
void Delay200ms();
void delay_10us(void);

void main()
{
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		P_SW2 &= ~S4_S0;   					//S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
//  P_SW2 |= S4_S0;             //S4_S0=1 (P5.2/RxD4_2, P5.3/TxD4_2)		
		SCON = 0x50;														
		S3CON = 0x10;
		S4CON = 0x10;
	

		AUXR = 0x40;               //定时器1为1T模式
    TMOD = 0x20;                //定时器1为模式2(8位自动重载)
    TL1 = (256 - (FOSC/32/BAUD));   //设置波特率重装值
    TH1 = (256 - (FOSC/32/BAUD));
    TR1 = 1;                   //定时器1开始工作
    ES = 1;                     //使能串口中断
    
		T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
		AUXR = 0x54;                //T2为1T模式, 并启动定时器2
    IE2 = 0x18;                 //使能串口3,4中断
    EA = 1;
	 Transmit_init();
    
    while(1)
		{
			unsigned char test;
			//Uart3SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			//Uart1SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			P36=1;
			Delay1000ms();
			P36=0;
			//test = Read_Byte();
			Delay200ms();
		}
}

/*----------------------------
UART1 中断服务程序
-----------------------------*/
void Uart1() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                                        
    }
    if (TI)
    {
        TI = 0;              
        Uart1busy = 0;            
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
				Uart4SendData(S3BUF);
        //Uart1SendData(S3BUF);             //P30显示串口数据
			  //SendString("interrupt !\r\n");
			
    }
    if (S3CON & S3TI)
    {
        S3CON &= ~S3TI;         //清除S3TI位
        Uart3busy = 0;               //清忙标志
    }
}
/*----------------------------
UART4 中断服务程序
-----------------------------*/
void Uart4() interrupt 18 using 1
{
    if (S4CON & S4RI)
    {
        S4CON &= ~S4RI;         
        Uart1SendData(S4BUF);                
    }
    if (S4CON & S4TI)
    {
        S4CON &= ~S4TI;        
        Uart4busy = 0;             
    }
}


void Uart1SendData(BYTE dat)
{
    while (Uart1busy);              
    ACC = dat;                 
    if (P)                      
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;               
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;               
#endif
    }
    Uart1busy = 1;
    SBUF = ACC;                
}

void Uart1SendString(char *s)
{
    while (*s)                 
    {
         Uart1SendData(*s++);         
    }
}
/*----------------------------
发送串口数据
----------------------------*/

void Uart3SendData(BYTE dat)
{
    while (Uart3busy);          
    Uart3busy = 1;
    S3BUF = dat;               
}

/*----------------------------
发送字符串
----------------------------*/

void Uart3SendString(char *s)
{
    while (*s)               
    {
        Uart3SendData(*s++);        
    }
}

/*----------------------------
串口4发送数据
----------------------------*/
void Uart4SendData(BYTE dat)
{
    while (Uart4busy);                            
    Uart4busy = 1;
    S4BUF = dat;              
}

/*----------------------------
串口4发送字符串
----------------------------*/
void Uart4SendString(char *s)
{
    while (*s)                 
    {
        Uart4SendData(*s++);         
    }
}

/***************************************************************/
/*函数名称：Void Transmit_init(void)                           */
/*函数功能：发送初始化程序                                     */
/*入口参数：无                                                 */
/*出口参数：无                                                 */
/***************************************************************/
void Transmit_init(void)
 { //P2M0=P2M0&(~BIT0);/*设定P2.0为高阻输入*/
   //P2M1=P2M0|BIT0;
   //P2M0=P2M0&(~BIT1);/*设定P2.1为高阻输入*/
  // P2M1=P2M0|BIT1;
  // P2M0=P2M0&(~BIT2);/*设定P2.2为高阻输入*/
  // P2M1=P2M0|BIT2;
 //  P2M0=P2M0&(~BIT3);/*设定P2.3为高阻输入*/
  // P2M1=P2M0|BIT3;
  // P2M0=P2M0&(~BIT4);/*设定P2.4为输出*/
  // P2M1=P2M0&(~BIT4);
  // P2M0=P2M0&(~BIT5);/*设定P2.5为输出*/
  // P2M1=P2M0&(~BIT5);
  // P2M0=P2M0&(~BIT6);/*设定P2.6为高阻输入*/
  // P2M1=P2M0|BIT6;
 //  P2M0=P2M0&(~BIT7);/*设定P2.7为高阻输入*/
  // P2M1=P2M0|BIT7;*/
	 P2M0 = 0x0c;				//00001100
	 P2M1 = 0xf3;				//11110011			设定P2.0 P2.1 P2.2 P2.3 为高阻输入 P2.4 P2.5为输出  P2.6 P2.7为高阻输入
   P1M0 = 0x20;			  //00100000
	 P1M1 = 0x10;				//00010000			设定P1.2为输出,设定P1.3为高阻输入
	 P3M0 = 0x00;						//00000000
	 P3M1 = 0x03;						//00000011
	 
  }

/***************************************************************/
/*函数名称：void Sent_Byte(unsigned char Data0)                */
/*函数功能：发送一个字节的低4位                                */
/*入口参数：无                                                 */
/*出口参数：无                                                 */
/*发送数据流程① 一方单片机检查8位端口状态信号CHKREQ ，以判断对方是否已提 出发送数据的申请。
              ② 如果对方提出申请则等待，否则提出发送数据申请，并置REQ有效。 
			  ③ 再次检查8位端口状态信号CHKREQ判断对方是否提出发送数据的申请。
			  ④ 如果对方提出申请则发生冲突，清楚REQ并延时，然后执行第一步。
			  ⑤ 将数据送8位端口，再设定STB有效, 使对方进入中断以便取走数据。
			  ⑥ 检查CHK，等待对方单片机取走数据。
			  ⑦ 检查数据是否发 送完，如果没有则继续执行步骤5进行发送。
			  ⑧ 撤销REQ信号，释放8位端口。
			  ⑨ 数据发送 完毕退出流程。
数据接收流程：①进入中断服务流程。
              ②从8位端口读取数据。
			  ③设置ACK信号有 效，表示数据已成功读取。
			  ④退出中断服务流程。
/*           P2>----------------------P2                     */
/*           STB>---------------------INT                    */
/*           REQ>---------------------CHKREQ                 */
/*           INT<---------------------STB                    */
/*           CHKREQ<---------------------REQ                 */
/*           ACK--------------------->CHKACK               */
/*           CHKACK<---------------------ACK               */
/*************************************************************/
unsigned char  Sent_Byte(unsigned char Data0)/*发送一个字节的低4位*/
  {/*首先检测当前的位总线是否忙*/
  if((!Transmit_U2REQ)&&(Transmit_U2BusACK))/*检查U2是否提出发送数据申请，有申请则等待*/
   {/*U2等待*/
    Transmit_U1REQ=1;/*提出申请*/
	if(!Transmit_U2REQ)
	 {   if((Data0&BIT0)==BIT0) {Transmit_DATA0=Transmit_DATA0|BIT0;}
     	 else{Transmit_DATA0=Transmit_DATA0&(~BIT0);}
   		 if((Data0&BIT1)==BIT1) {Transmit_DATA0=Transmit_DATA0|BIT1;}
   		 else{Transmit_DATA0=Transmit_DATA0&(~BIT1);}
         if((Data0&BIT2)==BIT2) {Transmit_DATA0=Transmit_DATA0|BIT2;}
         else{Transmit_DATA0=Transmit_DATA0&(~BIT2);}
         if((Data0&BIT3)==BIT3) {Transmit_DATA0=Transmit_DATA0|BIT3;}
         else{Transmit_DATA0=Transmit_DATA0&(~BIT3);}
	     Transmit_U1STB=1;/*提出中断申请*/
		 while(!Transmit_U2BusACK);/*等待退出*/
		 Transmit_U1REQ=0;
		 Transmit_U1STB=0;
		 return(0);
	  }
	else 
	 {Transmit_U1REQ=0;return(1);}
    }
  else
   {return(1);}/*U2忙*/
   }
/***************************************************************/
/*函数名称：unsigned char Read_Byte(void)                      */
/*函数功能：接收一个字节的低4位                                */
/*入口参数：无                                                 */
/*出口参数：无                                                 */
/***************************************************************/
unsigned char Read_Byte(void)/*接收一个字节的低4位*/
  { unsigned char Read_Byte_data=0;
    if(Transmit_U2STB)/*U2STB=1,读取一个数据*/									  
     { if(Transmit_DATA0) {Read_Byte_data=Read_Byte_data|BIT0;}
	   if(Transmit_DATA1) {Read_Byte_data=Read_Byte_data|BIT1;}
	   if(Transmit_DATA2) {Read_Byte_data=Read_Byte_data|BIT2;}
	   if(Transmit_DATA3) {Read_Byte_data=Read_Byte_data|BIT3;}
	   Transmit_U2BusACK=1;/*发送数据读取完成相应信号*/
	   delay_10us();
	   Transmit_U2BusACK=0;/*发送数据读取完成相应信号*/
	   return(Read_Byte_data);
	  } 
	 else
	  {return(0);}  
   }
void delay_10us(void)
  {	unsigned char i=0,j=0;
    for(i=0;i<255;i++)
	  for(j=0;j<255;j++)
	   {_nop_();}
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


