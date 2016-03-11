#include	<stdio.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"

#ifdef	REGISTERED
INIT int	isreg = ISREGVAL;	/* Is program registered flag */
#else	/* !REGISTERED */
INIT int	isreg = ISNOTREGVAL;	/* Is program registered flag */
#endif	/* !REGISTERED */

/*
 *	These are no longer used, but must be kept for init to still work
 */
INIT long	chksum = -1;		/* Rom check-sum (from 3) */
INIT long	regrcode = -1;		/* registration reply code (from PIE) */

#define	MAXRCODE	((1L<<24)-1)

#ifdef	REGISTERED
#ifdef neverintheuniverse
INIT int	regchoice;	/* This is a dummy, only to be compatible with
			 	 *  nonregistered.
				 */
void	regstuff() {
	int	y, i;
	unsigned char	buf[REGLINELEN], *p1, *p2, *pm, mask;

	extern int	tkyo();
	extern char	version[];
	extern unsigned char	regnames[][REGLINELEN];
	extern char	regserialno[];
	extern int	nregnames, regseed, rand();
#ifdef	__TURBOC__
	extern void	srand();
#else
	extern long	_seed;
#endif	/* !__TURBOC__ */

	y = RGYMESS;
	simplesetup(RGXMESS, y++, RGXWID, RGYWID, "Registration Status",
						BROWN, CYAN, BRIGHTWHITE);
	y += 3;
	xyprintf(RGXMESS+5, y++,
	   "Corncob Deluxe version %s", version);
	xyprintf(RGXMESS+5, y++,
	   "Copyright (C) 1991 by Kevin Stokes and Pie-in-the-Sky software.");
	y++;
	color(BLUE, CYAN);
	xyprintf(RGXMESS+10, y++,
		"This program is registered to:");
	color(YELLOW, CYAN);
#ifdef	__TURBOC__
	srand(regseed);
#else
	_seed = (long)regseed;
#endif
	for (i = 0; i < nregnames; i++) {
		pm = &regnames[i][REGLINELEN];
		for (p1 = regnames[i], p2 = buf; p1 < pm; p1++, p2++) {
			mask = (unsigned char)(rand()/128);		/**/
			*p2 = (unsigned char)(*p1^mask);
		}
		xyprintf(RGXMESS+20, y++, buf);
	}
	y++;
	xyprintf(RGXMESS+20, y++, "Serial number:  %s", regserialno);

	color(BLUE, CYAN);
	tkyo(RGXMESS+3, RGYMESS+RGYWID-2);
}
#endif /* neverintheunvierse */
#else	/* !REGISTERED */

#ifndef	OLDSHARE

void	regstuff() {
	extern int	fileout();
	simplesetup(1, 1, 80, 25,
			"Registration Information", GREEN, RED, WHITE);
	fileout("moag.inf", 9, 2, 2, 78, 23,
								WHITE, RED, 4);
/*
	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Registration Information", GREEN, RED, WHITE);
	fileout("moag.inf", 9, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
*/
}

#else	/* OLDSHARE */

/*
 **************** Registration menu (level 2)
 */

extern void	eregno(), reghelp(), unreg(), quit();
static struct cmdt	choices[] = {
	"get registration \\Information",			reghelp,
	"\\Enter registration reply code",			eregno,
	"\\Unregister this shareware version",			unreg,
	"\\Return to previous menu",				quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	regchoice;
void	regstuff() {
	struct cmdt	*ch;
	int far	*scr;
	extern void	outchoice(), showchoices();
	extern struct cmdt	*getchoice();

	regchoice = DEFCHOICE;
	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID, "Registration Menu",
							BROWN, CYAN, BLUE);
	scr = yankscr();
	showchoices();
	color(BLUE, CYAN);
	while ((ch =
	        getchoice(choices, &regchoice, nchoices, outchoice,
				(int (*)())NULL))->ccmd != (void (*)())quit) {
		(*ch->ccmd)();
		putscr(scr);
		showchoices();
		color(BLUE, CYAN);
	}
	freescr(scr);
}

static void	showchoices() {
	int	i, y;
	char	buf[LINELEN];
	extern void	outchoice();

	color(BLUE, CYAN);
	cursor(RGXMESS+20, RGYMESS+1);
	sput("Use arrow keys or type highlighted");
	cursor(RGXMESS+22, RGYMESS+2);
	sput("  letter to choose action:");

	for (i = 0; i < nchoices; i++)
		outchoice(i);

	if (chksum < 0)
		y = RGYMESS+11;
	else
		y = RGYMESS+12;
	cursor(RGXMESS+15, y++);
	color(BLUE, CYAN);
	sput("Status:  ");
	color(YELLOW, CYAN);
	sput("Program is ");
	if (isreg == ISNOTREGVAL)
		sput("not ");
	sput("registered.");
	if (isreg != ISREGVAL) {
		long	now;
		extern long	time(), instdate;
		extern unsigned long	oneweek;

		color(BRIGHTWHITE, CYAN);
		cursor(RGXMESS+24, y++);
		time(&now);
		now -= instdate;
		if (now < 0)
			now = 0;
		if (now > oneweek) {
			sput("Your trial period has expired.");
		}
		else {
			char	tbuf[16], *pr;
			int	n;

			tbuf[0] = '0';
			sprintf(&tbuf[1], "%ld", (oneweek - now)/8640L);
			n = strlen(tbuf);
			tbuf[n+1] = '\0';
			tbuf[n] = tbuf[n-1];
			tbuf[n-1] = '.';
			pr ="There are %s days remaining in your trial period.";
			if (n == 2)
				sprintf(buf, pr, &tbuf[0]);
			else
				sprintf(buf, pr, &tbuf[1]);
			sput(buf);
		}
	}

	cursor(RGXMESS+15, y++);
	color(BLUE, CYAN);
	sput("Registration Number:  ");
	color(YELLOW, CYAN);
	if (chksum < 0) {
		sput("not yet available.");
		color(BRIGHTWHITE, CYAN);
		cursor(RGXMESS+15+22, y++);
		sput("You must fly at least one mission");
		cursor(RGXMESS+15+22, y++);
		sput("to obtain a registration number.");
	}
	else {
		sprintf(buf, "%ld", chksum);
		sput(buf);
	}

	cursor(RGXMESS+15, y++);
	color(BLUE, CYAN);
	sput("Reply Code (from Pie-in-the-Sky):  ");
	color(YELLOW, CYAN);
	if (regrcode < 0)
		sput("not yet entered.");
	else {
		sprintf(buf, "%ld", regrcode);
		sput(buf);
	}
}

static void	outchoice(i) {
	cursor(RGXMESS+24, i+(RGYMESS+4));
	hsput(choices[i].scmd, LINELEN);
}

static void	eregno() {
	long	l;
	int	y;
	char	*p;
	extern char	*nget();

	y = RGYMESS+5;
	simplesetup(RGXMESS+15, y++, 45, 8,
			"Enter Reply Code", GREEN, RED, WHITE);
	xyprintf(RGXMESS+17, y++,
		"Please enter the reply code that");
	xyprintf(RGXMESS+17, y++,
		"you received from Pie-in-the-Sky software:");
	cursor(RGXMESS+18, y++);
	p = nget(9, '\0');
	if (p == NULL || *p == '\0')
		return;
	if (sscanf(p, "%ld", &l) != 1 || l < 0 || l > MAXRCODE) {
		y++;
		beep();
		xyprintf(RGXMESS+17, y++, "Sorry, that is a bad code.");
		tkyo(RGXMESS+19, y);
		return;
	}
	regrcode = l;
}

static void	unreg() {
	int	y, c;

	y = RGYMESS+7;
	simplesetup(10, y++, 60, 8,
			"Unregister This Program", GREEN, RED, WHITE);
	xyprintf(12, y++,
		"Warning!  This selection will unregister this shareware");
	xyprintf(12, y++,
		"copy of Corncob-3d.  You should only do this if you are");
	xyprintf(12, y++,
		"giving the copy to a friend, or if you feel you have");
	xyprintf(12, y++,
		"entered the registration reply code incorrectly.");
	y++;
	xyprintf(12, y++,
		" Type y to confirm... (Anything else cancels)");
	c = spgetch();
	if (c == 'y' || c == 'Y') {
		isreg = ISNOTREGVAL;
		regrcode = -1;
		chksum = -1;
	}
}

static void	reghelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Registration Menu Information", GREEN, RED, WHITE);
	fileout("moag.inf", 4, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
}

#endif	/* OLDSHARE */
#endif	/* !REGISTERED */
