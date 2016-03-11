#define	NEWSTUFF	1

#include	<stdio.h>
#include	<string.h>
#include	<bios.h>
#include	<dos.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

static int	widx, widy, xs, ys, efore, eback, ewhflag;
static int	nelm;
#ifdef	TOWER
static char	**epp;
#else
static char	far * far *epp;
#endif

struct mesg {
	char	*mesgs;
	char	*mesgl;
} messages[] = {
#ifdef	TOWER
{
	/*	Called by tower during flight...  */
"Type <CR> to return to the game...",
"Type \030, \031, PGUP, PGDN,   <CR> to select,   <esc> to cancel",
},
{
	/*	Called by tu for detailed info screens... */
"Type <CR> to return...",
"Type \030\031 <P-UP> <P-DN> etc, or <CR> to return...",
},
#else
{
"",
"",
},
{
	/*	Called by moag with tower info..., `n' for new tower */
"Type <CR> to continue, <ESC> aborts, or `n' for new tower, `i' for info",
"Type \030\031 etc..., <CR> to fly, <ESC> aborts, `n' for new tower, `i' for info",
},
{
	/*	Called by moag with tower info..., no `n'  */
"Type <CR> to continue, <ESC> to abort",
"Type \030 \031 etc..., <CR> to continue, <ESC> to abort",
},
{
	/*	Called by moag for moag.inf...  */
"Type <CR> or <ESC> to exit...",
"Type \030 \031 <P-UP> <P-DN> etc..., <CR> or <ESC> to exit",
},
{
	/*	Called by moag for corncob.doc, 1'st screen...  */
"Choose a chapter number for more info, <CR> to continue or <ESC> to exit",
"\030\031 PGDN PGUP etc,  Type a chapter number,  <CR> next chap,  <ESC> exits",
},
{
	/*	Called by moag for corncob.doc, chapter screens... */
"Type b for beginning,         <CR> next chapter,       <ESC> to exit",
"Type \030\031 PGUP PGDN,   b for beginning,   <CR> to continue,  <ESC> exits",
},
{
	/*	Called by moag for detailed info screens... */
"Type <CR> to return...",
"Type \030\031 <P-UP> <P-DN> etc, or <CR> to return...",
},
#endif
};

char	*mesgs, *mesgl;

int	fileout(file, no, x, y, xwid, ywid, fore, back, whflag)
	char	*file;
	int	no, x, y, xwid, ywid, fore, back, whflag;
{
	FILE	*fp;
	long	here, amt;
#ifdef	TOWER
	char	*pbuf, *pb;
#else
	char far	*pbuf;
	char far	*pb;
#endif
	int	i, c;
	char	buf[LINELEN], *p;

	extern int	scroll();
	extern char	*fngets();
#ifdef	TOWER
	extern void	free(), *malloc();
#else
	extern void	farfree();
	extern void far	*farmalloc();
#endif

	ywid--;
	xwid--;
	widx = xwid;
	widy = ywid;
	xs = x;
	ys = y;
	efore = fore;
	eback = back;
	ewhflag = whflag;
	mesgs = messages[whflag-1].mesgs;
	mesgl = messages[whflag-1].mesgl;

	fp = fopen(file, "rb");
	if (fp == NULL) {
		x += 3;
		y++;
		xyprintf(x, y++, "Error:  Cannot open %s for reading.", file);
		x++;
		y++;
		tkyo(x, y);
		return(0);
	}

	for (i = 0; i < no; i++) {
		while (fgets(buf, LINELEN, fp) != NULL) {
			if (buf[0] == '\014') {
				break;
			}
		}
	}

	here = ftell(fp);
	nelm = 0;
	while (fgets(buf, LINELEN, fp) != NULL) {
		if (buf[0] == '\014')
			break;
		else
			nelm++;
	}
	if (nelm == 0) {
		if (whflag == 6) {
			fclose(fp);
			return('b');
		}
		xyprintf(x, y++,
			"There is no information to print here, number=%d",
								nelm, no);
		x++;
		y++;
		fclose(fp);
		tkyo(x, y);
		return(1);
	}
	epp = NULL;
#ifdef	TOWER
	if ((epp = (char **)malloc(nelm*sizeof(char *))) == NULL)
#else
	if ((epp = (char far * far *)farmalloc((unsigned long)nelm*
						sizeof(char far *))) == NULL)
#endif
	{
		xyprintf(x, y++, "Error:  Cannot alloc %d pointers.", nelm);
		x++;
		y++;
		fclose(fp);
		tkyo(x, y);
		return(0);
	}

	i = 0;
	amt = ftell(fp) - here + nelm + 1;
#ifdef	TOWER
	if (amt > 32767L) {
		xyprintf(x, y++, "Error:  Cannot alloc %ld bytes.", amt);
		x++;
		y++;
		fclose(fp);
		tkyo(x, y);
		return(0);
	}
	if ((pbuf = (char *)malloc((unsigned int)amt*sizeof(char))) == NULL)
#else
	if ((pbuf = (char far *)farmalloc((unsigned long)amt*
					sizeof(char))) == NULL)
#endif
	{
		xyprintf(x, y++, "Error:  Cannot alloc %ld bytes.", amt);
		x++;
		y++;
		fclose(fp);
		tkyo(x, y);
		return(0);
	}
	fseek(fp, here, SEEK_SET);
	pb = pbuf;
	while (fngets(buf, LINELEN, fp) != NULL) {
		if (buf[0] == '\014')
			break;
		epp[i] = pb;
		for (p = buf; *p; *pb++ = *p++);
		*pb++ = '\0';
		if (i++ == nelm) {
			beep();beep();beep();beep();
		}
	}
	fclose(fp);
	c = scroll();
#ifdef	TOWER
	free(pbuf);
	free(epp);
#else
	farfree(pbuf);
	farfree(epp);
#endif
	epp = NULL;
	return(c);
}

char	*fngets(s, n, f)
	char	*s;
	int	n;
	FILE	*f;
{
	char	*p;
	int	i;

	*s = '\0';
	p = fgets(s, n, f);
	i = strlen(s);
	if (i != 0) {
		if (s[i-1] == '\r' || s[i-1] == '\n')
			s[i-1] = '\0';
		if (s[i-2] == '\r' || s[i-2] == '\n')
			s[i-2] = '\0';
	}
	return(p);
}

static int	wflag;

static int	scroll() {
	int	start, maxstart, c, uwid, pwid;
	char	buf[LINELEN];
#ifndef	TOWER
	int far	*scr;
	int	y, nflag, i, dog;
	char	*p;
	extern char	*nget();
	extern int	noplanes, noobjectives, nhobjectives, newplane, atoi();
	extern struct theatert	curtheater;
#endif
	extern void	out(), lineout(), center();

	start = 0;
	uwid = (widy + 1)/2;
	pwid = widy - 2;
	if (pwid < 1)
		pwid = 1;
	if (nelm > widy)
		maxstart = nelm - widy;
	else
		maxstart = 0;
	if (efore == BRIGHTWHITE)
		color(BRIGHTWHITE, BLUE);
	else
		color(BRIGHTWHITE,BLACK);
	cursor(xs, ys+widy);
	if (nelm > widy) {
		center(buf, widx+1, mesgl);
		wflag = 0;
	}
	else {
		center(buf, widx+1, mesgs);
		wflag = 1;
	}
	sput(buf);
	color(efore, eback);
	while (1) {
		out(start);
#ifndef	TOWER
#ifdef	NEWSTUFF
		c = spgetch();
#else	/* !NEWSTUFF */
		nflag = 0;
		c = 0;
		if (noobjectives) {
			scr = yankscr();
			savepos();
			y = 10;
			simplesetup(15, y++, 50, 7, "Theater is Complete",
							MAGENTA, WHITE, RED);
			beep();
			xyprintf(17, y++,
				"There are no mission objectives remaining in");
			xyprintf(17, y++, "this Theater of Operations.");
			y++;
			xyprintf(21, y++, "Type <ESC> to abort,");
			xyprintf(22, y++,
				"or almost anything else to continue...");
			c = spgetch();
			putscr(scr);
			freescr(scr);
			restpos();
			noobjectives = 0;
			if (c != 033 && !noplanes)
				c = spgetch();
			nflag = 1;
		}
		else if (nhobjectives) {
			scr = yankscr();
			savepos();
			y = 12;
			simplesetup(4, y++, 72, 6,
			 "No Objectives at this Airfield", MAGENTA, WHITE, RED);
			beep();
			xyprintf(5, y++,
     "Warning! There are no uncompleted mission objectives at this airstrip.");
			y++;
			xyprintf(16, y++,
			     "Type 'N' to select a different airstrip.");
			xyprintf(12, y++,
		"Type <ESC> to abort, or almost anything else to continue...");

			c = spgetch();
			putscr(scr);
			freescr(scr);
			restpos();
			nhobjectives = 0;
			if ((c|040) != 'n' && (c|040) != 'i' && c != 033
								&& !noplanes)
				c = spgetch();
			nflag = 1;
		}
		if ((c|040) != 'n' && (c|040) != 'i' && c != 033 && noplanes) {
			scr = yankscr();
			savepos();
			y = 12;
			simplesetup(9, y++, 62,
				   ((curtheater.nstoredplanes) ? 8 : 6),
			     "No Planes at this Airfield", MAGENTA, WHITE, RED);
			beep();
			xyprintf(12, y++,
		"Warning!  There are no planes available at this airstrip.");
			if (curtheater.nstoredplanes != 0)
				xyprintf(12, y++,
		"        (However, you have %d plane%s in storage.)",
				      curtheater.nstoredplanes,
				    (curtheater.nstoredplanes == 1) ? "" : "s");
			y++;
			if (curtheater.nstoredplanes > 0)
				xyprintf(13, y++,
		"        Type 'P' to pull a new plane from storage.");
			xyprintf(13, y++,
		"         Type 'N' to select a different airstrip.");
			xyprintf(11, y++,
		"Type <ESC> to abort, or almost anything else to continue...");

			c = spgetch();
			putscr(scr);
			freescr(scr);
			restpos();
			noplanes = 0;
			if (curtheater.nstoredplanes > 0 && (c|040) == 'p')
				newplane = 1;
			if ((c|040) != 'n' && (c|040) != 'i' && c != 033)
				c = spgetch();
			nflag = 1;
		}
		if (!nflag)
			c = spgetch();
#endif	/* !NEWSTUFF */
		if (ewhflag == 5 && c >= '0' && c <= '9') {
			cursor(xs, ys+widy);
			if (efore == BRIGHTWHITE)
				color(BRIGHTWHITE, BLUE);
			else
				color(eback, efore);
			p = "   Enter a Chapter number:  ";
			sput(p);
			dog = strlen(p);
			for (i = dog; i <= widx; i++)
				cput(' ');
			cursor(xs + dog, ys+widy);
			p = nget(3, c);
			if (p == NULL) {
				beep();
				cursor(xs, ys+widy);
				sput(buf);
				color(efore, eback);
				c = spgetch();
			}
			else {
				cursor(xs, ys+widy);
				color(efore, eback);
				lineout((char far *)"");
				cput(' ');
				return(atoi(p));
			}
		}
#else
		c = spgetch();
#endif
		switch (c) {
#ifndef	TOWER
		case 'b':
		case 'B':
		case 'n':
		case 'N':
		case 'i':
		case 'I':
#endif
		case 033:
		case 010:
		case 'q':
		case '\n':
			cursor(xs, ys+widy);
#ifdef	TOWER
			lineout("");
#else
			lineout((char far *)"");
#endif
			cput(' ');
			if (c == '\n')
				return(1);
			else if ((c|040)=='n' || (c|040)=='i' || (c|040)=='b')
				return(c);
			else
				return(0);
		case RIGHTARROW:
		case DOWNARROW:
		case 'j':
		case '2':
		case '6':
			start++;
			if (start > maxstart) {
				beep();
				start = maxstart;
			}
			break;
		case PAGEDOWN:
		case ' ':
		case 'd':
			if (start == maxstart) {
				beep();
				continue;
			}
			if (c == PAGEDOWN || c == ' ')
				start += pwid;
			else /* c == 'd' */
				start += uwid;
			if (start > maxstart)
				start = maxstart;
			break;
		case UPARROW:
		case LEFTARROW:
		case 'k':
		case '8':
		case '4':
			start--;
			if (start < 0) {
				beep();
				start = 0;
			}
			break;
		case PAGEUP:
		case 'u':
			if (start == 0) {
				beep();
				continue;
			}
			if (c == PAGEUP)
				start -= pwid;
			else /* c == 'u' */
				start -= uwid;
			if (start < 0)
				start = 0;
			break;
		case HOME:
		case 'g':
			start = 0;
			break;
		case END:
		case 'G':
			start = maxstart;
			break;
		default:
			beep();
		}
	}
#ifndef	__TURBOC__
	return(0);
#endif
}

#ifdef startloc
int startckline(char *startstring) 
{
	extern char slocarr[];
	if(0==strncmp(startstring,"Sxyth ",6)) {
	  strcpy(slocarr,startstring+6);
	  return(0);
	}
	else
	  return(1);
}
#endif

static void	out(start)
	int	start;
{
	int	y, i, ii, ioff, sstart, sstop;
	extern void	lineout();

#ifdef startloc
	int outvirgin;
	char tbuf[82];
	extern char slocarr[];
#ifndef TOWER
	char far *tbufptr;

	tbufptr = MK_FP(_DS, tbuf);
#endif
	outvirgin=1;
	strcpy(slocarr,"");
#endif

	sstart = start*widy/nelm;
	sstop = sstart + widy*widy/nelm;
	y = ys;
	ioff = start;
	for (i = 0; i < widy; i++) {
		cursor(xs, y);
		if (ioff >= nelm)
#ifdef	TOWER
			lineout("");
#else
			lineout((char far *)"");
#endif
		else
#ifdef startloc
			if(outvirgin == 1)
#ifdef TOWER
			  if((outvirgin=startckline(epp[ioff]))==0) {
				strcpy(tbuf,"\\(");
				strcat(tbuf,epp[ioff]);
				strcat(tbuf,"\\)");
				lineout(tbuf);
			  }
			  else lineout(epp[ioff]);
#else
#ifdef batflag
			  {
			     strcpy(tbuf,"\\(");
			     for(ii=0;ii<80;ii++) {
				if((tbuf[2+ii]=*(epp[ioff]+ii))==0) break;
			     }
			     strcat(tbuf,"\\)");
			     if((outvirgin=startckline(&tbuf[2]))==0) {
				lineout(tbufptr);
			     }
			     else lineout(epp[ioff]);
			  }
#else
			     lineout(epp[ioff]);
#endif	/* batflag */
#endif
			else lineout(epp[ioff]);
#else
			lineout(epp[ioff]);
#endif
		if (wflag)
			cput(' ');
		else if (i < sstart || i > sstop)
			cput(FUZZ2);
		else
			cput(BLOCK);
		ioff++;
		y++;
	}
}

#ifdef	TOWER
static void	lineout(p)
	char *p;
{
	int	n;

	for (n = hsput(p, widx); n < widx; n++)
		cput(' ');
}
#else
static void	lineout(pl)
	char far	*pl;
{
	int	n;
	char	buf[LINELEN], *p;

	for (p = buf; *pl; *p++ = *pl++);
	*p = '\0';
	for (n = hsput(buf, widx); n < widx; n++)
		cput(' ');
}
#endif

int	tkyo(x, y)
	int	x, y;
{
	extern void	flushin();

	xyprintf(x, y, "Type almost any key to continue...");
	flushin();
	return(spgetch());
}

void	center(targ, wid, str)
	char	*targ, *str;
	int	wid;
{
	char	*p1, *p2;
	int	i, len, pad1, pad2;

	len = strlen(str);
	if (len > wid)
		len = wid;
	pad1 = (wid - len)/2;
	pad2 = wid - len - pad1;
	p1 = targ;
	for (i = 0; i < pad1; i++)
		*p1++ = ' ';
	for (p2 = str; *p2; *p1++ = *p2++);
	for (i = 0; i < pad2; i++)
		*p1++ = ' ';
	*p1 = '\0';
}
