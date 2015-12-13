#include "uart.h"
#include "stc15w4k32s4.h"

bit Uart1busy;
bit Uart3busy;
bit Uart4busy;
void InitUart()
{
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		P_SW2 &= ~S4_S0;   					//S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
//  P_SW2 |= S4_S0;             //S4_S0=1 (P5.2/RxD4_2, P5.3/TxD4_2)		
		SCON = 0x50;														
		S3CON = 0x10;
		S4CON = 0x10;
	
/*    P_SW2 &= ~S3_S0;            //S3_S0=0 (P0.0/RxD3, P0.1/TxD3)
//  	P_SW2 |= S3_S0;             //S3_S0=1 (P5.0/RxD3_2, P5.1/TxD3_2)
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
    IE2 = 0x18;                 //ʹ�ܴ���3,4�ж�
    EA = 1;
	}
/*----------------------------
UART1 �жϷ������
-----------------------------*/
void Uart1() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                                        
    }
    if (TI)
    {
        TI = 0;              
        Uart1busy = 0;            
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
				//Uart4SendData(S3BUF);
        //Uart1SendData(S3BUF);             //P30��ʾ��������
			  //SendString("interrupt !\r\n");
			
    }
    if (S3CON & S3TI)
    {
        S3CON &= ~S3TI;         //���S3TIλ
        Uart3busy = 0;               //��æ��־
    }
}
/*----------------------------
UART4 �жϷ������
-----------------------------*/
void Uart4() interrupt 18 using 1
{
    if (S4CON & S4RI)
    {
        S4CON &= ~S4RI;         
        Uart1SendData(S4BUF);                
    }
    if (S4CON & S4TI)
    {
        S4CON &= ~S4TI;        
        Uart4busy = 0;             
    }
}

/*----------------------------
UART1 ��������
-----------------------------*/
void Uart1SendData(BYTE dat)
{
    while (Uart1busy);              
    Uart1busy = 1;
    SBUF = dat;                
}

void Uart1SendString(char *s)
{
    while (*s)                 
    {
         Uart1SendData(*s++);         
    }
}
/*----------------------------
����3��������
----------------------------*/

void Uart3SendData(BYTE dat)
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

/*----------------------------
����4��������
----------------------------*/
void Uart4SendData(BYTE dat)
{
    while (Uart4busy);                            
    Uart4busy = 1;
    S4BUF = dat;              
}

/*----------------------------
����4�����ַ���
----------------------------*/
void Uart4SendString(char *s)
{
    while (*s)                 
    {
        Uart4SendData(*s++);         
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


