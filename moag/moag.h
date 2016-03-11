extern char	*prog;
extern char	*file3;

#define	MINREQTH	3	/* Min number of required theaters */

#define	SN	24	/* Saved-pilots-file serial number */
#define	ISREGVAL	10973	/* isreg == this when prog is registered */
#define	ISNOTREGVAL	7315	/* isreg == this when prog is not registered */
#define	TIMEFLAGVAL	13702	/* isreg == this when system clock set back */
#define	TIMEFLAGNOTVAL	6039	/* isreg == this when system clock not back */

#define	NWICK	8	/* Number of Wickednesses */
/*
 *	This is the structure written by 3 after a mission
 */
struct stuffrm3 {
	unsigned int	flag;		/* What happened */
	int	deathcode;		/* How did he meet his fate */
	int	planeslost;		/* Number of planes lost */
	int	tickskilled;		/* Number of ticks killed */
	int	dbkilled[NWICK];	/* Number of death-balls killed */
	int	aaakilled[NWICK];	/* Number of aaa-sites killed */
	int	shckilled[NWICK];	/* Number of shopping-carts killed */
	int	siteskilled[NWICK];	/* Number of sites killed */
	int	objectives;		/* Number of objectives accomplished */
	int	friendkilled;		/* Number of friendlies killed */
	int	totaldamage;		/* Total plane damage */
	int	dbonme[NWICK];		/* Number of death-balls on me */
	int	aaaonme[NWICK];		/* Number of aaa-sites on me */
	long	seconds;		/* Seconds in the air */
	int	orbskilled[NWICK];	/* Number of orbs killed */
};
#define	NEVERLEFT	0x1
#define	KILLED		0x2
#define	LANDEDHOME	0x4
#define	CAPTURED	0x8
#define	ATHOME		0x10
#define	EJECTED		0x20
#define	CRASHNOTHOME	0x40
#define	HITBLDG		0x80
#define	RESCUED		0x100
#define	CRASHATHOME	0x200
#define	LOSTINSPACE	0x400
#define	KILLEDBYENEMY	0x800
#define	INVULNERABLE	0x2000
#define	ABORT		0x4000
#define SUICIDE		0x8000

/*
 *	This is the structure for a single pilot-mission
 */
struct pilotmiss {
	struct stuffrm3	st3;
	long	score;			/* The score */
};

/*
 *	This is the "roll your own mission" parameters
 */
struct ryot {
	char	ndballs;	/* Number of death balls */
	char	naaa;		/* Number of aaa sites */
	char	nscarts;	/* Number of shopping cart sites */
	char	edballs;	/* wickedness of death balls */
	char	eaaa;		/* wickedness of aaa */
	char	escarts;	/* wickedness of shopping carts */
	char	actflag;	/* start at action flag */
	unsigned char	invflag;	/* invulnerable plane status */
};
/*
 *	Bits on invflag:
 */
#define	INVVAL			0x1
#define	TRAININGVAL		0x2
#define	TRAININGSELECTED	0x4
#define	COMBATVAL		0x8
#define	COMBATSELECTED		0x10
#define FLTMODVAL		0x20
#define RMSGVAL		0x40

#define	NDBALLS		5	/* default number of deathball sites */
#define	NAAA		6	/* default number of AAA sites */
#define	NSCARTS		2	/* default number of shopping-cart sites */

#define	EDBALLS		0	/* deathball site effectiveness */
#define	EAAA		0	/* AAA effectiveness */
#define	ESCARTS		4	/* shopping-cart site effectiveness */

#define	MAXNDBALLS	25	/* max number of deathball sites */
#define	MAXNSCARTS	25	/* max number of shopping-cart sites */
#define	MAXNAAA		25	/* max number of AAA sites */

#define	DRPWID		20		/* Max length of a pilots name */
#define	DRPWIDP		(DRPWID+9)	/* Lt. Col. DRPWID */
#define	DRPWIDPP	(DRPWID+15)	/* 1st Lieutenant DRPWID */
#define	MAXTHEATERS	20
#define	MAXMEDALS	20
/*
 *	A pilot
 */
struct pilot {
	char	name[DRPWID+2];		/* His name */
	unsigned int	pflag;		/* His current state (see bits below) */
	int	nmission;		/* Number of missions */
	int	nresurected;		/* Number of times resurected */
	struct pilotmiss	lastmiss;	/* His last mission */
	struct pilotmiss	bestmiss;	/* His best mission */
	struct pilotmiss	bestofeach;	/* His best of each */
	struct pilotmiss	totalmiss;	/* total of all missions */
	struct ryot	ryo;		/* Roll your own mission stuff */
	unsigned char	ntheaters;
	unsigned char	curtheater;
	unsigned char	theatnos[MAXTHEATERS];
		/* curpilot->theatnos[curpilot->curtheater] = file number */
	long	date;
	unsigned char	rank;
	unsigned char	nmedals[MAXMEDALS];
	long	stockade;
};
/*
 *	These are the bits on pflag above.
 */
#define	PACTIVE		0x8000		/* active pilot is current pilot */
#define	PKILLED		0x2
#define	PCAPTURED	0x8
#define	PFROZEN		0x1		/* Pilot frozen until you register */

extern struct ryot	*curryo;
extern struct pilot	pilots[], *curpilot;

#define	THEATNAMLEN	(DRPWIDP-2)
struct theatert {
	char	name[THEATNAMLEN+1];
	char	fname[10];
	int	nstoredplanes;
	int	nobjectives;
	int	ncobjectives;
	int	isreq;
	int	completed;
};

/*
 *	This is a singly linked list for keeping track of the
 *	individual pilots theaters.  Probably should have been doubly linked.
 */
struct thnamlist {
	char	name[THEATNAMLEN+1];
	int	nstoredplanes;
	int	nobjectives;
	int	ncobjectives;
	int	fno;
	struct thnamlist	*next;
	int	completed;
};

struct rank {
	char	*rank;
	char	*abbrank;
};

struct medal {
	char	*how, *name, *abb, *whatfor;
	unsigned char	left, right, fgnd, bgnd;
};

struct cmdt {
	char	*scmd;
	void	(*ccmd)();
};

struct pertowerinfo {
	long	ox;
	long	oy;
	int	ncloseplanes;
	int	nobjectives;
	int	ncobjectives;
};

struct strip {
	char	*s1;
	int	x1;
	char	*s2;
	int	x2;
	int	x3;
};

struct demot {
	char	name[22];
	char	fname[10];
	int	theatno;
	long	descoffs;
};

union un3 {
	FILE	*fp;
	unsigned char far	*pp;
};

extern void	errorbox();
extern int	spgetch();
extern void	simplesetup();
extern int	tkyo();
extern void	outpname();
extern void	foutpname();

#define	LINELEN	128
#define	REGLINELEN	80
#define	INIT

#define	UPARROW		16	/* ^P */
#define	DOWNARROW	14	/* ^N */
#define	LEFTARROW	2	/* ^B */
#define	RIGHTARROW	6	/* ^F */
#define	DELETE		24	/* ^X */
#define	BACKSPACE	8	/* ^H */
#define	INSERT		1	/* ^A */
#define	END		18	/* ^R */
#define	HOME		20	/* ^T */
#define	REDRAW		12	/* ^L */
#define	PAGEUP		15	/* ^O */
#define	PAGEDOWN	25	/* ^Y */

#define	FLXMESS		5	/* Flashy thing at the beginning */
#define	FLYMESS		2
#define FLXWID		72
#define FLYWID		23

#define	MXMESS		1	/* Main Menu */
#define	MYMESS		2
#define	MXSTUFF		10
#define	MYSTUFF		21

#define	DRXMESS		2	/* Duty-Roster menu */
#define	DRYMESS		6
#define	DRXWID		78
#define	DRYWID		19
#define	MAXPILOTS	DRYWID-4

#define	MIXMESS		2	/* Choose Theater */
#define	MIYMESS		6
#define	MIXWID		78
#define	MIYWID		19

#define	ROXMESS		3	/* Roll your own mission */
#define	ROYMESS		6
#define	ROXWID		75
#define	ROYWID		18

#define	WCKXMESS	15	/* modify enemy wickedness parameters */
#define	WCKYMESS	9
#define	WCKXWID		52
#define	WCKYWID		12

#define	MWCKXMESS	17	/* Modify wickedness */
#define	MWCKYMESS	10
#define	MWCKXWID	47
#define	MWCKYWID	9

#define	NSTXMESS	10	/* choose number of enemy sites */
#define	NSTYMESS	9
#define	NSTXWID		60
#define	NSTYWID		12

#define	MNSTXMESS	12	/* choose number */
#define	MNSTYMESS	10
#define	MNSTXWID	56
#define	MNSTYWID	9

#define	PPXMESS		2	/* Mission Briefing */
#define	PPYMESS		6
#define	PPXWID		77
#define	PPYWID		18

#define	RPXMESS		20	/* Mission Report not available */
#define	RPYMESS		6
#define	RPXWID		46
#define	RPYWID		13

#define	PRXMESS		7	/* Promotion */
#define	PRYMESS		8
#define	PRXWID		66
#define	PRYWID		11

#define	RGXMESS		3	/* Registration Menu */
#define	RGYMESS		6
#define	RGXWID		75
#define	RGYWID		18

#define	KJXMESS		11	/* Keyboard/Joystick setup */
#define	KJYMESS		6
#define	KJXWID		57
#define	KJYWID		16

#define	SEXMESS		3	/* Sounds Editor Menu */
#define	SEYMESS		6
#define	SEXWID		75
#define	SEYWID		18

#define	WDXMESS		3	/* Watch a demo  */
#define	WDYMESS		6
#define	WDXWID		75
#define	WDYWID		18

#define	HSXMESS		13	/* High-Score menu */
#define	HSYMESS		6
#define	HSXWID		57
#define	HSYWID		15

#define	HSPXMESS	3	/* Top-Score List */
#define	HSPYMESS	6
#define	HSPXWID		75
#define	HSPYWID		16

/* #define		MVPCD  /*  This flag is set if program runs on CD */

#ifdef MVPCD
#define fileres  strcat(strcpy(scrlist,wpath),"3.res")
#define fileuniv  strcat(strcpy(scrlist,wpath),"3univ.dat")
#define filetower  strcat(strcpy(scrlist,wpath),"towers.dat")
#define filedemo  strcat(strcpy(scrlist,wpath),"3demo.dat")
#define filecmd  strcat(strcpy(scrlist,wpath),"3.cmd")
#define thtpathname  strcat(strcpy(scrlist,wpath),"tht")
#define thtwildname  strcat(strcpy(scrlist,wpath),"tht\\*.tht")
#define savefile  strcat(strcpy(scrlist,wpath),"moag.cfg")
#endif

#ifndef REGISTERED
#define JPEG 1 /*  This flag turns on JPEG decompression */
#endif

#define NOEXITSCRN 1 /* This flag prevents an exit screen */

#define MAXROLFILES 128

#define NEWYANK 1 /* controls old stupid screen yank vs new smart yank */

#define NEWRANKS 1 /* change ranking system */

/* the following flag SOFSRC is now defined in the makefile */
/* #define SOFSRC 1 /* define this to make a version for retail */

#define musicflag 1 /* this flag allows music */

#ifndef SOFSRC
#define musicmenu 1 /* this flag allows music to play during menus */
#endif

#define	MEMNEEDED	389000	/* Memory needed to run (in paragraphs*/

