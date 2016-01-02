#include "STC15W4KXX.h"
#include <intrins.H>
#define S2_S0 0x01              //P_SW2.0
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define FOSC 11059200L          //ϵͳƵ��
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
#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3 
#define FE_START 0x68/*֡ͷ*/
#define	FE_Adress_length 1 /*֡��ַ�ֽڵĳ���*/
#define	FE_DATALENGTH FE_Adress_length+4 /*����֡�����ݳ�������֡�е�λ��*/
#define FE_OVER 0x16/*֡β*/
#define UART_FERE_DATALENGTH FE_DATALENGTH-2/*���յ����ݳ����ֽ�ʱ��Ӧ��UART_RE_DATA_Counterֵ*/  
#define FE_TXDATA_FIXLENGTH  6+FE_Adress_length //���������ݳ����������ֽ���Ĺ̶����ȣ������ֽڲ��㣬����֡ͷ
#define FE_CTL 2/*�������λ��*/          
sbit Transmit_DATA0 =P2^0;
sbit Transmit_DATA1 =P2^1;
sbit Transmit_DATA2 =P2^2;
sbit Transmit_DATA3 =P2^3;
sbit Transmit_U1STB =P2^4;/*����������ʱ����Ҫ��ʹ��λ�øߣ�Ĭ��Ϊ������͵�ƽ*/
sbit Transmit_U1REQ =P2^5;/*�����յ��������������ĸߵ�ƽ�ź�ʱ��Ĭ��Ϊ���*/
sbit Transmit_U2STB =P2^6;/*�����������򱾻���������ʱ��Ĭ��Ϊ����*/
sbit Transmit_U2REQ =P2^7;/*�����������򱾻���������ʱ��Ĭ��Ϊ����*/
sbit Transmit_U1BusACK =P1^2;/*Ĭ��Ϊ�����æ�ź�*/
sbit Transmit_U2BusACK =P1^3;/*Ĭ�����룬æ�źż��*/
unsigned int i=0;
unsigned char xdata UART2_Fe_flag=0;
unsigned char xdata UART2_Fe_DATA_Buf=0;
unsigned int xdata UART2_RE_DATA_Counter=0;
unsigned int UART2_Fe_DATA_CounterBuf=0;
unsigned char xdata UART2_Fe_DATA_CS=0;
unsigned char xdata UART2_RE_DATA[32];
unsigned char xdata UART2_Fe_RE_DATA=0;
unsigned char UART2_int_buf1=0;
void delay_10us(void);
void Uart2_intrupt_function(void);
/***************************************************************/
/*�������ƣ�Void Transmit_init(void)                           */
/*�������ܣ����ͳ�ʼ������                                     */
/*��ڲ�������                                                 */
/*���ڲ�������                                                 */
/***************************************************************/
void Transmit_init(void)
 { P2M0=P2M0&(~BIT0);/*�趨P2.0Ϊ��������*/
   P2M1=P2M0|BIT0;
   P2M0=P2M0&(~BIT1);/*�趨P2.1Ϊ��������*/
   P2M1=P2M0|BIT1;
   P2M0=P2M0&(~BIT2);/*�趨P2.2Ϊ��������*/
   P2M1=P2M0|BIT2;
   P2M0=P2M0&(~BIT3);/*�趨P2.3Ϊ��������*/
   P2M1=P2M0|BIT3;
   P2M0=P2M0&(~BIT4);/*�趨P2.4Ϊ���*/
   P2M1=P2M0&(~BIT4);
   P2M0=P2M0&(~BIT5);/*�趨P2.5Ϊ���*/
   P2M1=P2M0&(~BIT5);
   P2M0=P2M0&(~BIT6);/*�趨P2.6Ϊ��������*/
   P2M1=P2M0|BIT6;
   P2M0=P2M0&(~BIT7);/*�趨P2.7Ϊ��������*/
   P2M1=P2M0|BIT7;
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
{
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
    //ES = 1;                     //ʹ�ܴ����ж�
    //EA = 1;
    //P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
    P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
    S2CON = 0x50;               //8λ�ɱ䲨����
	T2L = (65536 - (FOSC/4/BAUD1));		//�趨��ʱ��ֵ
	T2H = (65536 - (FOSC/4/BAUD))>>8;		//�趨��ʱ��ֵ
    AUXR = 0x54;                //T2Ϊ1Tģʽ, ��������ʱ��2
    IE2 = 0x01;                 //ʹ�ܴ���2�ж�
    EA = 1;
    while(1)
	  {  i=i+1;  
	    if(i>65505){SBUF=0x5a;while(!TI);TI=0;i=0;}
		/*39 37 38 37 31 32 31 30 30 31 36 39 37 
	    if((S2CON&0x01)==0x01)
	     { SBUF=0x68;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x0b;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x00;while(!TI);TI=0;
		   SBUF=0x76;while(!TI);TI=0;
		   SBUF=0x16;while(!TI);TI=0;
		  }  */   
		//Sent_Byte(0x55);
		//if(i>65505){S2BUF=0x43;while(!(S2CON&0X02));S2CON &=~0X02;}
	   }
}
  /*----------------------------
UART2 �жϷ������
-----------------------------*/
void Uart2() interrupt 8 using 1
{Uart2_intrupt_function();
}
/*****************************************************************************/
/*����2����֡���ճ���*/
/*������֯��ͨ�ó���ֻ�ô�����صĲ������Լ��ϳɷ���֡������ṹ�������   */
/*****************************************************************************/
void Uart2_intrupt_function(void)
{if(S2CON&0X01)  /*����*/
   {S2CON &=~0X01;								   
 /*���յ����ݺ����㳬ʱ������*/
    UART2_Fe_DATA_Buf=S2BUF;
	SBUF=0x68;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x0b;while(!TI);TI=0;
		   SBUF=0x01;while(!TI);TI=0;
		   SBUF=0x00;while(!TI);TI=0;
		   SBUF=0x76;while(!TI);TI=0;
		   SBUF=0x16;while(!TI);TI=0;
    UART2_Fe_RE_DATA=UART2_Fe_DATA_Buf;
	if((UART2_Fe_flag &BIT3)==0)/*��֡δ�����꣬�򲻽���֡����*/
   {
   if((UART2_Fe_flag &BIT1)==0)/*�Ƿ���յ�֡ͷ*/
    {/*δ���յ�֡ͷ*/
      if(UART2_Fe_DATA_Buf==FE_START)
      { UART2_Fe_flag |=BIT1;/*��λ֡ͷ���յ���־λ*/
        UART2_Fe_DATA_CS=UART2_Fe_DATA_Buf;/*У�����*/
		/*���յ�֡ͷ��ʼ��ʱ�����ﵽ��ٺ���󣬻���û���������յ����ݣ���ֹͣ���գ��������б�־λ*/
		//Flag_UART_TimeOut250msStart=1;/* ������ʱ��ʱ*/
		//ET0=0;
	    //TR0 = 0;		//��ʱ��0��ʼ��ʱ
      }
    }
    else/*�ѽ��յ�֡ͷ�������յ������ݴ洢�ڽ������ݴ洢�����У��ڲ���Ӧ���ж��Ƿ���յ�֡β*/
    {/*���ݽ�����������ָ�뽫���ݴ洢�ڽ��������У����ڽ��չ����з�������У����Ƿ���ȷ����ȷ*/
     UART2_RE_DATA[UART2_RE_DATA_Counter]=UART2_Fe_DATA_Buf;/*�ݴ��������*/
     if(UART2_RE_DATA_Counter>UART_FERE_DATALENGTH)/*�ж��Ƿ���յ����������ֽ�*/
     {UART2_Fe_DATA_CounterBuf=FE_DATALENGTH+UART2_RE_DATA[FE_DATALENGTH-1];/*ָ��У�����λ��*/
      if(UART2_RE_DATA_Counter==UART2_Fe_DATA_CounterBuf)
	  { 
		//RS485_CTL=1; SBUF=UART_Fe_DATA_CS;while(!TI0);TI0=0;RS485_CTL=0;
		//RS485_CTL=1; SBUF=UART_RE_DATA[UART_RE_DATA_Counter];while(!TI0);TI0=0;RS485_CTL=0;
		//RS485_CTL=1; SBUF=UART_RE_DATA_Counter;while(!TI0);TI0=0;RS485_CTL=0;
		//RS485_CTL=1; SBUF=UART_Fe_DATA_CounterBuf;while(!TI0);TI0=0;RS485_CTL=0;
		UART2_int_buf1=UART2_RE_DATA[UART2_RE_DATA_Counter];
	   if((UART2_Fe_DATA_CS-UART2_int_buf1)==0)/*�ж��Ƿ���յ�У����*/
      { UART2_Fe_flag &=~BIT2;UART2_Fe_flag |=BIT4;}/*����֡����У��ʹ����־λ*/        
       else
       { UART2_Fe_flag |=BIT2;UART2_Fe_flag |=BIT4;}/*��λ֡����У��ʹ����־λ*/	
	  }
	  UART2_int_buf1=UART2_RE_DATA[UART2_RE_DATA_Counter];
      if(((UART2_RE_DATA_Counter-(UART2_Fe_DATA_CounterBuf+1))==0)&&((UART2_int_buf1-FE_OVER)==0))/*�ж��Ƿ���յ�֡β�������ݽ������*/
       {  UART2_Fe_flag |=BIT3;}/*�ǣ���֡�������*/ 
     }
     /*���պ���У��ͣ������յ�У����ʱ��ֹͣ���*/
     if((UART2_Fe_flag&BIT4)==0)
     {UART2_Fe_DATA_CS=UART2_Fe_DATA_CS+UART2_Fe_RE_DATA;}
     UART2_RE_DATA_Counter=UART2_RE_DATA_Counter+1;
	 if(UART2_RE_DATA_Counter>(UART2_RE_DATA_Counter+16)) 
	   {UART2_RE_DATA_Counter=UART2_RE_DATA_Counter+16;}
    }
  }
 }
}
