#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

INIT struct pilot	pilots[MAXPILOTS];
INIT struct pilot	trainee;
INIT struct pilot	*curpilot = &trainee;
INIT int	npilots = 0;

extern void	actvpilot(), deactvpilot(), creatpilot(), deletpilot(),
		renamepilot(), viewpilot(), respilot(), rohelp(), quit();
static struct cmdt	choices[] = {
	"\\Create Pilot",				creatpilot,
	"\\Activate Pilot",				actvpilot,
	"\\Deactivate Pilot",				deactvpilot,
	"de\\Lete Pilot",				deletpilot,
	"r\\Esurrect Pilot",				respilot,
	"re\\Name Pilot",				renamepilot,
	"\\View Pilot Records",				viewpilot,
	"get \\Information about this menu",		rohelp,
	"\\Return to previous menu",			quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	roschoice;
void	roster() {
	int far	*scr;
	struct cmdt	*ch;
	extern void	outchoice(), showchoices(), rostersetup();
	extern struct cmdt	*getchoice();

	roschoice = DEFCHOICE;
	rostersetup();
	scr = yankscr();
	showchoices();
	while ((ch =
	        getchoice(choices, &roschoice, nchoices, outchoice,
				(int (*)())NULL))->ccmd != (void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		showchoices();
	}
	freescr(scr);
}

static void	showchoices() {
	int	i;
	extern void	outchoice(), outpilots();

	color(BLUE, CYAN);
	cursor(DRXMESS+3, DRYMESS+2);
	sput("Use arrow keys or type highlighted letter");
	cursor(DRXMESS+3, DRYMESS+3);
	sput("        to choose action:");

	for (i = 0; i < nchoices; i++)
		outchoice(i);

	outpilots();

	cursor(DRXMESS+2, DRYMESS+DRYWID-3);
	sput("Active pilot:  ");
	outpname(curpilot);
}

static void	outchoice(i) {
	cursor(DRXMESS+9, i+(DRYMESS+5));
	hsput(choices[i].scmd, LINELEN);
}

static int	cmppilots(p1, p2)
	struct pilot	*p1, *p2;
{
	return(stricmp(p1->name, p2->name));
}

static void	outpilots() {
	int	i;
	extern void	qsort();

	qsort(pilots, npilots, sizeof(struct pilot), cmppilots);
	for (i = 0; i < npilots; i++) {
		cursor(DRXMESS+DRXWID-DRPWIDP-2, DRYMESS+3+i);
		if (pilots[i].pflag&PACTIVE) {
			curpilot = &pilots[i];
			curryo = &curpilot->ryo;
			cput(020);
		}
		else
			cput(' ');
		outpname(&pilots[i]);
	}
}

static void	deactvpilot() {
	if (npilots == 0 || !(curpilot->pflag&PACTIVE)) {
		simplesetup(DRXMESS+11, DRYMESS+8, 52, 6,
				"No Pilot to deactivate", LIGHTRED, WHITE, RED);
		beep();
		xyprintf(DRXMESS+13, DRYMESS+9,
			"There is no pilot in the list to be deactivated.");
		xyprintf(DRXMESS+13, DRYMESS+10,
					"You will have to activate one first.");
		tkyo(DRXMESS+14, DRYMESS+12);
		return;
	}
	curpilot->pflag &= ~PACTIVE;
	curpilot = &trainee;
	curryo = &curpilot->ryo;
}

static void	actvpilot() {
	char	*p;
	int	i, y, c;
	long	now;

	extern long	time();
	extern int	viewps();
	extern char	*ctime();

	if (npilots == 0) {
		y = DRYMESS+7;
		simplesetup(DRXMESS+11, y++, 52, 6,
				"No Pilot to activate", LIGHTRED, WHITE, RED);
		beep();
		xyprintf(DRXMESS+13, y++,
			"There are no pilots in the list to be activated.");
		xyprintf(DRXMESS+13, y++, "You will have to create one first.");
		y++;
		tkyo(DRXMESS+14, y);
		return;
	}
	i = viewps("Choose Active Pilot",
				"Select pilot with \030\031, <CR> to activate");
	if (i < 0)
		return;
	time(&now);
	if (pilots[i].pflag&(PCAPTURED|PKILLED|PFROZEN) 
						| pilots[i].stockade > now) {
		y = DRYMESS+7;
		simplesetup(DRXMESS+5, y++, 65, 6,
			"Pilot Cannot be Activated", LIGHTRED, WHITE, RED);
		beep();
		cursor(DRXMESS+7, y++);
		outpname(&pilots[i]);
		sput(" has been ");
#ifdef	OLDSHARE
		if (pilots[i].pflag&PFROZEN) {
			sput("frozen until CORNCOB-3D");
			cursor(DRXMESS+7, y++);
	hsput("is registered.  Please see the \\Registration selection under",
								65);
			xyprintf(DRXMESS+7, y++, "the main menu for details.");
			tkyo(DRXMESS+9, y);
			return;
		}
		else
#endif	/* OLDSHARE */
		if (pilots[i].pflag&PCAPTURED)
			sput("captured by the enemy.");
		else if (pilots[i].pflag&PKILLED)
			sput("killed in action.");
		else {
			sput("confined to the stockade.");
			p = ctime(&pilots[i].stockade);
			p[24] = '\0';
			xyprintf(DRXMESS+7, y++,"Release is scheduled for %s.", 
							p);
			y++;
			tkyo(DRXMESS+8, y);
			return;
		}
		y++;
		xyprintf(DRXMESS+7, y++,
			"Do you want to resurrect the pilot?  (Y or N)");
		c = spgetch();
		if ((c|0x20) != 'y')
			return;
		pilots[i].pflag &= ~(PKILLED|PCAPTURED);
		pilots[i].nresurected++;
	}
	curpilot->pflag &= ~PACTIVE;
	curpilot = &pilots[i];
	curryo = &curpilot->ryo;
	curpilot->pflag |= PACTIVE;
}

static void	creatpilot() {
	char	*p;
	int	c;
	extern struct pilot	nullpilot;
	extern void	mdefryo();
	extern char	*getpname();

	if (npilots == MAXPILOTS) {
		simplesetup(DRXMESS+5, DRYMESS+8, 65, 6,
			"Too many pilots", LIGHTRED, WHITE, RED);
		beep();
		xyprintf(DRXMESS+7, DRYMESS+9,
			"A new pilot cannot be created because the pilot list");
		sput(" is full.");
		xyprintf(DRXMESS+7, DRYMESS+10,
					"You will have to delete one first.");
		tkyo(DRXMESS+8, DRYMESS+12);
		return;
	}

	simplesetup(DRXMESS+2, DRYMESS+4, DRPWID+27, 8,
				"Create New Pilot", MAGENTA, WHITE, RED);
	xyprintf(DRXMESS+4, DRYMESS+5, "Enter name for new pilot.");
	if ((p = getpname(DRXMESS+4, DRYMESS+6)) == NULL)
		return;

	pilots[npilots] = nullpilot;
	mdefryo(&pilots[npilots].ryo);
	strcpy(pilots[npilots].name, p);
	pilots[npilots].curtheater = (unsigned char)0x80;
	npilots++;
	xyprintf(DRXMESS+4, DRYMESS+9, "Activate %s? [Yes or No]", p);
	c = spgetch();
	if (c == 'Y' || c == 'y' || c == '\r' || c == '\n') {
		curpilot->pflag &= ~PACTIVE;
		curpilot = &pilots[npilots-1];
		curryo = &curpilot->ryo;
		curpilot->pflag |= PACTIVE;
	}
}

static char	*getpname(x, y)
	int	x, y;
{
	int	c, i;
	char	*p1, *p, *pmax;
	static char	buf[LINELEN];

	xyprintf(x, y++, "(Maximum of %d characters)", DRPWID);
	cursor(x+1, y++);
	p1 = buf;
	pmax = &p1[DRPWID];
	p = p1;
	*p = '\0';
	while (1) {
		cput('_');
		cback();
		c = spgetch();
		if (c == 033) {
			*p1 = '\0';
			break;
		}
		if (c == 010) {
			if (p == p1)
				continue;
			p--;
			cput(' ');
			cback();
			cback();
			cput(' ');
			cback();
		}
		else if (c == '\r' || c == '\n') {
			*p = '\0';
			break;
		}
		else {
			if (p == pmax) {
				beep();
			}
			else {
				*p++ = (char)c;
				cput(c);
			}
		}
	}
	if (p1[0] == '\0')
		return(NULL);
	for (i = 0; i < npilots; i++)
		if (strcmp(p1, pilots[i].name) == 0)
			break;
	if (i != npilots) {
		beep();
		y++;
		xyprintf(x, y++, "Sorry, Pilot %s already exists.", p1);
		tkyo(x+1, y++);
		return(NULL);
	}
	return(buf);
}

static void	deletpilot() {
	int	i, n;
	extern int	viewps();
	extern void 	delpilot();

	if (npilots == 0) {
		simplesetup(DRXMESS+11, DRYMESS+9, 47, 6,
			"No Pilots to Delete", LIGHTRED, WHITE, RED);
		xyprintf(DRXMESS+13, DRYMESS+10,
			"There are no pilots in the list to delete.");
		xyprintf(DRXMESS+13, DRYMESS+11,
					"You will have to create one first.");
		tkyo(DRXMESS+14, DRYMESS+13);
		return;
	}
	n = viewps("Choose Pilot to Delete",
			"Select pilot with \030\031, <CR> to delete");

	if (n < 0)
		return;
	simplesetup(DRXMESS+2, DRYMESS+4, DRPWIDP+28, 7,
				"Confirm Deletion", MAGENTA, BROWN, YELLOW);
	xyprintf(DRXMESS+4, DRYMESS+6, "Type <ENTER> to delete ");
	outpname(&pilots[n]);
	xyprintf(DRXMESS+4, DRYMESS+7, " Almost anything else cancels...");
	i = spgetch();
	if (i != '\n')
		return;
	delpilot(n);
}

void	delpilot(n)
	int	n;
{
	int	i, fno;
	struct thnamlist	*pth, *ppth;
	extern struct thnamlist	*thhead;
	extern void	markdead();
	extern char	*mkthname();

	for (i = 0; i < pilots[n].ntheaters; i++) {
		fno = pilots[n].theatnos[i];
		unlink(mkthname(fno));
		for (pth = thhead,ppth = thhead; pth != NULL; pth = pth->next) {
			if (pth->fno == fno) {
				if (pth == thhead)
					thhead = pth->next;
				else
					ppth->next = pth->next;
			}
			else
				ppth = pth;
		}
	}
	if (curpilot == &pilots[n]) {
		curpilot = &trainee;
		curryo = &curpilot->ryo;
	}
	markdead(pilots[n].name);
	for (i = n; i < npilots; i++) {
		pilots[i] = pilots[i+1];
		if (pilots[i].pflag&PACTIVE) {
			curpilot = &pilots[i];
			curryo = &curpilot->ryo;
		}
	}
	npilots--;
}

int	mmode;
static void	viewpilot() {
	int	pilot, c, vflag;
	int far	*scr;
	extern int	viewps();
	extern void	pmissrep();

	if (npilots == 0) {
		simplesetup(DRXMESS+11, DRYMESS+4, 48, 6,
			"No Pilots to View", LIGHTRED, WHITE, RED);
		beep();
		xyprintf(DRXMESS+13, DRYMESS+5,
			"There are no pilots in the list to be viewed");
		xyprintf(DRXMESS+13, DRYMESS+6,
					"You will have to create one first.");
		tkyo(DRXMESS+14, DRYMESS+8);
		return;
	}

	vflag = 0;
	scr = yankscr();
	if (curpilot == &trainee)
		pilot = 0;
	else
		pilot = curpilot - pilots;
	mmode = 0;
	while (1) {
/*
		if (!vflag) {
			putscr(scr);
			pilot = viewps("View Pilot Records",
			   "Select pilot with \030\031, <CR> to view");
			if (pilot < 0)
				break;
			vflag = 1;
		}
 */
		pmissrep(&pilots[pilot], 0, NULL);

		c = spgetch();
		if (c == 033 || c == 010 || c == '\n') {
/*
			vflag = 0;
			continue;
 */
			setblink(1);
 			break;
		}
		else if (c == INSERT) {
			if (mmode == 1)
				mmode = 0;
			else
				mmode = 1;
		}
		else if (c == UPARROW || c == LEFTARROW) {
			if (pilot == 0)
				pilot = npilots - 1;
			else
				pilot--;
		}
		else if (c == DOWNARROW || c == RIGHTARROW) {
			if (pilot == npilots-1)
				pilot = 0;
			else
				pilot++;
		}
		else {
			beep();
			continue;
		}
	}
	freescr(scr);
}

static void	respilot() {
	char	*p;
	int	i, n, fd;
	long	now;

	struct pilot	tp, *pp;
	extern long	time();
	extern int	viewps();
	extern char	*mkthname(), *ctime();

	for (i = 0; i < npilots; i++) {
		if (pilots[i].pflag&(PCAPTURED|PKILLED|PFROZEN))
			break;
	}
	if (i == npilots) {
		simplesetup(DRXMESS+11, DRYMESS+2, 53, 6,
			"No Pilot to Resurrect", LIGHTRED, WHITE, RED);
		beep();
		xyprintf(DRXMESS+13, DRYMESS+3,
			"There are no dead or captured pilots in the list.");
		xyprintf(DRXMESS+13, DRYMESS+4,
				"You will have to kill or lose one first.");
		tkyo(DRXMESS+14, DRYMESS+6);
		return;
	}
	n = viewps("Choose Pilot to Resurrect",
		"Select pilot with \030\031, <CR> to resurrect");

	if (n < 0)
		return;
	time(&now);
	if (pilots[n].stockade > now) {
		simplesetup(DRXMESS+5, DRYMESS+7, 43, 6,
			"Pilot is in the Stockade", LIGHTRED, WHITE, RED);
		beep();
		cursor(DRXMESS+7, DRYMESS+8);
		outpname(&pilots[n]);
		sput(" is confined to the stockade");
		p = ctime(&pilots[n].stockade);
		p[24] = '\0';
		xyprintf(DRXMESS+7, DRYMESS+9, "until %s.", p);
		tkyo(DRXMESS+8, DRYMESS+11);
		return;
	}
	if (!(pilots[n].pflag&(PKILLED|PCAPTURED))) {
		simplesetup(DRXMESS+5, DRYMESS+7, 58, 6,
		      "Pilot doesn't Need Resurrection", LIGHTRED, WHITE, RED);
		beep();
		cursor(DRXMESS+7, DRYMESS+8);
		outpname(&pilots[n]);
		sput(" is not dead or captured.");
		xyprintf(DRXMESS+7, DRYMESS+9,
			"You can only resurrect dead or captured pilots.");
		tkyo(DRXMESS+8, DRYMESS+11);
		return;
	}
	simplesetup(DRXMESS+2, DRYMESS+4, DRPWIDP+30, 7,
				"Confirm Resurrection", MAGENTA, CYAN, RED);
	cursor(DRXMESS+4, DRYMESS+6);
	sput("Type <ENTER> to resurrect ");
	outpname(&pilots[n]);
	xyprintf(DRXMESS+4, DRYMESS+7, " Anything else cancels...");
	i = spgetch();
	if (i != '\n')
		return;
	pp = &pilots[n];
	pp->pflag &= ~PKILLED;
	pp->pflag &= ~PCAPTURED;
	pp->nresurected++;
	cursor(DRXMESS+4, DRYMESS+9);
	sput("Activate ");
	outpname(pp);
	sput("? [Yes or No]");
	for (i = 0; i < pp->ntheaters; i++) {
		if ((fd = open(mkthname(pp->theatnos[i]), O_RDWR)) < 0)
			continue;
		lseek(fd, (long)(sizeof(struct theatert)), SEEK_SET);
		if (read(fd, (void *)&tp, sizeof(struct pilot)) !=
						sizeof(struct pilot)) {
			close(fd);
			continue;
		}
		if (!(tp.pflag&(PKILLED|PCAPTURED))) {
			close(fd);
			continue;
		}
		tp.nresurected++;
		tp.pflag &= ~(PKILLED|PCAPTURED);
		lseek(fd, (long)(sizeof(struct theatert)), SEEK_SET);
		write(fd, (void *)&tp, sizeof(struct pilot));
		close(fd);
	}

	i = spgetch();
	if (i == 'Y' || i == 'y' || i == '\r' || i == '\n') {
		curpilot->pflag &= ~PACTIVE;
		curpilot = &pilots[n];
		curryo = &curpilot->ryo;
		curpilot->pflag |= PACTIVE;
	}
}

static void	renamepilot() {
	char	*p;
	int	n;
	extern int	viewps();
	extern void	chhname();

	if (npilots == 0) {
		simplesetup(DRXMESS+11, DRYMESS+3, 47, 6,
			"No Pilot to Rename", LIGHTRED, WHITE, RED);
		xyprintf(DRXMESS+13, DRYMESS+4,
			"There is no pilot in the list to rename.");
		xyprintf(DRXMESS+13, DRYMESS+5,
					"You will have to create one first.");
		tkyo(DRXMESS+14, DRYMESS+7);
		return;
	}
	n = viewps("Choose Pilot to Rename",
			"Select pilot with \030\031, <CR> to rename");

	if (n < 0)
		return;

	simplesetup(DRXMESS+2, DRYMESS+5, DRPWIDP+24, 9,
				"Rename Pilot", MAGENTA, BROWN, YELLOW);
	cursor(DRXMESS+4, DRYMESS+6);
	sput("Enter new name for ");
	outpname(&pilots[n]);
	cput(' ');
	if ((p = getpname(DRXMESS+4, DRYMESS+8)) == NULL)
		return;

	chhname(pilots[n].name, p);
	strcpy(pilots[n].name, p);
}

static int	viewps(s, s2)
	char	*s, *s2;
{
	int	pilot, c;
	extern void	showpilot(), showp();

	if (npilots == 0) {
		errorbox("No pilots to view.");
		return(0);
	}
	if (curpilot == &trainee)
		pilot = 0;
	else
		pilot = curpilot - pilots;

	while (1) {
		color(CYAN, BLUE);
		showp(pilot);
		showpilot(&pilots[pilot], s, s2, NULL);
		c = spgetch();
		if (c == 033 || c == 010)
			return(-1);
		else if (c == '\r' || c == '\n')
			return(pilot);
		else if (c == UPARROW || c == LEFTARROW) {
			color(BLUE, CYAN);
			showp(pilot);
			if (pilot == 0)
				pilot = npilots - 1;
			else
				pilot--;
		}
		else if (c == DOWNARROW || c == RIGHTARROW) {
			color(BLUE, CYAN);
			showp(pilot);
			if (pilot == npilots-1)
				pilot = 0;
			else
				pilot++;
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

void	showpilot(pp, s, s2, pthn)
	struct pilot	*pp;
	char	*s, *s2;
	struct thnamlist	*pthn;
{
	char	buf1[LINELEN], buf2[LINELEN], *p, *p1;
	long	now;
	int	i, y, pk, ok, w, m, mflag;

	extern struct rank	ranks[];
	extern struct medal	medals[];
	extern long	time();
	extern int	nmedals, medalo[];
	extern void	center();

	simplesetup(DRXMESS+1, DRYMESS+1, DRXWID-3-DRPWIDP-2, DRYWID-3,
							s, RED, WHITE, BLUE);
	cursor(DRXMESS+2, DRYMESS+2);
	fcolor(RED);
	center(buf1, DRXWID-3-DRPWIDP-2-2, s2);
	sput(buf1);
	fcolor(BLUE);

	y = DRYMESS+4;
	time(&now);
	mflag = 0;
	if (pthn == NULL) {
#ifdef	OLDSHARE
		if (pp->pflag&PFROZEN)
			p = "(frozen)";
		else
#endif	/* oldshare */
		if (pp->pflag&PACTIVE)
			p = "(active)";
		else if (pp->pflag&PKILLED)
			p = "(K.I.A.)";
		else if (pp->pflag&PCAPTURED)
			p = "(captured)";
		else if (pp->stockade > now)
			p = "(stockade)";
		else
			p = "(ready for action)";
		/* NAME */
		sprintf(buf1, "Pilot:  %s %s",ranks[pp->rank].abbrank,pp->name);
		center(buf2, DRXWID-3-DRPWIDP-2-2, buf1);
		for (p1 = buf2; *p1 != ':'; p1++);
		p1 += 3;
		*p1 = '\0';
		cursor(DRXMESS+3, y++);
		sput(buf2);
		outpname(pp);

		buf1[0] = '\0';
		for (i = 0; i < nmedals; i++) {
			m = medalo[i];
			if (!pp->nmedals[m])
				continue;
			sprintf(buf2, "%s%s", (i==0) ? "" : " ", medals[m].abb);
			strcat(buf1, buf2);
			if (pp->nmedals[m] > 1) {
				sprintf(buf2, ":%d", pp->nmedals[m]);
				strcat(buf1, buf2);
			}
		}
		strcat(buf1, " ");
		i = strlen(buf1);

		if (i > DRXWID-3-DRPWIDP-2-2) {
			i /= 2;
			for (p1 = &buf1[i]; *p1 != ' '; p1--);
			*p1++ = '\0';
			center(buf2, DRXWID-3-DRPWIDP-2-3, buf1);
			cursor(DRXMESS+3, y++);
			sput(buf2);
			center(buf2, DRXWID-3-DRPWIDP-2-3, p1);
			cursor(DRXMESS+3, y++);
			sput(buf2);
			mflag = 1;
		}
		else {
			center(buf2, DRXWID-3-DRPWIDP-2-3, buf1);
			cursor(DRXMESS+3, y++);
			sput(buf2);
			mflag = 0;
		}

		sprintf(buf1, " %d mission%s %s", pp->nmission,
					(pp->nmission == 1) ? "" : "s", p);
		center(buf2, DRXWID-3-DRPWIDP-2-3, buf1);
		cursor(DRXMESS+3, y++);
		sput(buf2);

		xyprintf(DRXMESS+3, y++, "Mission Objectives = %-5d",
						pp->totalmiss.st3.objectives);
		if (pp->nmission) {
			sprintf(buf2, "%d",
				(pp->totalmiss.st3.objectives*10)/pp->nmission);
			for (p = buf2; *p; p++);
			*(p+1) = '\0';
			*p = *(p-1);
			*(p-1) = '.';
			sprintf(buf1, "Avg = %s", buf2);
			sput(buf1);
		}
	}
	else {
		sprintf(buf1, "%s T.O. ", pthn->name);
		center(buf2, DRXWID-3-DRPWIDP-2-3, buf1);
		cursor(DRXMESS+3, y++);
		sput(buf2);
		xyprintf(DRXMESS+3, y++,
			"%d misn%s.  %d objctvs killed, %d remain.",
					pp->nmission,
					(pp->nmission == 1) ? "" : "s",
					pthn->ncobjectives,
					pthn->nobjectives);
		if (pthn != NULL)
			if (pthn->nstoredplanes == 0)
				xyprintf(DRXMESS+3, y++,
				   "There are no planes remaining in storage.");
			else
				xyprintf(DRXMESS+3, y++,
				"There are %d plane%s remaining in storage.",
					pthn->nstoredplanes,
					(pthn->nstoredplanes == 1) ? "" : "s");
		y++;

	}

	xyprintf(DRXMESS+3, y++,
	 	"Total points = %-9ld  ", pp->totalmiss.score);
	if (pp->nmission) {
		sprintf(buf1, "Avg = %ld", pp->totalmiss.score/pp->nmission);
		sput(buf1);
	}

	pk = pp->totalmiss.st3.tickskilled + pp->totalmiss.st3.objectives;
	for (w = 0; w < NWICK; w++)
		pk += pp->totalmiss.st3.siteskilled[w] +
		     pp->totalmiss.st3.aaakilled[w] +
		     pp->totalmiss.st3.orbskilled[w];
	xyprintf(DRXMESS+3, y++, "Primary Kills = %-5d  ", pk);
	if (pp->nmission) {
		sprintf(buf1, "Avg = %d", pk/pp->nmission);
		sput(buf1);
	}

	ok = 0;
	for (w = 0; w < NWICK; w++)
		ok += pp->totalmiss.st3.shckilled[w] +
		      pp->totalmiss.st3.dbkilled[w];
	xyprintf(DRXMESS+3, y++, "Other Kills = %-5d   ", ok);
	if (pp->nmission) {
		sprintf(buf1, "Avg = %d", ok/pp->nmission);
		sput(buf1);
	}

	if (!mflag)
		y++;
	pk = pp->bestofeach.st3.tickskilled + pp->bestofeach.st3.objectives;
	for (w = 0; w < NWICK; w++)
		pk += pp->bestofeach.st3.siteskilled[w] +
		      pp->bestofeach.st3.aaakilled[w] +
		      pp->bestofeach.st3.orbskilled[w];
	ok = 0;
	for (w = 0; w < NWICK; w++)
		ok += pp->bestofeach.st3.shckilled[w] +
		      pp->bestofeach.st3.dbkilled[w];
	xyprintf(DRXMESS+3, y++, "Combined best-single-mission:");
	xyprintf(DRXMESS+3, y++, " %d primary kills \304\304 %d other kills",
									pk, ok);

	xyprintf(DRXMESS+3, y++, "Best mission score = %ld:",
							pp->bestmiss.score);
	pk = pp->bestmiss.st3.tickskilled + pp->bestmiss.st3.objectives;
	for (w = 0; w < NWICK; w++)
		pk += pp->bestmiss.st3.siteskilled[w] +
		      pp->bestmiss.st3.aaakilled[w] +
		      pp->bestmiss.st3.orbskilled[w];
	ok = 0;
	for (w = 0; w < NWICK; w++)
		ok += pp->bestmiss.st3.shckilled[w] +
		      pp->bestmiss.st3.dbkilled[w];
	xyprintf(DRXMESS+3, y++, " %d primary kills \304\304 %d other kills",
									pk, ok);
}

static void	showp(n)
	int	n;
{
	cursor(DRXMESS+DRXWID-DRPWIDP-1, DRYMESS+3+n);
	outpname(&pilots[n]);
}

static void	rohelp() {
	extern int	fileout();
	extern void	nbmplesetup();

	nbmplesetup(DRXMESS, DRYMESS, DRXWID, DRYWID,
			"Duty Roster Menu Information", GREEN, RED, WHITE);
	fileout("moag.inf", 2, DRXMESS+2, DRYMESS+1, DRXWID-3, DRYWID-2,
								WHITE, RED, 4);
}

/*	ROSTER MENU BROWN, CYAN, BLUE */

extern int	menux;
extern int	menuy;


char	*msg = "Duty Roster";

void	rostersetup() {
	int	i, cc;
	char	buf[128], *p;
	extern int	menuwid;
	extern void	center();

	cursor(menux, menuy);
	color(BLUE, CYAN);
	center(buf, menuwid, msg);
	sput(buf);

	cc = DRXWID-3-DRPWIDP-1;

	cursor(DRXMESS, DRYMESS);
	fcolor(BROWN);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 0; i < cc; i++)
		*p++ = STOP;
	*p++ = STMIDDLE;
	for (i = 0; i < DRPWIDP+1; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	cursor(DRXMESS, DRYMESS+1);
	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < cc; i++)
		*p++ = ' ';
	*p++ = SVMIDDLE;
	*p = '\0';
	sput(buf);
	fcolor(BLUE);
	center(buf, DRPWIDP+1, "Pilots:");
	sput(buf);
	fcolor(BROWN);
	cput(SRIGHT);

	cursor(DRXMESS, DRYMESS+2);
	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < cc; i++)
		*p++ = ' ';
	*p++ = SLEFTMIDDLE;
	for (i = 0; i < DRPWIDP+1; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	*p = '\0';
	sput(buf);

	cursor(DRXMESS, DRYMESS+3);
	fcolor(BROWN);
	p = buf;
	*p++ = SLEFT;
	for (i = 0; i < cc; i++)
		*p++ = ' ';
	*p++ = SVMIDDLE;
	for (i = 0; i < DRPWIDP+1; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	sput(buf);

	for (i = 4; i < DRYWID-1; i++) {
		cursor(DRXMESS, DRYMESS+i);
		sput(buf);
	}

	cursor(DRXMESS, DRYMESS+i);
	p = buf;
	*p++ = SLOWERLEFT;
	for (i = 0; i < cc; i++)
		*p++ = SBOTTOM;
	*p++ = SBMIDDLE;
	for (i = 0; i < DRPWIDP+1; i++)
		*p++ = SBOTTOM;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);
}
