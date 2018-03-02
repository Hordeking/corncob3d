
#include <alloc.h>
#include <io.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define NSTEPS 16

extern int GRMODE();
extern void TXMODE();
extern  void WRTPLANE();

void readfilepal(frp,palptr)
	int	frp;
unsigned char *palptr;
{
	unsigned int color;
	long size;


	/*
	size=filelength(frp);
	if(size < 768) { myprintf("Error, palette file must be 768 bytes");
                                     exit(0); }
         */
	lseek(frp,-768L,SEEK_END);
	read(frp,palptr,768);
	lseek(frp,0L,SEEK_SET);
/*
*/
	return;
}


int fadeup(origpal)
unsigned char origpal[256];
{
	extern void fadepal();
	int	i;
	for(i=0;i<NSTEPS;i++) {
	 fadepal(origpal,i);
	 delay(400/NSTEPS);
	 if(kbhit()) return(-1);
	}
	fadepal(origpal,NSTEPS);
	return(0);
}

int fadedown(origpal)
unsigned char origpal[256];
{
	extern void fadepal();
	int	i;
	for(i=0;i<NSTEPS;i++) {
	 fadepal(origpal,NSTEPS-i-1);
	 delay(400/NSTEPS);
	 if(kbhit()) return(-1);
	}
	return(0);
}


void fadepal(origpal,factor)
unsigned char origpal[256];
unsigned int factor;
{
	unsigned int color,divisor;
	unsigned char ired1,igrn1,iblue1;

		divisor=NSTEPS;
		for (color=0; color < 256; color++)
		{
			ired1=origpal[(color*3)+0];
			igrn1=origpal[(color*3)+1];
			iblue1=origpal[(color*3)+2];

			if(factor!=divisor) {
			  ired1 = (long) ired1 * (long) factor / divisor;
			  igrn1 = (long) igrn1 * (long) factor / divisor;
			  iblue1 = (long) iblue1 * (long) factor / divisor;
			}

			outportb(0x3C8,color);
			outportb(0x3C9,ired1);
			outportb(0x3C9,igrn1);
			outportb(0x3C9,iblue1);
		}
}





void showpics() {

	extern	void mygprintf();
	extern	int mygetch();
	int vhandle,vxorg,vyorg,i,vxsize,vysize;
	unsigned char tnambcpal[768];
	unsigned char gline[256];
	int picnum;
	int textcolor;
	int numlines;
	char	vbuf[40];
	FILE *fpict;

	if ((fpict = fopen("mpict.dat", "rb")) != NULL) {

	fgets(vbuf,39,fpict);
	vxorg=atoi(vbuf);

	while(vxorg >= 0) {
	fgets(vbuf,39,fpict);
	vyorg=atoi(vbuf);

	fgets(vbuf,39,fpict);

	if(strlen(vbuf)>2) vbuf[strlen(vbuf)-2]=0;  /* remove cr character */
	GRMODE();

	vhandle=open(vbuf, O_RDONLY | O_BINARY);
	if(vhandle >0) { 
	readfilepal(vhandle,tnambcpal);
//	setpal(tnambcpal);

	fadepal(tnambcpal,0);

	read(vhandle,gline,4);
	vxsize=gline[0]+gline[1]*256+1;
	vysize=gline[2]+gline[3]*256+1;


	for(i=0;i<vysize;i++) {
	    read(vhandle,gline,vxsize);
            WRTGLINE(vxsize,320*(i+vyorg)+vxorg, gline);
	}
	close(vhandle);
	}

	fgets(vbuf,39,fpict);
	numlines=atoi(vbuf);

	fgets(vbuf,39,fpict);
	textcolor=atoi(vbuf);

	for(i=0;i<numlines;i++) {

	fgets(vbuf,39,fpict);
	vxorg=atoi(vbuf);

	fgets(vbuf,39,fpict);
	vyorg=atoi(vbuf);

	fgets(vbuf,39,fpict);

	if(strlen(vbuf)>2) vbuf[strlen(vbuf)-2]=0;  /* remove cr character */
	mygprintf(vbuf,vxorg,vyorg,textcolor);

	}

	fadeup(tnambcpal);
	mygetch();
	fadedown(tnambcpal);
	fgets(vbuf,39,fpict);
	vxorg=atoi(vbuf);

	}
	fclose(fpict);

	}
	TXMODE();
}

void	poscurs(int txorg, int tyorg) {
	union REGS	regs;

	regs.h.ah = 02;
	regs.h.bh = 0;
	regs.h.dh = (unsigned char)tyorg;
	regs.h.dl = (unsigned char)txorg;
	int86(0x10, &regs, &regs);
}

void	mygprintf(char st[],int txorg,int tyorg,int tcolor) {
	union REGS	regs;
	int i;
	extern void poscurs();

	for(i=0;i<strlen(st);i++) {
	  poscurs(txorg+i,tyorg);
	  regs.h.ah = 0x9;
	  regs.h.al = st[i];
	  regs.h.bh = 0;
	  regs.h.bl = (unsigned char) tcolor;
	  regs.h.ch = (unsigned char)0;
	  regs.h.cl = (unsigned char)1;
	  int86(0x10, &regs, &regs);
	}
}


int	mygetch() {
	union REGS	regs;
	int	c;

	regs.h.ah = 0x8;
	int86(0x21, &regs, &regs);
	c = regs.h.al;
	return(c);
}

	main() {
	showpics();
	}
