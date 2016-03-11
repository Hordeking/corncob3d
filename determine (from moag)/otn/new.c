#include	<stdio.h>
#include	<io.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<string.h>
#include	"n.h"

static char	*pilotlist = "pilot.scr";
extern struct npilot	mpilots[], ntrainee, *ncurpilot;
extern int	npilots;
	/* reg.c */
extern int	isreg;			/* Is program registered flag */
extern long	chksum;			/* Rom check-sum (from 3) */
extern long	regrcode;		/* registration reply code (from PIE) */
extern long	instdate;			/* Install date */

static unsigned int	seed;

void	savescore() {
	int	sn, fd, i;
	unsigned char	abyte;
	long	lastflown, now;
	extern int	kbflag, njflag, wopen(), mwrite();
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

	mwrite(fd, (void *)&npilots, sizeof(int));
	for (i = 0; i < npilots; i++) {
		mwrite(fd, (void *)&mpilots[i], sizeof(struct npilot));
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
	unsigned char	*pp, buf[sizeof(struct npilot)], *pb, mask;
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

