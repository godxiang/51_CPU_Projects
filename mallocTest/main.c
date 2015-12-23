#include <stdlib.h>
#include <STC15W4K32S4.h>
unsigned char *ptr;

void test();

void main(){
	unsigned int i;
	ptr = (unsigned char *)realloc(ptr,sizeof(unsigned char)*100);
	for(i=0;i<101;i++){
		*ptr = 0x32;
		ptr++;
	}
	free(ptr);
	ptr=ptr-100;
	test();
	ptr = NULL;
}



void test(){
	
unsigned char  test[100] ={0};
	unsigned int i;
	
	for(i=0;i<100;i++){
		test[i]=*ptr;
		ptr++;
	}

}