#include "stc15w4k32s4.h"
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

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4RB8 0x04              //S4CON.2
#define S4TB8 0x08              //S4CON.3
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define S3_S0 0x02              //P_SW2.1
#define S4_S0 0x04
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80             
sbit Transmit_DATA0 =P2^0;
sbit Transmit_DATA1 =P2^1;
sbit Transmit_DATA2 =P2^2;
sbit Transmit_DATA3 =P2^3;
sbit Transmit_U2STB =P2^6;/*����������ʱ����Ҫ��ʹ��λ�øߣ�Ĭ��Ϊ������͵�ƽ*/
sbit Transmit_U2REQ =P2^7;/*�����յ��������������ĸߵ�ƽ�ź�ʱ��Ĭ��Ϊ���*/
sbit Transmit_U1STB =P2^4;/*�����������򱾻���������ʱ��Ĭ��Ϊ����*/
sbit Transmit_U1REQ =P2^5;/*�����������򱾻���������ʱ��Ĭ��Ϊ����*/
sbit Transmit_U2BusACK =P1^3;/*Ĭ��Ϊ�����æ�ź�*/
sbit Transmit_U1BusACK =P1^2;/*Ĭ�����룬æ�źż��*/



bit Uart1busy;
bit Uart3busy;
bit Uart4busy;

void Uart1SendData(BYTE dat);
void Uart1SendString(char *s);
void Uart3SendData(BYTE dat);
void Uart3SendString(char *s);
void Uart4SendData(BYTE dat);
void Uart4SendString(char *s);
unsigned char Sent_Byte(unsigned char Data0);
unsigned char Read_Byte(void);
void Transmit_init(void);
void Delay1000ms();
void Delay200ms();
void delay_10us(void);

void main()
{
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		P_SW2 &= ~S4_S0;   					//S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
//  P_SW2 |= S4_S0;             //S4_S0=1 (P5.2/RxD4_2, P5.3/TxD4_2)		
		SCON = 0x50;														
		S3CON = 0x10;
		S4CON = 0x10;
	

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
	 Transmit_init();
    
    while(1)
		{
			unsigned char test;
			//Uart3SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			//Uart1SendString("STC15W4K32S4\r\nUart3 Test !\r\n");
			P36=1;
			Delay1000ms();
			P36=0;
			//test = Read_Byte();
			Delay200ms();
		}
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
				Uart4SendData(S3BUF);
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


void Uart1SendData(BYTE dat)
{
    while (Uart1busy);              
    ACC = dat;                 
    if (P)                      
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;               
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;               
#endif
    }
    Uart1busy = 1;
    SBUF = ACC;                
}

void Uart1SendString(char *s)
{
    while (*s)                 
    {
         Uart1SendData(*s++);         
    }
}
/*----------------------------
���ʹ�������
----------------------------*/

void Uart3SendData(BYTE dat)
{
    while (Uart3busy);          
    Uart3busy = 1;
    S3BUF = dat;               
}

/*----------------------------
�����ַ���
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

/***************************************************************/
/*�������ƣ�Void Transmit_init(void)                           */
/*�������ܣ����ͳ�ʼ������                                     */
/*��ڲ�������                                                 */
/*���ڲ�������                                                 */
/***************************************************************/
void Transmit_init(void)
 { //P2M0=P2M0&(~BIT0);/*�趨P2.0Ϊ��������*/
   //P2M1=P2M0|BIT0;
   //P2M0=P2M0&(~BIT1);/*�趨P2.1Ϊ��������*/
  // P2M1=P2M0|BIT1;
  // P2M0=P2M0&(~BIT2);/*�趨P2.2Ϊ��������*/
  // P2M1=P2M0|BIT2;
 //  P2M0=P2M0&(~BIT3);/*�趨P2.3Ϊ��������*/
  // P2M1=P2M0|BIT3;
  // P2M0=P2M0&(~BIT4);/*�趨P2.4Ϊ���*/
  // P2M1=P2M0&(~BIT4);
  // P2M0=P2M0&(~BIT5);/*�趨P2.5Ϊ���*/
  // P2M1=P2M0&(~BIT5);
  // P2M0=P2M0&(~BIT6);/*�趨P2.6Ϊ��������*/
  // P2M1=P2M0|BIT6;
 //  P2M0=P2M0&(~BIT7);/*�趨P2.7Ϊ��������*/
  // P2M1=P2M0|BIT7;*/
	 P2M0 = 0x0c;				//00001100
	 P2M1 = 0xf3;				//11110011			�趨P2.0 P2.1 P2.2 P2.3 Ϊ�������� P2.4 P2.5Ϊ���  P2.6 P2.7Ϊ��������
   P1M0 = 0x20;			  //00100000
	 P1M1 = 0x10;				//00010000			�趨P1.2Ϊ���,�趨P1.3Ϊ��������
	 P3M0 = 0x00;						//00000000
	 P3M1 = 0x03;						//00000011
	 
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
	 {   if((Data0&BIT0)==BIT0) {Transmit_DATA0=Transmit_DATA0|BIT0;}
     	 else{Transmit_DATA0=Transmit_DATA0&(~BIT0);}
   		 if((Data0&BIT1)==BIT1) {Transmit_DATA0=Transmit_DATA0|BIT1;}
   		 else{Transmit_DATA0=Transmit_DATA0&(~BIT1);}
         if((Data0&BIT2)==BIT2) {Transmit_DATA0=Transmit_DATA0|BIT2;}
         else{Transmit_DATA0=Transmit_DATA0&(~BIT2);}
         if((Data0&BIT3)==BIT3) {Transmit_DATA0=Transmit_DATA0|BIT3;}
         else{Transmit_DATA0=Transmit_DATA0&(~BIT3);}
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


