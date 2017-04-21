#include <stdio.h>
int main(){
	int a = 0, b = 1 ,c;
	for(int i = 0; i< 10 ;i ++){
		a++;
		b++;
		c = a+b;
	}
	printf("a = %d, b = %d, c = %d\n",a,b,c);
	return 0;
}
