#include<stdio.h>
int main()
{
	int n;
	scanf("%d",&n);
	int flag=0;
	int a=n%10;
	n=n/10;
	int b=n%10;
	n=n/10;
	int c=n%10;
	n=n/10;
	int d=n%10;
	if(d!=0) flag=(a==d)&&(b==c);
	else if(c!=0) flag=(a==c);
	else if(b!=0) flag=a==b;
	else flag=1;
	if(flag){
		printf("Y");
	}else{
		printf("N");
	}
	return 0;
}
