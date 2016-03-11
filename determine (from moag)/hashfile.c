#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <alloc.h>

#define S_IREAD 0x0100  /* owner may read */
#define S_IWRITE 0x0080 /* owner may write */

main()
{
unsigned char *ramptr;
int fhand;
unsigned int i,nbytes;
	ramptr=malloc(14000);
	if(ramptr==NULL) {printf("\nallocfail"); exit(1); }
	fhand=open("drvr.com",O_RDONLY | O_BINARY);
	if(fhand == -1)  {printf("\nopenfail"); exit(1); }
	nbytes=read(fhand,ramptr,14000);
	close(fhand);

	for(i=0;i<nbytes;i++)		/* Scramble  data */
           *(ramptr+i) ^= 0xa5;
	fhand=open("moagdata",
		O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
	if(fhand == -1)  {printf("\nopenfail"); exit(1); }
	write(fhand,ramptr,nbytes);
	close(fhand);
}
