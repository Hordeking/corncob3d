#include	"../texto.h"

#define	BACKGROUND	CYAN

char	buf[] =
"\
          Now is the time to test out what all the colors look like            \
 ";
main() {
	int	y;
	inito();

	setblink(0);

	y = 1;

	color(BLACK, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(RED, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(GREEN, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(BLUE, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(CYAN, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(MAGENTA, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(BROWN, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	cursor(1, y++);
	color(WHITE, BACKGROUND);
	sput(buf);

	color(BACKGROUND, GRAY);
	cursor(1, y++);
	sput(buf);

	color(LIGHTRED, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(LIGHTGREEN, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(LIGHTBLUE, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	color(LIGHTCYAN, BACKGROUND);
	cursor(1, y++);
	sput(buf);

	cursor(1, y++);
	color(LIGHTMAGENTA, BACKGROUND);
	sput(buf);

	cursor(1, y++);
	color(YELLOW, BACKGROUND);
	sput(buf);

	cursor(1, y++);
	color(BRIGHTWHITE, BACKGROUND);
	sput(buf);

	getch();
	setblink(1);
}
