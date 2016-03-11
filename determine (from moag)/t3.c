#include	<process.h>

char	*file3 = ".\\3.exe";
int	kbflag;
int	invflag;
#define	LINELEN	128

main() {
	char	*argvec[LINELEN], flags[LINELEN];
	int	fl, status, argc;

	/*
	 *	set up argv[0]
	 */
	argc = 0;
	argvec[argc++] = file3;

	/*
	 *	Add in flags
	 */
	fl = 0;
	if (kbflag) {
		strcpy(flags, "-j");
		fl = 1;
	}
	if (invflag) {
		if (!fl) {
			strcpy(flags, "-");
			fl = 1;
		}
		strcat(flags, "i");
	}
	if (fl)
		argvec[argc++] = flags;

	argvec[argc++] = 0;
	status = spawnv(P_OVERLAY, file3, argvec);
}
