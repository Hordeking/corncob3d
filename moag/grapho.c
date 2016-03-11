#include	"moag.h"
#include	"texto.h"
#include	<bios.h>
#include	<dos.h>

#define	GRAPHMODE	0x10
#define	TEXTMODE	0x3

#define	BASESEG	(0xB800)

unsigned int	shft_attr;
unsigned int	cur_attr, blk_attr, forecolor, backcolor, isbold;
unsigned int far	*scrnbase;
unsigned int far	*scrnp;
static int	row, col;

static int	scanstart, scanstop;

void	videomode(mode)
	int	mode;
{
	union REGS	regs;

	regs.h.ah = 0;
	regs.h.al = (unsigned char)mode;
	int86(0x10, &regs, &regs);
}

void	bioscursor(row0, col0)
	int	row0, col0;
{
	union REGS	regs;

	regs.h.ah = 02;
	regs.h.bh = 0;
	regs.h.dh = (unsigned char)row0;
	regs.h.dl = (unsigned char)col0;
	int86(0x10, &regs, &regs);
}

void	inito() {
	union REGS	regs;
	extern void far	*farmalloc();
	extern void	exit();

	blk_attr = ((BLACK<<4) | GRAY)<<8;
/*	scrnbase = (unsigned int far *)MK_FP(BASESEG, 0); */
	if ((scrnbase =(unsigned int far *)farmalloc((long)NCOL*NROW*2))==0){
		beep();
		exit(1);
	}
	color(WHITE, BLACK);
	erscrn();

	scrnp = scrnbase;

	regs.h.ah = (unsigned char)3;
	regs.h.bh = (unsigned char)0;
	int86(0x10, &regs, &regs);
	scanstart = regs.h.ch;
	scanstop  = regs.h.cl;
	cursoroff();

	videomode(GRAPHMODE);
	cursor(1, 1);
}

void	cursoroff() {
	union REGS	regs;

return;
	regs.h.ah = (unsigned char)1;
	regs.h.ch = (unsigned char)scanstart;
	regs.h.cl = (unsigned char)0x20;
	int86(0x10, &regs, &regs);
}

void	exito() {
	videomode(TEXTMODE);
	bioscursor(0, 0);
	setblink(1);
	cursoron();
}

void	ncexito(y)
	int	y;
{
	videomode(TEXTMODE);
	bioscursor(y-1, 0);
	cursoron();
}

void	cursoron() {
	union REGS	regs;

return;
	regs.h.ah = (unsigned char)1;
	regs.h.ch = (unsigned char)scanstart;
	regs.h.cl = (unsigned char)scanstop;
	int86(0x10, &regs, &regs);
}

void	erscrn() {
	unsigned int	sp;

	sp = shft_attr|' ';
	for (scrnp = scrnbase; scrnp < &scrnbase[NCOL*NROW]; *scrnp++ = sp);
	cursor(1, 1);
	videomode(GRAPHMODE);
}

void	cursor(x, y)
	int	x, y;
{
	row = y-1;
	col = x-1;
	scrnp = scrnbase + row*NCOL + col;
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

return;
	pm = &scrnbase[NCOL*NROW];
	for (p = scrnbase; p < pm; *p++ = BLACKSPACE);
	regs.h.bl = (unsigned char)on;
	regs.x.ax = 0x1003;
	int86(0x10, &regs, &regs);
}

#define	HIGHCOLOR	0xE;
void	highlite() {
	cur_attr = (backcolor<<4) | HIGHCOLOR;
	shft_attr = cur_attr<<8;
}

void	gput(i)
	unsigned int	i;
{
	union REGS	regs;
	static int	grow = -1, gcol = -1;

	if (grow != row || gcol != col) {
		grow = row;
		gcol = col;
		bioscursor(row, col);
	}

	regs.h.ah = 0xa;
	regs.h.al = (unsigned char)(i&0xff);
	regs.h.bh = 0;
	regs.h.bl = (unsigned char)((i>>8)&0xff);
	regs.x.cx = 1;
	int86(0x10, &regs, &regs);
}

void	cput(c)
	int	c;
{
	unsigned int	i;
	extern void	cforw();

	i = shft_attr|c;
	if (i != *scrnp) {
		gput(i);
		*scrnp = i;
	}
	cforw();
}

void	cforw() {
	if (col == (NCOL-1)) {
		col = 0;
		if (row == (NROW-1))
			row = 0;
		else
			row++;
	}
	else
		col++;
	scrnp++;
}

void	cback() {
	scrnp--;
	if (col == 0) {
		col = (NCOL-1);
		row--;
		if (row < 0)
			row = (NROW-1);
	}
	else
		col--;
}

void	cblock() {
	*scrnp &= 0xff;
	*scrnp |= blk_attr;
	gput(*scrnp);
	cforw();
}

void	sput(s)
	char	*s;
{
	unsigned int	i;

	while (*s) {
		i = shft_attr|(*s++);
		if (i != *scrnp) {
			gput(i);
			*scrnp = i;
		}
		cforw();
	}
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

int far	*yankscr() {
	int far	*p;
	int	*pp;
	extern void far	*farmalloc();
	extern void	*malloc();
	extern int	farmemcpy();

	if ((p = (int far *)farmalloc((long)2*NCOL*NROW)) == (int far *)0) {
		if ((pp = (int *)malloc((long)2*NCOL*NROW)) == (int *)0) {
			cursor(1, 1);
			sput("Warning!!!  Cannot alloc memory for screen yank");
			return((int far *)0);
		}
		p = MK_FP(_DS, pp);
	}
	farmemcpy(p, scrnbase, 2*NCOL*NROW);
	return(p);
}

void	putscr(p)
	int far	*p;
{
	int	i;

	if (p == (int far *)0)
		return;
	cursor(1, 1);
	for (i = 0; i < NROW*NCOL; i++) {
		if (*p != *scrnp) {
			gput(*p);
			*scrnp = *p;
		}
		p++;
		cforw();
	}
}

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
