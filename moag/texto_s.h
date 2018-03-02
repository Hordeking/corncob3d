/*	Backgrounds	*/
#define	BLACK	0
#define	BLUE	1
#define	GREEN	2
#define	CYAN	3
#define	RED		4
#define	MAGENTA	5
#define	BROWN	6
#define	WHITE	7
#define	BLINK	8

/*	Foregrounds	*/
#define	GRAY		8
#define	LIGHTBLUE	9
#define	LIGHTGREEN	10
#define	LIGHTCYAN	11
#define	LIGHTRED	12
#define	LIGHTMAGENTA	13
#define	YELLOW		14
#define	BRIGHTWHITE	15

#define	REALYELLOW	14

/*	Screen	*/
#define	NCOL	80
#define	NROW	25

/*	 Externals	*/
extern void	bioscursor();
extern void	inito();
extern void	cursoroff();
extern void	exito();
extern void	cursoron();
extern void	erscrn();
extern void	cursor();
extern void	color();
extern void	fcolor();
extern void	bold();
extern void	normal();
extern void	inverse();
extern void	blink();
extern void	setblink();
extern void	highlite();
extern void	cput();
extern void	cback();
extern void	cblock();
extern void	sput();
extern int	hsput();
extern int	xyprintf();
extern void	beep();
extern int far	*yankscr();
extern void	putscr();
extern void	freescr();
extern void	savepos();
extern void	restpos();

extern int	textoinited;
