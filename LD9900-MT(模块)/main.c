#include"main.h"
#include "LD9900MT.h"




void main(void)
{
	ZA_16U *u16MatchScore;
	init_Uart();	
	Delay_ms(1000);
	if(LD9900MT_Init() == 0x00){
		//Uart3SendString("Init Seccess!\n");
	}
while(1){
	Delay_ms(1000);
	//Uart3SendString("First!");
	while(LD9900MT_GetImage() != 0x00){												//获取指纹图像成功
			Delay_ms(200);
	}
		Uart3SendData(LD9900MT_GenChar(0x01));									//生成模板储存在 CharBuffer 1				
		Uart3SendData(LD9900MT_UpChar(0x01));
	//Uart3SendData(LD9900MT_Store(0x01,0x0001));
	//Uart3SendData(LD9900MT_LoadChar(0x01,0x0001));
	Delay_ms(1000);
	//Uart3SendString("First!");
	
	while(LD9900MT_GetImage() != 0x00){												//获取指纹图像成功
			Delay_ms(200);
	}
		Uart3SendData(LD9900MT_GenChar(0x02));									//生成模板储存在 CharBuffer 1				

	Uart3SendData(LD9900MT_CreateTemplate());
	
	
	//Uart3SendData(LD9900MT_Store(0x02,0x0002));
	//Uart3SendData(LD9900MT_LoadChar(0x02,0x0001));
	Uart3SendData(LD9900MT_Match(u16MatchScore));
	Uart3SendData((ZA_8U)*u16MatchScore);
	Uart3SendData(((ZA_8U)*u16MatchScore<<8));


	//Uart3SendString("Second!");
//	if(LD9900MT_GetImage() == 0x00){												//获取指纹图像成功
//			Delay_ms(200);
//		Uart3SendData(LD9900MT_GenChar(0x02));									//生成模板储存在 CharBuffer 2				
//	}
	//Uart3SendData(LD9900MT_CreateTemplate());								//生成模板
}
	//Uart1SendString("test!\n");
	//Uart3SendString("test!\n");	
	//Uart4SendString("test!\n");
}