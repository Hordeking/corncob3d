#include	"texto.h"
char	*p =
"Now is the time for all good men to come to the aid of their country.";

main() {
	int	i;

	inito();
	for (i = 0; i < 16; i++) {
		color(i, RED);
		cursor(1, 3+i);
		sput(p);
	}
	getch();
	exito();
}
