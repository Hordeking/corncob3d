#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<string.h>

#include	"moag.h"
#include	"texto.h"

#ifdef JPEG
#include	<process.h>
#endif /* JPEG */

#ifndef MVPCD
char	*pilotlist = "pilot.scr";
#else
extern	char	wpath[80];
extern	char	scrlist[80];
#define pilotlist  strcat(strcpy(scrlist,wpath),"pilot.scr")
#endif


extern struct pilot	pilots[], trainee, *curpilot;
extern int	npilots;
	/* reg.c */
extern int	isreg;			/* Is program registered flag */
extern long	chksum;			/* Rom check-sum (from 3) */
extern long	regrcode;		/* registration reply code (from PIE) */
long	instdate;			/* Install date */

unsigned int	seed;
int	firsttime = 0;
char	*initp = "Initializing";
void	initscore() {
	long	lastflown, now;
	int	i, fd;
	unsigned char	abyte;

	extern int	kbflag, njflag, ropen(), mread(), inity;
#ifdef	OLDSHARE
	extern int	timeflag;
	extern void	unfreeze();
	extern void	checksum();
#endif	/* OLDSHARE */
	extern struct ryot	defryo;
	extern long	time();
	extern void	nuke();
#ifdef	__TURBOC__
	extern void	srand();
#else
	extern long	_seed;
#endif
	extern char	*initp;
#ifdef musicflag
	extern char musicwanted;
#endif

	strcpy(trainee.name, "trainee");
	trainee.ryo = defryo;
	trainee.curtheater = (unsigned char)0x80;
	curryo = &trainee.ryo;
	if ((fd = ropen(pilotlist)) < 0) {
		/*
		 *	First time played...
		 */
		firsttime = 1;
		xyprintf(1, inity++, "%s pilots...", initp);
		time(&instdate);
		nuke("3d.cfg");
		return;
	}
	if (read(fd, (void *)&i, sizeof(int)) != sizeof(int) || i != SN) {
		close(fd);
		time(&instdate);
		nuke("3d.cfg");
		return;
	}
	read(fd, (void *)&seed, sizeof(unsigned int));
#ifdef	__TURBOC__
	srand(seed);
#else
	_seed = (long)seed;
#endif
	mread(fd, (void *)&abyte, 1);
	instdate += ((long)abyte)<<24;
	mread(fd, (void *)&abyte, 1);
	lastflown = ((long)abyte)<<8;
	if (mread(fd, (void *)&kbflag, sizeof(int)) != sizeof(int)) {
		errorbox("Error reading file %s", pilotlist);
		time(&instdate);
		close(fd);
		return;
	}
	mread(fd, (void *)&abyte, 1);
	instdate += ((long)abyte);
	mread(fd, (void *)&abyte, 1);
	lastflown += ((long)abyte)<<16;
	if (mread(fd, (void *)&njflag, sizeof(int)) != sizeof(int)) {
		errorbox("Error reading file %s", pilotlist);
		time(&instdate);
		close(fd);
		return;
	}
	mread(fd, (void *)&abyte, 1);
#ifdef	REGISTERED
	isreg = ISREGVAL;
#else
	isreg = 0;
#endif	/* !REGISTERED */
	time(&now);
	instdate += ((long)abyte)<<16;
	mread(fd, (void *)&abyte, 1);
	lastflown += ((long)abyte);
	mread(fd, (void *)&chksum, sizeof(long));
	mread(fd, (void *)&regrcode, sizeof(long));
	mread(fd, (void *)&abyte, 1);
	instdate += ((long)abyte)<<8;
	mread(fd, (void *)&abyte, 1);
	lastflown += ((long)abyte)<<24;
	mread(fd, (void *)&abyte, 1);
#ifndef	REGISTERED
	isreg += ((int)abyte)<<8;
#endif	/* !REGISTERED */

#ifdef musicflag
	mread(fd, (void *)&musicwanted, 1);
#endif

	mread(fd, (void *)&npilots, sizeof(int));
	for (i = 0; i < npilots; i++) {
		mread(fd, (void *)&pilots[i], sizeof(struct pilot));
		if (pilots[i].pflag&PACTIVE)
			curpilot = &pilots[i];
		if (i == 0) {
			mread(fd, (void *)&abyte, 1);
#ifndef	REGISTERED
			isreg += abyte;
#ifdef	OLDSHARE
			if ((isreg != ISREGVAL) && (lastflown > now))
				timeflag = TIMEFLAGVAL;
#endif	/* OLDSHARE */
#endif	/* !REGISTERED */
		}
		if (i == MAXPILOTS-1) {
			i++;
			break;
		}
	}
	if (i == 0) {
		mread(fd, (void *)&abyte, 1);
#ifndef	REGISTERED
		isreg += abyte;
#ifdef	OLDSHARE
		if ((isreg != ISREGVAL) && (lastflown > now))
			timeflag = TIMEFLAGVAL;
#endif	/* OLDSHARE */
#endif	/* !REGISTERED */
	}
	curryo = &(curpilot->ryo);
	npilots = i;
	close(fd);
#ifdef	OLDSHARE
	checksum();
	if (isreg == ISREGVAL)
		unfreeze();
#endif	/* OLDSHARE */
}

#ifdef	OLDSHARE
void	checksum() {
	char	*p, *p1;
	int	fd;
	unsigned int	u, n, i;
	long	sum, rsum, rsize;
	struct stat	sb;
	extern int	ropen()/*, isreg */;
	extern void	*malloc(), free(), nuke(), saybyebye();
	extern unsigned int	coreleft();

/*
	if (isreg == ISREGVAL)
		return;
 */
	fd = ropen("moag.exe");
	if (fd < 0) {
		nuke("3d.cfg");
		return;
	}
	u = coreleft();
	u -= 18;
	u = (u/1024)*1024;
	while ((p = (char *)malloc(u)) == NULL) {
		if (u < 1024) {
			errorbox("Cannot alloc p in fdcopy");
			close(fd);
			return;
		}
		u -= 1024;
	}
	if (u < 1024) {
		close(fd);
		free(p);
		return;
	}
	sum = 0;
	while ((n = read(fd, p, u)) > 0)
		for (i = 0, p1 = p; i < n; i++)
			sum += *p1++;
	free(p);
	close(fd);

	if ((fd = ropen("tower.exe")) < 0) {
		nuke("3d.cfg");
		errorbox("tower.exe program error");
		saybyebye();
	}
	lseek(fd, -(long)2*sizeof(long), SEEK_END);
	rsize = 0;
	rsum = 0;
	read(fd, (void *)&rsize, sizeof(long));
	read(fd, (void *)&rsum, sizeof(long));
	if (stat("moag.exe", &sb) < 0) {
		nuke("3d.cfg");
		close(fd);
		return;
	}
	if (sb.st_size != rsize || rsum != sum) {
		nuke("3d.cfg");
		saybyebye();
	}
	close(fd);
}
#endif	/* OLDSHARE */

int	mread(fd, p, n)
	int	fd;
	void	*p;
	unsigned int	n;
{
	unsigned char	*pp, mask;
	unsigned int	n1, nn;
	extern int	rand();

	n1 = read(fd, p, n);
	nn = n1;
	pp = (unsigned char *)p;
	while (nn--) {
		mask = (unsigned char)(rand()/128);		/**/
		*pp++ ^= mask;
	}
	return(n1);
}

int	ropen(s)
	char	*s;
{
	return(open(s, O_RDONLY));
}

void	savescore() {
	int	sn, fd, i;
	unsigned char	abyte;
	long	lastflown, now;
	extern int	kbflag, njflag, wopen(), mwrite();
#ifdef musicflag
	extern char musicwanted;
#endif
	extern long	time();
#ifdef	__TURBOC__
	extern void	srand();
#else
	extern long	_seed;
#endif

	if ((fd = wopen(pilotlist)) < 0)
		return;
	sn = SN;
	write(fd, (void *)&sn, sizeof(int));

	time(&now);
	seed = (unsigned int)(now);
#ifdef	__TURBOC__
	srand(seed);
#else
	_seed = (long)seed;
#endif
	write(fd, (void *)&seed, sizeof(unsigned int));

	abyte = (unsigned char)((instdate>>24)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	time(&lastflown);
	abyte = (unsigned char)((lastflown>>8)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	mwrite(fd, (void *)&kbflag, sizeof(int));
	abyte = (unsigned char)((instdate)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	abyte = (unsigned char)((lastflown>>16)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	mwrite(fd, (void *)&njflag, sizeof(int));
	abyte = (unsigned char)((instdate>>16)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	abyte = (unsigned char)((lastflown)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	mwrite(fd, (void *)&chksum, sizeof(long));
	mwrite(fd, (void *)&regrcode, sizeof(long));
	abyte = (unsigned char)((instdate>>8)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	abyte = (unsigned char)((lastflown>>24)&0xff);
	mwrite(fd, (void *)&abyte, 1);
	abyte = (unsigned char)((isreg>>8)&0xff);
	mwrite(fd, (void *)&abyte, 1);
#ifdef musicflag
	abyte = (unsigned char)(musicwanted&0xff);
	mwrite(fd, (void *)&abyte, 1);
#endif

	mwrite(fd, (void *)&npilots, sizeof(int));
	for (i = 0; i < npilots; i++) {
		mwrite(fd, (void *)&pilots[i], sizeof(struct pilot));
		if (i == 0) {
			abyte = (unsigned char)(isreg&0xff);
			mwrite(fd, (void *)&abyte, 1);
		}
	}
	if (i == 0) {
		abyte = (unsigned char)(isreg&0xff);
		mwrite(fd, (void *)&abyte, 1);
	}

	close(fd);
}

int	mwrite(fd, p, n)
	int	fd;
	void	*p;
	unsigned int	n;
{
	unsigned char	*pp, buf[sizeof(struct pilot)], *pb, mask;
	unsigned int	nn;
	extern int	rand();
	nn = n;
	pp = (unsigned char *)p;
	pb = buf;
	while (nn--) {
		mask = (unsigned char)(rand()/128);		/**/
		*pb++ = (unsigned char)(*pp++^mask);
	}
	return(write(fd, (void *)buf, n));
}

int	wopen(s)
	char	*s;
{
	return(open(s, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, S_IREAD|S_IWRITE));
}

#ifdef	__TURBOC__
#include	<dir.h>
#else
#include	<direct.h>
#endif
#include	<dos.h>
#include	<io.h>
/*
 *	Make def/ if necessary, and move .def/.cct and .twr files into it.
 *	Make img/ if necessary, and move .img files into it.
 *
 *	Construct and sort theater table.
 *
 *	Check for absent .tht file.
 *	Check for extra .tht file.
 *
 *	Make dmo/ if necessary, and move .dmo and demo.ndx files into it.
 *	Construct demo table.
 */
void	initialize() {
	int	i, j, n;
	char	*p1, *p2, *prd, buf[LINELEN];
char buf1[80];
	unsigned char	dummy;
	struct ffblk	fb;
	struct ilist {
		int	val;
		unsigned char	mark;
		struct ilist	*next;
	} *head, *pc, *pl;
	struct pilot	*pp;
	FILE	*fp,*fp1;
	union un3	u3;

	extern int	npilots, ntheaters, threeuniv(),
			cmptheater(), inity;
	extern struct theatert	*theaters;
#ifdef	REGISTERED
	extern struct demot	*demos;
	extern int	ndemos, cmpdemo();
#endif
	extern void	fatal(), *calloc(), missing(), mylower(), chkdef(),
			free(), qsort(), inithiscr();
#ifdef JPEG
	extern void	convjpgs();
#endif /* JPEG */
	extern char	*mkthname(), *fngets(), *initp;

#ifdef  MVPCD
extern	char	wpath[80];
extern	char	scrlist[80];
#else
	char	*thtpathname;
#endif

	if (firsttime)
		xyprintf(1, inity++, "%s flight images...", initp);
	p1 = "img";
	p2 = "Sorry, I cannot initialize the image directory `img'";
	if (findfirst(p1, &fb, 0) != 0)
		mkdir(p1);
	else if (!(fb.ff_attrib&FA_DIREC)) {
		chmod(p1, S_IREAD|S_IWRITE);
		unlink(p1);
		if (mkdir(p1) < 0)
			fatal(p2);
	}
	chkdef(p1, "*.img");

	if (firsttime)
		xyprintf(1, inity++, "%s missions...", initp);
	ntheaters = 0;
	p1 = "def";
	p2 = "Sorry, I cannot initialize the universe directory `def'";
	if (findfirst(p1, &fb, 0) != 0)
		mkdir(p1);
	else if (!(fb.ff_attrib&FA_DIREC)) {
		chmod(p1, S_IREAD|S_IWRITE);
		unlink(p1);
		if (mkdir(p1) < 0)
			fatal(p2);
	}
#ifdef	REGISTERED
	chkdef(p1, "*.def");
#else	/* !REGISTERED */
	chkdef(p1, "*.cct");
#endif	/* !REGISTERED */
	chkdef(p1, "*.twr");
	prd = "Cannot get to the right directory!";
	if (chdir(p1) >= 0) {
#ifdef	REGISTERED
		p1 = "*.def";
#else	/* !REGISTERED */
		p1 = "*.cct";
#endif	/* !REGISTERED */
		for (n = findfirst(p1, &fb, 0); !n; n = findnext(&fb)) {
			if (fb.ff_attrib&(FA_DIREC|FA_LABEL))
				continue;
			ntheaters++;
		}
		if ((theaters = (struct theatert *)calloc(ntheaters,
			     sizeof(struct theatert))) == NULL && ntheaters)
			fatal("Cannot allocate memory for theaters");
		i = 0;
		/*
		 * Find a file matching *.def or *.cct...
		 */
		for (n = findfirst(p1, &fb, 0); !n; n = findnext(&fb)) {
			/*
			 * Skip it if it's not a plain file
			 */
			if (fb.ff_attrib&(FA_DIREC|FA_LABEL))
				continue;

			/* convert name to lower case and parse out .def or .cct
			 */
			mylower(fb.ff_name);
			for (p1 = fb.ff_name; *p1 != '.' && *p1; p1++);
			if (*p1)
				*p1 = ' ';

			/* scan what's left into file name
			 */
#ifdef	REGISTERED
			if (sscanf(fb.ff_name, "%s%s", theaters[i].fname, buf)
					!= 2 || strcmp(buf, "def") != 0)
#else	/* !REGISTERED */
			if (sscanf(fb.ff_name, "%s%s", theaters[i].fname, buf)
					!= 2 || strcmp(buf, "cct") != 0)
#endif	/* !REGISTERED */
				continue;

			/* get number of stored planes, and objectives
			 */
#ifdef	REGISTERED
			sprintf(buf, "%s.def", theaters[i].fname);
#else	/* !REGISTERED */
			sprintf(buf, "%s.cct", theaters[i].fname);
#endif	/* !REGISTERED */
			if ((fp = fopen(buf, "rb")) == NULL)
				continue;
			fseek(fp, 10L, SEEK_CUR);
			if (fread(&dummy, sizeof(unsigned char), 1, fp) != 1)
				theaters[i].nstoredplanes = 0;
			else
				theaters[i].nstoredplanes = dummy;
			if (theaters[i].nstoredplanes & (unsigned)0x80) {
				theaters[i].nstoredplanes &= ~(unsigned)0x80;
				theaters[i].isreq = 1;
			}
			else
				theaters[i].isreq = 0;
			fseek(fp, 0L, SEEK_SET);
			u3.fp = fp;
			threeuniv(&u3, NULL, &theaters[i].nobjectives,
					     &theaters[i].ncobjectives, buf, 0);
			fclose(fp);

			/* open tower file, and assign theater name
			 */
			sprintf(buf, "%s.twr", theaters[i].fname);
			if ((fp = fopen(buf, "rb")) == NULL)
				strcpy(theaters[i].name, theaters[i].fname);
			else {

					/* the following just removes the
					 first line of the tower file if it
					starts with an 'X' */
				fgets(buf,80,fp);
				if(buf[0]!=0x58) /* If no X line, reset fp */
					fseek(fp,0L,SEEK_SET);

				buf[0] = '\0';
				fread(buf, sizeof(char), THEATNAMLEN+2, fp);
				buf[THEATNAMLEN] = '\0';
				for (p1 = buf, p2 = theaters[i].name;
						*p1 != '\0' && *p1 != '\n' &&
						*p1 != '\r' && *p1 != 12; p1++)
					*p2++ = *p1;
				*p2 = '\0';
				if (buf[0] == '\0')
					strcpy(theaters[i].name,
							theaters[i].fname);
				fclose(fp);
			}
			i++;
		}
		if (i != ntheaters) {
			/* Weirdness ??? */
			ntheaters = i;
		}
		qsort(theaters, ntheaters, sizeof(struct theatert), cmptheater);
		if (chdir("..") < 0)
			fatal(prd);
	}
	else {
		errorbox("Cannot change dirs to def");
	}

	if (firsttime)
		xyprintf(1, inity++, "%s current theaters...", initp);
#ifndef MVPCD
	thtpathname = "tht";
#endif
	p2 = "Sorry, I cannot initialize the theaters directory `tht'";
	if (findfirst(thtpathname, &fb, 0) != 0)
		mkdir(thtpathname);
	else if (!(fb.ff_attrib&FA_DIREC)) {
		chmod(thtpathname, S_IREAD|S_IWRITE);
		unlink(thtpathname);
		if (mkdir(thtpathname) < 0)
			fatal(p2);
	}

#ifndef MVPCD
	if (chdir(thtpathname) < 0)
		fatal(prd);
	head = NULL;
	pl = NULL;
	if (findfirst("*.tht", &fb, 0) == 0) do {
		if (sscanf(fb.ff_name, "%d.tht", &n) != 1)
			continue;
		if (fb.ff_attrib&FA_RDONLY)
			chmod(fb.ff_name, S_IREAD|S_IWRITE);
		mylower(fb.ff_name);
		if ((pc = calloc(1, sizeof(struct ilist))) == NULL)
			fatal("Cannot allocate memory for ilist");
		pc->val = n;
		if (head == NULL)
			head = pc;
		else
			pl->next = pc;
		pl = pc;
	} while (findnext(&fb) == 0);
	if (head != NULL)
		pl->next = NULL;
	if (chdir("..") < 0)
		fatal(prd);
#else

	head = NULL;
	pl = NULL;
	if (findfirst(thtwildname, &fb, 0) == 0) do {
/*		strcpy(scrlist,"Theater file : ");	*/
/*		strcat(scrlist,"fb.ff_name");	*/
/*		errorbox(scrlist);	*/
		if (sscanf(fb.ff_name, "%d.tht", &n) != 1)
			continue;
		if (fb.ff_attrib&FA_RDONLY)
			chmod(fb.ff_name, S_IREAD|S_IWRITE);
		mylower(fb.ff_name);
		if ((pc = calloc(1, sizeof(struct ilist))) == NULL)
			fatal("Cannot allocate memory for ilist");
		pc->val = n;
		if (head == NULL)
			head = pc;
		else
			pl->next = pc;
		pl = pc;
	} while (findnext(&fb) == 0);
	if (head != NULL)
		pl->next = NULL;
#endif /* MVPCD */
	/*
	 *	Check for absent .tht file
	 */
	for (i = 0; i < npilots; i++) {
		pp = &pilots[i];
		for (j = pp->ntheaters-1; j >= 0; j--) {
			for (pc = head; pc != NULL; pc = pc->next)
				if (pc->val == pp->theatnos[j])
					break;
			if (pc == NULL)
				missing(pp, j);
			else
				pc->mark = 1;
		}
	}
	for (pl = head; pl != NULL; pl = pc) {
		if (!pl->mark)
			unlink(mkthname(pl->val));
		pc = pl->next;
		free(pl);
	}

	/*
	 *	Initialize hi-score list
	 */
	if (firsttime)
		xyprintf(1, inity++, "%s high scores...", initp);
	inithiscr();

	/*
	 *	Move .vga's and .pal's
	 */
	if (firsttime)
		xyprintf(1, inity++, "%s happy landing screens...", initp);
	p1 = "vga";
	p2 = "Sorry, I cannot initialize the directory `vga'";
	if (findfirst(p1, &fb, 0) != 0)
		mkdir(p1);
	else if (!(fb.ff_attrib&FA_DIREC)) {
		chmod(p1, S_IREAD|S_IWRITE);
		unlink(p1);
		if (mkdir(p1) < 0)
			fatal(p2);
	}
	chkdef(p1, "*.vga");
	chkdef(p1, "*.pal");
#ifdef JPEG
	if(firsttime) convjpgs(p1,"*.jpg");
#endif /* JPEG */

#ifdef	REGISTERED
	/*
	 *	Initialize demos
	 */
	if (firsttime)
		xyprintf(1, inity++, "%s demos...", initp);
	p1 = "dmo";
	p2 = "Sorry, I cannot initialize the demo directory `dmo'";
	if (findfirst(p1, &fb, 0) != 0)
		mkdir(p1);
	else if (!(fb.ff_attrib&FA_DIREC)) {
		chmod(p1, S_IREAD|S_IWRITE);
		unlink(p1);
		if (mkdir(p1) < 0)
			fatal(p2);
	}
	p2 = "*.dmo";
	chkdef(p1, p2);
	chkdef(p1, "demo.ndx");
	if (chdir(p1) < 0) {
		ndemos = 0;
		errorbox("Cannot chdir to demos directory `dmo'");
		goto end;
	}

	for (n = findfirst(p2, &fb, 0); !n; n = findnext(&fb)) {
		if (fb.ff_attrib&(FA_DIREC|FA_LABEL))
			continue;
		ndemos++;
	}
	if (ndemos == 0) {
		chdir("..");
		goto end;
	}
	if ((demos = (struct demot *)calloc(ndemos, sizeof(struct demot)))
								== NULL) {
		errorbox("Cannot allocate memory for demos");
		ndemos = 0;
		chdir("..");
		goto end;
	}
	for (i = 0; i < ndemos; i++) {
		demos[i].name[0] = '\0';
		demos[i].fname[0] = '\0';
		demos[i].theatno = -1;
		demos[i].descoffs = -1L;
	}
	i = 0;
	for (n = findfirst(p2, &fb, 0); !n; n = findnext(&fb)) {
		if (fb.ff_attrib&(FA_DIREC|FA_LABEL))
			continue;
		mylower(fb.ff_name);
		for (p1 = fb.ff_name; *p1 != '.' && *p1; p1++);
		if (*p1)
			*p1 = ' ';
		if (sscanf(fb.ff_name, "%s%s", demos[i].fname, buf) != 2 ||
							strcmp(buf, "dmo") != 0)
			continue;
		i++;
	}
	if (i != ndemos) {
		/* Weirdness ??? */
		ndemos = i;
	}
	if (ndemos == 0) {
		chdir("..");
		free(demos);
		goto end;
	}
	if ((fp = fopen("demo.ndx", "rb")) == NULL) {
		errorbox("Cannot open demo.ndx");
		ndemos = 0;
		free(demos);
		chdir("..");
		goto end;
	}
	n = 0;
	while (fngets(buf, 80, fp) != NULL) {
		if (buf[0] == 12) {
			n++;
			continue;
		}
		if (buf[0] == '\0')
			continue;
		for (i = 0; i < ndemos; i++)
			if (strcmp(buf, demos[i].fname) == 0)
				break;
		if (i == ndemos) {
			errorbox("demo %s.dmo (#%d in demo index) not present.",
									buf, n);
			while ((p1 = fngets(buf, 80, fp)) != NULL && *p1 != 12);
			if (p1 == NULL)
				break;
			if (buf[0] == 12)
				continue;
		}
		if ((p1 = fngets(buf, 80, fp)) == NULL || buf[0] == 12) {
			errorbox("bad demo name %s (#%d in demo index)",buf, n);
			strcpy(demos[i].name, demos[i].fname);
			demos[i].theatno = -1;
			demos[i].descoffs = -1L;
			if (p1 == NULL)
				break;
			if (buf[0] == 12)
				continue;
		}
		buf[21] = '\0';
		buf[22] = '\0';
		strcpy(demos[i].name, buf);
		if ((p1 = fngets(buf, 80, fp)) == NULL || buf[0] == 12) {
			errorbox("bad demo theater name %s (#%d in demo index)",
									buf, n);
			if (p1 == NULL)
				break;
			if (buf[0] == 12)
				continue;
		}
		for (j = 0; j < ntheaters; j++)
			if (strcmp(theaters[j].fname, buf) == 0)
				break;
		if (j == ntheaters) {
			errorbox(
			  "unknown demo theater name %s (#%d in demo index)",
									buf, n);
			while ((p1 = fngets(buf, 80, fp)) != NULL && *p1 != 12);
			if (p1 == NULL)
				break;
			if (buf[0] == 12)
				continue;
		}
		demos[i].theatno = j;
		demos[i].descoffs = ftell(fp);
		while ((p1 = fngets(buf, 80, fp)) != NULL && *p1 != 12);
		if (p1 == NULL)
			break;
		if (buf[0] == 12)
			continue;
	}
	for (i = 0; i < ndemos; ) {
		if (demos[i].theatno != -1) {
			i++;
			continue;
		}
		errorbox("demo %s.dmo has no entry in demo index",
								demos[i].fname);
		ndemos--;
		for (j = i; j < ndemos; j++)
			demos[j] = demos[j+1];
	}
	qsort(demos, ndemos, sizeof(struct demot), cmpdemo);
	chdir("..");
	fclose(fp);
end:
	;
#endif	/* REGISTERED */
}


#ifdef musicflag
#ifdef musicmenu
void openmusic()
{
	int i;
	FILE *fp,*fp1;
	char buf[80];
	char buf1[80];

	extern char *rolfiles[MAXROLFILES];
	extern	int numrolfiles;

	numrolfiles=0;
	if ((fp = fopen("rolfiles.txt", "rb")) != NULL) {
	  i=0;
	  while((fgets(buf,80,fp)!= NULL) && (i<MAXROLFILES)) {
	    if(buf[0] != ';') { 
	      if(strlen(buf)>2) buf[strlen(buf)-2]=0;  /* remove cr character */
	      if(strcmpi( &buf[strlen(buf)-3],"rol")==0) {
	          if ((fp1 = fopen(buf, "rb")) == NULL) {
                    sprintf(buf1,"Music file %s not found",buf);
		    errorbox(buf1);
		    continue;
		  }
		  else
                    fclose(fp1);
	          rolfiles[i]=(char *) malloc(strlen(buf)+1);
	          if(rolfiles[i]==NULL) { 
		    errorbox("Can't alloc ROL ptrs");
		    break;
	          }
	        else {
		  strcpy(rolfiles[i],buf);
	          i=i+1;
                }
	      }
	    }
	  }
	  numrolfiles=i;
	fclose(fp);
	}
}
#endif
#endif

int	cmptheater(p1, p2)
	struct theatert	*p1, *p2;
{
	return(strcmp(p1->name, p2->name));
}

#ifdef	REGISTERED
int	cmpdemo(p1, p2)
	struct demot	*p1, *p2;
{
	return(stricmp(p1->name, p2->name));
}
#endif	/* REGISTERED */

static void	chkdef(s1, s)
	char	*s1, *s;
{
	char	buf[LINELEN];
	int	n, fd;
	struct ffblk	fb;
	extern int	ropen();

	for (n = findfirst(s, &fb, 0); !n; n = findnext(&fb)) {
		if (fb.ff_attrib&(FA_DIREC|FA_LABEL))
			continue;
		sprintf(buf, "%s\\%s", s1, fb.ff_name);
		if (rename(fb.ff_name, buf) < 0) {
			fd = ropen(buf);
			if (fd < 0) {
				errorbox("Cannot rename %s to %s",
							fb.ff_name,buf);
			}
			close(fd);
		}
	}
}
#ifdef JPEG
static void	convjpgs(s1, s)
	char	*s1, *s;
{
	extern int	inity;
	char	buf[LINELEN];
	int	n, fd;
	struct ffblk	fb;
	extern int	ropen();

	inity++;
	xyprintf(1, inity++, "Note: File decompression occurs only the first time Corncob 3D is run.",fb.ff_name);
	for (n = findfirst(s, &fb, 0); !n; n = findnext(&fb)) {
		if (fb.ff_attrib&(FA_DIREC|FA_LABEL))
			continue;
		sprintf(buf, "%s\\%s", s1, fb.ff_name);
		strcpy(buf+(strlen(buf)-3),"vga");
		xyprintf(1, inity++, "Uncompressing image file %s......",fb.ff_name);
		if (spawnl(P_WAIT,"jvcc.exe","jvcc.exe",fb.ff_name, buf,NULL)
			 == 0) {
			fd = ropen(buf);
			if (fd < 0) {
				errorbox("Cannot convert Jpeg %s to %s",
							fb.ff_name,buf);
			}
			else /* if everything went OK, delete original JPEG */
			  unlink(fb.ff_name);
			close(fd);
		}
		else errorbox("Could not decompress %s",fb.ff_name);
	}
}
#endif /* JPEG */


static void	mylower(s)
	char	*s;
{
	for (; *s; s++)
		if (*s >= 'A' && *s <= 'Z')
			*s += 'a' - 'A';
}

void	fatal(s) {
	extern void	boxsetup(), saybyebye();

	if (!textoinited)
		inito();
	beep();
	color(LIGHTRED, WHITE);
	boxsetup(10, 9, 60, 5, 0);
	color(BLUE, WHITE);
	xyprintf(17, 10, s);
	tkyo(18, 12);
	saybyebye();
}

void	missing(pp, n)
	struct pilot	*pp;
	int	n;
{
	int	i;

	beep();
	simplesetup(10, 9, 60, 6, "Theater missing", LIGHTRED, WHITE, BLUE);
	cursor(17, 10);
	sput("Pilot: ");
	outpname(pp);
	xyprintf(17, 11, "Theater number %d is missing!", n);
	if (pp->curtheater == n)
		pp->curtheater = (unsigned char)0x80;
	for (i = n+1; i < pp->ntheaters; i++) {
		pp->theatnos[i-1] = pp->theatnos[i];
		if (pp->curtheater == i)
			pp->curtheater--;
	}
	pp->ntheaters--;
	tkyo(18, 13);
}

#ifndef MVPCD
char	*savefile = "moag.cfg";
#else
extern	char	wpath[80];
extern	char	scrlist[80];
#endif

	/* moag.c */
extern int	wzflag, kbflag, jsflag;
extern int	choice;

	/* roster.c */
extern int	npilots;
extern struct pilot	pilots[], trainee, *curpilot;
extern int	roschoice;

	/* misn.c */
extern int	ntheaters;
extern struct theatert	*theaters;
extern struct thnamlist	*thhead;
extern int	mischoice, xrp, yrp,
		stoutthno, noutths, outthflag, stoutptno, noutpts, outptflag;

	/* ryo.c */
extern int	ryochoice, stlch, wckch, ngetflag, nstch;

	/* go.c */
extern int	timeflag, towerno;
extern struct pilot	curthpilot;
extern struct theatert	curtheater;

	/* screenma.c */
extern int	menux, menuy, menuwid, spoof;

	/* texto.c */
extern unsigned int 	shft_attr;
extern unsigned int 	forecolor, backcolor, isbold;

	/* report.c */
extern int	newplane;

	/* kbjs.c */
extern int	kjchoice, kbflag, jsflag, njflag;

	/* demo.c */
extern struct demot	*demos;
extern int	ndemos, stoutdmno, noutdms, outdmflag, rundemo;

	/* init.c */
extern long	instdate;

static FILE	*fp;
void	savestate() {
	int	a;
	char	buf[LINELEN];
	struct thnamlist	*pth;
	extern void	iwrite();
	extern long	time();
	extern unsigned int	coreleft();
#ifdef musicflag
	extern char musicwanted;
#endif

	if ((fp = fopen(savefile, "wb")) == NULL) {
		sprintf(buf, "Cannot creat %s for writing  cl=%u",
					savefile, coreleft());
		fatal(buf);
	}

	/* go.c */		/* put go.c and roster.c first to make
					iscore easier */
	iwrite(timeflag);
	iwrite(towerno);
	fwrite(&curthpilot, sizeof(struct pilot), 1, fp);
	fwrite(&curtheater, sizeof(struct theatert), 1, fp);

	/* roster.c */
	iwrite(npilots);
	if (curpilot == &trainee)
		a = -1;
	else
		a = curpilot - pilots;
	iwrite(a);
	fwrite(pilots, sizeof(struct pilot), npilots, fp);
	fwrite(&trainee, sizeof(struct pilot), 1, fp);
	iwrite(roschoice);

	/* moag.c */
	iwrite(wzflag);
	iwrite(kbflag);
	iwrite(jsflag);
	iwrite(choice);

	/* misn.c */
	iwrite(ntheaters);
	fwrite(theaters, sizeof(struct theatert), ntheaters, fp);
	a = 0;
	for (pth = thhead; pth != NULL; pth = pth->next)
		a++;
	iwrite(a);
	for (pth = thhead; pth != NULL; pth = pth->next)
		fwrite(pth, sizeof(struct thnamlist), 1, fp);
	/* theatlist */
	iwrite(mischoice);
	iwrite(xrp);
	iwrite(yrp);
	iwrite(stoutthno);
	iwrite(noutths);
	iwrite(outthflag);
	iwrite(stoutptno);
	iwrite(noutpts);
	iwrite(outptflag);

	/* ryo.c */
	iwrite(ryochoice);
	iwrite(stlch);
	iwrite(wckch);
	iwrite(ngetflag);
	iwrite(nstch);

	/* screenma.c */
	iwrite(menux);
	iwrite(menuy);
	iwrite(menuwid);
	iwrite(spoof);
	time(&curpilot->date);				/* Misdirection */

	/* reg.c */
	iwrite(isreg);
	fwrite(&chksum, sizeof(long), 1, fp);
	fwrite(&regrcode, sizeof(long), 1, fp);

	/* report.c */
	iwrite(newplane);

	/* kbjs.c */
	iwrite(kjchoice);
	iwrite(kbflag);
	iwrite(jsflag);
	iwrite(njflag);

	/* demo.c */
	iwrite(ndemos);
	fwrite((void *)demos, sizeof(struct demot), ndemos, fp);
	iwrite(stoutdmno);
	iwrite(noutdms);
	iwrite(outdmflag);
	iwrite(rundemo);
#ifdef musicflag
	iwrite((int) musicwanted);
#endif

	/* init.c */
	fwrite((void *)&instdate, sizeof(long), 1, fp);

	fclose(fp);
}

void	iwrite(a)
	int	a;
{
	fwrite(&a, sizeof(int), 1, fp);
}

void	reinstate() {
	int	a, b;
	char	buf[LINELEN];
	extern void	iread(), *calloc();
#ifdef musicflag
	extern char musicwanted;
#endif

	if ((fp = fopen(savefile, "rb")) == NULL) {
		sprintf(buf, "Cannot open %s for reading", savefile);
		fatal(buf);
	}

	/* go.c */
	iread(&timeflag);
	iread(&towerno);
	fread(&curthpilot, sizeof(struct pilot), 1, fp);
	fread(&curtheater, sizeof(struct theatert), 1, fp);

	/* roster.c */
	iread(&npilots);
	iread(&b);
	if (b == -1)
		curpilot = &trainee;
	else
		curpilot = &pilots[b];
	fread(pilots, sizeof(struct pilot), npilots, fp);
	fread(&trainee, sizeof(struct pilot), 1, fp);
	curryo = &curpilot->ryo;
	iread(&roschoice);

	/* moag.c */
	iread(&wzflag);
	iread(&kbflag);
	iread(&jsflag);
	iread(&choice);

	/* misn.c */
	iread(&ntheaters);
	theaters = (struct theatert *)calloc(ntheaters,sizeof(struct theatert));
	if (theaters == NULL && ntheaters > 0)
		fatal("Cannot allocate memory for theater table.");
	fread(theaters, sizeof(struct theatert), ntheaters, fp);

	iread(&a);
	thhead = (struct thnamlist *)calloc(a, sizeof(struct thnamlist));
	if (thhead == NULL && a > 0)
		fatal("Cannot allocate memory to construct theater list.");
	fread(thhead, sizeof(struct thnamlist), a, fp);
	for (b = 0; b < a; b++) {
		if (b == a-1)
			thhead[b].next = NULL;
		else
			thhead[b].next = &thhead[b+1];
	}
	iread(&mischoice);
	iread(&xrp);
	iread(&yrp);
	iread(&stoutthno);
	iread(&noutths);
	iread(&outthflag);
	iread(&stoutptno);
	iread(&noutpts);
	iread(&outptflag);

	/* ryo.c */
	iread(&ryochoice);
	iread(&stlch);
	iread(&wckch);
	iread(&ngetflag);
	iread(&nstch);

	/* screenma.c */
	iread(&menux);
	iread(&menuy);
	iread(&menuwid);
	iread(&spoof);

	/* reg.c */
	iread(&isreg);
	fread(&chksum, sizeof(long), 1, fp);
	fread(&regrcode, sizeof(long), 1, fp);

	/* report.c */
	iread(&newplane);

	/* kbjs.c */
	iread(&kjchoice);
	iread(&kbflag);
	iread(&jsflag);
	iread(&njflag);

	/* demo.c */
	iread(&ndemos);
	demos = (struct demot *)calloc(ndemos, sizeof(struct demot));
	if (demos == NULL && ndemos > 0)
		fatal("Cannot allocate memory to construct demo list.");
	fread((void *)demos, sizeof(struct demot), ndemos, fp);
	iread(&stoutdmno);
	iread(&noutdms);
	iread(&outdmflag);
	iread(&rundemo);
#ifdef musicflag
	iread(&a);
	musicwanted=(unsigned char) a;
#endif

	/* init.c */
	fread((void *)&instdate, sizeof(long), 1, fp);

	fclose(fp);
	unlink(savefile);
}

void	iread(p)
	int	*p;
{
	fread(p, sizeof(int), 1, fp);
}
