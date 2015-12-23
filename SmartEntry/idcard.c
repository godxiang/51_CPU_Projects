#include "uart.h"
#include "w25q64.h"
#include "LD9900MT.h"


#define Uint unsigned int
#define Ulongint unsigned long int
#define Uchar unsigned char

#define Hex2Int(x)		( (x/10)*10+(x%10) )				//十六进制转换成十进制
unsigned char const code FIND_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x01, 0x22};
unsigned char const code SELECT_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x02, 0x21};
unsigned char const code READ_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x30, 0x01, 0x32};
ZA_8CONST HEARTBREAK[]={0x55, 0xaa, 0x00, 0x01, 0x03, 0x71, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x16};
unsigned char Id_Number[20]={0x00};						//保存身份证号码
unsigned int length = 0;								//保存数据长度
unsigned char  spi_re_data_buffer[20];	//flsh读取缓存数组
unsigned long int FLASH_Address;				//写入Flash的地址
unsigned char IdData[20]={0x00};
unsigned char Re_length;												//保存上位机数据长度
unsigned char LocalHostNum[10]={0x00, 0x00, 0x00, 0x00, 0x03, 0x71, 0x00, 0x01, 0x00, 0x01};
unsigned char const code MessageType[][2]={{0x00,0x01}, {0x00,0x02}, {0x00,0x03}, {0x10, 0x01}, {0x10, 0x03}, {0xf0, 0x01},
																				{0x30,0x01}, {0x31,0x03}, {0x32,0x03},{0x30,0x04}};	
//返回上位机的常量数据定义区
ZA_8CONST ID_DL_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x10,0xff,0x01,0x01,0x16};											//下载身份证成功
ZA_8CONST ID_DL_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x10,0xff,0x00,0x00,0x16};											//下载身份证失败
ZA_8CONST FIRST_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x31,0xff,0x01,0x01,0x16};											//第一次采集指纹成功
ZA_8CONST SECOND_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x32,0xff,0x01,0x01,0x16};											//第一次采集指纹失败
ZA_8CONST FIRST_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x31,0xff,0x00,0x00,0x16};											//第二次采集指纹成功
ZA_8CONST SECOND_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x32,0xff,0x00,0x00,0x16};											//第二次采集指纹失败
ZA_8CONST LD9900MT_CT_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x3f,0xff,0x01,0x01,0x16};								//	生成模板成功
ZA_8CONST LD9900MT_CT_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x3f,0xff,0x00,0x00,0x16};								//	生成模板失败
unsigned char MessType = 0xff;
bit Uart3HaveData;
bit OpCart_FindCart_State = 0;					//寻卡状态位 1成功 0失败 初始化为0
bit OpCart_SelectCart_State = 0;				//选卡状态位 1成功 0失败 初始化为0
bit OpCart_ReadCart_State = 0;					//读卡状态位 1成功 0失败 初始化为0
unsigned int Search_Data(Uchar *IdDtata,Ulongint StartAddr);


/*----------------------------------------
串口3处理上位机信息（从WIFI模块）函数
----------------------------------------*/
void UART3_Proccess(void){
bit Header_State;
unsigned char i,j = 0;
unsigned char CS = 0x00;
	if(S3_Re_Buffer[0] == 0x55 && S3_Re_Buffer[1] == 0xaa)
		Header_State = 1;
	else
		Header_State = 0;
	if(Header_State == 1){																								//头没有出错
	Re_length = Hex2Int (S3_Re_Buffer[2])*256 + Hex2Int (S3_Re_Buffer[3]);//获得data的长度，以便计算校验码CS
	for(i=4;i<10;i++){
			if(LocalHostNum[i] == S3_Re_Buffer[i])														//判断是否是本机编号
				j++;
	}
		if(j==6){																														//是本机编号
			for	(j = 13;j<Re_length + 13;j++){																//计算校验码CS
				CS+=S3_Re_Buffer[j];
			}
			if(CS == S3_Re_Buffer[13+Re_length])															//校验码CS是否出错 不出错则进行数据解析
			{
				//Uart3SendString("CS Passed!\n");
				//Uart3SendData(CS);
				if(S3_Re_Buffer[14+Re_length] == 0x16 && S3_Re_Buffer[15+Re_length] == 0x00){						//检查尾巴0x16
				for(j=0;j<sizeof(MessageType)/2;j++)
						{
						if(MessageType[j][0] == S3_Re_Buffer[11] && MessageType[j][1] == S3_Re_Buffer[12]) //这里确定MessType 
						MessType = j;
					}
				}
				else 
					MessType = 0xff;																								//如果输入过长，提示输入错误，MessType为默认
			}
			else
				Uart3SendString("CS Error!\n");
		}
		else
			Uart3SendString("not localhost number!\n");
	}
	Uart3HaveData = 0; 																											//数据处理完置串口3数据标记为0，等待下一次数据进入
	Re_Cnt = 0;																															//计数器归零
	if(MessType != 0x03){																										//存身份证数据需要用到接收区数据
		for(i=0;i<100;i++)
	S3_Re_Buffer[i] = 0x00;																									//接收取初始化为0x00
	}
}
/***********************************************
*函数名称：Uart4_Process
*功    能：串口命令处理函数
*入口参数：无
*返 回 值：无	
*备    注：无
************************************************/
void Uart4_Process(void)  
{
unsigned int i,j;
length = Hex2Int (re_data_buffer[5])*256 + Hex2Int (re_data_buffer[6]);//十六进制转换为10进制
if(length == 4 &&re_data_buffer[9] == 0x80 && re_data_buffer[10] == 0x84){
OpCart_FindCart_State = 0;
}
if(length == 4 &&re_data_buffer[9] == 0x81 && re_data_buffer[10] == 0x85){
OpCart_SelectCart_State = 0;
}
if(length == 4 &&re_data_buffer[9] == 0x41 && re_data_buffer[10] == 0x45){
OpCart_ReadCart_State = 0;
}
if(length == 8)
	OpCart_FindCart_State = 1;
if(length == 12)
	OpCart_SelectCart_State = 1;
if(length == 1288){
	OpCart_ReadCart_State = 1;
	for(j=2,i = 136;j<20,i < 171;i++,j++){																		//取身份证号码																																			
		Id_Number[j] = re_data_buffer[i];
		i++;																																		//隔一位取一位
	}	
}
Rece_Cnt = 0;																																//接收计数器归零
}

/***********************************************
*函数名称：Save_IDNum_ToFlash
*功    能：将上位机发来的数据有序地放进Flash储存器中
*入口参数1：Data 20位（2位序号  18位身份证号）
*返 回 值：无	
*备    注：无
************************************************/
bit Save_IDNum_ToFlash(void){
Uchar TotalNumData[2]={0x00, 0x00};			//用来保存数据的索引号
Ulongint CurrentAddr;
Ulongint EndAddr;
Uint TotalNum;

sFLASH_ReadBuffer(TotalNumData,FLASH_ReadAddress,2);	 							   //上电稳定以后先读取Flash中储存数据的个数
TotalNum = Hex2Int (TotalNumData[0])*256 + Hex2Int (TotalNumData[1]);	//十六进制转换为10进制
if(TotalNum != 0xffff){
EndAddr = (unsigned long int)(TotalNum*4096) + 4096;
}
else{
TotalNum = 0;
EndAddr = 0x00001000;
}

CurrentAddr = Search_Data(IdData,0x00001000);
if(CurrentAddr == 0){																									//没有搜索到  在后面追加写入
TotalNum++;
TotalNumData[0] = (unsigned char)(TotalNum>>8);												//TotalNum的高8位
TotalNumData[1] = (unsigned char)TotalNum;														//TotalNum的低8位
sFLASH_EraseSector(FLASH_SectorToErase);															//删除一个扇区
sFLASH_WritePage(TotalNumData,FLASH_WriteAddress,2);									//保存一下数据的个数  以便下次写操作追加  下次写操作先读取这个数 然后定位写的地址
IdData[0] = TotalNumData[0];
IdData[1] = TotalNumData[1];
sFLASH_EraseSector(EndAddr);																			//删除一个扇区
sFLASH_WritePage(IdData,EndAddr,20);
return 1;
}
else
return 0;																				
}

unsigned int Search_Data(Uchar *IdData,Ulongint StartAddr){
Uchar TotalNumData[2]={0x00, 0x00};			//用来保存数据的索引号
Uchar i = 2;
Uchar TempBuffer[20] = {0};
unsigned int TotalNum = 0x0000;									//数据的总个数
Ulongint EndAddr;
Ulongint CurrentAddr;

CurrentAddr = StartAddr;
sFLASH_ReadBuffer(TotalNumData,FLASH_ReadAddress,2);	 							   //上电稳定以后先读取Flash中储存数据的个数
TotalNum = Hex2Int (TotalNumData[0])*256 + Hex2Int (TotalNumData[1]);	//十六进制转换为10进制
if(TotalNum != 0xffff){
EndAddr = (unsigned long int)(TotalNum*4096) + 4096;
}
else{
TotalNum = 0;
EndAddr = 0x00001000;
}
IdData=IdData + 2;
while (CurrentAddr < EndAddr){
sFLASH_ReadBuffer(TempBuffer,CurrentAddr,20);	 												//先读取在比较						   
while(TempBuffer[i] == *IdData && i < 20){
i++;
IdData++;
}
if(i==0x14){																													//数据相同
return CurrentAddr;																										//搜索到相同数据  返回数据的当前地址
}
CurrentAddr+=4096;																										//数据不相同将当前地址后移继续搜索
}
return 0;
}

/***********************************************
*函数名称：Compare_Id_NumberInfo
*功    能：身份证信息验证
*入口参数：无
*返 回 值：无	
*备    注：无
************************************************/
bit Compare_Id_NumberInfo(void){
unsigned int Cnt,Cnt0,ID;
ID=sFLASH_ReadID();								//读取W25Q64的器件ID号																		
if(ID!=sFLASH_W25Q64_ID)				//如果ID号不同
{
Uart3SendString("ID ERROR!");
while(1);	
}

Delay_ms(200);
//将该页的数据读
sFLASH_ReadBuffer(spi_re_data_buffer,FLASH_Address,20);
//比较写入和读取的数据是否一致
for(Cnt0=2,Cnt=2;Cnt<20,Cnt0<20;Cnt++,Cnt0++) 
{
//Uart1SendData(spi_re_data_buffer[Cnt]);
//如果有数据不同
if(spi_re_data_buffer[Cnt]!=Id_Number[Cnt0])
{			
return 0;			
}	
}	
if(Cnt == 0x0014){															//数据没有错
return 1;
}
return 0;
}



/***********************************************
*函数名称：Op_IdCard
*功    能：身份证模块操作函数
*入口参数：无
*返 回 值：无	
*备    注：无
************************************************/
void Op_IdCard(void){
unsigned int n,Cnt;
bit flag = 0;
while(!flag & Uart3HaveData != 1){																								//不断循环等待放卡
while(!OpCart_FindCart_State & Uart3HaveData != 1){														//一直等待寻卡成功
for (n=0;n<10;n++){
		Uart4SendData(FIND_CARD[n]);  
	}
			//Uart4_Process();	
	Delay_ms(200);																					//等待接收数据
	Uart4_Process();
}

Cnt = 0;
while(OpCart_FindCart_State == 1 & !OpCart_SelectCart_State & Cnt < 10 & Uart3HaveData != 1){							//一直等待选卡成功,最多重试10次 避免寻卡成功后移除了卡 而导致死循环
for (n=0;n<10;n++){
	Uart4SendData(SELECT_CARD[n]);  
}
		//Uart4_Process();	
Delay_ms(200);																						//等待接收数据
Uart4_Process();
Cnt++;
}

Cnt = 0;
while(OpCart_FindCart_State == 1 & OpCart_SelectCart_State == 1 & !OpCart_ReadCart_State & Cnt < 3 & Uart3HaveData != 1){												//一直等待读卡成功
for (n=0;n<10;n++){
	Uart4SendData(READ_CARD[n]);  
}	
Delay_ms(3000);																					//等待接收数据，这里模块返回数据延迟高需要等待的时间长一些。
Uart4_Process();
FLASH_Address = Search_Data(Id_Number,0x00001000);
if(Compare_Id_NumberInfo()){																					//开始对比数据
Uart3SendString("ID Number Verification Pass!Open The Door!\n");
P43 = 0;																														//开闸
P44 = 0;
Delay_ms(1000);
P43 = 1;
P44 = 1;
}
else
Uart3SendString("ID Number Error!\n");
Cnt++;
}
if(Cnt == 1){
Uart3SendString("Id Cart Read Success!\n");
flag = 1;																								//读卡成功置标志位Flag为0
}

OpCart_FindCart_State = 0;														//读取一张卡后重新初始化
OpCart_SelectCart_State = 0;
OpCart_ReadCart_State = 0;
}
}


/***********************************************
*函数名称：CommonMessProce
*功    能：通用消息处理函数函数
*入口参数：无
*返 回 值：无	
*备    注：根据消息类型MessType值做出相应动作
************************************************/

void CommonMessProce(void){

unsigned char i,j;
static bit  FirstState = 0;																													//第一次指纹对比状态位
static bit  SecondState = 0;																												//第二次指纹对比状态位
unsigned char cnt = 0;																											//重试次数计数
ZA_16U *u16MatchScore;
switch (MessType){
	case 0x00 :
		Uart3SendString("Heart packet!\n");
		break;
	case 0x01 :
		Uart3SendString("Realoading!\n");
		break;
	case 0x02 :																																//开闸
		Uart3SendString("Open The Door!\n");
		P43 = 0;
		P44 = 0;
		Delay_ms(2000);
		P43 = 1;
		P44 = 1;
		break;
	case 0x03 :																														//下载业主身份证信息
for(i=2,j=13;i<20,j<31;i++,j++)
IdData[i] = S3_Re_Buffer[j];																						//整理数据，准备往Flash中存
for (i=0;i<100;i++)																											//存完将接收数组置零
S3_Re_Buffer[i] = 0x00;
if(Save_IDNum_ToFlash())
USART3_Send(ID_DL_SUCCESS,sizeof(ID_DL_SUCCESS));
//Uart3SendString("Save Id Number Success!\n");
else
USART3_Send(ID_DL_FAILURE,sizeof(ID_DL_FAILURE));
//Uart3SendString("Id Number already exist!\n");
break;
case 0x04 :																															//身份信息验证
Uart3SendString("Please Scan the ID Cart!\n");
Op_IdCard();																													//身份证信息扫描录入
break;
	case 0x05 :																																//发送心跳包
		for(i=0;i<16;i++){
		Uart3SendData(HEARTBREAK[i]);
		}
	case 0x06 :
		Uart3SendString("Ready!\n");
	break;
case 0x07 :
	//Uart3SendString("Please put your finger to the module!\n");
							
	Delay_ms(1000);
	while(LD9900MT_GetImage() != 0x00 & cnt < 10){						//一直等待获取指纹图像成功  最多尝试10次
		//Uart3SendString("First input!\n");
		FirstState = 0;																					//置状态位为0
		cnt++;
		Delay_ms(200);			
	}
	if(cnt < 10){
		FirstState = 1;
		if(LD9900MT_GenChar(0x01) == 0x00)											//生成特征储存在 CharBuffer 1
		//Uart3SendString("First GenChar Success!\n");
		USART3_Send(FIRST_SUCCESS,sizeof(FIRST_SUCCESS));
	}
	else{
		FirstState = 0;
		USART3_Send(FIRST_FAILURE,sizeof(FIRST_FAILURE));
	}
	break;
	
case 0x08 :
		Delay_ms(1000);
	cnt = 0;																									//cnt重新初始化为0
	while(LD9900MT_GetImage() != 0x00 & cnt < 10){						//一直等待获取指纹图像成功	 最多尝试10次
		//Uart3SendString("Second input!\n");
		SecondState = 0;																				//置状态位为0
		cnt++;
		Delay_ms(200);		
	}
	if(cnt < 10){
		SecondState = 1;
		if(LD9900MT_GenChar(0x02) == 0x00)									//生成特征储存在 CharBuffer 2	
		//Uart3SendString("Second GenChar Success!\n");
		USART3_Send(SECOND_SUCCESS,sizeof(SECOND_SUCCESS));
	}
	else{
		SecondState = 0;
		USART3_Send(SECOND_FAILURE,sizeof(SECOND_FAILURE));
	}

		if(FirstState == 1 & SecondState == 1){
		FirstState = 0;	
		SecondState = 0;
		//Uart3SendString("Two all success!\n");
			if(LD9900MT_CreateTemplate() == 0x00){								//生成模板
				//Uart3SendString("Create Template Success!\n");
				if(LD9900MT_Store(0x02,0x0001) == 0x00)							//生成模板后CharBuffer1和CharBuffer2中的内容是一样的，储存模板到库序号1
				//Uart3SendString("Store Template Success!\n");
				USART3_Send(LD9900MT_CT_SUCCESS,sizeof(LD9900MT_CT_SUCCESS));
			}
			else
				USART3_Send(LD9900MT_CT_FAILURE,sizeof(LD9900MT_CT_FAILURE));
				//Uart3SendString("Create Template Falure!\n");
	}
		else
			Uart3SendString("Falure!\n");													//两次取手指中间有失败
	break;
case 0x09 :
Uart3SendData(LD9900MT_LoadChar(0x02,0x0001));							//将模板从库中拿出来放到CharBuffer2
Delay_ms(1000);
while(LD9900MT_GetImage() != 0x00){												//等待获取指纹图像成功
	//Uart3SendString("Please put your finger to the module!\n");
	Delay_ms(200);			
}
Uart3SendData(LD9900MT_GenChar(0x01));									//生成特征储存在 CharBuffer1
Uart3SendString("Now compare data!\n");
if(LD9900MT_Match(u16MatchScore) == 0x00){						//开始对比
	Uart3SendString("Pass!Open The Door!\n");
			P43 = 0;																				//开闸
			P44 = 0;
			Delay_ms(2000);
			P43 = 1;
			P44 = 1;
}
else
	Uart3SendString("Not Pass!\n");;
	break;
default :
	//Uart3SendString("Please Input right command!\n")
			;
}
MessType = 0xff;																													//MessType 消息处理完后，恢复到默认状态。
}

