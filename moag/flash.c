#include	<stdio.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

#ifdef	NOINTRO

extern char	*titlemsg;
extern int	menux;
extern int	menuwid;

int	motheroff;

char	*play[] = {
	"Carl Schnurr",
	"George Welch",
	"Kevin Stokes",
	"Molly Stokes",
	"Zandy Williams",
	"Patrick Laverty",
	"Allan Bacon",
	"Michael Marable",
	"Jeff Gardner",
	"Thomas Pope",
	"Sean \"Choke\" Gallaty",
	"Darell C. Phillips",
        "Bob Roberds",
	"Don Burch"        
};
int	nplay = (sizeof(play)/sizeof(char *));
int	psortfn(p1, p2)
	char	**p1, **p2;
{
	return(stricmp(*p1, *p2));
}
#define	NPLAY	7

void	flash() {
	char	buf[LINELEN], buf2[LINELEN], *p;
	extern int	kbhit();
	extern char	version[];
	extern void	doit(), waitit(), outplay(), center(), boxsetup(),
			qsort();

	erscrn();
	qsort(play, nplay, sizeof(char *), psortfn);

	setblink(0);
	color(LIGHTRED, GRAY);
	boxsetup(FLXMESS, FLYMESS, FLXWID, FLYWID, 0);

	color(BRIGHTWHITE, GRAY);
	center(buf, FLXWID-2, "Pie in the Sky Software Presents...");
	cursor(FLXMESS+1, FLYMESS+2);
	sput(buf);

	color(WHITE, GRAY);
	boxsetup(menux, FLYMESS+3, menuwid, 3, 0);
	color(YELLOW, GRAY);
	center(buf, menuwid-2, titlemsg);
	for (p = buf; *p == ' '; p++);
	motheroff = p - buf;
	motheroff += menux + 1;
	cursor(menux+1, FLYMESS+4);
	sput(buf);

	color(CYAN, GRAY);
	cursor(FLXMESS+24, FLYMESS+7);
	sput("Programming:  ");
	color(BRIGHTWHITE, GRAY);
	sput("Kevin Stokes");

	color(CYAN, GRAY);
	cursor(FLXMESS+23, FLYMESS+9);
	sput("Play Testing:  ");
	outplay();

	color(CYAN, GRAY);
	cursor(FLXMESS+19, FLYMESS+9+NPLAY+1);
	sput("Additional Programming:  ");
	color(WHITE, GRAY);
	sput("George Welch");

	color(YELLOW, GRAY);
	sprintf(buf2, "Ver. %s - Copyright (C) 1991 by Kevin Stokes", version);
	center(buf, FLXWID-2, buf2);
	cursor(FLXMESS+1, FLYMESS+FLYWID-4);
	sput(buf);

	color(WHITE, GRAY);
	center(buf, FLXWID-2, "  Type almost any key to continue...");
	cursor(FLXMESS+1, FLYMESS+FLYWID-2);
	sput(buf);

	while (!kbhit()) {
		doit();
		waitit();
	}

	spgetch();
	setblink(1);
}

static void	outplay() {
	int	i, j, k;
	static int	playstart = 0;

	color(WHITE, GRAY);
	k = playstart;
	for (i = 0; i < NPLAY; i++) {
		cursor(FLXMESS+38, FLYMESS+9+i);
		sput(play[k]);
		for (j = strlen(play[k]); j < 25; j++)
			cput(' ');
		if (k == nplay-1)
			k = 0;
		else
			k++;
	}
	if (playstart == nplay-1)
		playstart = 0;
	else
		playstart++;
}

static unsigned int	count;
static int	off;
static int	skip = 3;
static int	skk, sk2;
static unsigned char	dodad, highfore, highback, lowfore, lowback;
static void	doit() {
	int	x, y, i;
	extern void	dodah(), randah();

	dodad = 15;
	highfore = LIGHTRED;
	highback = GRAY;
	lowfore = WHITE;
	lowback = GRAY;

	x = menux;
	y = FLYMESS+3;
	randah(x++, y, SUPPERLEFT);
	for (i = 1; i < menuwid-1; i++)
		randah(x++, y, STOP);
	randah(x, y++, SUPPERRIGHT);
	randah(x, y++, SRIGHT);
	randah(x--, y, SLOWERRIGHT);
	for (i = 1; i < menuwid-1; i++)
		randah(x--, y, SBOTTOM);
	randah(x, y--, SLOWERLEFT);
	randah(x, y, SLEFT);
	if ((skk++%3) != 0)
		return;

	dodad = 4;
	highfore = LIGHTCYAN;
	x = FLXMESS;
	y = FLYMESS;

	count = skip;
	off++;
	if (off > skip)
		off = 1;

	dodah(x++, y, DUPPERLEFT);
	for (i = 1; i < FLXWID-1; i++)
		dodah(x++, y, DTOP);
	dodah(x, y++, DUPPERRIGHT);
	for (i = 1; i < FLYWID-1; i++)
		dodah(x, y++, DRIGHT);
	dodah(x, y, DLOWERRIGHT);
	for (i = 1; i < FLXWID-1; i++)
		dodah(--x, y, DBOTTOM);
	dodah(--x, y, DLOWERLEFT);
	for (i = 1; i < FLYWID-1; i++)
		dodah(x, --y, DLEFT);

	if (sk2&1) {
		color(BRIGHTWHITE, GRAY);
		xyprintf(motheroff, FLYMESS+4, "CORNCOB");
	}
	else {
		color(YELLOW, GRAY);
		xyprintf(motheroff, FLYMESS+4, "CORNCOB");
	}

	sk2++;
	if ((sk2%3) == 0)
		outplay();
}

#else	/* !NOINTRO */

static unsigned int	count;
static int	off;
static int	skip = 3;
static int	skk, sk2;
static unsigned char	dodad, highfore, highback, lowfore, lowback;

#endif	/* !NOINTRO */

#include	<dos.h>
void	waitit() {
	long	now, next;
	extern int	kbhit();

	now = *(long far *)MK_FP(0x40, 0x6C);
	next = now+2;
	while (*(long far *)MK_FP(0x40, 0x6C) < next)
		if (kbhit())
			break;
}

static void	dodah(x, y, c)
	int	x, y;
	char	c;
{
	cursor(x, y);
	if ( ((count-off) % skip) == 0) {
		color(highfore, highback);
		cput(dodad);
	}
	else {
		color(lowfore, lowback);
		cput(c);
	}
	count++;
}

static void	randah(x, y, c)
	int	x, y;
	char	*c;
{
	extern int	rand();

	cursor(x, y);
	if (rand() < 2000) {
		color(highfore, highback);
		cput(dodad);
	}
	else {
		color(lowfore, lowback);
		cput(c);
	}
}

extern int	generoffx, generoffy;
extern char	*genername;
void	gendoit() {
	int	x, y, i;
	extern void	dodah(), randah();

	dodad = 15;
	highfore = LIGHTRED;
	highback = BLUE;
	lowfore = WHITE;
	lowback = BLUE;

	for (y = 1; y < FLYMESS; y++)
		for (x = 1; x <= NCOL; x++)
			randah(x, y, ' ');
	for (; y < FLYMESS+FLYWID; y++) {
		for (x = 1; x < FLXMESS; x++)
			randah(x, y, ' ');
		for (x = FLXMESS+FLXWID; x <= NCOL; x++)
			randah(x, y, ' ');
	}
	for (; y <= NROW; y++)
		for (x = 1; x <= NCOL; x++)
			randah(x, y, ' ');
	if ((skk++%3) != 0)
		return;

	dodad = 4;
	highfore = LIGHTCYAN;
	highback = GRAY;
	lowfore = WHITE;
	lowback = GRAY;
	x = FLXMESS;
	y = FLYMESS;

	count = skip;
	off++;
	if (off > skip)
		off = 1;

	dodah(x++, y, DUPPERLEFT);
	for (i = 1; i < FLXWID-1; i++)
		dodah(x++, y, DTOP);
	dodah(x, y++, DUPPERRIGHT);
	for (i = 1; i < FLYWID-1; i++)
		dodah(x, y++, DRIGHT);
	dodah(x, y, DLOWERRIGHT);
	for (i = 1; i < FLXWID-1; i++)
		dodah(--x, y, DBOTTOM);
	dodah(--x, y, DLOWERLEFT);
	for (i = 1; i < FLYWID-1; i++)
		dodah(x, --y, DLEFT);

	if (sk2&1) {
		color(BRIGHTWHITE, GRAY);
		xyprintf(generoffx, generoffy, genername);
	}
	else {
		color(YELLOW, GRAY);
		xyprintf(generoffx, generoffy, genername);
	}
	sk2++;
}

