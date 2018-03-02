/*#define	BETATIME	(694242000L)	/* Jan 1 */
#define		BETATIME	(695365200L)	/* Jan 14 */


#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<dos.h>
#include	<process.h>
#include	"moag.h"
#include	"texto.h"

char	*file3 = "3.exe";
char	*omp = "One moment please...";
char	**eargs;
INIT int	wzflag = 0;
int	reflag = 0;
#ifdef musicflag
int	musicstat=0;
char	musicwanted=-1;
int	sbexist;
int	driverloaded;
int	numrolfiles;
int	rolindex;
char	*rolfiles[MAXROLFILES];
#endif
char	version[16];

#ifdef MVPCD
	char wpath[80];
	char scrlist[80];
	char autodemoflag=0;
	extern char fileinf[80];
#endif


struct ryot	defryo = {	/* Trainee ryo stuff */
	NDBALLS, NAAA, NSCARTS,
	EDBALLS, EAAA, ESCARTS,
	0, (TRAININGVAL|COMBATVAL)
};
struct ryot	firstryo = {	/* First time ryo stuff */
	5, 6, 2,
	0, 0, 4,
	0, (TRAININGVAL|COMBATVAL)
};
INIT struct ryot	*curryo = &firstryo;
int	inity;

char	*prog;
main(argc, argv)
	char	**argv;
{
	char buf[80];
	long	now, next, far *ptime;
	unsigned long	total;
	int	y;

	extern void	interact(), come(), initscore(), savescore(),
			uglyinit(), exit(), nuke(), saybyebye(), initialize();
	extern long	time(), instdate;
#ifdef	OLDSHARE
	extern long	chksum, regrcode;
	extern void	checksum();
#endif	/* OLDSHARE */
	extern unsigned long	twoweeks;
#ifndef	__TURBOC__
	extern unsigned int	farsetsize();
#endif
	extern int	_fmode, isreg, kbhit(), firsttime;
	extern char	version[], rversion[];
	extern void	flushin();
#ifdef	MVPCD
	extern	void	godemo();
#endif
	extern	void	prepresetup();
#ifdef musicflag
	extern void musicinit();
#endif
	extern unsigned long farcoreleft();


#ifdef	MVPCD
	strcpy(wpath,".\\");
#endif

#ifdef	BETA
	sprintf(version, "BETA-%s", rversion);
#else
	strcpy(version, rversion);
#endif
	if (argc < 2 || strcmp(argv[1], "-_-") != 0) {
		printf("\n");
		printf(
		   "This program is designed to be called by `corncob.bat'.\n");
		printf(
		  "Please do not call it directly.  Instead, type `corncob'\n");
		printf("at the DOS prompt.\n");
		printf("\n");
		exit(1);
	}
//	printf("Far Ram remaining : %ld",farcoreleft());
//	mygetch();

#ifdef	__TURBOC__
	total = farcoreleft();
#else
	total = farsetsize(0xffff);
#endif
	if (total < MEMNEEDED) {
		printf("\n");
		printf(
	"Sorry, but there is too little memory available to run Corncob 3D.\n");
		printf(
#ifdef SOFSRC
	"Corncob 3D requires about 549,000 bytes available DOS RAM.\n\n");
#else
#ifdef REGISTERED
	"Corncob 3D requires about 566,000 bytes available DOS RAM.\n\n");
#else
	"Corncob 3D requires about 556,000 bytes available DOS RAM.\n\n");
#endif
#endif
		printf(
        "You may have some TSR (pop-up) programs loaded which are taking up space.\n");
		printf(
        "If you boot your computer from your original DOS floppy disk, you should\n");
		printf(
        "have enough memory to run Corncob.\n\n");
		printf(
	"Amount of memory you need to free up: %ld\n",
						MEMNEEDED-total);
		printf("\n");
		exit(137);
	}
/*
	else {
		printf("total=%u  MEM=%u\n", total, MEMNEEDED);
		mygetch();
	}
*/

	prog = argv[0];
	if (argc > 2 && strcmp(argv[2], "-i") == 0) {
		wzflag = 1;
		eargs = &argv[3];
	}
	else if (argc > 2 && strcmp(argv[2], "-r") == 0) {
		reflag = 1;
		eargs = &argv[3];
#ifdef MVPCD
	if (argc > 3 && strncmpi(argv[3], "-p",2) == 0) {
		strcpy(wpath,argv[3]+2);	/* set file writing path up */
		strcat(strcpy(fileinf,wpath),"towers.inf");
/*		printf("new path set : %s",wpath); /**/
/*		getch(); /**/
		eargs = &argv[4];
	}
	else
	if (argc > 3 && strncmpi(argv[3], "-z",2) == 0) {
		strcpy(wpath,argv[3]+2);	/* set file writing path up */
		strcat(strcpy(fileinf,wpath),"towers.inf");
		autodemoflag=-1;
/*		printf("new path set : %s",wpath); /**/
/*		getch(); /**/
		eargs = &argv[4];
	}
#endif
	}
#ifdef MVPCD
	else if (argc > 2 && strncmpi(argv[2], "-p",2) == 0) {
		strcpy(wpath,argv[2]+2);	/* set file writing path up */
		strcat(strcpy(fileinf,wpath),"towers.inf");
/*		printf("new path set : %s",wpath); /**/
/*		getch(); /**/
		eargs = &argv[3];
	}
	else if (argc > 2 && strncmpi(argv[2], "-z",2) == 0) {
		strcpy(wpath,argv[2]+2);	/* set file writing path up */
		strcat(strcpy(fileinf,wpath),"towers.inf");
		autodemoflag=-1;
/*		printf("new path set : %s",wpath); /**/
/*		getch(); /**/
		eargs = &argv[3];
	}
#endif
	else
		eargs = &argv[2];

	_fmode = O_BINARY;

	if(!reflag) {  /* only do the following checks if starting up */

/* check for running under windows, and inform user if so */
	if(windowscheck()) {
		printf("Warning!!!!\n");
		printf("Corncob 3D is not designed to run under Windows.\n\n");
		printf("Please exit Windows, and run the game from the\n");
		printf("DOS command line.\n");
		printf("If you are not running Windows, or you would like\n");
		printf("to try the game under Windows at your own risk, then\n");
		printf("type a 'g', any other key will quit : ");
	        buf[0]=0x20|mygetch();
		if(buf[0]!='g') exit(137);
	}

/* check for running on an non-VGA , and inform user if so */
	if(vgacheck()) {
		printf("Warning!!!!\n");
		printf("Corncob 3D requires a VGA color card and monitor to run.\n\n");
		printf("A VGA card has not been found in your system.");
		printf("  Err Code: %d\n",vgacheck());
		printf("If you are sure that you indeed do have a VGA system, and you\n");
		printf("would like to try the game at your own risk, then\n");
		printf("type a 'g'.  Any other key will quit : ");
	        buf[0]=0x20|mygetch();
		if(buf[0]!='g') exit(137);
	}

}

	uglyinit();

/* Initialize the sound board and load the nice drivers */

	if (reflag) {
		time(&now);
		come();
#ifdef MVPCD
		if(autodemoflag) {
		if(reflag) saybyebye();
		}
#endif
/* cut this darn stuff out! */
//		if ( ((isreg != ISREGVAL) && now - instdate > twoweeks)
#ifdef	BETA
//			|| (now > BETATIME)
#endif
//									) {
//			nuke("3d.cfg");
//			instdate = now;
//			savescore();
//			saybyebye();
//		}
prepresetup();
#ifdef musicflag
	musicinit();
#endif
	}
	else {
		inito();
#ifdef	BETA
		if (time(NULL) > BETATIME)
			saybyebye();
#endif
		/*
		ptime = (long far *)MK_FP(0x40, 0x6C);
		now = *ptime;
		next = now+100;
		 */
		flushin();
		y = 5;
#ifdef	REGISTERED
		xyprintf(1, y++,
	"Corncob Other Worlds Campaign %s", version);
#else
		xyprintf(1, y++,
	"Corncob 3D  Version %s", version);
#endif

		xyprintf(1, y++, "An MVP Software Production");


		xyprintf(1, y++,
	"Copyright 1991,1992   Pie in the Sky Software.");
#ifdef	REGISTERED
		xyprintf(1, y++,
	"This program is not in the public domain, nor is it shareware.");
#endif
		y++;
		inity = y;
		xyprintf(1, inity++, omp);
		y++;
		initscore();
#ifdef	OLDSHARE
		checksum();
#endif	/* OLDSHARE */
		initialize();
#ifdef MVPCD
		if(autodemoflag) {
		if(reflag) saybyebye();
		godemo(0);
				/* godemo doesn't return */
		}
#endif



		/*
		inity++;
		if (!firsttime) {
			xyprintf(1, inity++,
					"Type almost any key to continue...");
			while (*ptime < next)
				if (kbhit())
					break;
		}
		*/
		inity++;
		if (firsttime)
			xyprintf(1, inity++, "Loading images...");

	prepresetup();
#ifdef musicflag
	musicinit();
#endif
		buf[0]=0;	/* null string is default */
#ifdef musicflag
		if(sbexist==0) strcpy(buf,"s m");
		  else
		   if((driverloaded<0) || !musicwanted) strcpy(buf,"m");
#else
		   strcpy(buf,"m");   /* No music for in Title scrn either */
#endif
		spawnl(P_WAIT, "cctitle.exe",
			"cctitle", buf,NULL);  /* Put up title screen */
		inito();
	}

#ifdef	OLDSHARE
	if (isreg == ISREGVAL && (chksum == -1 || regrcode == -1))
		isreg = ISNOTREGVAL;
#endif	/* OLDSHARE */
	interact();
	color(BRIGHTWHITE, BLUE);
	xyprintf(31, 10, omp);
	savescore();
	saybyebye();
}

extern void	roster(), choozmsn(), rollmsn(), go(), hiscore();
#ifndef REGISTERED
extern	void	regstuff();
#endif
extern	void	kbjsset(), mohelp(), viewdoc(), quit();
#ifndef REGISTERED
#ifndef SOFSRC
extern	void printdoc();
#endif
#endif
#ifdef musicflag
extern void	musicset();
#endif
#ifdef	REGISTERED
extern void	chdemo(), sndedit();
#endif	/* REGISTERED */

static struct cmdt	choices[] = {
	"\\Duty roster",				roster,
	"\\Combat missions",				choozmsn,
	"\\Training missions",				rollmsn,
/*	"\\Fly mission",				go,		*/
	"display \\High score list",			hiscore,

#ifndef MVPCD
#ifndef SOFSRC
#ifndef REGISTERED
	"\\Read This!!!",				regstuff,
#endif
#endif
#endif

	"keyboard/\\Joystick setup",			kbjsset,
#ifdef	REGISTERED
#ifndef	MVPCD	/* no sound editor for CD users */
	"sounds \\Editor",				sndedit,
#endif
	"\\Watch a demo flight",			chdemo,
#endif	/* REGISTERED */
#ifndef MVPCD
#ifdef musicflag
#ifdef musicmenu
	"\\Music toggle",				musicset,
#endif
#endif
#endif
#ifndef SOFSRC
#ifndef REGISTERED
	"\\View  the Corncob 3D documentation",		viewdoc,
#else
	"\\View  the Corncob documentation",		viewdoc,
#endif
#ifndef REGISTERED
	"\\Print the Corncob 3D documentation",		printdoc,
#endif
#else
	"\\View the Corncob 3D documentation",		viewdoc,
#endif
	"get \\Information about this menu",		mohelp,
	"e\\Xit to dos",				quit,
};
static int	nchoices = (sizeof(choices)/sizeof(struct cmdt));

int	done;

#ifdef musicflag
#ifdef musicmenu
void musicset()
{
	char buf[80];
	extern void endsong();
	if(musicwanted) {
          if((musicstat&2)!=0)
	   if((musicstat &1) != 0) {
		endsong();
		musicstat=0;
	   }
	}
	else
	  if((sbexist==-1) && (driverloaded>=0)) { musicstat=2;
	  }
	  else { 
		sprintf(buf,"Err: Sound Card not found. Sbe=%d Drl=%d"
				,sbexist,driverloaded);
		errorbox(buf); return; }

	musicwanted ^= -1;
}
#endif
#endif

#define	DEFCHOICE	2
INIT int	choice = DEFCHOICE;




void	interact() {
	struct cmdt	*ch;
	int far	*scr;
	char buf[80];
	extern void	outchoice(), showchoices(),
			presetup(), mainsetup(), firsttimestuff();
#ifdef	BETA
	extern void	saybyebye();
#endif
	extern int	firsttime;
#ifdef	BETA
	extern long	time();
#endif
#ifdef musicflag
	extern int startsong();
	extern int BoardInstalled();
#endif
	extern struct cmdt	*getchoice();

	presetup();


	if (!reflag) {
		color(WHITE, BLACK);
		erscrn();
	}
	mainsetup();
	scr = yankscr();
	if (!reflag) {
		color(BRIGHTWHITE, BLUE);
		xyprintf(31, 10, omp);
/*		initialize(); */
		putscr(scr);
		if (firsttime) {
			firsttimestuff(scr);
		}
	}
#ifdef	BETA
	if (time(NULL) > BETATIME)
		saybyebye();
#endif

	while (!done) {
		showchoices();
		ch = getchoice(choices, &choice, nchoices, outchoice,
							(int (*)())NULL);
		(*ch->ccmd)();
		putscr(scr);
	}
	freescr(scr);
}

static void	showchoices() {
	int	i, y;
	extern void	outchoice();
	extern struct pilot	*curpilot;
	extern struct thnamlist	*getthnam();
	extern int	kbflag, njflag;

	color(LIGHTCYAN, BLUE);
	cursor(MXMESS+10, MYMESS+5);
	sput("Use arrow keys or type highlighted letter to choose action:");
	fcolor(WHITE);
	for (i = 0; i < nchoices; i++)
		outchoice(i);

	y = MYSTUFF+1;
	cursor(MXSTUFF, y++);
	color(YELLOW, BLUE);
	sput("Active pilot:  ");
	color(WHITE, BLUE);
	foutpname(curpilot);

	cursor(MXSTUFF, y++);
	color(YELLOW, BLUE);
	sput("Theater-of-Operations:  ");
	color(WHITE, BLUE);
	if (curpilot->curtheater&0x80) {
		sput("Training Mission ");
		color(YELLOW, BLUE);
	}
	else {
		sput(getthnam(curpilot->curtheater)->name);
	}

	cursor(MXSTUFF, y);
	color(YELLOW, BLUE);
	sput("Control:  ");
	color(WHITE, BLUE);
	if (kbflag)
		sput("Keyboard");
	else {
		sput("Joystick  (");
		if (njflag)
			sput("manual");
		else
			sput("automatic");
		sput(" recentering)");
	}

#ifdef musicflag
#ifdef musicmenu
	cursor(MXSTUFF+44, y);
	color(YELLOW, BLUE);
	sput("Music ");
	color(WHITE, BLUE);
	if (musicwanted) {
		color(LIGHTCYAN,BLUE);
		sput("On           ");
          if((musicstat &2)==0) { 
		color( BLACK,WHITE); 
		sput("No Sound Card"); 
		color(YELLOW, BLUE);
		}
	}
	else {
		sput("Off         ");
	}

#endif
#endif

	color(WHITE, BLUE);

}

static void	outchoice(i) {
#ifdef SOFSRC
	cursor(MXMESS+27, i+(MYMESS+9));
#else
	cursor(MXMESS+27, i+(MYMESS+7));
#endif
	hsput(choices[i].scmd, LINELEN);
}

static void	assdummy() {}
static struct cmdt	asscdummy = {
	"", assdummy
};

struct cmdt	*getchoice(menu, num, nnum, outline, assnum)
	struct cmdt	*menu;
	int	*num, nnum;
	void	(*outline)();
	int	(*assnum)();
{
	int	newnum, i, c, ac, lc;
	char	*p;

	bold();
	(*outline)(*num);
	ac = 0;
	while (1) {
		if (ac) {
			c = ac;
			ac = 0;
		}
		else
			c = spgetch();
		if (c == '\n') {
			normal();
			return(&menu[*num]);
		}
		if (c == 010 || c == 0177 || c == 033) {
			if (menu == choices && c == 010) {
				beep();
				continue;
			}
			*num = nnum - 1;
			normal();
			return(&menu[*num]);
		}
		else if ((lc = (c|040)) >= 'a' && lc <= 'z') {
			for (i = 0; i < nnum; i++) {
				for (p = menu[i].scmd; *p; p++) {
					if (*p == '\\' && (*(p+1)|040) == lc) {
						normal();
						(*outline)(*num);
						*num = i;
						bold();
						(*outline)(*num);
						normal();
						return(&menu[*num]);
					}
				}
			}
			beep();
			continue;
		}
		else if (assnum != NULL && c >= '0' && c <= '9') {
			ac = (*assnum)(*num, c);
			if (ac) {
				newnum = *num;
			}
			else {
				bold();
				(*outline)(*num);
				return(&asscdummy);
			}
		}
		else if (c == UPARROW || c == LEFTARROW) {
			if (*num > 0)
				newnum = *num - 1;
			else
				newnum = nnum - 1;
		}
		else if (c == DOWNARROW || c == RIGHTARROW) {
			if (*num < nnum-1)
				newnum = *num + 1;
			else
				newnum = 0;
		}
		else if (c == 0) {
			newnum = nnum-1;
		}
		else {
			beep();
			continue;
		}
		normal();
		(*outline)(*num);
		*num = newnum;
		bold();
		(*outline)(*num);
	}
#ifndef	__TURBOC__
	return(NULL);
#endif
}

static void	mohelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Information about Main Menu", GREEN, RED, WHITE);
	fileout("moag.inf", 1, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								WHITE, RED, 4);
}

static void	firsttimestuff(scr)
	int far	*scr;
{
	char	*phead, buf[LINELEN], *p1, *p2;
	int	c, x, y, w;
	long	now, next, far *ptime;
	extern int	kbhit(), fileout(), kbflag;
	extern void	putin(), flushin();

	phead = "First-time Information";
	simplesetup(RGXMESS, RGYMESS+3, RGXWID, 11, phead, BLUE, CYAN, BLUE);

	blink();
	putin();
	normal();

	xyprintf(RGXMESS+2, RGYMESS+8,
	"You may view it at any time later by choosing the");
	cursor(RGXMESS+2, RGYMESS+9);
	hsput(
   "\"\\View the \\(Corncob 3D\\) documentation\" selection in the main menu.",
   								RGXWID);
	xyprintf(RGXMESS+3, RGYMESS+12, "Type almost any key when ready...");

	ptime = (long far *)MK_FP(0x40, 0x6C);
	now = *ptime;
	next = now+20;
	flushin();
	while (*ptime < next)
		if (kbhit())
			break;
	color(BRIGHTWHITE, CYAN);
	putin();
	now = *ptime;
	next = now+600;
	while (*ptime < next)
		if (kbhit())
			break;
	flushin();
	simplesetup(DRXMESS, DRYMESS, DRXWID-1, DRYWID-1,
			"Information about Main Menu", BLUE, CYAN, BLUE);
	fileout("corncob.doc", 1, DRXMESS+2, DRYMESS+1, DRXWID-4, DRYWID-3,
							BRIGHTWHITE, CYAN, 4);

	putscr(scr);
	w = 50;
	x = (80 - w)/2;
	y = 7;
	simplesetup(x, y++, w, 11,
			"Configure for Joystick", BRIGHTWHITE, CYAN, YELLOW);
	y++;
	x += 2;
	w -= 4;
	xyprintf(x, y++, "Do you have a Joystick? (Y/N) ");
	color(BRIGHTWHITE, CYAN);
	p1 = "Program will now default to %s control.";
	p2 = "You may choose %s control at any time";
	while (1) {
	c = spgetch();
		if (c == 'Y' || c == 'y') {
			cput('Y');
			y++;
			color(BLUE, CYAN);
			xyprintf(x, y++, p1, "joystick");
			xyprintf(x, y++, p2, "keyboard");
			kbflag = 0;
			break;
		}
		else if (c == 'n' || c == 'n' || c == 033) {
			cput('N');
			y++;
			color(BLUE, CYAN);
			xyprintf(x, y++, p1, "keyboard");
			xyprintf(x, y++, p2, "joystick");
			kbflag = 1;
			break;
		}
		else
			beep();
	}
	sprintf(buf, "from the Main Menu via the \\(keyboard/\\Joystick\\)");
	cursor(x, y++);
	hsput(buf, w);
	xyprintf(x, y++, "selection.");

	y++;
	color(BRIGHTWHITE, CYAN);
	sleep(1);
	flushin();
	xyprintf(x+1, y, "Type <CR> to continue...");
	while ((c = spgetch()) != '\n' && c != 033);
	putscr(scr);
}

static void	putin() {
	xyprintf(RGXMESS+2, RGYMESS+5,
	"The following information will only appear this first time that you");
	xyprintf(RGXMESS+2, RGYMESS+6,
	"run Corncob 3D.");
}

#ifndef REGISTERED
#ifndef SOFSRC
static void printdoc() {
	int far *scr;
	int x,y;
	int	c, i;
	int deedisdone;
	char	*p1, *p, *pmax;
	static char	buf[LINELEN];
	FILE *fp1;
	FILE *fp0;



	x=RGXMESS+10+2;
	y=RGYMESS+1;
	deedisdone=0;
	scr = yankscr();

	while(deedisdone==0) {
	simplesetup(RGXMESS+10, RGYMESS, RGXWID-20, RGYWID-10,
			"Print the Documentation", BRIGHTWHITE, CYAN, YELLOW);
	xyprintf(x, y++,
	"If your printer is on LPT1, just hit <enter>");
	xyprintf(x, y++,
	"Or type the device name of your printer");
	xyprintf(x, y++,
	"I.E. lpt2, com1, com2    Esc cancels.");

	cursor(x+1, y++);
	p1 = buf;
	pmax = &p1[DRPWID];
	p = p1;
	strcpy(p,"lpt1");
	p += 4;
	xyprintf(x+2, y, "lpt1");

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
	deedisdone=1;
	if(*p1 != '\0') {
	xyprintf(x, y, "Now printing on %s",p1);
          if ((fp1 = fopen(buf, "w")) == NULL) {
                    xyprintf(x,y,"Device %s not found",p1);
		    deedisdone=0;	/* signify another loop */ 
		}
		else {
          	   if ((fp0 = fopen("corncob.doc", "r")) == NULL) {
			errorbox("Corncob.doc file missing!");
	           	}
		   else
			{
				do {
				  i=fgetc(fp0);
				  fputc(i,fp1);
				  if(kbhit()) if(mygetch()==0x1b) break;
				}
				while(i != EOF);
				fclose(fp0);
				fclose(fp1);
			}
		}
	}

	}  /* while deedisdone */
	putscr(scr);
}
#endif
#endif

static void	viewdoc() {
	int	c, n;
	extern int	fileout();
	extern void	boxsetup();

	setblink(0);
	color(LIGHTBLUE, GRAY);
	boxsetup(1, 1, 80, 25, 0);
	color(WHITE, GRAY);
	n = 0;
	while (1) {
		if (n == 0) {
			c = fileout("corncob.doc", n, 2, 2, 78,23,WHITE,GRAY,5);
			if (c == 0)
				break;
			else
				n = c;
		}
		else {
			c = fileout("corncob.doc", n, 2, 2, 78,23,WHITE,GRAY,6);
			if (c == 'b')
				n = 0;
			else if (c)
				n++;
			else
				break;
		}
	}
}

void	quit() {
	done = 1;
}

#include	<stdarg.h>
/*VARARGS0*/
void	errorbox(cntrl)
	char	*cntrl;
{
	va_list	args;
	int far	*scr;
	char	buf[256];
	extern int	vsprintf();

	scr = yankscr();
	savepos();
	beep();
	simplesetup(10, 9, 60, 6, "Unexpected Error", LIGHTRED, WHITE, RED);

	va_start(args, cntrl);
	vsprintf(buf, cntrl, args);
	va_end(args);
	cursor(12, 10);
	sput(buf);

	if(buf[0]!='!') tkyo(18, 13);
	 else getch();
//	tkyo(18, 13);
//	getch();
	putscr(scr);
	freescr(scr);
	restpos();
}

void	saybyebye() {
	char	buf[LINELEN];
#ifndef	REGISTERED
	char	tbuf[16];
	char	*pr, *pn;
	long	now;
#else
	unsigned char	mask, *p, *pm;
	int	i;
#endif
#ifdef	BETA
	char	*pp;
#endif
	int	y, n;

	extern void	ncexito(), cout(), center(), boxsetup(), exit();
	extern char	version[];
#ifdef	REGISTERED
	extern unsigned char	regnames[][REGLINELEN];
	extern char	regserialno[];
	extern int	nregnames, regseed, rand();
#ifdef	__TURBOC__
	extern void	srand();
#else
	extern long	_seed;
#endif	/* !__TURBOC__ */
#else	/* !REGISTERED */
	extern int	isreg;
#ifdef	OLDSHARE
	extern unsigned long	oneweek;
	extern long	chksum, instdate, time();
#endif	/* OLDSHARE */
#endif	/* !REGISTERED */

#ifdef musicflag
#ifdef musicmenu
	extern void endsong();
#endif
#endif

#ifdef	REGISTERED
#ifdef	__TURBOC__
	srand(regseed);
#else
	_seed = (long)regseed;
#endif
	for (i = 0; i < nregnames; i++) {
		pm = &regnames[i][REGLINELEN];
		for (p = regnames[i]; p < pm; p++) {
			mask = (unsigned char)(rand()/128);		/**/
			*p = (unsigned char)(*p^mask);
		}
		regnames[i][39] = '\0';
	}
#endif	/* REGISTERED */
	setblink(1);

#ifndef NOEXITSCRN
	color(WHITE, BROWN);
	erscrn();
	color(YELLOW, BLUE); 
	y = 2;
	boxsetup(4, y++, 72, 20, 1);
	color(BRIGHTWHITE, BLUE);
#else
	color(WHITE, BLACK);
#ifndef	SOFSRC
	erscrn();
#endif
#endif
#ifdef	BETA
	if (isreg == ISREGVAL)
		y += 2;
#else	/* !BETA */
#ifdef	REGISTERED
	y += 0;
#else	/* !REGISTERED */
#ifdef	OLDSHARE
	if (isreg == ISREGVAL) {
		y += 4;
		color(YELLOW, BLUE);
	}
	else {
		y++;
	}
#else	/* !OLDSHARE */
	y += 0;
#endif	/* !OLDSHARE */
#endif	/* !REGISTERED */
#endif	/* !BETA */
#ifndef	NOEXITSCRN
	color(YELLOW, BLUE);
	cout(y++, "Thank you for playing Corncob 3D");
	y++;
	color(LIGHTCYAN, BLUE);
	cout(y++, "This game is copyright (C) 1991,1992 by Kevin Stokes");
	sprintf(buf, "and Pie in the Sky Software.  Ver. %s", version);
	cout(y++, buf);
#endif

#ifdef	BETA
	pr = "This is a%sregistered BETA release of Corncob-3d";
	pn = "Your registration number is ";
	pp = "This software may not be distributed or copied.";
	color(BRIGHTWHITE, BLUE);
	y++;
	if (isreg == ISREGVAL) {
		y++;
		sprintf(buf, pr, " ");
		cout(y++, buf);
		color(YELLOW, BLUE);
		cout(y++, pp);
		color(BRIGHTWHITE, BLUE);
		sprintf(buf, "%s%ld.", pn, chksum);
		cout(y++, buf);
		y++;
	}
	else {
		sprintf(buf, pr, "n un");
		cout(y++, buf);
		color(YELLOW, BLUE);
		cout(y++, pp);
		color(BRIGHTWHITE, BLUE);
		time(&now);
		now -= instdate;
		if (now < 0)
			now = 0;
		if (now >= oneweek)
			cout(y++, "Your trial period has expired.");
		else {
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
			cout(y++, buf);
		}
		if (chksum >= 0) {
			sprintf(buf, "%s%ld.", pn, chksum);
			cout(y++, buf);
		}
		else {
			sprintf(buf, "%s%s. (Play the game!)", pn,
							"not yet available");
			cout(y++, buf);
		}
		color(WHITE, BLUE);
		y++;
		cout(y++,
	     "Please read the file \"readme\" for instructions on recieving");
		cout(y++,
	     "a possible free registration.");
		y++;
	}
	color(YELLOW, BLUE);
	cout(y++,
	  "This Beta version of CORNCOB-3d will only work for a limited time.");
	y++;
#else	/* !BETA */
#ifdef	REGISTERED
#ifndef	NOEXITSCRN
	color(YELLOW, BLUE);
	y++;
	cout(y++,
	    "This program is not in the public domain, nor is it shareware.");
	cout(y++,
	    "Use of this program by persons other than those listed below");
	cout(y++,
	    "is a violation of the copyright.");
	color(BRIGHTWHITE, BLUE);
	cout(y++, "Registered to:");
	color(WHITE, BLUE);
	for (n = 0; n < nregnames; n++)
		xyprintf(35, y++, regnames[n]);
	y++;
	xyprintf(35, y++, "Serial number:  %s", regserialno);

#endif /* NOEXITSCRN */
#else	/* !REGISTERED */
#ifdef	OLDSHARE
	pr = "This shareware program is %sregistered.";
	pn = "Your registration number is ";
	color(BRIGHTWHITE, BLUE);
	y++;
	if (isreg == ISREGVAL) {
		sprintf(buf, pr, "");
		cout(y++, buf);
		sprintf(buf, "%s%ld.", pn, chksum);
		cout(y++, buf);
	}
	else {
		sprintf(buf, pr, "not ");
		cout(y++, buf);
		time(&now);
		now -= instdate;
		if (now < 0)
			now = 0;
		if (now >= oneweek)
			cout(y++, "Your trial period has expired.");
		else {
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
			cout(y++, buf);
		}
		if (chksum >= 0) {
			sprintf(buf, "%s%ld.", pn, chksum);
			cout(y++, buf);
		}
		else {
			sprintf(buf, "%s%s. (Play the game!)", pn,
							"not yet available");
			cout(y++, buf);
		}
		y += 2;
		color(YELLOW, BLUE);
		cout(y++, "Register CORNCOB-3D for as little as $15"); 
		cout(y++,
		      "Please see the registration menu for more information.");
		y++;
		color(WHITE, BLUE);
	}
#else	/* !OLDSHARE */
#ifndef	NOEXITSCRN
	color(WHITE, BLUE);
	y++;
//	cout(y++, "This program is shareware; distribute freely without modification.");
	y++;
	color(YELLOW, BLUE);
	cout(y++, "Get the Deluxe version of CORNCOB-3D for only $15!"); 
	color(BRIGHTWHITE, BLUE);
	cout(y++,
	"Hear the low thruming of generator orbs and the hair-raising shriek");
	cout(y++,
	"of approaching KLA's with CC Deluxe's amazing ADLIB(tm) sounds.");
	cout(y++,
	"Corncob Deluxe also includes a built-in mission builder, inflight");
	cout(y++,
	"score reporting, inflight theater analysis, a sound editor, hundreds");
	cout(y++,"of exciting new missions, and many other features!");
	y++;
	color(YELLOW, BLUE);
	cout(y++, "Please see the registration menu for more information.");
#endif /* NOEXITSCRN */
#endif	/* !OLDSHARE */
#endif	/* !REGISTERED */
#endif	/* !BETA */
#ifndef	NOEXITSCRN
	color(WHITE, BLUE);
	y++;
	cout(y++,
	"To get the latest version of CORNCOB-3D, or technical assistance");
	cout(y++,
	"call our Bulletin Board (2400,N,8,1) at (919)-489-6900 (24 hrs.).");

	color(WHITE, BLACK);
	cursor(1, 24);
#endif	/* !MVPCD */
	center(buf, 80, " ");
	sput(buf);
	cursor(1, 25);
	sput(buf);
#ifdef musicflag

#ifdef musicmenu
	if((musicstat &1) != 0)
	{
	  musicstat=0;
	  endsong();
	}
	if(!driverloaded) {
	  SoundCall(5,0);	/* turn off music */
	  if(unloadsound()) errorbox("error unloading driver");
	}
#else		/* if no menu music, just unload driver */
	if(!driverloaded) {
	  if(unloadsound()) errorbox("error unloading driver");
	}
#endif
#endif
	ncexito(24);
#ifdef musicdiag
	printf("\n There are %d songs in file",numrolfiles);
	for(i=0;i<numrolfiles;i++)
	printf("\n%s",rolfiles[i]);
#endif
#ifndef REGISTERED
#ifndef SOFSRC
		spawnl(P_WAIT, "cc3d.com",
			"cctitle", buf,NULL);  /* Put up title screen */
#endif
#endif
	exit(137);
}

void	cout(y, s)
	int	y;
	char	*s;
{
	char	buf[LINELEN];
	extern void	center();

	center(buf, 70, s);
	cursor(5, y++);
	sput(buf);
}

int	windowscheck()
{
	union REGS	regs;

	regs.h.ah = 0x16;
	regs.h.al = 0;
	int86(0x2f, &regs, &regs);
	if((regs.h.al==0) || (regs.h.al==0x80)) return(0);
	  else return(-1);
}

int	vgacheck()
{
	union REGS	regs;

	regs.h.ah = 0x1a;
	regs.h.al = 0;
	int86(0x10, &regs, &regs);
	if(regs.h.al != 0x1a) return(-1);  /* if bios doesn't support funct */
	if(regs.h.bl==0x08) return(0);
	  else return((int) regs.h.bl);
}

#ifdef musicflag
void musicinit()
{
#ifdef musicmenu
	extern void openmusic();
#endif
	extern int	firsttime;
	char buf[LINELEN];
#ifdef musicmenu
	openmusic();
#endif
	sbexist = 0;
	musicstat = 0;
	rolindex = 0;
	driverloaded=-1;
	  if(BoardInstalled()) { 
	    musicstat = 2;
	    sbexist=-1;
	     if(GetSoundDrvVersion())
	       driverloaded=1;
	      else
	       {
	       driverloaded=loadsound();
	       if(driverloaded != 0) {
		   sprintf(buf,"Can't open music system.  Err: %d",
			driverloaded);
		   errorbox(buf);
	           }
	       }
	  }
	  else /* !Boardinstalled */
	    {
	      if(firsttime) musicwanted=0;
 	    }
	if(sbexist) {
	  if(!GetSoundDrvVersion()) {
		musicstat = 0;
		errorbox("Can't find music system");
	  } 
	}
	if(!musicwanted) musicstat=0;
}
#endif

