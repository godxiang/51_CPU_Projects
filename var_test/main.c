 #include "stc15w4k32s4.h"
 #include <intrins.h>
 #include <stdlib.h>
// #include <stdio.h>
 //十六进制转换成十进制
 //#define Hex2Int(x)	( (x/10)*10+(x%10) )


 //十进制转换成十六进制
 #define Int2Hex(x)	( (x/10)*16+(x%10) )
 
 void Delay2000ms();
 
 void main(void){
	 unsigned char Data[] =0xff;
	 unsigned int longth = 0x0010;
	 unsigned char *ptr;
	 unsigned char i;
	 	 ptr  = (unsigned char *)malloc(sizeof(unsigned char)*longth);
	 for(i=0;i<10;i++){
		 *ptr = 0xff;
		 ptr++;
	 }
	 ptr= ptr-10;
 for(i=0;i<10;i++){
		 ptr++;
	 }
 	 	 ptr= ptr-10;
		free(ptr);

 for(i=0;i<10;i++){
		 ptr++;
	 }
 }



 void Delay2000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 169;
	j = 24;
	k = 59;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
