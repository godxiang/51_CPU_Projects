#include "STC15W4K32S4.h"
#include <intrins.H>
#define S2_S0 0x01              //P_SW2.0
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define FOSC 22118400L          //ϵͳƵ��
#define BAUD 9600             //���ڲ�����
#define BAUD1 9600             //���ڲ�����
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
void delay_10us(void);
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
   P1M0=P2M0&(~BIT2);/*�趨P1.2Ϊ���*/
   P1M1=P2M0&(~BIT2);
   P1M0=P2M0&(~BIT3);/*�趨P1.3Ϊ��������*/
   P1M1=P2M0|BIT3;
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
void main(void)
{	unsigned char uart_data[20]=0;unsigned int i=0;
    Transmit_init();

    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)     
    SCON = 0x50;                //8λ�ɱ䲨����
    AUXR = 0x40;                //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x00;                //��ʱ��1Ϊģʽ0(16λ�Զ�����)
    TL1 = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
    EA = 1;
    //P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
    P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
    S2CON = 0x50;               //8λ�ɱ䲨����
    T2L = (65536 - (FOSC/4/BAUD1));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD1))>>8;
    AUXR = 0x54;                //T2Ϊ1Tģʽ, ��������ʱ��2
    IE2 = 0x01;                 //ʹ�ܴ���2�ж�
    EA = 1;
    while(1)
	  { //i++;      
	  //if(i>65505){SBUF=0x43;while(!TI);TI=0;i=0;}

	   /*if((S2CON&0x01)==0x01)
	    {S2CON=S2CON&0xfe;
		 uart_data[0]=S2BUF;
		 SBUF=uart_data[0];while(!TI);TI=0;
		 } */
	   }
	  ;
}

