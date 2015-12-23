#include "LD9900MT.h"
#include "uart.h"

ZA_8CONST GETIMAGE_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};
ZA_8CONST GENCHAR_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02};
ZA_8CONST CREATET_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09};
ZA_8CONST UPCHAR_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x08};
ZA_8CONST DOWNCHAR_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x09};
ZA_8CONST STORE_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06};
ZA_8CONST LOADCHAR_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x07};
ZA_8CONST DELETECHAR_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C};
ZA_8CONST EMPTY_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};
ZA_8CONST MATCH_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x03,0x00,0x07};
ZA_8CONST SEARCH_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04};
ZA_8CONST READTN_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21};
ZA_8CONST SETSYSP_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x05,0x0E};
ZA_8CONST GETSYSP_CMD[]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0F,0x00,0x13};


//下面函数由外部提供,下面是51单片机的实现，与硬件和外部有关函数的实现

ZA_8U LD9900MT_ReceiveBuff[32]={0};
void LD9900MT_DelayMs(ZA_16U u16Count)
{
Delay_ms(u16Count);
}
/*
void LD9900MT_PowerOn(void)
{
LD9900MT_POWER_PIN=1;
}
void LD9900MT_PowrDown(void)
{
LD9900MT_POWER_PIN=0;
}
*/

ZA_8U u8RevcLen=0;
/*
void UART1_Receive(void)  interrupt 4 using 1
{	
unsigned char dat;
EA = 0;
if(RI)
{	
dat = SBUF;
if(u8RevcLen<32)
{
	LD9900MT_ReceiveBuff[u8RevcLen]=dat;
	u8RevcLen++;
}

else
{
	u8RevcLen=0;
}
		RI = 0;
}

if (TI)
{
	TI = 0;
	Uart1busy=0;
}

EA = 1;						
}
*/

void UART4_Receive(void) interrupt 18 using 1
{	
unsigned char dat;
EA = 0;
if(S4CON & S4RI)
{	
dat = S4BUF;
if(u8RevcLen<32)
{
	LD9900MT_ReceiveBuff[u8RevcLen]=dat;
	u8RevcLen++;
}

else
{
	u8RevcLen=0;
}
		S4CON &= ~S4RI;
}

if (S4CON & S4TI)
{
	S4CON&= ~S4TI;
	Uart4busy=0;
}

EA = 1;						
}


ZA_8U LD9900MT_Receive(void)
{
ZA_8U len=0;
//ZA_16U iTimeOut=0;
//while(true)
//{
//	iTimeOut++;
//	if(50000==iTimeOut)
//	{
//		#ifdef __C51__
//		WatchDog();
//		#endif
//		break;
//	}
//}	
len=u8RevcLen;
u8RevcLen=0;
ET0=1;
return len;
}


void LD9900MT_Send(ZA_8U *u8Data,ZA_8U len)
{
USART_Send(u8Data,len);
ET0=0;//close extern interrupt 0
}


//与硬件有关外部提供的函数到此结束


static const ZA_16U CMD_AddSum(ZA_8U *u8CmdData,ZA_8U len)
{
ZA_8U i=0;
ZA_16U res=0;
for(i=6;i<len;i++)
{
res +=u8CmdData[i];
}
return res;
}



static ZALD_Res LD9900MT_CMDPorcess(ZA_8U *u8CMD,ZA_16U u16CMDLen,ZA_8U *u8AttachData,ZA_16U u16ADLen)
{
ZA_8U i=0;
for(i=0;i<32;i++)
{
LD9900MT_ReceiveBuff[i]=0;
}
LD9900MT_Send(u8CMD,u16CMDLen);

if(0!=u16ADLen)
{
LD9900MT_Send(u8AttachData,u16ADLen);
}
LD9900MT_Receive();
Delay_ms(2000);										//等待接收数据
if((0xEF==LD9900MT_ReceiveBuff[0]) && (0x01==LD9900MT_ReceiveBuff[1]))
{
return LD9900MT_ReceiveBuff[9];
}
else
{
return LD_NODEVICE;
}
}


ZALD_Res LD9900MT_Init(void)
{
//LD9900MT_PowerOn();
LD9900MT_DelayMs(600); //at least 500ms
//send the GETIMAGE_CM test the LD9900MT module is work normal
return LD9900MT_CMDPorcess(GETIMAGE_CMD,sizeof(GETIMAGE_CMD),(ZA_8U*)0,0);
}

ZALD_Res LD9900MT_DeInit(void)
{
//LD9900MT_PowrDown();
return LD_OK;
}

ZALD_Res LD9900MT_GetImage(void)
{
return LD9900MT_CMDPorcess(GETIMAGE_CMD,sizeof(GETIMAGE_CMD),(ZA_8U*)0,0);
}

ZALD_Res LD9900MT_GenChar(ZA_8U u8BufferID)
{
ZA_8U u8AttachData[3]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8BufferID;
u16Sum=CMD_AddSum(GENCHAR_CMD,sizeof(GENCHAR_CMD))+u8AttachData[0];
u8AttachData[1]=(ZA_8U)(u16Sum>>8);
u8AttachData[2]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(GENCHAR_CMD,sizeof(GENCHAR_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_CreateTemplate(void)
{
return LD9900MT_CMDPorcess(CREATET_CMD,sizeof(CREATET_CMD),(ZA_8U*)0,0);
}

ZALD_Res LD9900MT_UpChar(ZA_8U u8BufferID)
{
ZA_8U u8AttachData[3]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8BufferID;
u16Sum=CMD_AddSum(GENCHAR_CMD,sizeof(GETIMAGE_CMD))+u8AttachData[0];
u8AttachData[1]=(ZA_8U)(u16Sum>>8);
u8AttachData[2]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(GENCHAR_CMD,sizeof(GENCHAR_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_DwonChar(ZA_8U u8BufferID)
{
ZA_8U u8AttachData[3]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8BufferID;
u16Sum=CMD_AddSum(DOWNCHAR_CMD,sizeof(DOWNCHAR_CMD))+u8AttachData[0];
u8AttachData[1]=(ZA_8U)(u16Sum>>8);
u8AttachData[2]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(DOWNCHAR_CMD,sizeof(DOWNCHAR_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_Store(ZA_8U u8BufferID ,ZA_16U u16PageID)
{
ZA_8U u8AttachData[5]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8BufferID;
u8AttachData[1]=(ZA_8U)(u16PageID>>8);
u8AttachData[2]=(ZA_8U)u16PageID;
u16Sum=CMD_AddSum(STORE_CMD,sizeof(STORE_CMD))+u8AttachData[0]+u8AttachData[1]+u8AttachData[2];
u8AttachData[3]=(ZA_8U)(u16Sum>>8);
u8AttachData[4]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(STORE_CMD,sizeof(STORE_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_LoadChar(ZA_8U u8BufferID ,ZA_16U u16PageID){
ZA_8U u8AttachData[5]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8BufferID;
u8AttachData[1]=(ZA_8U)(u16PageID>>8);
u8AttachData[2]=(ZA_8U)u16PageID;
u16Sum=CMD_AddSum(LOADCHAR_CMD,sizeof(LOADCHAR_CMD))+u8AttachData[0]+u8AttachData[1]+u8AttachData[2];
u8AttachData[3]=(ZA_8U)(u16Sum>>8);
u8AttachData[4]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(LOADCHAR_CMD,sizeof(LOADCHAR_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_DeleteChar(ZA_16U u16PageID ,ZA_16U u16DeleteNum)
{
ZA_8U u8AttachData[6]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=(ZA_8U)(u16PageID>>8);
u8AttachData[1]=(ZA_8U)u16PageID;
u8AttachData[2]=(ZA_8U)(u16DeleteNum>>8);
u8AttachData[3]=(ZA_8U)u16DeleteNum;
u16Sum=CMD_AddSum(DELETECHAR_CMD,sizeof(DELETECHAR_CMD))+u8AttachData[0]+u8AttachData[1]+u8AttachData[2]+u8AttachData[3];
u8AttachData[3]=(ZA_8U)(u16Sum>>8);
u8AttachData[4]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(DELETECHAR_CMD,sizeof(DELETECHAR_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_Empty(void)
{
return LD9900MT_CMDPorcess(EMPTY_CMD,sizeof(EMPTY_CMD),(ZA_8U *)0,0);
}

ZALD_Res LD9900MT_Match(ZA_16U *u16MatchScore)
{
ZALD_Res res=LD_OK;
res=LD9900MT_CMDPorcess(MATCH_CMD,sizeof(MATCH_CMD),(ZA_8U *)0,0);
if(LD_OK==res)
{
*u16MatchScore=(ZA_16U)(LD9900MT_ReceiveBuff[10]<<8)+LD9900MT_ReceiveBuff[11];
}
return res;
}

ZALD_Res LD9900MT_Search(ZA_8U u8BufferID ,ZA_16U u16StartPage ,ZA_16U u16PageNum)
{
ZA_8U u8AttachData[7]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8BufferID;
u8AttachData[1]=(ZA_8U)(u16StartPage>>8);
u8AttachData[2]=(ZA_8U)u16StartPage;
u8AttachData[3]=(ZA_8U)(u16PageNum>>8);
u8AttachData[4]=(ZA_8U)u16PageNum;
u16Sum=CMD_AddSum(SEARCH_CMD,sizeof(SEARCH_CMD))+u8AttachData[0]+u8AttachData[1]+u8AttachData[2]
																											+u8AttachData[3]+u8AttachData[4];
u8AttachData[5]=(ZA_8U)(u16Sum>>8);
u8AttachData[6]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(SEARCH_CMD,sizeof(SEARCH_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_TemplateNum(ZA_16U *u16TeNum)
{
ZALD_Res res=LD_OK;
res=LD9900MT_CMDPorcess(READTN_CMD,sizeof(READTN_CMD),(ZA_8U *)0,0);
if(LD_OK==res)
{
*u16TeNum=(ZA_16U)(LD9900MT_ReceiveBuff[10]<<8)+LD9900MT_ReceiveBuff[11];
}
return res;
}

ZALD_Res LD9900MT_SetSysParm(ZA_8U u8ParmNum, ZA_8U u8Data)
{
ZA_8U u8AttachData[4]={0};
ZA_16U u16Sum=0;
u8AttachData[0]=u8ParmNum;
u8AttachData[1]=u8Data;
u16Sum=CMD_AddSum(SETSYSP_CMD,sizeof(SETSYSP_CMD))+u8AttachData[0]+u8AttachData[1];
u8AttachData[3]=(ZA_8U)(u16Sum>>8);
u8AttachData[4]=(ZA_8U)u16Sum;
return LD9900MT_CMDPorcess(SETSYSP_CMD,sizeof(SETSYSP_CMD),u8AttachData,sizeof(u8AttachData));
}

ZALD_Res LD9900MT_GetSysParm(ZA_8U *u8SysParm)
{
ZALD_Res res=LD_OK;
ZA_8U i=0;
res=LD9900MT_CMDPorcess(GETSYSP_CMD,sizeof(GETSYSP_CMD),(ZA_8U *)0,0);
if(LD_OK==res)
{
for(i=0;i<16;i++)
{
	u8SysParm[i]=LD9900MT_ReceiveBuff[10+i];
}
}
return res;
}


