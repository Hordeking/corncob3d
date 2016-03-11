#include	<stdio.h>
#include	<string.h>
#include	"texto.h"
#include	"screens.h"

#define	TOXMESS	1
#define	TOYMESS	1
#define	TOXWID	80
#define	TOYWID	25



#define	MVPCD	1

#ifndef MVPCD
char	*towerfname = "towers.dat";
#else
char	towerfname[40]= "towers.dat";
#endif
#ifdef startloc
	char slocarr[80];
#endif

main(int argc, char **argv) {
	char	buffer[81], *p;
	int	i, nskip, y;
#ifdef startloc
	int	itemp;
	FILE *fp;
#endif


	extern int	fileout(), atoi();
	extern void	erexit(), doboss(), exit();

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

	inito();
	if (argc < 3)
		erexit("Error: wrong # of arguments.  tower n ");
#ifdef MVPCD
	if (argc > 3) strcpy(towerfname,argv[3]);
/*	errorbox("towerfname"); */
#endif

	nskip = atoi(argv[2]) + 1;
	if (nskip == 0) {
		doboss();
		exito();
		exit(0);
	}

	y = TOYMESS;
	color(WHITE, BLUE);
	cursor(1, y++);
	p = buffer;
	*p++ = SUPPERLEFT;
	for (i = 0; i < TOXWID-2; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buffer);

	p = buffer;
	*p++ = SLEFT;
	for (i = 0; i < TOXWID-2; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	for (i = 0; i < TOYWID-2; i++) {
		cursor(1, y++);
		sput(buffer);
	}

	cursor(1, y++);
	p = buffer;
	*p++ = SLOWERLEFT;
	for (i = 0; i < TOXWID-2; i++)
		*p++ = SBOTTOM;
	*p++ = SLOWERRIGHT;
	*p = '\0';
	sput(buffer);

	if (nskip > 9 || nskip < 1)
		erexit("Error: tower # out of range.");

	color(WHITE, BLUE);
#ifdef startloc
	itemp=
#endif
	fileout(towerfname, nskip,
		TOXMESS+1, TOYMESS+1, TOXWID-2, TOYWID-2, WHITE, BLUE, 1);

#ifdef startloc
	if((itemp!=0) && (strlen(slocarr)!=0)) {
		fp = fopen("startloc.dat", "w+t");
		strcat(slocarr,"\n");
		fputs(slocarr,fp);
		fclose(fp);
	}
	else
		unlink("startloc.dat");
#endif

	exito();
	exit(0);
}

void	erexit(s)
	char	*s;
{
	extern int	tkyo();
	extern void	exit();

	cursor(5, TOYMESS+2);
	sput(s);
	tkyo(7, TOYMESS+4);
	exito();
	exit(0);
}
