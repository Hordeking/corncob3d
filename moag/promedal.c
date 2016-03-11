/* #define showp 1 */
#include	<stdio.h>
#include	<string.h>
#ifdef showp
#include <io.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <stdlib.h>
#endif

#include	"moag.h"
#include	"texto.h"

/*
 *	Rank and promotion defs...
 */
struct rank	ranks[] = {
	{ "2nd Lieutenant",	"2nd Lt."	},
	{ "1st Lieutenant",	"1st Lt."	},
	{ "Captain",		"Capt."		},
	{ "Major",		"Maj."		},
	{ "Lt. Colonel",	"Lt. Col."	},
	{ "Colonel",		"Col."		},
	{ "General",		"Gen."		},
};
#define	LTN2	0
#define	LTN1	1
#define	CAPTAIN	2
#define	MAJOR	3
#define	LTCOL	4
#define	COLONEL	5
#define	GENERAL	6

/*
 *	Medal and award defs...
 */
#define	PHDAMAGE	27		/* Damage required for purple heart */
#define	BSSCORE		10000L		/* Points required for Bronze Star */
#define	SSSCORE		20000L		/* Points required for Silver Star */
#define	GSSCORE		50000L		/* Points required for Gold Star */
#define	STSCORE		100000L		/* Points required for Stellar Cluster*/
#define	DSCOBJS		5		/* Objectives required for DFC */
#define	SSMISNS		10		/* Missions required of Service Star */

char	award[] = "awarded the";
struct medal	medals[] = {
	{ award, "Purple Heart",			"PH",
		"surviving extreme injury in combat",
		208, 224, LIGHTMAGENTA, BLUE
	},

	{ award, "Bronze Star",				"BS",
		"inflicting heavy damage to the enemy",
		209, 225, BROWN, BLUE
	},

	{ award, "Silver Star",				"SS",
		"inflicting severe damage to the enemy",
		209, 225, WHITE, BLUE
	},

	{ award, "Gold Star",				"GS",
		"inflicting extreme damage to the enemy",
		209, 225, REALYELLOW, BLUE
	},

	{ award, "Distinguished Service Cross",		"DSC",
		"extraordinary accomplishment of objectives",
		210, 226, BROWN, BLUE
	},

	{ award, "Service Award",			"SA",
		"surviving numerous combat sorties",
		211, 227, REALYELLOW, BLUE
	},

	{ award, "Stellar Cluster",			"SC",
		"inflicting catastrophic damage to the enemy",
		212, 228, BRIGHTWHITE, BLACK
	},

	{ "designated a", "Combat Master",		"CM",
		"Completely obliterating the Alien threat",
		213, 229, BRIGHTWHITE, BLUE
	},

	{ award, "Distinguished Flying Cross",		"DFC",
		"extraordinary services without losing a plane",
		214, 230, REALYELLOW, BLUE
	},

	{ award, "Theater Combat Medal",		"TCM",
		"completing all mission objectives within a T-of-O",
		215, 231, LIGHTRED, BLACK
	},
};
int	nmedals = (sizeof(medals)/sizeof(struct medal));
#define	PURPHEART	0
#define	BRONZEST	1
#define	SILVERST	2
#define	GOLDST		3
#define	DSCROSS		4
#define	SERVAWD		5
#define	STELLAR		6
#define	CMASTER		7
#define	DFCROSS		8
#define	TCMEDAL		9

int	medalo[MAXMEDALS] = {
	SERVAWD,
	PURPHEART,
	BRONZEST,
	SILVERST,
	GOLDST,
	STELLAR,
	DSCROSS,
	TCMEDAL,
	DFCROSS,
	CMASTER,
};



#ifdef showp


#define NSTEPS 16

extern int GRMODE();
extern void TXMODE();
extern  void WRTPLANE();

void readfilepal(frp,palptr)
	int	frp;
unsigned char *palptr;
{
	unsigned int color;
	long size;


	/*
	size=filelength(frp);
	if(size < 768) { myprintf("Error, palette file must be 768 bytes");
                                     exit(0); }
         */
	lseek(frp,-768L,SEEK_END);
	read(frp,palptr,768);
	lseek(frp,0L,SEEK_SET);
/*
*/
	return;
}


int fadeup(origpal)
unsigned char origpal[256];
{
	extern void fadepal();
	int	i;
	for(i=0;i<NSTEPS;i++) {
	 fadepal(origpal,i);
	 delay(400/NSTEPS);
	 if(kbhit()) return(-1);
	}
	fadepal(origpal,NSTEPS);
	return(0);
}

int fadedown(origpal)
unsigned char origpal[256];
{
	extern void fadepal();
	int	i;
	for(i=0;i<NSTEPS;i++) {
	 fadepal(origpal,NSTEPS-i-1);
	 delay(400/NSTEPS);
	 if(kbhit()) return(-1);
	}
	return(0);
}


void fadepal(origpal,factor)
unsigned char origpal[256];
unsigned int factor;
{
	unsigned int color,divisor;
	unsigned char ired1,igrn1,iblue1;

		divisor=NSTEPS;
		for (color=0; color < 256; color++)
		{
			ired1=origpal[(color*3)+0];
			igrn1=origpal[(color*3)+1];
			iblue1=origpal[(color*3)+2];

			if(factor!=divisor) {
			  ired1 = (long) ired1 * (long) factor / divisor;
			  igrn1 = (long) igrn1 * (long) factor / divisor;
			  iblue1 = (long) iblue1 * (long) factor / divisor;
			}

			outportb(0x3C8,color);
			outportb(0x3C9,ired1);
			outportb(0x3C9,igrn1);
			outportb(0x3C9,iblue1);
		}
}





void showpics() {

	extern	void mygprintf();
	extern	int vsmygetch();
	int vhandle,vxorg,vyorg,i,vxsize,vysize;
	unsigned char tnambcpal[768];
	unsigned char gline[256];
	int picnum;
	int textcolor;
	int numlines;
	char	vbuf[40];
	FILE *fpict;

	if ((fpict = fopen("mpict.dat", "rb")) != NULL) {

	fgets(vbuf,39,fpict);
	vxorg=atoi(vbuf);

	while(vxorg >= 0) {
	fgets(vbuf,39,fpict);
	vyorg=atoi(vbuf);

	fgets(vbuf,39,fpict);

	if(strlen(vbuf)>2) vbuf[strlen(vbuf)-2]=0;  /* remove cr character */
	GRMODE();

	vhandle=open(vbuf, O_RDONLY | O_BINARY);
	if(vhandle >0) { 
	readfilepal(vhandle,tnambcpal);
//	setpal(tnambcpal);

	fadepal(tnambcpal,0);

	read(vhandle,gline,4);
	vxsize=gline[0]+gline[1]*256+1;
	vysize=gline[2]+gline[3]*256+1;


	for(i=0;i<vysize;i++) {
	    read(vhandle,gline,vxsize);
            WRTGLINE(vxsize,320*(i+vyorg)+vxorg, gline);
	}
	close(vhandle);
	}

	fgets(vbuf,39,fpict);
	numlines=atoi(vbuf);

	fgets(vbuf,39,fpict);
	textcolor=atoi(vbuf);

	for(i=0;i<numlines;i++) {

	fgets(vbuf,39,fpict);
	vxorg=atoi(vbuf);

	fgets(vbuf,39,fpict);
	vyorg=atoi(vbuf);

	fgets(vbuf,39,fpict);

	if(strlen(vbuf)>2) vbuf[strlen(vbuf)-2]=0;  /* remove cr character */
	mygprintf(vbuf,vxorg,vyorg,textcolor);

	}

	fadeup(tnambcpal);
	vsmygetch();
	fadedown(tnambcpal);
	fgets(vbuf,39,fpict);
	vxorg=atoi(vbuf);

	}
	fclose(fpict);

	}
	TXMODE();
}

void	poscurs(int txorg, int tyorg) {
	union REGS	regs;

	regs.h.ah = 02;
	regs.h.bh = 0;
	regs.h.dh = (unsigned char)tyorg;
	regs.h.dl = (unsigned char)txorg;
	int86(0x10, &regs, &regs);
}

void	mygprintf(char st[],int txorg,int tyorg,int tcolor) {
	union REGS	regs;
	int i;
	extern void poscurs();

	for(i=0;i<strlen(st);i++) {
	  poscurs(txorg+i,tyorg);
	  regs.h.ah = 0x9;
	  regs.h.al = st[i];
	  regs.h.bh = 0;
	  regs.h.bl = (unsigned char) tcolor;
	  regs.h.ch = (unsigned char)0;
	  regs.h.cl = (unsigned char)1;
	  int86(0x10, &regs, &regs);
	}
}


int	vsmygetch() {
	union REGS	regs;
	int	c;

	regs.h.ah = 0x8;
	int86(0x21, &regs, &regs);
	c = regs.h.al;
	return(c);
}





#endif /* ifdef showp */






#ifndef	ISCORE

int	promoted, genflag;
int	newmedals[MAXMEDALS];

/*
 *	Calculate promotion and award
 */
void	promote(pp)
	struct pilot	*pp;
{
	int	new, i, t, npt, nct, mk, cp, ct, nreqth, tcmedal, dfcross;
	long	now;
	struct thnamlist	*pthn;

	extern int	ntheaters, nmedals;
	extern long	time();
	extern struct thnamlist	*getthnam();
	extern struct theatert	*theaters, curtheater;
	extern struct pilot	curthpilot;

	/*
	 *	First check the Stockade.
	 */
	if (!(pp->pflag&(PKILLED|PCAPTURED)) && pp->lastmiss.st3.friendkilled) {
		time(&now);
		if (pp->stockade < now) {
			pp->stockade = now;
		}
		pp->stockade += 86400L*pp->lastmiss.st3.friendkilled;
		return;
	}

	/*
	 *	Now do the promotions...
	 */
 	nct = 0;	/* Number of complete theaters */
	nreqth = 0;	/* Number of "required" theaters */
 	npt = 0;	/* Number of promotion-worthy theaters */
	tcmedal = 0;	/* Is current theater complete? */
	dfcross = 0;	/* If so, are any planes lost */
 	for (t = 0; t < ntheaters; t++) {
		ct = 0;
		mk = -1;
		for (i = 0; i < pp->ntheaters; i++) {
			cp = 0;
			pthn = getthnam(i);
			if (strcmp(theaters[t].name, pthn->name) != 0)
				continue;
			if (pthn->ncobjectives > mk)
				mk = pthn->ncobjectives;
/*
			if (pthn->ncobjectives >= theaters[t].nobjectives &&
					pthn->ncobjectives > 0)
 */
 /*
			if (!pthn->completed &&
			     pthn->nobjectives == 0 && pthn->ncobjectives > 0)
				cp = 1;
 */
			if (pthn->nobjectives == 0 && pthn->ncobjectives > 0) {
				ct = 1;
				if (!pthn->completed)
					cp = 1;
			}
			if (cp && i == pp->curtheater) {
				if (!pthn->completed) {
					tcmedal = 1;
					if (curthpilot.totalmiss.st3.planeslost
					 				== 0)
						dfcross = 1;
					pthn->completed = 1;
					curtheater.completed = 1;
				}
			}
		}
		if (theaters[t].isreq) {
			nreqth++;
			if (ct)
				nct++;
		}
		if (mk >= 20)
			npt++;
	}

#ifndef REGISTERED
#ifdef NEWRANKS
	if (pp->lastmiss.st3.flag&LOSTINSPACE) {
		new = MAJOR;
		if (pp->rank != MAJOR)
			genflag = 1;
	}
	else
#else
	if (pp->lastmiss.st3.flag&LOSTINSPACE) {
		new = GENERAL;
		if (pp->rank != GENERAL)
			genflag = 1;
	}
	else
#endif
#endif
#ifdef REGISTERED

	 if (npt >= 3)		/* Three partial theaters for Colonel */
		new = COLONEL;
	else if (npt >= 2)			/* Two for Light Col. */
		new = LTCOL;
	else if (pp->totalmiss.st3.objectives >= 20)	/* 20 MO for Major */
		new = MAJOR;
	else
#endif /* registered */

	if (pp->totalmiss.st3.objectives >= 7)	/* 7 for Capt */
		new = CAPTAIN;
	else if (pp->totalmiss.st3.objectives >= 1)	/* 1 for 1st Lt. */
		new = LTN1;
	else
		new = LTN2;

	/*
	 *	Now do the medals...
	 */
	for (i = 0; i < nmedals; i++)
		newmedals[i] = 0;

	if (dfcross) {
		newmedals[DFCROSS] = 1;
		pp->nmedals[DFCROSS]++;
	}
	if (tcmedal) {
		newmedals[TCMEDAL] = 1;
		pp->nmedals[TCMEDAL]++;
	}

	if (nct == nreqth && nreqth >= MINREQTH) {
#ifdef REGISTERED
		new = GENERAL;
		if (pp->rank != GENERAL && !(pp->pflag&PKILLED))
			genflag = 1;
#endif
		if (!(pp->pflag&PKILLED)) {
			pp->nmedals[CMASTER] = 1;
			newmedals[CMASTER] = 1;
		}
	}

	if (new > pp->rank) {
		promoted = new - pp->rank;
		pp->rank = (unsigned char)new;
	}
	else
		promoted = 0;

	if (pp->lastmiss.st3.totaldamage >= PHDAMAGE &&
	    (pp->lastmiss.st3.flag&ATHOME) &&
	     pp->lastmiss.st3.planeslost == 0 &&
             !(pp->lastmiss.st3.flag&CRASHNOTHOME)) {
		newmedals[PURPHEART] = 1;
		pp->nmedals[PURPHEART]++;
	}

	if (pp->lastmiss.score > STSCORE) {
		newmedals[STELLAR] = 1;
		pp->nmedals[STELLAR]++;
	}
	else if (pp->lastmiss.score > GSSCORE) {
		newmedals[GOLDST] = 1;
		pp->nmedals[GOLDST]++;
	}
	else if (pp->lastmiss.score > SSSCORE) {
		newmedals[SILVERST] = 1;
		pp->nmedals[SILVERST]++;
	}
	else if (pp->lastmiss.score > BSSCORE) {
		newmedals[BRONZEST] = 1;
		pp->nmedals[BRONZEST]++;
	}

	if (pp->lastmiss.st3.objectives >= DSCOBJS) {
		newmedals[DSCROSS] = 1;
		pp->nmedals[DSCROSS]++;
	}

	if (pp->nmission >= (pp->nmedals[5]+1)*SSMISNS &&
				!(pp->lastmiss.st3.flag&KILLED)) {
		newmedals[SERVAWD] = 1;
		pp->nmedals[SERVAWD]++;
	}
}

char	*adjs[] = {
	"",
	"dazzling ",
	"fantastic ",
	"really-beyond-belief ",
};
void	showprom(pp)
	struct pilot	*pp;
{
	int	y;
	char	buf[LINELEN], buf1[LINELEN], rbuf[16], *p, *pc;
	extern struct rank	ranks[];
	extern void	center(), boxsetup();

	if (promoted < 1)
		return;
	beep();beep();beep();
	color(BLUE, CYAN);
	y = PRYMESS;
	boxsetup(PRXMESS, y++, PRXWID, PRYWID, 1);
	y++;
	if (pp->pflag&PKILLED)
		pc = "Former ";
	else
		pc = "Congratulations former ";
	sprintf(buf, "%s %s %s", pc, ranks[pp->rank-promoted].abbrank,
								pp->name);
	if (pp->nresurected) {
		sprintf(rbuf, " (%d res.),", pp->nresurected);
		strcat(buf, rbuf);
	}
	else
		strcat(buf, "!");
	center(buf1, PRXWID-2, buf);
	cursor(PRXMESS+2, y++);
	for (p = buf1; *p == ' '; p++)
		cput(' ');
	sput(pc);
	pp->rank -= promoted;
	outpname(pp);
	pp->rank += promoted;
	if (pp->nresurected) {
		color(REALYELLOW, CYAN);
		sput(rbuf);
		color(BLUE, CYAN);
	}
	else {
		if (pp->pflag&PKILLED)
			cput(',');
		else
			cput('!');
	}
	y++;
	sprintf(buf, "For %s %sdisplay of combat",
			(pp->pflag&PKILLED) ? "a" : "your",
				adjs[(promoted < 4) ? (promoted-1) : 3]); 
	center(buf1, PRXWID-2, buf);
	cursor(PRXMESS+1, y++);
	sput(buf1);
	if (pp->pflag&PKILLED)
		xyprintf(PRXMESS+10, y++,
			"prowess, is posthumously promoted to the rank of");
	else
		xyprintf(PRXMESS+10, y++,
			"prowess, you have been promoted to the rank of");
	sprintf(buf, "%c%c%c%c%c%c%c%c %s %c%c%c%c%c%c%c%c",
		219, 219, 178, 178, 177, 177, 176, 176, ranks[pp->rank].rank,
		176, 176, 177, 177, 178, 178, 219, 219);
	center(buf1, PRXWID-2, buf);
	y++;
	color(REALYELLOW, CYAN);
	cursor(PRXMESS+1, y++);
	sput(buf1);
	y++;
	color(BLUE, CYAN);
	tkyo(PRXMESS+16, y);
	promoted = 0;
}

void	showmedals(pp)
	struct pilot	*pp;
{
	int	y, i, m;
	char	buf[LINELEN], buf1[LINELEN], rbuf[16], *p, *pc;
	int far *scr;
	extern struct medal	medals[];
	extern struct rank	ranks[];
	extern int	nmedals;
	extern void	center(), boxsetup(), showmaster();
#ifdef showp
	extern int far *yankscr();
	extern void putscr(),inito();
#endif

	for (i = 0; i < nmedals; i++) {
		m = medalo[i];
		if (!newmedals[m])
			continue;
		beep();beep();beep();
		color(WHITE, RED);
		y = PRYMESS;
		boxsetup(PRXMESS, y++, PRXWID, PRYWID, 1);
		y++;
		if (pp->pflag&PKILLED)
			pc = "";
		else
			pc = "Congratulations ";
		sprintf(buf, "%s%s %s", pc, ranks[pp->rank].abbrank,
								pp->name);
		if (pp->nresurected) {
			sprintf(rbuf, " (%d res.),", pp->nresurected);
			strcat(buf, rbuf);
		}
		else
			strcat(buf, "!");
		center(buf1, PRXWID-2, buf);
		cursor(PRXMESS+2, y++);
		for (p = buf1; *p == ' '; p++)
			cput(' ');
		sput(pc);
		outpname(pp);
		if (pp->nresurected) {
			color(REALYELLOW, RED);
			sput(rbuf);
			color(WHITE, RED);
		}
		else {
			if (pp->pflag&PKILLED)
				cput(',');
			else
				cput('!');
		}
		y++;
		sprintf(buf, "For %s%s", medals[m].whatfor,
			(pp->pflag&PKILLED) ? "," : "");
		center(buf1, PRXWID-2, buf);
		cursor(PRXMESS+1, y++);
		sput(buf1);
		if (pp->pflag&PKILLED)
			sprintf(buf, "is posthumously %s", medals[m].how);
		else
			sprintf(buf, "you have been %s", medals[m].how);
		center(buf1, PRXWID-2, buf);
		cursor(PRXMESS+1, y++);
		sput(buf1);
		y++;
		color(REALYELLOW, RED);
		sprintf(buf, "%s  __ ", medals[m].name);
		center(buf1, PRXWID-2, buf);
		cursor(PRXMESS+1, y);
		sput(buf1);
		for (p = buf1; *p != '_'; p++);
		cursor(PRXMESS+1 + (p-buf1-1), y++);
		color(medals[m].fgnd, BLUE);
		cput(' ');
		cput(medals[m].left);
		cput(medals[m].right);
		cput(' ');

		color(WHITE, RED);
		y++;

#ifndef	REGISTERED
		if (m == CMASTER) {
			if (newmedals[CMASTER]) {
#ifdef showp
				spgetch();
				scr = yankscr();
				showpics();  /* show picture of earth w/ text */
				inito();
				putscr(scr);
#endif /* showp */
				center(buf1, PRXWID-2,
				    "Prepare to receive for new orders.");
				cursor(PRXMESS+1, y);
				color(BRIGHTWHITE, RED);
				blink();
				sput(buf1);
				normal();
				spgetch();
				showmaster();
				newmedals[CMASTER] = 0;
				continue;
			}
		}
#endif	/* !REGISTERED */
		newmedals[m] = 0;
		tkyo(PRXMESS+16, y);
	}
}

/* #ifdef	REGISTERED */
char	*secretorders[] = {
	"  You have done an excellent job of protecting the earth from",
	"the terrible immediate threat of defeat.  Unfortunately, our",
	"story does not come to an end yet.  Our top scientists are",
        "reporting a massive force gathering in nearby solar systems.",
	"",
	"  The war research department has just finished a prototype for a",
	"new fighter, capable of speeds greater that 1000 mph with an un-",
	"limited ceiling.  This fighter also has some experimental appar-",
	"atus aboard which is based on stolen alien portal technology.",
	"We are really hoping that this new hardware will prevent another",
	"war from being fought here on the surface of the earth.",
	"",
	"  Your mission is to report to the White-Sands proving grounds in",
	"order to test fly this new aircraft.  Your orders are to climb",
	"as high as you can, and then hit the large button marked '<enter>'.",
	"Don't forget to use the emergency rocket booster, and note that",
	"the altimeter fails at 29,000 ft.  This plane can fly far beyond",
	"this height, and hopefully you can help us determine exactly how",
	"high that is.",
	"",
};

static void	showmaster() {
	int	i, n, y;
	extern void	boxsetup();

	setblink(0);
	color(BLACK, GRAY);
	erscrn();
	color(BRIGHTWHITE, RED);
	boxsetup(RGXMESS+2, 2, RGXWID-4, 22, 1);
	color(WHITE, RED);
	n = sizeof(secretorders)/sizeof(char *);
	y = 3;
	for (i = 0; i < n; i++) {
		cursor(RGXMESS+4, y++);
		hsput(secretorders[i], RGXWID-6);
	}
	y++;
	tkyo(RGXMESS+5, y);
}

char	*otherworlds[] = {
	"  The valiant success you have demonstrated in defending the",
	"planet is without comparison.  By all rights, clearing these",
	"aliens from our planet was our first priority.  However, we",
	"realize now that the aliens are NOT giving up this easily...",
	"",
	"  You are hereby invited to take part in the newly formed",
	"OtherWorld Alliance.  It is an elite force of top pilots who",
	"demonstrated topnotch abilities in the battle of Earth.",
	"",
        "  Join with us on our quest to rid our neighboring planets of",
	"hostile alien bases.  These are NOT just more missions.  Think of",
	"the challenge of air combat in the thick atmosphere and heavy",
	"gravity of a red giant;  Or the challenge of an iceworld with air",
	"so thin and gravity so low that they are almost non-existant!",
	"",
	"  We are planning more than 150 day and night attacks on some",
	"of the most bizzare alien bases imaginable.  If you think you've",
	"seen it all in your defense of earth, then you are dead wrong!",
	"",
#ifndef SOFSRC
	" To begin your exciting new journey, select 'R' from main menu.",
#else
	" See the Corncob Documentation for more information about this.",
#endif
};

static void	showothers() {
	int	i, n, y;
	extern void	boxsetup();

	setblink(0);
	color(BLACK, GRAY);
	erscrn();
	color(BRIGHTWHITE, RED);
	boxsetup(RGXMESS+2, 2, RGXWID-4, 22, 1);
	color(WHITE, RED);
	n = sizeof(otherworlds)/sizeof(char *);
	y = 3;
	for (i = 0; i < n; i++) {
		cursor(RGXMESS+4, y++);
		hsput(otherworlds[i], RGXWID-6);
	}
	y++;
	tkyo(RGXMESS+5, y);
}

int	generoffx, generoffy;
char	*genername;
void	showgen() {
	int	y;
	char	buf[LINELEN], buf2[LINELEN], *p;

	extern int	kbhit();
	extern void	center(), boxsetup(), ccout();
#ifndef NEWRANKS
	extern void	gendoit(), waitit();
#endif

	genflag = 0;
	setblink(0);
	color(WHITE, BLUE);
	erscrn();
	color(BRIGHTWHITE, GRAY);
	y = FLYMESS;
	boxsetup(FLXMESS, y++, FLXWID, FLYWID, 0);
	y += 2;
	ccout(y++, "!!! CONGRATULATIONS AND EXCLAMATIONS !!!");

	y++;
	color(REALYELLOW, GRAY);
#ifdef REGISTERED
	sprintf(buf, "General %s", curpilot->name);
#else
	sprintf(buf, "Major %s", curpilot->name);
#endif
	generoffy = y;
	center(buf2, FLXWID-2, buf);
	cursor(FLXMESS+1, y++);
	hsput(buf2, FLXWID-2);
	for (p = buf2; *p == ' '; p++);
	generoffx = FLXMESS+1 + (p - buf2);
	genername = buf;

	if (curpilot->nresurected) {
		color(REALYELLOW, GRAY);
		sprintf(buf2, "(With %d resurrection%s,)",
				curpilot->nresurected,
				(curpilot->nresurected == 1) ? "" : "s");
		ccout(y++, buf2);
	}
	y++;
	color(CYAN, GRAY);
	ccout(y++, "having successfully completed all the relevant");
	ccout(y++, "Theaters-of-Operation,");
	ccout(y++, "and thereby \\(OBLITERATING\\) the alien threat,");
#ifndef REGISTERED
	ccout(y++, "as well as completing the new fighter test");
	ccout(y++, "at great personal risk,");
#endif
	ccout(y++, "is hereby declared...");
	y++;
	color(REALYELLOW, GRAY);
#ifndef REGISTERED
	ccout(y++,  "MASTER of Corncob 3D");
#else
	ccout(y++,  "MASTER of Corncob Other Worlds");
#endif
	y++;
	if (curpilot->nresurected) {
		y++;
		color(WHITE, GRAY);
		ccout(y++,
			"Now, don't you want to try to do it without");
		ccout(y++,
			"resurrections?");
	}
	else {
		color(BRIGHTWHITE, GRAY);
		ccout(y++,
			"There is no greater accomplishment in Corncob 3D.");
		ccout(y++,
			"You have thus earned the utmost respect from");
#ifdef REGISTERED
		ccout(y++,
			"all who risked their lives in defense of Humankind.");
#else
		ccout(y++,
			"all who risked their lives in defense of Earth.");
#endif
	}

	color(GREEN, GRAY);
	ccout(FLYMESS+FLYWID-2, "Type almost any key to continue...");

	beep();
	beep();
	beep();
#ifndef NEWRANKS
	while (!kbhit()) {
		gendoit();
		waitit();
	}
#endif

	spgetch();
	setblink(1);
#ifdef NEWRANKS
#ifndef REGISTERED
	showothers();
#endif
#endif
}
/*#endif	/* REGISTERED */

int	ismaster(pp)
	struct pilot	*pp;
{
	return((int)pp->nmedals[CMASTER]);
}

void	ccout(y, s)
	int	y;
	char	*s;
{
	char	buf[LINELEN];
	extern void	center();

	center(buf, FLXWID-2, s);
	cursor(FLXMESS+1, y++);
	hsput(buf, FLXWID-2);
}

#endif		/* !ISCORE */
