

#include <stdio.h>
#include <bios.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
extern void readfont(); 

modfont() {
	int fp;
	long	size;
	int	ccount,i;
	unsigned char newfont[17],ichar;
	unsigned char *filename="medal.fnc";


	fp=open(filename, O_RDONLY | O_BINARY);
	if(fp == NULL) { printf("Error, can't open file"); exit(137); }

	size=filelength(fp);
	if( (( (int) size % 17) != 0) || (size==0L) )
	  {printf("error, medal.fnc size must be multiple of 17"); exit(137);}


	ccount=size/17;


	for(i=0;i<ccount;i++) {
		read(fp,newfont,17);
		ichar=newfont[16];
		readfont(ichar,newfont);
/* printf("\n%d  %c",(int)  ichar,ichar); */
	}
		
	close(fp);

}

