#include	<stdio.h>
#include	"moag.h"
#include	"texto.h"

INIT int	kbflag = 0;	/* 1 for keyboard, 0 for joystick */
INIT int	jsflag = 0; 	/* force joystick recenter */
INIT int	njflag = 0;	/* disable automatic joystick recentering */

/*
 **************** Keyboard/Joystick setup menu (level 2)
 */

extern void	kbcntrl(), jscntrl(), autojs(), manjs(), forcejs(),
		kjhelp(), quit();
static struct cmdt	choices[] = {
	"choose \\Keyboard control",				kbcntrl,
	"choose \\Joystick control",				jscntrl,
	"choose \\Automatic joystick centering",		autojs,
	"choose \\Manual joystick centering",			manjs,
	"\\Force joystick re-centering next flight",		forcejs,
	"get \\Information about this menu",			kjhelp,
	"\\Return to previous menu",				quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

#define	DEFCHOICE	0
INIT int	kjchoice;
void	kbjsset() {
	struct cmdt	*ch;
	int far	*scr;
	extern void	outchoice(), showchoices();
	extern struct cmdt	*getchoice();

	kjchoice = DEFCHOICE;
	simplesetup(KJXMESS, KJYMESS, KJXWID, KJYWID, "Keyboard/joystick setup",
							BROWN, CYAN, BLUE);
	scr = yankscr();
	showchoices();
	while ((ch =
	        getchoice(choices, &kjchoice, nchoices, outchoice,
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
	cursor(KJXMESS+10, KJYMESS+1);
	sput("Use arrow keys or type highlighted");
	cursor(KJXMESS+10, KJYMESS+2);
	sput("  letter to choose action:");

	for (i = 0; i < nchoices; i++)
		outchoice(i);

	i = KJYMESS+KJYWID-4;
	color(YELLOW, CYAN);
	xyprintf(KJXMESS+2, i,    "Current");
	xyprintf(KJXMESS+2, i+1,  " choices:");
	color(BRIGHTWHITE, CYAN);
	cursor(KJXMESS+14, i++);
	cput(254);
	if (kbflag)
		sput("Keyboard");
	else
		sput("Joystick");
	sput(" control.");

	if (!kbflag) {
		cursor(KJXMESS+14, i++);
		cput(254);
		if (njflag)
			sput("Manual");
		else
			sput("Automatic");
		sput(" joystick recentering.");

		if (jsflag) {
			cursor(KJXMESS+14, i++);
			cput(254);
			sput("Forcing joystick recenter next flight.");
		}
	}
	color(BLUE, CYAN);
}

static void	outchoice(i) {
	cursor(KJXMESS+10, i+(KJYMESS+4));
	hsput(choices[i].scmd, LINELEN);
}

static void	kbcntrl() {
	kbflag = 1;
	jsflag = 0;
}

static void	jscntrl() {
	kbflag = 0;
}

static void	forcejs() {
	jsflag = 1;
}

static void	autojs() {
	njflag = 0;
}

static void	manjs() {
	njflag = 1;
}

static void	kjhelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
		"Keyboard/Joystick Menu Information", GREEN, RED, WHITE);
	fileout("moag.inf", 5, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
}
