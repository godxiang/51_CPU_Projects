#include "W25Q64.h"
#include <intrins.h>

/***********************************************
�������ƣ�Delay_us
��    �ܣ�STC 1T��Ƭ��1us��ʱ����
��ڲ�����us:��ʱ��΢����
�� �� ֵ����	
��    ע���ⲿʱ��11.0592MHz
************************************************/
void Delay1us()		//@22.1184MHz
{
	unsigned char i;

	i = 3;
	while (--i);
}

/***********************************************
�������ƣ�sFLASH_SendByte
��    �ܣ���W25Q64 FLASH����һ���ֽ����ݡ�
��ڲ�����dat:���͵�����
�� �� ֵ����	
��    ע����
************************************************/
void sFLASH_SendByte(unsigned char dat)
{
	unsigned char i;	       //���������
	//�ͳ�����
	for(i=0;i<8;i++)		 	
    {				
		W25Q64_WCK_Clr();
		//�ȷ��͸��ֽ�
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
		//��������
	    dat<<=1;								
	}  
}
/***********************************************
�������ƣ�sFLASH_ReadByte
��    �ܣ���W25Q64 FLASH��ȡһ���ֽ����ݡ�
��ڲ�������
�� �� ֵ��unsigned char�����������ݡ�	
��    ע����
************************************************/
unsigned char sFLASH_ReadByte(void)
{
	unsigned char i;
    unsigned char dat = 0;
	W25Q64_WDO_Set();
	W25Q64_WCK_Clr();
    for (i=0; i<8; i++)             //8λ������
    {
		W25Q64_WCK_Set();           //ʱ��������
		 Delay1us();
        dat <<= 1;	                //��������һλ
		if (WDO) 
		{
			dat |= 0x01;            //��ȡ����
		}
		W25Q64_WCK_Clr();           //ʱ��������
	}
    return dat;
}
/***********************************************
�������ƣ�sFLASH_WriteEnable
��    �ܣ�дʹ�ܡ�
��ڲ�������
�� �� ֵ����	
��    ע����
************************************************/
void sFLASH_WriteEnable(void)
{
  	//ʹ��оƬ 
  	W25Q64_WCS_Clr();

  	//����дʹ��ָ�� 
  	sFLASH_SendByte(sFLASH_CMD_WREN);

  	//��ֹоƬ 
  	W25Q64_WCS_Set();
}
/***********************************************
�������ƣ�sFLASH_WaitForWriteEnd
��    �ܣ��ȴ�д����
��ڲ�������
�� �� ֵ����	
��    ע����
************************************************/
void sFLASH_WaitForWriteEnd(void)
{
  	unsigned char flashstatus = 0;

  	//оƬʹ�� 
  	W25Q64_WCS_Clr();

  	//���Ͷ�ȡ�Ĵ���ָ��
  	sFLASH_SendByte(sFLASH_CMD_RDSR);

  	//ѭ����ȡæ���ֱ��æ���� 
  	do
  	{
    	flashstatus = sFLASH_ReadByte();
  	}
	//�ж�æ���
  	while ((flashstatus & sFLASH_WIP_FLAG) == 0x01); 

  	//оƬ��ֹ 
  	W25Q64_WCS_Set();
}
/***********************************************
�������ƣ�sFLASH_ReadID
��    �ܣ���W25Q64 FLASH��ȡ����ID�š�
��ڲ�������
�� �� ֵ��unsigned int��������ID��	
��    ע����
************************************************/
unsigned int sFLASH_ReadID(void)
{
  	unsigned int Temp = 0; 
	unsigned char Temp0 = 0, Temp1 = 0;
  	//ʹ��flash
  	W25Q64_WCS_Clr();
  	//���Ͷ�flash  id��ָ��
  	sFLASH_SendByte(sFLASH_CMD_RDID);
	//����24λ��ַ ��ַΪ0
	sFLASH_SendByte(0);
	sFLASH_SendByte(0);
	sFLASH_SendByte(0);
  	//��ȡ���ص�����
  	Temp0 = sFLASH_ReadByte();
  	Temp1 = sFLASH_ReadByte();
  	//��ֹflash
  	W25Q64_WCS_Set();
  	//�������
  	Temp = (Temp0 << 8) | Temp1;
  	return Temp;
}
/***********************************************
�������ƣ�sFLASH_WritePage
��    �ܣ���W25Q64 FLASHд��һҳ���ݡ�
��ڲ�����pBuffer:���д������ݵĻ�������
          WriteAddr����ʼд��ĵ�ַ��
		  NumByteToWrite��д����ֽ���
�� �� ֵ���ޣ����������ݡ�	
��    ע����
************************************************/
void sFLASH_WritePage(unsigned char* pBuffer, unsigned long int WriteAddr, unsigned int NumByteToWrite)
{
  	//оƬдʹ�ܣ�д֮ǰ��дʹ�ܣ������޷�д�� 
  	sFLASH_WriteEnable();

  	//оƬʹ�� 
  	W25Q64_WCS_Clr();
  	//����дָ�� 
  	sFLASH_SendByte(sFLASH_CMD_PAGEPRO);
  	//����24λ��ַ 
  	sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  	sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  	sFLASH_SendByte(WriteAddr & 0xFF);

  	//ѭ�����ֽ�д�� 
  	while (NumByteToWrite--)
  	{
    	sFLASH_SendByte(*pBuffer);
    	pBuffer++;
  	}

  	//оƬ��ֹ 
  	W25Q64_WCS_Set();

  	//�ȴ�д���
  	sFLASH_WaitForWriteEnd();
}
/***********************************************
�������ƣ�sFLASH_ReadBuffer
��    �ܣ���W25Q64 FLASH��ȡһ�����ݡ�
��ڲ�����pBuffer:���ո��ַ����Ļ�����
          ReadAddr����ʼ���ĵ�ַ
		  NumByteToRead�������ֽ���
�� �� ֵ����	
��    ע����
************************************************/
void sFLASH_ReadBuffer(unsigned char * pBuffer, unsigned long int ReadAddr, unsigned int NumByteToRead)
{
  	//оƬʹ�� 
  	W25Q64_WCS_Clr();

  	//���Ͷ�����
  	sFLASH_SendByte(sFLASH_CMD_READ);

  	//����24λ��ַ 
  	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  	sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  	sFLASH_SendByte(ReadAddr & 0xFF);

	//ѭ�����ֽڶ�ȡ
  	while (NumByteToRead--) 
  	{
    	*pBuffer = sFLASH_ReadByte();
    	pBuffer++;
  	}

  	//оƬ��ֹ 
  	W25Q64_WCS_Set();
}
/***********************************************
�������ƣ�sFLASH_EraseSector
��    �ܣ�����W25Q64 FLASH��һ��������
��ڲ�������
�� �� ֵ��unsigned char�����������ݡ�	
��    ע����
************************************************/
void sFLASH_EraseSector(unsigned long int SectorAddr)
{
  	//оƬдʹ��
  	sFLASH_WriteEnable();

  	//оƬʹ�� 
  	W25Q64_WCS_Clr();
  	//������������ָ��
  	sFLASH_SendByte(sFLASH_CMD_SE);
  	//����24λ��ַ
  	sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  	sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  	sFLASH_SendByte(SectorAddr & 0xFF);
  	//оƬ��ֹ 
  	W25Q64_WCS_Set();

  	//�ȴ�д���
  	sFLASH_WaitForWriteEnd();
}