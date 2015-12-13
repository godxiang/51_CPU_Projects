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
sbit P36 = P3^6;

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3

sfr P_SW2   = 0xBA;             //���蹦���л��Ĵ���2
#define S3_S0 0x02              //P_SW2.1

bit busy;

//void SendData(BYTE dat);
//void SendString(char *s);
//void delay();
void UartInit(void);
void Delay1000ms();
void main()
{
	UartInit();
	while(1)
 {
	Delay1000ms();
 S3BUF=0x55;    //01010101
 while(!(S3CON&S3TI));		//00000010
 S3CON =S3CON & ~S3TI; 
 }
}
void UartInit(void)
{
	S3CON = 0x10;		
	S3CON &= 0xBF;		
	AUXR &= 0xFB;		
	T2L = 0xD0;		
	T2H = 0xFF;		
	AUXR |= 0x10;		
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







