#ifndef __W25Q64_H__
#define	__W25Q64_H__

#include "stc15w4k32s4.h"
#include <intrins.h>

 //W25Q64�ӿڶ���
 sbit 		WDI 			= P0^5;    //W25Q64��������
 sbit 		WDO 			= P0^7;    //W25Q64�������
 sbit 		WCK 			= P0^6;    //W25Q64ʱ��
 sbit 		WCS 			= P0^4;    //W25Q64ѡ��
 
 //����W25Q64�Ķ�д��ַ
 #define  FLASH_WriteAddress     0x000000			//д��ַ
 #define  FLASH_ReadAddress      FLASH_WriteAddress	//����ַ
 #define  FLASH_SectorToErase    FLASH_WriteAddress	//������ַ
 

 //����˿ڲ���
 #define	W25Q64_WCS_Clr()		{WCS=0;}
 #define	W25Q64_WCS_Set()		{WCS=1;}

 #define	W25Q64_WDI_Clr()		{WDI=0;} 
 #define	W25Q64_WDI_Set()		{WDI=1;}

 #define	W25Q64_WDO_Clr()		{WDO=0;} 
 #define	W25Q64_WDO_Set()		{WDO=1;}

 #define	W25Q64_WCK_Clr()		{WCK=0;} 
 #define	W25Q64_WCK_Set()		{WCK=1;}

//W25Q64����FLASH��ID����
#define 	sFLASH_W25Q64_ID		  0xEF16	//оƬID
#define 	sFLASH_SPI_PAGESIZE       0x0100	//���ҳ��С256�ֽ�

//W25Q64����FLASH�Ĳ��������
#define 	sFLASH_CMD_WREN           0x06  	//дʹ��
#define 	sFLASH_CMD_WRDIS          0x04  	//д��ֹ
#define 	sFLASH_CMD_RDSR           0x05  	//��״̬�Ĵ���
#define 	sFLASH_CMD_WRSR           0x01  	//д״̬�Ĵ���
#define 	sFLASH_CMD_READ           0x03  	//������
#define 	sFLASH_CMD_HREAD          0x0B		//���ٶ�����
#define 	sFLASH_CMD_HREADDO        0x3B		//���ٶ���˫·���
#define 	sFLASH_CMD_PAGEPRO        0x02  	//ҳ��� 
#define 	sFLASH_CMD_SE             0x20  	//��������
#define 	sFLASH_CMD_BE             0xD8  	//�����
#define 	sFLASH_CMD_CE             0xC7  	//оƬ����
#define 	sFLASH_CMD_POWERD         0xB9  	//����ģʽ
#define 	sFLASH_CMD_RDID           0x90  	//���������Һ�����ID��
#define 	sFLASH_CMD_JEDECID        0x9F  	//��JEDECID�� 
#define 	sFLASH_DUMMY_BYTE         0x00
#define 	sFLASH_WIP_FLAG           0x01  	//FLASHæ��� 


//��������
unsigned int sFLASH_ReadID(void);
void sFLASH_EraseSector(unsigned long int SectorAddr);
void sFLASH_ReadBuffer(unsigned char * pBuffer, unsigned long int ReadAddr, unsigned int NumByteToRead);
void sFLASH_WritePage(unsigned char* pBuffer, unsigned long int WriteAddr, unsigned int NumByteToWrite);

#endif