#include "uart.h"
#include "w25q64.h"
#include "LD9900MT.h"


#define Uint unsigned int
#define Ulongint unsigned long int
#define Uchar unsigned char

#define Hex2Int(x)		( (x/10)*10+(x%10) )				//ʮ������ת����ʮ����
unsigned char const code FIND_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x01, 0x22};
unsigned char const code SELECT_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x20, 0x02, 0x21};
unsigned char const code READ_CARD[10]={0xaa, 0xaa, 0xaa, 0x96, 0x69, 0x00, 0x03, 0x30, 0x01, 0x32};
ZA_8CONST HEARTBREAK[]={0x55, 0xaa, 0x00, 0x01, 0x03, 0x71, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x16};
unsigned char Id_Number[20]={0x00};						//�������֤����
unsigned int length = 0;								//�������ݳ���
unsigned char  spi_re_data_buffer[20];	//flsh��ȡ��������
unsigned long int FLASH_Address;				//д��Flash�ĵ�ַ
unsigned char IdData[20]={0x00};
unsigned char Re_length;												//������λ�����ݳ���
unsigned char LocalHostNum[10]={0x00, 0x00, 0x00, 0x00, 0x03, 0x71, 0x00, 0x01, 0x00, 0x01};
unsigned char const code MessageType[][2]={{0x00,0x01}, {0x00,0x02}, {0x00,0x03}, {0x10, 0x01}, {0x10, 0x03}, {0xf0, 0x01},
																				{0x30,0x01}, {0x31,0x03}, {0x32,0x03},{0x30,0x04}};	
//������λ���ĳ������ݶ�����
ZA_8CONST ID_DL_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x10,0xff,0x01,0x01,0x16};											//�������֤�ɹ�
ZA_8CONST ID_DL_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x10,0xff,0x00,0x00,0x16};											//�������֤ʧ��
ZA_8CONST FIRST_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x31,0xff,0x01,0x01,0x16};											//��һ�βɼ�ָ�Ƴɹ�
ZA_8CONST SECOND_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x32,0xff,0x01,0x01,0x16};											//��һ�βɼ�ָ��ʧ��
ZA_8CONST FIRST_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x31,0xff,0x00,0x00,0x16};											//�ڶ��βɼ�ָ�Ƴɹ�
ZA_8CONST SECOND_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x32,0xff,0x00,0x00,0x16};											//�ڶ��βɼ�ָ��ʧ��
ZA_8CONST LD9900MT_CT_SUCCESS[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x3f,0xff,0x01,0x01,0x16};								//	����ģ��ɹ�
ZA_8CONST LD9900MT_CT_FAILURE[]={0x55,0xAA,0x00,0x01,0x03,0x71,0x00,0x01,0x00,0x01,0x01,0x3f,0xff,0x00,0x00,0x16};								//	����ģ��ʧ��
unsigned char MessType = 0xff;
bit Uart3HaveData;
bit OpCart_FindCart_State = 0;					//Ѱ��״̬λ 1�ɹ� 0ʧ�� ��ʼ��Ϊ0
bit OpCart_SelectCart_State = 0;				//ѡ��״̬λ 1�ɹ� 0ʧ�� ��ʼ��Ϊ0
bit OpCart_ReadCart_State = 0;					//����״̬λ 1�ɹ� 0ʧ�� ��ʼ��Ϊ0
unsigned int Search_Data(Uchar *IdDtata,Ulongint StartAddr);


/*----------------------------------------
����3������λ����Ϣ����WIFIģ�飩����
----------------------------------------*/
void UART3_Proccess(void){
bit Header_State;
unsigned char i,j = 0;
unsigned char CS = 0x00;
	if(S3_Re_Buffer[0] == 0x55 && S3_Re_Buffer[1] == 0xaa)
		Header_State = 1;
	else
		Header_State = 0;
	if(Header_State == 1){																								//ͷû�г���
	Re_length = Hex2Int (S3_Re_Buffer[2])*256 + Hex2Int (S3_Re_Buffer[3]);//���data�ĳ��ȣ��Ա����У����CS
	for(i=4;i<10;i++){
			if(LocalHostNum[i] == S3_Re_Buffer[i])														//�ж��Ƿ��Ǳ������
				j++;
	}
		if(j==6){																														//�Ǳ������
			for	(j = 13;j<Re_length + 13;j++){																//����У����CS
				CS+=S3_Re_Buffer[j];
			}
			if(CS == S3_Re_Buffer[13+Re_length])															//У����CS�Ƿ���� ��������������ݽ���
			{
				//Uart3SendString("CS Passed!\n");
				//Uart3SendData(CS);
				if(S3_Re_Buffer[14+Re_length] == 0x16 && S3_Re_Buffer[15+Re_length] == 0x00){						//���β��0x16
				for(j=0;j<sizeof(MessageType)/2;j++)
						{
						if(MessageType[j][0] == S3_Re_Buffer[11] && MessageType[j][1] == S3_Re_Buffer[12]) //����ȷ��MessType 
						MessType = j;
					}
				}
				else 
					MessType = 0xff;																								//��������������ʾ�������MessTypeΪĬ��
			}
			else
				Uart3SendString("CS Error!\n");
		}
		else
			Uart3SendString("not localhost number!\n");
	}
	Uart3HaveData = 0; 																											//���ݴ������ô���3���ݱ��Ϊ0���ȴ���һ�����ݽ���
	Re_Cnt = 0;																															//����������
	if(MessType != 0x03){																										//�����֤������Ҫ�õ�����������
		for(i=0;i<100;i++)
	S3_Re_Buffer[i] = 0x00;																									//����ȡ��ʼ��Ϊ0x00
	}
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
unsigned int i,j;
length = Hex2Int (re_data_buffer[5])*256 + Hex2Int (re_data_buffer[6]);//ʮ������ת��Ϊ10����
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
	for(j=2,i = 136;j<20,i < 171;i++,j++){																		//ȡ���֤����																																			
		Id_Number[j] = re_data_buffer[i];
		i++;																																		//��һλȡһλ
	}	
}
Rece_Cnt = 0;																																//���ռ���������
}

/***********************************************
*�������ƣ�Save_IDNum_ToFlash
*��    �ܣ�����λ����������������طŽ�Flash��������
*��ڲ���1��Data 20λ��2λ���  18λ���֤�ţ�
*�� �� ֵ����	
*��    ע����
************************************************/
bit Save_IDNum_ToFlash(void){
Uchar TotalNumData[2]={0x00, 0x00};			//�����������ݵ�������
Ulongint CurrentAddr;
Ulongint EndAddr;
Uint TotalNum;

sFLASH_ReadBuffer(TotalNumData,FLASH_ReadAddress,2);	 							   //�ϵ��ȶ��Ժ��ȶ�ȡFlash�д������ݵĸ���
TotalNum = Hex2Int (TotalNumData[0])*256 + Hex2Int (TotalNumData[1]);	//ʮ������ת��Ϊ10����
if(TotalNum != 0xffff){
EndAddr = (unsigned long int)(TotalNum*4096) + 4096;
}
else{
TotalNum = 0;
EndAddr = 0x00001000;
}

CurrentAddr = Search_Data(IdData,0x00001000);
if(CurrentAddr == 0){																									//û��������  �ں���׷��д��
TotalNum++;
TotalNumData[0] = (unsigned char)(TotalNum>>8);												//TotalNum�ĸ�8λ
TotalNumData[1] = (unsigned char)TotalNum;														//TotalNum�ĵ�8λ
sFLASH_EraseSector(FLASH_SectorToErase);															//ɾ��һ������
sFLASH_WritePage(TotalNumData,FLASH_WriteAddress,2);									//����һ�����ݵĸ���  �Ա��´�д����׷��  �´�д�����ȶ�ȡ����� Ȼ��λд�ĵ�ַ
IdData[0] = TotalNumData[0];
IdData[1] = TotalNumData[1];
sFLASH_EraseSector(EndAddr);																			//ɾ��һ������
sFLASH_WritePage(IdData,EndAddr,20);
return 1;
}
else
return 0;																				
}

unsigned int Search_Data(Uchar *IdData,Ulongint StartAddr){
Uchar TotalNumData[2]={0x00, 0x00};			//�����������ݵ�������
Uchar i = 2;
Uchar TempBuffer[20] = {0};
unsigned int TotalNum = 0x0000;									//���ݵ��ܸ���
Ulongint EndAddr;
Ulongint CurrentAddr;

CurrentAddr = StartAddr;
sFLASH_ReadBuffer(TotalNumData,FLASH_ReadAddress,2);	 							   //�ϵ��ȶ��Ժ��ȶ�ȡFlash�д������ݵĸ���
TotalNum = Hex2Int (TotalNumData[0])*256 + Hex2Int (TotalNumData[1]);	//ʮ������ת��Ϊ10����
if(TotalNum != 0xffff){
EndAddr = (unsigned long int)(TotalNum*4096) + 4096;
}
else{
TotalNum = 0;
EndAddr = 0x00001000;
}
IdData=IdData + 2;
while (CurrentAddr < EndAddr){
sFLASH_ReadBuffer(TempBuffer,CurrentAddr,20);	 												//�ȶ�ȡ�ڱȽ�						   
while(TempBuffer[i] == *IdData && i < 20){
i++;
IdData++;
}
if(i==0x14){																													//������ͬ
return CurrentAddr;																										//��������ͬ����  �������ݵĵ�ǰ��ַ
}
CurrentAddr+=4096;																										//���ݲ���ͬ����ǰ��ַ���Ƽ�������
}
return 0;
}

/***********************************************
*�������ƣ�Compare_Id_NumberInfo
*��    �ܣ����֤��Ϣ��֤
*��ڲ�������
*�� �� ֵ����	
*��    ע����
************************************************/
bit Compare_Id_NumberInfo(void){
unsigned int Cnt,Cnt0,ID;
ID=sFLASH_ReadID();								//��ȡW25Q64������ID��																		
if(ID!=sFLASH_W25Q64_ID)				//���ID�Ų�ͬ
{
Uart3SendString("ID ERROR!");
while(1);	
}

Delay_ms(200);
//����ҳ�����ݶ�
sFLASH_ReadBuffer(spi_re_data_buffer,FLASH_Address,20);
//�Ƚ�д��Ͷ�ȡ�������Ƿ�һ��
for(Cnt0=2,Cnt=2;Cnt<20,Cnt0<20;Cnt++,Cnt0++) 
{
//Uart1SendData(spi_re_data_buffer[Cnt]);
//��������ݲ�ͬ
if(spi_re_data_buffer[Cnt]!=Id_Number[Cnt0])
{			
return 0;			
}	
}	
if(Cnt == 0x0014){															//����û�д�
return 1;
}
return 0;
}



/***********************************************
*�������ƣ�Op_IdCard
*��    �ܣ����֤ģ���������
*��ڲ�������
*�� �� ֵ����	
*��    ע����
************************************************/
void Op_IdCard(void){
unsigned int n,Cnt;
bit flag = 0;
while(!flag & Uart3HaveData != 1){																								//����ѭ���ȴ��ſ�
while(!OpCart_FindCart_State & Uart3HaveData != 1){														//һֱ�ȴ�Ѱ���ɹ�
for (n=0;n<10;n++){
		Uart4SendData(FIND_CARD[n]);  
	}
			//Uart4_Process();	
	Delay_ms(200);																					//�ȴ���������
	Uart4_Process();
}

Cnt = 0;
while(OpCart_FindCart_State == 1 & !OpCart_SelectCart_State & Cnt < 10 & Uart3HaveData != 1){							//һֱ�ȴ�ѡ���ɹ�,�������10�� ����Ѱ���ɹ����Ƴ��˿� ��������ѭ��
for (n=0;n<10;n++){
	Uart4SendData(SELECT_CARD[n]);  
}
		//Uart4_Process();	
Delay_ms(200);																						//�ȴ���������
Uart4_Process();
Cnt++;
}

Cnt = 0;
while(OpCart_FindCart_State == 1 & OpCart_SelectCart_State == 1 & !OpCart_ReadCart_State & Cnt < 3 & Uart3HaveData != 1){												//һֱ�ȴ������ɹ�
for (n=0;n<10;n++){
	Uart4SendData(READ_CARD[n]);  
}	
Delay_ms(3000);																					//�ȴ��������ݣ�����ģ�鷵�������ӳٸ���Ҫ�ȴ���ʱ�䳤һЩ��
Uart4_Process();
FLASH_Address = Search_Data(Id_Number,0x00001000);
if(Compare_Id_NumberInfo()){																					//��ʼ�Ա�����
Uart3SendString("ID Number Verification Pass!Open The Door!\n");
P43 = 0;																														//��բ
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
flag = 1;																								//�����ɹ��ñ�־λFlagΪ0
}

OpCart_FindCart_State = 0;														//��ȡһ�ſ������³�ʼ��
OpCart_SelectCart_State = 0;
OpCart_ReadCart_State = 0;
}
}


/***********************************************
*�������ƣ�CommonMessProce
*��    �ܣ�ͨ����Ϣ����������
*��ڲ�������
*�� �� ֵ����	
*��    ע��������Ϣ����MessTypeֵ������Ӧ����
************************************************/

void CommonMessProce(void){

unsigned char i,j;
static bit  FirstState = 0;																													//��һ��ָ�ƶԱ�״̬λ
static bit  SecondState = 0;																												//�ڶ���ָ�ƶԱ�״̬λ
unsigned char cnt = 0;																											//���Դ�������
ZA_16U *u16MatchScore;
switch (MessType){
	case 0x00 :
		Uart3SendString("Heart packet!\n");
		break;
	case 0x01 :
		Uart3SendString("Realoading!\n");
		break;
	case 0x02 :																																//��բ
		Uart3SendString("Open The Door!\n");
		P43 = 0;
		P44 = 0;
		Delay_ms(2000);
		P43 = 1;
		P44 = 1;
		break;
	case 0x03 :																														//����ҵ�����֤��Ϣ
for(i=2,j=13;i<20,j<31;i++,j++)
IdData[i] = S3_Re_Buffer[j];																						//�������ݣ�׼����Flash�д�
for (i=0;i<100;i++)																											//���꽫������������
S3_Re_Buffer[i] = 0x00;
if(Save_IDNum_ToFlash())
USART3_Send(ID_DL_SUCCESS,sizeof(ID_DL_SUCCESS));
//Uart3SendString("Save Id Number Success!\n");
else
USART3_Send(ID_DL_FAILURE,sizeof(ID_DL_FAILURE));
//Uart3SendString("Id Number already exist!\n");
break;
case 0x04 :																															//�����Ϣ��֤
Uart3SendString("Please Scan the ID Cart!\n");
Op_IdCard();																													//���֤��Ϣɨ��¼��
break;
	case 0x05 :																																//����������
		for(i=0;i<16;i++){
		Uart3SendData(HEARTBREAK[i]);
		}
	case 0x06 :
		Uart3SendString("Ready!\n");
	break;
case 0x07 :
	//Uart3SendString("Please put your finger to the module!\n");
							
	Delay_ms(1000);
	while(LD9900MT_GetImage() != 0x00 & cnt < 10){						//һֱ�ȴ���ȡָ��ͼ��ɹ�  ��ೢ��10��
		//Uart3SendString("First input!\n");
		FirstState = 0;																					//��״̬λΪ0
		cnt++;
		Delay_ms(200);			
	}
	if(cnt < 10){
		FirstState = 1;
		if(LD9900MT_GenChar(0x01) == 0x00)											//�������������� CharBuffer 1
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
	cnt = 0;																									//cnt���³�ʼ��Ϊ0
	while(LD9900MT_GetImage() != 0x00 & cnt < 10){						//һֱ�ȴ���ȡָ��ͼ��ɹ�	 ��ೢ��10��
		//Uart3SendString("Second input!\n");
		SecondState = 0;																				//��״̬λΪ0
		cnt++;
		Delay_ms(200);		
	}
	if(cnt < 10){
		SecondState = 1;
		if(LD9900MT_GenChar(0x02) == 0x00)									//�������������� CharBuffer 2	
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
			if(LD9900MT_CreateTemplate() == 0x00){								//����ģ��
				//Uart3SendString("Create Template Success!\n");
				if(LD9900MT_Store(0x02,0x0001) == 0x00)							//����ģ���CharBuffer1��CharBuffer2�е�������һ���ģ�����ģ�嵽�����1
				//Uart3SendString("Store Template Success!\n");
				USART3_Send(LD9900MT_CT_SUCCESS,sizeof(LD9900MT_CT_SUCCESS));
			}
			else
				USART3_Send(LD9900MT_CT_FAILURE,sizeof(LD9900MT_CT_FAILURE));
				//Uart3SendString("Create Template Falure!\n");
	}
		else
			Uart3SendString("Falure!\n");													//����ȡ��ָ�м���ʧ��
	break;
case 0x09 :
Uart3SendData(LD9900MT_LoadChar(0x02,0x0001));							//��ģ��ӿ����ó����ŵ�CharBuffer2
Delay_ms(1000);
while(LD9900MT_GetImage() != 0x00){												//�ȴ���ȡָ��ͼ��ɹ�
	//Uart3SendString("Please put your finger to the module!\n");
	Delay_ms(200);			
}
Uart3SendData(LD9900MT_GenChar(0x01));									//�������������� CharBuffer1
Uart3SendString("Now compare data!\n");
if(LD9900MT_Match(u16MatchScore) == 0x00){						//��ʼ�Ա�
	Uart3SendString("Pass!Open The Door!\n");
			P43 = 0;																				//��բ
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
MessType = 0xff;																													//MessType ��Ϣ������󣬻ָ���Ĭ��״̬��
}

