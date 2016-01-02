#include "main.h"


void main(void)
	{
		init_Uart();
		while(1){
		Delay2000ms();												//�ȴ����֤ģ���ϵ��ȶ�
		UART4_Proccess();
		}
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
		S4CON = 0x10;             	//10000
    T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;								//T2Ϊ1Tģʽ, ��������ʱ��2 10100
		AUXR |= 0x01;               //ѡ��ʱ��2Ϊ����1�Ĳ����ʷ�����
    ES = 1;                     //ʹ�ܴ���1�ж�
    IE2 = 0x10;             		//ʹ�ܴ���4�ж�
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
			if(Re_Cnt < 100)
			Re_Buffer[Re_Cnt++] = dat;
      S4CON &= ~S4RI;        																				//���ڽ����жϱ�����������
		}
				
		
		if (S4CON & S4TI)
		{
			S4CON&= ~S4TI;
			Uart4busy=0;
		}
	EA=1;
}
/*----------------------------
����4������
----------------------------*/
void UART4_Proccess(void){
	bit Header_State;
	unsigned char i,j = 0;
	unsigned char CS = 0x00;
		if(Re_Buffer[0] == 0x55 && Re_Buffer[1] == 0xaa)
			Header_State = 1;
		else
			Header_State = 0;
		if(Header_State == 1){																						//ͷû�г���
		length = Hex2Int (Re_Buffer[2])*256 + Hex2Int (Re_Buffer[3]);			//���data�ĳ��ȣ��Ա����У����CS
		for(i=4;i<10;i++){
				if(LocalHostNum[i] == Re_Buffer[i])														//�ж��Ƿ��Ǳ������
					j++;
			
		}
			if(j==6){																												//�Ǳ������
				for	(j = 11;j<length + 11;j++){																		//����У����CS
					CS+=Re_Buffer[j];
				}
				if(CS == Re_Buffer[11+length])															//У����CS�Ƿ���� ��������������ݽ���
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
UART1 �жϷ������
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
