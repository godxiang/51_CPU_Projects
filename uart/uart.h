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

void init_Uart();
void Uart1SendData(unsigned char dat);
void Uart1SendString(char *s);
void Uart3SendData( unsigned char dat);
void Uart3SendString(char *s);
void Uart4SendData(unsigned char Udat);
void Uart4SendString(char *s);

#endif
