#include	<stdio.h>
#include	<dos.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

INIT int	menux;
INIT int	menuy;
INIT int	menuwid;
INIT int	spoof;

/*	MAIN MENU	 */

char	*titlemsg;
#ifndef REGISTERED
char	maintitle[] = "            Corncob 3D             ";
#else
char	maintitle[] = "   Corncob Other Worlds Campaign   ";
#endif
int	mainwid = sizeof(maintitle)/sizeof(char)-1;
char	*msg2 = "Main Menu";


void	prepresetup() {
	int	n, n1;

	
	titlemsg = maintitle;
	menuwid = mainwid;
	n = NCOL - menuwid - 4;
	n1 = n/2;
	menuy = 4;
	menux = 3 + n1;
}

void	presetup() {
	int	n, n1;

		titlemsg = maintitle;
		menuwid = mainwid;
	n = NCOL - menuwid - 4;
	n1 = n/2;
	menuy = 4;
	menux = 3 + n1;
}

void	mainsetup() {
	int	i, n, n1, n2;
	char	buf[128], *p;
	extern void	center();

	n = NCOL - menuwid - 4;
	n1 = n/2;
	n2 = n - n1;

	cursor(1, 1);
	color(WHITE, BLACK);
	p = buf;
	*p++ = ' ';
	for (i = 0; i < n1; i++)
		*p++ = ' ';
	*p = '\0';
	sput(buf);
	p = buf;
	color(YELLOW, BLUE);
	*p++ = SUPPERLEFT;
	for (i = 0; i < menuwid; i++)
		*p++ = SHMIDDLE;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);
	p = buf;
	color(WHITE, BLACK);
	for (i = 0; i < n2; i++)
		*p++ = ' ';
	*p++ = ' ';
	*p = '\0';
	sput(buf);

	p = buf;
	color(YELLOW, BLUE);
	*p++ = DUPPERLEFT;
	for (i = 0; i < n1; i++)
		*p++ = DTOP;
	*p++ = LDTOVERT;
	*p = '\0';
	sput(buf);
	color(BRIGHTWHITE, BLUE);
	sput(titlemsg);
	p = buf;
	color(YELLOW, BLUE);
	*p++ = RDTOVERT;
	for (i = 0; i < n2; i++)
		*p++ = DTOP;
	*p++ = DUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	color(YELLOW, BLUE);
	*p++ = DLEFT;
	for (i = 0; i < n1; i++)
		*p++ = ' ';
	*p++ = SLEFTMIDDLE;
	for (i = 0; i < menuwid; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	for (i = 0; i < n2; i++)
		*p++ = ' ';
	*p++ = DRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	color(YELLOW, BLUE);
	*p++ = DLEFT;
	for (i = 0; i < n1; i++)
		*p++ = ' ';
	*p++ = SVMIDDLE;
	*p = '\0';
	sput(buf);
	color(WHITE, BLUE);
	center(buf, menuwid, msg2);
	sput(buf);
	p = buf;
	color(YELLOW, BLUE);
	*p++ = SVMIDDLE;
	for (i = 0; i < n2; i++)
		*p++ = ' ';
	*p++ = DRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	color(YELLOW, BLUE);
	*p++ = DLEFT;
	for (i = 0; i < n1; i++)
		*p++ = ' ';
	*p++ = SLOWERLEFT;
	for (i = 0; i < menuwid; i++)
		*p++ = SHMIDDLE;
	*p++ = SLOWERRIGHT;
	for (i = 0; i < n2; i++)
		*p++ = ' ';
	*p++ = DRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	color(YELLOW, BLUE);
	*p++ = DLEFT;
	n = NCOL - 2;
	for (i = 0; i < n; i++)
		*p++ = ' ';
	*p++ = DRIGHT;
	*p = '\0';
	for (i = 6; i < NROW; i++)
		sput(buf);

	p = buf;
	color(YELLOW, BLUE);
	*p++ = DLOWERLEFT;
	n = NCOL - 2;
	for (i = 0; i < n; i++)
		*p++ = DBOTTOM;
	*p++ = DLOWERRIGHT;
	*p = '\0';
	sput(buf);

	color(YELLOW, BLUE);
}

void	simplesetup(x1, y1, xwid, ywid, str, fore, back, finalfore)
	int	x1, y1, xwid, ywid, fore, back;
	char	*str;
{
	int	i, n, rn, r;
	char	buf[128], *p;
	extern void	center();

	cursor(menux, menuy);
	color(finalfore, back);
	center(buf, menuwid, str);
	sput(buf);

	n = xwid - 2;

	cursor(x1, y1);
	p = buf;
	color(fore, back);
	*p++ = SUPPERLEFT;
	for (i = 0; i < n; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < n; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	rn = y1 + ywid - 1;
	for (r = y1+1; r < rn; r++) {
		color(fore, back);
		cursor(x1, r);
		sput(buf);
		cblock();
	}

	cursor(x1, r++);
	p = buf;
	color(fore, back);
	*p++ = SLOWERLEFT;
	for (i = 0; i < n; i++)
		*p++ = STOP;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);
	cblock();

	cursor(x1+1, r);
	n++;
	for (i = 0; i <= n; i++)
		cblock();

	color(finalfore, back);
}

void	nbmplesetup(x1, y1, xwid, ywid, str, fore, back, finalfore)
	int	x1, y1, xwid, ywid, fore, back;
	char	*str;
{
	int	i, n, rn, r;
	char	buf[128], *p;
	extern void	center();

	cursor(menux, menuy);
	color(finalfore, back);
	center(buf, menuwid, str);
	sput(buf);

	n = xwid - 2;

	cursor(x1, y1);
	p = buf;
	color(fore, back);
	*p++ = SUPPERLEFT;
	for (i = 0; i < n; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < n; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	rn = y1 + ywid - 1;
	for (r = y1+1; r < rn; r++) {
		color(fore, back);
		cursor(x1, r);
		sput(buf);
	}

	cursor(x1, r++);
	p = buf;
	color(fore, back);
	*p++ = SLOWERLEFT;
	for (i = 0; i < n; i++)
		*p++ = STOP;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);

	color(finalfore, back);
}

void	boxsetup(x, y, xwid, ywid, sflag)
	int	x, y, xwid, ywid, sflag;
{
	char	buf[LINELEN], *p;
	int	i, ywidm;

	cursor(x, y);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	*p++ = SLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	ywidm = ywid - 1;
	for (i = 1; i < ywidm; i++) {
		cursor(x, y+i);
		sput(buf);
		if (sflag)
			cblock();
	}

	cursor(x, y+ywid-1);
	p = buf;
	*p++ = SLOWERLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = SBOTTOM;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);
	if (sflag) {
		cblock();
		cursor(x+1, y+ywid);
		for (i = 0; i < xwid; i++)
			cblock();
	}
}

