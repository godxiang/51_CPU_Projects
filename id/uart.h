#ifndef __UART_H__
#define	__UART_H__
#include "intrins.h"
typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 22118400L          //系统频率
#define BAUD 115200             //串口波特率

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位

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

void InitUart();
void Uart1SendData(BYTE dat);
void Uart1SendString(char *s);
void Uart3SendData(BYTE dat);
void Uart3SendString(char *s);
void Uart4SendData(BYTE dat);
void Uart4SendString(char *s);
void Delay1000ms();
void Delay200ms();
