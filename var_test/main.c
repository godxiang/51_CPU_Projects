 
 //ʮ������ת����ʮ����
 #define Hex2Int(x)	( (x/16)*10+(x%16) )

 //ʮ����ת����ʮ������
 #define Int2Hex(x)	( (x/10)*16+(x%10) )
 
 
 
 void main(void){
	 unsigned int i;
	 i=Hex2Int(0x0c);
 }