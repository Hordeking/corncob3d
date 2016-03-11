#include	<stdio.h>
#include	<bios.h>
#include	<dos.h>
#include	"moag.h"
#include	"texto.h"

#define	BASESEG	(0xB800)

int	textoinited = 0;
unsigned int	shft_attr;
unsigned int	cur_attr, blk_attr, forecolor, backcolor, isbold;
unsigned int far	*scrnbase;
unsigned int far	*scrnp;

static int	scanstart, scanstop;

void	bioscursor(row, col)
	int	row, col;
{
	union REGS	regs;

	regs.h.ah = 02;
	regs.h.bh = 0;
	regs.h.dh = (unsigned char)row;
	regs.h.dl = (unsigned char)col;
	int86(0x10, &regs, &regs);
}

void	inito() {
	union REGS	regs;
#ifdef	MOAG
	extern void	modfont();
#endif	/* MOAG */

	regs.h.al = (unsigned char)3;
	regs.h.ah = (unsigned char)0;
	int86(0x10, &regs, &regs);

#ifdef	MOAG
	modfont();		/* put medal info stuff into font */
#endif	/* MOAG */

	blk_attr = ((BLACK<<4) | GRAY)<<8;
	scrnbase = (unsigned int far *)MK_FP(BASESEG, 0);
	color(WHITE, BLACK);
	erscrn();

	scrnp = scrnbase;

	regs.h.ah = (unsigned char)3;
	regs.h.bh = (unsigned char)0;
	int86(0x10, &regs, &regs);
	scanstart = regs.h.ch;
	scanstop  = regs.h.cl;
	cursoroff();
	textoinited = 1;
}

void	cursoroff() {
	union REGS	regs;

	regs.h.ah = (unsigned char)1;
	regs.h.ch = (unsigned char)scanstart;
	regs.h.cl = (unsigned char)0x20;
	int86(0x10, &regs, &regs);
}

void	unfont() {
	union REGS	regs;

	regs.h.ah = 0;
;	regs.h.al = 3+0x80;
	regs.h.al = 3;			/* some vga's won't accept high mode */
	int86(0x10, &regs, &regs);	/* bit */
}

void	exito() {
	erscrn();
	setblink(1);
#ifdef	MOAG
	unfont();
#endif
	bioscursor(0, 0);
	cursoron();
	textoinited = 0;
}

void	ncexito(y)
	int	y;
{
#ifdef	MOAG
	unfont();
#endif
	bioscursor(y-1, 0);
	cursoron();
	textoinited = 0;
}

void	cursoron() {
	union REGS	regs;

	regs.h.ah = (unsigned char)1;
	regs.h.ch = (unsigned char)scanstart;
	regs.h.cl = (unsigned char)scanstop;
	int86(0x10, &regs, &regs);
}

void	erscrn() {
	unsigned int	sp;

	sp = shft_attr|' ';
	for (scrnp = scrnbase; scrnp < &scrnbase[NCOL*NROW]; *scrnp++ = sp);
}

void	cursor(x, y)
	int	x, y;
{
	scrnp = scrnbase + (y - 1)*NCOL + x - 1;
}

void	color(fore, back)
	unsigned int	fore, back;
{
	forecolor = fore;
	backcolor = back;
	cur_attr = (back<<4) | fore;
	shft_attr = cur_attr<<8;
	isbold = 0;
}

void	fcolor(fore)
	unsigned int	fore;
{
	forecolor = fore;
	cur_attr = (backcolor<<4) | fore;
	shft_attr = cur_attr<<8;
	isbold = 0;
}

void	bold() {
	if (forecolor != BLUE)
		cur_attr = (backcolor<<4) | forecolor | 010;
	else
		cur_attr = (backcolor<<4) | BRIGHTWHITE | 010;
	shft_attr = cur_attr<<8;
	isbold = 1;
}

void	normal() {
	cur_attr = (backcolor<<4) | forecolor;
	shft_attr = cur_attr<<8;
	isbold = 0;
}

void	inverse() {
	cur_attr = (forecolor<<4) | backcolor;
	shft_attr = cur_attr<<8;
}

void	blink() {
	cur_attr = ((backcolor|0x8)<<4) | forecolor;
	shft_attr = cur_attr<<8;
}

#define	BLACKSPACE	((((BLACK<<4)|WHITE)<<8)|' ')
void	setblink(on) {
	union REGS	regs;
	unsigned int far	*p;
	unsigned int far	*pm;

	if (on) {
		pm = &scrnbase[NCOL*NROW];
		for (p = scrnbase; p < pm; *p++ = BLACKSPACE);
	}
	regs.h.bl = (unsigned char)on;
	regs.x.ax = 0x1003;
	int86(0x10, &regs, &regs);
}

#define	HIGHCOLOR	0xE;
void	highlite() {
	cur_attr = (backcolor<<4) | HIGHCOLOR;
	shft_attr = cur_attr<<8;
}

void	cput(c)
	int	c;
{
	*scrnp++ = shft_attr|c;
}

void	cback() {
	scrnp--;
}

void	cblock() {
	*scrnp &= 0xff;
	*scrnp |= blk_attr;
	scrnp++;
}

void	sput(s)
	char	*s;
{
	while (*s)
		*scrnp++ = shft_attr|(*s++);
}

int	hsput(s, lim)
	char	*s;
{
	char	c;
	int	wasbold, l;
	extern unsigned int	isbold;

	l = 0;
	while (*s) {
		if (l == lim)
			break;
		if (*s == '\\') {
			c = *++s;
			if (c == '\0')
				break;
			else if (c == '\\') {
				cput(c);
				l++;
			}
			else if (c == '(')
				bold();
			else if (c == ')')
				normal();
			else {
				wasbold = isbold;
				highlite();
				cput(c);
				l++;
				if (wasbold)
					bold();
				else
					normal();
			}
			s++;
		}
		else {
			cput(*s++);
			l++;
		}
	}
	return(l);
}

#include	<stdarg.h>
/*VARARGS0*/
int	xyprintf(x, y, cntrl)
	int	x, y;
	char	*cntrl;
{
	va_list	args;
	int	ret;
	char	buf[256];
	extern int	vsprintf();

	va_start(args, cntrl);
	ret = vsprintf(buf, cntrl, args);
	va_end(args);
	cursor(x, y);
	sput(buf);
	return(ret);
}

void	beep() {
#define	FREQ	622		/* D# */
#ifdef	__TURBOC__
	long	now, next;

	now = *(long far *)MK_FP(0x40, 0x6C);
	next = now+1;
	while (*(long far *)MK_FP(0x40, 0x6C) < next)
		if (kbhit())
			break;
	next = now+2;
	sound(FREQ);
	while (*(long far *)MK_FP(0x40, 0x6C) < next) {
		if (kbhit())
			break;
	}
	nosound();
#else
	sound(FREQ, 1);
#endif
/*	putch(7);*/
}

#ifdef	__TURBOC__
int	farmemcpy(p1, p2, n)
	unsigned char far	*p1;
	unsigned char far	*p2;
	int	n;
{
	movedata(FP_SEG(p2), FP_OFF(p2), FP_SEG(p1), FP_OFF(p1), n);
	return(0);
}
#endif	__TURBOC__

#ifdef NEWYANK

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


#ifdef crazyfuckingmind
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

	obufptr=farmalloc((long) (nbytes+ncbytes+2+2));
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
#endif
	
int far	*yankscr() {
	int far	*p;
	int	*pp;
	extern void far	*farmalloc();
	extern void	*malloc();
	extern int	farmemcpy();
	unsigned char far *fptr;
	unsigned char far *obufptr;
	unsigned char *nobufptr;
	unsigned char far *page2ptr;
	int i,nbytes,ncbytes;
	

	page2ptr=MK_FP(0xb900,0);
	fptr=MK_FP(0xb800,0);
	nbytes=compr(page2ptr,fptr);
	ncbytes=compr(page2ptr+nbytes+1,fptr+1);

	obufptr=farmalloc((long)(nbytes+ncbytes+2+2));
	  if(obufptr==NULL) {
	    nobufptr=malloc(nbytes+ncbytes+2+2);
	      if(nobufptr==NULL) {
			cursor(1, 1);
			sput("Warning!!!  Cannot alloc memory for screen yank");
			return(NULL);
		}
	      obufptr = MK_FP(_DS, nobufptr);
            }
        *( (int far *) obufptr)=nbytes;
	_fmemcpy(obufptr+2,page2ptr,nbytes+ncbytes+2);
	return((int far *) obufptr);
}

void putscr(obufptr)
	unsigned int far *obufptr;
{
	unsigned char far *fptr;
	int nbytes;

	if(obufptr == NULL) return;
	nbytes=*obufptr;
	if((nbytes<0) || (nbytes > 2000)) {printf("\nNbytes PANIC!"); exit(137); }
	fptr=MK_FP(0xb800,0);
	uncompr( ((char far *) obufptr)+2,fptr);
	uncompr( ((char far *) obufptr)+nbytes+2+1,fptr+1);
}

void	freescr(p)
	int far	*p;
{
	extern void	free(), farfree();

	if (p == (int far *)0)
		return;
	if ( FP_SEG(p) == (_DS))  /* if near RAM, release it as such */
		free(FP_OFF(p));
	else
		farfree(p);
}
#else
int far	*yankscr() {
	int far	*p;
	int	*pp;
	extern void far	*farmalloc();
	extern void	*malloc();
	extern int	farmemcpy();

	if ((p = (int far *)farmalloc((long)2*(NCOL*NROW+1))) == (int far *)0) {
		if ((pp = (int *)malloc((long)2*(NCOL*NROW+1))) == (int *)0) {
			cursor(1, 1);
			sput("Warning!!!  Cannot alloc memory for screen yank");
			return((int far *)0);
		}
/*
savepos();
xyprintf(1, 1, "Using near heap for screen buffer in yankscr...       ");
beep();mygetch();
restpos();
*/
		pp[NCOL*NROW] = 1;
		p = MK_FP(_DS, pp);
	}
	else
		p[NROW*NCOL] = 0;
	farmemcpy(p, scrnbase, 2*NCOL*NROW);
	return(p);
}

void	putscr(p)
	int far	*p;
{
	extern int	farmemcpy();

	if (p == (int far *)0)
		return;
	farmemcpy(scrnbase, p, 2*NCOL*NROW);
}

void	freescr(p)
	int far	*p;
{
	extern void	free(), farfree();

	if (p == (int far *)0)
		return;
	if (p[NROW*NCOL] == 1)
		free(FP_OFF(p));
	else
		farfree(p);
}
#endif	NEWYANK

static unsigned int far	*scrnpsave;
static unsigned int	sv_shft_attr;
static unsigned int	sv_cur_attr, sv_forecolor, sv_backcolor, sv_isbold;
void	savepos() {
	scrnpsave = scrnp;
	sv_shft_attr = shft_attr;
	sv_cur_attr = cur_attr;
	sv_forecolor = forecolor;
	sv_backcolor = backcolor;
	sv_isbold = isbold;
}

void	restpos() {
	scrnp = scrnpsave;
	shft_attr = sv_shft_attr;
	cur_attr = sv_cur_attr;
	forecolor = sv_forecolor;
	backcolor = sv_backcolor;
	isbold = sv_isbold;
}
