#include	<stdio.h>
#include	<fcntl.h>
#include	"n.h"

struct opilot {
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
};

struct otheatert {
	char	name[THEATNAMLEN+1];
	char	fname[10];
	int	nstoredplanes;
	int	nobjectives;
	int	ncobjectives;
};

struct opilot	opilots[MAXPILOTS], otrainee, *ocurpilot;
struct npilot	mpilots[MAXPILOTS], ntrainee, *ncurpilot;

int	npilots;
int	isreg;			/* Is program registered flag */
long	chksum;			/* Rom check-sum (from 3) */
long	regrcode;		/* registration reply code (from PIE) */
long	instdate;			/* Install date */

int	kbflag, njflag, timeflag;
struct ryot	defryo;

char	*prog;
main () {
	int	i, j;
	struct opilot	*o;
	struct npilot	*n;

	extern int	_fmode;
	extern void	initscore(), savescore();

	_fmode = O_BINARY;
	initscore();
	printf("npilots=%d\n", npilots);
	for (i = 0; i < npilots; i++) {
		o = &opilots[i];
		n = &mpilots[i];
		pcopy(n, o);
		printf("pilot %d (:%s:)\n", i, n->name);
		printf("nmission=%d  ", n->nmission);
		printf("nres=%d  ", n->nresurected);
		printf("ntheaters=%d  ", n->ntheaters);
		printf("curtheater=%d  ", n->curtheater);
		printf("rank=%d  ", n->rank);
		printf("\ndoing theaters...\n");
		dotheats(o, n);
		printf("\n");
	}
	savescore();
}

dotheats(o, n)
	struct opilot	*o;
	struct mpilot	*n;
{
	struct opilot	op;
	struct npilot	np;
	struct otheatert	ot;
	struct theatert	nt;
	int	i, fdi, fdo;
	char	buf1[32], buf2[32], buf[128];

	for (i = 0; i < o->ntheaters; i++) {
		printf("%d:%d\n", i, o->theatnos[i]);
		sprintf(buf1, "tht\\%d.tht", o->theatnos[i]);
		if ((fdi = ropen(buf1)) < 0) {
			printf("Cannot open %s to read\n", buf1);
			continue;
		}
		read(fdi, &ot, sizeof(struct otheatert));
		read(fdi, &op, sizeof(struct opilot));
		tcopy(&nt, &ot);
		pcopy(&np, &op);
		sprintf(buf2, "tht\\%d.thn", o->theatnos[i]);
		if ((fdo = wopen(buf2)) < 0) {
			printf("Cannot open %s to write\n", buf2);
			close(fdi);
			continue;
		}
		write(fdo, &nt, sizeof(struct theatert));
		write(fdo, &np, sizeof(struct npilot));
		fdcopy(fdo, fdi);
		close(fdo);
		close(fdi);
		unlink(buf1);
		rename(buf2, buf1);
	}
}

void	errorbox(s)
	char	*s;
{
	printf("%s\n", s);
	exit(1);
}


pcopy(n, o)
	struct npilot	*n;
	struct opilot	*o;
{
	int	j;

	strcpy(n->name, o->name);
	n->flag = o->flag;
	n->nmission = o->nmission;
	n->nresurected = o->nresurected;
	n->lastmiss = o->lastmiss;
	n->bestmiss = o->bestmiss;
	n->bestofeach = o->bestofeach;
	n->totalmiss = o->totalmiss;
	n->ryo = o->ryo;
	n->ntheaters = o->ntheaters;
	n->curtheater = o->curtheater;
	for (j = 0; j < MAXTHEATERS; j++)
		n->theatnos[j] = o->theatnos[j];
	n->date = o->date;
	n->rank = o->rank;
	for (j = 0; j < MAXMEDALS; j++)
		n->nmedals[j] = o->nmedals[j];
	n->stockade = 0;
}

tcopy(n, o)
	struct theatert	*n;
	struct otheatert	*o;
{
	strcpy(n->name, o->name);
	strcpy(n->fname, o->fname);
	n->nstoredplanes = o->nstoredplanes;
	n->nobjectives = o->nobjectives;
	n->ncobjectives = o->ncobjectives;
	n->isreq = 0;
	n->completed = 0;
}

fdcopy(fo, fi)
	int	fo, fi;
{
	char	*p;
	extern void	*malloc(), free();
	unsigned int	u, n;
	extern unsigned int	coreleft();

	u = coreleft();
	u -= 18;
	u = (u/1024)*1024;
	while ((p = (char *)malloc(u)) == NULL) {
		if (u < 1024) {
			errorbox("Cannot alloc p in fdcopy");
			return;
		}
		u -= 1024;
	}

	while ((n = read(fi, p, u)) > 0)
		write(fo, (void *)p, n);
	free(p);
}
