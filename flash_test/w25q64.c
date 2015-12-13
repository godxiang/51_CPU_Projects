#include "W25Q64.h"
#include <intrins.h>

/***********************************************
函数名称：Delay_us
功    能：STC 1T单片机1us延时程序
入口参数：us:延时的微秒数
返 回 值：无	
备    注：外部时钟11.0592MHz
************************************************/
void Delay1us()		//@22.1184MHz
{
	unsigned char i;

	i = 3;
	while (--i);
}

/***********************************************
函数名称：sFLASH_SendByte
功    能：向W25Q64 FLASH发送一个字节数据。
入口参数：dat:发送的数据
返 回 值：无	
备    注：无
************************************************/
void sFLASH_SendByte(unsigned char dat)
{
	unsigned char i;	       //定义计数器
	//送出数据
	for(i=0;i<8;i++)		 	
    {				
		W25Q64_WCK_Clr();
		//先发送高字节
		if( (dat&0x80)==0x80 )
	 	{
	   		W25Q64_WDI_Set();	
	 	}
	 	else
	 	{
			W25Q64_WDI_Clr();
	 	}
		 Delay1us();	
		W25Q64_WCK_Set();
		//数据左移
	    dat<<=1;								
	}  
}
/***********************************************
函数名称：sFLASH_ReadByte
功    能：从W25Q64 FLASH读取一个字节数据。
入口参数：无
返 回 值：unsigned char：读出的数据。	
备    注：无
************************************************/
unsigned char sFLASH_ReadByte(void)
{
	unsigned char i;
    unsigned char dat = 0;
	W25Q64_WDO_Set();
	W25Q64_WCK_Clr();
    for (i=0; i<8; i++)             //8位计数器
    {
		W25Q64_WCK_Set();           //时钟线拉高
		 Delay1us();
        dat <<= 1;	                //数据右移一位
		if (WDO) 
		{
			dat |= 0x01;            //读取数据
		}
		W25Q64_WCK_Clr();           //时钟线拉低
	}
    return dat;
}
/***********************************************
函数名称：sFLASH_WriteEnable
功    能：写使能。
入口参数：无
返 回 值：无	
备    注：无
************************************************/
void sFLASH_WriteEnable(void)
{
  	//使能芯片 
  	W25Q64_WCS_Clr();

  	//发送写使能指令 
  	sFLASH_SendByte(sFLASH_CMD_WREN);

  	//禁止芯片 
  	W25Q64_WCS_Set();
}
/***********************************************
函数名称：sFLASH_WaitForWriteEnd
功    能：等待写结束
入口参数：无
返 回 值：无	
备    注：无
************************************************/
void sFLASH_WaitForWriteEnd(void)
{
  	unsigned char flashstatus = 0;

  	//芯片使能 
  	W25Q64_WCS_Clr();

  	//发送读取寄存器指令
  	sFLASH_SendByte(sFLASH_CMD_RDSR);

  	//循环读取忙标记直到忙结束 
  	do
  	{
    	flashstatus = sFLASH_ReadByte();
  	}
	//判断忙标记
  	while ((flashstatus & sFLASH_WIP_FLAG) == 0x01); 

  	//芯片禁止 
  	W25Q64_WCS_Set();
}
/***********************************************
函数名称：sFLASH_ReadID
功    能：从W25Q64 FLASH读取器件ID号。
入口参数：无
返 回 值：unsigned int：读出的ID。	
备    注：无
************************************************/
unsigned int sFLASH_ReadID(void)
{
  	unsigned int Temp = 0; 
	unsigned char Temp0 = 0, Temp1 = 0;
  	//使能flash
  	W25Q64_WCS_Clr();
  	//发送读flash  id号指令
  	sFLASH_SendByte(sFLASH_CMD_RDID);
	//发送24位地址 地址为0
	sFLASH_SendByte(0);
	sFLASH_SendByte(0);
	sFLASH_SendByte(0);
  	//读取返回的数据
  	Temp0 = sFLASH_ReadByte();
  	Temp1 = sFLASH_ReadByte();
  	//禁止flash
  	W25Q64_WCS_Set();
  	//组合数据
  	Temp = (Temp0 << 8) | Temp1;
  	return Temp;
}
/***********************************************
函数名称：sFLASH_WritePage
功    能：向W25Q64 FLASH写入一页数据。
入口参数：pBuffer:存放写入的数据的缓冲区，
          WriteAddr：开始写入的地址，
		  NumByteToWrite：写入的字节数
返 回 值：无：读出的数据。	
备    注：无
************************************************/
void sFLASH_WritePage(unsigned char* pBuffer, unsigned long int WriteAddr, unsigned int NumByteToWrite)
{
  	//芯片写使能，写之前需写使能，否则无法写入 
  	sFLASH_WriteEnable();

  	//芯片使能 
  	W25Q64_WCS_Clr();
  	//发送写指令 
  	sFLASH_SendByte(sFLASH_CMD_PAGEPRO);
  	//发送24位地址 
  	sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  	sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  	sFLASH_SendByte(WriteAddr & 0xFF);

  	//循环按字节写入 
  	while (NumByteToWrite--)
  	{
    	sFLASH_SendByte(*pBuffer);
    	pBuffer++;
  	}

  	//芯片禁止 
  	W25Q64_WCS_Set();

  	//等待写完成
  	sFLASH_WaitForWriteEnd();
}
/***********************************************
函数名称：sFLASH_ReadBuffer
功    能：从W25Q64 FLASH读取一串数据。
入口参数：pBuffer:接收该字符串的缓冲区
          ReadAddr：开始读的地址
		  NumByteToRead：读的字节数
返 回 值：无	
备    注：无
************************************************/
void sFLASH_ReadBuffer(unsigned char * pBuffer, unsigned long int ReadAddr, unsigned int NumByteToRead)
{
  	//芯片使能 
  	W25Q64_WCS_Clr();

  	//发送读命令
  	sFLASH_SendByte(sFLASH_CMD_READ);

  	//发送24位地址 
  	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  	sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  	sFLASH_SendByte(ReadAddr & 0xFF);

	//循环按字节读取
  	while (NumByteToRead--) 
  	{
    	*pBuffer = sFLASH_ReadByte();
    	pBuffer++;
  	}

  	//芯片禁止 
  	W25Q64_WCS_Set();
}
/***********************************************
函数名称：sFLASH_EraseSector
功    能：擦除W25Q64 FLASH的一个扇区。
入口参数：无
返 回 值：unsigned char：读出的数据。	
备    注：无
************************************************/
void sFLASH_EraseSector(unsigned long int SectorAddr)
{
  	//芯片写使能
  	sFLASH_WriteEnable();

  	//芯片使能 
  	W25Q64_WCS_Clr();
  	//发送扇区擦除指令
  	sFLASH_SendByte(sFLASH_CMD_SE);
  	//发送24位地址
  	sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  	sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  	sFLASH_SendByte(SectorAddr & 0xFF);
  	//芯片禁止 
  	W25Q64_WCS_Set();

  	//等待写完成
  	sFLASH_WaitForWriteEnd();
}