#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>

main() {
	int textmode;
	char crap[128];
	printf("\nHi there\n");
	textmode=1;
	printf("%d",textmode);
	sprintf(crap,"%d",textmode);
	putchar(*crap);
}
