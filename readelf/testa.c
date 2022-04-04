#include<stdio.h>
int a[22000];
int main(){
	char c[100];
	int b[200]={0};
	int i;
	for(i=0;i<20;i++){
		a[i]=i+'A';
		b[i]=-i;
		if(i<10){
			c[i]=a[i]-'A'+'a';
		}
	}
	for(i=0;i<20;i++){
		printf("%d, %d",a[i],b[i]);
		if(i<10){
			printf("c: %c,\n ",c[i]);
		}
	}
	return 0;
}
