#include"main.h"




void main(void)
{
		init_Uart();	
		Uart1SendString("test!\n");
		Uart3SendString("test!\n");	
		Uart4SendString("test!\n");
}