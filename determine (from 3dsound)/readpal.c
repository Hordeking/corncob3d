#include <stdio.h>
#include <malloc.h>
#include <io.h>


void *load256pal(char *filename)
{
	int fp;
	unsigned int color;
	long size;
	unsigned char *image;

	fp=open(filename, O_RDONLY | O_BINARY);
	if(fp == NULL) { printf("Error, cannot open file"); exit(0); }
	size=filelength(fp);
	if(size != 768) { printf("Error, palette file must be 768 bytes");
                                     exit(0); }
	image=malloc(size);
	if(image == NULL) { printf("Error, can't alloc RAM"); exit(0); }
	read(fp,image,size);
	close(fp);

		for (color=0; color < 256; color++)
		{
			outportb(0x3C8,color);
			outportb(0x3C9,*(image+(color*3)+0));
			outportb(0x3C9,*(image+(color*3)+1));
			outportb(0x3C9,*(image+(color*3)+2));
		}
/*
*/
	return;
}

