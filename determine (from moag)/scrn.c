
#include	<stdio.h>
#include	<bios.h>
#include	<dos.h>
#include	<alloc.h>
#include	<mem.h>
#include	"moag.h"
#include	"texto.h"

#define ptrincr 2


int	nchrstring(cfptr)
	char far *cfptr;
	{
	register char basechr;
	int	i;
	basechr=*cfptr;
	cfptr+=ptrincr;
	for(i=1;i<128;i++) {
 	  if(basechr!=*cfptr) break;
	  if(i==127) break;
          cfptr+=ptrincr;
	}
	return(i);
	}

int compr(obufptr,fptr)
	unsigned char far *fptr;
	unsigned char far *obufptr;
{
	unsigned char far *tempptr;
	int ngot,nlen,i,iobuf,obuflen;
	register unsigned char thischr;

	nlen=0;
	iobuf=0;
//	fptr=MK_FP(_DS,buf);
	do
	{
	  ngot=nchrstring(fptr);
	  if(ngot>1) {
//		printf("\nNchstr '%c' : %d",*fptr,ngot);
		*(obufptr+iobuf)=(char) ngot;
		iobuf++;
		*(obufptr+iobuf)=*fptr;
		iobuf++;
	        fptr+= ptrincr*ngot; 
		}
	  else
	    {
	      tempptr=obufptr+iobuf;
	      iobuf++;
	      for(i=0;i<128;i++) {
		thischr=*fptr;
//	putch(thischr);
//	printf("%d ",(int) thischr);
		if(i==127) break;
	        if(*(fptr+ptrincr)==thischr) break;
	        *(obufptr+iobuf)=thischr;
	        iobuf++;
                fptr+=ptrincr;
	      }
	      ngot=i;
	      *tempptr=ngot | 0x80;
 //            printf("\nVstring : %d",ngot);
	  }
	  nlen+=ngot;
	}
	while(nlen<(80*25));
	*(obufptr+iobuf)=0;
	obuflen=iobuf;
	return(obuflen);
}



void uncompr(obufptr,fptr)
	unsigned char far *fptr;
	unsigned char far *obufptr;
{
	int  findex;
	unsigned char far *tempptr;
	int ngot,nlen,i,iobuf;
	register unsigned char thischr;

	iobuf=0;
	ngot=0;
	findex=0;
	
	while(((*(obufptr+iobuf)&0x7f) != 0) && (findex<(ptrincr*2000))) {
          ngot=*(obufptr+iobuf);
//	  printf("(T%d)",ngot);
	  iobuf++;
	  if(ngot&0x80) {
            for(i=0;i<(ngot&0x7f);i++) {
              *(fptr+findex)=*(obufptr+iobuf);
	      findex+=ptrincr;
	      iobuf++;
              }
	    }
	  else {
	    thischr=*(obufptr+iobuf);
	    iobuf++;
            for(i=0;i<(ngot&0x7f);i++) {
              *(fptr+findex)=thischr;
	      findex+=ptrincr;
              }
	
	    }
	  }
	return;
}


unsigned char far *savescrn()
{
	unsigned char far *fptr;
	unsigned char far *obufptr;
	unsigned int far *fiptr;
	unsigned char far *page2ptr;
	int i,nbytes,ncbytes;
	

	page2ptr=MK_FP(0xb900,0);
	fptr=MK_FP(0xb800,0);
	nbytes=compr(page2ptr,fptr);
	ncbytes=compr(page2ptr+nbytes+1,fptr+1);
	fiptr=MK_FP(0xb800,0);
	for(i=0;i<2000;i++)
         *(fiptr+i)=0x0720;

	obufptr=farmalloc((long) nbytes+ncbytes+2+2);
		if(obufptr==NULL) {return(NULL); }
        *( (int far *) obufptr)=nbytes;
	_fmemcpy(obufptr+2,page2ptr,nbytes+ncbytes+2);
	return(obufptr);
}

void restscrn(obufptr)
	unsigned char far *obufptr;
{
	unsigned char far *fptr;
	int nbytes;

	if(obufptr == NULL) return;
	nbytes=*( (int far *) obufptr);
	if((nbytes<0) || (nbytes > 2000)) return;
	fptr=MK_FP(0xb800,0);
	uncompr(obufptr+2,fptr);
	uncompr(obufptr+nbytes+2+1,fptr+1);
	farfree(obufptr);
}
	
main()
{
	unsigned char far *obufptr;
	unsigned int far *fiptr;
	int i;
	

	obufptr=savescrn();
	fiptr=MK_FP(0xb800,0);
	for(i=0;i<2000;i++)
         *(fiptr+i)=0x0720;

	printf("\nScreen Compressed into %d bytes",*( (int far *) obufptr));
	getch();

	restscrn(obufptr);

	exit(0);
}
	
