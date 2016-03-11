#include	<stdio.h>
#include	"moag.h"
#include	"texto.h"

/*
 **************** Roll-your-own menu (level 2)
 */

int	curryoch = 0;	/* Ryo stuff has been changed flag */
extern void	strloc(), mdbparm(), mwick(), mnum(), mdef(), ryohelp(), go(),
		tinvulnerable(), quit();
static struct cmdt	choices[] = {
	"\\Fly mission",					go,
	"modify \\Wickedness of enemies",			mwick,
	"choose \\Number of enemies",				mnum,
	"choose \\Default values",				mdef,
	"toggle invulnerable \\Plane",				tinvulnerable,
	"get \\Information about this menu",			ryohelp,
	"\\Return to previous menu",				quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	ryochoice;
void	rollmsn() {
	struct cmdt	*ch;
	int far	*scr;
	extern void	outchoice(), showchoices();
	extern struct cmdt	*getchoice();

	curpilot->curtheater |= 0x80;
	ryochoice = DEFCHOICE;
	simplesetup(ROXMESS, ROYMESS, ROXWID, ROYWID, "Training Missions",
							BROWN, CYAN, BLUE);
	scr = yankscr();
	showchoices();
	while ((ch =
	        getchoice(choices, &ryochoice, nchoices, outchoice,
				(int (*)())NULL))->ccmd != (void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		showchoices();
	}
	freescr(scr);
}

static void	showchoices() {
	int	i, x, y, x2;
	char	buf[LINELEN], *pr, *ps;
	extern void	outchoice();

	color(BLUE, CYAN);
	cursor(ROXMESS+20, ROYMESS+1);
	sput("Use arrow keys or type highlighted");
	cursor(ROXMESS+20, ROYMESS+2);
	sput("    letter to choose action:");

	for (i = 0; i < nchoices; i++)
		outchoice(i);

	x = ROXMESS+13;
	y = ROYMESS+ROYWID-6;
	pr = "random spread of strength";
	ps = "strength %d";
	color(YELLOW, CYAN);
	xyprintf(x, y++, "Current choices:");
	color(BLUE, CYAN);
	x += 3;
	x2 = x + 25;

	cursor(x, y++);
	sput("Plane:  ");
	if (curryo->invflag&TRAININGVAL) {
		sput("Invulnerable");
		curryo->invflag |= INVVAL;
	}
	else {
		sput("Standard");
		curryo->invflag &= ~INVVAL;
	}

	xyprintf(x, y, "%d missile sites", curryo->ndballs);
	if (curryo->ndballs) {
		cput(':');
		if (curryo->edballs)
			sprintf(buf, ps, curryo->edballs);
		else
			sprintf(buf, pr);
		cursor(x2, y);
		sput(buf);
	}
	y++;
	xyprintf(x, y, "%d AAA sites", curryo->naaa);
	if (curryo->naaa) {
		cput(':');
		if (curryo->eaaa)
			sprintf(buf, ps, curryo->eaaa);
		else
			sprintf(buf, pr);
		cursor(x2, y);
		sput(buf);
	}
	y++;
	xyprintf(x, y, "%d ground vehicle sites", curryo->nscarts);
	if (curryo->nscarts) {
		cput(':');
		if (curryo->escarts)
			sprintf(buf, ps, curryo->escarts);
		else
			sprintf(buf, pr);
		cursor(x2, y);
		sput(buf);
	}
}

static void	outchoice(i) {
	cursor(ROXMESS+22, i+(ROYMESS+4));
	hsput(choices[i].scmd, LINELEN);
}

INIT int	stlch;
/*
 **************** modify Wickedness menu (level 3)
 */

extern void	mdblwick(), mshcwick(), maaawick(), quit();
static struct cmdt	wckchs[] = {
	"modify enemy \\Missile wickedness",		mdblwick,
	"modify enemy \\Ground vehicle",		mshcwick,
	"modify \\AAA wickedness",			maaawick,
	"\\Return to previous menu",			quit,
};
static int	nwckchs = (sizeof(wckchs)/sizeof(struct cmdt));

#define	DEFWCKCH	0
INIT int	wckch;
void	mwick() {
	struct cmdt	*ch;
	int far	*scr;
	extern void	outwckch(), showwckchs();
	extern int	asswckch();
	extern struct cmdt	*getchoice();

	wckch = DEFWCKCH;
	simplesetup(WCKXMESS, WCKYMESS, WCKXWID, WCKYWID,
			"Modify Wickedness of Enemies", GREEN, RED, WHITE);
	scr = yankscr();
	showwckchs();
	while ((ch =
	        getchoice(wckchs, &wckch, nwckchs, outwckch, asswckch))->ccmd !=
						(void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		showwckchs();
	}
	freescr(scr);
}

static void	showwckchs() {
	int	i;
	extern void	outwckch();

	color(WHITE, RED);
	cursor(WCKXMESS+2, WCKYMESS+1);
	sput("Use arrow keys or type highlighted");
	cursor(WCKXMESS+2, WCKYMESS+2);
	sput(" letter to choose action:");

	for (i = 0; i < nwckchs; i++)
		outwckch(i);
}

static void	outwckch(i) {
	int	d;
	char	buf[32];

	cursor(WCKXMESS+4, i+(WCKYMESS+4));
	hsput(wckchs[i].scmd, LINELEN);
	if (i == 0)
		d = curryo->edballs;
	else if (i == 1)
		d = curryo->escarts;
	else if (i == 2)
		d = curryo->eaaa;
	else
		return;
	cursor(WCKXMESS+36, i+(WCKYMESS+4));
	sprintf(buf, "  currently %-3d", d);
	sput(buf);
}

INIT int	ngetflag;
int	asswckch(i, c)
	int	i;
	char	c;
{
	char	*p;
	unsigned int	n;
	extern int	atoi();
	extern char	*nget();
	extern void	asswckerror();

	cursor(WCKXMESS+48, i+(WCKYMESS+4));
	inverse();
	p = nget(2, c);
	if (p == NULL)
		return(ngetflag);
	n = atoi(p);
	if (n > 8) {
		asswckerror(n);
		ngetflag = 0;
		return(ngetflag);
	}

	if (i == 0) {
		curryo->edballs = (char)n;
		curryoch = 1;
	}
	else if (i == 1) {
		curryo->escarts = (char)n;
		curryoch = 1;
	}
	else if (i == 2) {
		curryo->eaaa = (char)n;
		curryoch = 1;
	}
	return(ngetflag);
}

static void	asswckerror(n)
	int	n;
{
	int far	*scr;

	scr = yankscr();
	simplesetup(WCKXMESS+3, WCKYMESS+9, 55, 7,
			"Error:  Value is too large", LIGHTRED, BROWN, YELLOW);
	xyprintf(WCKXMESS+4, WCKYMESS+10, "Sorry, %d is too large.", n);
	xyprintf(WCKXMESS+4, WCKYMESS+11,
		"The wickedness of enemies must be in the range 1-8");
	xyprintf(WCKXMESS+4, WCKYMESS+12,
		"    or 0 for a random spread of wickedness.");
	beep();
	tkyo(WCKXMESS+5, WCKYMESS+14);
	putscr(scr);
	freescr(scr);
	color(WHITE, RED);
}

static void	mdblwick() {
	extern void	lmwick();

	lmwick("Missile", &(curryo->edballs));
}

static void	mshcwick() {
	extern void	lmwick();

	lmwick("Ground-Vehicle", &(curryo->escarts));
}

static void	maaawick() {
	extern void	lmwick();

	lmwick("AAA", &(curryo->eaaa));
}

static void	lmwick(swick, nwick)
	char	*swick;
	char	*nwick;
{
	char	buf[LINELEN], *p;
	int	c, y;
	extern int	atoi();
	extern char	*nget();

	sprintf(buf, "Modify %s Wickedness", swick);
	y = MWCKYMESS;
	simplesetup(MWCKXMESS, y++, MWCKXWID, MWCKYWID,
						buf, MAGENTA, WHITE, RED);
	y++;
	xyprintf(MWCKXMESS+2, y++, "%s wickedness is currently %d.",
						swick, *nwick);
	xyprintf(MWCKXMESS+3, y++, "Values of 1-8 mean increasing wickedness.");
	xyprintf(MWCKXMESS+3, y++, "A value of 0 will choose a random");
	xyprintf(MWCKXMESS+3, y++, "spread of wickedness.");
	y++;
	xyprintf(MWCKXMESS+2, y++, "Enter new value [0-8]  ");
	p = nget(2, '\0');
	if (p == NULL || *p == '\0')
		return;
	else if ((c = atoi(p)) > 8) {
		asswckerror(c);
		return;
	}
	else {
		*nwick = (char)c;
		curryoch = 1;
	}
}

/*
 **************** modify Number of enemies menu (level 3)
 */

extern void	mdblnsit(), mshcnsit(), maaansit(), quit();
static struct cmdt	nstchs[] = {
	"change number of \\Missile sites",		mdblnsit,
	"change number of \\Ground-Vehicle sites",	mshcnsit,
	"change number of \\AAA sites",			maaansit,
	"\\Return to previous menu",			quit,
};
static int	nnstchs = (sizeof(nstchs)/sizeof(struct cmdt));

#define	DEFNSTCH	0
INIT int	nstch;
static void	mnum() {
	struct cmdt	*ch;
	int far	*scr;
	extern void	outnstch(), shownstchs();
	extern int	assnstch();
	extern struct cmdt	*getchoice();

	nstch = DEFNSTCH;
	simplesetup(NSTXMESS, NSTYMESS, NSTXWID, NSTYWID,
			"Choose Number of Enemies", GREEN, RED, WHITE);
	scr = yankscr();
	shownstchs();
	while ((ch =
	        getchoice(nstchs, &nstch, nnstchs, outnstch, assnstch))->ccmd !=
						(void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		shownstchs();
	}
	freescr(scr);
}

static void	shownstchs() {
	int	i;
	extern void	outnstch();

	color(WHITE, RED);
	cursor(NSTXMESS+2, NSTYMESS+1);
	sput("Use arrow keys or type highlighted");
	cursor(NSTXMESS+2, NSTYMESS+2);
	sput(" letter to choose action:");

	for (i = 0; i < nnstchs; i++)
		outnstch(i);
}

static void	outnstch(i)
	int	i;
{
	char	buf[32];
	int	d;

	cursor(NSTXMESS+4, i+(NSTYMESS+4));
	hsput(nstchs[i].scmd, LINELEN);
	if (i == 0)
		d = curryo->ndballs;
	else if (i == 1)
		d = curryo->nscarts;
	else if (i == 2)
		d = curryo->naaa;
	else
		return;
	cursor(NSTXMESS+41, i+(NSTYMESS+4));
	sprintf(buf, "  currently %-5d", d);
	sput(buf);
}

int	assnstch(i, c)
	int	i;
	char	c;
{
	char	*p;
	unsigned int	n;
	extern int	atoi();
	extern char	*nget();
	extern void	assnerror();

	cursor(NSTXMESS+53, i+(NSTYMESS+4));
	inverse();
	p = nget(5, c);
	if (p == NULL) {
		normal();
		return(ngetflag);
	}
	n = atoi(p);

	if (i == 0) {
		if (n > MAXNDBALLS) {
			normal();
			assnerror("Missile", n, MAXNDBALLS);
			ngetflag = 0;
			return(ngetflag);
		}
		curryo->ndballs = (char)n;
		curryoch = 1;
	}
	else if (i == 1) {
		if (n > MAXNSCARTS) {
			normal();
			assnerror("Ground-Vehicle", n, MAXNSCARTS);
			ngetflag = 0;
			return(ngetflag);
		}
		curryo->nscarts = (char)n;
		curryoch = 1;
	}
	else if (i == 2) {
		if (n > MAXNDBALLS) {
			normal();
			assnerror("AAA", n, MAXNAAA);
			ngetflag = 0;
			return(ngetflag);
		}
		curryo->naaa = (char)n;
		curryoch = 1;
	}
	normal();
	return(ngetflag);
}

static void	assnerror(site, n, max)
	char	*site;
	int	n, max;
{
	int far	*scr;

	scr = yankscr();
	simplesetup(NSTXMESS+2, NSTYMESS+9, 55, 6,
			"Error:  Value is too large", LIGHTRED, BROWN, YELLOW);
	xyprintf(NSTXMESS+4, NSTYMESS+10, "Sorry, %d is too large.", n);
	xyprintf(NSTXMESS+4, NSTYMESS+11,
		"The maximum number of %s-sites is %d.", site, max);
	beep();
	tkyo(NSTXMESS+5, NSTYMESS+13);
	putscr(scr);
	freescr(scr);
	color(WHITE, RED);
}

static void	mdblnsit() {
	extern void	lmnsit();

	lmnsit("Missile", &curryo->ndballs, MAXNDBALLS);
}

static void	mshcnsit() {
	extern void	lmnsit();

	lmnsit("Ground-Vehicle", &curryo->nscarts, MAXNSCARTS);
}

static void	maaansit() {
	extern void	lmnsit();

	lmnsit("AAA", &curryo->naaa, MAXNAAA);
}

static void	lmnsit(snsit, nnsit, mnsit)
	char	*snsit;
	char	*nnsit;
	int	mnsit;
{
	char	buf[LINELEN], *p;
	int	y, n;
	extern int	tkyo(), atoi();
	extern char	*nget();
	extern void	simplesetup();

	sprintf(buf, "Choose # of %s Sites", snsit);
	y = MNSTYMESS;
	simplesetup(MNSTXMESS, y++, MNSTXWID, MNSTYWID,
						buf, MAGENTA, WHITE, RED);
	y++;
	xyprintf(MNSTXMESS+2, y++,"The number of %s sites is currently %d.",
							snsit, *nnsit);
	y++;
	xyprintf(MNSTXMESS+2, y++, "Enter new value [0-%d]  ", mnsit);
	p = nget(8, '\0');
	if (p == NULL || *p == '\0')
		return;
	if ((n = atoi(p)) > mnsit) {
		ngetflag = 0;
		beep();
		xyprintf(MNSTXMESS+2, y++, "%d is too big, max is %d", n,mnsit);
		tkyo(MNSTXMESS+3, y++);
		return;
	}
	*nnsit = (char)n;
	curryoch = 1;
}

char	*nget(lim, c1)
	int	lim;
	char	c1;
{
	static char	buf[LINELEN];
	char	*p, *pmax;
	int	c;

	buf[0] = '\0';
	pmax = &buf[lim];
	if (c1) {
		cput(c1);
		buf[0] = (char)c1;
		p = &buf[1];
	}
	else
		p = buf;
	ngetflag = 0;
	while (1) {
		cput('_');
		cback();
		c = spgetch();
		if (c == 033)
			return(NULL);
		else if (c == UPARROW || c == DOWNARROW ||
				c == LEFTARROW || c == RIGHTARROW) {
			if (p != buf)
				*p = '\0';
			ngetflag = c;
			return(buf);
		}
		else if (c == '\n') {
			if (p != buf)
				*p = '\0';
			return(buf);
		}
		else if (c == 010) {
			if (p > buf) {
				cput(' ');
				cback();
				cback();
				cput('_');
				cback();
				p--;
				continue;
			}
			else
				beep();
		}
		else if (c >= '0' && c <= '9') {
			if (p == pmax) {
				beep();
			}
			else {
				*p++ = (char)c;
				cput(c);
			}
		}
		else
			beep();
	}
#ifndef	__TURBOC__
	return(NULL);
#endif
}

void	mdef() {
	char	buf[LINELEN], *ps, *pr;
	int	y;
	extern void	mdefryo(), simplesetup();
	extern int	tkyo();

	mdefryo(curryo);

	y = ROYMESS+1;
	simplesetup(ROXMESS+1, y++, ROXWID-3, ROYWID-3,
			"Values Set to Default", GREEN, RED, WHITE);
	cursor(ROXMESS+3, y++);
	sput("Pilot:  ");
	outpname(curpilot);
	cput('.');
	xyprintf(ROXMESS+3, y++, "New mission parameters:");
	y++;
	xyprintf(ROXMESS+3, y, "%d missile sites", curryo->ndballs);
	pr = "random spread of strength";
	ps = "strength %d";
	if (curryo->ndballs) {
		cput(':');
		if (curryo->edballs)
			sprintf(buf, ps, curryo->edballs);
		else
			sprintf(buf, pr);
		cursor(ROXMESS+27, y);
		sput(buf);
	}
	y++;
	xyprintf(ROXMESS+3, y, "%d AAA sites", curryo->naaa);
	if (curryo->naaa) {
		cput(':');
		if (curryo->eaaa)
			sprintf(buf, ps, curryo->eaaa);
		else
			sprintf(buf, pr);
		cursor(ROXMESS+27, y);
		sput(buf);
	}
	y++;
	xyprintf(ROXMESS+3, y, "%d ground vehicle sites", curryo->nscarts);
	if (curryo->nscarts) {
		cput(':');
		if (curryo->escarts)
			sprintf(buf, ps, curryo->escarts);
		else
			sprintf(buf, pr);
		cursor(ROXMESS+27, y);
		sput(buf);
	}
	y += 2;
	cursor(ROXMESS+3, y++);
	sput("Starting at the ");
	if (curryo->actflag)
		sput("action");
	else
		sput("base");

	y++;
	tkyo(ROXMESS+4, y);
}

void	mdefryo(pryo)
	struct ryot	*pryo;
{
	pryo->ndballs = NDBALLS;
	pryo->naaa    = NAAA;
	pryo->nscarts = NSCARTS;

	pryo->edballs = EDBALLS;
	pryo->eaaa    = EAAA;
	pryo->escarts = ESCARTS;

	pryo->actflag = 0;
	pryo->invflag = (TRAININGVAL);
}

static void	tinvulnerable() {
	curryo->invflag |= TRAININGSELECTED;
	if (curryo->invflag&TRAININGVAL)
		curryo->invflag &= ~(TRAININGVAL|INVVAL);
	else
		curryo->invflag |= (TRAININGVAL|INVVAL);
}

static void	ryohelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Training Missions Information", GREEN, RED, WHITE);
	fileout("moag.inf", 6, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
}
