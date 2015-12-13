
#include "reg51.h"
#include "intrins.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 22118400L          //ϵͳƵ��
#define BAUD 115200             //���ڲ�����

#define NONE_PARITY     0       //��У��
#define ODD_PARITY      1       //��У��
#define EVEN_PARITY     2       //żУ��
#define MARK_PARITY     3       //���У��
#define SPACE_PARITY    4       //�հ�У��

#define PARITYBIT NONE_PARITY   //����У��λ

sfr AUXR  = 0x8e;               //�����Ĵ���
sfr S3CON = 0xac;               //UART3 ���ƼĴ���
sfr S3BUF = 0xad;               //UART3 ���ݼĴ���
sfr T2H   = 0xd6;               //��ʱ��2��8λ
sfr T2L   = 0xd7;               //��ʱ��2��8λ
sfr IE2   = 0xaf;               //�жϿ��ƼĴ���2
sfr P_SW1   = 0xA2;
sbit P36 = P3^6;
#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define S3_S0 0x02              //P_SW2.1
sfr P_SW2   = 0xBA;             //���蹦���л��Ĵ���2


bit busy;
bit busy0;

void SendData(BYTE dat);
void SendString(char *s);
void Uart1SendData(BYTE dat);
void Uart1SendString(char *s);
void Delay1000ms();
void Delay200ms();

void main()
{
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		SCON = 0x50;
		S3CON = 0x10;
	
/*    P_SW2 &= ~S3_S0;            //S3_S0=0 (P0.0/RxD3, P0.1/TxD3)
//  P_SW2 |= S3_S0;             //S3_S0=1 (P5.0/RxD3_2, P5.1/TxD3_2)
#if (PARITYBIT == NONE_PARITY)
		SCON = 0x50;     
    S3CON = 0x10;               //8λ�ɱ䲨����

#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    S3CON = 0x9a;               //9λ�ɱ䲨����,У��λ��ʼΪ1
#elif (PARITYBIT == SPACE_PARITY)
    S3CON = 0x92;               //9λ�ɱ䲨����,У��λ��ʼΪ0
#endif
*/
		AUXR = 0x40;               //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x20;                //��ʱ��1Ϊģʽ2(8λ�Զ�����)
    TL1 = (256 - (FOSC/32/BAUD));   //���ò�������װֵ
    TH1 = (256 - (FOSC/32/BAUD));
    TR1 = 1;                   //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
    
		T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
		AUXR = 0x54;                //T2Ϊ1Tģʽ, ��������ʱ��2
    IE2 = 0x08;                 //ʹ�ܴ���3�ж�
    EA = 1;

    
    while(1)
		{
			SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			//Uart1SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			P36=1;
			Delay1000ms();
			P36=0;
			Delay200ms();
		}
}

/*----------------------------
UART3 �жϷ������
-----------------------------*/
void Uart3() interrupt 17 using 1
{
    if (S3CON & S3RI)
    {
        S3CON &= ~S3RI;         //���S3RIλ
				Uart1SendData(S3BUF);
        //Uart1SendData(S3BUF);             //P30��ʾ��������
			  //SendString("interrupt !\r\n");
			
    }
    if (S3CON & S3TI)
    {
        S3CON &= ~S3TI;         //���S3TIλ
        busy = 0;               //��æ��־
    }
}

/*----------------------------
UART1 �жϷ������
-----------------------------*/
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                                        
    }
    if (TI)
    {
        TI = 0;              
        busy0 = 0;            
    }
}

/*----------------------------
���ʹ�������
----------------------------*/

void SendData(BYTE dat)
{
    while (busy);               //�ȴ�ǰ������ݷ������
    busy = 1;
    S3BUF = dat;                //д���ݵ�UART3���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/

void SendString(char *s)
{
    while (*s)                  //����ַ���������־
    {
        SendData(*s++);         //���͵�ǰ�ַ�
    }
}

void Uart1SendData(BYTE dat)
{
    while (busy0);               //???????????
    ACC = dat;                  //?????P (PSW.0)
    if (P)                      //??P??????
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //??????0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //??????1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //??????1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //??????0
#endif
    }
    busy0 = 1;
    SBUF = ACC;                 //????UART?????
}

void Uart1SendString(char *s)
{
    while (*s)                 
    {
         Uart1SendData(*s++);         
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


