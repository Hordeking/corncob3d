/*#define	TEST	1			/**/
#include	<stdio.h>
#include	<fcntl.h>
#include	"moag.h"
#include	"texto.h"

struct pilot	pilot, *curpilot = &pilot;
struct pilot	curthpilot;
struct theatert	curtheater;

#ifndef	MVPCD
char	*psavefile = "moag.cfg";
char	pfileres[] = "3.res";
#else
char	*vsavefile = "moag.cfg";
char	vfileres[] = "3.res";
char	psavefile[40] = "moag.cfg";
char	pfileres[40] = "3.res";
#endif

main(argc, argv)
	int	argc;
	char	**argv;
{
#ifdef	TEST
	char	buf[LINELEN];
	int	w, s, y, m2, m;
	long	m1;
	struct stuffrm3	*p3;
#endif
	struct pilot	*ppt, *ppc;
	struct pilotmiss	pmiss, *pm = &pmiss;

	extern void	getsave(), getres(), tally(), pmissrep(), exit();
	extern int	_fmode, tkyo();

	if (argc < 2 || strcmp(argv[1], "-_-") != 0) {
		printf("\n");
		printf(
		  "This program is designed to be called via `corncob.bat'.\n");
		printf(
		  "Please do not call it directly.  Instead, type `corncob'\n");
		printf("at the DOS prompt.\n");
		printf("\n");
		exit(1);
	}

#ifdef	MVPCD
	if(argc > 2) {
		strcpy(psavefile,argv[2]);
		strcpy(pfileres,argv[2]);
		strcat(psavefile,vsavefile);	/* Put path stuff on */
		strcat(pfileres,vfileres);
	}
#endif	\* MVPCD *\

	_fmode = O_BINARY;
	inito();

	getsave();
	getres(&(pmiss.st3));

	pm->st3.flag |= (LANDEDHOME|ATHOME);
	pm->st3.flag &= ~NEVERLEFT;

	ppc = curpilot;
	if (curpilot->curtheater&0x80)
		ppt = NULL;
	else
		ppt = &curthpilot;

	if (ppt != NULL)
		tally(ppt, pm);
	tally(ppc, pm);

#ifdef	TEST
	y = 1;
	p3 = &pm->st3;
	m1 = 0;
	xyprintf(1, y++, "PRINCIPAL KILLS:");

	xyprintf(1, y++,
	"objectives   = %3d                        * 15 = %d",
					p3->objectives, 15*p3->objectives);
	m1 += 15*p3->objectives;
	xyprintf(1, y++,
	"ticks                                          = %d",
							p3->tickskilled);
	m1 += p3->tickskilled;

	xyprintf(1, y++, "aaa          = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->aaakilled[w];
		sprintf(buf, "%3d ", p3->aaakilled[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m1 += s;

	xyprintf(1, y++, "sites        = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->siteskilled[w];
		sprintf(buf, "%3d ", p3->siteskilled[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m1 += s;

	xyprintf(1, y++, "orbs         = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->orbskilled[w];
		sprintf(buf, "%3d ", p3->orbskilled[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m1 += s;
	xyprintf(1, y++,
	"TOTAL ---------------------------------------- = %ld", m1);

	m = 0;
	y++;
	xyprintf(1, y++, "DIFFICULTY FACTOR:");
	xyprintf(1, y++, "dballs       = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->dbonme[w]*(w+1);
		sprintf(buf, "%3d ", p3->dbonme[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m += s;

	xyprintf(1, y++, "aaa          = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->aaaonme[w]*(w+1);
		sprintf(buf, "%3d ", p3->aaaonme[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m += s;
	xyprintf(1, y++,
	"TOTAL ---------------------------------------- = %d", m);

	y++;
	m2 = 0;
	xyprintf(1, y++, "SECONDARY KILLS:");
	xyprintf(1, y++, "dballs       = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->dbkilled[w]*(w+1);
		sprintf(buf, "%3d ", p3->dbkilled[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m2 += s;

	xyprintf(1, y++, "scarts       = ");
	s = 0;
	for (w = 0; w < NWICK; w++) {
		s += p3->shckilled[w]*(w+1);
		sprintf(buf, "%3d ", p3->shckilled[w]);
		sput(buf);
	}
	sprintf(buf, "= %d", s);
	sput(buf);
	m2 += s;
	xyprintf(1, y++,
	"TOTAL ---------------------------------------- = %d", m2);

	y += 2;
	xyprintf(10, y++, "mobf = 5 * %ld * (10 + %d) / 10  +  %d = %ld",
				m1, m, m2, 5L*m1*(10 + m)/10 + m2);
	tkyo(25, 24);
#endif
	if (ppt != NULL)
		pmissrep(ppt, 3, curtheater.name);
	else
		pmissrep(ppc, 2, NULL);
	tkyo(25, 24);
	setblink(1);
	exito();
	exit(0);
}

char	*cant = "Iscore cannot open %s for reading (errno=%d)";
void	getsave() {
	FILE	*fp;
	int	npilots, b;
	char	*p, buf[LINELEN];

	extern int	errno;
	extern void	fatal();

	errno = 0;
	if ((fp = fopen(psavefile, "rb")) == NULL) {
		sprintf(buf, cant, psavefile, errno);
		fatal(buf);
	}

	p = "error reading from psavefile";
	if (fseek(fp, 2L*sizeof(int), SEEK_CUR) < 0 ||
	    fread(&curthpilot, sizeof(struct pilot), 1, fp) != 1 ||
	    fread(&curtheater, sizeof(struct theatert), 1, fp) != 1 )
		fatal(p);

	if (fread(&npilots, sizeof(int), 1, fp) != 1 ||
	    fread(&b, sizeof(int), 1, fp) != 1 )
		fatal(p);

	if (b >= npilots)
		fatal("Fatal confusion reading psavefile");

	if (b == -1) {
		/* curpilot = &trainee; */
		if (fseek(fp,(long)npilots*sizeof(struct pilot),SEEK_CUR) < 0 ||
		    fread(curpilot, sizeof(struct pilot), 1, fp) != 1)
			fatal(p);
	}
	else {
		/* curpilot = &pilots[b]; */
		if (b > 0)
			if (fseek(fp,((long)b)*sizeof(struct pilot),SEEK_CUR)
									< 0)
				fatal(p);
		if (fread(curpilot, sizeof(struct pilot), 1, fp) != 1)
			fatal(p);
	}
	fclose(fp);
}

void	getres(p3)
	struct stuffrm3	*p3;
{
	FILE	*fp;
	char	buf[LINELEN];

	extern void	fatal();
	extern int	errno;

	errno = 0;
	if ((fp = fopen(pfileres, "rb"))  == NULL) {
		sprintf(buf, cant, pfileres, errno);
		fatal(buf);
	}

	if (fread((void *)p3, sizeof(struct stuffrm3), 1, fp) != 1)
		fatal("Cannot read info from 3.res");
	fclose(fp);
}

void	fatal(s)
	char	*s;
{
	extern void	exit();

	xyprintf(3, 10, s);
	tkyo(4, 12);
	exito();
	exit(1);
}

int	tkyo(x, y)
	int	x, y;
{
	extern void	flushin();

	flushin();
	xyprintf(x, y, "Type almost any key to continue...");
	return(spgetch());
}

extern struct rank	ranks[];

void	foutpname(pp)
	struct pilot	*pp;
{
	sput(ranks[pp->rank].rank);
	cput(' ');
	sput(pp->name);
}

