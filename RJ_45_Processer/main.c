 //�ļ�����
#include "stc15w4k32s4.h"
#include "intrins.h"

#define FOSC 22118400L          //ʱ��Ƶ��
#define BAUD 115200             //������
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4_S0 0x04              //P_SW2.2
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

void init_Uart();
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);
void Uart4_Process(void);
unsigned char htd(unsigned char a);
void Delay1000ms();
void Delay200ms();

unsigned char ReceiveData_Buf[7]; //���ջ�����

bit Uart4busy;
bit Uart1busy;


void main(void)
	{
		unsigned char n;
		init_Uart();
		while(1){
		Delay1000ms();
		for (n=0;n<10;n++){
			Uart4SendData(FIND_CARD[n]);  
		}

		Uart4_Process();		
		}
	}
/*****************************************/
/* ����4��ʼ��													 */
/* �����ܣ�����4��ʼ��								 */
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
		EA=0;
	    if (S4CON & S4RI)
    {
			if(i<300){
			re_data_buffer[i++]=S4BUF;
			}	
        S4CON &= ~S4RI;        												//���ڽ����жϱ�����������
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
