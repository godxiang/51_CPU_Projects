#include "STC15W4KXX.h"
#include <intrins.H>
#define S2_S0 0x01              //P_SW2.0
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define FOSC 11059200L          //系统频率
#define BAUD 9600             //串口波特率
#define BAUD1 9600             //串口波特率
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80  
#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3 
#define FE_START 0x68/*帧头*/
#define	FE_Adress_length 1 /*帧地址字节的长度*/
#define	FE_DATALENGTH FE_Adress_length+4 /*这是帧中数据长度所在帧中的位置*/
#define FE_OVER 0x16/*帧尾*/
#define UART_FERE_DATALENGTH FE_DATALENGTH-2/*接收到数据长度字节时对应的UART_RE_DATA_Counter值*/  
#define FE_TXDATA_FIXLENGTH  6+FE_Adress_length //待发送数据除了数据域字节外的固定长度，数据字节不算，不算帧头
#define FE_CTL 2/*控制码的位置*/          
sbit Transmit_DATA0 =P2^0;
sbit Transmit_DATA1 =P2^1;
sbit Transmit_DATA2 =P2^2;
sbit Transmit_DATA3 =P2^3;
sbit Transmit_U1STB =P2^4;/*当发送数据时，需要先使此位置高，默认为输出，低电平*/
sbit Transmit_U1REQ =P2^5;/*当接收到来自其他机器的高电平信号时，默认为输出*/
sbit Transmit_U2STB =P2^6;/*当其他机器向本机发送数据时，默认为输入*/
sbit Transmit_U2REQ =P2^7;/*当其他机器向本机发送数据时，默认为输入*/
sbit Transmit_U1BusACK =P1^2;/*默认为输出，忙信号*/
sbit Transmit_U2BusACK =P1^3;/*默认输入，忙信号检测*/
unsigned int i=0;
unsigned char xdata UART2_Fe_flag=0;
unsigned char xdata UART2_Fe_DATA_Buf=0;
unsigned int xdata UART2_RE_DATA_Counter=0;
unsigned int UART2_Fe_DATA_CounterBuf=0;
unsigned char xdata UART2_Fe_DATA_CS=0;
unsigned char xdata UART2_RE_DATA[32];
unsigned char xdata UART2_Fe_RE_DATA=0;
unsigned char UART2_int_buf1=0;
void delay_10us(void);
void Uart2_intrupt_function(void);
/***************************************************************/
/*函数名称：Void Transmit_init(void)                           */
/*函数功能：发送初始化程序                                     */
/*入口参数：无                                                 */
/*出口参数：无                                                 */
/***************************************************************/
void Transmit_init(void)
 { P2M0=P2M0&(~BIT0);/*设定P2.0为高阻输入*/
   P2M1=P2M0|BIT0;
   P2M0=P2M0&(~BIT1);/*设定P2.1为高阻输入*/
   P2M1=P2M0|BIT1;
   P2M0=P2M0&(~BIT2);/*设定P2.2为高阻输入*/
   P2M1=P2M0|BIT2;
   P2M0=P2M0&(~BIT3);/*设定P2.3为高阻输入*/
   P2M1=P2M0|BIT3;
   P2M0=P2M0&(~BIT4);/*设定P2.4为输出*/
   P2M1=P2M0&(~BIT4);
   P2M0=P2M0&(~BIT5);/*设定P2.5为输出*/
   P2M1=P2M0&(~BIT5);
   P2M0=P2M0&(~BIT6);/*设定P2.6为高阻输入*/
   P2M1=P2M0|BIT6;
   P2M0=P2M0&(~BIT7);/*设定P2.7为高阻输入*/
   P2M1=P2M0|BIT7;
   P1M0=P2M0&(~BIT2);/*设定P1.2为输出*/
   P1M1=P2M0&(~BIT2);
   P1M0=P2M0&(~BIT3);/*设定P1.3为高阻输入*/
   P1M1=P2M0|BIT3;
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
void main(void)
{
    Transmit_init();
    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)     
    SCON = 0x50;                //8位可变波特率
    AUXR = 0x40;                //定时器1为1T模式
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //定时器1开始启动
    //ES = 1;                     //使能串口中断
    //EA = 1;
    //P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
    P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
    S2CON = 0x50;               //8位可变波特率
	T2L = (65536 - (FOSC/4/BAUD1));		//设定定时初值
	T2H = (65536 - (FOSC/4/BAUD))>>8;		//设定定时初值
    AUXR = 0x54;                //T2为1T模式, 并启动定时器2
    IE2 = 0x01;                 //使能串口2中断
    EA = 1;
    while(1)
	  {  i=i+1;  
	    if(i>65505){SBUF=0x5a;while(!TI);TI=0;i=0;}
		/*39 37 38 37 31 32 31 30 30 31 36 39 37 
	    if((S2CON&0x01)==0x01)
	     { SBUF=0x68;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x0b;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x00;while(!TI);TI=0;
		   SBUF=0x76;while(!TI);TI=0;
		   SBUF=0x16;while(!TI);TI=0;
		  }  */   
		//Sent_Byte(0x55);
		//if(i>65505){S2BUF=0x43;while(!(S2CON&0X02));S2CON &=~0X02;}
	   }
}
  /*----------------------------
UART2 中断服务程序
-----------------------------*/
void Uart2() interrupt 8 using 1
{Uart2_intrupt_function();
}
/*****************************************************************************/
/*串口2数据帧接收程序*/
/*程序组织成通用程序，只用传递相关的参数，自己合成返回帧，定义结构体变量。   */
/*****************************************************************************/
void Uart2_intrupt_function(void)
{if(S2CON&0X01)  /*接收*/
   {S2CON &=~0X01;								   
 /*接收到数据后清零超时计数器*/
    UART2_Fe_DATA_Buf=S2BUF;
	SBUF=0x68;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x0b;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x00;while(!TI);TI=0;
		   SBUF=0x76;while(!TI);TI=0;
		   SBUF=0x16;while(!TI);TI=0;
    UART2_Fe_RE_DATA=UART2_Fe_DATA_Buf;
	if((UART2_Fe_flag &BIT3)==0)/*本帧未处理完，则不接收帧数据*/
   {
   if((UART2_Fe_flag &BIT1)==0)/*是否接收到帧头*/
    {/*未接收到帧头*/
      if(UART2_Fe_DATA_Buf==FE_START)
      { UART2_Fe_flag |=BIT1;/*置位帧头接收到标志位*/
        UART2_Fe_DATA_CS=UART2_Fe_DATA_Buf;/*校验求和*/
		/*接收到帧头后开始计时，当达到五百毫秒后，还是没有完整接收到数据，则停止接收，清零所有标志位*/
		//Flag_UART_TimeOut250msStart=1;/* 启动超时计时*/
		//ET0=0;
	    //TR0 = 0;		//定时器0开始计时
      }
    }
    else/*已接收到帧头，将接收到的数据存储在接收数据存储数组中，在部分应该判断是否接收到帧尾*/
    {/*根据接收数据数据指针将数据存储在接收数组中，并在接收过程中分析数据校验和是否正确不正确*/
     UART2_RE_DATA[UART2_RE_DATA_Counter]=UART2_Fe_DATA_Buf;/*暂存接收数据*/
     if(UART2_RE_DATA_Counter>UART_FERE_DATALENGTH)/*判断是否接收到数据数据字节*/
     {UART2_Fe_DATA_CounterBuf=FE_DATALENGTH+UART2_RE_DATA[FE_DATALENGTH-1];/*指向校验码的位置*/
      if(UART2_RE_DATA_Counter==UART2_Fe_DATA_CounterBuf)
	  { 
		//RS485_CTL=1; SBUF=UART_Fe_DATA_CS;while(!TI0);TI0=0;RS485_CTL=0;
		//RS485_CTL=1; SBUF=UART_RE_DATA[UART_RE_DATA_Counter];while(!TI0);TI0=0;RS485_CTL=0;
		//RS485_CTL=1; SBUF=UART_RE_DATA_Counter;while(!TI0);TI0=0;RS485_CTL=0;
		//RS485_CTL=1; SBUF=UART_Fe_DATA_CounterBuf;while(!TI0);TI0=0;RS485_CTL=0;
		UART2_int_buf1=UART2_RE_DATA[UART2_RE_DATA_Counter];
	   if((UART2_Fe_DATA_CS-UART2_int_buf1)==0)/*判断是否接收到校验码*/
      { UART2_Fe_flag &=~BIT2;UART2_Fe_flag |=BIT4;}/*清零帧接收校验和错误标志位*/        
       else
       { UART2_Fe_flag |=BIT2;UART2_Fe_flag |=BIT4;}/*置位帧接收校验和错误标志位*/	
	  }
	  UART2_int_buf1=UART2_RE_DATA[UART2_RE_DATA_Counter];
      if(((UART2_RE_DATA_Counter-(UART2_Fe_DATA_CounterBuf+1))==0)&&((UART2_int_buf1-FE_OVER)==0))/*判断是否接收到帧尾，是数据接收完成*/
       {  UART2_Fe_flag |=BIT3;}/*是，则本帧接收完成*/ 
     }
     /*接收后求校验和，当接收到校验码时则停止求和*/
     if((UART2_Fe_flag&BIT4)==0)
     {UART2_Fe_DATA_CS=UART2_Fe_DATA_CS+UART2_Fe_RE_DATA;}
     UART2_RE_DATA_Counter=UART2_RE_DATA_Counter+1;
	 if(UART2_RE_DATA_Counter>(UART2_RE_DATA_Counter+16)) 
	   {UART2_RE_DATA_Counter=UART2_RE_DATA_Counter+16;}
    }
  }
 }
}
