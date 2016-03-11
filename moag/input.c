#include	<stdio.h>
#include	<string.h>
#include	<bios.h>
#include	<dos.h>
#include	"moag.h"
#include	"texto.h"

int	spgetch() {
	int	c;
	extern int	mygetch();
	extern void	doboss();

	do {
		c = mygetch();
		if (c == '\r')
			c = '\n';
		else if (c == 0) {
			switch (c = mygetch()) {
			case 72:
				c = UPARROW;
				break;
			case 80:
				c = DOWNARROW;
				break;
			case 75:
				c = LEFTARROW;
				break;
			case 77:
				c = RIGHTARROW;
				break;
			case 83:
				c = DELETE;
				break;
			case 73:
				c = PAGEUP;
				break;
			case 81:
				c = PAGEDOWN;
				break;
			case 82:
				c = INSERT;
				break;
			case 115:
			case 116:
				break;
			case 71:
				c = HOME;
				break;
			case 79:
				c = END;
				break;
			case 68:
				c = 'H';
				break;
			case 59:
				doboss();
				c = -1;
				break;
			default:
				beep();
				c = -1;
				break;
			}
		}
	}
	while (c == -1);
	return(c);
}

static int	bossx, bossy;
#define		BOSSCURCHAR	'$'

void	doboss() {
	int far	*scr;
	extern void	boss(), savepos(), restpos();
	extern int	mygetch();

	savepos();
	scr = yankscr();
	color(WHITE, BLACK);
	erscrn();
	cursoron();
	boss();
	bioscursor(bossy-1, bossx-1);
	if (mygetch() == 0)
		mygetch();
	cursoroff();
	putscr(scr);
	freescr(scr);
	restpos();
}

char	*bossfile = "boss.txt";
void	boss() {
	FILE	*fp;
	char	buf[LINELEN], buf2[LINELEN], *p, *p2;
	int	y, n;

	if ((fp = fopen(bossfile, "rb")) == NULL) {
		beep();
		return;
	}

	y = 1;
	bossx = 1;
	bossy = 1;
	while (fgets(buf, LINELEN, fp) != NULL) {
		buf[LINELEN-1] = '\0';
		n = strlen(buf);
		if (buf[n-1] == '\r' || buf[n-1] == '\n')
			buf[n-1] = '\0';
		if (buf[n-2] == '\r' || buf[n-2] == '\n')
			buf[n-2] = '\0';
		if (n != 0) {
			for (p = buf, p2 = buf2; *p; p++) {
				if (*p == BOSSCURCHAR) {
					if (*(p+1) == BOSSCURCHAR) {
						p++;
					}
					else {
						bossx = p - buf + 1;
						bossy = y;
						*p = ' ';
					}
				}
				*p2++ = *p;
			}
			*p2 = '\0';
			buf2[80] = '\0';
			cursor(1, y);
			sput(buf2);
		}
		y++;
		if (y > 25)
			break;
	}
	if (bossx > 80 || bossy > 25) {
		bossx = bossy = 1;
	}
	fclose(fp);
}

#define memflag

int	mygetch() {
	union REGS	regs;
	int	c;
	char buf[80];
#ifdef musicflag
#ifdef musicmenu
	int musicerr;
	extern int	musicstat,rolindex,numrolfiles;
	extern char *rolfiles[MAXROLFILES];
	extern int	servicesong(),startsong();
	extern void	endsong();
#endif
#endif
#ifdef memflag
	extern unsigned int coreleft();
	extern unsigned long farcoreleft();
#endif

#ifdef musicflag
#ifdef musicmenu
	if((musicstat & 2) != 0 ) {
		while(!kbhit() && musicstat != 0) {
		 if((musicstat & 1) != 0) {
		     if(servicesong() == 0) {
//			errorbox("song is over");
			 musicstat &= 0xfffe;
			 endsong();
//			errorbox("song is ended");
		     }
		 }
		 else
		    if((musicstat & 2) != 0) {

//			musicstat=0;
//			sprintf(buf,"Starting to play song # %d : %s",
//			 rolindex,rolfiles[rolindex]);
//			errorbox(buf);
			musicstat=2;
		 	if((musicerr=startsong(rolfiles[rolindex])) < 0 ) {

//			  if(musicerr==-2) endsong();
			  musicstat = 0;
			sprintf(buf,"!Music error occured : %d ",musicerr);
			  errorbox(buf);
			}
			else {
			  musicstat |= 1;
			  rolindex++;
                          if(rolindex >= numrolfiles) rolindex=0;
			}
                      }
		    }
	}
#endif
#endif
	regs.h.ah = 0x8;
	int86(0x21, &regs, &regs);
	c = regs.h.al;
#ifdef musicflag
#ifdef musicmenu
#ifdef memflag
	if(c == '?') {
		sprintf(buf,"Ram : %d  Far Ram : %lu"
				,coreleft(),farcoreleft());
		errorbox(buf);
	}
#endif
#endif
#endif
	return(c);
}

void	flushin() {
	extern int	kbhit();

	while (kbhit())
		mygetch();
}
