extern char	*prog;
extern char	*file3;

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
	int	planesflown;		/* Number of planes flown */
	int	planeslost;		/* Number of planes lost */
	int	tickskilled;		/* Number of ticks killed */
	int	dbkilled[8];		/* Number of death-balls killed */
	int	aaakilled[8];		/* Number of aaa-sites killed */
	int	shckilled[8];		/* Number of shopping-carts killed */
	int	siteskilled[8];		/* Number of sites killed */
	int	objectives;		/* Number of objectives accomplished */
	int	friendkilled;		/* Number of friendlies killed */
	int	totaldamage;		/* Total plane damage */
	int	dbonme[8];		/* Number of death-balls on me */
	int	aaaonme[8];		/* Number of aaa-sites on me */
	long	seconds;		/* Seconds in the air */
	int	orbskilled[8];		/* Number of orbs killed */
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
	char	invflag;	/* invincible plane flag */
};
#define	NDBALLS		2	/* default number of deathball sites */
#define	NAAA		4	/* default number of AAA sites */
#define	NSCARTS		2	/* default number of shopping-cart sites */

#define	EDBALLS		1	/* deathball site effectiveness */
#define	EAAA		2	/* AAA effectiveness */
#define	ESCARTS		0	/* shopping-cart site effectiveness */

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
	unsigned int	flag;		/* His current state (as above) */
	int	nmission;		/* Number of missions */
	int	nresurected;		/* Number of times resurected */
	struct pilotmiss	lastmiss;	/* His last mission */
	struct pilotmiss	bestmiss;	/* His best mission */
	struct pilotmiss	bestofeach;	/* His best of each */
	struct pilotmiss	totalmiss;	/* total of all missions */
	struct ryot	ryo;		/* Roll your own mission stuff */
	unsigned char	ntheaters;
	signed char	curtheater;
	unsigned char	theatnos[MAXTHEATERS];
		/* curpilot->theatnos[curpilot->curtheater] = file number */
	long	date;
	unsigned char	rank;
	unsigned char	nmedals[MAXMEDALS];
	long	stockade;
};
#define	ACTIVE		0x8000

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
 *	individual pilots theaters.  Probably should be doubly linked.
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
	char	*how;
	char	*name;
	char	*abb;
	char	*whatfor;
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

struct demot {
	char	name[22];
	char	fname[10];
	int	theatno;
	long	descoffs;
};

extern void	errorbox();
extern int	spgetch();
extern void	simplesetup();
extern int	tkyo();
extern void	outpname();
extern void	foutpname();

#define	LINELEN	128
#define	INIT

#define	DRYWID		19
#define	MAXPILOTS	DRYWID-4
