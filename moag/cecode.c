#include	<stdio.h>
#include	<io.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"
#include	"cecode.h"

int	cecode(p3)
	struct stuffrm3	*p3;
{
	int	y;

	extern int	criterr;
	extern char	whynorep[];
	extern void	repsetup(), beep();
	extern long	regrcode;

	if (criterr == JOYESCAPE) {
		return(0);
	}
	if (whynorep[0]) {
		beep();
		repsetup();
		xyprintf(RPXMESS+3, RPYMESS+3, "Mission report not available.");
		xyprintf(RPXMESS+3, RPYMESS+4, "(%s)", whynorep);
		tkyo(RPXMESS+4, RPYMESS+6);
		return(0);
	}
	if (criterr == MEMERR) {
		beep();
		repsetup();
		y = RPYMESS+2;
		if (p3->planeslost == 0) {
			xyprintf(RPXMESS+3, y++,
				"Corncob 3D was not able to run due to a");
			xyprintf(RPXMESS+3, y++,
				"memory allocation error.");
			y++;
		}
		else {
			xyprintf(RPXMESS+3, y++,
				"There is too little memory availble to");
			xyprintf(RPXMESS+3, y++,
				"play Corncob 3D.");
			xyprintf(RPXMESS+3, y++,
				"You will need approximately %d Kbytes",
					p3->planeslost/64 + 26);
			xyprintf(RPXMESS+3, y++, "more to run the program");
			y++;
		}
		xyprintf(RPXMESS+3, y++,
			"Please try to free up some memory by");
		xyprintf(RPXMESS+3, y++,
			"removing some TSR programs, or by");
		xyprintf(RPXMESS+3, y++,
			"reconfiguring your system.");
		y++;
		tkyo(RPXMESS+4, y++);
		return(0);
	}
	else if (criterr == JOYSTICK) {
		beep();
		repsetup();
		y = RPYMESS+2;
		xyprintf(RPXMESS+3, y++,
			"Corncob 3D could not run because a");
		xyprintf(RPXMESS+3, y++,
			"joystick was expected but not found.");
		y++;
		xyprintf(RPXMESS+3, y++,
			"Make sure your joystick is connected to");
		xyprintf(RPXMESS+3, y++,
			"the correct port.  If you do not have a");
		xyprintf(RPXMESS+3, y++,
			"joystick, choose \"keyboard\" control from");
		xyprintf(RPXMESS+3, y++, "the keyboard/joystick menu.");
		y++;
		tkyo(RPXMESS+4, y++);
		return(0);
	}
#ifdef	OLDSHARE
	else if (criterr == BADREG && regrcode >= 0) {
		beep();
		repsetup();
		y = RPYMESS+2;
		xyprintf(RPXMESS+3, y++,
			"You have entered a registration reply");
		xyprintf(RPXMESS+3, y++,
			"code which is not correct for the com-");
		xyprintf(RPXMESS+3, y++,
			"puter you are using.  This may be due to");
		xyprintf(RPXMESS+3, y++,
			"a typographical error when entering the");
		xyprintf(RPXMESS+3, y++,
			"reply code, or because you have instal-");
		xyprintf(RPXMESS+3, y++,
			"led Corncob 3D on a different machine");
		xyprintf(RPXMESS+3, y++,
			"than it was registered for.");
		y++;
		tkyo(RPXMESS+4, y++);
		return(0);
	}
	else if (criterr == TRIALPER || (criterr == BADREG && regrcode < 0)) {
		beep();
		repsetup();
		y = RPYMESS+2;
		xyprintf(RPXMESS+3, y++,
			"The trial period for playing CORNCOB-3d");
		xyprintf(RPXMESS+3, y++,
			"has expired.  Please register the");
		xyprintf(RPXMESS+3, y++,
			"program.");
		y++;
		xyprintf(RPXMESS+3, y++,
			"The top pilots have now been frozen.");
		xyprintf(RPXMESS+3, y++,
			"They will become playable again when the");
		xyprintf(RPXMESS+3, y++,
			"program is registered.");
		y++;
		tkyo(RPXMESS+4, y++);
		return(0);
	}
#endif	/* OLDSHARE */
	else if (criterr) {
		beep();
		repsetup();
		y = RPYMESS+3;
		xyprintf(RPXMESS+3, y++,
				"A critical error has occurred in 3.exe.");
		y++;
		xyprintf(RPXMESS+3, y++,
				"Pie in the Sky software would greatly");
		xyprintf(RPXMESS+3, y++,
				"appreciate being notified of this.");
		xyprintf(RPXMESS+3, y++,
				"Please report the circumstances of this");
		xyprintf(RPXMESS+3, y++,
				"error and the following code:  CE #%u",
				criterr);
		y++;
		tkyo(RPXMESS+4, y++);
	}
	return(1);
}

#ifdef	OLDSHARE
void	trialper1() {
	int	y;
	extern void	repsetup(), beep(), sleep();

	beep();
	repsetup();
	y = RPYMESS+2;
	xyprintf(RPXMESS+3, y++,
		"The trial period for playing CORNCOB-3d");
	xyprintf(RPXMESS+3, y++,
		"will soon expire.  Please register the");
	xyprintf(RPXMESS+3, y++,
		"program.");
	y++;
	xyprintf(RPXMESS+3, y++,
		"When the trial period expires, the top");
	xyprintf(RPXMESS+3, y++,
		"pilots will be frozen, and not playable");
	xyprintf(RPXMESS+3, y++,
		"until the program is registered.");
	y++;
	xyprintf(RPXMESS+3, y++,
		"There will be a 5 second delay...");
	sleep(5);
	y++;
	tkyo(RPXMESS+4, y++);
	return;
}
#endif	/* OLDSHARE */

void	repsetup() {
	extern void	boxsetup();

	color(BROWN, CYAN);
	boxsetup(RPXMESS, RPYMESS, RPXWID, RPYWID, 0);
	color(BLUE, CYAN);
}

#ifdef	OLDSHARE
void	timechanged() {
	int	y;
	extern char	*pilotlist;
	extern void	nuke(), boxsetup();

	nuke(pilotlist);
	nuke("3d.cfg");
	y = 5;
	color(BLUE, CYAN);
	boxsetup(10, y++, 60, 15, 0);
	y++;
	xyprintf(14, y++, "   Error:  System time reading is anomolous.");
	xyprintf(14, y++, "This non-registered shareware version of Corncob");
	xyprintf(14, y++, "requires the system time and date to be set");
	xyprintf(14, y++, "correctly.  Please make sure the system time and");
	xyprintf(14, y++, "date are correct before running Corncob to avoid");
	xyprintf(14, y++, "this problem in the future.");
	y++;
	xyprintf(14, y++, "Pie-in-the-Sky Software regrets any inconvenience");
	xyprintf(14, y++, "which this may have caused.");
	y++;
	beep();
	color(BRIGHTWHITE, CYAN);
	tkyo(16, y);
}

#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/stat.h>
void	nuke(file)
	char	*file;
{
	struct stat	sb;
	int	wr, fd;
	long	n;
	char	buf[512];

	if (stat(file, &sb) < 0) {
		unlink(file);
		return;
	}
	n = sb.st_size;
	if ((fd = open(file, O_RDWR)) < 0) {
		unlink(file);
		return;
	}
	while (n > 0) {
		wr = (n > 512) ? 512 : (int)n;
		write(fd, buf, wr);
		n -= wr;
	}
	close(fd);
	unlink(file);
}

#else	/* !OLDSHARE */

void	nuke(file)
	char	*file;
{
	unlink(file);
}

#endif	/* !OLDSHARE */
