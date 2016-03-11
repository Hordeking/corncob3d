#include	<stdio.h>
#include	<fcntl.h>
#include	<dos.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

char	*prog;
void	pusage() {
	errorbox("Usage:  %s [-t] [univers]", prog);
	exito();
	exit(1);
}

main(argc, argv)
	char	**argv;
{
	union un3	u3;
	FILE	*fp;
	int	tflag, thflag, nobjectives, ncobjectives, i;
	unsigned int	seg, off;
	char	*file;

	extern int	_fmode, threeuniv(), newtower(), towerno;
	extern void	boxsetup();

	_fmode = O_BINARY;
	inito();
	color(BRIGHTWHITE, BLACK);

	prog = argv[0];
	tflag = 1;
	if (argc == 3 && strcmp(argv[1], "-_-") == 0) {
		if (sscanf(argv[2], "%4x%4x", &seg, &off) == 2) {
			u3.pp = MK_FP(seg, off);
			tflag = 2;
			errorbox("fp = %Fp from argv[2]=%s", u3.pp,argv[2]);/**/
		}
		else {
			errorbox("Cannot parse far pointer: %s", argv[2]);
			exito();
			exit(1);
		}
	}
	else {
		thflag = 0;
		if (argc == 1) {
			thflag = 0;
			file  = "3univ.dat";
		}
		else if (argc == 2) {
			if (argv[1][0] == '-')
				pusage();
			else {
				thflag = 0;
				file = argv[1];
			}
		}
		else if (argc == 3) {
			if (strcmp(argv[1], "-t") == 0) {
				thflag = 1;
				file = argv[2];
			}
			else
				pusage();
		}
		else
			pusage();

		if ((fp = fopen(file, "rb")) == NULL) {
			errorbox("Cannot open %s", file);
			exito();
			exit(1);
		}
		if (thflag) {
			if (fseek(fp,
				(long)(sizeof(struct theatert) +
					sizeof(struct pilot)),
						SEEK_SET) < 0) {
				errorbox("Seek error on %s", file);
				exito();
				exit(1);
			}
		}
		u3.fp = fp;
		tflag = 1;
	}

	if (!threeuniv(&u3, NULL, &nobjectives, &ncobjectives, file, tflag)) {
		errorbox("Error running threeuniv");
		exito();
		exit(0);
	}
	listobjs();

	if (tflag != 2)
		xyprintf(10, 1, "File:  %s    Type:  %s", file,
						thflag? "Theater" : "Def");
	xyprintf(10, 2,
	"Number of MO remaining = %d    Number of completed MO found = %d",
					nobjectives, ncobjectives);
	towerno = 4;
	newtower(towerno, 1);
	exito();
	exit(0);
}

#include	<stdarg.h>
/*VARARGS0*/
void	errorbox(cntrl)
	char	*cntrl;
{
	va_list	args;
	int far	*scr;
	char	buf[256];
	extern int	vsprintf();
	extern void	boxsetup();

	scr = yankscr();
	savepos();
	beep();
	color(LIGHTRED, WHITE);
	boxsetup(10, 9, 60, 6, 1);
	fcolor(RED);

	va_start(args, cntrl);
	vsprintf(buf, cntrl, args);
	va_end(args);
	cursor(12, 10);
	sput(buf);

	tkyo(18, 13);
	putscr(scr);
	freescr(scr);
	restpos();
}

void	nbsimplesetup(x, y, xwid, ywid, mesg, bord, back, fore)
	int	x, y, xwid, ywid, bord, back, fore;
	char	*mesg;
{
	extern void	_simplesetup();
	_simplesetup(x, y, xwid, ywid, mesg, bord, back, fore, 0);
}

void	simplesetup(x, y, xwid, ywid, mesg, bord, back, fore)
	int	x, y, xwid, ywid, bord, back, fore;
	char	*mesg;
{
	extern void	_simplesetup();
	_simplesetup(x, y, xwid, ywid, mesg, bord, back, fore, 1);
}

void	_simplesetup(x, y, xwid, ywid, mesg, bord, back, fore, bflag)
	int	x, y, xwid, ywid, bord, back, fore, bflag;
	char	*mesg;
{
	int	i;
	char	buf[LINELEN], *p;

	extern void	center();
	extern void	boxsetup();

	color(bord, back);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 2; i < xwid; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	cursor(x, y-2);
	sput(buf);

	cursor(x, y-1);
	cput(SLEFT);
	cursor(x+xwid-1, y-1);
	cput(SRIGHT);

	boxsetup(x, y, xwid, ywid, bflag);
	cursor(x, y);
	cput(SLEFTMIDDLE);
	cursor(x+xwid-1, y);
	cput(SRIGHTMIDDLE);

	color(fore, back);
	center(buf, xwid-2, mesg);
	cursor(x+1, y-1);
	sput(buf);
}

void	boxsetup(x, y, xwid, ywid, sflag)
	int	x, y, xwid, ywid, sflag;
{
	char	buf[LINELEN], *p;
	int	i, ywidm;

	cursor(x, y);
	p = buf;
	*p++ = SUPPERLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = STOP;
	*p++ = SUPPERRIGHT;
	*p = '\0';
	sput(buf);

	p = buf;
	*p++ = SLEFT;
	for (i = 1; i < xwid-1; i++)
		*p++ = ' ';
	*p++ = SRIGHT;
	*p = '\0';
	ywidm = ywid - 1;
	for (i = 1; i < ywidm; i++) {
		cursor(x, y+i);
		sput(buf);
		if (sflag)
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
	if (sflag) {
		cblock();
		cursor(x+1, y+ywid);
		for (i = 0; i < xwid; i++)
			cblock();
	}
}
