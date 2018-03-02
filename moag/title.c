#include	<stdio.h>
#include <alloc.h>
#include <io.h>
#include <dos.h>
#include <conio.h>
#include <bios.h>
#include <fcntl.h>
#include <stdlib.h>

#define TURBOC /**/

/* #define FORWARD /**/
/* #define ovlstag /**/
/* #define STOPBACK /**/
#define NPIECE 300
#define BKGND 0
#define LOGOBKGND 0
#define NAAF 3
#define NKLAF 4
#define doppmag 12
#define basefreq 0x200
#define fadealpha 16.
#define BASEVOL 10
 #define largelogo /**/
#define	TIMESCALE 768
#define nrows 16
#define nextra 3
#define nlprow (8+nextra)
#define fpcflag
/* #define diagflag */
/* #define memdiag */

#ifdef waste
extern int PIXEL13(); 
extern	int RANDN();
extern int GRMODE();
extern	int FILLSCR();
extern	int WRTGLINE();
extern	int FARWRTGLINE();
extern  void SCROLLUP();
extern void MERGELINE();
extern  void DIRTYPLANE();
extern	int MMODFARWRTGLINE();
#endif

extern void MODGRMODE();
extern void TXMODE();
extern	char *load256pal();
extern	void SETSTARTADR();
extern	void IMAGEMOV();
extern  void WRTPLANE();
extern	void FMOD8FARWRTGLINE();
extern	void FMODFARWRTGLINE();
extern	void FADEFARWRTGLINE();
extern	void VDISB();
extern	void VENBL();
extern	void MODFILLSCR();
extern	void FFARPLANECPY();
extern	void CNTLCDISB();

extern int LOADSOADL();
extern void VOICEON();
extern void VOICEOFF();
extern int ADLINIT();




	unsigned char	gline[320];
	char far *gptr;
	int	xsize,ysize;
	int ovlxsize, ovlysize;
        int ovlxorg,ovlyorg;
	int ovl1xsize, ovl1ysize;
        int ovl1xorg,ovl1yorg;
	int logoxsize, logoysize;
        int logoxorg,logoyorg;
	int log1xsize, log1ysize;
        int log1xorg,log1yorg;
	int cdtxsize, cdtysize;
        int cdtxorg,cdtyorg;
	int cdtyoff;
	int backyoff;


	int cldxsize, cldysize;
	int cldyoff;
	unsigned int dispaddr,wrtaddr,bkgndaddr;
	struct SREGS sregs;
	int planespeed,backgndspeed;
	long memalloced;
	int npieces;
	char far *logolines[71];
	char far *log1lines[51];
	char far *cldlines[131];
	char far *linesptr[200];
	char far *creditlines[nrows*nlprow+1];
	char far *videoptr;
	int curpal;
	int paltime;
	int fpp;
	unsigned char smpal[768];
	int	dirtflag;
	unsigned long far *tickptr;
	unsigned long far *ntickptr;
	unsigned long	nticks,startticks,curticks;
	int ncharout;
	int	backframe;
	int	totalmem,freedmem;
	int	tempptrfreed,lptrfreed;
#ifndef TURBOC
	long	fartotalmem,fartotalfreed;
#endif
unsigned char far *farmemptr;
	int	planepasses;
	int	soundflag;

	struct ovlpiece {
	  int bitplane;
	  unsigned int startadr;
	  int llength;
	  char far *dataptr; };

	struct linestruct {
	  int novlpiece;
	  struct ovlpiece **piecelistptr; };

	struct linestruct lstructs[123];
	struct linestruct l1structs[123];
	int startfadeflag,fadeindex;

	struct ovlpiece *ovlsptr[186];

	struct osize { int x; int y; };
	struct osize aasz[NAAF];
	struct osize klasz[NKLAF];

	char *aanameptr[NAAF]={"ssflak1.vga","ssflak2.vga","ssflak3.vga"};
	char *klanameptr[NKLAF]=
		{"skla1.vga","skla2.vga","skla3.vga","skla4.vga"};


	struct linestruct klalstructs1[36];
	struct linestruct klalstructs2[36];
	struct linestruct klalstructs3[36];
	struct linestruct klalstructs4[41];

	struct linestruct aalstructs1[40];
	struct linestruct aalstructs2[40];
	struct linestruct aalstructs3[50];
	struct linestruct *ptraalstructs[NAAF]=
		{aalstructs1,aalstructs2,aalstructs3};
	struct linestruct *ptrklalstructs[NKLAF]=
		{klalstructs1,klalstructs2,klalstructs3,klalstructs4};

	int	aaxorg,aayorg,aai,baseaaxorg,baseaayorg;
	int	aacount;
	int	klaxorg,klayorg,klai;

	int	aaitbl[NAAF+2]={0,1,1,2,2};
	int klavol;
	unsigned char virginflag=1;



#ifdef TURBOC
int _msize(rptr)
	char	*rptr;
	{
	return(0);
	}
#endif

#ifdef	__TURBOC__
/*
int	farmemcpy(p1, p2, n)
	unsigned char far	*p1;
	unsigned char far	*p2;
	int	n;
{
	movedata(FP_SEG(p2), FP_OFF(p2), FP_SEG(p1), FP_OFF(p1), n);
	return(0);
}
*/
#endif	__TURBOC__



#ifdef diag
void	pchar(mychar)
char	mychar;
	{
	int i;
	static int pcol;
	pcol++;
	if(pcol<79) { i=biosprint(0,(int) mychar,0); }
	  else  {
	   	i=biosprint(0,13,0);
	   	i=biosprint(0,10,0);
	   	i=biosprint(0,pchar,0);
		pcol=0;
	  }
	if(pchar==13) pcol=0;
	}
#endif

void far1free(fptr)
char far *fptr;
{
#ifndef TURBOC
	  fartotalfreed+=_fmsize(fptr);
#endif
	farfree(fptr);
}

long	freelines(fptrarr,nlines)
char far *fptrarr[];
int	nlines;
{
	int i;
#ifndef TURBOC
	long shitm;
	shitm=fartotalfreed;
#endif
	for(i=0;i<nlines;i++)
	  if(fptrarr[i] != NULL)
		far1free(fptrarr[i]);
#ifndef TURBOC
	return(fartotalfreed-shitm);
#else
	return(0);
#endif
}

char far *far1malloc(msize)
long	msize;
{
	char far *rptr;
	if(msize == 0) 
		{printf("allocation of zero size attempted\n"); return(NULL);}
	else {
	  rptr=farmalloc(msize);
	if(rptr == NULL) {VENBL(); printf("mem alloc failed %ld\n",msize); return(NULL);}
#ifndef TURBOC
	  fartotalmem+=_fmsize(rptr);
#endif
	}
	return(rptr);
}


char *nearmalloc(msize)
unsigned int msize;
{
	char	*rptr;
	if(msize == 0) 
		{printf("allocation of zero size attempted\n"); return(NULL);}
	else {
	  rptr=malloc(msize);
	if(rptr == NULL) {VENBL(); printf("mem alloc failed\n"); return(NULL);}
	  totalmem+=_msize(rptr);
	}
	return(rptr);
}

unsigned int nearfree(memptr)
char *memptr;
{
	unsigned int i,cursize;
	if(memptr!=NULL) {
#ifndef TURBOC
		i=_msize(memptr);
		cursize=_memavl();
		free(memptr);
		freedmem+=i;
		if(_memavl() != cursize+i+2) {
			printf("\nNearfree failure (prvsize,blksize,cursize) %d,%d,%d",cursize,i,_memavl());
			return(_memavl()-cursize);
		}
	return(i);
#else
		free(memptr);
#endif
	}
	else { printf("Tried to free null pointer\n"); return(0); }
}

void freelstruct(oysize,ovlptr)
int oysize;
struct linestruct *ovlptr;
{
struct ovlpiece *ovltemp;
int i,wrtlength,it,ii,carp;
	  for(i=0;i<oysize;i++)
	  {
		  it=i;
	  for(ii=0;ii<ovlptr[it].novlpiece;ii++) {
		ovltemp=*(ovlptr[it].piecelistptr+ii);
		far1free(ovltemp->dataptr);
		tempptrfreed+= nearfree(ovltemp);
		   }
		if(ovlptr[it].piecelistptr!=NULL) {
#ifndef TURBOC
			wrtlength=_msize(ovlptr[it].piecelistptr);
			if(wrtlength==0) printf("msize of zero ");
			carp=nearfree(ovlptr[it].piecelistptr);
			lptrfreed+= carp;
			if(carp!=wrtlength)
				 printf("free fail %d %d\n",wrtlength,carp);
#else
			carp=nearfree(ovlptr[it].piecelistptr);
#endif
 		}
		else
			wrtlength=0;
	  }
}

void	freeall()
{
	long junk;
	int	i;
	freedmem=0;

#ifndef memdiag
	junk=freelines(logolines,logoysize);
	junk=freelines(log1lines,log1ysize);
	junk=freelines(cldlines,cldysize);
	junk=freelines(linesptr,ysize);
	junk=freelines(creditlines,nrows*nlprow);
#else
	printf("\n");
	printf("Freeing %ld logo bytes\n",freelines(logolines,logoysize));
	printf("Freeing %ld log1 bytes\n",freelines(log1lines,log1ysize));
	printf("Freeing %ld cld  bytes\n",freelines(cldlines,cldysize));
	printf("Freeing %ld line bytes\n",freelines(linesptr,ysize));
	printf("Freeing %ld crd  bytes\n",freelines(creditlines,nrows*nlprow));
#ifndef TURBOC
	printf("Mem released : %ld\n",fartotalfreed);
#endif
#endif
	
	freelstruct(ovlysize,lstructs);
	freelstruct(ovl1ysize,l1structs);
	for(i=0;i<NAAF;i++)
		freelstruct(aasz[i].y,ptraalstructs[i]);
	for(i=0;i<NKLAF;i++)
		freelstruct(klasz[i].y,ptrklalstructs[i]);
	far1free(farmemptr);
}


void	fadeback(backfade,delfade)
int *backfade,delfade;
{
 	*backfade += delfade;
	if(*backfade<0 ) *backfade=0; 
	if(*backfade>paltime*7-1) *backfade = paltime*7-1;
}

void darkenbackpal(factor)
unsigned int factor;
{
	unsigned int color,divisor;
	unsigned char ired1,igrn1,iblue1;

		divisor=paltime*7;
		for (color=0; color < 32; color++)
		{
			ired1=smpal[32*3+(color*3)+0];
			igrn1=smpal[32*3+(color*3)+1];
			iblue1=smpal[32*3+(color*3)+2];

			ired1 = (long) ired1 * (long) factor / divisor;
			igrn1 = (long) igrn1 * (long) factor / divisor;
			iblue1 = (long) iblue1 * (long) factor / divisor;

			outportb(0x3C8,color+256-64);
			outportb(0x3C9,ired1);
			outportb(0x3C9,igrn1);
			outportb(0x3C9,iblue1);
		}
}

void swapbuffers()
{
	unsigned int temp;
	temp=dispaddr;
	dispaddr=wrtaddr;
	wrtaddr=temp;
	SETSTARTADR(dispaddr);
}


void decovlyorg()
{
	ovlyorg -= planespeed;
	if(ovlyorg < -ovlysize-90*planespeed) { ovlyorg=210+90*planespeed;
	   if(virginflag!=0 ) 
		{ startfadeflag=255; virginflag=0; }
	     else
		if(planepasses<2) planepasses++;
		else startfadeflag=255;
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
		   ovltemp->startadr+i*80+oxorg/4+wrtaddr,
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
		   ovltemp->startadr+(i+oyorg)*80+oxorg/4+wrtaddr,
		   ovltemp->llength,ovltemp->dataptr);
		   }
	  }
	}
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
	     ovlpptr=nearmalloc(sizeof(struct ovlpiece));
	     tempptrfreed+=sizeof(struct ovlpiece);
	     if(ovlpptr==NULL) {printf("Out of RAM\n"); exit(0); }
	     ovlpptr->bitplane=bplane;
	     ovlpptr->startadr=teepee;
	     ovlpptr->llength=ii;
	     ovlpptr->dataptr=far1malloc((long) ii);
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
  int fdist;
/*  float fiysize4,fiydist; */
  ifreq=(ifreq-iysize/2)/2+iysize/2;
  if(ifreq < 0) VOICEON(0,basefreq+doppmag);
   else
     if (ifreq > iysize) VOICEON(0,basefreq-doppmag);
       else
   	VOICEON(0,basefreq + doppmag -
           (int) ( 2L * (long) ifreq * (long) doppmag / (long) iysize)); 
           
  
/*  fiysize4=(float) iysize/4;
  fiydist= (float) iysize/2-ifreq;
  volume=(int) ( fiysize4*64./sqrt( 
          fiydist*fiydist/fadealpha + fiysize4*fiysize4 ));
*/
  fdist=(iysize/2)-ifreq;
  if(fdist<0) fdist=-fdist;
  if(fdist<iysize/2) volume=0;
    else if(fdist<(iysize+iysize*2/3)) volume=(fdist-iysize/2)*63/(iysize*2/3);
      else volume=0;

  volcode=volume+BASEVOL;
  if(volcode<0) volcode=0;
  if(volcode>63) volcode=63;
  if(soundflag==0) volcode=63;
  ADLWRT(0x43,volcode);
  ADLWRT(0x44,volcode);
}

/*	I take this out because we now have a assembler equivalent */
/*
farplanecpy(dptr,fptr,nbytes)
char far *dptr;
char far *fptr;
int nbytes;
{
	int 	i,nb4;
	nb4=nbytes/4;
	for(i=0;i<nb4;i++) {
	  *(dptr+i)=*(fptr+4*i);
	  *(dptr+i+nb4)=*(fptr+4*i+1);
	  *(dptr+i+2*nb4)=*(fptr+4*i+2);
	  *(dptr+i+3*nb4)=*(fptr+4*i+3);
	}
}
*/

void	readlstruct(fpo1,lsptr,olxsize,ol1ysize)
int	fpo1;
struct linestruct *lsptr;
int	olxsize,ol1ysize;
{
	int	ii,oldnleft,i,nleft;
	struct	linestruct *l1sptr;
	for(i=0;i<ol1ysize;i++)
          {
	    l1sptr=lsptr+i;
	    read(fpo1,gline,olxsize);
	    l1sptr->novlpiece=0;
	    l1sptr->piecelistptr=NULL;
/*	    printf("working line: %d  ",i); */
	    for(ii=0;ii<4;ii++)	{	/* cycle through bit planes */
	        nleft=olxsize/4;
	    	while(nleft>0) {
		oldnleft=nleft;
		/*	printf(" >%d",nleft);  */
			ovlsptr[l1sptr->novlpiece]=
			    runthru(&nleft,ii,&gline[(olxsize/4-nleft)*4]);
/*		printf("\n"); */
	        	if(ovlsptr[l1sptr->novlpiece] != NULL)  {
			ovlsptr[l1sptr->novlpiece]->startadr+=
				(olxsize/4-oldnleft);
		  	l1sptr->novlpiece++;
			npieces++;
		}
	    }
	    }
	    if(l1sptr->novlpiece != 0) {
	    	l1sptr->piecelistptr=nearmalloc(
			l1sptr->novlpiece*sizeof(struct ovlpiece *));
    	lptrfreed+= _msize(l1sptr->piecelistptr);
	    	if(l1sptr->piecelistptr==NULL) 
			{VENBL(); printf("Mem unavail\n"); exit(0);}
	    		memcpy(l1sptr->piecelistptr,ovlsptr,
                     l1sptr->novlpiece*sizeof(struct ovlpiece *));
		}
		else
	    		l1sptr->piecelistptr=NULL;

          }
	}


void	title()
{
	int	ii,i,icld,icdt;
	unsigned char palname[80];
	unsigned char	crap[320];
	char far *craptr;
	int fp,fpo,fpl,fpl1,fpc,fpo1;
	unsigned int vgasize, color;
	int nleft,oldnleft;
	long size;
	unsigned char *image;
	int tindex;
	char akey;
	int showlogoflag;
	int backfade;



int exitcode;

	printf("\nBefore running, %d near Heap left\n",coreleft());
	getch();


#ifdef memdiag
	printf("Memory available at start: %u\n",_memavl());
#endif
#ifndef TURBOC
	totalmem=0;
	fartotalmem=0L;
#endif
		
	segread(&sregs);
/*	printf("cs: %x  ds: %x  es:%x  ss: %x",sregs.cs,sregs.ds,sregs.es,
          sregs.ss);  /**/
	gptr=MK_FP(sregs.ds,gline);
	craptr=MK_FP(sregs.ds,crap);
	tickptr=MK_FP(0x40,0x6c);		/* Ptr to BIOS ticks */
	ntickptr=MK_FP(sregs.ds,&nticks);	/* Ptr to nticks variable */
	videoptr=MK_FP(0xa000,0);		/* Ptr to video ram */


#ifdef diagflag
	pchar(13);           
#endif
#ifdef diagflag
	pchar(10);
#endif
#ifdef diagflag
	pchar(10);           
#endif
#ifdef diagflag
	pchar(13);           
#endif

/* start keeping track of the time */
	farmemcpy(ntickptr,tickptr,sizeof(long));
	startticks=nticks;

memalloced=0;
farmemptr=far1malloc(20000L);
if(farmemptr==NULL) {printf("Error allocating Far RAM for SOADL"); exit(0); }

exitcode= 0xff & LOADSOADL(farmemptr);
if(exitcode != 0) {printf("Error loading SOADL.EXE driver\n"); exit(0);}

ADLINIT();		/* Initialize the sound driver */
#ifdef memdiag
	printf("Far memory used %ld\n",fartotalmem);
	printf("Memory available after sound init: %u\n",_memavl());
#endif


	planespeed=4;
	backgndspeed=1;
	dispaddr=0;
	wrtaddr=0x4000;
	bkgndaddr=0x8000;
	paltime=16;
	aacount=NAAF+2;
	klai=NKLAF;
	klavol=63;
	soundflag=-1;
	if(paltime<1) paltime=1;
	if(paltime>127) paltime=127;
	curpal=paltime*7-1;
	dirtflag=0;
	backfade=0;

/*	palname="tcors.pal";  */

ADLWRT(0x40,16);

/* Switch to graphics mode and load palette */

	fp=open("nambc.vga", O_RDONLY | O_BINARY);
	if(fp == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
/*	fpo=open("digcorsa.vga", O_RDONLY | O_BINARY); */
	fpo=open("hc6new.vga", O_RDONLY | O_BINARY);
	if(fpo == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
	fpo1=open("flash.vga", O_RDONLY | O_BINARY); 
	if(fpo1 == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
	fpl=open("tlogogr.vga", O_RDONLY | O_BINARY);
	if(fpl == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
	fpl1=open("blogogr.vga", O_RDONLY | O_BINARY);
	if(fpl1 == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
	fpc=open("cloud5s.vga", O_RDONLY | O_BINARY);
	if(fpc == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
	fpp=open("sky.pal", O_RDONLY | O_BINARY);
	if(fpp == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }

	read(fpp,smpal,768);
	close(fpp);
	
	


	size=filelength(fp);
	read(fp,gline,4);


	xsize=gline[0]+gline[1]*256+1;
	ysize=gline[2]+gline[3]*256+1;

	read(fpo,gline,4);
	ovlxsize=gline[0]+gline[1]*256+1;
	ovlysize=gline[2]+gline[3]*256+1;
	ovlxorg=56;
	ovlyorg=210;

	read(fpo1,gline,4);
	ovl1xsize=gline[0]+gline[1]*256+1;
	ovl1ysize=gline[2]+gline[3]*256+1;
	ovl1xorg=50;
	ovl1yorg=210;


	read(fpl,gline,4);
	logoxsize=gline[0]+gline[1]*256+1;
	logoysize=gline[2]+gline[3]*256+1;
	logoxorg=8;
	logoyorg=10;
		
	read(fpl1,gline,4);
	log1xsize=gline[0]+gline[1]*256+1;
	log1ysize=gline[2]+gline[3]*256+1;
	log1xorg=72;
	log1yorg=64;

	read(fpc,gline,4);
	cldxsize=gline[0]+gline[1]*256+1;
	cldysize=gline[2]+gline[3]*256+1;

	for(i=0;i<ysize;i++)
          {
	    read(fp,gline,xsize);
	linesptr[i]=far1malloc(320L);
	if(linesptr[i] == NULL) {VENBL(); printf("mem alloc failed"); exit(0);}
#ifdef fpcflag
	   FFARPLANECPY(linesptr[i],gptr,xsize);
#else
	   for(ii=0;ii<xsize/4;ii++)
           {
	    farmemcpy(linesptr[i]+ii,gptr+(4*ii),1); /**/
	    farmemcpy(linesptr[i]+ii+(xsize/4),gptr+(4*ii)+1,1); /**/
	    farmemcpy(linesptr[i]+ii+(xsize/2),gptr+(4*ii)+2,1); /**/
	    farmemcpy(linesptr[i]+ii+(3*xsize/4),gptr+(4*ii)+3,1); /**/
	    }
#endif
/*            FARWRTGLINE(xsize,i*320,linesptr[i]); /**/
          }
	close(fp);

#ifdef memdiag
	printf("Far memory used linesptr %ld\n",fartotalmem);
#endif

	for(i=0;i<cldysize;i++)
          {
	    read(fpc,gline,cldxsize);

	cldlines[i]=far1malloc((long) cldxsize+ 3L);
	if(cldlines[i] == NULL) {VENBL(); printf("mem alloc failed"); exit(0);}
#ifdef fpcflag
	   FFARPLANECPY(cldlines[i],gptr,cldxsize);
#else
	   for(ii=0;ii<cldxsize/4;ii++)
           {
	    farmemcpy(cldlines[i]+ii,gptr+(4*ii),1); /**/
	    farmemcpy(cldlines[i]+ii+(cldxsize/4),gptr+(4*ii)+1,1); /**/
	    farmemcpy(cldlines[i]+ii+(cldxsize/2),gptr+(4*ii)+2,1); /**/
	    farmemcpy(cldlines[i]+ii+(3*cldxsize/4),gptr+(4*ii)+3,1); /**/
	    }
#endif
	}
	close(fpc);

#ifdef memdiag
	printf("Far memory used cld %ld\n",fartotalmem);
#endif
	
	for(i=0;i<logoysize;i++)
          {
	    read(fpl,gline,logoxsize);

	logolines[i]=far1malloc((long) logoxsize+ 3L);
	if(logolines[i] == NULL) {VENBL(); printf("mem alloc failed"); exit(0);}
#ifdef fpcflag
	   FFARPLANECPY(logolines[i],gptr,logoxsize);
#else
	   for(ii=0;ii<logoxsize/4;ii++)
           {
	    farmemcpy(logolines[i]+ii,gptr+(4*ii),1); /**/
	    farmemcpy(logolines[i]+ii+(logoxsize/4),gptr+(4*ii)+1,1); /**/
	    farmemcpy(logolines[i]+ii+(logoxsize/2),gptr+(4*ii)+2,1); /**/
	    farmemcpy(logolines[i]+ii+(3*logoxsize/4),gptr+(4*ii)+3,1); /**/
	    }
#endif
	}
	close(fpl);
#ifdef memdiag
	printf("Far memory used logo %ld\n",fartotalmem);
#endif

	for(i=0;i<log1ysize;i++)
          {
	    read(fpl1,gline,log1xsize);

	log1lines[i]=far1malloc((long) log1xsize+ 3L);
	if(log1lines[i] == NULL) {VENBL(); printf("mem alloc failed"); exit(0);}
#ifdef fpcflag
	   FFARPLANECPY(log1lines[i],gptr,log1xsize);
#else
	   for(ii=0;ii<log1xsize/4;ii++)
           {
	    farmemcpy(log1lines[i]+ii,gptr+(4*ii),1); /**/
	    farmemcpy(log1lines[i]+ii+(log1xsize/4),gptr+(4*ii)+1,1); /**/
	    farmemcpy(log1lines[i]+ii+(log1xsize/2),gptr+(4*ii)+2,1); /**/
	    farmemcpy(log1lines[i]+ii+(3*log1xsize/4),gptr+(4*ii)+3,1); /**/
	    }
#endif
	}
	close(fpl1);
#ifdef memdiag
	printf("Far memory used log1 %ld\n",fartotalmem);
#endif

	for(i=0;i<ovlysize;i++)
          {
	    read(fpo,gline,ovlxsize);

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
			npieces++;
		}
	    }
	    }
	    if(lstructs[i].novlpiece != 0) {
	    	lstructs[i].piecelistptr=nearmalloc(
			lstructs[i].novlpiece*sizeof(struct ovlpiece *));
/*	    	lptrfreed+= lstructs[i].novlpiece*sizeof(struct ovlpiece *);
*/
    	lptrfreed+= _msize(lstructs[i].piecelistptr);
	    	if(lstructs[i].piecelistptr==NULL) 
			{VENBL(); printf("Mem unavail\n"); exit(0);}
	    	memcpy(lstructs[i].piecelistptr,ovlsptr,
                     lstructs[i].novlpiece*sizeof(struct ovlpiece *));
	 	}
	    else	lstructs[i].piecelistptr=NULL;
          }

	close(fpo);
#ifdef memdiag
	printf("Tempptrfreed, lptrfreed : %d, %d\n",tempptrfreed,lptrfreed);
	printf("Far memory used ovl %ld\n",fartotalmem);
#endif

	readlstruct(fpo1,&l1structs[0],ovl1xsize,ovl1ysize);
	close(fpo1);


	for(i=0;i<NAAF;i++) {
		fpo1=open(aanameptr[i], O_RDONLY | O_BINARY); 
		if(fpo1 == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
		read(fpo1,gline,4);
		aasz[i].x=gline[0]+gline[1]*256+1;
		aasz[i].y=gline[2]+gline[3]*256+1;
		readlstruct(fpo1,ptraalstructs[i],aasz[i].x,aasz[i].y);
		close(fpo1);
	}

	for(i=0;i<NKLAF;i++) {
		fpo1=open(klanameptr[i], O_RDONLY | O_BINARY); 
		if(fpo1 == NULL) { VENBL(); printf("Error, can't open file"); exit(0); }
		read(fpo1,gline,4);
		klasz[i].x=gline[0]+gline[1]*256+1;
		klasz[i].y=gline[2]+gline[3]*256+1;
		readlstruct(fpo1,ptrklalstructs[i],klasz[i].x,klasz[i].y);
		close(fpo1);
	}




#ifdef memdiag
	printf("Tempptrfreed, lptrfreed : %d, %d\n",tempptrfreed,lptrfreed);
	printf("Far memory used ovl1 %ld\n",fartotalmem);
#endif


#ifdef memdiag
	printf("Memory available just before shit : %u\n",_memavl());
	printf("Far memory used %ld\n",fartotalmem);
	getch();
#endif
	
	CNTLCDISB();
	MODGRMODE();  /**/
	VDISB();			/* disable video so user cant see */



#ifndef TURBOC
	poscurs(0,0);
#else
	gotoxy(1,1);
#endif
	printf("\n");
	printf("\n");
	printf("\n");
	printf(" Design and Programming : Kevin Stokes\n");
	printf(" Additional Programming : George Welch\n");
	printf("\n");
	printf("\n");
	printf(" Play testing  :\n");
	printf(" \n");
	printf(" Michael Marable          Carl Schnurr\n");
	printf(" Lawrence Gilberti           Don Burch\n");
	printf(" Bobo Olafsen          Darell Phillips\n");
	printf(" Bob Roberds              Sean Gallaty\n");
	printf(" General Dog Vomit         Thomas Pope\n");
	printf(" Jeff Gardner          Patrick Laverty\n");
	printf("\n");


	for(i=0;i<nrows;i++) {
	  for(ii=0;ii<8;ii++) {
/*		printf(" %d %d %d\n",i,ii,i*nlprow+ii); */
		creditlines[i*nlprow+ii]=far1malloc(320L);
		if(creditlines[i*nlprow+ii] == NULL) {
                   VENBL(); printf("No Mem left %d %d\n",i,ii); exit(0); }
		FFARPLANECPY(creditlines[i*nlprow+ii],
			videoptr+((i*8+ii)*320),320);

	  }
	  for(ii=0;ii<nextra;ii++) {
/*		printf(" %d\n",i*nlprow+8+ii); */
	  	creditlines[i*nlprow+8+ii]=far1malloc(320L);
	  	if(creditlines[i*nlprow+8+ii] == NULL) {
                   printf("No Far Mem left %d\n",i); exit(0); }
	  	FFARPLANECPY(creditlines[i*nlprow+8+ii],videoptr+199*320,320);
	}
	}
		
	cdtxsize=320;
	cdtysize=nrows*nlprow;
	cdtxorg=0;
	cdtyorg=144;    

	MODFILLSCR(0);  /* Blank Scree */
	VENBL();	/* reenable video */


	akey=0;
	while(akey!=0x1b)	{

/* Here is the terrain part */
/* Here is the terrain part */
/* Here is the terrain part */
/* Here is the terrain part */
/* Here is the terrain part */


#ifdef diagflag
	pchar(0x61);
#endif
	MODFILLSCR(0);  /* Blank Scree */
	load256pal("hc6.pal"); 
#ifdef diagflag
	pchar(0x62);
#endif
#ifdef diagflag
	pchar(0x63);
#endif
	startfadeflag=0;
	backfade=0;
#ifdef diagflag
	pchar(0x64);
#endif
	darkenbackpal(backfade);
#ifdef diagflag
	pchar(0x65);
#endif
	ovl1xorg=4;
	ovl1yorg=12;
	backframe=0;
	planepasses=0;

	for(i=0;i<ysize;i++)
          {
            FMODFARWRTGLINE(xsize/4,i*80+bkgndaddr,
		linesptr[i]); /**/
          }
#ifdef diagflag
	pchar(0x66);
#endif


	ADLWRT(0x4b,63);		/* start kla voice off quietly */	
	ADLWRT(0xa3,50);
	ADLWRT(0x45,10);		/* start boom voice off quietly */	
	VOICEON(3,50);
	backyoff=0;
	if(akey!=0x1b)	akey=0;
     while(akey!=0x1b)
       {
        if( (akey != 0x1b) )
        {
	  if(akey==0x20) ii=0;
	backframe++;
#ifdef diagflag
	if(backframe<3) pchar(0x67);
#endif
	  swapbuffers();
#ifdef diagflag
	if(backframe<3) pchar(0x68);
#endif
	  

#ifdef diagflag
	if(backframe<3) pchar(0x69);
#endif
	backyoff-=1;
	for(i=0;i<ysize;i++) {

	  if(backyoff<0) { backyoff=2*ysize-1; }
	  if(backyoff+i<ysize)  icld=backyoff+i;
	  else
	   if(backyoff+i<2*ysize) icld=2*ysize-backyoff-i-1;
	    else 
             if(backyoff<2*ysize)  icld=backyoff+i-2*ysize;
		else { backyoff=0; icld=i; }

	  IMAGEMOV(xsize/4,
		wrtaddr+i*80,
		  bkgndaddr+icld*80);
	}
	}
		if((startfadeflag==0) && (backframe>5)) {
		klai=(klai+1) % NKLAF;
		klayorg=2*(ovlyorg-150)+klasz[klai].y/2;
		if(klayorg<-200) klaxorg=160-klasz[klai].x/2+(rand()>>8)-64;
		if(klaxorg<160) klaxorg++;
		if(klaxorg>160) klaxorg--;

		klavol=klayorg-100;
		if(klavol<0) klavol=-klavol;
		klavol=klavol/20;
		if(klavol>63) klavol=63;
		if(klavol<12) klavol=12;

		ADLWRT(0x4b,klavol);	
		ADLWRT(0xa3,50);
		newdrawovl(klaxorg,klayorg,
			klasz[klai].x,klasz[klai].y,ptrklalstructs[klai]); 
		}

/*	if(akey==0x20) aacount=0; */
	if(aacount>(10+(rand()>>9))) aacount=0;
	if((startfadeflag==0) && (aacount<NAAF+2)) {
		aai=aaitbl[aacount];
		aai=aai % NAAF;
		if(aai==0) {
			baseaaxorg=160+(rand()>>7)-128;
			baseaayorg=100+(rand()>>8)-64;
			VOICEOFF(2,0);
			VOICEON(2,0);
		}
		else
			baseaayorg++;

		aaxorg=baseaaxorg-aasz[aai].x/2;
		aayorg=baseaayorg-aasz[aai].y/2;
		newdrawovl(aaxorg,aayorg,
			aasz[aai].x,aasz[aai].y,ptraalstructs[aai]); 
	if(aai==NAAF-1) akey=0;
	}
	aacount++;

#ifdef diagflag
	if(backframe<3) pchar(0x70);
#endif

#ifdef diagflag
	if(backframe<3) pchar(0x71);
#endif
	if((startfadeflag == 0) && (backfade<paltime*7-1)) {
		fadeback(&backfade,10);
		darkenbackpal(backfade);
	}
#ifdef diagflag
	if(backframe<3) pchar(0x72);
#endif

#ifdef diagflag
	if(backframe<3) pchar(0x73);
#endif
	if((startfadeflag==255) && (backfade>0)) {
		fadeback(&backfade,-10);
		darkenbackpal(backfade);
	}
#ifdef diagflag
	if(backframe<3) pchar(0x74);
#endif


#ifdef diagflag
	if(backframe<3) pchar(0x75);
#endif
        if( (akey != 0x1b) && (backfade==paltime*7-1) ) {
	newdrawovl(ovlxorg,ovlyorg,ovlxsize,ovlysize,lstructs); 
	decovlyorg(); 
#ifdef diagflag
	if(backframe<3) pchar(0x76);
#endif

	if((ovlyorg>-50) && (ovlyorg<160) && (backyoff % 2 == 0)) {
/*		ovl1yorg=ovlyorg+29;
		ovl1xorg=ovlxorg+52;
*/
		ovl1yorg=ovlyorg+29-15+8;
		ovl1xorg=ovlxorg+52-8+8;
		newdrawovl(ovl1xorg,ovl1yorg,ovl1xsize,ovl1ysize,l1structs); 
		ovl1yorg=ovlyorg+31-16+7;
		ovl1xorg=ovlxorg+128-20+8;
		newdrawovl(ovl1xorg,ovl1yorg,ovl1xsize,ovl1ysize,l1structs); 
#ifdef diagflag
	if(backframe<3) pchar(0x77);
#endif
		VOICEOFF(1,0x200);
		VOICEON(1,0x200);
		}
	}
	else ovlyorg=210+30*planespeed;
#ifdef diagflag
	if(backframe<3) pchar(0x78);
#endif



	 changesound((ysize*2/3)-(ovlyorg+(ovlysize/2)),ysize);
#ifdef diagflag
	if(backframe<3) pchar(0x79);
#endif
	 if(kbhit()) {
	   akey=getch();
	   if(akey==0x1b) break;
	    else
	   if(akey==0x2d) { planespeed -= 1; if(planespeed<1) planespeed = 1; }
	    else
	   if(akey==0x2b) { planespeed += 1; if(planespeed>150) planespeed = 150; }
	    else
	   if((akey==0x51) || (akey==0x71)) { soundflag ^= -1; }
	    else
	   if(akey==0x6c) { showlogoflag ^= 0xff; 
		fadeindex=224; startfadeflag=255; akey=0;}
/*		else 
		  if( akey!=0x20 ) akey=0x1b; */
	}
	if((startfadeflag==255) && (backfade==0)) break;
	
	}
	VOICEOFF(3,50);

#ifdef diagflag
	if(backframe<3) pchar(0x80);
#endif
	VOICEOFF(0,basefreq); 

/* Here is the Logo Part */
/* Here is the Logo Part */
/* Here is the Logo Part */
/* Here is the Logo Part */
/* Here is the Logo Part */

#ifdef diagflag
	pchar(0x41);
#endif
	MODFILLSCR(0);  /* Blank Screen */
#ifdef diagflag
	pchar(0x42);
#endif
	load256pal("gradsky.pal"); 
cldyoff=0;
showlogoflag=255;
startfadeflag=-1;
fadeindex=32;
backframe=0;
#ifdef diagflag
	pchar(0x43);
#endif
			outportb(0x3C8,7);
			outportb(0x3C9,0);
			outportb(0x3C9,0);
			outportb(0x3C9,0);



#ifdef diagflag
	pchar(0x44);
#endif
	for(i=0;i<cldysize;i++)
          {
            FMODFARWRTGLINE(cldxsize/4,i*80+bkgndaddr,
		cldlines[i]); /**/
          }
#ifdef diagflag
	pchar(0x45);
#endif

	for(i=0;i<cdtysize;i++)
          {
            FMODFARWRTGLINE(cdtxsize/4,i*80+0xc000,
		creditlines[i]); 
          }
#ifdef diagflag
	pchar(0x46);
#endif




/* set up initial palettes */

     while(akey!=0x1b)
       {
        if( (akey != 0x1b) )
        {
	  if(akey==0x20) ii=0;
	backframe++;
#ifdef diagflag
	if(backframe<3) pchar(0x47);
#endif
	  swapbuffers();
#ifdef diagflag
	if(backframe<3) pchar(0x48);
#endif
	  

	cldyoff-=1;
	for(i=0;i<logoysize;i++) {

	  if(cldyoff<0) { cldyoff=2*cldysize-1; }
	  if(cldyoff+i<cldysize)  icld=cldyoff+i;
	  else
	   if(cldyoff+i<2*cldysize) icld=2*cldysize-cldyoff-i-1;
	    else 
             if(cldyoff<2*cldysize)  icld=cldyoff+i-2*cldysize;
		else { cldyoff=0; icld=i; }

	IMAGEMOV(logoxsize/4,
		wrtaddr+(i+logoyorg)*80 +logoxorg/4,
		 /*  logoxorg/4+ */ bkgndaddr+icld*80);
	if(startfadeflag == 0)
	  FMOD8FARWRTGLINE(logoxsize/4,(logoyorg+i)*80+wrtaddr+logoxorg/4,
		logolines[i]);
	else
	  FADEFARWRTGLINE(logoxsize/4,(logoyorg+i)*80+wrtaddr+logoxorg/4,
		logolines[i],fadeindex); 
	}
#ifdef diagflag
	if(backframe<3) pchar(0x49);
#endif

	for(i=0;i<log1ysize;i++) {
	  ii=i+log1yorg-logoyorg;
	  if(cldyoff<0) cldyoff=2*cldysize-1;
	  if(cldyoff+ii<cldysize) icld=cldyoff+ii;
	  else
	   if(cldyoff+ii<2*cldysize) icld=2*cldysize-cldyoff-ii-1;
	    else 
             if(cldyoff<2*cldysize)  icld=cldyoff+ii-2*cldysize;
		else { cldyoff=0; icld=ii; }

	IMAGEMOV(log1xsize/4,
		wrtaddr+(i+log1yorg)*80 +log1xorg/4 ,
		   log1xorg/4-logoxorg/4 +bkgndaddr+icld*80);
	if(startfadeflag == 0)
	  FMOD8FARWRTGLINE(log1xsize/4,(log1yorg+i)*80+wrtaddr+log1xorg/4,
		log1lines[i]);
	else
	  FADEFARWRTGLINE(log1xsize/4,(log1yorg+i)*80+wrtaddr+log1xorg/4,
		log1lines[i],fadeindex);
	}
#ifdef diagflag
	if(backframe<3) pchar(0x50);
#endif
	

	if(startfadeflag==-1) 
		if(fadeindex<224) {
#ifdef diagflag
	if(backframe<3) pchar(0x51);
#endif

			fadeindex+=32;
			outportb(0x3C8,7);
			outportb(0x3C9,42*fadeindex/224);
			outportb(0x3C9,42*fadeindex/224);
			outportb(0x3C9,42*fadeindex/224);
		}
		else 
			startfadeflag=0;

	if(startfadeflag==255) 
		if(fadeindex>32) {
#ifdef diagflag
	if(backframe<3) pchar(0x52);
#endif

			fadeindex-=32;
			outportb(0x3C8,7);
			outportb(0x3C9,42*fadeindex/224);
			outportb(0x3C9,42*fadeindex/224);
			outportb(0x3C9,42*fadeindex/224);
		}
	else break;	/* Leave loop when faded out */

/*	if(startfadeflag!=0) 
		getch(); */

#ifdef diagflag
	if(backframe<3) pchar(0x53);
#endif
	if(showlogoflag) {
		for(i=0;i<199-cdtyorg;i++) {
	icdt=i+cdtyoff;
	if(icdt>=cdtysize) icdt -= cdtysize;

	IMAGEMOV(cdtxsize/4,
		wrtaddr+(i+cdtyorg)*80+cdtxorg/4,
		   0xc000+icdt*80);
		}
	if((cldyoff % 2)==0) cdtyoff++;
	if(cdtyoff>=cdtysize) { cdtyoff=0; startfadeflag=255; }
	}

	 if(kbhit()) {
	   akey=getch();
	   if(akey==0x1b) break;
	    else
	   if(akey==0x2d) { planespeed -= 1; if(planespeed<1) planespeed = 1; }
	    else
	   if(akey==0x2b) { planespeed += 1; if(planespeed>150) planespeed = 150; }
	    else
	   if((akey==0x51) || (akey==0x71)) { soundflag ^= -1; }
	    else
	   if(akey==0x6c) { showlogoflag ^= 0xff; 
		fadeindex=224; startfadeflag=255; akey=0;}
		else akey=0x1b;
	}

         }

/*	if((startfadeflag==255) && (fadeindex == 32)) break; */
        }
#ifdef diagflag
	pchar(0x54);
#endif

	}
#ifdef diagflag
	pchar(0x55);
#endif
	MODFILLSCR(0);		/* erase old screen */
	VOICEOFF(0,basefreq); 
	TXMODE();
	ADLWRT(0x43,63);		/* Turn off volume */
/*	for(i=0;i<NAAF;i++) printf("\n%s",aanameptr[i]); */
#ifdef memdiag
	printf("Near Memory allocated : %ld\n  Near Memory remaining : %d\n"
		,memalloced,_freect(1));
	printf("Number of pieces : %d\n",npieces);
	printf("\n Logo xsz : %d  Logo ysz : %d shoflag %d\n",
	  logoxsize,logoysize,(int) showlogoflag);
	farmemcpy(ntickptr,tickptr,sizeof(long));
	printf("Elapsed time = %ld\n",nticks-startticks);
	printf("showlogoflag : %d startfadeflag : %d fadeindex : %d\n",
                showlogoflag,startfadeflag,fadeindex);
	printf("Memory available at end before freeall : %u\n",_memavl());
	printf("Tempptrfreed, lptrfreed : %d, %d\n",tempptrfreed,lptrfreed);
#endif

	tempptrfreed=0;
	lptrfreed=0;
#ifndef TURBOC
	fartotalfreed=0L;
#endif

	freeall();

#ifdef memdiag
	printf("Memory available at end : %u\n",_memavl());
	printf("Total memory allocated : %d\n",totalmem);
	printf("Total memory released : %d\n",freedmem);
	printf("Tempptrfreed, lptrfreed : %d, %d\n",tempptrfreed,lptrfreed);
	printf("Far memory freed %ld\n",fartotalfreed);
#endif

#ifndef TURBOC
	if((totalmem-freedmem!=0) || (fartotalmem-fartotalfreed!=0)) {
	printf("\n Memory not released error!\n\n");
	printf("Total near memory allocated : %d\n",totalmem);
	printf("Total near memory released : %d\n",freedmem);
	printf("Total far memory allocated : %ld\n",fartotalmem);
	printf("Far memory freed %ld\n",fartotalfreed);
	printf("type a key to continue");
	getch();
	}
#endif
	return(0);
}

