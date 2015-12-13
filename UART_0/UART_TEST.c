
//�����ļ�����
//#include "reg51.h"
//#include <stc.h>
#include "stc15w4k32s4.h"
#include "intrins.h"
#include "string.h"
//������س�������
#define  FOSC              22118400L   //ϵͳƵ��11059200L 22118400L
#define  Uart1_MaxString   32          //����1�����ջ������ַ�����(0-31)��32���ַ�
#define  Uart2_MaxString   32          //����2�����ջ������ַ�����(0-31)��32���ַ�
//������ر�������
char           Uart1_Data[Uart1_MaxString];   //����1�����ִ�
unsigned char  Uart1_String_P;                //����1�����ִ�λ��ָ��
bit            Uart1_busy;                    //����1���ڷ��ͱ��,1�����ڷ��ͣ�0���������
unsigned char  Uart1_ParitySet;               //����1��żУ�鷽ʽ 0��У�� 1��У�� 2żУ�� 3ʼ��1 4ʼ��0
bit            Uart1_Parity;                  //����1���յ�����żУ��״̬ 1,0
bit            Uart1_Parity_out;              //����1��֤���յ�����żУ��״̬ 1,��֤,0��֤����
char           Uart2_Data[Uart1_MaxString];   //����2�����ִ�
unsigned char  Uart2_String_P;                //����2�����ִ�λ��ָ��
bit            Uart2_busy;                    //����2���ڷ��ͱ��,1�����ڷ��ͣ�0���������
unsigned char  Uart2_ParitySet;               //����2��żУ�鷽ʽ 0��У�� 1��У�� 2żУ�� 3ʼ��1 4ʼ��0
bit            Uart2_Parity;                  //����2���յ�����żУ��״̬ 1,0
bit            Uart2_Parity_out;              //����2��֤���յ�����żУ��״̬ 1,��֤,0��֤����
//Uart1������������
void Uart1_Setup(unsigned long Uart1_Baud,bit Uart1_Tx); //����1�����ʺ�ʱ��Դ���ú���
void Uart1_SendData(unsigned char Uart1_dat);                  //����1���ַ����ͺ���
void Uart1_SendString(char *s);                          //����1�ַ������ͺ���
//void Uart1_Isr() interrupt 4 using 1;                    //����1�жϷ�����
void Uart2_Setup(unsigned long Uart2_Baud);              //����2������
void Uart2_SendData(unsigned char Uart2_dat);                  //����2���ַ����ͺ���
void Uart2_SendString(char *s);                          //����2�ַ������ͺ���
//void Uart1_Isr() interrupt 4 using 1;                    //����2�жϷ�����
/*���ڱ��ʾ��============================================================================
Uart1_String_P=0;                //����1�����ִ��±��0
Uart1_ParitySet=1;               //����1ʹ��9λ�ɱ䲨���ʷ�ʽ��У�鷽ʽΪ��У��
Uart1_Setup(9600,0);             //����1������Ϊ9600bps��ʱ��Դѡ��ʱ��1
Uart1_SendString("hello word");  //����1�������ݣ�Ӣ����ĸ hello word
Uart2_String_P=0;                //����2�����ִ��±��0
Uart2_ParitySet=1;               //����2ʹ��9λ�ɱ䲨���ʷ�ʽ��У�鷽ʽΪ��У��
Uart2_Setup(9600);               //����2������Ϊ9600bps
Uart2_SendString("hello word");  //����2�������ݣ�Ӣ����ĸ hello word
=======================================================================================*/
/*********************************************************
** ��������: void Uart1_Setup(unsigned long Uart1_Baud,bit Uart1_Tx) 
** ��������: ��ʼ�����ں���
** �䡡  ��: unsigned long Uart1_Baud,bit Uart1_Tx
** �䡡  ��: ��
** ȫ�ֱ���: PCON,SCON,TMOD,AUXR,TCON,TH1,
** ����ģ��: 
** ��    ע:ʱ��Ϊ22118400Hz
            ����1:����������,300;600;1200;2400;4800;9600;19200
            ����2:������ѡ��,0,��ʱ��1;1,����������
**********************************************************/
void Uart1_Setup(unsigned long Uart1_Baud,bit Uart1_Tx) 
{
unsigned int Uart1_Brt;
//ʱ��Դ����-------------------------------------------------
if(Uart1_Tx)
    {
AUXR |= 0x01;  //����1ѡ����������ʷ�����
}
else
    {
AUXR &= 0xFE;  //����1ѡ��ʱ��1�������ʷ�����
}
//У�鷽ʽ����-------------------------------------------------
switch (Uart1_ParitySet)
    {//�ж�У�鷽ʽ
    case 1:
        {//9λ����, ��У�� Uart1_ParitySet=1;   //У��λ��ֵ״̬�����1
        SCON=0xD8;           //9λ�����ʿɱ�,��������,TB8������9λ��ʼ��1
        break;
        }
    case 2:
        {//9λ����, żУ�� Uart1_ParitySet=2;   //У��λ��ֵ״̬�����2
        SCON=0xD4;           //9λ�����ʿɱ�,��������,RB8���յ�9λ��ʼ��1
        break;
        }
    case 3:
        {//9λ����, ����У�� У��ֵʼ��Ϊ 1 Uart1_ParitySet=3;   //У��λ��ֵ״̬�����3
        SCON|=0xD8;         //9λ�����ʿɱ�,��������,TB8������9λ��ʼ��1
        break;
        }
    case 4:
        {//9λ����, ��У�� У��ֵʼ��Ϊ 0 Uart1_ParitySet=4;   //У��λ��ֵ״̬�����4
        SCON=0xD4;          //9λ�����ʿɱ�,��������,RB8���յ�9λ��ʼ��1
        break;
        }
    default:
        {//8λ����, ��У�� Uart1_ParitySet=0;   //У��λ��ֵ״̬�����0
        SCON&=0x5F;         //�����5λ,��7λ
        SCON|=0x50;         //8λ�����ʿɱ�,��������
        break;
        }
    }
//���������ò���ʼ���д���-------------------------------------------------
//ʱ��Ϊ22118400Hz
Uart1_Brt=FOSC/32/Uart1_Baud;
if(Uart1_Brt>255)
    {//�����װֵ>255,ʹ��12Tģʽ12��Ƶ
    if(Uart1_Tx)
        {//���ʹ�ö��������ʷ�����
        //PCON |= 0x80;                                 //����1�����ʼӱ�
        //PCON &= 0x7f;                                 //����1�����ʲ��ӱ�
        AUXR &= 0xFB;                                 //����������ʹ��12Tģʽ
        //AUXR |= 0x04;                                 //����������ʹ��1Tģʽ
        BRT = 256-Uart1_Brt/12;                       //���������ʷ�������ֵ����
        AUXR|= 0x10;                                  //���������ʷ�������ʼ����
        }
    else
        {//���ʹ�ö�ʱ��1�������ʷ�����
        TMOD&=0xCF;                                 //�����ʱ��1����״̬����ʱ��0���޸�
        TMOD|=0x20;                                 //��ʱ��1ʹ��8λ�Զ���װģʽ
        //PCON |= 0x80;                               //����1�����ʼӱ�
        //PCON &= 0x7f;                               //����1�����ʲ��ӱ�
        AUXR &= 0xBF;                               //��ʱ��1ʹ��12Tģʽ
        //AUXR|=0x40;                                 //��ʱ��1ʹ��1Tģʽ
        TH1  = TL1 = 256-Uart1_Brt/12;              //�����Զ�װ���ֵ
        ET1  = 0;                                   //�رն�ʱ��1�ж�
        TR1  = 1;                                   //TCON|=0x40;��ʱ��1��ʼ����,TCON&=0xBF;�رռ���
        }
    }
else
    {//�����װֵ<255,ʹ��1Tģʽ1��Ƶ
    if(Uart1_Tx)
        {//���ʹ�ö��������ʷ�����
        //PCON |= 0x80;                                 //����1�����ʼӱ�
        //PCON &= 0x7f;                                 //����1�����ʲ��ӱ�
        //AUXR &= 0xFB;                                 //����������ʹ��12Tģʽ
        AUXR |= 0x04;                                 //����������ʹ��1Tģʽ
        BRT  = 256-Uart1_Brt;                         //���������ʷ�������ֵ����
        AUXR|= 0x10;                                  //���������ʷ�������ʼ����
        }
    else
        {//���ʹ�ö�ʱ��1�������ʷ�����
        TMOD&=0xCF;                                 //�����ʱ��1����״̬����ʱ��0���޸�
        TMOD|=0x20;                                 //��ʱ��1ʹ��8λ�Զ���װģʽ
        //PCON |= 0x80;                               //����1�����ʼӱ�
        //PCON &= 0x7f;                               //����1�����ʲ��ӱ�
        //AUXR &= 0xBF;                               //��ʱ��1ʹ��12Tģʽ
        AUXR|=0x40;                                 //��ʱ��1ʹ��1Tģʽ
        TH1  = TL1 = 256-Uart1_Brt;                 //�����Զ�װ���ֵ
        ET1  = 0;                                   //�رն�ʱ��1�ж�
        TR1  = 1;                                   //TCON|=0x40;��ʱ��1��ʼ����,TCON&=0xBF;�رռ���
        }
    }
    ES        = 1;                                  //�򿪴����ж�
    EA        = 1;                                  //�����ж�
}
/*********************************************************
** ��������: void Uart2_Setup(unsigned long Uart2_Baud) 
** ��������: ��ʼ�����ں���
** �䡡  ��: unsigned long Uart1_Baud
** �䡡  ��: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע:ʱ��Ϊ22118400Hz
            ����1:����������,300;600;1200;2400;4800;9600;19200
**********************************************************/
void Uart2_Setup(unsigned long Uart2_Baud) 
{
unsigned int Uart2_Brt;
//У�鷽ʽ����-------------------------------------------------
switch (Uart2_ParitySet)
    {//�ж�У�鷽ʽ
    case 1:
        {//9λ����, ��У�� Uart1_ParitySet=1;   //У��λ��ֵ״̬�����1
        S2CON=0xD8;           //9λ�����ʿɱ�,��������,TB8������9λ��ʼ��1
        break;
        }
    case 2:
        {//9λ����, żУ�� Uart1_ParitySet=2;   //У��λ��ֵ״̬�����2
        S2CON=0xD4;           //9λ�����ʿɱ�,��������,RB8���յ�9λ��ʼ��1
        break;
        }
    case 3:
        {//9λ����, ����У�� У��ֵʼ��Ϊ 1 Uart1_ParitySet=3;   //У��λ��ֵ״̬�����3
        S2CON|=0xD8;         //9λ�����ʿɱ�,��������,TB8������9λ��ʼ��1
        break;
        }
    case 4:
        {//9λ����, ��У�� У��ֵʼ��Ϊ 0 Uart1_ParitySet=4;   //У��λ��ֵ״̬�����4
        S2CON=0xD4;          //9λ�����ʿɱ�,��������,RB8���յ�9λ��ʼ��1
        break;
        }
    default:
        {//8λ����, ��У�� Uart1_ParitySet=0;   //У��λ��ֵ״̬�����0
        S2CON&=0x5F;         //�����5λ,��7λ
        S2CON|=0x50;         //8λ�����ʿɱ�,��������
        break;
        }
    }
//���������ò���ʼ���д���-------------------------------------------------
//ʱ��Ϊ22118400Hz
Uart2_Brt=FOSC/32/Uart2_Baud;
if(Uart2_Brt>255)
    {//�����װֵ>255,ʹ��12Tģʽ12��Ƶ
        //PCON |= 0x80;                                 //����1�����ʼӱ�
        //PCON &= 0x7f;                                 //����1�����ʲ��ӱ�
        AUXR &= 0xFB;                                 //����������ʹ��12Tģʽ
        //AUXR |= 0x04;                                 //����������ʹ��1Tģʽ
        BRT = 256-Uart2_Brt/12;                       //���������ʷ�������ֵ����
    }
else
    {//�����װֵ<255,ʹ��1Tģʽ1��Ƶ
        //PCON |= 0x80;                                 //����1�����ʼӱ�
        //PCON &= 0x7f;                                 //����1�����ʲ��ӱ�
        //AUXR &= 0xFB;                                 //����������ʹ��12Tģʽ
        AUXR |= 0x04;                                 //����������ʹ��1Tģʽ
        BRT  = 256-Uart2_Brt;                         //���������ʷ�������ֵ����
    }
    AUXR|= 0x10;                                  //���������ʷ�������ʼ����
IE2|=0x01;                                    ////�򿪴���2�ж�
    EA   = 1;                                     //�����ж�
}
//=======================================================================================
/**********************************************************
����1�жϷ�����
**********************************************************/
void Uart1_Isr() interrupt 4 using 1
{// UART �жϷ�����
EA=0; //�����ж�
if (RI)
{//��� UART ���ղ����ж�
    RI = 0;                                           //��� UART �����жϱ��
    ACC= SBUF;                                        //���ջ���������д���ۼ���A
    Uart1_Parity = P;                                 //����У��ֵ��Uart1_Parity
    Uart1_Parity_out=0;                               //�������д���ǣ�Ϊ�����ж�׼��
    switch(Uart1_ParitySet)
    {//����У��Ҫ���ж�
        case 1:
        {//Ҫ����У��
        Uart1_Parity_out=Uart1_Parity^RB8;    //����������ͽ��ս�������������������д��
        break;
        }
        case 2:
        {//Ҫ��żУ��
        Uart1_Parity_out=~(Uart1_Parity^RB8); //����������ͽ��ս�����������������д��
        break;
        }
        case 3:
        {//Ҫ��1У��
        Uart1_Parity_out=RB8;                 //������ս��Ϊ1�����������д��
        break;
        }
        case 4:
        {//Ҫ��0У��
        Uart1_Parity_out=~RB8;                //������ս��Ϊ0�����������д��
        break;
        }
        default:
        {//Ҫ����У��
        Uart1_Parity_out=1;                  //�����У�飬����д��
        break;
        }
    }
    if(Uart1_Parity_out)
    {//�������д��
        Uart1_Data[Uart1_String_P]= SBUF;              //���ջ���������˳��д�봮�ڽ����ִ�
        Uart1_String_P++;                              //���ڽ����ִ�ָ���ۼ�
        //��������ڽ����ִ�ָ���ڴ��������ջ������ַ�����,���ڽ����ִ�ָ�����
        if(Uart1_String_P >= Uart1_MaxString) 
        {//��������ִ�ָ��Խ��
            Uart1_String_P=0;                          //�����ִ�ָ�����
        }
    }
}
if (TI)
{//��� UART ���Ͳ����ж�
    TI   = 0;                  //��� UART �����жϱ��
    Uart1_busy = 0;            //������ɣ�������ڷ��ͱ��
}
EA=1;                          //�����ж�
}
//=======================================================================================
/**********************************************************
����2�жϷ�����
**********************************************************/
void Uart2_Isr() interrupt 8 using 1
{// UART �жϷ�����
EA=0; //�����ж�
if (S2CON&0x01)
{//��� UART ���ղ����ж�
    S2CON &= 0xFE;                                     //��� UART �����жϱ��
    ACC= S2BUF;                                        //���ջ���������д���ۼ���A
    Uart2_Parity = P;                                  //����У��ֵ��Uart1_Parity
    Uart2_Parity_out=0;                                //�������д���ǣ�Ϊ�����ж�׼��
    switch(Uart2_ParitySet)
    {//����У��Ҫ���ж�
        case 1:
        {//Ҫ����У��
        Uart2_Parity_out=Uart2_Parity^(S2CON&0x04);    //����������ͽ��ս�������������������д�� (S2CON&0x04)==S2RB8
        break;
        }
        case 2:
        {//Ҫ��żУ��
        Uart2_Parity_out=~(Uart2_Parity^(S2CON&0x04)); //����������ͽ��ս�����������������д��
        break;
        }
        case 3:
        {//Ҫ��1У��
        Uart2_Parity_out=(S2CON&0x04);                 //������ս��Ϊ1�����������д��
        break;
        }
        case 4:
        {//Ҫ��0У��
        Uart2_Parity_out=~(S2CON&0x04);                //������ս��Ϊ0�����������д��
        break;
        }
        default:
        {//Ҫ����У��
        Uart2_Parity_out=1;                  //�����У�飬����д��
        break;
        }
    }
    if(Uart2_Parity_out)
    {//�������д��
        Uart2_Data[Uart2_String_P]= S2BUF;              //���ջ���������˳��д�봮�ڽ����ִ�
        Uart2_String_P++;                              //���ڽ����ִ�ָ���ۼ�
        //��������ڽ����ִ�ָ���ڴ��������ջ������ַ�����,���ڽ����ִ�ָ�����
        if(Uart2_String_P >= Uart2_MaxString) 
        {//��������ִ�ָ��Խ��
            Uart2_String_P=0;                          //�����ִ�ָ�����
        }
    }
}
if (S2CON&0x02)
{//��� UART ���Ͳ����ж�
    S2CON &= 0xFD;                  //��� UART �����жϱ��
    Uart2_busy = 0;            //������ɣ�������ڷ��ͱ��
}
EA=1;                          //�����ж�
}

//=======================================================================================
/**********************************************************
** ��������: void Uart1_SendData(unsigned char Uart1_dat)
** ��������: UART1 �����ַ����ͺ���
** �䡡  ��: unsigned char Uart1_dat
** �䡡  ��: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע:����1:�����͵ĵ����ַ�
**********************************************************/
void Uart1_SendData(unsigned char Uart1_dat)
{//UART �����ֽں���
    while (Uart1_busy);       //ѭ���ȴ����ڷ��ͱ�Ǳ����
    ACC = Uart1_dat;                //�������������ۼ���A,׼������żУ��λ P
    TB8 = P;                  //д��żУ����
    switch(Uart1_ParitySet)
    {//����У��Ҫ���ж�
        case 1:
        {//Ҫ����У��
            TB8=~TB8;        //��תTB8��ֵ
            break;
        }
        /*żУ�飬TB8����
        case 2:
        {//Ҫ��żУ��
            TB8=TB8;            //TB8����
            break;
        }
        */
        case 3:
        {//Ҫ��1У��
            TB8=1;            //TB8�����1
            break;
        }
        case 4:
        {//Ҫ��0У��
            TB8=0;            //TB8�����0
            break;
        }
        default:
        {//Ҫ����У��
        break;
        }
    }
    Uart1_busy = 1;          //���ڷ��ͱ���� 1
    SBUF = ACC;              //�ۼ��� A �е������ͷ��ͻ���������
}
//=======================================================================================
/**********************************************************
** ��������: void Uart2_SendData(unsigned char Uart2_dat)
** ��������: UART2 �����ַ����ͺ���
** �䡡  ��: unsigned char Uart2_dat
** �䡡  ��: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע:����1:�����͵ĵ����ַ�
**********************************************************/
void Uart2_SendData(unsigned char Uart2_dat)
{//UART �����ֽں���
    while (Uart2_busy);       //ѭ���ȴ����ڷ��ͱ�Ǳ����
    ACC = Uart2_dat;                //�������������ۼ���A,׼������żУ��λ P
    TB8 = P;                  //д��żУ����
    switch(Uart2_ParitySet)
    {//����У��Ҫ���ж�
        case 1:
        {//Ҫ����У��
            S2CON^=0x08;        //��תS2TB8��ֵ
            break;
        }
        /*żУ�飬S2TB8����
        case 2:
        {//Ҫ��żУ��
            S2TB8=1;            //S2TB8����
            break;
        }
        */
        case 3:
        {//Ҫ��1У��
            S2CON|=0x08;            //S2TB8�����1
            break;
        }
        case 4:
        {//Ҫ��0У��
            S2CON&=0xF7;            //TB8�����0
            break;
        }
        default:
        {//Ҫ����У��
        break;
        }
    }
    Uart2_busy = 1;          //���ڷ��ͱ���� 1
    S2BUF = ACC;              //�ۼ��� A �е������ͷ��ͻ���������
}
//=======================================================================================
/**********************************************************
** ��������: void Uart1_SendString(char *s)
** ��������: UART1 ���ַ������ͺ���
** �䡡  ��: char *s
** �䡡  ��: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע:����1:�����͵��ַ���
**********************************************************/
void Uart1_SendString(char *s)
{//UART ���ַ�������
    while (*s)
    {
        Uart1_SendData(*s++);   //�������ַ�������ʹ��SendData�������ֽڷ���
    }
}
//=======================================================================================
/**********************************************************
** ��������: void Uart2_SendString(char *s)
** ��������: UART2 ���ַ������ͺ���
** �䡡  ��: char *s
** �䡡  ��: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע:����1:�����͵��ַ���
**********************************************************/
void Uart2_SendString(char *s)
{//UART ���ַ�������
    while (*s)
    {
        Uart2_SendData(*s++);   //�������ַ�������ʹ��SendData�������ֽڷ���
    }
}
//=======================================================================================
/*���ô������ÿ���===============================��¼===================================
AUXR |= 0x01;  //����1ѡ����������ʷ�����
AUXR &= 0xFE;  //����1ѡ��ʱ��1�������ʷ�����
AUXR |= 0x40;  //��ʱ��1ʹ��1Tģʽ
AUXR &= 0xBF;  //��ʱ��1ʹ��12Tģʽ
AUXR |= 0x04;  //����������ʹ��1Tģʽ
AUXR &= 0xFB;  //����������ʹ��12Tģʽ
AUXR |= 0x10;  //�򿪶��������ʷ�����
AUXR &= 0xEF;  //�رն��������ʷ�����
PCON |= 0x80;  //����1�����ʼӱ�
PCON &= 0x7f;  //����1�����ʲ��ӱ�
AUXR |= 0x08;   //����2�����ʼӱ�
AUXR &= 0xf7;   //����2�����ʲ��ӱ�
=======================================================================================*/