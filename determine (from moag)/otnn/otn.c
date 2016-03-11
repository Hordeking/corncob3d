#include	<stdio.h>
#include	<fcntl.h>
#include	"p.h"

struct pilot	opilots[MAXPILOTS], otrainee, *ocurpilot;
struct pilot	mpilots[MAXPILOTS], ntrainee, *ncurpilot;

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
	struct pilot	*o;
	struct pilot	*n;

	extern int	_fmode;
	extern void	initscore(), savescore();

	_fmode = O_BINARY;
	initscore();
	printf("npilots=%d\n", npilots);
	for (i = 0; i < npilots; i++) {
		o = &opilots[i];
		n = &mpilots[i];
		pcopy(n, o);

		if (strcmp(n->name, "Surfer") == 0) {
			printf("\nFIXING Surfer\n\n");
			n->stockade = 0L;
			n->nresurected = 0;
		}

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

struct theatert	tt;
struct pilot	pp;
dotheats(o, n)
	struct pilot	*o;
	struct pilot	*n;
{
	struct pilot	op;
	struct pilot	np;
	struct theatert	ot;
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
		op = pp;
		np = pp;
		ot = tt;
		nt = tt;
		read(fdi, &ot, sizeof(struct theatert));
		read(fdi, &op, sizeof(struct pilot));
		tcopy(&nt, &ot);
		pcopy(&np, &op);

		if (strcmp(np.name, "Surfer") == 0) {
			printf("\nFIXING Surfer\n\n");
			np.stockade = 0L;
			np.nresurected = 0;
		}

		sprintf(buf2, "tht\\%d.thn", o->theatnos[i]);
		if ((fdo = wopen(buf2)) < 0) {
			printf("Cannot open %s to write\n", buf2);
			close(fdi);
			continue;
		}
		write(fdo, &nt, sizeof(struct theatert));
		write(fdo, &np, sizeof(struct pilot));
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
	struct pilot	*n;
	struct pilot	*o;
{
	int	j, i;

	for (i = 0; i < DRPWID+2; i++)
		n->name[i] = o->name[i];
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
	n->stockade = o->stockade;
}

tcopy(n, o)
	struct theatert	*n;
	struct theatert	*o;
{
	int	i;
	for (i = 0; i < THEATNAMLEN+1; i++)
		n->name[i] = o->name[i];
	for (i = 0; i < 10; i++)
		n->fname[i] = o->fname[i];
	n->nstoredplanes = o->nstoredplanes;
	n->nobjectives = o->nobjectives;
	n->ncobjectives = o->ncobjectives;
	n->isreq = o->isreq;
	n->completed = o->completed;
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
