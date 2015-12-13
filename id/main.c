 //�ļ�����
#include "stc15w4k32s4.h"
#include "w25q64.h"

#define FOSC 22118400L          //ʱ��Ƶ��
#define BAUD 115200             //������
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4_S0 0x04              //P_SW2.2
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

void init_Uart();
void IO_Init(void);
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);
void Uart4_Process(void);
void Delay_ms(unsigned int ms);
void Delay1000ms();
void Delay200ms();

struct Re_FindCard{
unsigned char Seccess_tag[8];
unsigned char Failure_tag[4];
};

struct Re_SelectCard{
unsigned char Seccess_tag[12];
unsigned char Failure_tag[4];
};

struct Re_ReadCard{
unsigned char Seccess_tag[3];
unsigned char Failure_tag[4];
};

struct re_data{
unsigned char header[5];
struct Re_FindCard Re_FindCard_Data;
struct Re_SelectCard Re_SelectCard_Data;
struct Re_ReadCard Re_ReadCard_Data;
}Re_Data={
{0xaa,0xaa,0xaa,0x96,0x69},
{{0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x97},{0x00, 0x00, 0x80, 0x84}},
{{0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C},{0x00, 0x00, 0x81, 0x85}},
{{0x00, 0x00, 0x90},{0x00, 0x00, 0x41, 0x45}}
};

unsigned char xdata re_data_buffer[1295];
unsigned  int i=0,Re_num=0;
unsigned char const code FIND_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x01, 0x22};
unsigned char const code SELECT_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x02, 0x21};
unsigned char const code READ_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x30, 0x01, 0x32};
//unsigned char const code TestData[10] = {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
//unsigned char OpCart_FindCart(void);
//unsigned char OpCart_SelectCart(void);
//unsigned char OpCart_ReadCart(void);
bit Uart4busy;
bit Uart1busy;


void main(void)
	{
		unsigned int n,Cnt,ID;
		init_Uart();
		IO_Init();
		for (n=0;n<10;n++){
			Uart4SendData(FIND_CARD[n]);  
		}
				//Uart4_Process();
		Delay200ms();			//�ȴ���������
		Re_num=0;
		for (n=0;n<10;n++){
			Uart4SendData(SELECT_CARD[n]);  
		}
				//Uart4_Process();	
		Delay200ms();			//�ȴ���������
		Re_num=0;
		for (n=0;n<10;n++){
			Uart4SendData(READ_CARD[n]);  
		}	
		Delay1000ms();		//�ȴ��ϵ��ȶ�(��������)
		Delay1000ms();
		ID=sFLASH_ReadID();							//��ȡW25Q64������ID��																		
		if(ID!=sFLASH_W25Q64_ID)				//���ID�Ų�ͬ
		{
			Uart1SendString("ID ERROR!");
			while(1);	
		}

	Uart1SendString("Start Read Write Test!\n");
	Uart1SendString("Erase!\n");
	Delay_ms(100);
	//д֮ǰ���Ȳ���������һ������
	sFLASH_EraseSector(FLASH_SectorToErase);
	Uart1SendString("Write!\n");
	Delay_ms(300);
	//д��һҳ������
	sFLASH_WritePage(re_data_buffer,FLASH_WriteAddress,Re_num);
	Uart1SendString("Read!\n");
	Delay_ms(300);
	//����ҳ�����ݶ�
	sFLASH_ReadBuffer(re_data_buffer,FLASH_ReadAddress,Re_num);
	//�Ƚ�д��Ͷ�ȡ�������Ƿ�һ��
	for(Cnt=0;Cnt<Re_num;Cnt++) 
	{
		Uart1SendData(re_data_buffer[Cnt]);
		//��������ݲ�ͬ
		/*if(TestData[Cnt]!=re_data_buffer[Cnt])
		{			
			//ToDisplayError(ERR2);//��ʾ������Ϣ
			Uart1SendString("Read Error!");
			while(1);			
		}	*/
	}	
		Uart1SendString("Success!");
		while(1);
			//Uart4_Process();		
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
	unsigned char dat;
	EA=0;
	dat = S4BUF;
	    if (S4CON & S4RI)
    {
			if(Re_num<1295){			
			re_data_buffer[Re_num++]=dat;
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
	unsigned char j;
			//for(;j<5;j++)
			//while (re_data_buffer[j] != Re_Data.header[j])	//�жϽ�������ͷ��5���ֽ�
				//Uart1SendString("Received Data Error!");
			//Uart1SendData(0x01);
			//j=0;																						//����ָ�����
				if (re_data_buffer[5]==0x00){
					
						if (re_data_buffer[6] == 0x04){
									unsigned char a=7,b=0;
									unsigned char temp[3]={0,0,0};
										for(;a<11,b<4;a++,b++){
										if(re_data_buffer[a] == Re_Data.Re_FindCard_Data.Failure_tag[b])
											temp[0]++;
										if(re_data_buffer[a] == Re_Data.Re_SelectCard_Data.Failure_tag[b])
											temp[1]++;
										if(re_data_buffer[a] == Re_Data.Re_ReadCard_Data.Failure_tag[b])
											temp[2]++;
							}
							if(temp[0] == 4){
								//Uart1SendString("FindCard!\n");
							for(j=0;j<11;j++);
							//Uart1SendData(re_data_buffer[j]);			
							}
							if(temp[1] == 4){
								//Uart1SendString("SelectCard!\n");
							for(j=0;j<11;j++);
							//Uart1SendData(re_data_buffer[j]);
							}
							if(temp[2] == 4)
								//Uart1SendString("ReadCard!\n");
							for(j=0;j<11;j++);
							//Uart1SendData(re_data_buffer[j]);
							}
						else if (re_data_buffer[6]==0x08)
						for(j=0;j<15;j++);
							//Uart1SendData(re_data_buffer[j]);
						else if (re_data_buffer[6]==0x0c)
							for(j=0;j<19;j++);
							//Uart1SendData(re_data_buffer[j]);
						}
				else if(re_data_buffer[5]==0x05){
						//Uart1SendString("ID INFO START!\n");
					for(j=0;j<300;j++)
					Uart1SendData(re_data_buffer[j]);
				}
				i=0;
}
/*----------------------------------
16����ת������

-----------------------------------*/
unsigned char htd(unsigned char a){
unsigned char b,c;
	b=a%10;
	c=b;
	a=a/10;
	b=a%10;
	c=c|b>>4;
	return c;
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
/*****************************************
���֤����Ѱ��												
��ڲ���:��												  
���ڲ���:��												   
*****************************************/
/*
unsigned char OpCart_FindCard(void)
	{
			unsigned char const FIND_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x01, 0x22};
			unsigned char Response[15];
			unsigned char Res_i=0;
			unsigned char OpCart_FindCard_i=0;
			while(OpCart_FindCard_i<10){
			Uart4SendData(FIND_CARD[OpCart_FindCard_i]);
			OpCart_FindCard_i++;
		}
}*/
/*****************************************
���֤����ѡ��												 
�����ܣ����֤ѡ������							
��ڲ���:��												   
���ڲ���:��												    
*******************************************/
/*
unsigned char OpCart_SelectCard(void){
	    unsigned char SELECT_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x02, 0x21};
			unsigned char SELECT_CARD_RESPONSE[15]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x0c, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x00, 0x00, 0x97};
			unsigned char Response[15];
			unsigned char Res_i=0;
			unsigned char OpCart_SelectCard_i=0;
			while(OpCart_SelectCard_i<10){
			Uart4SendData(SELECT_CARD[OpCart_SelectCard_i++]);//��������
		}
			while(!(S4CON&0x01));
			Response[Res_i++]=S4BUF;													//����ģ������ݲ�����
		  if(SELECT_CARD_RESPONSE[Res_i]==Response[Res_i])
			S4CON&=~0x01;
}*/
/*****************************************
���֤��������													 
�����ܣ����֤��������							
��ڲ���:��												   
���ڲ���:��												  
*****************************************/
/*unsigned char OpCart_ReadCard(void){
	
			unsigned char READ_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x30, 0x01, 0x32};
			unsigned char OpCart_ReadCard_i=0;
			while(OpCart_ReadCard_i<10){
			Uart4SendData(READ_CARD[OpCart_ReadCard_i]);
			OpCart_ReadCard_i++;
		}
}*/
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
	
	//P10 P14��λ��Ϊǿ���������ǿ������
	//��ֹ�����������¶˿��𻵣���ʼ��Ϊ׼˫��� 
	//��P10 P14����Ϊ׼˫���
	//P1M1 &=~( (1<<0) | (1<<4) );  
	//P1M0 &=~( (1<<0) | (1<<4) );     
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
