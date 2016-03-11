#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

#ifndef MVPCD
char	*scorelist = "pilot.hsc";
#else
extern	char wpath[80];
extern	char scrlist[80];
#define scorelist  strcat(strcpy(scrlist,wpath),"pilot.hsc")
#endif

#define	NKEPT	10

struct hiscore {
	char	name[22];
	char	theater[22];
	struct pilotmiss	lastmiss;	/* His last mission */
	int	deadflag;
} topscores[NKEPT];

struct hicareer	{
	char	name[22];
	int	nmission;			/* Number of missions */
	int	nresurected;			/* Number of times resurected */
	struct pilotmiss	totalmiss;	/* total of all missions */
	int	deadflag;
} topcareers[NKEPT], fastcareers[NKEPT];

void	inithiscr() {
	char	*p;
	FILE	*fp;
	extern unsigned int	coreleft();

	p = "Error!  Cannot %s high-score list %s cl=%u";
	if ((fp = fopen(scorelist, "rb")) == NULL) {
		if ((fp = fopen(scorelist, "wb")) == NULL) {
			extern int	errno;
			errorbox(p, "creat", scorelist, coreleft());
			errorbox("errno=%d ", errno);
			return;
		}
		fwrite((void *)topscores, sizeof(struct hiscore), NKEPT, fp);
		fwrite((void *)topcareers, sizeof(struct hicareer), NKEPT, fp);
		fwrite((void *)fastcareers, sizeof(struct hicareer), NKEPT, fp);
		if (ferror(fp)) {
			errorbox(p, "write on", scorelist, coreleft());
			fclose(fp);
			return;
		}
		fclose(fp);
	}
	fclose(fp);
}

/*
 **************** High Score Menu (level 2)
 */

extern void	topscore(), topcareer(), fastcareer(),
		dellist(), hshelp(), quit();
static struct cmdt	choices[] = {
	"see list of high \\Scores",				topscore,
	"see list of top \\Careers",				topcareer,
	"see list of \\Fastest careers",			fastcareer,
	"\\Delete one of the above lists",			dellist,
	"get \\Information about this menu",			hshelp,
	"\\Return to previous menu",				quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	hschoice;
void	hiscore() {
	struct cmdt	*ch;
	int far	*scr;
	extern void	outchoice(), showchoices();
	extern struct cmdt	*getchoice();

	hschoice = DEFCHOICE;
	simplesetup(HSXMESS, HSYMESS, HSXWID, HSYWID, "High Score Menu",
							BROWN, CYAN, BLUE);
	scr = yankscr();
	showchoices();
	while ((ch =
	        getchoice(choices, &hschoice, nchoices, outchoice,
				(int (*)())NULL))->ccmd != (void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		showchoices();
	}
	freescr(scr);
}

static void	showchoices() {
	int	i;
	extern void	outchoice();

	color(BLUE, CYAN);
	cursor(HSXMESS+10, HSYMESS+3);
	sput("Use arrow keys or type highlighted");
	cursor(HSXMESS+10, HSYMESS+4);
	sput("  letter to choose action:");

	for (i = 0; i < nchoices; i++)
		outchoice(i);
}

static void	outchoice(i) {
	cursor(HSXMESS+10, i+(HSYMESS+6));
	hsput(choices[i].scmd, LINELEN);
}

void	topscore() {
	int	i, j, y;
	struct hiscore	*pscore;
	FILE	*fp;
	extern int	pk(), ok();

	y = HSPYMESS;
	simplesetup(HSPXMESS, y++, HSPXWID, HSPYWID, "High Scores",
							BROWN, CYAN, BLUE);

	if ((fp = fopen(scorelist, "rb")) == NULL) {
		errorbox("Error!  Cannot open %s for reading", scorelist);
		return;
	}
	if (fread((void *)topscores, sizeof(struct hiscore), NKEPT, fp) !=
								NKEPT) {
		errorbox("Error!  Read problem on %s at high scores",scorelist);
		fclose(fp);
		unlink(scorelist);
		return;
	}
	fclose(fp);
	color(YELLOW, CYAN);
	xyprintf(HSPXMESS+2, y++,
"Name                       Score  Theater-of-Operations      Kills");
	y++;
	color(BRIGHTWHITE, CYAN);
	pscore = &topscores[0];
	for (i = 0; i < NKEPT; i++) {
		if (pscore->name[0] == '\0')
			break;
		cursor(HSPXMESS+2, y);
		sput(pscore->name);
		if (pscore->deadflag) {
			color(YELLOW, CYAN);
			cput('*');
			color(BRIGHTWHITE, CYAN);
		}
		else
			cput(' ');
		for (j = strlen(pscore->name); j < 20; j++)
			cput(' ');
		xyprintf(HSPXMESS+23, y++, "  %9ld  %-20s  %3d  %3d  %3d",
					pscore->lastmiss.score,
					pscore->theater,
					pscore->lastmiss.st3.objectives,
					pk(&pscore->lastmiss.st3),
					ok(&pscore->lastmiss.st3));

		pscore++;
	}
	color(BLUE, CYAN);
	y++;
	tkyo(HSPXMESS+3, y);
}

void	topcareer() {
	int	i, j, y;
	struct hicareer	*pcareer;
	FILE	*fp;
	extern int	pk(), ok();

	y = HSPYMESS;
	simplesetup(HSPXMESS, y++, HSPXWID, HSPYWID, "Top Careers",
							BROWN, CYAN, BLUE);

	if ((fp = fopen(scorelist, "rb")) == NULL) {
		errorbox("Error!  Cannot open %s for reading", scorelist);
		return;
	}
	fseek(fp, (long)NKEPT*sizeof(struct hiscore), SEEK_SET);
	if (fread((void *)topcareers, sizeof(struct hicareer), NKEPT, fp) !=
									NKEPT) {
		errorbox("Error!  Read problem on %s at high careers",
								scorelist);
		fclose(fp);
		unlink(scorelist);
		return;
	}
	fclose(fp);
	color(YELLOW, CYAN);
	xyprintf(HSPXMESS+2, y++,
"Name                 Total Score  Missions Resurrections     Kills");
	y++;
	color(BRIGHTWHITE, CYAN);
	pcareer = &topcareers[0];
	for (i = 0; i < NKEPT; i++) {
		if (pcareer->name[0] == '\0')
			break;
		cursor(HSPXMESS+2, y);
		sput(pcareer->name);
		if (pcareer->deadflag) {
			color(YELLOW, CYAN);
			cput('*');
			color(BRIGHTWHITE, CYAN);
		}
		else
			cput(' ');
		for (j = strlen(pcareer->name); j < 20; j++)
			cput(' ');
		xyprintf(HSPXMESS+23, y++,
			"  %9ld  %-8d %-8d    %4d %5d %5d",
					pcareer->totalmiss.score,
					pcareer->nmission,
					pcareer->nresurected,
					pcareer->totalmiss.st3.objectives,
					pk(&pcareer->totalmiss.st3),
					ok(&pcareer->totalmiss.st3));

		pcareer++;
	}
	color(BLUE, CYAN);
	y++;
	tkyo(HSPXMESS+3, y);
}

void	fastcareer() {
	int	i, j, y;
	struct hicareer	*pcareer;
	FILE	*fp;

	y = HSPYMESS;
	simplesetup(HSPXMESS, y++, HSPXWID, HSPYWID, "Fastest Careers",
							BROWN, CYAN, BLUE);

	if ((fp = fopen(scorelist, "rb")) == NULL) {
		errorbox("Error!  Cannot open %s for reading", scorelist);
		return;
	}
	fseek(fp, (long)NKEPT*sizeof(struct hiscore) +
			(long)NKEPT*sizeof(struct hicareer), SEEK_SET);
	if (fread((void *)fastcareers, sizeof(struct hicareer), NKEPT, fp) !=
									NKEPT) {
		errorbox("Error!  Read problem on %s at fast careers",
								scorelist);
		fclose(fp);
		unlink(scorelist);
		return;
	}
	fclose(fp);
	color(YELLOW, CYAN);
	xyprintf(HSPXMESS+2, y++,
"Name                 Total Score  Missions Resurrections   Points/Misn");
	y++;
	color(BRIGHTWHITE, CYAN);
	pcareer = &fastcareers[0];
	for (i = 0; i < NKEPT; i++) {
		if (pcareer->name[0] == '\0')
			break;
		cursor(HSPXMESS+2, y);
		sput(pcareer->name);
		if (pcareer->deadflag) {
			color(YELLOW, CYAN);
			cput('*');
			color(BRIGHTWHITE, CYAN);
		}
		else
			cput(' ');
		for (j = strlen(pcareer->name); j < 20; j++)
			cput(' ');
		xyprintf(HSPXMESS+23, y++,
			"  %9ld  %-8d %-8d   %16ld",
			pcareer->totalmiss.score, pcareer->nmission,
				pcareer->nresurected,
				pcareer->totalmiss.score/pcareer->nmission);

		pcareer++;
	}
	color(BLUE, CYAN);
	y++;
	tkyo(HSPXMESS+3, y);
}

void	hshelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Information about High Score Menu", GREEN, RED, WHITE);
	fileout("moag.inf", 7, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
}


/*
 **************** Delete a list Menu (level 3)
 */

extern void	 dtopscore(), dtopcareer(), dfastcareer(), dhhelp(), quit();
static struct cmdt	dchoices[] = {
	"delete the list of top \\Scores",			dtopscore,
	"delete the list of top \\Careers",			dtopcareer,
	"delete the list of \\Fastest careers",			dfastcareer,
	"get \\Information about this menu",			dhhelp,
	"\\Return to previous menu",				quit,
};
static int	ndchoices = (sizeof(dchoices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	dhschoice;
void	dellist() {
	struct cmdt	*ch;
	int far	*scr;
	int	fd;
	extern void	outdchoice(), showdchoices(), savehiscores();
	extern int	gethiscores();
	extern struct cmdt	*getchoice();

	dhschoice = DEFCHOICE;
	simplesetup(HSXMESS, HSYMESS, HSXWID, HSYWID, "Delete a Score List",
							GREEN, RED, WHITE);
	scr = yankscr();
	showdchoices();
	if ((fd = gethiscores()) < 0)
		return;
	while ((ch =
	        getchoice(dchoices, &dhschoice, ndchoices, outdchoice,
				(int (*)())NULL))->ccmd != (void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		showdchoices();
	}
	savehiscores(fd);
	freescr(scr);
}

static void	showdchoices() {
	int	i;
	extern void	outdchoice();

	color(WHITE, RED);
	cursor(HSXMESS+10, HSYMESS+3);
	sput("Use arrow keys or type highlighted");
	cursor(HSXMESS+10, HSYMESS+4);
	sput("  letter to choose action:");

	for (i = 0; i < ndchoices; i++)
		outdchoice(i);
}

static void	outdchoice(i) {
	cursor(HSXMESS+10, i+(HSYMESS+6));
	hsput(dchoices[i].scmd, LINELEN);
}

void	dtopscore() {
	int	i;
	extern int	dchksure();

	if (!dchksure("high score"))
		return;
	for (i = 0; i < NKEPT; i++)
		topscores[i].name[0] = '\0';
}

void	dtopcareer() {
	int	i;
	extern int	dchksure();

	if (!dchksure("top career"))
		return;
	for (i = 0; i < NKEPT; i++)
		topcareers[i].name[0] = '\0';
}

void	dfastcareer() {
	int	i;
	extern int	dchksure();

	if (!dchksure("fast career"))
		return;
	for (i = 0; i < NKEPT; i++)
		fastcareers[i].name[0] = '\0';
}

int	dchksure(s)
	char	*s;
{
	int	i;

	simplesetup(HSXMESS+7, HSYMESS+4, 43, 7,
				"Confirm Deletion", MAGENTA, WHITE, RED);
	xyprintf(HSXMESS+9, HSYMESS+6, "Type <ENTER> to delete %s list", s);
	xyprintf(HSXMESS+9, HSYMESS+7, " Almost anything else cancels...");
	i = spgetch();
	if (i != '\n')
		return(0);
	return(1);
}

void	dhhelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Information about Deleting a List", GREEN, RED, WHITE);
	fileout("moag.inf", 8, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
}

/*
 *	End of menu routines, start called-after-mission code
 */

void	addin(pp)
	struct pilot	*pp;
{
	struct hiscore	*pscore;
	struct hicareer	*pcareer;
	int	i, addin, fd;

	extern int	cmpscore(), cmpfast(), gethiscores();
	extern void	savehiscores();
	extern struct thnamlist	*getthnam();
	extern struct pilot	trainee;


	if ((fd = gethiscores()) < 0)
		return;

	for (i = 0, pscore = topscores; i < NKEPT; i++, pscore++)
		if (pscore->name[0] == '\0')
			break;
	addin = -1;
	if (i < NKEPT)
		addin = i;
	else {
		if (cmpscore(&pp->lastmiss, &topscores[NKEPT-1].lastmiss) > 0)
			addin = NKEPT-1;
	}
	if (addin != -1) {
		for (i = addin-1; i >= 0; i--) {
			if (cmpscore(&pp->lastmiss, &topscores[i].lastmiss) > 0)
				topscores[i+1] = topscores[i];
			else
				break;
		}
		i++;
		topscores[i].lastmiss = pp->lastmiss;
		strcpy(topscores[i].name, pp->name);
		if (pp->curtheater&0x80)
			strcpy(topscores[i].theater, "Training Mission");
		else
			strcpy(topscores[i].theater,
					getthnam(pp->curtheater)->name);
		topscores[i].deadflag = 0;
	}

	if (pp == &trainee)
		goto end;

	for (i = 0, pcareer = topcareers; i < NKEPT; i++, pcareer++)
		if (pcareer->name[0] != '\0' &&
		    strcmp(pcareer->name, pp->name) == 0 && !pcareer->deadflag)
			break;
	if (i != NKEPT)
		addin = i;
	else {
		for (i = 0, pcareer = topcareers; i < NKEPT; i++, pcareer++)
			if (pcareer->name[0] == '\0')
				break;
		if (i < NKEPT)
			addin = i;
		else if (cmpscore(&pp->totalmiss,
					&topcareers[NKEPT-1].totalmiss) > 0)
			addin = NKEPT-1;
		else
			addin = -1;
	}
	if (addin != -1) {
		for (i = addin-1; i >= 0; i--) {
			if (cmpscore(&pp->totalmiss,&topcareers[i].totalmiss)>0)
				topcareers[i+1] = topcareers[i];
			else
				break;
		}
		i++;
		topcareers[i].totalmiss = pp->totalmiss;
		topcareers[i].nmission = pp->nmission;
		topcareers[i].nresurected = pp->nresurected;
		strcpy(topcareers[i].name, pp->name);
		topcareers[i].deadflag = 0;
	}

	if (pp->nmission < 3)
		goto end;

	for (i = 0, pcareer = fastcareers; i < NKEPT; i++, pcareer++)
		if (pcareer->name[0] != '\0' &&
		    strcmp(pcareer->name, pp->name) == 0 && !pcareer->deadflag)
			break;
	if (i != NKEPT)
		addin = i;
	else {
		for (i = 0, pcareer = fastcareers; i < NKEPT; i++, pcareer++)
			if (pcareer->name[0] == '\0')
				break;
		if (i < NKEPT)
			addin = i;
		else if (cmpfast(pp, &fastcareers[NKEPT-1]) > 0)
			addin = NKEPT-1;
		else
			addin = -1;
	}
	if (addin != -1) {
		for (i = addin-1; i >= 0; i--) {
			if (cmpfast(pp, &fastcareers[i]) > 0)
				fastcareers[i+1] = fastcareers[i];
			else
				break;
		}
		i++;
		fastcareers[i].totalmiss = pp->totalmiss;
		fastcareers[i].nmission = pp->nmission;
		fastcareers[i].nresurected = pp->nresurected;
		strcpy(fastcareers[i].name, pp->name);
		fastcareers[i].deadflag = 0;
	}

end:
	savehiscores(fd);
}

int	gethiscores() {
	int	fd;

	if ((fd = open(scorelist, O_RDWR)) < 0) {
		errorbox("Error!  cannot open %s to modify", scorelist);
		return(-1);
	}
	if (read(fd, (void *)topscores, NKEPT*sizeof(struct hiscore)) !=
						NKEPT*sizeof(struct hiscore)) {
		errorbox("Error!  cannot read high scores from %s", scorelist);
		close(fd);
		return(-1);
	}
	if (read(fd, (void *)topcareers, NKEPT*sizeof(struct hicareer)) !=
						NKEPT*sizeof(struct hicareer)) {
		errorbox("Error!  cannot read top careers from %s", scorelist);
		close(fd);
		return(-1);
	}
	if (read(fd, (void *)fastcareers, NKEPT*sizeof(struct hicareer)) !=
						NKEPT*sizeof(struct hicareer)) {
		errorbox("Error!  cannot read fast careers from %s", scorelist);
		close(fd);
		return(-1);
	}
	return(fd);
}

void	savehiscores(fd)
	int	fd;
{
	lseek(fd, 0L, SEEK_SET);
	write(fd, (void *)topscores, NKEPT*sizeof(struct hiscore));
	write(fd, (void *)topcareers, NKEPT*sizeof(struct hicareer));
	write(fd, (void *)fastcareers, NKEPT*sizeof(struct hicareer));
	close(fd);
}

int	cmpscore(p1, p2)
	struct pilotmiss	*p1, *p2;
{
	int	k1, k2;
	extern int	pk(), ok();

	if (p1->score > p2->score)
		return(1);
	else if (p1->score < p2->score)
		return(-1);

	if (p1->st3.objectives > p2->st3.objectives)
		return(1);
	else if (p1->st3.objectives < p2->st3.objectives)
		return(-1);

	k1 = pk(&p1->st3);
	k2 = pk(&p2->st3);
	if (k1 > k2)
		return(1);
	else if (k1 < k2)
		return(-1);

	k1 = ok(&p1->st3);
	k2 = ok(&p2->st3);
	if (k1 > k2)
		return(1);
	else if (k1 < k2)
		return(-1);

	return(0);
}

int	cmpfast(p1, p2)
	struct pilot	*p1;
	struct hicareer	*p2;
{
	long	rat;

	if (p1->nmission == 0) {
		if (p2->nmission == 0)
			return(0);
		else
			return(-1);
	}
	else if (p2->nmission == 0)
		return(1);

	rat = p1->totalmiss.score/p1->nmission -
					p2->totalmiss.score/p2->nmission;
	if (rat > 0)
		return(1);
	else if (rat < 0)
		return(-1);

	if (p1->totalmiss.score > p2->totalmiss.score)
		return(1);
	else if (p1->totalmiss.score < p2->totalmiss.score)
		return(-1);

	return(0);
}

int	pk(p3)
	struct stuffrm3	*p3;
{
	int	v, w;

	v = p3->tickskilled + p3->objectives;
	for (w = 0; w < NWICK; w++)
		v += p3->siteskilled[w] + p3->aaakilled[w] + p3->orbskilled[w];
	return(v);
}

int	ok(p3)
	struct stuffrm3	*p3;
{
	int	v, w;

	v = 0;
	for (w = 0; w < NWICK; w++)
		v += p3->shckilled[w] + p3->dbkilled[w];
	return(v);
}

void	markdead(name)
	char	*name;
{
	int	i, fd;
	extern int	gethiscores();
	extern void	savehiscores();

	if ((fd = gethiscores()) < 0)
		return;

	for (i = 0; i < NKEPT; i++)
		if (topscores[i].name[0] != '\0' &&
		    strcmp(topscores[i].name, name) == 0)
			topscores[i].deadflag = 1;

	for (i = 0; i < NKEPT; i++)
		if (topcareers[i].name[0] != '\0' &&
		    strcmp(topcareers[i].name, name) == 0)
			topcareers[i].deadflag = 1;

	for (i = 0; i < NKEPT; i++)
		if (fastcareers[i].name[0] != '\0' &&
		    strcmp(fastcareers[i].name, name) == 0)
			fastcareers[i].deadflag = 1;

	savehiscores(fd);
}

void	chhname(old, new)
	char	*old, *new;
{
	int	i, fd;
	extern int	gethiscores();
	extern void	savehiscores();

	if ((fd = gethiscores()) < 0)
		return;

	for (i = 0; i < NKEPT; i++)
		if (topscores[i].name[0] != '\0' && 
		    strcmp(topscores[i].name, old) == 0 &&
		    !topscores[i].deadflag)
			strcpy(topscores[i].name, new);

	for (i = 0; i < NKEPT; i++)
		if (topcareers[i].name[0] != '\0' &&
		    strcmp(topcareers[i].name, old) == 0 &&
		    !topcareers[i].deadflag)
			strcpy(topcareers[i].name, new);

	for (i = 0; i < NKEPT; i++)
		if (fastcareers[i].name[0] != '\0' &&
		    strcmp(fastcareers[i].name, old) == 0 &&
		    !fastcareers[i].deadflag)
			strcpy(fastcareers[i].name, new);

	savehiscores(fd);
}

/*
| |                                                         Objectives      |X |
| |                                                         |   Primary     |X |
| |                                                         |   |    Second |X |
| |                                                                         |X |
| | Name                       Score  Theater-of-Operations      Kills      |X |
| | Gonzo Q. Tortilla iv*  123456789  Milk-runs for idiots   14  123  147   |X |
| |                                                                         |X |
| | Name                 Total Score  Missions Resurrections     Kills      |X |
| | Gonzo Q. Tortilla iv*  123456789  130      127           145 1235  1475 |X |
| |                                                                         |X |
| | Name                 Total Score  Missions Resurrections   Points/Misn  |X |
| | Gonzo Q. Tortilla iv*  123456789  130      127             123456789.0  |X |
| |                                                                         |X |
*/
