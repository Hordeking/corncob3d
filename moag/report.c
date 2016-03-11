#include	<stdio.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

#ifndef	ISCORE

int	noplanes,		/* No planes at this airstrip */
	noobjectives,		/* No objectives in theater */
	nhobjectives;		/* No objectives at this airstrip */

INIT int	newplane;
#ifdef startloc
	char slocarr[80];
#endif

int	newpreport(tower)
	int	tower;
{
	char	buf[LINELEN], buf2[LINELEN], *p, *p1;
	int	y, c, tflag, i;
	extern struct rank	ranks[];
	extern struct pertowerinfo	ptos[];
	extern struct theatert	curtheater;
#ifndef MVPCD
	extern char	filetower[];
#else
	extern	char	wpath[80];
	extern	char	scrlist[80];
#endif
	extern int	fileout();
	extern void	center();

	y = PPYMESS;
	simplesetup(PPXMESS, y++, PPXWID, PPYWID, "Mission Briefing",
							GREEN, RED, WHITE);
	fcolor(YELLOW);
	p1 = ", prepare for your mission!";
	sprintf(buf, "%s %s%s", ranks[curpilot->rank].rank, curpilot->name, p1);
	center(buf2, PPXWID-2, buf);
	cursor(PPXMESS+1, y++);
	for (p = buf2; *p == ' '; p++)
		cput(' ');
	foutpname(curpilot);
	sput(p1);

	cursor(PPXMESS, y++);
	fcolor(GREEN);
	p = buf;
	*p++ = SLEFTMIDDLE;
	for (i = 2; i < PPXWID; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	*p = '\0';
	sput(buf);

	fcolor(BRIGHTWHITE);
	if (curpilot->curtheater&0x80) {
		center(buf, PPXWID-2, "Training Mission:");
		tower = 4;
	}
	else {
		sprintf(buf2, "Your Theater of Operation:  %s",
							curtheater.name);
		center(buf, PPXWID-2, buf2);
	}
	xyprintf(PPXMESS+1, y++, buf);

	fcolor(WHITE);
	newplane = 0;
	noplanes = 0;
	noobjectives = 0;
	nhobjectives = 0;
	if (!(curpilot->curtheater&0x80)) {
		if (ptos[tower].ncloseplanes == 0)
			noplanes = 1;
		if (curtheater.nobjectives == 0)
			noobjectives = 1;
		else if (ptos[tower].nobjectives == 0)
			nhobjectives = 1;
		tflag = 2;
	}
	else
		tflag = 3;
	c = 'n';
	return(c);
}


int	preport(tower)
	int	tower;
{
	char	buf[LINELEN], buf2[LINELEN], *p, *p1;
	int	y, c, tflag, i;
#ifdef startloc
	int	itemp;
	FILE *fp;
#endif
	extern struct rank	ranks[];
	extern struct pertowerinfo	ptos[];
	extern struct theatert	curtheater;
#ifndef MVPCD
	extern char	filetower[];
#else
	extern	char	wpath[80];
	extern	char	scrlist[80];
#endif
	extern int	fileout();
	extern void	center();

	y = PPYMESS;
	simplesetup(PPXMESS, y++, PPXWID, PPYWID, "Mission Briefing",
							GREEN, RED, WHITE);
	fcolor(YELLOW);
	p1 = ", prepare for your mission!";
	sprintf(buf, "%s %s%s", ranks[curpilot->rank].rank, curpilot->name, p1);
	center(buf2, PPXWID-2, buf);
	cursor(PPXMESS+1, y++);
	for (p = buf2; *p == ' '; p++)
		cput(' ');
	foutpname(curpilot);
	sput(p1);

	cursor(PPXMESS, y++);
	fcolor(GREEN);
	p = buf;
	*p++ = SLEFTMIDDLE;
	for (i = 2; i < PPXWID; i++)
		*p++ = SHMIDDLE;
	*p++ = SRIGHTMIDDLE;
	*p = '\0';
	sput(buf);

	fcolor(BRIGHTWHITE);
	if (curpilot->curtheater&0x80) {
		center(buf, PPXWID-2, "Training Mission:");
		tower = 4;
	}
	else {
		sprintf(buf2, "Your Theater of Operation:  %s",
							curtheater.name);
		center(buf, PPXWID-2, buf2);
	}
	xyprintf(PPXMESS+1, y++, buf);

	fcolor(WHITE);
	newplane = 0;
	noplanes = 0;
	noobjectives = 0;
	nhobjectives = 0;
	if (!(curpilot->curtheater&0x80)) {
		if (ptos[tower].ncloseplanes == 0)
			noplanes = 1;
		if (curtheater.nobjectives == 0)
			noobjectives = 1;
		else if (ptos[tower].nobjectives == 0)
			nhobjectives = 1;
		tflag = 2;
	}
	else
		tflag = 3;
	c = fileout(filetower, tower+1,
		     PPXMESS+1, y, PPXWID-2, PPYMESS+PPYWID-1-y, WHITE, RED,
		     							tflag);

#ifdef startloc
#ifdef batflag
	if((c !=0 ) && (strlen(slocarr)!=0)) {
		fp = fopen("startloc.dat", "w+t");
		strcat(slocarr,"\n");
		fputs(slocarr,fp);
		fclose(fp);
	}
	else
		unlink("startloc.dat");
#endif
#endif
	return(c);
}

struct pilot	nullpilot;
extern struct pilot	curthpilot;
extern int	promoted;
void	report(pm)
	struct pilotmiss	*pm;
{
	char	*p, *p2;
	int	c;
	long	now;
	struct pilot	spc, *ppc, *ppt;
	struct thnamlist	*pthn;

	extern struct theatert	curtheater;
	extern struct thnamlist	*getthnam();
	extern struct pilot	trainee, *curpilot;
	extern long	time();
	extern int	criterr, crit(), genflag, wzflag;
	extern void	savescore(), addin(), savetheat(), pmissrep(), tally(),
			promote(), showprom(), showmedals(), showwhat(),
			showgen();

	ppc = curpilot;
	if (curpilot->curtheater&0x80)
		ppt = NULL;
	else
		ppt = &curthpilot;
	spc = *ppc;

	if (pm->st3.flag&KILLED) {
		ppc->pflag |= PKILLED;
		if (ppt != NULL)
			ppt->pflag |= PKILLED;
	}
	else if (pm->st3.flag&CAPTURED) {
		ppc->pflag |= PCAPTURED;
		if (ppt != NULL)
			ppt->pflag |= PCAPTURED;
	}

	if (ppt != NULL)
		tally(ppt, pm);
	tally(ppc, pm);

	p = "This mission will not be recorded because ";
	if (((curryo->invflag&INVVAL) || pm->st3.flag&INVULNERABLE) && !wzflag){
		p2 = "an invulnerable plane was flown.";
		showwhat(&pm->st3);
		if (ppt != NULL) {
			pmissrep(ppt, 3, curtheater.name);
			xyprintf(4, 23, p);
			sput(p2);
			*ppc = spc;
			tkyo(25, 24);
			pmissrep(ppc, 4, NULL);
		}
		else {
			pmissrep(ppc, 2, NULL);
			xyprintf(4, 23, p);
			*ppc = spc;
			sput(p2);
		}
		tkyo(25, 24);
		setblink(1);
	}
	else if (criterr) {
		p2 = "of a critical error during flight.";
		showwhat(&pm->st3);
		if (ppt != NULL) {
			pmissrep(ppt, 3, curtheater.name);
			c = crit();
			if (c) {
				pthn = getthnam(curpilot->curtheater);
				if (newplane) {
					pthn->nstoredplanes--;
					curtheater.nstoredplanes--;
				}
				pthn->ncobjectives +=
						ppt->lastmiss.st3.objectives;
				curtheater.ncobjectives +=
						ppt->lastmiss.st3.objectives;
				pthn->nobjectives -=
						ppt->lastmiss.st3.objectives;
				curtheater.nobjectives -=
						ppt->lastmiss.st3.objectives;
				if (pthn->nobjectives < 0)
						pthn->nobjectives = 0;
				if (curtheater.nobjectives < 0)
						curtheater.nobjectives = 0;
				promote(ppc);
				savescore();
				addin(ppc);
				ppt->rank = ppc->rank;
				savetheat();
				tkyo(25, 24);
				showprom(ppc);
				showmedals(ppc);
			}
			else {
				xyprintf(3, 23, p);
				sput(p2);
				*ppc = spc;
				tkyo(25, 24);
			}
			pmissrep(ppc, 4, NULL);
			tkyo(25, 24);
		}
		else {
			pmissrep(ppc, 2, NULL);
			c = crit();
			if (c) {
				promote(ppc);
				savescore();
				addin(ppc);
			}
			else {
				xyprintf(3, 23, p);
				sput(p2);
				*ppc = spc;
			}
			tkyo(25, 24);
			showprom(ppc);
			showmedals(ppc);
		}
		setblink(1);
	}
	else if (pm->st3.flag&ABORT) {
		p2 = "of an abort.";
		showwhat(&pm->st3);
		if (ppt != NULL) {
			pmissrep(ppt, 3, curtheater.name);
			xyprintf(14, 23, p);
			sput(p2);
			tkyo(25, 24);
			pmissrep(ppc, 4, NULL);
		}
		else {
			pmissrep(ppc, 2, NULL);
			xyprintf(14, 23, p);
			sput(p2);
		}
		*ppc = spc;
		tkyo(25, 24);
		setblink(1);
	}
	else {
		if (ppt != NULL) {
			pthn = getthnam(curpilot->curtheater);
			if (newplane) {
				pthn->nstoredplanes--;
				curtheater.nstoredplanes--;
			}
			pthn->ncobjectives += ppt->lastmiss.st3.objectives;
			curtheater.ncobjectives += ppt->lastmiss.st3.objectives;
			pthn->nobjectives -= ppt->lastmiss.st3.objectives;
			curtheater.nobjectives -= ppt->lastmiss.st3.objectives;
			if (pthn->nobjectives < 0)
					pthn->nobjectives = 0;
			if (curtheater.nobjectives < 0)
					curtheater.nobjectives = 0;
			promote(ppc);
			ppt->rank = ppc->rank;
			savescore();
			addin(ppc);
			savetheat();
			showwhat(&pm->st3);
			ppt->rank -= promoted;
			pmissrep(ppt, 3, curtheater.name);
			ppt->rank += promoted;
			tkyo(25, 24);
			showprom(ppc);
			showmedals(ppc);
			pmissrep(ppc, 4, NULL);
			tkyo(25, 24);
		}
		else {
			promote(ppc);
			savescore();
			addin(ppc);
			showwhat(&pm->st3);
			ppc->rank -= promoted;
			pmissrep(ppc, 2, NULL);
			ppc->rank += promoted;
			tkyo(25, 24);
			showprom(ppc);
			showmedals(ppc);
		}
		setblink(1);
	}

/* #ifdef	REGISTERED */
	if (genflag)
		showgen();
/*#endif */

	time(&now);
	if (ppc == &trainee) {
		ppc->pflag &= ~(PKILLED|PCAPTURED);
		ppc->stockade = 0;
	}
	else if ((ppc->pflag&(PKILLED|PCAPTURED)) || ppc->stockade > now) {
		ppc->pflag &= ~PACTIVE;
		curpilot = &trainee;
		curpilot->pflag |= PACTIVE;
		curryo = &curpilot->ryo;
	}
	if ((ppc->pflag&(PKILLED|PCAPTURED)) && ppc->stockade > now) {
		ppc->stockade = now;
	}
	setblink(1);
}

int	crit() {
	int	c;
	char	*p;

	p = " (Critical error occured) ";
	beep();

	cursor(9, 23);
	color(LIGHTRED, LIGHTBLUE);
	sput(p);

	cursor(36, 23);
	color(BRIGHTWHITE, BLUE);
	sput("Do you want this mission recorded?  ");
	color(BRIGHTWHITE, LIGHTBLUE);
	cput('_');

	c = spgetch();

	color(LIGHTRED, BLUE);
	cursor(9, 23);
	sput(" (Critical error occured) ");

	cursor(72, 23);
	fcolor(BRIGHTWHITE);
	if (c == 'Y' || c == 'y') {
		sput("Yes");
		fcolor(YELLOW);
		return(1);
	}
	else {
		sput("No");
		fcolor(YELLOW);
		return(0);
	}
}

#ifdef	OLDSTUFF
static char	crashed1[] =
	"Your plane has smashed into the ground, tearing";
static char	crashed2[] =
	"itself to bits, and bursting into flame.";

static char	*nothome[] = {
	crashed1,
	crashed2,
	"",
	"Some time later, an aerial reconnaissance team",
	"identifies the wreckage, and reports your death",
	"to sector command.",
};

static char	*athome[] = {
	crashed1,
	crashed2,
	"",
	"The tower ground crew removes what it can find of",
	"your remains from the wreckage.  There is nothing",
	"left of the aircraft to salvage.",
};
 
static char	*badeject[] = {
	"With your failed parachute trailing behind you,",
	"your body slams into the ground, killing you in-",
	"stantly.",
	"",
	"Some time later, an aerial reconnaissance team",
	"identifies the bloody remains, and reports your",
	"death to sector command.",
};

static char	capt1[] =
	"were carried to the alien home-world, where all";
static char	capt2[] =
	"captured humans are forced to clean the alien";
static char	capt3[] =
	"toilets for the rest of their lives.";

static char	*shccaptured[] = {
	"You have been taken prisoner by an alien ground",
	"vehicle.  Unfortunately, you were taken alive, and",
	capt1,
	capt2,
	capt3,
};

static char	*alncaptured[] = {
	"You have been taken prisoner by an alien.  The",
	"alien overcame your superior strength by drugging",
	"you.  Unfortunately, you were taken alive, and",
	capt1,
	capt2,
	capt3,
};

static char	*unkcaptured[] = {
	"You have been taken prisoner by an orbiting alien",
	"starship, which beamed you up.  This was not known",
	"possible.  Unfortunately, you were taken alive, and",
	capt1,
	capt2,
	capt3,
};

static char	blasted0[] =
	"After leaving your plane, you have been blasted to";
static char	blasted1[] =
	"";
static char	blasted2[] =
	"Some days after your flight, an aerial reconnais-";
static char	blasted3[] =
	"sance team identifies your bloody remains on the";
static char	blasted4[] =
	"combat field, and reports your death to sector";
static char	blasted5[] =
	"command.";

static char	*bdball[] = {
	blasted0,
	"pulp by an enemy missile.",
	blasted1,
	blasted2,
	blasted3,
	blasted4,
	blasted5,
};

static char	*baaa[] = {
	blasted0,
	"pulp by enemy AAA fire.",
	blasted1,
	blasted2,
	blasted3,
	blasted4,
	blasted5,
};

static char	*bcrater[] = {
	"After leaving your plane, you have fallen into a",
	"a bomb crater filled with razor sharp debris.",
	blasted1,
	blasted2,
	blasted3,
	blasted4,
	blasted5,
};

static char	*bbbees[] = {
	"After leaving your plane, you have been ripped to",
	"shreds by a swarm of alien \"bumble bees\".",
	blasted1,
	blasted2,
	blasted3,
	blasted4,
	blasted5,
};

static char	*bbballoon[] = {
	"After leaving your plane, you have been squashed",
	"flat by a descending barrage balloon.",
	blasted1,
	blasted2,
	blasted3,
	blasted4,
	blasted5,
};

static char	*bunknown[] = {
	"After leaving your plane, you have been killed by",
	"a secret alien death zapper which was previously",
	"unknown to our science.",
	blasted1,
	blasted2,
	blasted3,
	blasted4,
	blasted5,
};

static char	*stockade[] = {
	"Upon return from your flight, military police drag",
	"you before the brass at sector command.  They are",
	"outraged at your destruction of friendly assets.",
	"",
	"The Lt. General in charge of this TO decides that",
	"cooling your heels in the stockade will help you",
	"aim more carefully next time you fly.",
};

static void	showwhat(p3)
	struct stuffrm3	*p3;
{
	int	i, n, nl, y, stkflag, bcdflag;
	char	**pp;
	extern void	boxsetup();

	nl = 0;
	stkflag = 0;
	bcdflag = -1;
	if (p3->flag&KILLED) {
		if (p3->flag&KILLEDBYENEMY) {
			if (p3->deathcode == 22) {
				n = sizeof(baaa)/sizeof(char *);
				pp = baaa;
			}
			else if (p3->deathcode == 24) {
				n = sizeof(bdball)/sizeof(char *);
				pp = bdball;
			}
			else if (p3->deathcode == 32) {
				n = sizeof(bcrater)/sizeof(char *);
				pp = bcrater;
			}
			else if (p3->deathcode == 40) {
				n = sizeof(bbballoon)/sizeof(char *);
				pp = bbballoon;
			}
			else if (p3->deathcode == 41) {
				n = sizeof(bbbees)/sizeof(char *);
				pp = bbbees;
			}
			else {
				n = sizeof(bunknown)/sizeof(char *);
				pp = bunknown;
			 	bcdflag = p3->deathcode;
				nl += 3;
			}
		}
		else if (p3->flag&EJECTED) {
			n = sizeof(badeject)/sizeof(char *);
			pp = badeject;
		}
		else {
			if (p3->flag&ATHOME) {
				n = sizeof(athome)/sizeof(char *);
				pp = athome;
			}
			else {
				n = sizeof(nothome)/sizeof(char *);
				pp = nothome;
			}
		}
	}
	else if (p3->flag&CAPTURED) {
		if (p3->deathcode == 12) {
			n = sizeof(shccaptured)/sizeof(char *);
			pp = shccaptured;
		}
		else if (p3->deathcode == 57) {
			n = sizeof(alncaptured)/sizeof(char *);
			pp = alncaptured;
		}
		else {
			n = sizeof(unkcaptured)/sizeof(char *);
			pp = unkcaptured;
			bcdflag = p3->deathcode;
			nl += 3;
		}
	}
	else if (p3->friendkilled) {
		n = sizeof(stockade)/sizeof(char *);
		pp = stockade;
		nl++;
		stkflag = 1;
	}
	else
		return;

	nl += n + 6;
	y = (25 - nl - 1)/2;

	setblink(0);
	color(BLACK, RED);
	erscrn();
	color(LIGHTBLUE, GRAY);
	boxsetup(12, y++, 54, nl, 1);
	fcolor(BRIGHTWHITE);

	y++;
	for (i = 0; i < n; i++) {
		cursor(12+2, y++);
		hsput(pp[i], 52);
	}
	if (stkflag) {
		char	buf[LINELEN];
		sprintf(buf, "You will be released in %d day%s.",
			p3->friendkilled, (p3->friendkilled == 1) ? "" : "s");
		cursor(12+2, y++);
		hsput(buf, 52);
	}
	if (bcdflag != -1) {
		char	buf[LINELEN];
		y++;
		sprintf(buf, "Please report this event and code # %d to",
					bcdflag);
		cursor(12+2, y++);
		hsput(buf, 52);
		sprintf(buf, "Pie in the Sky software.", bcdflag);
		cursor(12+2, y++);
		hsput(buf, 52);
	}
	y++;
	tkyo(12+5, y);
}

#else	/* !OLDSTUFF */

static char	*bunknown[] = {
	"You have been killed in a manner not designed by",
	"the programmer."
};

static char	*unkcaptured[] = {
	"You have been captured in a manner not designed",
	"by the programmer."
};

static void	showwhat(p3)
	struct stuffrm3	*p3;
{
	int	i, n, nl, y, bcdflag;
	char	**pp, buf[LINELEN];
	extern void	boxsetup();

	bcdflag = -1;
	if (p3->flag&KILLEDBYENEMY) {
		if (p3->deathcode != 22 &&
		    p3->deathcode != 24 &&
		    p3->deathcode != 32 &&
		    p3->deathcode != 40 &&
		    p3->deathcode != 41) {
			n = sizeof(bunknown)/sizeof(char *);
			pp = bunknown;
			bcdflag = p3->deathcode;
		}
		else
			return;
	}
	else if (p3->flag&CAPTURED) {
		if (p3->deathcode != 12 &&
		    p3->deathcode != 57) {
			n = sizeof(unkcaptured)/sizeof(char *);
			pp = unkcaptured;
			bcdflag = p3->deathcode;
		}
		else
			return;
	}
	else
		return;

	if (bcdflag == -1)
		return;

	nl = n + 9;
	y = (25 - nl - 1)/2;

	setblink(0);
	color(BLACK, RED);
	erscrn();
	color(LIGHTBLUE, GRAY);
	boxsetup(12, y++, 54, nl, 1);
	fcolor(BRIGHTWHITE);

	y++;
	for (i = 0; i < n; i++) {
		cursor(12+2, y++);
		hsput(pp[i], 52);
	}
	y++;
	sprintf(buf, "Please report this occurence and code %d to", bcdflag);
	cursor(12+2, y++);
	hsput(buf, 52);
	sprintf(buf, "Pie in the Sky software.", bcdflag);
	cursor(12+2, y++);
	hsput(buf, 52);
	y++;
	tkyo(12+5, y);
}

#endif	/* !OLDSTUFF */

#endif					/* ! ISCORE */

void	tally(pp, pm)
	struct pilot	*pp;
	struct pilotmiss	*pm;
{
	int	w;
	extern long	calcscore();

	pp->nmission++;
	pm->score = calcscore(&pm->st3);
	pp->lastmiss = *pm;

	if (pp->lastmiss.score > pp->bestmiss.score)
		pp->bestmiss = pp->lastmiss;
	pp->totalmiss.score += pp->lastmiss.score;

	if (pp->lastmiss.st3.objectives > pp->bestofeach.st3.objectives)
		pp->bestofeach.st3.objectives = pp->lastmiss.st3.objectives;
	pp->totalmiss.st3.objectives += pp->lastmiss.st3.objectives;

	if (pp->lastmiss.st3.tickskilled > pp->bestofeach.st3.tickskilled)
		pp->bestofeach.st3.tickskilled = pp->lastmiss.st3.tickskilled;
	pp->totalmiss.st3.tickskilled += pp->lastmiss.st3.tickskilled;

	for (w = 0; w < NWICK; w++) {
		if (pp->lastmiss.st3.aaakilled[w] >
						pp->bestofeach.st3.aaakilled[w])
			pp->bestofeach.st3.aaakilled[w] =
						pp->lastmiss.st3.aaakilled[w];
		pp->totalmiss.st3.aaakilled[w] +=
						pp->lastmiss.st3.aaakilled[w];
	}

	for (w = 0; w < NWICK; w++) {
		if (pp->lastmiss.st3.siteskilled[w] >
					pp->bestofeach.st3.siteskilled[w])
			pp->bestofeach.st3.siteskilled[w] =
					pp->lastmiss.st3.siteskilled[w];
		pp->totalmiss.st3.siteskilled[w] +=
					pp->lastmiss.st3.siteskilled[w];
	}

	for (w = 0; w < NWICK; w++) {
		if (pp->lastmiss.st3.orbskilled[w] >
					pp->bestofeach.st3.orbskilled[w])
			pp->bestofeach.st3.orbskilled[w] =
					pp->lastmiss.st3.orbskilled[w];
		pp->totalmiss.st3.orbskilled[w] +=
					pp->lastmiss.st3.orbskilled[w];
	}


	for (w = 0; w < NWICK; w++) {
		if (pp->lastmiss.st3.dbkilled[w] >
						pp->bestofeach.st3.dbkilled[w])
			pp->bestofeach.st3.dbkilled[w] =
						pp->lastmiss.st3.dbkilled[w];
		pp->totalmiss.st3.dbkilled[w] +=
						pp->lastmiss.st3.dbkilled[w];
	}

	for (w = 0; w < NWICK; w++) {
		if (pp->lastmiss.st3.shckilled[w] >
						pp->bestofeach.st3.shckilled[w])
			pp->bestofeach.st3.shckilled[w] =
						pp->lastmiss.st3.shckilled[w];
		pp->totalmiss.st3.shckilled[w] +=
						pp->lastmiss.st3.shckilled[w];
	}


	if (pp->lastmiss.st3.planeslost > pp->bestofeach.st3.planeslost)
		pp->bestofeach.st3.planeslost = pp->lastmiss.st3.planeslost;
	pp->totalmiss.st3.planeslost += pp->lastmiss.st3.planeslost;

	if (pp->lastmiss.st3.friendkilled > pp->bestofeach.st3.friendkilled)
		pp->bestofeach.st3.friendkilled = pp->lastmiss.st3.friendkilled;
	pp->totalmiss.st3.friendkilled += pp->lastmiss.st3.friendkilled;

	if (pp->lastmiss.st3.totaldamage > pp->bestofeach.st3.totaldamage)
		pp->bestofeach.st3.totaldamage = pp->lastmiss.st3.totaldamage;
	pp->totalmiss.st3.totaldamage += pp->lastmiss.st3.totaldamage;

	if (pp->lastmiss.st3.seconds > pp->bestofeach.st3.seconds)
		pp->bestofeach.st3.seconds = pp->lastmiss.st3.seconds;
	pp->totalmiss.st3.seconds += pp->lastmiss.st3.seconds;
}

static int	pmsf, msf;
static long	mobf;
long	calcscore(p3)
	struct stuffrm3	*p3;
{
	int	m, w;

	if (p3->flag&ABORT)
		return((long)0);

	if (p3->flag&KILLED)
		pmsf = 2;
	else if ((p3->flag&SUICIDE) &&(p3->flag&RESCUED))
		pmsf = 5;
	else if (p3->flag&CAPTURED)
		pmsf = 1;
	else if (p3->flag&ATHOME)
		pmsf = 5;
	else if (p3->flag&RESCUED)
		pmsf = 4;
	else if (p3->flag&SUICIDE)
		pmsf = 2;
	else
		pmsf = 1;

	if (p3->flag&CRASHNOTHOME)
		msf = 1;
	else if ((p3->flag&EJECTED) && (p3->flag&SUICIDE==0))
		msf = 1;
	else if (p3->flag&CRASHATHOME)
		msf = 3;
	else if (p3->flag&LANDEDHOME)
		msf = 5;
	else if (p3->flag&SUICIDE)
		msf = 5;
	else	msf = 1;

	mobf = p3->tickskilled + 15*p3->objectives;
	for (w = 0; w < NWICK; w++)
		mobf += p3->aaakilled[w] +p3->siteskilled[w] +p3->orbskilled[w];

	m = 10;
	for (w = 0; w < NWICK; w++)
		m += p3->dbonme[w]*(w+1) + p3->aaaonme[w]*(w+1);
	mobf *= m;
	mobf *= 5;
	mobf /= 10;
	for (w = 0; w < NWICK; w++)
		mobf += p3->dbkilled[w]*(w+1) + p3->shckilled[w]*(w+1);

	return(mobf*pmsf*msf/(p3->planeslost+1));
}

static int	liney, lvflag, llflag, evflag;
static int	pmissback,
		borderfore, presfore, letterfore, highfore, normfore;
char	p66s[] =
"                                                                 ";

#ifdef	ISCORE
int	mmode = 1;
#endif	/* ISCORE */
void	pmissrep(pp, vflag, thname)
	struct pilot	*pp;
	int	vflag;
	char	*thname;
{
	int	i, j, n, nn1, nn2, w, last, beoe, best, totl, m, nres,
		hismeds[20];
	char	*p, buf[LINELEN], buf2[LINELEN], lbuf[32];
	long	now;

	extern struct rank	ranks[];
	extern struct medal	medals[];
	extern long	time();
	extern void	begline(), endline(), lineout();
	extern int	nmedals, medalo[];
#ifndef	ISCORE
	extern char	*plural();
	extern int	mmode;
	extern void	center(), mkmedls();
#endif

	/*
	 *	Values for flag:
	 *
	 *	0:  called by roster to view pilot career.
	 *		mmode == 0 -> show medals
	 *		mmode == 1 -> show stuff
	 *	1:  called by misn to view theater career.
	 *	2:  called after flight, no theater, mission report.
	 *	3:  called after flight, mission theater report.
	 *	4:  called after flight, post mission career report.
	 */
	evflag = vflag;
	if (vflag == 0 || vflag == 1 || vflag == 4) {
		lvflag = 0;
		setblink(0);
		pmissback = GRAY;
		borderfore = LIGHTBLUE;
		presfore = GREEN;
		letterfore = BROWN;
		highfore = BRIGHTWHITE;
		normfore = WHITE;
	}
	else {
		lvflag = 1;
		setblink(1);
		pmissback = BLUE;
		borderfore = YELLOW;
		presfore = YELLOW;
		letterfore = LIGHTRED;
		highfore = BRIGHTWHITE;
		normfore = WHITE;
	}

#ifndef	ISCORE
	if (vflag == 0)
		strcpy(buf2, "CAREER REPORT");
	else if (vflag == 4)
		strcpy(buf2, "POST-MISSION CAREER REPORT");
	else if (vflag == 1)
		sprintf(buf2, "CAREER REPORT:  %s Theater of Operations",
									thname);
	else if (vflag == 2)
		strcpy(buf2, "MISSION REPORT");
	else if (vflag == 3)
		sprintf(buf2, "MISSION REPORT:  %s Theater of Operations",
									thname);
#else
	if (vflag == 2)
		strcpy(buf2, "PRELIMINARY REPORT");
	else if (vflag == 3)
		sprintf(buf2, "PRELIMINARY REPORT:  %s Theater of Operations",
									thname);
#endif
	color(borderfore, pmissback);
	cursor(1, 1);
	n = strlen(buf2);
	nn1 = (NCOL - n - 4)/2;
	nn2 = NCOL - nn1 - n - 4;
	p = buf;
	*p++ = DUPPERLEFT;
	for (i = 0; i < nn1; i++)
		*p++ = DTOP;
	*p++ = ' ';
	*p++ = '\0';
	sput(buf);
	fcolor(presfore);
	sput(buf2);
	fcolor(borderfore);
	p = buf;
	*p++ = ' ';
	for (i = 0; i < nn2; i++)
		*p++ = DTOP;
	*p++ = DUPPERRIGHT;
	*p++ = '\0';
	sput(buf);

	liney = 2;
	begline();
	fcolor(normfore);
	sput(p66s);
	endline();

	begline();
	fcolor(presfore);
	sput("         PILOT:  ");
	fcolor(highfore);
	foutpname(pp);
	buf[0] = 0;
	if (vflag != 0)
		nres = curpilot->nresurected;
	else
		nres = pp->nresurected;
	if (nres) {
		sprintf(buf, ", %d resurrection%s", nres, (nres==1) ? "" : "s");
		sput(buf);
	}
	for (i = strlen(pp->name) + strlen(ranks[pp->rank].rank) + 1;
							i < DRPWIDPP; i++)
		cput(' ');
	for (i = strlen(buf); i < 13; i++)
		cput(' ');
	endline();
	cursor(80, liney-1);
	cput(DRIGHT);

	if (!lvflag && !((vflag == 0 || vflag == 4) && mmode == 0)) {
		begline();
		fcolor(presfore);
		sprintf(lbuf, "%d mission%s",pp->nmission,
#ifndef	ISCORE
							plural(pp->nmission)
#else	/* ISCORE */
						(pp->nmission == 1) ? "" : "s"
#endif	/* ISCORE */
							);
		n = strlen(lbuf);
		sprintf(buf, "                 %s ", lbuf);
		strcpy(buf2, buf);
		for (p = buf2; *p; *p++ = ' ');
		nn2 = 0;
		for (i = 0; i < nmedals; i++) {
			m = medalo[nmedals-1 - i];
			if (!pp->nmedals[m])
				continue;
			*p++ = ' ';
			*p++ = medals[m].left;
			*p++ = medals[m].right;
			hismeds[nn2++] = m;
			sprintf(lbuf, " %s", medals[m].abb);
			strcat(buf, lbuf);
			nn1 = strlen(lbuf);
			for (j = 3; j < nn1; j++)
				*p++ = ' ';
			if (pp->nmedals[m] > 1) {
				sprintf(lbuf, ":%d", pp->nmedals[m]);
				strcat(buf, lbuf);
				nn1 = strlen(lbuf);
				for (j = 0; j < nn1; j++)
					*p++ = ' ';
			}
		}
		*p++ = ' ';
		*p = '\0';
		strcat(buf, " ");
		j = 0;
		i = strlen(&buf[j]);
		if (i > 72) {
			j = n + 2;
			i = strlen(&buf[j]);
		}
		if (i > 72) {
			j += i - 72;
			if (j > 17) {
				j = 17;
				i = strlen(&buf[j]);
			}
		}
		nn1 = i;
		for (p = &buf[i+j]; i < (72+j); i++)
			*p++ = ' ';
		*p = '\0';
		i = nn1;
		for (p = &buf2[i+j]; i < (72+j); i++)
			*p++ = ' ';
		*p = '\0';
		sput(&buf[j]);
		endline();
		cursor(80, liney-1);
		cput(DRIGHT);

		if (vflag == 0 || vflag == 4) {	/* already know mmode == 1 */
			begline();
			fcolor(presfore);
			nn2 = 0;
			for (p = &buf2[j]; *p; p++) {
				if (*p != ' ') {
/*					color(medals[hismeds[nn2]].fgnd,
						medals[hismeds[nn2]].bgnd);*/
					fcolor(medals[hismeds[nn2]].fgnd);
					nn2++;
					cput(*p++);
					cput(*p);
					color(presfore, pmissback);
				}
				else
					cput(*p);
			}
			endline();
			cursor(80, liney-1);
			cput(DRIGHT);
		}
	}

	begline();
	fcolor(presfore);
	if (lvflag) {
		long	tot, hours;
		int	mins;

		tot = pp->lastmiss.st3.seconds;
		hours = tot/3600;
		tot -= hours*3600;
		mins = (int)(tot/60);
		tot -= mins*60;
		sprintf(buf,"                 Flying time = %ld:%02d:%02ld",
						hours, mins, tot);
	}
	else {
		lbuf[0] = '\0';
#ifndef	ISCORE
		if ((vflag == 0 || vflag == 4) && mmode == 0) {
			sprintf(lbuf, "%d mission%s,  ",
					pp->nmission, plural(pp->nmission));
		}
#endif	/* !ISCORE */
		sprintf(buf, "                 %s%ld",
				lbuf, pp->totalmiss.st3.seconds/360);
		p = &buf[strlen(buf)+1];
		*p-- = '\0';
		*p = *(p-1);
		p--;
		*p = '.';
		strcat(buf, " hours flying time.");
	}
	i = strlen(buf);
	for (p = &buf[i]; i < 65; i++)
		*p++ = ' ';
	*p++ = '\0';
	sput(buf);
	endline();

	buf2[0] = '\0';
	if (!lvflag) {
		time(&now);
#ifdef	OLDSHARE
		if (pp->pflag&PFROZEN)
			strcpy(buf2, "FROZEN UNTIL PROG IS REGISTERED");
		else
#endif	/* OLDSHARE */
		     if (pp->pflag&PKILLED)
			strcpy(buf2, "K.I.A.");
		else if (pp->pflag&PCAPTURED)
			strcpy(buf2, "CAPTURED");
		else if (pp->stockade > now)
			strcpy(buf2, "CONFINED TO STOCKADE");
		else if (pp->pflag&PACTIVE)
			strcpy(buf2, "ACTIVE");
		else
			strcpy(buf2, "WAITING FOR DUTY");
	}
	else {
		if (pp->lastmiss.st3.flag&ABORT)
			strcat(buf2, "MISSION-ABORTED  ");
/*		if (pp->lastmiss.st3.flag&NEVERLEFT)
			strcat(buf2, "NEVER-LEFT-HOME  ");
 */
		if (pp->lastmiss.st3.flag&KILLED)
			strcat(buf2, "KILLED  ");
		if (pp->lastmiss.st3.flag&CAPTURED)
			strcat(buf2, "CAPTURED  ");
		if (pp->lastmiss.st3.flag&ATHOME)
			strcat(buf2, "AT-HOME  ");
		if (pp->lastmiss.st3.flag&RESCUED)
			strcat(buf2, "RESCUED  ");
		if (pp->lastmiss.st3.flag&CRASHNOTHOME)
			strcat(buf2, "CRASHED-NOT-HOME  ");
		if (pp->lastmiss.st3.flag&EJECTED)
			strcat(buf2, "EJECTED  ");
		if (pp->lastmiss.st3.flag&CRASHATHOME)
			strcat(buf2, "CRASHED-AT-HOME  ");
		if (pp->lastmiss.st3.flag&LANDEDHOME)
			strcat(buf2, "LANDED-AT-HOME  ");
		if (pp->lastmiss.st3.flag&HITBLDG)
			strcat(buf2, "COLLISION  ");
		if (pp->lastmiss.st3.flag&SUICIDE)
			strcat(buf2, "SUICIDE  ");
#ifdef REGISTERED
		if (pp->lastmiss.st3.flag&LOSTINSPACE)
			strcat(buf2, "IN ORBIT  ");
#endif
	}
	begline();
	fcolor(presfore);
	sput("        STATUS:  ");
	fcolor(highfore);
	sprintf(buf,  "%-48s", buf2);
	if (!lvflag && pp->stockade > now) {
		extern char	*ctime();

		buf[20] = '\0';
		sput(buf);
		fcolor(presfore);
		p = ctime(&pp->stockade);
		p[24] = '\0';
		sprintf(buf, " until %s.", p);
		sput(buf);
		endline();
	}
	else {
		sput(buf);
		endline();
	}
	cursor(80, liney-1);
	cput(DRIGHT);

	begline();
	fcolor(presfore);
	if (!lvflag) {
		sput("    HIGH SCORE:  ");
		fcolor(highfore);
		sprintf(buf, "%-7ld ", pp->bestmiss.score);
		sput(buf);
		fcolor(presfore);
		sput("     TOTAL SCORE:  ");
		fcolor(highfore);
		sprintf(buf, "%-10ld           ", pp->totalmiss.score);
		sput(buf);
	}
	else {
		sput("         SCORE:  ");
		sprintf(buf, "%ld ", pp->lastmiss.score);
		fcolor(highfore);
		sput(buf);
		i = strlen(buf);
		fcolor(letterfore);
		if (pp->lastmiss.score == pp->bestmiss.score)
			cput(3);
		else
			cput(' ');
		for (; i < 10; i++)
			cput(' ');
		fcolor(presfore);
		sput("BEST:  ");
		sprintf(buf, "%-7ld    ", pp->bestmiss.score);
		fcolor(highfore);
		sput(buf);
		fcolor(presfore);
		sput("TOTAL:  ");
		sprintf(buf, "%-10ld ", pp->totalmiss.score);
		fcolor(highfore);
		sput(buf);
	}
	endline();

	begline();
	if (!lvflag) {
		fcolor(normfore);
		sput(p66s);
	}
	else {
		fcolor(presfore);
		sput("          PMSF:  ");
		sprintf(buf, "%-5d", pmsf);
		fcolor(highfore);
		sput(buf);
		fcolor(presfore);
		sput("  MSF:  ");
		sprintf(buf, "%-5d", msf);
		fcolor(highfore);
		sput(buf);
		fcolor(presfore);
		sput("  MOBF:  ");
		sprintf(buf, "%-9ld", mobf);
		fcolor(highfore);
		sput(buf);
		sput("            ");
	}
	endline();

	if (lvflag) {
		begline();
		fcolor(normfore);
		sput(p66s);
		endline();
	}

	begline();
	fcolor(letterfore);
	p = "                                   ";
	if (vflag == 0 && mmode == 0)
		sprintf(buf, "                        Career%s", p);
	else if (vflag == 4 && mmode == 0)
		sprintf(buf, "                          This%s", p);
	else
		sprintf(buf,
"                          %s      Career      Best       Career",
						(!lvflag) ? "Last" : "This");
	sput(buf);
	endline();

	begline();
	fcolor(presfore);
	sput(" PRIMARY KILLS:");
	fcolor(letterfore);
	if (vflag == 0 && mmode == 0)
		sprintf(buf, "          Total%s", p);
	else if (vflag == 4 && mmode == 0)
		sprintf(buf, "        Mission%s", p);
	else
		sprintf(buf,
			"          Mission     High      Mission      Total");
	sput(buf);
	endline();

	llflag = 0;
	begline();
	lineout("Mission Objective",
		pp->lastmiss.st3.objectives, pp->bestofeach.st3.objectives,
		pp->bestmiss.st3.objectives, pp->totalmiss.st3.objectives);
	endline();

	begline();
	lineout("TRFRU/Fuel Dumps",
		pp->lastmiss.st3.tickskilled, pp->bestofeach.st3.tickskilled,
		pp->bestmiss.st3.tickskilled, pp->totalmiss.st3.tickskilled);
	endline();

	begline();
	last = 0;
	beoe = 0;
	best = 0;
	totl = 0;
	for (w = 0; w < NWICK; w++) {
		last += pp->lastmiss.st3.aaakilled[w];
		beoe += pp->bestofeach.st3.aaakilled[w];
		best += pp->bestmiss.st3.aaakilled[w];
		totl += pp->totalmiss.st3.aaakilled[w];
	}
	lineout("AAA Batteries", last, beoe, best, totl);
	endline();

	begline();
	last = 0;
	beoe = 0;
	best = 0;
	totl = 0;
	for (w = 0; w < NWICK; w++) {
		last += pp->lastmiss.st3.siteskilled[w];
		last += pp->lastmiss.st3.orbskilled[w];
		beoe += pp->bestofeach.st3.siteskilled[w];
		beoe += pp->bestofeach.st3.orbskilled[w];
		best += pp->bestmiss.st3.siteskilled[w];
		best += pp->bestmiss.st3.orbskilled[w];
		totl += pp->totalmiss.st3.siteskilled[w];
		totl += pp->totalmiss.st3.orbskilled[w];
	}
	lineout("Other Sites", last, beoe, best, totl);
	endline();

	if ((vflag == 0 || vflag == 4) && mmode == 0) {
		begline();
		fcolor(normfore);
		sput(p66s);
		endline();
	}

	begline();
	fcolor(presfore);
	sput(
"SECONDARY KILLS:                                                 ");
	endline();

	begline();
	last = 0;
	beoe = 0;
	best = 0;
	totl = 0;
	for (w = 0; w < NWICK; w++) {
		last += pp->lastmiss.st3.shckilled[w];
		beoe += pp->bestofeach.st3.shckilled[w];
		best += pp->bestmiss.st3.shckilled[w];
		totl += pp->totalmiss.st3.shckilled[w];
	}
	lineout("Ground Transports", last, beoe, best, totl);
	endline();

	begline();
	last = 0;
	beoe = 0;
	best = 0;
	totl = 0;
	for (w = 0; w < NWICK; w++) {
		last += pp->lastmiss.st3.dbkilled[w];
		beoe += pp->bestofeach.st3.dbkilled[w];
		best += pp->bestmiss.st3.dbkilled[w];
		totl += pp->totalmiss.st3.dbkilled[w];
	}
	lineout("Enemy Missiles", last, beoe, best, totl);
	endline();

	if (((vflag == 0 || vflag == 4) && mmode == 0) || vflag == 1) {
		begline();
		fcolor(normfore);
		sput(p66s);
		endline();
	}

	begline();
	fcolor(presfore);
	sput(p66s);
	endline();

	llflag = 1;
	begline();
	lineout("Number of Planes Lost",
		pp->lastmiss.st3.planeslost, pp->bestofeach.st3.planeslost,
		pp->bestmiss.st3.planeslost, pp->totalmiss.st3.planeslost);
	endline();

	begline();
	lineout("Friendly Sites Lost",
		pp->lastmiss.st3.friendkilled, pp->bestofeach.st3.friendkilled,
		pp->bestmiss.st3.friendkilled, pp->totalmiss.st3.friendkilled);
	endline();

	begline();
	lineout("Total Damage",
		pp->lastmiss.st3.totaldamage, pp->bestofeach.st3.totaldamage,
		pp->bestmiss.st3.totaldamage, pp->totalmiss.st3.totaldamage);
	endline();

	begline();
	fcolor(normfore);
	sput(p66s);
	endline();

	begline();
	fcolor(normfore);
	sput(p66s);
	endline();

	if (lvflag) {
		begline();
		fcolor(normfore);
		sput(p66s);
		endline();
	}
	if (vflag == 0 || vflag == 1) {
#ifndef	ISCORE
		if (vflag == 1)
			p = "";
		else if (mmode == 0)
			p = "<INS> for stats, ";
		else if (mmode == 1)
			p = "<INS> for medals, ";
		else
			p = "!!, ";
		sprintf(buf,
		 "Type \030\031 for next/prev, %sor ESC, BS, or CR to exit", p);
		center(buf2, 78, buf);
		cursor(2, 24);
		fcolor(presfore);
		sput(buf2);
#else	/* ISCORE */
		cursor(19, 24);
		fcolor(presfore);
		sput("Type \030\031 for next/prev, or ESC, BS, or CR to exit");
#endif	/* ISCORE */
	}

	cursor(1, liney);
	fcolor(borderfore);
	p = buf;
	*p++ = DLOWERLEFT;
	for (i = 0; i < 78; i++)
		*p++ = DBOTTOM;
	*p++ = DLOWERRIGHT;
	*p++ = '\0';
	sput(buf);

#ifndef	ISCORE
	if ((vflag == 0 || vflag == 4) && mmode == 0)
		mkmedls(pp);
#endif
}

static void	lineout(s, last, boe, best, total)
	char	*s;
	int	last, boe, best, total;
{
	char	buf[LINELEN];
	extern int	mmode;

	fcolor(letterfore);
	sprintf(buf, "%21s:  ", s);
	sput(buf);

#ifndef	ISCORE
	if (mmode == 0 && (evflag == 0 || evflag == 4)) {
		fcolor(normfore);
		sprintf(buf, "%41s", "");
		if (evflag == 0)
			sprintf(buf, "%6d", total);
		else if (evflag == 4)
			sprintf(buf, "%6d", last);
		buf[6] = ' ';
		sput(buf);
		return;
	}
#endif	/* !ISCORE */
	if (lvflag != 0)
		fcolor(highfore);
	else
		fcolor(normfore);
	sprintf(buf, "%5d ", last);
	sput(buf);
	fcolor(letterfore);
	if (lvflag != 0 && llflag == 0 && last == boe && last != 0)
		cput(3);
	else
		cput(' ');
	sput("    ");

	if (lvflag != 0 && !llflag) {
		if (last == boe)
			fcolor(highfore);
		else
			fcolor(normfore);
	}
	else
		fcolor(normfore);
	sprintf(buf, "%5d      ", boe);
	sput(buf);

	if (lvflag != 0 && !llflag) {
		if (last == best)
			fcolor(highfore);
		else
			fcolor(normfore);
	}
	sprintf(buf, "%5d      ", best);
	sput(buf);

	fcolor(normfore);
	sprintf(buf, "%8d", total);
	sput(buf);
}

static void	begline() {
	cursor(1, liney++);
	fcolor(borderfore);
	cput(DLEFT);
	sput("      ");
}

static void	endline() {
	fcolor(borderfore);
	sput("       ");
	cput(DRIGHT);
}

#ifndef	ISCORE

#define	NMEDLINES	15
#define	NMEDROWS	43
#define	MAXMEDPL	14

#define	MDXMESS		(80-NMEDROWS+1)
#define	MDYMESS		8

void	mkmedls(pp)
	struct pilot	*pp;
{
	int	i, j, k, y, m, nml, kmax, x;
	extern int	nmedals, medalo[];
	extern struct medal	medals[];

	nml = 0;
	kmax = 0;
	for (i = 0; i < nmedals; i++) {
		m = medalo[nmedals-1 - i];
		if (pp->nmedals[m] == 0)
			continue;
		for (j = pp->nmedals[m]; j > 0; ) {
			nml++;
			for (k = 0; k < MAXMEDPL && j > 0; k++, j--);
			if (k > kmax)
				kmax = k;
		}
	}
	fcolor(presfore);
	if (nml <= 12)
		j = 1;
	else
		j = 0;
	y = MDYMESS+j;
	if (kmax < 4)
		kmax = 4;
	x = MDXMESS+5 + (MAXMEDPL - kmax);
	xyprintf(x, y++, "DECORATIONS:");
	for (i = 0; i < nmedals; i++) {
		m = medalo[nmedals-1 - i];
		if (pp->nmedals[m] == 0)
			continue;
		fcolor(letterfore);
		xyprintf(x, y, "%3s:", medals[m].abb);
		fcolor(medals[m].fgnd);
		for (j = pp->nmedals[m]; j > 0; ) {
			cursor(x+5, y++);
			for (k = 0; k < MAXMEDPL && j > 0; k++, j--) {
				cput(medals[m].left);
				cput(medals[m].right);
			}
		}
	}
}
#endif	/* !ISCORE */

/*
-------------------------------- PILOT REPORT ---------------------------------|
-------------- MISSION REPORT:  This is a longg name Theater of Operations ----|
|                                                                              |
|               PILOT:  This is a long name.                                   |
|                       100 missions    100 resurrections                      |
|              STATUS:  HOME (KILLED, CRASHLANDED, etc)                        |
|               SCORE:  500000     BEST: 500000     TOTAL: 50000000            |
|                                                                              |
|                                This      Career      Best       Career       |
|       PRIMARY KILLS:          Mission     High      Mission      Total       |
|          Mission Objective:      1          1          1           200       |
|                 Fuel Dumps:     15         15         15         15000       |
|                  AAA Sites:     25         25         25         25000       |
|                Other Sites:     50         50         50         50000       |
|                                                                              |
|     SECONDARY KILLS:                                                         |
|          Ground Transports:     25         25         25         25000       |
|                Death Balls:     25         25         25         25000       |
|                                                                              |
|              DAMAGE:                                                         |
|      Number of Planes Lost:      2          2          0          1120       |
|        Friendly Sites Lost:     10         10          0           100       |
|               Total Damage:    100        100        100           100       |
|                                                                              |
|        (^v new pilot, ESC BS or CR to exit)                                  |
|------------------------------------------------------------------------------|
|          BEST SCORE:  500000     TOTAL SCORE:  50000000                      |
*/
