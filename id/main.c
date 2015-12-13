#include "main.h"


void main(void)
	{
		init_Uart();
		IO_Init();
		Delay2000ms();												//等待身份证模块上电稳定
		P4M0 = 0;
		P4M1 = 0;
	
		while(1){
			
			if(Uart3HaveData){
			Delay200ms();												//等待串口接收数据完毕
			UART3_Proccess();										//串口数据处理
			}
		};
		//Sent_Byte	(0x0c);	
		
		
		//sFLASH_EraseSector(FLASH_SectorToErase);
		//sFLASH_ReadBuffer(TotalNumData,FLASH_ReadAddress,2);	 							   //上电稳定以后先读取Flash中储存数据的个数
		//Save_IDNum_ToFlash(TestIdData);
		//Op_IdCard();		
		//Flash_Op();
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
		S3CON = 0x10;
		S4CON = 0x10;             	//10000
    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;								//T2为1T模式, 并启动定时器2 10100
		AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
    ES = 1;                     //使能串口1中断
    IE2 = 0x18;                 //使能串口3,4中断
    EA = 1;
}
	

/*----------------------------
UART3 中断服务程序
-----------------------------*/
void Uart3_ISR() interrupt 17 using 1
{
	EA =0;
    if (S3CON & 0x01 )
    {
			if(Re_Cnt < 100)
				S3_Re_Buffer[Re_Cnt++] = S3BUF;
			Uart3HaveData = 1; 				//标志串口3有数据进来
        S3CON &= ~0x01;         //清除S3RI位	
    }
    if (S3CON & 0x02)
    {
        S3CON &= ~0x02;         //清除S3TI位
        Uart3busy = 0;               //清忙标志
    }
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
			if(Rece_Cnt < 1295)																	
			re_data_buffer[Rece_Cnt++] = dat;
		/*if(length == 4 && Rece_Cnt < 12){																		//返回错误消息	
				//Re_Data_Buffer.Re_FindCard_Data.Failure[Rece_Cnt++]=dat;					//继续接收4个字节数据																			//计数标记成功对比一位相应计数标签加1 tag[0]--寻卡 tag[1]--选卡 tag[2]--读卡
				testdata[Rece_Cnt++] = dat;
		}
			
		else if(length == 8 && Rece_Cnt < 14){
				Re_Data_Buffer.Re_FindCard_Data.Seccess[Rece_Cnt++]=dat;					//继续接收8个字节数据
				 if(Rece_Cnt == 13){																							//接收完毕开始对比数据		
					for(i = 6;i < 13; i++){
						if( ! Re_Data_Buffer.Re_FindCard_Data.Seccess[i] == Re_Data.Re_FindCard_Data.Seccess[i])
							OpCart_FindCart_State = 0;
					}
					OpCart_FindCart_State = 1;
				}
		}
		else if(length == 12 && Rece_Cnt < 19){
			Re_Data_Buffer.Re_SelectCard_Data.Seccess[Rece_Cnt++]=dat;				//继续接收12个字节数据
			if(Rece_Cnt == 18){																							//接收完毕开始对比数据		
					for(i = 6;i < 18; i++){
						if( ! Re_Data_Buffer.Re_SelectCard_Data.Seccess[i] == Re_Data.Re_SelectCard_Data.Seccess[i])
							OpCart_SelectCart_State = 0;
					}
					OpCart_SelectCart_State = 1;
				}
		}
		else if(length == 1288 && Rece_Cnt < 1295){
			if(Rece_Cnt < 10)
			Re_Data_Buffer.Re_ReadCard_Data.Seccess[Rece_Cnt++]=dat;				//继续接收3个字节数据(返回成功标志位)
			if(Rece_Cnt == 9){																							//接收完毕开始对比数据		
					for(i = 7;i < 10; i++){
						if( ! Re_Data_Buffer.Re_ReadCard_Data.Seccess[i] == Re_Data.Re_ReadCard_Data.Seccess[i])
							OpCart_ReadCart_State = 0;
					}
					OpCart_ReadCart_State = 1;
				}
					if(Rece_Cnt > 137  && Rece_Cnt < 167){																														//捡起数据，即接收身份证号码
						Rece_Cnt++;																																											//每隔1个字节取数据
						Id_Number[Rece_Cnt++] = dat;																																		//18位身份证号保存下来
					}
		}*/
        S4CON &= ~S4RI;        																				//串口接收中断标记需软件清零
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
	unsigned int i,j;
	length = Hex2Int (re_data_buffer[5])*256 + Hex2Int (re_data_buffer[6]);//十六进制转换为10进制
	if(length == 4 &&re_data_buffer[9] == 0x80 && re_data_buffer[10] == 0x84){
	OpCart_FindCart_State = 0;
	}
	if(length == 4 &&re_data_buffer[9] == 0x81 && re_data_buffer[10] == 0x85){
	OpCart_SelectCart_State = 0;
	}
	if(length == 4 &&re_data_buffer[9] == 0x41 && re_data_buffer[10] == 0x45){
	OpCart_ReadCart_State = 0;
	}
	if(length == 8)
		OpCart_FindCart_State = 1;
	if(length == 12)
		OpCart_SelectCart_State = 1;
	if(length == 1288){
		OpCart_ReadCart_State = 1;
		for(j=0,i = 136;j<18,i < 171;i++,j++){																		//取身份证号码																																			
			Id_Number[j] = re_data_buffer[i];
			i++;																																		//隔一位取一位
		}	
	}
	Rece_Cnt = 0;																																//接收计数器归零
}
/*----------------------------------------
串口3处理上位机信息（从WIFI模块）函数
----------------------------------------*/
void UART3_Proccess(void){
	bit Header_State;
	unsigned char i,j = 0;
	unsigned char CS = 0x00;
		if(S3_Re_Buffer[0] == 0x55 && S3_Re_Buffer[1] == 0xaa)
			Header_State = 1;
		else
			Header_State = 0;
		if(Header_State == 1){																								//头没有出错
		Re_length = Hex2Int (S3_Re_Buffer[2])*256 + Hex2Int (S3_Re_Buffer[3]);//获得data的长度，以便计算校验码CS
		for(i=4;i<10;i++){
				if(LocalHostNum[i] == S3_Re_Buffer[i])														//判断是否是本机编号
					j++;
			
		}
			if(j==6){																														//是本机编号
				for	(j = 10;j<Re_length + 10;j++){																//计算校验码CS
					CS+=S3_Re_Buffer[j];
				}
				if(CS == S3_Re_Buffer[10+Re_length])															//校验码CS是否出错 不出错则进行数据解析
				{
					Uart3SendString("the CS is\n");
					Uart3SendData(CS);
					for(j=0;j<3;j++)
						{
						if(MessageType[j][0] == S3_Re_Buffer[11] && MessageType[j][0] == S3_Re_Buffer[12])
						MessType = j;
					}
				}
				else
					Uart3SendString("CS Error!\n");
			}
			else
				Uart3SendString("not localhost number!\n");
		}
		Uart3HaveData = 0; 																											//数据处理完置串口3数据标记为0，等待下一次数据进入
		Re_Cnt = 0;
}

/***********************************************
*函数名称：Flash_Op
*功    能：flash操作函数
*入口参数：无
*返 回 值：无	
*备    注：无
************************************************/
void Flash_Op(void){
	unsigned int Cnt,Cnt0,ID;
	ID=sFLASH_ReadID();							//读取W25Q64的器件ID号																		
		if(ID!=sFLASH_W25Q64_ID)				//如果ID号不同
		{
			Uart1SendString("ID ERROR!");
			while(1);	
		}

	//Uart1SendString("Start Read Write Test!\n");
	//Uart1SendString("Erase!\n");
	//Delay_ms(100);
	//写之前需先擦除，擦除一个扇区
	//sFLASH_EraseSector(FLASH_SectorToErase);
	//Uart1SendString("Write!\n");
	//Delay_ms(300);
	//写入一页的数据
	//sFLASH_WritePage(Id_Number,FLASH_WriteAddress,18);
	//Uart1SendString("Read!\n");
	Delay_ms(300);
	//将该页的数据读
	sFLASH_ReadBuffer(spi_re_data_buffer,FLASH_Address,20);
	//比较写入和读取的数据是否一致
	for(Cnt0=0,Cnt=2;Cnt<20,Cnt0<18;Cnt++,Cnt0++) 
	{
		//Uart1SendData(spi_re_data_buffer[Cnt]);
		//如果有数据不同
		if(spi_re_data_buffer[Cnt]!=Id_Number[Cnt0])
		{			
			//ToDisplayError(ERR2);//显示错误信息
			//Uart1SendString("Read Error!");
			while(1);			
		}	
		
	}	
	if(Cnt == 0x0014){															//数据没有错
	P43 = 0;
	P44 = 0;
	Delay2000ms();
	P43 = 1;
	P44 = 1;
	}
		//Uart1SendString("Success!");
		//while(1);
		//Uart4_Process();
	
}


/***********************************************
*函数名称：Save_IDNum_ToFlash
*功    能：将上位机发来的数据有序地放进Flash储存器中
*入口参数1：Data 20位（2位序号  18位身份证号）
*返 回 值：无	
*备    注：无
************************************************/
void Save_IDNum_ToFlash(unsigned char * Data){
	

	TotalNumData[0] = (unsigned char)(TotalNum>>8);												//TotalNum的高8位
	TotalNumData[1] = (unsigned char)TotalNum;														//TotalNum的低8位
	sFLASH_WritePage(TotalNumData,FLASH_WriteAddress,2);									//保存一下数据的个数  以便下次写操作追加  下次写操作先读取这个数 然后定位写的地址
	TotalNum = Hex2Int (TotalNumData[0])*256 + Hex2Int (TotalNumData[1]);	//十六进制转换为10进制
	FLASH_Address = (unsigned long int)(TotalNum*20);
	FLASH_Address+=2;																											//指针右移2个字节
	sFLASH_WritePage(Data,FLASH_Address,20);
	TotalNum++;
	
}
/***********************************************
*函数名称：Op_IdCard
*功    能：身份证模块操作函数
*入口参数：无
*返 回 值：无	
*备    注：无
************************************************/
void Op_IdCard(void){
	unsigned int n,Cnt;
	while(1){																								//不断循环等待放卡
		while(!OpCart_FindCart_State){												//一直等待寻卡成功
			for (n=0;n<10;n++){
					Uart4SendData(FIND_CARD[n]);  
				}
						//Uart4_Process();
				Delay200ms();			//等待接收数据
				Uart4_Process();
			}
	if(OpCart_FindCart_State == 1){
		Cnt = 0;
		while(!OpCart_SelectCart_State && Cnt < 10){										  //一直等待选卡成功,最多重试10次 避免寻卡成功后移除了卡 而导致死循环
			for (n=0;n<10;n++){
				Uart4SendData(SELECT_CARD[n]);  
			}
					//Uart4_Process();	
			Delay200ms();				//等待接收数据
			Uart4_Process();
			Cnt++;
		}
	}
	if(OpCart_FindCart_State == 1 && OpCart_SelectCart_State == 1){
		Cnt = 0;
		while(!OpCart_ReadCart_State && Cnt < 3){												//一直等待读卡成功
			for (n=0;n<10;n++){
				Uart4SendData(READ_CARD[n]);  
			}
			Delay2000ms();																			//等待接收数据，这里模块返回数据延迟高需要等待的时间长一些。
			Uart4_Process();
			Cnt++;
		}
	if(Cnt == 1)
	Flash_Op();
	}
		OpCart_FindCart_State = 0;														//读取一张卡后重新初始化
		OpCart_SelectCart_State = 0;
		OpCart_ReadCart_State = 0;
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
/*----------------------------
串口3发送串口数据
----------------------------*/

void Uart3SendData( unsigned char dat )
{
    while (Uart3busy);          
    Uart3busy = 1;
    S3BUF = dat;               
}

/*----------------------------
串口3发送字符串
----------------------------*/

void Uart3SendString(char *s)
{
    while (*s)               
    {
        Uart3SendData(*s++);        
    }
}


/***********************************************
函数名称：IO_Init
功    能：单片机IO端口模式初始化
入口参数：无
返 回 值：无	
备    注：STC15W4K32S4系列A版单片机部分端口复位后
          不是准双向口,需要设置才能正常使用
************************************************/
void IO_Init(void)
{

	//将P04 P05 P06 P07设置为开漏口
	//因为单片机为5V端口，W25Q64为3.3V端口，为了实现电平匹配
	//将单片机设置为开漏结构，由外部上拉到3.3V。
	
	 P0M1 |= (1<<4) | (1<<5) | (1<<6) | (1<<7) ;  
 	 P0M0 |= (1<<4) | (1<<5) | (1<<6) | (1<<7) ;
	//分析
	//  1<<0等价于0x01 即 0000 0001
	//  1<<1等价于0x02 即 0000 0010
	//  1<<2等价于0x04 即 0000 0100
	//  1<<3等价于0x08 即 0000 1000
	//  以此类推1<<n 即第n位为1，其余位是0

	//  x |=(1<<n)  即对x执行按位取或 
	//  即x中的第n位置为1，不改变其他位状态  
	
	//  y &=~(1<<n)  即将1<<n按位取反，然后对y按位取与 
	//  即y中的第n位置为0，不改变其他位状态  
   P2M0 = 0x30;				//00110000
	 P2M1 = 0xc0;				//11000000			设定P2.0 P2.1 P2.2 P2.3 为高阻输入 P2.4 P2.5为输出  P2.6 P2.7为高阻输入
   P1M0 = 0x20;			  //00000100
	 P1M1 = 0x10;				//00001000			设定P1.2为输出,设定P1.3为高阻输入
	 
	 
	//P10 P14复位后为强推挽输出、强上拉，
	//防止不当操作导致端口损坏，初始化为准双向口 
	//将P10 P14设置为准双向口
	//P1M1 &=~( (1<<0) | (1<<4) );  
	//P1M0 &=~( (1<<0) | (1<<4) );
	
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
	 {   
		 if((Data0&BIT0)==BIT0) {
	 Transmit_DATA0=Transmit_DATA0|BIT0;
	 }
     	 else{Transmit_DATA0=Transmit_DATA0&(~BIT0);
			 }
   		 if((Data0&BIT1)==BIT1) {
			 Transmit_DATA1=Transmit_DATA1|BIT1;
			 }
   		 else{Transmit_DATA1=Transmit_DATA1&(~(BIT1>>1));
			 }
         if((Data0&BIT2)==BIT2) {
				 Transmit_DATA2=Transmit_DATA2|BIT2;}
         else{Transmit_DATA2=Transmit_DATA2&(~(BIT2>>2));
				 }
         if((Data0&BIT3)==BIT3) {
				 Transmit_DATA3=Transmit_DATA3|BIT3;}
         else{Transmit_DATA3=Transmit_DATA3&(~(BIT3>>3));
				 }
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
