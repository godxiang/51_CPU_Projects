#ifndef __W25Q64_H__
#define	__W25Q64_H__

#include "stc15w4k32s4.h"
#include <intrins.h>

 //W25Q64接口定义
 sbit 		WDI 			= P0^5;    //W25Q64数据输入
 sbit 		WDO 			= P0^7;    //W25Q64数据输出
 sbit 		WCK 			= P0^6;    //W25Q64时钟
 sbit 		WCS 			= P0^4;    //W25Q64选择
 
 //定义W25Q64的读写地址
 #define  FLASH_WriteAddress     0x000000			//写地址
 #define  FLASH_ReadAddress      FLASH_WriteAddress	//读地址
 #define  FLASH_SectorToErase    FLASH_WriteAddress	//擦除地址
 

 //定义端口操作
 #define	W25Q64_WCS_Clr()		{WCS=0;}
 #define	W25Q64_WCS_Set()		{WCS=1;}

 #define	W25Q64_WDI_Clr()		{WDI=0;} 
 #define	W25Q64_WDI_Set()		{WDI=1;}

 #define	W25Q64_WDO_Clr()		{WDO=0;} 
 #define	W25Q64_WDO_Set()		{WDO=1;}

 #define	W25Q64_WCK_Clr()		{WCK=0;} 
 #define	W25Q64_WCK_Set()		{WCK=1;}

//W25Q64串行FLASH的ID定义
#define 	sFLASH_W25Q64_ID		  0xEF16	//芯片ID
#define 	sFLASH_SPI_PAGESIZE       0x0100	//编程页大小256字节

//W25Q64串行FLASH的操作命令定义
#define 	sFLASH_CMD_WREN           0x06  	//写使能
#define 	sFLASH_CMD_WRDIS          0x04  	//写禁止
#define 	sFLASH_CMD_RDSR           0x05  	//读状态寄存器
#define 	sFLASH_CMD_WRSR           0x01  	//写状态寄存器
#define 	sFLASH_CMD_READ           0x03  	//读数据
#define 	sFLASH_CMD_HREAD          0x0B		//快速读数据
#define 	sFLASH_CMD_HREADDO        0x3B		//快速读，双路输出
#define 	sFLASH_CMD_PAGEPRO        0x02  	//页编程 
#define 	sFLASH_CMD_SE             0x20  	//扇区擦除
#define 	sFLASH_CMD_BE             0xD8  	//块擦除
#define 	sFLASH_CMD_CE             0xC7  	//芯片擦除
#define 	sFLASH_CMD_POWERD         0xB9  	//掉电模式
#define 	sFLASH_CMD_RDID           0x90  	//读生产厂家和器件ID号
#define 	sFLASH_CMD_JEDECID        0x9F  	//读JEDECID号 
#define 	sFLASH_DUMMY_BYTE         0x00
#define 	sFLASH_WIP_FLAG           0x01  	//FLASH忙标记 


//函数声明
unsigned int sFLASH_ReadID(void);
void sFLASH_EraseSector(unsigned long int SectorAddr);
void sFLASH_ReadBuffer(unsigned char * pBuffer, unsigned long int ReadAddr, unsigned int NumByteToRead);
void sFLASH_WritePage(unsigned char* pBuffer, unsigned long int WriteAddr, unsigned int NumByteToWrite);

#endif