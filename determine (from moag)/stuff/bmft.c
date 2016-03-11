#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

char	*bufs[] = {
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
	"Now is the time for all good men to come to the aid fo their country",
	"Froggy, him am a queer bird",
	"The quick brown fox jumped over the little birdy by the lazy dog",
};

main(argc, argv)
	char	**argv;
{
	int	i, j;

	inito();

	color(BLUE, RED);
	cursor(1, 1);
	sput("bleat bleat bleat");
	cursor(2, 2);
	sput("bleat bleat bleat");

	color(LIGHTCYAN, BLUE);
	cursor(20, 20);
	sput("bleat bleat bleat");
	color(YELLOW, BLUE);
	cursor(25, 25);
	sput("bleat bleat bleat");

	getch();
/*
 	for (i = 0; i < 300; i++) {
		color((i%8), ((i+4)%8));
		for (j = 1; j <= 25; j++) {
			cursor(2, j);
			sput(bufs[j]);
		}
	}
 */
}
