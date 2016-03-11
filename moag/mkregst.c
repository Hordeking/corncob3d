#include	<stdio.h>
#include	"moag.h"
#define		DEFSN	22

char	*infofile;
char	regstuff[] = "regstuff.c";
char	serialno[] = "sn.val";

main (argc, argv)
	char	**argv;
{
	FILE	*fpi, *fpo, *fps;
	long	now, sn;
	unsigned int	seed;
	int	nregnames, zero;
	char	buf[REGLINELEN], *pm, *p, mask, *psn;

	extern int	rand();
	extern long	time();
#ifdef	__TURBOC__
	extern void	srand();
#else
	extern long	_seed;
#endif
	extern void	exit();

	if (argc == 1) {
		fprintf(stderr, "Usage:  %s username-file\n", argv[0]);
		exit(1);
	}
	infofile = argv[1];

	if ((fpi = fopen(infofile, "r")) == NULL) {
		fprintf(stderr, "Cannot open %s to read\n", infofile);
		exit(1);
	}
	if ((fpo = fopen(regstuff, "w")) == NULL) {
		fprintf(stderr, "Cannot open %s to read\n", regstuff);
		exit(1);
	}
	psn = "Defaulting to serial number = %ld\n";
	if ((fps = fopen(serialno, "r")) == NULL) {
		fprintf(stderr, "Cannot open serial number file %s to read\n",
								serialno);
		fprintf(stderr, "Defaulting to serial number = %ld\n", DEFSN);
		sn = DEFSN;
	}
	else if (fscanf(fps, "%ld", &sn) != 1 || sn <= DEFSN) {
		fprintf(stderr, "Bad read on serial number from %s\n",serialno);
		fprintf(stderr, psn, DEFSN);
		sn = DEFSN;
		fclose(fps);
	}

	time(&now);
	seed = (unsigned int)(now&0x7fff);
#ifdef	__TURBOC__
	srand(seed);
#else
	_seed = (long)seed;
#endif
	fprintf(fpo, "int\tregseed = %d;\n", seed);
	fprintf(fpo, "unsigned char\tregnames[][%d] = {\n", REGLINELEN);
	nregnames = 0;
	zero = 0;
	pm = &buf[REGLINELEN];
	if (fgets(buf, REGLINELEN, fpi) == NULL ||
	    fgets(buf, REGLINELEN, fpi) == NULL) {
		fprintf(stderr, "Error reading %s on copywrite info\n");
		exit(1);
	}
	printf("\n----------------------------------------\n");
	while (fgets(buf, REGLINELEN, fpi) != NULL) {
		fprintf(fpo, "\t{\n");
		for (p = buf; *p; p++) {
			if (*p == '\r' || *p == '\n') {
				*p = '\0';
				break;
			}
			putchar(*p);
			mask = (unsigned char)(rand()/128);
			*p = (unsigned char)(*p^mask);
			fprintf(fpo, "\t\t%d,\n", *p&0xff);
		}
		for (; p < pm; p++) {
			mask = (unsigned char)(rand()/128);
			*p = (unsigned char)(zero^mask);
			fprintf(fpo, "\t\t%d,\n", *p&0xff);
		}
		fprintf(fpo, "\t},\n");
		printf("\n");
		nregnames++;
	}
	printf("\n");
	printf("Assigned serial number = %ld\n", sn);
	printf("----------------------------------------\n\n");
	fprintf(fpo, "};\n");
	fprintf(fpo, "int\tnregnames = %d;\n", nregnames);
	fprintf(fpo, "char\tregserialno[] = \"%ld\";\n", sn);
	fclose(fpo);
	fclose(fpi);
	if ((fps = fopen(serialno, "w")) == NULL)
		fprintf(stderr, "Cannot open serial number file %s to write\n",
								serialno);
	else {
		fprintf(fps, "%ld\n", ++sn);
		fclose(fps);
	}
	exit(0);
}
