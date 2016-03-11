#define hashfile 1 /**/
#define S_IREAD 0x0100  /* owner may read */
#define S_IWRITE 0x0080 /* owner may write */

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <alloc.h>
#include <dos.h>
#include	<process.h>

 void interrupt (*oldint8)();
void interrupt (*oldint65)();
void interrupt (*newint8)(); /**/
/* void far *oldint8;
void far *oldint65;
void far *newint8; /**/
unsigned int driverseg;

#ifdef hashfile
#define chunksize 64

void unhashfile()
{
register int ii;
int fhandi,fhando;
int i;
int nbytes;
int nparas;
char buf[chunksize];
char far *fptr;
char far *fptr16;


	fptr=farmalloc(14000L);
		if(farmalloc==NULL) {printf("\nnofmem err"); exit(137);}
	fhandi=open("moagdata",O_RDONLY | O_BINARY);
	fhando=open("drvr.exe",
		O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
	if((fhandi == -1) || (fhando==-1))  {errorbox("\nopenfail"); exit(137); }

	nparas=0;
	for(i=0;i<14000/chunksize;i++) {		/* Scramble  data */
		nbytes=read(fhandi,buf,chunksize);
		nparas++;
		if(nbytes<0) {printf("\nfatal read err"); exit(137);}
		if(nbytes<chunksize) break;
		fptr16=fptr+(i*chunksize);
		for(ii=0;ii<chunksize;ii++)
		*(fptr16+ii)=buf[ii] ^ 0xa5;
	}
	fptr16=MK_FP(_DS,buf);
	for(i=0;i<nparas;i++) {		/* Scramble  data */
		farmemcpy(fptr16,fptr+(i*chunksize),chunksize);
		write(fhando,buf,chunksize);
	}
	close(fhandi);
	close(fhando);
	farfree(fptr);
}
#endif

int loadsound()
{
	int retval;
	char buf[80];
	void far *dummyptr;
	oldint8 = getvect(8);
	oldint65 = getvect(0x65);
#ifdef hashfile
	unhashfile();
#endif
	/* now alloc a whole lot of far mem space so that the resident */
	/* TSR won't cramp our style */

	/* Give the driver some space to breathe */

	dummyptr = farmalloc(farcoreleft() - 40000L);
           if(dummyptr==NULL) errorbox("Impos farmalloc fail");

	/* now load the damn driver */
	retval=0xff & spawnl(P_WAIT, "drvr.exe", NULL);

	/* and release all the play space */
	if(dummyptr != NULL) farfree(dummyptr);

#ifdef hashfile
	unlink("drvr.exe");
#endif
//	return(0);
	if(!retval)
		{
			newint8=getvect(8);
			driverseg=FP_SEG( (void far *) newint8);
			if(FP_SEG((void far *) oldint8) == driverseg) {
//				sprintf(buf,"Vector not changed! %d",driverseg);
//				errorbox(buf);
				return(-1);
			}
		}
	return(retval);
}

int unloadsound()
{
	extern int freeup();
	char buf[80];
	int ndriver;
	_dos_setvect(8,*oldint8);
//	return(0);
//	sprintf(buf,"Pres vec: %Fp  Old vec : %Fp",(void far *)getvect(0x65),
//		(void far *)	oldint65);
//	errorbox(buf);
	setvect(0x65,oldint65);	/* unhook ints */
/* free the ram */
//	if(_dos_freemem(driverseg)) {
//		errorbox("Couldn't free driver ram");
//		return(-1);
//	}
	ndriver=freeup(driverseg);
	if(ndriver == -1) {
		errorbox("freeup reports memory messed up");
		}
	return(0);
}


