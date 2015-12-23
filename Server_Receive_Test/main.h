#include "stc15w4k32s4.h"
#include "STC15W4K32S4.h"
#include <intrins.h>


#define FOSC 22118400L          //时钟频率
#define BAUD 115200             //波特率
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4_S0 0x04              //P_SW2.2
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

 
#define Hex2Int(x)		( (x/16)*10+(x%16) )				//十六进制转换成十进制


void init_Uart();
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);
void Delay_ms(unsigned int ms);
void Delay1000ms();
void Delay2000ms();
void Delay200ms();
void UART4_Proccess(void);
unsigned char xdata Re_Buffer[100];						//接收缓冲区
unsigned int Re_Cnt = 0;											//接收计数器
unsigned int length;													//保存数据长度
unsigned char LocalHostNum[10]={0x00, 0x00, 0x00, 0x00, 0x03, 0x71, 0x00, 0x01, 0x00, 0x01};
unsigned char MessageType[3][2]={{0x00,0x01}, {0x00,0x02}, {0x00,0x03}};							//消息类型
unsigned char MessType;
bit Uart4busy;
bit Uart1busy;

