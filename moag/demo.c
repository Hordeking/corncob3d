#define	MYSPAWN	1
#include	<stdio.h>
#ifndef	MYSPAWN
#include	<process.h>
#endif
#include	<string.h>
#include	<io.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

INIT struct demot	*demos;
INIT int	ndemos;
INIT int	rundemo;

/*
 **************** Demo menu (level 2)
 */

INIT int	stoutdmno;
INIT int	noutdms;
INIT int	outdmflag;
#ifdef	REGISTERED
#define	OUTTOP	1
#define	OUTBOT	2

void	chdemo() {
	int	c, cur, last, x, y;
	extern void	demosetup(), outdms(), outdm(), outdmdesc(), godemo();

	if (ndemos == 0) {
		x = MIXMESS+14;
		y = MIYMESS+4;
		simplesetup(x, y++, 47, 8, "No Demos",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		xyprintf(x, y++, "Sorry, there are no demos to choose from.");
		xyprintf(x, y++, "The demos should be files stored in the");
		xyprintf(x, y++, "subdirectory `dmo', along with the index");
		xyprintf(x, y++, "file `demo.ndx'.");
		beep();
		tkyo(++x, ++y);
		return;
	}
	demosetup();
	color(BLUE, CYAN);
	xyprintf(WDXMESS+3, WDYMESS+1,
		"Choose the desired demo from the list at the");
	xyprintf(WDXMESS+3, WDYMESS+2,
		"right using the arrow keys (\030\031).  Press <CR>");
	xyprintf(WDXMESS+3, WDYMESS+3,
		"to run the demo, or <ESC> to abort.");
	xyprintf(WDXMESS+3, WDYMESS+5,
		"A brief description of the demo appears below.");

	color(BLUE, CYAN);
	outdms();
	if (stoutdmno > 0)
		cur = 1;
	else
		cur = 0;
	last = cur;
	color(BRIGHTWHITE, CYAN);
	outdm(cur);
	outdmdesc(cur + stoutdmno);
	while (1) {
		c = spgetch();
		if (c == 033 || c == 010)
			return;
		if (c == '\n') {
			godemo(cur + stoutdmno);
			return;
		}
		else if (c == UPARROW || c == LEFTARROW) {
			if (ndemos > noutdms) {
				if (cur == (outdmflag&OUTTOP) ? 1 : 0) {
					if (stoutdmno == 0)
						beep();
					else {
						stoutdmno--;
						color(BLUE, CYAN);
						outdms();
					}
				}
				else
					cur--;
			}
			else {
				if (cur == 0)
					cur = ndemos-1;
				else
					cur--;
			}
		}
		else if (c == DOWNARROW  || c == RIGHTARROW) {
			if (ndemos > noutdms) {
				if (cur == noutdms - ((outdmflag&OUTBOT)?
								2 : 1) ) {
					if (stoutdmno == ndemos-noutdms)
						beep();
					else {
						stoutdmno++;
						color(BLUE, CYAN);
						outdms();
					}
				}
				else
					cur++;
			}
			else {
				if (cur == ndemos-1)
					cur = 0;
				else
					cur++;
			}
		}
		else {
			beep();
			continue;
		}
		color(BLUE, CYAN);
		outdm(last);
		color(BRIGHTWHITE, CYAN);
		outdm(cur);
		outdmdesc(cur + stoutdmno);
		last = cur;
	}
}

static void	outdms() {
	int	i;
	extern void	outdm();

	outdmflag = 0;
	for (i = 0; i < noutdms; i++) {
		if (i + stoutdmno >= ndemos)
			break;
		if (i == 0 && stoutdmno > 0) {
			outdmflag |= OUTTOP;
			outdm(-1);
		}
		else if (i == noutdms-1 && stoutdmno+noutdms < ndemos) {
			outdmflag |= OUTBOT;
			outdm(-2);
		}
		else
			outdm(i);
	}
}

static char	more[] = "              MORE  ";
static int	nm = sizeof(more) - 2;
static void	outdm(i)
	int	i;
{
	char	*p;
	extern void	savepos(), restpos();

	savepos();
	if (i == -1) {
		cursor(WDXMESS+WDXWID-3-DRPWID-3+3, WDYMESS+3 + 0);
		p = more;
		p[nm] = '\030';
		color(BRIGHTWHITE, CYAN);
	}
	else if (i == -2) {
		cursor(WDXMESS+WDXWID-3-DRPWID-3+3, WDYMESS+3 + noutdms-1);
		p = more;
		p[nm] = '\031';
		color(BRIGHTWHITE, CYAN);
	}
	else {
		cursor(WDXMESS+WDXWID-3-DRPWID-3+3, WDYMESS+3 + i);
		p = demos[i+stoutdmno].name;
	}

	sput(p);
	for (i = strlen(p); i < DRPWID+1; i++)
		cput(' ');
	restpos();
}

static void	outdmdesc(n)
	int	n;
{
	char	buf[LINELEN], *p;
	int	i;
	FILE	*fp;

	extern struct theatert	*theaters;
	extern char	*fngets();

	cursor(WDXMESS+2, WDYMESS+7);
	color(YELLOW, CYAN);
	sput("Demo:  ");
	sprintf(buf, "%-20s         ", demos[n].name);
	color(BRIGHTWHITE, CYAN);
	sput(buf);
	cursor(WDXMESS+2, WDYMESS+8);
	color(YELLOW, CYAN);
	sput("Set in theater:  ");
	sprintf(buf, "%-20s", theaters[demos[n].theatno].name);
	color(BRIGHTWHITE, CYAN);
	sput(buf);

	p = buf;
	for (i = 0; i < WDXWID-3-DRPWID-3; i++)
		*p++ = ' ';
	*p = '\0';
	for (i = 9; i < WDYWID-1; i++) {
		cursor(WDXMESS+1, WDYMESS+i);
		sput(buf);
	}

	cursor(WDXMESS+2, WDYMESS+9);
	color(BLUE, CYAN);
	if ((fp = fopen("dmo\\demo.ndx", "rb")) == NULL) {
		sprintf(buf, "There is no further information about this demo");
		sput(buf);
		return;
	}
	fseek(fp, demos[n].descoffs, SEEK_SET);
	i = 9;
	while (fngets(buf, 80, fp) != NULL) {
		if (i == WDYWID-1 || buf[0] == 12)
			break;
		cursor(WDXMESS+2, WDYMESS+i);
		sput(buf);
		i++;
	}
	fclose(fp);
}

	void	godemo(n)
	int	n;
{
	int	fdi, fdo, argcnt;
	char	*argvec[LINELEN], buf2[16];
	extern struct theatert	*theaters;
	extern char	*file3;
#ifndef MVPCD	/* if CD ROM, these are #defines, not actual pointers */
	extern	char	 filedemo[], fileuniv[];
#else
extern	char	wpath[80];
extern	char	scrlist[80];
#endif
	extern int	ropen(), wopen();
	extern void	fdcopy(), savestate(), prepfor3();
#ifdef	MYSPAWN
	extern void	myspawn();
#endif

	sprintf(buf2, "dmo\\%s.dmo", demos[n].fname);
	if ((fdi = ropen(buf2)) < 0) {
		errorbox("Cannot open demo %s for reading", buf2);
		return;
	}
	if ((fdo = wopen(filedemo)) < 0) {
		errorbox("Cannot open %s for writing", filedemo);
		close(fdi);
		return;
	}
	fdcopy(fdo, fdi);
	close(fdi);
	close(fdo);

	sprintf(buf2, "def\\%s.def", theaters[demos[n].theatno].fname);
	if ((fdi = ropen(buf2)) < 0) {
		errorbox("Cannot open theater %s for reading", buf2);
		return;
	}
	if ((fdo = wopen(fileuniv)) < 0) {
		errorbox("Cannot open %s for writing", fileuniv);
		close(fdi);
		return;
	}
	fdcopy(fdo, fdi);
	close(fdi);
	close(fdo);

	/*
	 *	set up argv[0]
	 */
 	argcnt = 0;
	argvec[argcnt++] = file3;
	argvec[argcnt++] = "-rbsk";
	argvec[argcnt++] = (char *)0;

	rundemo = 1;

	savestate();
	prepfor3();
#ifdef	MYSPAWN
	myspawn(argvec);
#else
	spawnv(P_OVERLAY, file3, argvec);		/**/
#endif
}

static char	*msg = "Choose a Demo Flight";

static void	demosetup() {
	int	i, cc;
	char	buf[128], buf2[128], *p;
	extern int	menuwid;
	extern void	center();

	cursor(menux, menuy);
	color(BLUE, CYAN);
	center(buf, menuwid, msg);
	sput(buf);

	cc = WDXWID-3-DRPWID-3;

	cursor(WDXMESS, WDYMESS);
	color(BROWN, CYAN);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 0; i < cc; i++)
		*p++ = STOP;
	*p++ = STMIDDLE;
	for (i = 0; i < DRPWID+3; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	cursor(WDXMESS, WDYMESS+1);
	color(BROWN, CYAN);
	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < cc; i++)
		*p++ = ' ';
	*p++ = SVMIDDLE;
	*p = '\0';
	sput(buf);
	color(BLUE, CYAN);
	center(buf, DRPWID+3, "Available Demos: ");
	sput(buf);
	color(BROWN, CYAN);
	cput(SRIGHT);
	cblock();

	cursor(WDXMESS, WDYMESS+2);
	color(BROWN, CYAN);
	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < cc; i++)
		*p++ = ' ';
	*p++ = SLEFTMIDDLE;
	for (i = 0; i < DRPWID+3; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	*p = '\0';
	sput(buf);
	cblock();

	cursor(WDXMESS, WDYMESS+3);
	color(BROWN, CYAN);
	p = buf2;
	*p++ = SLEFT;
	for (i = 0; i < cc; i++)
		*p++ = ' ';
	*p++ = SVMIDDLE;
	for (i = 0; i < DRPWID+3; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	sput(buf2);
	cblock();

	cursor(WDXMESS, WDYMESS+4);
	sput(buf2);
	cblock();

	cursor(WDXMESS, WDYMESS+5);
	sput(buf2);
	cblock();

	cursor(WDXMESS, WDYMESS+6);
	p = buf;
	*p++ = SLEFTMIDDLE;
	for (i = 0; i < cc; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	for (i = 0; i < DRPWID+3; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	sput(buf);
	cblock();

	for (i = 7; i < WDYWID-1; i++) {
		cursor(WDXMESS, WDYMESS+i);
		color(BROWN, CYAN);
		sput(buf2);
		cblock();
	}

	cursor(WDXMESS, WDYMESS+i);
	color(BROWN, CYAN);
	p = buf;
	*p++ = SLOWERLEFT;
	for (i = 0; i < cc; i++)
		*p++ = SBOTTOM;
	*p++ = SBMIDDLE;
	for (i = 0; i < DRPWID+3; i++)
		*p++ = SBOTTOM;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);
	cblock();

	cursor(WDXMESS+1, WDYMESS+WDYWID);
	for (i = 0; i < WDXWID; i++)
		cblock();

	noutdms = WDYWID-4;
}
#endif	/* REGISTERED */
