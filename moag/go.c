#define	NEWSTUFF	1
#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<string.h>
#include	<dos.h>
#include	"moag.h"
#include	"texto.h"


INIT int	towerno;
int	onetower;
extern char	fileinf[];

INIT struct pilot	curthpilot;
INIT struct theatert	curtheater;

#ifndef	TU

#define		MYSPAWN		1		/**/
#ifndef	MYSPAWN
#include	<process.h>
#endif	/* !MYSPAWN */
#include	<bios.h>
#include	"cecode.h"
#include	"screens.h"

extern char	**eargs;


#ifndef MVPCD  /* if mvpcd, these definitions are equates */
char	fileres[] = "3.res";
char	fileuniv[] = "3univ.dat";
char	filetower[] = "towers.dat";
char	filedemo[] = "3demo.dat";
char	filecmd[] = "3.cmd";
#else
extern	char	wpath[80];
extern	char	scrlist[80];
#endif

extern char	*file3;
extern int	wzflag;
extern int	kbflag;
extern int	jsflag;
extern int	njflag;
#define	LINELEN	128

char	whynorep[LINELEN];

char	flags[LINELEN];
char	edblflag[] = "-t ";		/* Wickedness of death balls */
char	eaaaflag[] = "-v ";		/* Wickedness of aaa */
char	eshcflag[] = "-w ";		/* Wickedness of shopping carts */

char	ndblflag[] = "-l     ";		/* Number of death balls */
char	nshcflag[] = "-m     ";		/* Number of aaa */
char	naaaflag[] = "-n     ";		/* Number of shopping carts */


int	flflag;
INIT int	timeflag = TIMEFLAGNOTVAL;	/* system time changed */

void	go() {
	char	*argvec[LINELEN], buf[LINELEN], **pp;
	int	argcnt, c, newt;
	int	flyflag;
	struct ryot	*pryo;

	extern struct pilot	*curpilot;
	extern struct ryot	firstryo;
	extern int	newplane, preport(), copytheat(), newtower(),
			ismaster(), firsttime, curryoch, fileout();
	extern int	newpreport();
	extern long	regrcode, time();
	extern void	doflag(), mkryotwr(), savestate(), prepfor3();
#ifdef	MYSPAWN
	extern void	myspawn();
#endif
#ifdef	MVPCD
extern	char	wpath[80];
extern	char	scrlist[80];
#endif

	flyflag=0;
	/*
	 *	set up argv[0]
	 */
 	argcnt = 0;
	argvec[argcnt++] = file3;
	flflag = 0;

	/*
	 *	First time stuff.
	 */
//	if (firsttime) {
//		if (!curryoch) /* */
//			pryo = &firstryo;
//		else
//			pryo = curryo;
//		doflag(1, "i");
//	}
//	else
		pryo = curryo;

	/*
	 *	Add in flags
	 */
	doflag(njflag, "g");
	doflag(wzflag, "i");
	doflag((pryo->invflag&INVVAL), "i");
	doflag(~(pryo->invflag| ~FLTMODVAL), "y");
	doflag(~(pryo->invflag| ~RMSGVAL), "{");
	doflag(jsflag, "j");
	doflag(kbflag, "k");
#ifndef REGISTERED
	doflag(ismaster(curpilot), "f");
#else
	doflag(1, "f");
#endif
	towerno = -1;
	if (curpilot->curtheater&0x80) {
		doflag(pryo->actflag, "a");
		if (pryo->edballs) {
			edblflag[2] = (char)('0' + pryo->edballs - 1);
			argvec[argcnt++] = edblflag;
		}
		if (pryo->escarts) {
			eshcflag[2] = (char)('0' + pryo->escarts - 1);
			argvec[argcnt++] = eshcflag;
		}
		if (pryo->eaaa) {
			eaaaflag[2] = (char)('0' + pryo->eaaa - 1);
			argvec[argcnt++] = eaaaflag;
		}

		sprintf(&ndblflag[2], "%-5d", pryo->ndballs);
		argvec[argcnt++] = ndblflag;
		sprintf(&nshcflag[2], "%-5d", pryo->nscarts);
		argvec[argcnt++] = nshcflag;
		sprintf(&naaaflag[2], "%-5d", pryo->naaa);
		argvec[argcnt++] = naaaflag;
		mkryotwr(pryo);
	}
	else {
		copytheat();
		doflag(1, "r");
	}
	if (flflag)
		argvec[argcnt++] = flags;

	if (regrcode >= 0) {
		sprintf(buf, "-c%ld", regrcode);
		argvec[argcnt++] = buf;
	}

/*	argvec[argcnt++] = "-s";			/**/

	newplane = 0;
	c = -1;
	while (1) {
#ifdef	NEWSTUFF
		if (curpilot->curtheater&0x80) {
			if (c < 0)
				c = preport(towerno);
		}
		else {
			if (c < 0)
				c = 'n';
		}
#else	/* !NEWSTUFF */
		if (c < 0)
  		if (curpilot->curtheater&0x80) {
			c =preport(towerno);
		}
		else
		{
			c = 'n';
		if (flyflag!=0) /* time to fly! */
		//	c =preport(towerno);
			c='\n';

		}
#endif	/* !NEWSTUFF */
		if (c == 'N' || c == 'n') {
			newplane = 0;
			if (curpilot->curtheater&0x80) {
				simplesetup(20, 10, 40, 6, "Only one tower",
							MAGENTA, WHITE, RED);
				xyprintf(22, 11,
					"Sorry, there is only one tower in");
				xyprintf(22, 12, "the training missions.");
		     		tkyo(23, 14);
#ifdef	NEWSTUFF
				c = -1;
#endif	/* NEWSTUFF */
				continue;
			}
			if ((newt = newtower(towerno, 0)) < 0) {
/*				unlink(fileinf);	/**/
				unlink(fileuniv); 	/**/
				unlink(filetower);	/**/
				return;
			}
			towerno = newt;
#ifdef	NEWSTUFF
			break;
#endif	/* NEWSTUFF */
			c = -1;
			flyflag=-1;
			continue;
		}
		else if ((c|040) == 'i') {
			simplesetup(PPXMESS, PPYMESS, PPXWID, PPYWID,
					"Detailed Info", YELLOW, BROWN, WHITE);
			c = fileout(fileinf, towerno+1,
				PPXMESS+2, PPYMESS+1, PPXWID-3, PPYWID-2,
							WHITE, BROWN, 7);
			if ((c|040) != 'n')
				c = -1;
			continue;
		}
		else if (c)
			break;
/*		unlink(fileinf);	/**/
		unlink(fileuniv); 	/**/
		unlink(filetower);	/**/
		return;
	}
	if (newplane) {
		doflag(1, "h");
/*		curtheater.nstoredplanes--;	/**/
	}
	for (pp = eargs; *pp != 0; pp++)
		argvec[argcnt++] = *pp;

	argvec[argcnt] = 0;

	time(&curpilot->date);
	savestate();
	prepfor3();
	firsttime = 0;
#ifdef	MYSPAWN
	myspawn(argvec);
#else
	spawnv(P_OVERLAY, file3, argvec);		/**/
#endif
	inito();
	sprintf(buf, "Cannot exec :%s: ", file3);
	for (c = 0; c < argcnt; c++) {
		strcat(buf, " ");
		strcat(buf, argvec[c]);
	}
	errorbox(buf);
}

void	prepfor3() {
	char	buf[LINELEN], *p;
	int	i, xwidm, ywidm, x, y, xwid, ywid;

	extern void	ncexito();

	setblink(1);
	ncexito(1);
	color(CYAN, BLUE);
	erscrn();

	x = 10;
	y = 3;
	xwid = 60;
	ywid = 20;

	cursor(x, y);
	color(YELLOW, CYAN);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	xwidm = xwid - 1;
	for (i = 1; i < xwidm; i++)
		*p++ = ' ';
	*p = '\0';
	ywidm = ywid - 1;
	for (i = 1; i < ywidm; i++) {
		cursor(x, y+i);
		color(YELLOW, CYAN);
		cput(SLEFT);
		color(BLUE, CYAN);
		sput(buf);
		color(YELLOW, CYAN);
		cput(SRIGHT);
		cblock();
	}

	cursor(x, y+ywid-1);
	p = buf;
	*p++ = SLOWERLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = SBOTTOM;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buf);

	cblock();
	cursor(x+1, y+ywid);
	for (i = 0; i < xwid; i++)
		cblock();

	color(BLUE, CYAN);
}

unsigned long	oneweek = ((86400UL*7)-18436572UL);
unsigned long	twoweeks = ((86400UL*14)-154780273UL);

void	come() {
	int	teflag = 0;
	struct pilotmiss	pmiss;
	union REGS	regs;
#ifdef	OLDSHARE
	long	now;

	extern long	instdate, time(), regrcode;
	extern void	timechanged(), trialper1(), nuke(), freeze(),unfreeze();
	extern int	isreg, criterr;
#endif	/* OLDSHARE */
	extern void	reinstate(), getres(), report();
	extern int	kbhit(), cecode(), rundemo;

	reinstate();
	while (kbhit())
		spgetch();

	regs.h.ah = (unsigned char)15;
	int86(0x10, &regs, &regs);
	regs.h.al= (regs.h.al & (unsigned char) 0x7f);
	if (regs.h.al != 0x3) {
		spgetch();
	}

	inito();
/*	errorbox("regs.h.al = %x", regs.h.al);		/**/

#ifdef	OLDSHARE
	if ((isreg != ISREGVAL) && (timeflag != TIMEFLAGNOTVAL)) {
		teflag = 2;
		timechanged();
	}
	time(&now);
	now -= instdate;
#endif	/* OLDSHARE */
	getres(&(pmiss.st3));
#ifdef	OLDSHARE
	if (regrcode >= 0 && criterr != BADREG) {
		isreg = ISREGVAL;
		unfreeze();
	}
	if (isreg != ISREGVAL) {
		if (now > twoweeks) {
			teflag = 1;
			criterr = TRIALPER;
			regrcode = -1;
		}
		else if (now > oneweek)
			trialper1();
	}
	if (teflag || criterr == TRIALPER) {
		nuke("3d.cfg");			/**/
		if (teflag != 2)
			freeze();
		isreg = ISNOTREGVAL;
	}
	if (criterr == BADREG) {
/*		nuke("3d.cfg");			/**/
		isreg = ISNOTREGVAL;
	}
#endif	/* OLDSHARE */
	if (cecode(&pmiss.st3) && !rundemo && !teflag)
		report(&pmiss);
/*	unlink(fileinf);	/**/
	unlink(fileuniv); 	/**/
	unlink(filetower);	/**/
	if (rundemo) {
		unlink(filedemo);	/**/
		rundemo = 0;
	}
	jsflag = 0;
}

#ifdef	OLDSHARE
void	freeze() {
	int	i;
	extern int	npilots;
	extern struct pilot	trainee, *curpilot;

	for (i = 0; i < npilots; i++) {
		if (pilots[i].pflag&PACTIVE) {
			pilots[i].pflag &= ~PACTIVE;
			curpilot = &trainee;
			curpilot->pflag |= PACTIVE;
			curryo = &curpilot->ryo;
		}
		pilots[i].pflag |= PFROZEN;
	}
}

void	unfreeze() {
	int	i;
	extern int	npilots;

	for (i = 0; i < npilots; i++)
		pilots[i].pflag &= ~PFROZEN;
}
#endif	/* OLDSHARE */

void	doflag(flag, s)
	int	flag;
	char	*s;
{
	if (flag) {
		if (!flflag) {
			strcpy(flags, "-");
			flflag = 1;
		}
		strcat(flags, s);
	}
}

/*INIT struct pilot	curthpilot;
INIT struct theatert	curtheater;*/
int	copytheat() {
	union un3	u3;
	FILE	*fpi, *fpo;
	int	fdi, fdo, nobjectives, ncobjectives;
	unsigned int	u;
	char	*p1, *p2, *tname;
	unsigned char	*b1, *b2;
	struct thnamlist	*pthn;

	extern struct thnamlist	*getthnam();
	extern char	*mkthname(), *mktwrname(), *omp;
	extern int	ropen(), wopen(), threeuniv();
	extern unsigned int	coreleft();
	extern void	gettower(), fdcopy(), *malloc(), free(), listobjs();

	onetower = 0;
	simplesetup(29, 11, 24, 3,
			"Analyzing Theater", YELLOW, BROWN, WHITE);
	xyprintf(31, 12, omp);

	pthn = getthnam(curpilot->curtheater);
	u = coreleft();
	u -= 1024;
	u -= 18;
	u /= 2;
	u = (u/512)*512;
	if (u < 512) {
		errorbox("not enough core in copytheat, u=%d\n", u);
		return(0);
	}
	if ((b1 = (unsigned char *)malloc(u)) == NULL) {
		errorbox("Cannot alloc b1 in copytheat");
		return(0);
	}
	if ((b2 = (unsigned char *)malloc(u)) == NULL) {
		errorbox("Cannot alloc b2 in copytheat");
		free(b1);
		return(0);
	}
	tname = mkthname(curpilot->theatnos[curpilot->curtheater]);
	if ((fpi = fopen(tname, "rb")) == NULL) {
		errorbox("Cannot open %s to read", tname);
		free(b2);
		free(b1);
		return(0);
	}
	if (setvbuf(fpi, (void *)b1, _IOFBF, u))
		errorbox("Cannot setvbuf() for fpi in copytheat");
	if ((fpo = fopen(fileuniv, "wb")) == NULL) {
		fclose(fpi);
		free(b2);
		free(b1);
		errorbox("Cannot create universe file for 3!");
		return(0);
	}
	if (setvbuf(fpo, (void *)b2, _IOFBF, u))
		errorbox("Cannot setvbuf() for fp2 in copytheat");
	if (fread((void *)&curtheater, sizeof(struct theatert), 1, fpi) != 1) {
		fclose(fpi);
		fclose(fpo);
		free(b2);
		free(b1);
		errorbox("Cannot read theater from pilots theater file.");
		return(0);
	}
	if (fread((void *)&curthpilot, sizeof(struct pilot), 1, fpi) != 1) {
		fclose(fpi);
		fclose(fpo);
		free(b2);
		free(b1);
		errorbox(
		       "Cannot read pilot structure form pilots theater file.");
		return(0);
	}
	strcpy(curthpilot.name, curpilot->name);

	/* threeuniv checks for onetower */
	u3.fp = fpi;
	if (!threeuniv(&u3, fpo, &nobjectives, &ncobjectives, tname, 1)) {
		fclose(fpi);
		fclose(fpo);
		free(b2);
		free(b1);
		return(0);
	}
	curtheater.nobjectives = nobjectives;
	curtheater.ncobjectives = ncobjectives;
	if (pthn->nobjectives != curtheater.nobjectives) {
		errorbox(
		"Confusion:  %d objectives read, %d objectives remembered",
		  		curtheater.nobjectives, pthn->nobjectives);
		pthn->nobjectives = curtheater.nobjectives;
	}
 
	gettower(fpi);
	fclose(fpi);
	fclose(fpo);
	free(b2);
	free(b1);

	listobjs();

	if ((fdi = ropen(p1 = mktwrname(curtheater.fname))) < 0) {
		if ((fdi = ropen(p2 = mktwrname(NULL))) < 0) {
			errorbox("Cannot open either %s or %s", p1, p2);
			return(0);
		}
	}
	if ((fdo = wopen(filetower)) < 0) {
		close(fdi);
		errorbox("Cannot open %s for writing.", filetower);
		return(0);
	}
	fdcopy(fdo, fdi);
	close(fdi);
	close(fdo);
	return(1);
}

static void	mkryotwr(ryo)
	struct ryot	*ryo;
{
	FILE	*fp;
	char	*p1, *p2;
	extern void	showit();

	if ((fp = fopen(filetower, "wb")) == NULL) {
		errorbox("Cannot open %s for writing.", filetower);
		return;
	}

	p1 = "   ";
	p2 = "             ";

	fprintf(fp, "\r\n\014\r\n\014\r\n\014\r\n\014\r\n\014\r\n");
	fprintf(fp,
  "%sYour objective is to destroy the flying saucer and to take out\r\n",
									p1);
	fprintf(fp,
  "%sas many enemy fuel dumps and ground sites as possible.\r\n", p1);

	fprintf(fp, "\r\n");
	fprintf(fp,
	   "%sLatest intelligence reports enemy strengths as follows:\r\n", p1);

	showit(fp, p2, "Missile site", ryo->ndballs, ryo->edballs);
	showit(fp, p2, "AAA site", ryo->naaa, ryo->eaaa);
	showit(fp, p2, "Ground-Vehicle site", ryo->nscarts, ryo->escarts);
	showit(fp, p2, "Enemy Saucers", 1, ryo->edballs);
#ifndef REGISTERED
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	fprintf(fp, " \\(Note:\\)  Training missions are much harder than the combat missions.");
#endif
	fclose(fp);
}

char	*plural(n)
	int	n;
{
	return((n == 1) ? "" : "s");
}

static void	showit(fp, p2, wh, n, e)
	FILE	*fp;
	char	*p2, *wh;
	int	n, e;
{
	char	buf[LINELEN], *p;
	extern char	*plural();

	sprintf(buf, "%s%d %s%s", p2, n, wh, plural(n));
	if (n) {
		p = &buf[strlen(buf)];
		*p++ = ':';
		for (; p < &buf[38]; *p++ = ' ');
		if (e)
			sprintf(p, "strength %d", e);
		else
			strcpy(p, "random spread of strength");
	}
	fprintf(fp, buf);
	fprintf(fp, "\r\n");
}

void	savetheat() {
	int	fdi, fdo;
	extern int	wopen(), ropen();
	extern char	*mkthname();
	extern void	fdcopy();

	if ((fdo = wopen(mkthname(curpilot->theatnos[curpilot->curtheater])))
									< 0)
		return;
	if ((fdi = ropen(fileuniv)) < 0) {
		close(fdo);
		return;
	}
	if (write(fdo, (void *)&curtheater, sizeof(struct theatert)) !=
						sizeof(struct theatert)) {
		close(fdi);
		close(fdo);
		return;
	}
	if (write(fdo, (void *)&curthpilot, sizeof(struct pilot)) !=
						sizeof(struct pilot)) {
		close(fdi);
		close(fdo);
		return;
	}
	fdcopy(fdo, fdi);
	close(fdi);
	close(fdo);
}

int	criterr;
void	getres(p3)
	struct stuffrm3	*p3;
{
	int	fd;
	long	dummy;
	extern int	ropen();
	extern long	chksum;			/* Rom check-sum */

	if ((fd = ropen(fileres)) < 0) {
		sprintf(whynorep, "cannot open %s for reading", fileres);
		return;
	}
	if (read(fd, (void *)p3, sizeof(struct stuffrm3)) !=
						sizeof(struct stuffrm3)) {
		sprintf(whynorep, "cannot read info from %s", fileres);
		close(fd);
		unlink(fileres);	/**/
		return;
	}
	if (read(fd, (void *)&dummy, sizeof(long)) != sizeof(long)) {
		errorbox("Cannot read rom checksum in 3.res");
		close(fd);
		unlink(fileres);	/**/
		return;
	}
	if (dummy >= 0)
		chksum = dummy;
	criterr = 0;
	read(fd, (void *)&criterr, sizeof(int));
	whynorep[0] = '\0';
	close(fd);
	unlink(fileres); 	/**/
}

static void	gettower(fp)
	FILE	*fp;
{
	long	x, y, z;
	int	xp, yp;

	if (onetower) {
		towerno = 4;
		return;
	}

	towerno = -1;

	if (fseek(fp, -3L*sizeof(long), SEEK_END) < 0) {
		errorbox("Seek error looking for start-coordinates!");
		return;
	}
	if (fread((void *)&x, sizeof(long), 1, fp) != 1 ||
	    fread((void *)&y, sizeof(long), 1, fp) != 1 ||
	    fread((void *)&z, sizeof(long), 1, fp) != 1) {
		errorbox("Read error looking for start-coordinates!");
		return;
	}
	xp = (int)((x - (-2*(65536L) + 3001) - (-11*(65536L) - 1) -
				(-128*(65536L) + 0))/(64*65536) - 1);
	yp = (int)((y - (-128*(65536L) + 0))/(64*65536) - 1);
	yp *= 3;
	towerno = xp + yp;
/*
{
char buf[128];
errorbox("tower=%d  xp=%d  yp=%d  x=%ld  y=%ld  z=%ld",
towerno,xp,yp/3,x,y,z);
}
*/
}

#endif	/* !TU */

struct strip	strips[] = {
	{ "Henderson",			63,	"Field",		65, 60},
	{ "Dulles Inter-",		61,	"national Airport",	60, 60},
	{ "Bubba's",			63,	"Airstrip",		63, 60},
	{ "Weedsport",			39,	"Airport",		40, 36},
	{ "RDU ^international",		35,	"Airport",		40, 36},
	{ "Crickety Point",		37,	"Landing Strip",	37, 36},
	{ "Sweetly Bending Oak",	6,	"Aircraft Park",	8,   7},
	{ "Easter Island Municipal",	5,	"Air Travel Center",	6,   7},
	{ "Valmart Airstrip and",	6,	"Family Value Center",	6,   7}
};

void	outstrip(xp, yp)
	int	xp, yp;
{
	int	t, y;
	char	buf[32];
	extern struct pertowerinfo	ptos[];

	t = 3*yp + xp;
	y = PPYMESS + PPYWID - 4*(xp+1) - 2;

	cursor(strips[t].x1, y);
	sput(strips[t].s1);
	cursor(strips[t].x2, y+1);
	sput(strips[t].s2);
	sprintf(buf, "(%d plane%s  %d MO)", ptos[t].ncloseplanes,
			 (ptos[t].ncloseplanes == 1) ? "" : "s",
			 ptos[t].nobjectives);
	cursor(strips[t].x3, y+2);
	sput(buf);
}

int	newtower(ctn, tflag)
	int	ctn, tflag;
{
#ifndef	TU
	int	fd;
#endif	/* TU */
	int	xp, yp, nxp, nyp, c, t, fore, back, bord;
	char	buf[LINELEN], *p1, *p2, *p3;
	int far	*scr;
	long	xs, ys;
	extern int	fileout();
	extern void	center();
#ifdef	MVPCD
extern	char	wpath[80];
extern	char	scrlist[80];
#endif

#ifdef	NEWSTUFF
	int	y, first;
	extern int	noobjectives, noplanes, nhobjectives, oktower();
	extern void	checktower();
#endif	/* NEWSTUFF */

#ifndef	TU
	if (!tflag) {
		if ((fd = open(fileuniv, O_RDWR|O_BINARY)) < 0)
			return(-1);
		if (lseek(fd, -3L*sizeof(long), SEEK_END) < 0) {
			errorbox("Seek error looking for start-coordinates!");
			close(fd);
			return(-1);
		}
		if (onetower) {
p1 = "Your Starting Airfield";
p2 = "There is only one airfield in this Theater-of-Operation.";
#ifdef	NEWSTUFF
p3 = "Type <CR> to fly, `i' for detailed info, or <ESC> to abort mission.";
#else	/* !NEWSTUFF */
p3 = "Type <CR> to return, `i' for detailed info, or <ESC> to abort mission.";
#endif	/* !NEWSTUFF */
		}
		else {
p1 = "Choose New Starting Airfield";
p2 = "Choose new starting airfield with arrow keys, then use";
#ifdef	NEWSTUFF
p3 = "<CR> to select, `i' for detailed info, or <ESC> to abort mission.";
#else	/* !NEWSTUFF */
p3 = "<CR> to select and fly, `i' for detailed info, or <ESC> to abort.";
#endif	/* !NEWSTUFF */
		}
		fore = BLUE;
		back = CYAN;
		bord = MAGENTA;
	}
	else {
#endif	/* !TU */
		p1 = "Get Detailed info about Airfield";
		if (onetower) {
p2 = "Type <CR> or `i' for detailed info, <ESC> to return.";
p3 = "  ";
		}
		else {
p2 = "Choose desired airfield with arrow keys, then use";
p3 = "<CR> or `i' for detailed info, <ESC> to return.";
		}
		fore = WHITE;
		back = RED;
		bord = GREEN;
#ifndef	TU
	}
	simplesetup(PPXMESS, PPYMESS, PPXWID, PPYWID, p1, bord, back, fore);
#else	/* TU */
	nbsimplesetup(PPXMESS, PPYMESS, PPXWID, PPYWID, p1, bord, back, fore);
#endif	/* TU */
	color(YELLOW, back);
	center(buf, PPXWID-2, p2);
	xyprintf(PPXMESS+1, PPYMESS+1, buf);
	center(buf, PPXWID-2, p3);
	xyprintf(PPXMESS+1, PPYMESS+2, buf);

	color(fore, back);
	if (!onetower) {
		for (xp = 0; xp < 3; xp++)
			for (yp = 0; yp < 3; yp++)
				outstrip(xp, yp);
	}
	else {
		outstrip(1, 1);
	}

	xyprintf(PPXMESS+24, PPYMESS+PPYWID-2,
		"MO = Mission Objectives remaining");
	bold();
	xp = ctn%3;
	yp = ctn/3;
	nxp = xp;
	nyp = yp;

	outstrip(xp, yp);
#ifdef	NEWSTUFF
	checktower(3*yp + xp);
	if (noobjectives) {
		scr = yankscr();
		savepos();
		y = 10;
		simplesetup(15, y++, 50, 7, "Theater is Complete",
						MAGENTA, WHITE, RED);
		beep();
		xyprintf(17, y++,
			"There are no mission objectives remaining in");
		xyprintf(17, y++, "this Theater of Operations.");
		y++;
		xyprintf(21, y++, "Type <ESC> to abort,");
		xyprintf(22, y++,
			"or almost anything else to continue...");
		c = spgetch();
		putscr(scr);
		freescr(scr);
		restpos();
		noobjectives = 0;
		if (c == 033)
			return(-1);
	}
#endif	/* NEWSTUFF */
	while (1) {
		c = spgetch();
		if (c == 033 || c == 010) {
#ifndef	TU
			if (!tflag)
				close(fd);
#endif	/* TU */
			if (c == 033)
				return(-1);
			else
				return(ctn);
		}
		else if (c == 'i') {
			scr = yankscr();
			savepos();
			t = 3*yp + xp;
#ifndef	TU
			simplesetup(PPXMESS, PPYMESS, PPXWID, PPYWID,
					"Detailed Info", YELLOW, BROWN, WHITE);
			fileout(fileinf, t+1,
				PPXMESS+2, PPYMESS+1, PPXWID-3, PPYWID-2,
							WHITE, BROWN, 7);
#else	/* TU */
			color(WHITE, BROWN);
			boxsetup(1, 1, 80, 25, 0);
			fileout(fileinf, t+1,
				2, 2, 78, 23, WHITE, BROWN, 2);
#endif	/* TU */
			putscr(scr);
			restpos();
			freescr(scr);
		}
		else if (c== '\n') {
#ifdef	NEWSTUFF
			if (!tflag && c == '\n') {
				checktower(3*yp + xp);
				c = oktower();
				if (c == 1)
					break;
				else if (c == 0)
					continue;
				else
					return(-1);
			}
#else	/* !NEWSTUFF */
			break;
#endif	/* !NEWSTUFF */
		}
		else if (onetower)
			continue;
		else if (c == UPARROW || c == 'k') {
			if (xp == 2)
				nxp = 0;
			else
				nxp = xp + 1;
		}
		else if (c == DOWNARROW || c == 'j') {
			if (xp == 0)
				nxp = 2;
			else
				nxp = xp - 1;
		}
		else if (c == RIGHTARROW || c == 'l') {
			if (yp == 0)
				nyp = 2;
			else
				nyp = yp - 1;
		}
		else if (c == LEFTARROW || c == 'h') {
			if (yp == 2)
				nyp = 0;
			else
				nyp = yp + 1;
		}
		else {
			beep();
			continue;
		}
		normal();
		outstrip(xp, yp);
		xp = nxp;
		yp = nyp;
		bold();
		outstrip(xp, yp);
	}

	if (tflag)
		return(3*yp + xp);

#ifndef	TU
	xs = ((long)xp + 1L)*(64*65536L) + (-128*(65536L) + 0) +
			(-11*(65536L) - 1) + (-2*(65536L) + 3001);
	ys = ((long)yp + 1L)*(64*65536L) + (-128*(65536L) + 0);
/*
	nxp = (int)((xs - (-2*(65536L) + 3001) - (-11*(65536L) - 1) -
				(-128*(65536L) + 0))/(64*65536) - 1);
	nyp = (int)((ys - (-128*(65536L) + 0))/(64*65536) - 1);
	if (nxp != xp)
		errorbox("Got x wrong");
	if (nyp != yp)
		errorbox("Got y wrong");
 */
	if (write(fd, (void *)&xs, sizeof(long)) != sizeof(long) ||
	    write(fd, (void *)&ys, sizeof(long)) != sizeof(long)) {
		errorbox("Write error on tower!");
		close(fd);
		return(-1);
	}
	close(fd);
#endif	/* TU */
	return(3*yp + xp);
}

#ifdef	NEWSTUFF
void	checktower(tower)
	int	tower;
{
	extern int	noobjectives, noplanes, nhobjectives, oktower();
	extern struct pertowerinfo	ptos[];

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
	}
}

int	oktower() {
	int far	*scr;
	int	y, c;
	extern int	newplane, noobjectives, noplanes, nhobjectives,
			oktower();

	if (nhobjectives) {
		scr = yankscr();
		savepos();
		y = 12;
		simplesetup(4, y++, 72, 6,
		 "No Objectives at this Airfield", MAGENTA, WHITE, RED);
		beep();
		xyprintf(5, y++,
"Warning! There are no uncompleted mission objectives at this airstrip.");
		y++;
		xyprintf(18, y++,
		           "Type <CR> to fly anyway, <ESC> to abort,");
		xyprintf(12, y++,
		     "or almost anything else to choose a different airstrip.");

		c = spgetch();
		putscr(scr);
		freescr(scr);
		restpos();
		nhobjectives = 0;
		if (c == 033)
			return(-1);
		else if (!(c == '\n' || c == '\r'))
			return(0);
	}
	if (noplanes) {
		scr = yankscr();
		savepos();
		y = 12;
		simplesetup(9, y++, 62,
			   ((curtheater.nstoredplanes) ? 9 : 7),
		     "No Planes at this Airfield", MAGENTA, WHITE, RED);
		beep();
		xyprintf(12, y++,
	"Warning!  There are no planes available at this airstrip.");
		if (curtheater.nstoredplanes != 0)
			xyprintf(12, y++,
	"        (However, you have %d plane%s in storage.)",
			      curtheater.nstoredplanes,
			    (curtheater.nstoredplanes == 1) ? "" : "s");
		y++;
		if (curtheater.nstoredplanes > 0)
			xyprintf(13, y++,
	"        Type 'P' to pull a new plane from storage.");
		xyprintf(13, y++,
	"      Type <CR> to continue with the mission anyway.");
		xyprintf(11, y++,
	"                    Type <ESC> to abort.");
		xyprintf(11, y++,
	"       Type almost anything else to select a new tower.");

		c = spgetch();
		putscr(scr);
		freescr(scr);
		restpos();
		noplanes = 0;
		if (curtheater.nstoredplanes > 0 && (c|040) == 'p') {
			newplane = 1;
			return(1);
		}
		else if (c == '\r' || c == '\n')
			return(1);
		else if (c == 033)
			return(-1);
		else
			return(0);
	}
	return(1);
}
#endif	/* NEWSTUFF */

#ifndef	TU

#ifdef	MYSPAWN
void	myspawn(argvec)
	char	**argvec;
{
	FILE	*fp;
	extern void	exit();
#ifdef	MVPCD
extern	char	wpath[80];
extern	char	scrlist[80];
#endif
#ifdef musicflag
	extern int musicstat;
	extern int driverloaded;
	extern	int unloadsound();
#ifdef musicmenu
	extern	void endsong();
	extern	void SoundCall();
#endif
#endif

	if ((fp = fopen(filecmd , "wb")) == NULL) {
		errorbox("Cannot creat 3.cmd!");
		return;
	}

	argvec++;
	while (*argvec != NULL)
		fprintf(fp, "%s\r\n", *argvec++);
	fclose(fp);
#ifdef musicflag
#ifdef musicmenu
	if((musicstat &1) != 0)
	{
	  musicstat=0;
	  endsong();
	}
	if(!driverloaded) {
	  SoundCall(5,0);
	  if(unloadsound()) errorbox("error unloading driver");
	}
#else
	if(!driverloaded) {
	  if(unloadsound()) errorbox("error unloading driver");
	}
#endif
#endif
	exit(0);
}
#endif	/* MYSPAWN */

#endif	/* !TU */

/*
  Valmart Airstrip and          Crickety Point              Bubba's
  Family Value Center           Landing Strip              Airstrip
      (2 planes)                 (1 plane)                 (1 plane)
  
  Easter Island Municipal       RDU ^international        Dulles Inter-
    Air Travel Center              Airport              national Airport
       (0 planes)                 (1 plane)                 (1 plane)
  
  Sweetly Bending Oak             Weedsport                Henderson
    Aircraft Park                  Airport                   Field
       (2 planes)                 (1 plane)                 (1 plane)
*/
