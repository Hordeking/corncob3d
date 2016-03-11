#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<string.h>
#include	"p.h"

static char	*pilotlist = "pilot.scr";
extern struct pilot	opilots[], otrainee, *ocurpilot;
extern int	npilots;
	/* reg.c */
extern int	isreg;			/* Is program registered flag */
extern long	chksum;			/* Rom check-sum (from 3) */
extern long	regrcode;		/* registration reply code (from PIE) */
extern long	instdate;			/* Install date */
long	lastflown;

unsigned int	seed;
void	initscore() {
	long	now;
	unsigned int	m;
	int	i, fd;
	unsigned char	abyte;
	char	buf[128];

	extern int	kbflag, njflag, ropen(), timeflag, mread();
	extern struct ryot	defryo;
	extern long	time();
#ifdef	__TURBOC__
	extern void	srand();
#else
	extern long	_seed;
#endif

	if ((fd = ropen(pilotlist)) < 0) {
		errorbox("Cannot open pilotlist");
		return;
	}
	if (read(fd, (void *)&i, sizeof(int)) != sizeof(int) || i != SN) {
		errorbox("Cannot read pilotlist");
		close(fd);
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
		sprintf(buf, "Error 1, reading file %s", pilotlist);
		errorbox(buf);
		close(fd);
		return;
	}
	mread(fd, (void *)&abyte, 1);
	instdate += ((long)abyte);
	mread(fd, (void *)&abyte, 1);
	lastflown += ((long)abyte)<<16;
	if (mread(fd, (void *)&njflag, sizeof(int)) != sizeof(int)) {
		sprintf(buf, "Error 2, reading file %s", pilotlist);
		errorbox(buf);
		close(fd);
		return;
	}
	mread(fd, (void *)&abyte, 1);
	isreg = 0;
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
	isreg += ((int)abyte)<<8;
	if (mread(fd, (void *)&npilots, sizeof(int)) != sizeof(int)) {
		errorbox("error reading npilots");
		return;
	}
	for (i = 0; i < npilots; i++) {
		if ((m=mread(fd, (void *)&opilots[i], sizeof(struct pilot)))
				!= sizeof(struct pilot)) {
			sprintf(buf, "error (%d != %d) reading pilot %d", m,
						sizeof(struct pilot), i);
			errorbox(buf);
		}
		if (opilots[i].flag&ACTIVE)
			ocurpilot = &opilots[i];
		if (i == 0) {
			mread(fd, (void *)&abyte, 1);
			isreg += abyte;
			if ((isreg != ISREGVAL) && (lastflown > now))
				timeflag = TIMEFLAGVAL;
		}
		if (i == MAXPILOTS-1)
			break;
	}
	if (i == 0) {
		mread(fd, (void *)&abyte, 1);
		isreg += abyte;
		if ((isreg != ISREGVAL) && (lastflown > now))
			timeflag = TIMEFLAGVAL;
	}
	npilots = i;
	if (mread(fd, &m, 2) == 2) {
		errorbox("Error, expected EOF not found");
	}
	else
		printf("Got EOF as expected\n");
	close(fd);
}

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

