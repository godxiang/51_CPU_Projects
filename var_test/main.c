 
 //十六进制转换成十进制
 #define Hex2Int(x)	( (x/16)*10+(x%16) )

 //十进制转换成十六进制
 #define Int2Hex(x)	( (x/10)*16+(x%10) )
 
 
 
 void main(void){
	 unsigned int i;
	 i=Hex2Int(0x0c);
 }