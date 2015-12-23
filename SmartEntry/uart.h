#ifndef __UART_H__
#define	__UART_H__

#include "stc15w4k32s4.h"

#define FOSC 22118400L          //时钟频率
#define BAUD 115200             //波特率
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4_S0 0x04              //P_SW2.2
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7


extern unsigned char xdata re_data_buffer[1295];	//接收缓冲区
extern unsigned char S3_Re_Buffer[100];					
extern unsigned int Re_Cnt;									//接收数据计数器
extern unsigned int Rece_Cnt;								//接收数据计数器

extern bit Uart3HaveData;
extern bit Uart1busy;
extern  bit Uart3busy;
extern bit Uart4busy;

void init_Uart();
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart3SendData( unsigned char dat);
void Uart3SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);
void USART_Send(unsigned char *dat,unsigned char len);
void USART3_Send(unsigned char *dat,unsigned char len);
void Delay_ms(unsigned int ms);

#endif
