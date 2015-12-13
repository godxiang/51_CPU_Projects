 //文件包含
#include "stc15w4k32s4.h"
#include "w25q64.h"

#define FOSC 22118400L          //时钟频率
#define BAUD 115200             //波特率
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4_S0 0x04              //P_SW2.2
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

void init_Uart();
void IO_Init(void);
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);
void Uart4_Process(void);
void Delay_ms(unsigned int ms);
void Delay1000ms();
void Delay200ms();

struct Re_FindCard{
unsigned char Seccess_tag[8];
unsigned char Failure_tag[4];
};

struct Re_SelectCard{
unsigned char Seccess_tag[12];
unsigned char Failure_tag[4];
};

struct Re_ReadCard{
unsigned char Seccess_tag[3];
unsigned char Failure_tag[4];
};

struct re_data{
unsigned char header[5];
struct Re_FindCard Re_FindCard_Data;
struct Re_SelectCard Re_SelectCard_Data;
struct Re_ReadCard Re_ReadCard_Data;
}Re_Data={
{0xaa,0xaa,0xaa,0x96,0x69},
{{0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x97},{0x00, 0x00, 0x80, 0x84}},
{{0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C},{0x00, 0x00, 0x81, 0x85}},
{{0x00, 0x00, 0x90},{0x00, 0x00, 0x41, 0x45}}
};

unsigned char xdata re_data_buffer[1295];
unsigned  int i=0,Re_num=0;
unsigned char const code FIND_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x01, 0x22};
unsigned char const code SELECT_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x02, 0x21};
unsigned char const code READ_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x30, 0x01, 0x32};
//unsigned char const code TestData[10] = {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
//unsigned char OpCart_FindCart(void);
//unsigned char OpCart_SelectCart(void);
//unsigned char OpCart_ReadCart(void);
bit Uart4busy;
bit Uart1busy;


void main(void)
	{
		unsigned int n,Cnt,ID;
		init_Uart();
		IO_Init();
		for (n=0;n<10;n++){
			Uart4SendData(FIND_CARD[n]);  
		}
				//Uart4_Process();
		Delay200ms();			//等待接收数据
		Re_num=0;
		for (n=0;n<10;n++){
			Uart4SendData(SELECT_CARD[n]);  
		}
				//Uart4_Process();	
		Delay200ms();			//等待接收数据
		Re_num=0;
		for (n=0;n<10;n++){
			Uart4SendData(READ_CARD[n]);  
		}	
		Delay1000ms();		//等待上电稳定(接收数据)
		Delay1000ms();
		ID=sFLASH_ReadID();							//读取W25Q64的器件ID号																		
		if(ID!=sFLASH_W25Q64_ID)				//如果ID号不同
		{
			Uart1SendString("ID ERROR!");
			while(1);	
		}

	Uart1SendString("Start Read Write Test!\n");
	Uart1SendString("Erase!\n");
	Delay_ms(100);
	//写之前需先擦除，擦除一个扇区
	sFLASH_EraseSector(FLASH_SectorToErase);
	Uart1SendString("Write!\n");
	Delay_ms(300);
	//写入一页的数据
	sFLASH_WritePage(re_data_buffer,FLASH_WriteAddress,Re_num);
	Uart1SendString("Read!\n");
	Delay_ms(300);
	//将该页的数据读
	sFLASH_ReadBuffer(re_data_buffer,FLASH_ReadAddress,Re_num);
	//比较写入和读取的数据是否一致
	for(Cnt=0;Cnt<Re_num;Cnt++) 
	{
		Uart1SendData(re_data_buffer[Cnt]);
		//如果有数据不同
		/*if(TestData[Cnt]!=re_data_buffer[Cnt])
		{			
			//ToDisplayError(ERR2);//显示错误信息
			Uart1SendString("Read Error!");
			while(1);			
		}	*/
	}	
		Uart1SendString("Success!");
		while(1);
			//Uart4_Process();		
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
	unsigned char dat;
	EA=0;
	dat = S4BUF;
	    if (S4CON & S4RI)
    {
			if(Re_num<1295){			
			re_data_buffer[Re_num++]=dat;
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
	unsigned char j;
			//for(;j<5;j++)
			//while (re_data_buffer[j] != Re_Data.header[j])	//判断接收数据头，5个字节
				//Uart1SendString("Received Data Error!");
			//Uart1SendData(0x01);
			//j=0;																						//数据指针归零
				if (re_data_buffer[5]==0x00){
					
						if (re_data_buffer[6] == 0x04){
									unsigned char a=7,b=0;
									unsigned char temp[3]={0,0,0};
										for(;a<11,b<4;a++,b++){
										if(re_data_buffer[a] == Re_Data.Re_FindCard_Data.Failure_tag[b])
											temp[0]++;
										if(re_data_buffer[a] == Re_Data.Re_SelectCard_Data.Failure_tag[b])
											temp[1]++;
										if(re_data_buffer[a] == Re_Data.Re_ReadCard_Data.Failure_tag[b])
											temp[2]++;
							}
							if(temp[0] == 4){
								//Uart1SendString("FindCard!\n");
							for(j=0;j<11;j++);
							//Uart1SendData(re_data_buffer[j]);			
							}
							if(temp[1] == 4){
								//Uart1SendString("SelectCard!\n");
							for(j=0;j<11;j++);
							//Uart1SendData(re_data_buffer[j]);
							}
							if(temp[2] == 4)
								//Uart1SendString("ReadCard!\n");
							for(j=0;j<11;j++);
							//Uart1SendData(re_data_buffer[j]);
							}
						else if (re_data_buffer[6]==0x08)
						for(j=0;j<15;j++);
							//Uart1SendData(re_data_buffer[j]);
						else if (re_data_buffer[6]==0x0c)
							for(j=0;j<19;j++);
							//Uart1SendData(re_data_buffer[j]);
						}
				else if(re_data_buffer[5]==0x05){
						//Uart1SendString("ID INFO START!\n");
					for(j=0;j<300;j++)
					Uart1SendData(re_data_buffer[j]);
				}
				i=0;
}
/*----------------------------------
16进制转换函数

-----------------------------------*/
unsigned char htd(unsigned char a){
unsigned char b,c;
	b=a%10;
	c=b;
	a=a/10;
	b=a%10;
	c=c|b>>4;
	return c;
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
/*****************************************
身份证操作寻卡												
入口参数:无												  
出口参数:无												   
*****************************************/
/*
unsigned char OpCart_FindCard(void)
	{
			unsigned char const FIND_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x01, 0x22};
			unsigned char Response[15];
			unsigned char Res_i=0;
			unsigned char OpCart_FindCard_i=0;
			while(OpCart_FindCard_i<10){
			Uart4SendData(FIND_CARD[OpCart_FindCard_i]);
			OpCart_FindCard_i++;
		}
}*/
/*****************************************
身份证操作选卡												 
程序功能：身份证选卡操作							
入口参数:无												   
出口参数:无												    
*******************************************/
/*
unsigned char OpCart_SelectCard(void){
	    unsigned char SELECT_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x02, 0x21};
			unsigned char SELECT_CARD_RESPONSE[15]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x0c, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x00, 0x00, 0x97};
			unsigned char Response[15];
			unsigned char Res_i=0;
			unsigned char OpCart_SelectCard_i=0;
			while(OpCart_SelectCard_i<10){
			Uart4SendData(SELECT_CARD[OpCart_SelectCard_i++]);//发送数据
		}
			while(!(S4CON&0x01));
			Response[Res_i++]=S4BUF;													//接收模块的数据并保存
		  if(SELECT_CARD_RESPONSE[Res_i]==Response[Res_i])
			S4CON&=~0x01;
}*/
/*****************************************
身份证操作读卡													 
程序功能：身份证读卡操作							
入口参数:无												   
出口参数:无												  
*****************************************/
/*unsigned char OpCart_ReadCard(void){
	
			unsigned char READ_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x30, 0x01, 0x32};
			unsigned char OpCart_ReadCard_i=0;
			while(OpCart_ReadCard_i<10){
			Uart4SendData(READ_CARD[OpCart_ReadCard_i]);
			OpCart_ReadCard_i++;
		}
}*/
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
	
	//P10 P14复位后为强推挽输出、强上拉，
	//防止不当操作导致端口损坏，初始化为准双向口 
	//将P10 P14设置为准双向口
	//P1M1 &=~( (1<<0) | (1<<4) );  
	//P1M0 &=~( (1<<0) | (1<<4) );     
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
