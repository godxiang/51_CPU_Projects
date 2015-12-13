				   /*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC15F4K60S4 系列 IO测试举例------------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966-------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/* 如果要在文章中应用此代码,请在文章中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

//本示例在Keil开发环境下请选择Intel的8058芯片型号进行编译
//若无特别说明,工作频率一般为11.0592MHz


#include "reg51.h"

sfr P0M1 = 0x93;
sfr P0M0 = 0x94;
sfr P1M1 = 0x91;
sfr P1M0 = 0x92;
sfr P2M1 = 0x95;
sfr P2M0 = 0x96;
sfr P3M1 = 0xb1;
sfr P3M0 = 0xb2;
sfr P4M1 = 0xb3;
sfr P4M0 = 0xb4;
sfr P5M1 = 0xC9;
sfr P5M0 = 0xCA;
sfr P6M1 = 0xCB;
sfr P6M0 = 0xCC;
sfr P7M1 = 0xE1;
sfr P7M0 = 0xE2;

sbit P00 = P0^0;
sbit P01 = P0^1;
sbit P02 = P0^2;
sbit P03 = P0^3;
sbit P04 = P0^4;
sbit P05 = P0^5;
sbit P06 = P0^6;
sbit P07 = P0^7;
sbit P10 = P1^0;
sbit P11 = P1^1;
sbit P12 = P1^2;
sbit P13 = P1^3;
sbit P14 = P1^4;
sbit P15 = P1^5;
sbit P16 = P1^6;
sbit P17 = P1^7;
sbit P20 = P2^0;
sbit P21 = P2^1;
sbit P22 = P2^2;
sbit P23 = P2^3;
sbit P24 = P2^4;
sbit P25 = P2^5;
sbit P26 = P2^6;
sbit P27 = P2^7;
sbit P30 = P3^0;
sbit P31 = P3^1;
sbit P32 = P3^2;
sbit P33 = P3^3;
sbit P34 = P3^4;
sbit P35 = P3^5;
sbit P36 = P3^6;
sbit P37 = P3^7;

sfr P4   = 0xC0;
sbit P40 = P4^0;
sbit P41 = P4^1;
sbit P42 = P4^2;
sbit P43 = P4^3;
sbit P44 = P4^4;
sbit P45 = P4^5;
sbit P46 = P4^6;
sbit P47 = P4^7;

sfr P5   = 0xC8;
sbit P50 = P5^0;
sbit P51 = P5^1;
sbit P52 = P5^2;
sbit P53 = P5^3;
sbit P54 = P5^4;
sbit P55 = P5^5;
sbit P56 = P5^6;
sbit P57 = P5^7;

sfr P6   = 0xE8;
sbit P60 = P6^0;
sbit P61 = P6^1;
sbit P62 = P6^2;
sbit P63 = P6^3;
sbit P64 = P6^4;
sbit P65 = P6^5;
sbit P66 = P6^6;
sbit P67 = P6^7;

sfr P7   = 0xF8;
sbit P70 = P7^0;
sbit P71 = P7^1;
sbit P72 = P7^2;
sbit P73 = P7^3;
sbit P74 = P7^4;
sbit P75 = P7^5;
sbit P76 = P7^6;
sbit P77 = P7^7;

void delay()
{
    int i, j;

    for (i=0; i<1000; i++)
    for (j=0; j<500; j++);
}


void main()
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;

    P10 = 0;
    delay();
    P10 = 1;
    P11 = 0;
    delay();
    P11 = 1;
    P12 = 0;
    delay();
    P12 = 1;
    P13 = 0;
    delay();
    P13 = 1;
    P14 = 0;
    delay();
    P14 = 1;
    P15 = 0;
    delay();
    P15 = 1;
    P16 = 0;
    delay();
    P16 = 1;
    P17 = 0;
    delay();
    P17 = 1;

//    P30 = 0;          //P3.0/P3.1为仿真口和USB下载口时不可演示
//    delay();
//    P30 = 1;
//    P31 = 0;
//    delay();
//    P31 = 1;
    P32 = 0;
    delay();
    P32 = 1;
    P33 = 0;
    delay();
    P33 = 1;
    P34 = 0;
    delay();
    P34 = 1;
    P35 = 0;
    delay();
    P35 = 1;
    P36 = 0;
    delay();
    P36 = 1;
    P37 = 0;
    delay();
    P37 = 1;

    P20 = 0;
    delay();
    P20 = 1;
    P21 = 0;
    delay();
    P21 = 1;
    P22 = 0;
    delay();
    P22 = 1;
    P23 = 0;
    delay();
    P23 = 1;
    P24 = 0;
    delay();
    P24 = 1;
    P25 = 0;
    delay();
    P25 = 1;
    P26 = 0;
    delay();
    P26 = 1;
    P27 = 0;
    delay();
    P27 = 1;

    P00 = 0;
    delay();
    P00 = 1;
    P01 = 0;
    delay();
    P01 = 1;
    P02 = 0;
    delay();
    P02 = 1;
    P03 = 0;
    delay();
    P03 = 1;
    P04 = 0;
    delay();
    P04 = 1;
    P05 = 0;
    delay();
    P05 = 1;
    P06 = 0;
    delay();
    P06 = 1;
    P07 = 0;
    delay();
    P07 = 1;

    P40 = 0;
    delay();
    P40 = 1;
    P41 = 0;
    delay();
    P41 = 1;
    P42 = 0;
    delay();
    P42 = 1;
    P43 = 0;
    delay();
    P43 = 1;
    P44 = 0;
    delay();
    P44 = 1;
    P45 = 0;
    delay();
    P45 = 1;
    P46 = 0;
    delay();
    P46 = 1;
    P47 = 0;
    delay();
    P47 = 1;

    P50 = 0;
    delay();
    P50 = 1;
    P51 = 0;
    delay();
    P51 = 1;
    P52 = 0;
    delay();
    P52 = 1;
    P53 = 0;
    delay();
    P53 = 1;
    P54 = 0;
    delay();
    P54 = 1;
    P55 = 0;
    delay();
    P55 = 1;
    P56 = 0;
    delay();
    P56 = 1;
    P57 = 0;
    delay();
    P57 = 1;

    P60 = 0;
    delay();
    P60 = 1;
    P61 = 0;
    delay();
    P61 = 1;
    P62 = 0;
    delay();
    P62 = 1;
    P63 = 0;
    delay();
    P63 = 1;
    P64 = 0;
    delay();
    P64 = 1;
    P65 = 0;
    delay();
    P65 = 1;
    P66 = 0;
    delay();
    P66 = 1;
    P67 = 0;
    delay();
    P67 = 1;

    P70 = 0;
    delay();
    P70 = 1;
    P71 = 0;
    delay();
    P71 = 1;
    P72 = 0;
    delay();
    P72 = 1;
    P73 = 0;
    delay();
    P73 = 1;
    P74 = 0;
    delay();
    P74 = 1;
    P75 = 0;
    delay();
    P75 = 1;
    P76 = 0;
    delay();
    P76 = 1;
    P77 = 0;
    delay();
    P77 = 1;

    while (1)
    {
        P0 = 0x00;
        delay();
        P0 = 0xff;

        P1 = 0x00;
        delay();
        P1 = 0xff;

        P3 = 0x03;          //P3.0/P3.1为仿真口和USB下载口时不可演示
        delay();
        P3 = 0xff;

        P2 = 0x00;
        delay();
        P2 = 0xff;

        P4 = 0x00;
        delay();
        P4 = 0xff;

        P5 = 0x00;
        delay();
        P5 = 0xff;

        P6 = 0x00;
        delay();
        P6 = 0xff;

        P7 = 0x00;
        delay();
        P7 = 0xff;
    }
}

