#include "main.h"


void main(void)
	{
		init_Uart();
		IO_Init();
		Delay2000ms();												//�ȴ����֤ģ���ϵ��ȶ�
		P4M0 = 0;
		P4M1 = 0;
	
		while(1){
			
			if(Uart3HaveData){
			Delay200ms();												//�ȴ����ڽ����������
			UART3_Proccess();										//�������ݴ���
			}
		};
		//Sent_Byte	(0x0c);	
		
		
		//sFLASH_EraseSector(FLASH_SectorToErase);
		//sFLASH_ReadBuffer(TotalNumData,FLASH_ReadAddress,2);	 							   //�ϵ��ȶ��Ժ��ȶ�ȡFlash�д������ݵĸ���
		//Save_IDNum_ToFlash(TestIdData);
		//Op_IdCard();		
		//Flash_Op();
}
/*****************************************/
/* ����1��4��ʼ��													 */
/* �����ܣ�����1��4��ʼ��								 */
/* ��ڲ���:��												   */
/* ���ڲ���:��												   */
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
    T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;								//T2Ϊ1Tģʽ, ��������ʱ��2 10100
		AUXR |= 0x01;               //ѡ��ʱ��2Ϊ����1�Ĳ����ʷ�����
    ES = 1;                     //ʹ�ܴ���1�ж�
    IE2 = 0x18;                 //ʹ�ܴ���3,4�ж�
    EA = 1;
}
	

/*----------------------------
UART3 �жϷ������
-----------------------------*/
void Uart3_ISR() interrupt 17 using 1
{
	EA =0;
    if (S3CON & 0x01 )
    {
			if(Re_Cnt < 100)
				S3_Re_Buffer[Re_Cnt++] = S3BUF;
			Uart3HaveData = 1; 				//��־����3�����ݽ���
        S3CON &= ~0x01;         //���S3RIλ	
    }
    if (S3CON & 0x02)
    {
        S3CON &= ~0x02;         //���S3TIλ
        Uart3busy = 0;               //��æ��־
    }
		EA = 1;
}


/*----------------------------
UART4 �жϷ������
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
		/*if(length == 4 && Rece_Cnt < 12){																		//���ش�����Ϣ	
				//Re_Data_Buffer.Re_FindCard_Data.Failure[Rece_Cnt++]=dat;					//��������4���ֽ�����																			//������ǳɹ��Ա�һλ��Ӧ������ǩ��1 tag[0]--Ѱ�� tag[1]--ѡ�� tag[2]--����
				testdata[Rece_Cnt++] = dat;
		}
			
		else if(length == 8 && Rece_Cnt < 14){
				Re_Data_Buffer.Re_FindCard_Data.Seccess[Rece_Cnt++]=dat;					//��������8���ֽ�����
				 if(Rece_Cnt == 13){																							//������Ͽ�ʼ�Ա�����		
					for(i = 6;i < 13; i++){
						if( ! Re_Data_Buffer.Re_FindCard_Data.Seccess[i] == Re_Data.Re_FindCard_Data.Seccess[i])
							OpCart_FindCart_State = 0;
					}
					OpCart_FindCart_State = 1;
				}
		}
		else if(length == 12 && Rece_Cnt < 19){
			Re_Data_Buffer.Re_SelectCard_Data.Seccess[Rece_Cnt++]=dat;				//��������12���ֽ�����
			if(Rece_Cnt == 18){																							//������Ͽ�ʼ�Ա�����		
					for(i = 6;i < 18; i++){
						if( ! Re_Data_Buffer.Re_SelectCard_Data.Seccess[i] == Re_Data.Re_SelectCard_Data.Seccess[i])
							OpCart_SelectCart_State = 0;
					}
					OpCart_SelectCart_State = 1;
				}
		}
		else if(length == 1288 && Rece_Cnt < 1295){
			if(Rece_Cnt < 10)
			Re_Data_Buffer.Re_ReadCard_Data.Seccess[Rece_Cnt++]=dat;				//��������3���ֽ�����(���سɹ���־λ)
			if(Rece_Cnt == 9){																							//������Ͽ�ʼ�Ա�����		
					for(i = 7;i < 10; i++){
						if( ! Re_Data_Buffer.Re_ReadCard_Data.Seccess[i] == Re_Data.Re_ReadCard_Data.Seccess[i])
							OpCart_ReadCart_State = 0;
					}
					OpCart_ReadCart_State = 1;
				}
					if(Rece_Cnt > 137  && Rece_Cnt < 167){																														//�������ݣ����������֤����
						Rece_Cnt++;																																											//ÿ��1���ֽ�ȡ����
						Id_Number[Rece_Cnt++] = dat;																																		//18λ���֤�ű�������
					}
		}*/
        S4CON &= ~S4RI;        																				//���ڽ����жϱ�����������
		}
				
		
		if (S4CON & S4TI)
		{
			S4CON&= ~S4TI;
			Uart4busy=0;
		}
	EA=1;
}
/***********************************************
*�������ƣ�Uart4_Process
*��    �ܣ������������
*��ڲ�������
*�� �� ֵ����	
*��    ע����
************************************************/
void Uart4_Process(void)  
{
	unsigned int i,j;
	length = Hex2Int (re_data_buffer[5])*256 + Hex2Int (re_data_buffer[6]);//ʮ������ת��Ϊ10����
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
		for(j=0,i = 136;j<18,i < 171;i++,j++){																		//ȡ���֤����																																			
			Id_Number[j] = re_data_buffer[i];
			i++;																																		//��һλȡһλ
		}	
	}
	Rece_Cnt = 0;																																//���ռ���������
}
/*----------------------------------------
����3������λ����Ϣ����WIFIģ�飩����
----------------------------------------*/
void UART3_Proccess(void){
	bit Header_State;
	unsigned char i,j = 0;
	unsigned char CS = 0x00;
		if(S3_Re_Buffer[0] == 0x55 && S3_Re_Buffer[1] == 0xaa)
			Header_State = 1;
		else
			Header_State = 0;
		if(Header_State == 1){																								//ͷû�г���
		Re_length = Hex2Int (S3_Re_Buffer[2])*256 + Hex2Int (S3_Re_Buffer[3]);//���data�ĳ��ȣ��Ա����У����CS
		for(i=4;i<10;i++){
				if(LocalHostNum[i] == S3_Re_Buffer[i])														//�ж��Ƿ��Ǳ������
					j++;
			
		}
			if(j==6){																														//�Ǳ������
				for	(j = 10;j<Re_length + 10;j++){																//����У����CS
					CS+=S3_Re_Buffer[j];
				}
				if(CS == S3_Re_Buffer[10+Re_length])															//У����CS�Ƿ���� ��������������ݽ���
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
		Uart3HaveData = 0; 																											//���ݴ������ô���3���ݱ��Ϊ0���ȴ���һ�����ݽ���
		Re_Cnt = 0;
}

/***********************************************
*�������ƣ�Flash_Op
*��    �ܣ�flash��������
*��ڲ�������
*�� �� ֵ����	
*��    ע����
************************************************/
void Flash_Op(void){
	unsigned int Cnt,Cnt0,ID;
	ID=sFLASH_ReadID();							//��ȡW25Q64������ID��																		
		if(ID!=sFLASH_W25Q64_ID)				//���ID�Ų�ͬ
		{
			Uart1SendString("ID ERROR!");
			while(1);	
		}

	//Uart1SendString("Start Read Write Test!\n");
	//Uart1SendString("Erase!\n");
	//Delay_ms(100);
	//д֮ǰ���Ȳ���������һ������
	//sFLASH_EraseSector(FLASH_SectorToErase);
	//Uart1SendString("Write!\n");
	//Delay_ms(300);
	//д��һҳ������
	//sFLASH_WritePage(Id_Number,FLASH_WriteAddress,18);
	//Uart1SendString("Read!\n");
	Delay_ms(300);
	//����ҳ�����ݶ�
	sFLASH_ReadBuffer(spi_re_data_buffer,FLASH_Address,20);
	//�Ƚ�д��Ͷ�ȡ�������Ƿ�һ��
	for(Cnt0=0,Cnt=2;Cnt<20,Cnt0<18;Cnt++,Cnt0++) 
	{
		//Uart1SendData(spi_re_data_buffer[Cnt]);
		//��������ݲ�ͬ
		if(spi_re_data_buffer[Cnt]!=Id_Number[Cnt0])
		{			
			//ToDisplayError(ERR2);//��ʾ������Ϣ
			//Uart1SendString("Read Error!");
			while(1);			
		}	
		
	}	
	if(Cnt == 0x0014){															//����û�д�
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
*�������ƣ�Save_IDNum_ToFlash
*��    �ܣ�����λ����������������طŽ�Flash��������
*��ڲ���1��Data 20λ��2λ���  18λ���֤�ţ�
*�� �� ֵ����	
*��    ע����
************************************************/
void Save_IDNum_ToFlash(unsigned char * Data){
	

	TotalNumData[0] = (unsigned char)(TotalNum>>8);												//TotalNum�ĸ�8λ
	TotalNumData[1] = (unsigned char)TotalNum;														//TotalNum�ĵ�8λ
	sFLASH_WritePage(TotalNumData,FLASH_WriteAddress,2);									//����һ�����ݵĸ���  �Ա��´�д����׷��  �´�д�����ȶ�ȡ����� Ȼ��λд�ĵ�ַ
	TotalNum = Hex2Int (TotalNumData[0])*256 + Hex2Int (TotalNumData[1]);	//ʮ������ת��Ϊ10����
	FLASH_Address = (unsigned long int)(TotalNum*20);
	FLASH_Address+=2;																											//ָ������2���ֽ�
	sFLASH_WritePage(Data,FLASH_Address,20);
	TotalNum++;
	
}
/***********************************************
*�������ƣ�Op_IdCard
*��    �ܣ����֤ģ���������
*��ڲ�������
*�� �� ֵ����	
*��    ע����
************************************************/
void Op_IdCard(void){
	unsigned int n,Cnt;
	while(1){																								//����ѭ���ȴ��ſ�
		while(!OpCart_FindCart_State){												//һֱ�ȴ�Ѱ���ɹ�
			for (n=0;n<10;n++){
					Uart4SendData(FIND_CARD[n]);  
				}
						//Uart4_Process();
				Delay200ms();			//�ȴ���������
				Uart4_Process();
			}
	if(OpCart_FindCart_State == 1){
		Cnt = 0;
		while(!OpCart_SelectCart_State && Cnt < 10){										  //һֱ�ȴ�ѡ���ɹ�,�������10�� ����Ѱ���ɹ����Ƴ��˿� ��������ѭ��
			for (n=0;n<10;n++){
				Uart4SendData(SELECT_CARD[n]);  
			}
					//Uart4_Process();	
			Delay200ms();				//�ȴ���������
			Uart4_Process();
			Cnt++;
		}
	}
	if(OpCart_FindCart_State == 1 && OpCart_SelectCart_State == 1){
		Cnt = 0;
		while(!OpCart_ReadCart_State && Cnt < 3){												//һֱ�ȴ������ɹ�
			for (n=0;n<10;n++){
				Uart4SendData(READ_CARD[n]);  
			}
			Delay2000ms();																			//�ȴ��������ݣ�����ģ�鷵�������ӳٸ���Ҫ�ȴ���ʱ�䳤һЩ��
			Uart4_Process();
			Cnt++;
		}
	if(Cnt == 1)
	Flash_Op();
	}
		OpCart_FindCart_State = 0;														//��ȡһ�ſ������³�ʼ��
		OpCart_SelectCart_State = 0;
		OpCart_ReadCart_State = 0;
	}
}
/*----------------------------
����4��������
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
UART �жϷ������
-----------------------------*/
void Uart1_ISR() interrupt 4 using 1
{
	EA=0;
    if (RI)
    {
        RI = 0;                 //���RIλ
    }
    if (TI)
    {
        TI = 0;                 //���TIλ
        Uart1busy = 0;               //��æ��־
    }
	EA=1;
}

/*----------------------------
���ʹ�������
----------------------------*/
void Uart1SendData(unsigned char dat)
{
    while (Uart1busy);               //�ȴ�ǰ������ݷ������
    Uart1busy = 1;
    SBUF = dat;                 //д���ݵ�UART���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/
void Uart1SendString(char *s)
{
    while (*s)                  //����ַ���������־
    {
        Uart1SendData(*s++);         //���͵�ǰ�ַ�
    }
}
/*----------------------------
����3���ʹ�������
----------------------------*/

void Uart3SendData( unsigned char dat )
{
    while (Uart3busy);          
    Uart3busy = 1;
    S3BUF = dat;               
}

/*----------------------------
����3�����ַ���
----------------------------*/

void Uart3SendString(char *s)
{
    while (*s)               
    {
        Uart3SendData(*s++);        
    }
}


/***********************************************
�������ƣ�IO_Init
��    �ܣ���Ƭ��IO�˿�ģʽ��ʼ��
��ڲ�������
�� �� ֵ����	
��    ע��STC15W4K32S4ϵ��A�浥Ƭ�����ֶ˿ڸ�λ��
          ����׼˫���,��Ҫ���ò�������ʹ��
************************************************/
void IO_Init(void)
{

	//��P04 P05 P06 P07����Ϊ��©��
	//��Ϊ��Ƭ��Ϊ5V�˿ڣ�W25Q64Ϊ3.3V�˿ڣ�Ϊ��ʵ�ֵ�ƽƥ��
	//����Ƭ������Ϊ��©�ṹ�����ⲿ������3.3V��
	
	 P0M1 |= (1<<4) | (1<<5) | (1<<6) | (1<<7) ;  
 	 P0M0 |= (1<<4) | (1<<5) | (1<<6) | (1<<7) ;
	//����
	//  1<<0�ȼ���0x01 �� 0000 0001
	//  1<<1�ȼ���0x02 �� 0000 0010
	//  1<<2�ȼ���0x04 �� 0000 0100
	//  1<<3�ȼ���0x08 �� 0000 1000
	//  �Դ�����1<<n ����nλΪ1������λ��0

	//  x |=(1<<n)  ����xִ�а�λȡ�� 
	//  ��x�еĵ�nλ��Ϊ1�����ı�����λ״̬  
	
	//  y &=~(1<<n)  ����1<<n��λȡ����Ȼ���y��λȡ�� 
	//  ��y�еĵ�nλ��Ϊ0�����ı�����λ״̬  
   P2M0 = 0x30;				//00110000
	 P2M1 = 0xc0;				//11000000			�趨P2.0 P2.1 P2.2 P2.3 Ϊ�������� P2.4 P2.5Ϊ���  P2.6 P2.7Ϊ��������
   P1M0 = 0x20;			  //00000100
	 P1M1 = 0x10;				//00001000			�趨P1.2Ϊ���,�趨P1.3Ϊ��������
	 
	 
	//P10 P14��λ��Ϊǿ���������ǿ������
	//��ֹ�����������¶˿��𻵣���ʼ��Ϊ׼˫��� 
	//��P10 P14����Ϊ׼˫���
	//P1M1 &=~( (1<<0) | (1<<4) );  
	//P1M0 &=~( (1<<0) | (1<<4) );
	
}															
/***************************************************************/
/*�������ƣ�void Sent_Byte(unsigned char Data0)                */
/*�������ܣ�����һ���ֽڵĵ�4λ                                */
/*��ڲ�������                                                 */
/*���ڲ�������                                                 */
/*�����������̢� һ����Ƭ�����8λ�˿�״̬�ź�CHKREQ �����ж϶Է��Ƿ����� ���������ݵ����롣
              �� ����Է����������ȴ���������������������룬����REQ��Ч�� 
			  �� �ٴμ��8λ�˿�״̬�ź�CHKREQ�ж϶Է��Ƿ�����������ݵ����롣
			  �� ����Է��������������ͻ�����REQ����ʱ��Ȼ��ִ�е�һ����
			  �� ��������8λ�˿ڣ����趨STB��Ч, ʹ�Է������ж��Ա�ȡ�����ݡ�
			  �� ���CHK���ȴ��Է���Ƭ��ȡ�����ݡ�
			  �� ��������Ƿ� ���꣬���û�������ִ�в���5���з��͡�
			  �� ����REQ�źţ��ͷ�8λ�˿ڡ�
			  �� ���ݷ��� ����˳����̡�
���ݽ������̣��ٽ����жϷ������̡�
              �ڴ�8λ�˿ڶ�ȡ���ݡ�
			  ������ACK�ź��� Ч����ʾ�����ѳɹ���ȡ��
			  ���˳��жϷ������̡�
/*           P2>----------------------P2                     */
/*           STB>---------------------INT                    */
/*           REQ>---------------------CHKREQ                 */
/*           INT<---------------------STB                    */
/*           CHKREQ<---------------------REQ                 */
/*           ACK--------------------->CHKACK               */
/*           CHKACK<---------------------ACK               */
/*************************************************************/
unsigned char  Sent_Byte(unsigned char Data0)/*����һ���ֽڵĵ�4λ*/
  {/*���ȼ�⵱ǰ��λ�����Ƿ�æ*/
  if((!Transmit_U2REQ)&&(Transmit_U2BusACK))/*���U2�Ƿ���������������룬��������ȴ�*/
   {/*U2�ȴ�*/
    Transmit_U1REQ=1;/*�������*/
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
	     Transmit_U1STB=1;/*����ж�����*/
		 while(!Transmit_U2BusACK);/*�ȴ��˳�*/
		 Transmit_U1REQ=0;
		 Transmit_U1STB=0;
		 return(0);
	  }
	else 
	 {Transmit_U1REQ=0;return(1);}
    }
  else
   {return(1);}/*U2æ*/
   }
/***************************************************************/
/*�������ƣ�unsigned char Read_Byte(void)                      */
/*�������ܣ�����һ���ֽڵĵ�4λ                                */
/*��ڲ�������                                                 */
/*���ڲ�������                                                 */
/***************************************************************/
unsigned char Read_Byte(void)/*����һ���ֽڵĵ�4λ*/
  { unsigned char Read_Byte_data=0;
    if(Transmit_U2STB)/*U2STB=1,��ȡһ������*/									  
     { if(Transmit_DATA0) {Read_Byte_data=Read_Byte_data|BIT0;}
	   if(Transmit_DATA1) {Read_Byte_data=Read_Byte_data|BIT1;}
	   if(Transmit_DATA2) {Read_Byte_data=Read_Byte_data|BIT2;}
	   if(Transmit_DATA3) {Read_Byte_data=Read_Byte_data|BIT3;}
	   Transmit_U2BusACK=1;/*�������ݶ�ȡ�����Ӧ�ź�*/
	   delay_10us();
	   Transmit_U2BusACK=0;/*�������ݶ�ȡ�����Ӧ�ź�*/
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
�������ƣ�Delay_ms
��    �ܣ�STC15ϵ�е�Ƭ��1ms��ʱ����
��ڲ�����ms:��ʱ�ĺ�����
�� �� ֵ����	
��    ע��ʾ����ʵ�⣺0.997ms���ڲ�ʱ�ӣ�11.0592MHz           
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
