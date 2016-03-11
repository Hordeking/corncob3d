#include	<stdio.h>
#include	<io.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

INIT struct theatert	*theaters;
INIT int	ntheaters;

INIT struct thnamlist	*thhead = NULL;

extern void	chfrau(), actcu(), delth(), viewth(), getth(), mihelp(),
		go(), cinvulnerable(), quit(),cfltmodel(),crmsg();
#ifdef	GETTH
extern void	getth();
#endif	/* GETTH */
static struct cmdt	choices[] = {
	"\\Fly mission",				go,
	"\\Open a new theater of operations",		chfrau,
	"\\Activate an open theater",			actcu,
	"\\Delete a theater",				delth,
	"\\View status of a theater",			viewth,
#ifdef	GETTH
	"\\Get detailed info about a theater",		getth,
#endif	/* GETTH */
	"toggle invulnerable \\Plane",			cinvulnerable,
	"toggle aerodynamic \\Model",			cfltmodel,
	"toggle inflight \\Comments",			crmsg,
	"get \\Information about this menu",		mihelp,
	"\\Return to previous menu",			quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	mischoice;
void	choozmsn() {
	int far	*scr, far *scr2, far *scr3;
	int	x, y, c;
	struct cmdt	*ch;
	extern void	outchoice(), showchoices(), chunsetup();
	extern struct cmdt	*getchoice();
	extern struct pilot	trainee;

	if (ntheaters == 0) {
		x = MIXMESS+14;
		y = MIYMESS+4;
		simplesetup(x, y++, 47, 7, "No Theaters",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		xyprintf(x, y++,"Sorry, there are no theaters to choose from.");
		xyprintf(x, y++, "The Theaters of Action should be files");
		xyprintf(x, y++, "stored in the subdirectory `def'");
		beep();
		tkyo(++x, ++y);
		return;
	}
	if (curpilot->curtheater&0x80) {
		if (curpilot->ntheaters > 0) {
			curpilot->curtheater &= ~0x80;
			if (curpilot->curtheater > curpilot->ntheaters)
				curpilot->curtheater = 0;
		}
	}
	mischoice = DEFCHOICE;
	chunsetup();
	scr = yankscr();
	showchoices();

	if(curpilot == &trainee) {
		scr3=yankscr();
		x = MIXMESS+14;
		y = MIYMESS+4;
		simplesetup(x, y++, 47, 9, "Trainee Pilot",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		xyprintf(x, y++," You may fly combat missions as a trainee,");
		xyprintf(x, y++, " but your progress will be deleted when");
		xyprintf(x, y++, " you leave the program.  It is recommended");
		xyprintf(x, y++, " that you create a pilot by using the duty");
		xyprintf(x, y++, " roster selection from the main menu.");
		beep();
		tkyo(++x, ++y);
		putscr(scr3);
	color(BLUE, CYAN);
	}
	while ((ch = getchoice(choices, &mischoice, nchoices, outchoice,
				(int (*)())NULL))->ccmd != (void (*)())quit) {
		if (ch->ccmd == (void (*)())go && (curpilot->curtheater&0x80)) {
			scr2 = yankscr();
			x = MIXMESS+13;
			y = MIYMESS+4;
			simplesetup(x, y++, 51,
				(curpilot->ntheaters == 0) ? 7 : 8,
					"No Current Theater",
						LIGHTRED, WHITE, BLUE);
			x += 2;
			xyprintf(x, y++,
			   "Sorry, there is no active theater-of-operation.");
			cursor(x, y++);
			if (curpilot->ntheaters == 0) {
				hsput(
			   "You may choose a theater with the \"\\Open\" menu",
					50);
				xyprintf(x, y++,
				   "selection by typing `O'.");
			}
			else {
				sput(
			   "You may activate one on your theaters with the");
			   	cursor(x, y++);
				hsput(
			   "\"\\Activate\" menu selection by typing `A', or",
					50);
				cursor(x, y++);
				hsput(
			   "open a new one by typing `O'.",
			   		50);
			}
			beep();
			c = tkyo(++x, ++y);
			if (c == 'o' || c == 'O') {
				putscr(scr2);
				chfrau();
			}
			else if (c == 'a' || c == 'A') {
				putscr(scr2);
				actcu();
			}
			if (!(curpilot->curtheater&0x80)) {
				putscr(scr);
				go();
			}
			freescr(scr2);
		}
		else if (ch->ccmd == (void (*)())go) {
			go();
		}
		else
			(*ch->ccmd)();
		putscr(scr);
		showchoices();
	}
	freescr(scr);
}

INIT int	xrp;
INIT int	yrp;
static void	showchoices() {
	int	i, y;
	extern void	outchoice(), outths(), outpts();
	extern struct thnamlist	*getthnam();

	y = MIYMESS+1;
	color(BLUE, CYAN);
	xyprintf(MIXMESS+3, y, "Current Pilot:");
	fcolor(YELLOW);
	cursor(MIXMESS+4+14, y++);
	outpname(curpilot);
	fcolor(BLUE);
	xyprintf(MIXMESS+3, y++, "Active Theater :  ");
	fcolor(YELLOW);
	xyprintf(MIXMESS+6, y++, "%s", (curpilot->curtheater&0x80) ? "none" :
				      getthnam(curpilot->curtheater)->name);
	fcolor(BLUE);
	cursor(MIXMESS+35, MIYMESS+8+5);
	if (curryo->invflag&COMBATVAL) {
		sput("(Inv)");
		curryo->invflag |= INVVAL;
	}
	else {
		sput("(Std)");
		curryo->invflag &= ~INVVAL;
	}
	cursor(MIXMESS+32, MIYMESS+8+6);
	if (curryo->invflag&FLTMODVAL) {
		sput("(Advanced)");
	}
	else {
		sput("(Beginner)");
	}
	cursor(MIXMESS+35, MIYMESS+8+7);
	if (curryo->invflag&RMSGVAL) {
		sput("(Off)");
	}
	else {
		sput("(On)");
	}

	y++;
	fcolor(BRIGHTWHITE);
	xyprintf(MIXMESS+3, y, "Choose with arrow keys or letter");

	fcolor(BLUE);
	for (i = 0; i < nchoices; i++)
		outchoice(i);

	fcolor(YELLOW);
	xyprintf(xrp, MIYMESS+1, "Theaters-of-Operation:");
	fcolor(BLUE);
	outths();

	fcolor(YELLOW);
	xyprintf(xrp, yrp, "Your Open Theaters:");
	fcolor(BLUE);
	outpts();
}

static void	outchoice(i) {
#ifdef	GETTH
	cursor(MIXMESS+3, i+(MIYMESS+7));
#else	/* !GETTH */
	cursor(MIXMESS+3, i+(MIYMESS+8));
#endif	/* !GETTH */
	hsput(choices[i].scmd, LINELEN);
}

static void	chfrau() {
	int	n, t;
	extern int	selt(), mktheat(), checkstout();

	if (curpilot->ntheaters == MAXTHEATERS) {
		int	x, y;

		x = MIXMESS+13;
		y = MIYMESS+4;
		simplesetup(x, y++, 49, 7, "Too Many Theaters",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		cursor(x, y++);
		sput("Sorry, ");
		outpname(curpilot);
		sput(" already has");
		xyprintf(x, y++,
			"the maximum number of available theaters open.");
		xyprintf(x, y++, "You will have to delete one first.");
		beep();
		tkyo(++x, ++y);
		return;
	}
	n = selt();
	if (n < 0)
		return;

	if ((t = mktheat(n)) == -1)
		return;
	curpilot->theatnos[curpilot->ntheaters] = (unsigned char)t;
	curpilot->curtheater = curpilot->ntheaters;
	curpilot->ntheaters++;
	checkstout(curpilot->ntheaters-1);
}

int	mktheat(n)
	int	n;
{
	int	fileno, fdo, fdi;
	struct pilot	*pp;

	extern int	nextno(), wopen(), ropen();
	extern char	*mkthname(), *gtunivname();
	extern void	fdcopy(), *calloc(), free();

	fileno = nextno();
	if ((fdo = wopen(mkthname(fileno))) < 0) {
		errorbox("Cannot create theater for pilot.");
		return(-1);
	}
	if ((fdi = ropen(gtunivname(n))) < 0) {
		close(fdo);
		errorbox("Cannot theater def file.");
		return(-1);
	}

	if ((pp = (struct pilot *)calloc(1, sizeof(struct pilot))) == NULL) {
		errorbox("Cannot allocate memory for pilot structure.");
		close(fdo);
		close(fdi);
		return(-1);
	}
	strcpy(pp->name, curpilot->name);
	pp->rank = curpilot->rank;
	write(fdo, (void *)&theaters[n], sizeof(struct theatert));
	write(fdo, (void *)pp, sizeof(struct pilot));
	free(pp);

	fdcopy(fdo, fdi);

	close(fdo);
	close(fdi);

	return(fileno);
}

int	nextno() {
	int	i, fd;
	extern char	*mkthname();
	extern int	ropen();

	for (i = 1; i < 30000; i++) {
		if ((fd = ropen(mkthname(i))) < 0)
			break;
		close(fd);
	}
	return(i);
}

void	fdcopy(fo, fi)
	int	fo, fi;
{
	char	*p;
	extern void	*malloc(), free();
	unsigned int	u, n, cl;
	extern unsigned int	coreleft();

	cl = coreleft();
/*
savepos();
xyprintf(1, 1, "in fdcopy, cl = %u            ", cl);
beep();mygetch();
restpos();
*/
	u = cl - 18;
	u = (u/1024)*1024;
	while ((p = (char *)malloc(u)) == NULL) {
		if (u < 1024) {
			free(p);
			errorbox("Cannot alloc p in fdcopy, cl=%u", cl);
			return;
		}
		u -= 1024;
	}

	while ((n = read(fi, p, u)) > 0)
		write(fo, (void *)p, n);
	free(p);
}

INIT int	stoutthno;
INIT int	noutths;
INIT int	outthflag;
#define	OUTTOP	1
#define	OUTBOT	2

static int	selt() {
	int	c, cur, last;
	char	buf[LINELEN], *p;
	extern void	outth(), outths(), outthdesc();

	simplesetup(MIXMESS+1, MIYMESS+1, MIXWID-3-DRPWIDP-2, MIYWID-3,
		"Choose a Theater of Operations", RED, WHITE, RED);
	xyprintf(MIXMESS+3, MIYMESS+2,
		"Choose desired theater of operations with");
	xyprintf(MIXMESS+3, MIYMESS+3,
		"\030\031.  Select theater of operations with");
	xyprintf(MIXMESS+3, MIYMESS+4,
		"<CR> or abort with <BS> or <ESC>.");
	p = buf;
	*p++ = SLEFTMIDDLE;
	for (c = 0; c < MIXWID-3-DRPWIDP-2-2; c++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	*p = '\0';
	cursor(MIXMESS+1, MIYMESS+5);
	sput(buf);

	if (stoutthno > 0)
		cur = 1;
	else
		cur = 0;
	last = cur;
	color(BRIGHTWHITE, CYAN);
	outth(cur);
	outthdesc(cur + stoutthno);
	color(BRIGHTWHITE, CYAN);
	while (1) {
		c = spgetch();
		if (c == 033 || c == 010)
			return(-1);
		if (c == '\n')
			return(cur + stoutthno);
		else if (c == UPARROW || c == LEFTARROW) {
			if (ntheaters > noutths) {
				if (cur == (outthflag&OUTTOP) ? 1 : 0) {
					if (stoutthno == 0)
						beep();
					else {
						stoutthno--;
						color(BLUE, CYAN);
						outths();
					}
				}
				else
					cur--;
			}
			else {
				if (cur == 0)
					cur = ntheaters-1;
				else
					cur--;
			}
		}
		else if (c == DOWNARROW  || c == RIGHTARROW) {
			if (ntheaters > noutths) {
				if (cur == noutths - ((outthflag&OUTBOT)?
								2 : 1) ) {
					if (stoutthno == ntheaters-noutths)
						beep();
					else {
						stoutthno++;
						color(BLUE, CYAN);
						outths();
					}
				}
				else
					cur++;
			}
			else {
				if (cur == ntheaters-1)
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
		outth(last);
		fcolor(BRIGHTWHITE);
		outth(cur);
		outthdesc(cur + stoutthno);
		last = cur;
	}
#ifndef	__TURBOC__
	return(0);
#endif
}

static void	outths() {
	int	i;
	extern void	outth();

	outthflag = 0;
	for (i = 0; i < noutths; i++) {
		if (i + stoutthno >= ntheaters)
			break;
		if (i == 0 && stoutthno > 0) {
			outthflag |= OUTTOP;
			outth(-1);
		}
		else if (i == noutths-1 && stoutthno+noutths < ntheaters) {
			outthflag |= OUTBOT;
			outth(-2);
		}
		else
			outth(i);
	}
}

static char	more[] = "                    MORE  ";
static int	nm = sizeof(more) - 2;
static void	outth(i)
	int	i;
{
	char	*p;
	struct theatert	*th;
	int	fc, bc;

	extern int	forecolor, backcolor;
	extern void	savepos(), restpos();

	savepos();
	if (i == -1) {
		cursor(xrp, MIYMESS+2 + 0);
		p = more;
		p[nm] = '\030';
		color(BRIGHTWHITE, CYAN);
	}
	else if (i == -2) {
		cursor(xrp, MIYMESS+2 + noutths-1);
		p = more;
		p[nm] = '\031';
		color(BRIGHTWHITE, CYAN);
	}
	else {
		cursor(xrp, MIYMESS+2 + i);
		th = &theaters[i+stoutthno];
		fc = forecolor;
		bc = backcolor;
		if (th->isreq) {
			color(YELLOW, CYAN);
			cput(254);
			color(fc, bc);
		}
		else
			cput(' ');
		p = th->name;
	}

	sput(p);
	for (i = strlen(p); i < DRPWIDP-1; i++)
		cput(' ');
	restpos();
}

static void	outthdesc(n)
	int	n;
{
	FILE	*fp;
	int	x, i, y, yy;
	char	buf[LINELEN], *p;
	extern char	*gttwrname(), *fngets();
	extern void	noinfo(), center();

	x = MIXMESS+2;
	y = MIYMESS+6;
	center(buf, MIXWID-3-DRPWIDP-2-2, theaters[n].name);
	cursor(x, y);
	color(YELLOW, WHITE);
	sput(buf);
	y += 2;
	p = buf;
	for (i = 0; i <= MIXWID-3-DRPWIDP-2-3; i++)
		*p++ = ' ';
	*p = '\0';
	for (yy = y; yy < MIYMESS+MIYWID-3; yy++)
		xyprintf(x, yy, buf);
		
	fcolor(BLUE);
	if ((fp = fopen(gttwrname(n), "rb")) == NULL) {
		noinfo();
		return;
	}
	fngets(buf, LINELEN, fp);
	if (buf[0] == '\0' || buf[0] == 12) {
		noinfo();
		fclose(fp);
		return;
	}
	x++;
	yy = y;
	while (fngets(buf, LINELEN, fp) != NULL) {
		if (buf[0] == 12)
			break;
		if (y == yy && buf[0] == '\0') {
			noinfo();
			break;
		}
		if (y == MIYMESS+MIYWID-3)
			break;
		cursor(x, y++);
		hsput(buf, MIXWID-3-DRPWIDP-2-3);
	}
	if (y == yy)
		noinfo();
	fclose(fp);
}

static void	noinfo() {
	xyprintf(MIXMESS+2, MIYMESS+8,
			"There is no general information available");
	xyprintf(MIXMESS+2, MIYMESS+9,
			"about this Theater of Operations.  You");
	xyprintf(MIXMESS+2, MIYMESS+10,
			"may consult the per-mission information");
	xyprintf(MIXMESS+2, MIYMESS+11,
			"before flying or obtain more information");
	xyprintf(MIXMESS+2, MIYMESS+12,
			"by pressing <F2> during flight.");
}

struct thnamlist	*getthnam(n)
	int	n;
{
	struct thnamlist	*pth, *pth2;
	struct theatert	thistheat;
	struct pilot	thispilot;
	int	fd;
	extern void	*calloc();
	extern int	ropen();
	extern char	*mkthname();

	if (n >= curpilot->ntheaters) {
		errorbox("Cannot get theater #%d name -- pilot has too few.",n);
		return(NULL);
	}

	for (pth = thhead; pth != NULL; pth = pth->next)
		if (pth->fno == curpilot->theatnos[n])
			break;
	if (pth != NULL)
		return(pth);

	if ((fd = ropen(mkthname(curpilot->theatnos[n]))) < 0) {
		errorbox("Cannot open pilots theater file for reading.");
		return(NULL);
	}
	if (read(fd, (void *)&thistheat, sizeof(struct theatert)) !=
						sizeof(struct theatert)) {
		errorbox("Cannot read theater structure from theater file.");
		close(fd);
		return(NULL);
	}
	if (read(fd, (void *)&thispilot, sizeof(struct pilot)) !=
						sizeof(struct pilot)) {
		errorbox("Cannot read pilot structure from theater file.");
		close(fd);
		return(NULL);
	}
	close(fd);
	if ((pth2 = (struct thnamlist *)calloc(1, sizeof(struct thnamlist)))
						== NULL) {
		errorbox("Cannot allocate memory to add theater into list.");
		return(NULL);
	}
	pth2->fno = curpilot->theatnos[n];
	strcpy(pth2->name, thistheat.name);
	pth2->nstoredplanes = thistheat.nstoredplanes;
	pth2->nobjectives = thistheat.nobjectives;
	pth2->ncobjectives = thispilot.totalmiss.st3.objectives;
	pth2->next = NULL;
	pth2->completed = thistheat.completed;

	if (thhead == NULL)
		thhead = pth2;
	else {
		for (pth = thhead; pth->next != NULL; pth = pth->next);
		pth->next = pth2;
	}
	return(pth2);
}

static void	actcu() {
	int	n;
	extern int	viewpth();

	if (curpilot->ntheaters == 0) {
		int	x, y;

		x = MIXMESS+5;
		y = MIYMESS+3;
		simplesetup(x, y++, 49, 7, "No Theaters to Activate",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		cursor(x, y++);
		sput("Sorry, ");
		outpname(curpilot);
		sput(" has no");
		xyprintf(x, y++,
			"open theaters to activate.");
		xyprintf(x, y++, "You will have to open one first.");
		beep();
		tkyo(++x, ++y);
		return;
	}

 	n = viewpth("Activate an Open Theater",
		"Select theater with \030\031, <CR> to activate");
	if (n < 0)
		return;
	curpilot->curtheater = (unsigned char)n;
}

static void	delth() {
	struct thnamlist	*pth, *pth1;
	int	n, i, x, y;
	extern int	viewpth();
	extern char	*mkthname();

	if (curpilot->ntheaters == 0) {
		x = MIXMESS+5;
		y = MIYMESS+4;
		simplesetup(x, y++, 49, 7, "No Theaters to Delete",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		cursor(x, y++);
		sput("Sorry, ");
		outpname(curpilot);
		sput(" has no");
		xyprintf(x, y++,
			"open theaters to delete.");
		xyprintf(x, y++, "You will have to open one first.");
		beep();
		tkyo(++x, ++y);
		return;
	}

 	n = viewpth("Delete a Theater",
		"Select theater with \030\031, <CR> to Delete");
	if (n < 0)
		return;
	if ((pth = getthnam(n)) == NULL)
		return;
	if (pth->completed) {
		x = MIXMESS+5;
		y = MIYMESS+4;
		simplesetup(x, y++, 50, 8, "Theater is Complete",
						MAGENTA, BROWN, YELLOW);
		x += 2;
		xyprintf(x, y++,
			"Warning!  theater %s", pth->name);
		xyprintf(x, y++,
			"has already been completed!  Deleted theaters");
		xyprintf(x, y++,
			"can not be counted toward Combat Master status.");
		y++;
		xyprintf(x, y++,
			"Are you sure you want to delete this theater?");
		xyprintf(x, y++,
			"Type `y' to confirm, anything else cancels ");
		i = spgetch();
		if (i != 'y' && i != 'Y')
			return;
	}

	unlink(mkthname(curpilot->theatnos[n]));
	for (pth1 = thhead; pth1 != NULL; pth1 = pth1->next) {
		if (pth1->fno == curpilot->theatnos[n]) {
			if (pth1 == thhead)
				thhead = thhead->next;
			else {
				for (pth = thhead; pth != pth1; pth = pth->next)
					if (pth->next == pth1)
						break;
				pth->next = pth1->next;
			}
			break;
		}
	}

	if (curpilot->curtheater == n) {
		if (curpilot->ntheaters > 0)
			curpilot->curtheater = 0;
		else
			curpilot->curtheater = (unsigned char)0x80;
	}
	else if (curpilot->curtheater > n)
		curpilot->curtheater--;
	for (i = n+1; i < curpilot->ntheaters; i++)
		curpilot->theatnos[i-1] = curpilot->theatnos[i];
	curpilot->ntheaters--;
	if (curpilot->ntheaters == 0)
		curpilot->curtheater = (unsigned char)0x80;
}

static void	viewth() {
	int far	*scr;
	int	theat, vflag, c;

	extern struct pilot	*getpilot();
	extern int	viewpth();
	extern void	pmissrep();
	extern struct thnamlist	*getthnam();

	if (curpilot->ntheaters == 0) {
		int	x, y;

		x = MIXMESS+5;
		y = MIYMESS+5;
		simplesetup(x, y++, 49, 7, "No Theaters to View",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		cursor(x, y++);
		sput("Sorry, ");
		outpname(curpilot);
		sput(" has no");
		xyprintf(x, y++,
			"open theaters to view.");
		xyprintf(x, y++, "You will have to choose one first.");
		beep();
		tkyo(++x, ++y);
		return;
	}

	vflag = 0;
	scr = yankscr();
	while (1) {
		if (!vflag) {
			putscr(scr);
			theat = viewpth("View Theater Records",
			 "Select theater with \030\031, <CR> to View");
			if (theat < 0)
				break;
			vflag = 1;
		}

		pmissrep(getpilot(theat), 1, getthnam(theat)->name);

		c = spgetch();
		if (c == 033 || c == 010 || c == '\n') {
			vflag = 0;
			continue;
		}
		else if (c == UPARROW || c == LEFTARROW) {
			if (theat == 0)
				theat = curpilot->ntheaters - 1;
			else
				theat--;
		}
		else if (c == DOWNARROW || c == RIGHTARROW) {
			if (theat == curpilot->ntheaters-1)
				theat = 0;
			else
				theat++;
		}
		else {
			beep();
			continue;
		}
	}
	freescr(scr);
}

INIT int	stoutptno;
INIT int	noutpts;
INIT int	outptflag;

static int	viewpth(s, s2)
	char	*s, *s2;
{
	int	theat, c;
	struct pilot	*pp;
	extern struct pilot	*getpilot();
	extern struct thnamlist	*getthnam();
	extern int	checkstout();
	extern void	showpilot(), outpt(), outpts();

	if (curpilot->curtheater&0x80) {
		if (curpilot->ntheaters == 0) {
			errorbox("Current pilot has no theaters.");
			return(0);
		}
		theat = 0;
	}
	else
		theat = curpilot->curtheater;

	theat = checkstout(theat);
	color(BLUE, CYAN);
	outpts();

	while (1) {
		pp = getpilot(theat);
		color(CYAN, BLUE);
		outpt(theat);/*here*/
		showpilot(pp, s, s2, getthnam(theat+stoutptno));
		c = spgetch();
		if (c == 033 || c == 010)
			return(-1);
		else if (c == '\r' || c == '\n')
			return(theat+stoutptno);
		else if (c == UPARROW || c == LEFTARROW) {
			if (curpilot->ntheaters > noutpts) {
				if (theat == (outptflag&OUTTOP) ? 1 : 0) {
					if (stoutptno == 0)
						beep();
					else {
						stoutptno--;
						color(BLUE, CYAN);
						outpts();
					}
				}
				else {
					color(BLUE, CYAN);
					outpt(theat);/*here*/
					theat--;
				}
			}
			else {
				color(BLUE, CYAN);
				outpt(theat);/*here*/
				if (theat == 0)
					theat = curpilot->ntheaters - 1;
				else
					theat--;
			}
		}
		else if (c == DOWNARROW || c == RIGHTARROW) {
			if (curpilot->ntheaters > noutpts) {
				if (theat == noutpts - ((outptflag&OUTBOT)?
								2 : 1) ) {
					if (stoutptno == curpilot->ntheaters
								-noutpts)
						beep();
					else {
						stoutptno++;
						color(BLUE, CYAN);
						outpts();
					}
				}
				else {
					color(BLUE, CYAN);
					outpt(theat);/*here*/
					theat++;
				}
			}
			else {
				color(BLUE, CYAN);
				outpt(theat);/*here*/
				if (theat == curpilot->ntheaters-1)
					theat = 0;
				else
					theat++;
			}
		}
		else {
			beep();
			continue;
		}
	}
#ifndef	__TURBOC__
	return(0);
#endif
}

int	checkstout(t)
	int	t;
{
	if (t < stoutptno) {
		stoutptno = t;
		t = 0;
	}
	else if (t > stoutptno+ noutpts-1) {
		stoutptno = t - (noutpts-1);
		t = (noutpts-1);
	}
	else
		t -= stoutptno;
	if (t == noutpts-1 && curpilot->ntheaters > stoutptno + noutpts) {
		stoutptno++;
		t--;
	}
	return(t);
}

static void	outpts() {
	int	i;
	extern void	outpt();

	outptflag = 0;
	for (i = 0; i < noutpts; i++) {
		if (i + stoutptno >= curpilot->ntheaters)
			break;
		if (i == 0 && stoutptno > 0) {
			outptflag |= OUTTOP;
			outpt(-1);
		}
		else if (i == noutpts-1 && stoutptno+noutpts <
							curpilot->ntheaters) {
			outptflag |= OUTBOT;
			outpt(-2);
		}
		else
			outpt(i);
	}
}

static void	outpt(i)
	int	i;
{
	struct thnamlist	*pth;
	int	fc, bc, comp;
	char	*p;

	extern struct thnamlist	*getthnam();
	extern int	forecolor, backcolor;
	extern void	savepos(), restpos();

	savepos();
	comp = 0;
	if (i == -1) {
		cursor(xrp, yrp+1+0);
		p = more;
		p[nm] = '\030';
		color(BRIGHTWHITE, CYAN);
	}
	else if (i == -2) {
		cursor(xrp, yrp+1+noutpts-1);
		p = more;
		p[nm] = '\031';
		color(BRIGHTWHITE, CYAN);
	}
	else {
		cursor(xrp, yrp+1+i);
		pth = getthnam(i+stoutptno);
		p = pth->name;
		if (i+stoutptno == curpilot->curtheater)
			cput(020);
		else
			cput(' ');
		if (pth->completed)
			comp = 1;
	}

	sput(p);
	if (comp) {
		fc = forecolor;
		bc = backcolor;
		color(RED, bc);
		cput(3);
		fcolor(fc);
	}
	else
		cput(' ');
	for (i = strlen(p) + 1; i < THEATNAMLEN; i++)
		cput(' ');
	restpos();
}

static struct pilot	*getpilot(n)
	int	n;
{
	char	*p;
	int	fd;
	static struct pilot	tp;
	extern char	*mkthname();
	extern int	ropen();

	p = mkthname(curpilot->theatnos[n]);
	if ((fd = ropen(p)) < 0)
		return(NULL);
	if (lseek(fd, (long)sizeof(struct theatert), 0) < 0) {
		errorbox("Seek into pilots theater file failed.");
		close(fd);
		return(NULL);
	}
	if (read(fd, (void *)&tp, sizeof(struct pilot)) !=sizeof(struct pilot)){
		errorbox(
			"Cannot read pilot structure from pilot theater file.");
		close(fd);
		return(NULL);
	}
	close(fd);
	return(&tp);
}

static char	namebuf[40];
char	*mkthname(n)
	int	n;
{
#ifdef MVPCD
extern	char	wpath[80];
extern	char	scrlist[80];
	sprintf(scrlist, "tht\\%d.tht", n);
	strcpy(namebuf,wpath);
	strcat(namebuf,scrlist);
	return(namebuf);
#else
	sprintf(namebuf, "tht\\%d.tht", n);
	return(namebuf);
#endif /* MVPCD */
}

char	*gttwrname(n)
	int	n;
{
	sprintf(namebuf, "def\\%s.twr", theaters[n].fname);
	return(namebuf);
}

char	filedeftwr[] = "deftower";
char	*mktwrname(s)
	char	*s;
{
	if (s == NULL)
		sprintf(namebuf, "def\\%s.twr", filedeftwr);
	else
		sprintf(namebuf, "def\\%s.twr", s);
	return(namebuf);
}

char	*gtunivname(n)
	int	n;
{
#ifdef	REGISTERED
	sprintf(namebuf, "def\\%s.def", theaters[n].fname);
#else	/* !REGISTERED */
	sprintf(namebuf, "def\\%s.cct", theaters[n].fname);
#endif	/* !REGISTERED */
	return(namebuf);
}

#ifdef	GETTH
static void	getth() {
	int	x, y;

	extern int	towerno, newtower(), copytheat();
	extern char	fileinf[]
#ifndef MVPCD
	extern fileuniv[], filetower[];
#else
extern	char	wpath[80];
extern	char	scrlist[80];
#endif

	if (curpilot->ntheaters == 0) {
		x = MIXMESS+5;
		y = MIYMESS+6;
		simplesetup(x, y++, 49, 7, "No Open Theaters",
						LIGHTRED, WHITE, BLUE);
		x += 2;
		cursor(x, y++);
		sput("Sorry, ");
		outpname(curpilot);
		sput(" has no");
		xyprintf(x, y++,
			"open theaters to get information about.");
		xyprintf(x, y++, "You will have to open one first.");
		beep();
		tkyo(++x, ++y);
		return;
	}
	towerno = 4;
	if (!copytheat())
		return;
	newtower(towerno, 1);
/*	unlink(fileinf);	/**/
	unlink(fileuniv);
	unlink(filetower);
}
#endif	/* GETTH */

static void	crmsg() {
	register char iflg;
	iflg=curryo->invflag;
	curryo->invflag = (iflg & ~RMSGVAL) | (~iflg & RMSGVAL); 
}

static void	cfltmodel() {
	register char iflg;
	iflg=curryo->invflag;
	curryo->invflag = (iflg & ~FLTMODVAL) | (~iflg & FLTMODVAL); 
}

static void	cinvulnerable() {
	curryo->invflag |= COMBATSELECTED;
	if (curryo->invflag&COMBATVAL)
		curryo->invflag &= ~(COMBATVAL|INVVAL);
	else
		curryo->invflag |= COMBATVAL|INVVAL;
}

static void	mihelp() {
	extern int	fileout();
	extern void	nbmplesetup();

	nbmplesetup(MIXMESS, MIYMESS, MIXWID, MIYWID,
			"Information about Theaters Menu", GREEN, RED, WHITE);
	fileout("moag.inf", 3, MIXMESS+2, MIYMESS+1, MIXWID-3, MIYWID-2,
								WHITE, RED, 4);
}

static char	*msg = "Theater of Operations Menu";
static void	chunsetup() {
	int	i, c1, c2, nr, y;
	char	buf[LINELEN], buf2[LINELEN], *p;
	extern int	menuwid;
	extern void	center();

	cursor(menux, menuy);
	color(BLUE, CYAN);
	center(buf, menuwid, msg);
	sput(buf);

	c2 = DRPWIDP+1;
	c1 = MIXWID - c2 - 3;

	xrp = MIXMESS + c1 + 3;
	nr = (MIYWID - 8)/2 + 3;
	noutths = nr - 1;

	fcolor(BROWN);
	y = MIYMESS;
	cursor(MIXMESS, y++);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 0; i < c1; i++)
		*p++ = STOP;
	*p++ = STMIDDLE;
	for (i = 0; i < c2; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf2;
	*p++ = SLEFT;
	for (i = 0; i < c1; i++)
		*p++ = ' ';
	*p++ = SVMIDDLE;
	for (i = 0; i < c2; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';

	for (i = 0; i < nr; i++) {
		cursor(MIXMESS, y++);
		fcolor(BROWN);
		sput(buf2);
	}

	cursor(MIXMESS, y++);
	fcolor(BROWN);
	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < c1; i++)
		*p++ = ' ';
	*p++ = SLEFTMIDDLE;
	for (i = 0; i < c2; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	*p = '\0';
	sput(buf);

	noutpts = nr - 1;
	yrp = y;
	for (i = 0; i < nr; i++) {
		cursor(MIXMESS, y++);
		fcolor(BROWN);
		sput(buf2);
	}

	cursor(MIXMESS, y++);
	fcolor(BROWN);
	p = buf;
	*p++ = SLOWERLEFT;
	for (i = 0; i < c1; i++)
		*p++ = SBOTTOM;
	*p++ = SBMIDDLE;
	for (i = 0; i < c2; i++)
		*p++ = SBOTTOM;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);

	fcolor(BLUE);
}
