#include	<stdio.h>
#include	<process.h>
#include	<graphics.h>
#include	<dos.h>
int	ace;

#define	GRAPHMODE	0x10
#define	TEXTMODE	0x3
#define	MODE		GRAPHMODE

main(argc, argv)
	char	**argv;
{
	int	i, c;
	char	*argvec[3], buf[128], *p;
	FILE	*fp;
	union REGS	regs;

	regs.h.ah = 0;
	regs.h.al = (unsigned char)MODE;
	int86(0x10, &regs, &regs);

	printf("\n\n\n\n\n");
	for (i = 0; i < argc; i++) {
		printf("            argv[%d] = %s\n", i, argv[i]);
		if (argv[i][0] == '-') {
			for (p = &argv[i][1]; *p; p++) {
				if (*p == 'f') {
					ace = 1;
					printf("ACE!\n");
				}
			}
		}
	}
	i = 0;
	if ((fp = fopen("3.cmd", "rb")) != NULL) {
		while (fgets(buf, 128, fp) != NULL) {
			buf[strlen(buf)-1] = '\0';
			printf("            cmd[%d] = %s\n", i, buf);
			if (buf[0] == '-') {
				for (p = &buf[1]; *p; p++) {
					if (*p == 'f') {
						ace = 1;
						printf("ACE!\n");
					}
				}
			}
			i++;
		}
	}
	tce();
/*	system("mapmem"); */
	while (1) {
		c = getch();
		if (c == 0)
			c = getch();
		else
			break;
		if (c == 59)
			spawnl(P_WAIT, "tower.exe", "tower.exe", "-_-", "-1",0);
		else if (c == 61)
			spawnl(P_WAIT, "iscore.exe", "iscore.exe", "-_-", 0);
		else if (c == 60)
			spawnl(P_WAIT, "tower.exe", "tower.exe", "-_-", "0", 0);
		else
			break;
	}

 	exit(0);
}

#include	<stdio.h>

#define	NEVERLEFT	0x1
#define	KILLED		0x2
#define	LANDEDHOME	0x4
#define	CAPTURED	0x8
#define	ATHOME		0x10
#define	EJECTED		0x20
#define	CRASHATHOME	0x40
#define	HITBLDG		0x80
#define	RESCUED		0x100
#define	CRASHNOTHOME	0x200
#define	LOSTINSPACE	0x400
#define	KILLEDBYENEMY	0x800
#define	ABORT		0x4000
struct stuffrm3 {
	unsigned int	flag;		/* What happened */
	int	deathcode;		/* Number of planes flown */
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
} st3;

tce() {
	int	ce;
	FILE	*fp;
	long	chksum;			/* Rom check-sum */
	int	r, i;
	extern long	time();

	if ((fp = fopen("3.ce", "r")) == NULL)
		ce = 0;
	else {
		fscanf(fp, "%d", &ce);
		fclose(fp);
	}

	if ((fp = fopen("3.res", "wb")) == NULL) {
		fprintf(stderr, "Barf\n");
		exit(1);
	}

	r = time(NULL);
	srand((unsigned)r);

	st3.flag = 0;
	st3.flag |= LANDEDHOME|ATHOME; 			     /* What happened */
	if (ace)
		st3.flag |= LOSTINSPACE;			/**/
/*	st3.flag |= CAPTURED;				/**/
/*	st3.flag |= KILLED;				/**/
/*	st3.flag |= KILLEDBYENEMY;			/**/
	st3.deathcode = 5;			    /* Number of planes flown */
	st3.planeslost = 0;			     /* Number of planes lost */
	st3.tickskilled = mrand(50);		    /* Number of ticks killed */
	for (i = 0; i < 8; i++) {
		st3.dbkilled[i] = mrand(12);  /* Number of death-balls killed */
		st3.aaakilled[i] = mrand(3);	/* Number of aaa-sites killed */
		st3.shckilled[i] = mrand(12);/*Number of shopping-carts killed*/
		st3.siteskilled[i] = mrand(7);	    /* Number of sites killed */
	}
#ifdef	LOW
	st3.tickskilled = mrand(15);		    /* Number of ticks killed */
	for (i = 0; i < 8; i++) {
		st3.dbkilled[i] = mrand(2);  /* Number of death-balls killed */
		st3.aaakilled[i] = mrand(2);	/* Number of aaa-sites killed */
		st3.shckilled[i] = mrand(2);/*Number of shopping-carts killed*/
		st3.siteskilled[i] = mrand(2);	    /* Number of sites killed */
	}
#endif
/*	st3.objectives = 30;		 /* Number of objectives accomplished */
	st3.objectives = mrand(10);	 /* Number of objectives accomplished */
/*	st3.friendkilled = mrand(6)/5;	       /* Number of friendlies killed */
	st3.friendkilled = 0;		       /* Number of friendlies killed */
	st3.totaldamage = 0;				/* Total plane damage */
	for (i = 0; i < 8; i++) {
		st3.dbonme[i] = mrand(3);      /* Number of death-balls on me */
		st3.aaaonme[i] = mrand(3);	 /* Number of aaa-sites on me */
	}
	for (i = 0; i < 8; i++)
		st3.orbskilled[i] = mrand(5);	     /* Number of orbs killed */
#ifdef	LOW
	for (i = 0; i < 8; i++) {
		st3.dbonme[i] = mrand(2);      /* Number of death-balls on me */
		st3.aaaonme[i] = mrand(2);	 /* Number of aaa-sites on me */
	}
	for (i = 0; i < 8; i++)
		st3.orbskilled[i] = mrand(2);	     /* Number of orbs killed */
#endif
	st3.seconds = mrand(10000);			/* Seconds in the air */

	fwrite(&st3, sizeof(struct stuffrm3), 1, fp);
	chksum = 18436572;
	fwrite(&chksum, sizeof(long), 1, fp);
	fwrite(&ce, sizeof(int), 1, fp);
	fclose(fp);
}

int	mrand(n)
	int	n;
{
	extern int	rand();
	int	r;
	long	t;

	t = (long)rand()*n;
	t /= 32767;
	r = (int)t;
	return(r);
}
