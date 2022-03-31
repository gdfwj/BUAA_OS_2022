/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
//struct my_struct {
//int size;
//char c;
//int array[10];
//};

#include <printf.h>
#include <pmap.h>
void _my_putchar(char);
char _my_getchar();
void _my_exit();
int main()
{
//struct my_struct t1 = {10, 'Q', {0, -1, -2,4,4,4,4,7,8,9}};
//struct mystruct t2 = {2, 'Q', {1, 2}};
//printf("%T",&t1);
//printf("%T",&t2);
//printf("%04T",&t1);
//printf("%-04T",$t2);
	char a = _my_getchar();
	_my_putchar(a);
	printf("main.c:\tmain is start ...\n");
	_my_exit();
	mips_init();
	panic("main is over is error!");

	return 0;
}
