#include	<stdio.h>
#include <malloc.h>
#include <math.h>
#include <io.h>
#include <dos.h>
#include <conio.h>

/* #define FORWARD /**/
/* #define ovlstag /**/
#define NPIECE 300
#define BKGND 64
#define doppmag 6
#define basefreq 0x200
#define fadealpha 16.

extern int PIXEL13(); 
extern	int RANDN();
extern int GRMODE();
extern int MODGRMODE();
extern int TXMODE();
extern	int FILLSCR();
extern	int WRTGLINE();
extern	int FARWRTGLINE();
extern	int MMODFARWRTGLINE();
extern	char *load256pal();
extern  void SCROLLUP();
extern void MERGELINE();
extern	void SETSTARTADR();
extern	void IMAGEMOV();
extern  void WRTPLANE();

extern int LOADSOADL();
extern void VOICEON();
extern void VOICEOFF();
extern int ADLINIT();





	unsigned char	gline[320];
        char far *farptr;
	char far *gptr;
	int ovlxsize, ovlysize;
        int ovlxorg,ovlyorg;
	char far *ovllinesptr[200];
	unsigned int dispaddr,wrtaddr,bkgndaddr;
	struct SREGS sregs;
	int planespeed,backgndspeed;

	struct ovlpiece {
	  int bitplane;
	  unsigned int startadr;
	  int llength;
	  char far *dataptr; };

	struct linestruct {
	  int novlpiece;
	  struct ovlpiece **piecelistptr; };

	struct linestruct lstructs[200];



void swapbuffers()
{
	unsigned int temp;
	temp=dispaddr;
	dispaddr=wrtaddr;
	wrtaddr=temp;
	SETSTARTADR(dispaddr);
}


int shadeogreen(ival)
int ival;
{
	ival += 72;
	if(ival>79) ival=79;
	if(ival<64) ival=64;
	return(ival);
}

void decovlyorg()
{
	ovlyorg -= planespeed;
	if(ovlyorg < -ovlysize-30*planespeed) ovlyorg=210+30*planespeed;
}

void mergedline(nx,strtline,lineptr)
int nx,strtline;
char far *lineptr;
{
	int newsize;
	if( (strtline<ovlyorg) || (strtline>ovlyorg+ovlysize-1) ||
                  (ovlxorg>nx) || (ovlxorg+ovlxsize < 0) )
		MERGELINE(0,nx,0,gline,lineptr,gptr);
          else
           {
	      newsize=ovlxsize;
	      if(ovlxorg+ovlxsize>nx) newsize=nx-ovlxorg;
	      MERGELINE(ovlxorg,nx,newsize,gline,lineptr
                ,ovllinesptr[strtline-ovlyorg]); /**/
           }
	WRTGLINE(nx,strtline*320,gline);
	return;
}

void drawovl(oxorg,oyorg,oxsize,oysize,ovlptr)
int oxorg,oyorg,oxsize,oysize;
char far *ovlptr[200];
{
int nlines,i,wrtlength;
	wrtlength=oxsize;
	if(wrtlength+oxorg>320) wrtlength=320-oxorg;
	if(oyorg<0) 
	{
	  nlines=oyorg+oysize;
	  if(nlines<0) nlines=0;
	  for(i=0;i<nlines;i++)
	  {
		MMODFARWRTGLINE(wrtlength,i*320+oxorg,
		  ovlptr[i-oyorg],wrtaddr);
	  }
	}
	else
	{
	  nlines=200-oyorg;
	  if(nlines>oysize) nlines=oysize;
	  for(i=0;i<nlines;i++)
	  {
		MMODFARWRTGLINE(wrtlength,(i+oyorg)*320+oxorg,
		  ovlptr[i],wrtaddr);
	  }
	}
}


void newdrawovl(oxorg,oyorg,oxsize,oysize,ovlptr)
int oxorg,oyorg,oxsize,oysize;
struct linestruct *ovlptr;
{
struct ovlpiece *ovltemp;
int nlines,i,wrtlength,it,ii;
	wrtlength=oxsize;
	if(wrtlength+oxorg>320) wrtlength=320-oxorg;
	if(oyorg<0) 
	{
	  nlines=oyorg+oysize;
	  if(nlines<0) nlines=0;
	  for(i=0;i<nlines;i++)
	  {
		  it=i-oyorg;
	  for(ii=0;ii<ovlptr[it].novlpiece;ii++) {
		ovltemp=*(ovlptr[it].piecelistptr+ii);
		WRTPLANE(ovltemp->bitplane,
		   ovltemp->startadr+i*80+ovlxorg/4+wrtaddr,
		   ovltemp->llength,ovltemp->dataptr);
		   }
	  }
	}
	else
	{
	  nlines=200-oyorg;
	  if(nlines>oysize) nlines=oysize;
	  for(i=0;i<nlines;i++)
	  {
	  for(ii=0;ii<ovlptr[i].novlpiece;ii++) {
		ovltemp=*(ovlptr[i].piecelistptr+ii);
		WRTPLANE(ovltemp->bitplane,
		   ovltemp->startadr+(i+oyorg)*80+ovlxorg/4+wrtaddr,
		   ovltemp->llength,ovltemp->dataptr);
		   }
	  }
	}
}



void ovlclear(ovlbitptr)
struct ovlpiece *ovlbitptr;
{
		ovlbitptr->bitplane=0;
		ovlbitptr->startadr=0;
		ovlbitptr->llength=0;
		ovlbitptr->dataptr=NULL;
}

struct ovlpiece *runthru(nleft,bplane,glineptr)
int *nleft,bplane;
char *glineptr;
{
	char clrdata[80];
	char far *clrdataptr;
	int  i,ii,iii,ngood,teepee;
	char	temp;
	struct ovlpiece *ovlpptr;

	ngood=0;
	teepee=0;
	clrdataptr=MK_FP(sregs.ds,clrdata);
	ovlpptr=NULL;
	for(i=0;i<*nleft;i++) {
	   temp=*(glineptr+bplane+(4*i));
/*	   printf("%d ",(int) temp);  /**/
	   if( (temp != BKGND) && (ngood>0) ) {
	     clrdata[ii]=temp;
	     ii++;
	   }
	   if( (temp != BKGND) && (ngood == 0) ) {
/*	   printf("Vietnam: %d %d ",i,(int) temp);  /**/
	      clrdata[0]=temp;
 	      ii=1;
	      ngood++;
	      teepee=i;
	   }
	   if( (temp == BKGND) && (ngood>0) ) {
/*	   printf("char: %d %d ",ii,(int) temp);  /**/
	     ovlpptr=malloc(sizeof(*ovlpptr));
	     if(ovlpptr==NULL) {printf("Out of RAM\n"); exit(0); }
	     ovlpptr->bitplane=bplane;
	     ovlpptr->startadr=teepee;
	     ovlpptr->llength=ii;
	     ovlpptr->dataptr=farmalloc(ii);
	     if(ovlpptr->dataptr==NULL) {printf("Out of Mem\n"); exit(0); }
	     farmemcpy(ovlpptr->dataptr,clrdataptr,ii);
	     break;
           }   
	}
        *nleft-= i;
	return(ovlpptr);
}


void changesound(ifreq,iysize)
int ifreq,iysize;
{
  int volume,volcode;
  float fiysize4,fiydist;
  ifreq=(ifreq-iysize/2)/2+iysize/2;
  if(ifreq < 0) VOICEON(0,basefreq+doppmag);
   else
     if (ifreq > iysize) VOICEON(0,basefreq-doppmag);
       else
   	VOICEON(0,basefreq+ 
           (int) ((float) doppmag * cos((float) ifreq*3.14/(float) iysize)));
  
  fiysize4=(float) iysize/4;
  fiydist= (float) iysize/2-ifreq;
  volume=(int) ( fiysize4*64./sqrt( 
          fiydist*fiydist/fadealpha + fiysize4*fiysize4 ));
  volcode=64-volume;
  if(volcode<0) volcode=0;
  if(volcode>63) volcode=63;
  ADLWRT(0x43,volcode);
}

main()
{
	int	ii,i,npts;
	unsigned char palname[80];
	unsigned char	crap[320];
	char far *craptr;
	int *fp,*fpo;
	unsigned int xsize, ysize, vgasize, color;
	int nleft,oldnleft;
	long size;
	unsigned char *image;
	int boring;
	struct ovlpiece *ovlsptr[320];
	struct ovlpiece *ovltemp;
	char far *linesptr[320];
	int tindex;
	char akey;


int exitcode;
unsigned char far *farmemptr;


farmemptr=farmalloc(20000L);
if(farmemptr==NULL) {printf("Error allocating Far RAM for SOADL"); exit(0); }

exitcode= 0xff & LOADSOADL(farmemptr);
if(exitcode != 0) {printf("Error loading SOADL.EXE driver\n"); exit(0);}

ADLINIT();		/* Initialize the sound driver */


ADLWRT(0x40,16);
VOICEON(0,basefreq+doppmag);


	planespeed=9;
	backgndspeed=1;
	dispaddr=0;
	wrtaddr=0x4000;
	bkgndaddr=0x8000;

/*	palname="tcors.pal";  */
	MODGRMODE();  /**/
	load256pal("tcors2.pal"); 


	fp=open("namba.vga", O_RDONLY | O_BINARY);
	if(fp == NULL) { printf("Error, can't open file"); exit(0); }
	fpo=open("digcorsa.vga", O_RDONLY | O_BINARY);
	if(fpo == NULL) { printf("Error, can't open file"); exit(0); }
	size=filelength(fp);
	read(fp,gline,4);


	xsize=gline[0]+gline[1]*256+1;
	ysize=gline[2]+gline[3]*256+1;
	read(fpo,gline,4);
	ovlxsize=gline[0]+gline[1]*256+1;
	ovlysize=gline[2]+gline[3]*256+1;
	npts=320;
	ovlxorg=50;
	ovlyorg=210;
		
	segread(&sregs);
/*	printf("cs: %x  ds: %x  es:%x  ss: %x",sregs.cs,sregs.ds,sregs.es,
          sregs.ss);  /**/
	gptr=MK_FP(sregs.ds,gline);
	craptr=MK_FP(sregs.ds,crap);
	farptr=farmalloc(320L);
	if(farptr == NULL) {printf("mem alloc failed"); exit(0);}
	for(i=0;i<ysize;i++)
          {
	    read(fp,gline,npts);
	linesptr[i]=farmalloc(320L);
	if(linesptr[i] == NULL) {printf("mem alloc failed"); exit(0);}
	   for(ii=0;ii<xsize/4;ii++)
           {
	    farmemcpy(linesptr[i]+ii,gptr+(4*ii),1); /**/
	    farmemcpy(linesptr[i]+ii+(xsize/4),gptr+(4*ii)+1,1); /**/
	    farmemcpy(linesptr[i]+ii+(xsize/2),gptr+(4*ii)+2,1); /**/
	    farmemcpy(linesptr[i]+ii+(3*xsize/4),gptr+(4*ii)+3,1); /**/
	    }
/*            boring=FARWRTGLINE(xsize,i*320,linesptr[i]); /**/
          }

	for(i=0;i<ovlysize;i++)
          {
	    read(fpo,gline,ovlxsize);
	ovllinesptr[i]=farmalloc(320L);
	if(ovllinesptr[i] == NULL) {printf("mem alloc failed"); exit(0);}
#ifdef ovlstag
	   for(ii=0;ii<ovlxsize/4;ii++)
           {
	    farmemcpy(ovllinesptr[i]+ii,gptr+(4*ii),1); /**/
	    farmemcpy(ovllinesptr[i]+ii+(ovlxsize/4),gptr+(4*ii)+1,1); /**/
	    farmemcpy(ovllinesptr[i]+ii+(ovlxsize/2),gptr+(4*ii)+2,1); /**/
	    farmemcpy(ovllinesptr[i]+ii+(3*ovlxsize/4),gptr+(4*ii)+3,1); /**/
	    }
#endif
	    farmemcpy(ovllinesptr[i],gptr,ovlxsize); /**/

	    lstructs[i].novlpiece=0;
	    lstructs[i].piecelistptr=NULL;
/*	    printf("working line: %d  ",i); */
	    for(ii=0;ii<4;ii++)	{	/* cycle through bit planes */
	        nleft=ovlxsize/4;
	    	while(nleft>0) {
		oldnleft=nleft;
		/*	printf(" >%d",nleft);  */
			ovlsptr[lstructs[i].novlpiece]=
			    runthru(&nleft,ii,&gline[(ovlxsize/4-nleft)*4]);
/*		printf("\n"); */
	        	if(ovlsptr[lstructs[i].novlpiece] != NULL)  {
			ovlsptr[lstructs[i].novlpiece]->startadr+=
				(ovlxsize/4-oldnleft);
		  	lstructs[i].novlpiece++;
		}
	    }
	    lstructs[i].piecelistptr=malloc(
			lstructs[i].novlpiece*sizeof(&ovlsptr));
	    if(lstructs[i].piecelistptr==NULL) 
			{printf("Mem unavail\n"); exit(0);}
	    memcpy(lstructs[i].piecelistptr,ovlsptr,
                     lstructs[i].novlpiece*sizeof(&ovlsptr));

	    }
          }
/*
TXMODE();

	for(i=0;i<ovlysize;i++) {
	  printf("Line %d )  Npiece : %d",i,lstructs[i].novlpiece);
	  for(ii=0;ii<lstructs[i].novlpiece;ii++)
	    printf(" %X",*(lstructs[i].piecelistptr)+ii);
	  printf("\n");
	}

exit(0);
*/

/*
	for(i=0;i<ovlysize;i++) {
	  for(ii=0;ii<lstructs[i].novlpiece;ii++) {
		ovltemp=*(lstructs[i].piecelistptr+ii);
		WRTPLANE(ovltemp->bitplane,
		   ovltemp->startadr+(i*80)+ovlxorg/4,
		   ovltemp->llength,ovltemp->dataptr);
		printf("bitp,strtadr,len,ptr %d %d %d %Fp\n",ovltemp->bitplane,
		   ovltemp->startadr ,
		   ovltemp->llength,ovltemp->dataptr);
	  }
	}
getch();
*/

/*
	for(i=0;i<ysize-1;i++)
          {
            boring=FMODFARWRTGLINE(xsize/4,i*80,linesptr[i]);
          }
*/

	for(i=0;i<ysize;i++)
          {
            boring=FMODFARWRTGLINE(xsize/4,i*80+bkgndaddr,linesptr[i]); /**/
          }



/*	for(i=0;i<ovlysize;i++)
          {
            boring=FMODFARWRTGLINE(ovlxsize/4,(ysize+i)*80,ovllinesptr[i]); 
          }
*/


	akey=0;
     while(akey!=0x1b)
       {
	for(ii=0;ii<ysize-1;ii+=backgndspeed)
        {
	  if(akey==0x20) ii=0;
	  swapbuffers();
	  for(i=0;i<ii;i++)
            {
     /*         boring=mergedline(xsize,i,linesptr[ii-i]); /**/
	IMAGEMOV(xsize/4,wrtaddr+(i*80),bkgndaddr+(ii-i)*80);
            }
	  for(i=0;i<ysize-ii;i++)
            {
      /*        boring=mergedline(xsize,ii+i,linesptr[i]); /**/
	IMAGEMOV(xsize/4,wrtaddr+(i+ii)*80,bkgndaddr+i*80);
            }

	 newdrawovl(ovlxorg,ovlyorg,ovlxsize,ovlysize,lstructs);
	 changesound(ysize-ovlyorg,ysize);
	 if(kbhit()) {
	   akey=getch();
	   if(akey==0x1b) break;
	   if(akey==0x2d) { planespeed -= 1; if(planespeed<1) planespeed = 1; }
	   if(akey==0x2b) { planespeed += 1; if(planespeed>150) planespeed = 150; }
	}
	 decovlyorg();
         }
        if(akey != 0x1b)
	for(ii=0;ii<ysize-1;ii+=backgndspeed)
        {
	  if(akey==0x20) ii=0;
	  swapbuffers();
	  for(i=0;i<ii;i++)
            {
       /*       boring=mergedline(xsize,i,linesptr[ysize-ii+i-1]); /**/
	IMAGEMOV(xsize/4,wrtaddr+i*80,bkgndaddr+(ysize-ii+i-1)*80);
            }
	  for(i=0;i<ysize-ii;i++)
            {
        /*      boring=mergedline(xsize,ii+i,linesptr[ysize-i-1]); /**/
	IMAGEMOV(xsize/4,wrtaddr+(i+ii)*80,bkgndaddr+(ysize-i-1)*80);
            }
	 newdrawovl(ovlxorg,ovlyorg,ovlxsize,ovlysize,lstructs);
	 changesound(ysize-ovlyorg,ysize);
	 if(kbhit()) {
	   akey=getch();
	   if(akey==0x1b) break;
	   if(akey==0x2d) { planespeed -= 1; if(planespeed<1) planespeed = 1; }
	   if(akey==0x2b) { planespeed += 1; if(planespeed>150) planespeed = 150; }
	}
	 decovlyorg();
         }
        }

	VOICEOFF(0,0x200); 
	TXMODE();
	close(fp);
}

