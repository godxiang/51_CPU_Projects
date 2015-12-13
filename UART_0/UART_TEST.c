
//包含文件定义
//#include "reg51.h"
//#include <stc.h>
#include "stc15w4k32s4.h"
#include "intrins.h"
#include "string.h"
//串口相关常量定义
#define  FOSC              22118400L   //系统频率11059200L 22118400L
#define  Uart1_MaxString   32          //串口1最大接收缓冲区字符数量(0-31)共32个字符
#define  Uart2_MaxString   32          //串口2最大接收缓冲区字符数量(0-31)共32个字符
//串口相关变量定义
char           Uart1_Data[Uart1_MaxString];   //串口1接收字串
unsigned char  Uart1_String_P;                //串口1接收字串位置指针
bit            Uart1_busy;                    //串口1正在发送标记,1，正在发送，0，发送完成
unsigned char  Uart1_ParitySet;               //串口1奇偶校验方式 0不校验 1奇校验 2偶校验 3始终1 4始终0
bit            Uart1_Parity;                  //串口1接收到的奇偶校验状态 1,0
bit            Uart1_Parity_out;              //串口1验证接收到的奇偶校验状态 1,验证,0验证错误
char           Uart2_Data[Uart1_MaxString];   //串口2接收字串
unsigned char  Uart2_String_P;                //串口2接收字串位置指针
bit            Uart2_busy;                    //串口2正在发送标记,1，正在发送，0，发送完成
unsigned char  Uart2_ParitySet;               //串口2奇偶校验方式 0不校验 1奇校验 2偶校验 3始终1 4始终0
bit            Uart2_Parity;                  //串口2接收到的奇偶校验状态 1,0
bit            Uart2_Parity_out;              //串口2验证接收到的奇偶校验状态 1,验证,0验证错误
//Uart1包含函数申明
void Uart1_Setup(unsigned long Uart1_Baud,bit Uart1_Tx); //串口1波特率和时基源设置函数
void Uart1_SendData(unsigned char Uart1_dat);                  //串口1单字符发送函数
void Uart1_SendString(char *s);                          //串口1字符串发送函数
//void Uart1_Isr() interrupt 4 using 1;                    //串口1中断服务函数
void Uart2_Setup(unsigned long Uart2_Baud);              //串口2波特率
void Uart2_SendData(unsigned char Uart2_dat);                  //串口2单字符发送函数
void Uart2_SendString(char *s);                          //串口2字符串发送函数
//void Uart1_Isr() interrupt 4 using 1;                    //串口2中断服务函数
/*串口编程示范============================================================================
Uart1_String_P=0;                //串口1接收字串下标归0
Uart1_ParitySet=1;               //串口1使用9位可变波特率方式，校验方式为奇校验
Uart1_Setup(9600,0);             //串口1波特率为9600bps，时基源选择定时器1
Uart1_SendString("hello word");  //串口1发送数据，英文字母 hello word
Uart2_String_P=0;                //串口2接收字串下标归0
Uart2_ParitySet=1;               //串口2使用9位可变波特率方式，校验方式为奇校验
Uart2_Setup(9600);               //串口2波特率为9600bps
Uart2_SendString("hello word");  //串口2发送数据，英文字母 hello word
=======================================================================================*/
/*********************************************************
** 函数名称: void Uart1_Setup(unsigned long Uart1_Baud,bit Uart1_Tx) 
** 功能描述: 初始化串口函数
** 输　  入: unsigned long Uart1_Baud,bit Uart1_Tx
** 输　  出: 无
** 全局变量: PCON,SCON,TMOD,AUXR,TCON,TH1,
** 调用模块: 
** 备    注:时钟为22118400Hz
            参数1:波特率设置,300;600;1200;2400;4800;9600;19200
            参数2:计数器选择,0,定时器1;1,独立波特率
**********************************************************/
void Uart1_Setup(unsigned long Uart1_Baud,bit Uart1_Tx) 
{
unsigned int Uart1_Brt;
//时基源设置-------------------------------------------------
if(Uart1_Tx)
    {
AUXR |= 0x01;  //串口1选择独立波特率发生器
}
else
    {
AUXR &= 0xFE;  //串口1选择定时器1做波特率发生器
}
//校验方式设置-------------------------------------------------
switch (Uart1_ParitySet)
    {//判断校验方式
    case 1:
        {//9位串口, 奇校验 Uart1_ParitySet=1;   //校验位初值状态标记置1
        SCON=0xD8;           //9位波特率可变,允许接收,TB8待发第9位初始置1
        break;
        }
    case 2:
        {//9位串口, 偶校验 Uart1_ParitySet=2;   //校验位初值状态标记置2
        SCON=0xD4;           //9位波特率可变,允许接收,RB8待收第9位初始置1
        break;
        }
    case 3:
        {//9位串口, 掩码校验 校验值始终为 1 Uart1_ParitySet=3;   //校验位初值状态标记置3
        SCON|=0xD8;         //9位波特率可变,允许接收,TB8待发第9位初始置1
        break;
        }
    case 4:
        {//9位串口, 空校验 校验值始终为 0 Uart1_ParitySet=4;   //校验位初值状态标记置4
        SCON=0xD4;          //9位波特率可变,允许接收,RB8待收第9位初始置1
        break;
        }
    default:
        {//8位串口, 无校验 Uart1_ParitySet=0;   //校验位初值状态标记置0
        SCON&=0x5F;         //清除第5位,第7位
        SCON|=0x50;         //8位波特率可变,允许接收
        break;
        }
    }
//波特率设置并开始运行串口-------------------------------------------------
//时钟为22118400Hz
Uart1_Brt=FOSC/32/Uart1_Baud;
if(Uart1_Brt>255)
    {//如果重装值>255,使用12T模式12分频
    if(Uart1_Tx)
        {//如果使用独立波特率发生器
        //PCON |= 0x80;                                 //串口1波特率加倍
        //PCON &= 0x7f;                                 //串口1波特率不加倍
        AUXR &= 0xFB;                                 //独立波特率使用12T模式
        //AUXR |= 0x04;                                 //独立波特率使用1T模式
        BRT = 256-Uart1_Brt/12;                       //独立波特率发生器初值设置
        AUXR|= 0x10;                                  //独立波特率发生器开始计数
        }
    else
        {//如果使用定时器1做波特率发生器
        TMOD&=0xCF;                                 //清除定时器1设置状态，定时器0不修改
        TMOD|=0x20;                                 //定时器1使用8位自动重装模式
        //PCON |= 0x80;                               //串口1波特率加倍
        //PCON &= 0x7f;                               //串口1波特率不加倍
        AUXR &= 0xBF;                               //定时器1使用12T模式
        //AUXR|=0x40;                                 //定时器1使用1T模式
        TH1  = TL1 = 256-Uart1_Brt/12;              //设置自动装入的值
        ET1  = 0;                                   //关闭定时器1中断
        TR1  = 1;                                   //TCON|=0x40;定时器1开始计数,TCON&=0xBF;关闭计数
        }
    }
else
    {//如果重装值<255,使用1T模式1分频
    if(Uart1_Tx)
        {//如果使用独立波特率发生器
        //PCON |= 0x80;                                 //串口1波特率加倍
        //PCON &= 0x7f;                                 //串口1波特率不加倍
        //AUXR &= 0xFB;                                 //独立波特率使用12T模式
        AUXR |= 0x04;                                 //独立波特率使用1T模式
        BRT  = 256-Uart1_Brt;                         //独立波特率发生器初值设置
        AUXR|= 0x10;                                  //独立波特率发生器开始计数
        }
    else
        {//如果使用定时器1做波特率发生器
        TMOD&=0xCF;                                 //清除定时器1设置状态，定时器0不修改
        TMOD|=0x20;                                 //定时器1使用8位自动重装模式
        //PCON |= 0x80;                               //串口1波特率加倍
        //PCON &= 0x7f;                               //串口1波特率不加倍
        //AUXR &= 0xBF;                               //定时器1使用12T模式
        AUXR|=0x40;                                 //定时器1使用1T模式
        TH1  = TL1 = 256-Uart1_Brt;                 //设置自动装入的值
        ET1  = 0;                                   //关闭定时器1中断
        TR1  = 1;                                   //TCON|=0x40;定时器1开始计数,TCON&=0xBF;关闭计数
        }
    }
    ES        = 1;                                  //打开串口中断
    EA        = 1;                                  //打开总中断
}
/*********************************************************
** 函数名称: void Uart2_Setup(unsigned long Uart2_Baud) 
** 功能描述: 初始化串口函数
** 输　  入: unsigned long Uart1_Baud
** 输　  出: 无
** 全局变量: 
** 调用模块: 
** 备    注:时钟为22118400Hz
            参数1:波特率设置,300;600;1200;2400;4800;9600;19200
**********************************************************/
void Uart2_Setup(unsigned long Uart2_Baud) 
{
unsigned int Uart2_Brt;
//校验方式设置-------------------------------------------------
switch (Uart2_ParitySet)
    {//判断校验方式
    case 1:
        {//9位串口, 奇校验 Uart1_ParitySet=1;   //校验位初值状态标记置1
        S2CON=0xD8;           //9位波特率可变,允许接收,TB8待发第9位初始置1
        break;
        }
    case 2:
        {//9位串口, 偶校验 Uart1_ParitySet=2;   //校验位初值状态标记置2
        S2CON=0xD4;           //9位波特率可变,允许接收,RB8待收第9位初始置1
        break;
        }
    case 3:
        {//9位串口, 掩码校验 校验值始终为 1 Uart1_ParitySet=3;   //校验位初值状态标记置3
        S2CON|=0xD8;         //9位波特率可变,允许接收,TB8待发第9位初始置1
        break;
        }
    case 4:
        {//9位串口, 空校验 校验值始终为 0 Uart1_ParitySet=4;   //校验位初值状态标记置4
        S2CON=0xD4;          //9位波特率可变,允许接收,RB8待收第9位初始置1
        break;
        }
    default:
        {//8位串口, 无校验 Uart1_ParitySet=0;   //校验位初值状态标记置0
        S2CON&=0x5F;         //清除第5位,第7位
        S2CON|=0x50;         //8位波特率可变,允许接收
        break;
        }
    }
//波特率设置并开始运行串口-------------------------------------------------
//时钟为22118400Hz
Uart2_Brt=FOSC/32/Uart2_Baud;
if(Uart2_Brt>255)
    {//如果重装值>255,使用12T模式12分频
        //PCON |= 0x80;                                 //串口1波特率加倍
        //PCON &= 0x7f;                                 //串口1波特率不加倍
        AUXR &= 0xFB;                                 //独立波特率使用12T模式
        //AUXR |= 0x04;                                 //独立波特率使用1T模式
        BRT = 256-Uart2_Brt/12;                       //独立波特率发生器初值设置
    }
else
    {//如果重装值<255,使用1T模式1分频
        //PCON |= 0x80;                                 //串口1波特率加倍
        //PCON &= 0x7f;                                 //串口1波特率不加倍
        //AUXR &= 0xFB;                                 //独立波特率使用12T模式
        AUXR |= 0x04;                                 //独立波特率使用1T模式
        BRT  = 256-Uart2_Brt;                         //独立波特率发生器初值设置
    }
    AUXR|= 0x10;                                  //独立波特率发生器开始计数
IE2|=0x01;                                    ////打开串口2中断
    EA   = 1;                                     //打开总中断
}
//=======================================================================================
/**********************************************************
串口1中断服务函数
**********************************************************/
void Uart1_Isr() interrupt 4 using 1
{// UART 中断服务函数
EA=0; //关总中断
if (RI)
{//如果 UART 接收产生中断
    RI = 0;                                           //清除 UART 接收中断标记
    ACC= SBUF;                                        //接收缓冲区数据写入累加器A
    Uart1_Parity = P;                                 //保存校验值到Uart1_Parity
    Uart1_Parity_out=0;                               //清除允许写入标记，为下面判断准备
    switch(Uart1_ParitySet)
    {//根据校验要求判断
        case 1:
        {//要求奇校验
        Uart1_Parity_out=Uart1_Parity^RB8;    //如果检验结果和接收结果不符合则成立，允许写入
        break;
        }
        case 2:
        {//要求偶校验
        Uart1_Parity_out=~(Uart1_Parity^RB8); //如果检验结果和接收结果符合则成立，允许写入
        break;
        }
        case 3:
        {//要求常1校验
        Uart1_Parity_out=RB8;                 //如果接收结果为1则成立，允许写入
        break;
        }
        case 4:
        {//要求常0校验
        Uart1_Parity_out=~RB8;                //如果接收结果为0则成立，允许写入
        break;
        }
        default:
        {//要求无校验
        Uart1_Parity_out=1;                  //如果无校验，允许写入
        break;
        }
    }
    if(Uart1_Parity_out)
    {//如果允许写入
        Uart1_Data[Uart1_String_P]= SBUF;              //接收缓冲区数据顺序写入串口接收字串
        Uart1_String_P++;                              //串口接收字串指针累加
        //↓如果串口接收字串指针在大于最大接收缓冲区字符数量,串口接收字串指针归零
        if(Uart1_String_P >= Uart1_MaxString) 
        {//如果接受字串指针越界
            Uart1_String_P=0;                          //接受字串指针归零
        }
    }
}
if (TI)
{//如果 UART 发送产生中断
    TI   = 0;                  //清除 UART 发送中断标记
    Uart1_busy = 0;            //发送完成，清除正在发送标记
}
EA=1;                          //开总中断
}
//=======================================================================================
/**********************************************************
串口2中断服务函数
**********************************************************/
void Uart2_Isr() interrupt 8 using 1
{// UART 中断服务函数
EA=0; //关总中断
if (S2CON&0x01)
{//如果 UART 接收产生中断
    S2CON &= 0xFE;                                     //清除 UART 接收中断标记
    ACC= S2BUF;                                        //接收缓冲区数据写入累加器A
    Uart2_Parity = P;                                  //保存校验值到Uart1_Parity
    Uart2_Parity_out=0;                                //清除允许写入标记，为下面判断准备
    switch(Uart2_ParitySet)
    {//根据校验要求判断
        case 1:
        {//要求奇校验
        Uart2_Parity_out=Uart2_Parity^(S2CON&0x04);    //如果检验结果和接收结果不符合则成立，允许写入 (S2CON&0x04)==S2RB8
        break;
        }
        case 2:
        {//要求偶校验
        Uart2_Parity_out=~(Uart2_Parity^(S2CON&0x04)); //如果检验结果和接收结果符合则成立，允许写入
        break;
        }
        case 3:
        {//要求常1校验
        Uart2_Parity_out=(S2CON&0x04);                 //如果接收结果为1则成立，允许写入
        break;
        }
        case 4:
        {//要求常0校验
        Uart2_Parity_out=~(S2CON&0x04);                //如果接收结果为0则成立，允许写入
        break;
        }
        default:
        {//要求无校验
        Uart2_Parity_out=1;                  //如果无校验，允许写入
        break;
        }
    }
    if(Uart2_Parity_out)
    {//如果允许写入
        Uart2_Data[Uart2_String_P]= S2BUF;              //接收缓冲区数据顺序写入串口接收字串
        Uart2_String_P++;                              //串口接收字串指针累加
        //↓如果串口接收字串指针在大于最大接收缓冲区字符数量,串口接收字串指针归零
        if(Uart2_String_P >= Uart2_MaxString) 
        {//如果接受字串指针越界
            Uart2_String_P=0;                          //接受字串指针归零
        }
    }
}
if (S2CON&0x02)
{//如果 UART 发送产生中断
    S2CON &= 0xFD;                  //清除 UART 发送中断标记
    Uart2_busy = 0;            //发送完成，清除正在发送标记
}
EA=1;                          //开总中断
}

//=======================================================================================
/**********************************************************
** 函数名称: void Uart1_SendData(unsigned char Uart1_dat)
** 功能描述: UART1 单个字符发送函数
** 输　  入: unsigned char Uart1_dat
** 输　  出: 无
** 全局变量: 
** 调用模块: 
** 备    注:参数1:待发送的单个字符
**********************************************************/
void Uart1_SendData(unsigned char Uart1_dat)
{//UART 发送字节函数
    while (Uart1_busy);       //循环等待正在发送标记被清空
    ACC = Uart1_dat;                //被发送数据送累加器A,准备求奇偶校验位 P
    TB8 = P;                  //写奇偶校验标记
    switch(Uart1_ParitySet)
    {//根据校验要求判断
        case 1:
        {//要求奇校验
            TB8=~TB8;        //翻转TB8的值
            break;
        }
        /*偶校验，TB8不变
        case 2:
        {//要求偶校验
            TB8=TB8;            //TB8不变
            break;
        }
        */
        case 3:
        {//要求常1校验
            TB8=1;            //TB8恒等于1
            break;
        }
        case 4:
        {//要求常0校验
            TB8=0;            //TB8恒等于0
            break;
        }
        default:
        {//要求无校验
        break;
        }
    }
    Uart1_busy = 1;          //正在发送标记置 1
    SBUF = ACC;              //累加器 A 中的数据送发送缓冲区发送
}
//=======================================================================================
/**********************************************************
** 函数名称: void Uart2_SendData(unsigned char Uart2_dat)
** 功能描述: UART2 单个字符发送函数
** 输　  入: unsigned char Uart2_dat
** 输　  出: 无
** 全局变量: 
** 调用模块: 
** 备    注:参数1:待发送的单个字符
**********************************************************/
void Uart2_SendData(unsigned char Uart2_dat)
{//UART 发送字节函数
    while (Uart2_busy);       //循环等待正在发送标记被清空
    ACC = Uart2_dat;                //被发送数据送累加器A,准备求奇偶校验位 P
    TB8 = P;                  //写奇偶校验标记
    switch(Uart2_ParitySet)
    {//根据校验要求判断
        case 1:
        {//要求奇校验
            S2CON^=0x08;        //翻转S2TB8的值
            break;
        }
        /*偶校验，S2TB8不变
        case 2:
        {//要求偶校验
            S2TB8=1;            //S2TB8不变
            break;
        }
        */
        case 3:
        {//要求常1校验
            S2CON|=0x08;            //S2TB8恒等于1
            break;
        }
        case 4:
        {//要求常0校验
            S2CON&=0xF7;            //TB8恒等于0
            break;
        }
        default:
        {//要求无校验
        break;
        }
    }
    Uart2_busy = 1;          //正在发送标记置 1
    S2BUF = ACC;              //累加器 A 中的数据送发送缓冲区发送
}
//=======================================================================================
/**********************************************************
** 函数名称: void Uart1_SendString(char *s)
** 功能描述: UART1 按字符串发送函数
** 输　  入: char *s
** 输　  出: 无
** 全局变量: 
** 调用模块: 
** 备    注:参数1:待发送的字符串
**********************************************************/
void Uart1_SendString(char *s)
{//UART 按字符串发送
    while (*s)
    {
        Uart1_SendData(*s++);   //将待发字符串依次使用SendData函数按字节发送
    }
}
//=======================================================================================
/**********************************************************
** 函数名称: void Uart2_SendString(char *s)
** 功能描述: UART2 按字符串发送函数
** 输　  入: char *s
** 输　  出: 无
** 全局变量: 
** 调用模块: 
** 备    注:参数1:待发送的字符串
**********************************************************/
void Uart2_SendString(char *s)
{//UART 按字符串发送
    while (*s)
    {
        Uart2_SendData(*s++);   //将待发字符串依次使用SendData函数按字节发送
    }
}
//=======================================================================================
/*常用串口设置开关===============================副录===================================
AUXR |= 0x01;  //串口1选择独立波特率发生器
AUXR &= 0xFE;  //串口1选择定时器1做波特率发生器
AUXR |= 0x40;  //定时器1使用1T模式
AUXR &= 0xBF;  //定时器1使用12T模式
AUXR |= 0x04;  //独立波特率使用1T模式
AUXR &= 0xFB;  //独立波特率使用12T模式
AUXR |= 0x10;  //打开独立波特率发生器
AUXR &= 0xEF;  //关闭独立波特率发生器
PCON |= 0x80;  //串口1波特率加倍
PCON &= 0x7f;  //串口1波特率不加倍
AUXR |= 0x08;   //串口2波特率加倍
AUXR &= 0xf7;   //串口2波特率不加倍
=======================================================================================*/