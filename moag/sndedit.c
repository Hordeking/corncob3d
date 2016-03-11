#include	<stdio.h>
#include	<string.h>
#include	<dos.h>
#include	"moag.h"
#include	"texto.h"
#include	"screens.h"

char	soundfile[] = "3.adl";

struct sound3 {
	char	voiceno;
	char	*name;
	char	*desc;
	unsigned char	pervoice[3];
	unsigned char	perop1[5], perop2[5];
} sounds[] = {
	{ 0,	"Engine",	"engine noise" },
	{ 1,	"Guns",		"guns firing" },
	{ 2,	"Boom",		"enemy AAA shell exploding" },
	{ 3,	"Screech",	"landing gear hitting pavement" },
	{ 2,	"Blam",		"plane hit by enemy AAA or KLA" },
	{ 7, 	"Missile",	"missiles firing" },
	{ 7,	"Crash",	"crashlanding" },
	{ 7,	"Wind",		"wind sound heard when out of plane" },
	{ 5,	"Stall",	"stall indicator sound" },
	{ 3,	"Saucer",	"enemy saucer's non-newtonian drive" },
	{ 3,	"Orb",		"enemy headquarters' nausea-field" },
	{ 4,    "Kla",		"enemy KLA missle"},
};
int	nsounds = (sizeof(sounds)/sizeof(struct sound3));
int	nsm = (sizeof(sounds)/sizeof(struct sound3) - 1);
int	percflag = 0;

char	upflag = 0;		/* non zero means user has updated a sound */


/*
 *	Per voice registers
 */
#define	FNUML	0xa0
#define	FNUMH	0xb0
#define	FBC	0xc0
unsigned char	voiceregnos[3] = { FNUML, FNUMH, FBC };

#define	MSEXMESS		3	/* Sounds Editor Menu */
#define	MSEYMESS		6
#define	MSEXWID		32
#define	MSEYWID		12
#define MYOFF 5
#define MXOFF 4


/*
 *	Per operator registers
 */
#define	AVEKM	0x20
#define	KSLTL	0x40
#define	ARDR	0x60
#define	SLRR	0x80
#define	WS	0xe0
unsigned char	opregnos[5] = { AVEKM, KSLTL, ARDR, SLRR, WS };

struct opspervoice {
	signed char	op1no;
	signed char	op1off;
	signed char	op2no;
	signed char	op2off;
} opvoicemap[] = {
	{ 1,	0x0,	4,	0x3 },
	{ 2,	0x1,	5,	0x4 },
	{ 3,	0x2,	6,	0x5 },
	{ 7,	0x8,	10,	0xb },
	{ 8,	0x9,	11,	0xc },
	{ 9,	0xa,	12,	0xd },
	{ 13,	0x10,	16,	0x13 },
	{ 17,	0x14,	-1,	-1 },
	{ 15,	0x12,	-1,	-1 },
	{ 18,	0x15,	-1,	-1 },
	{ 14,	0x11,	-1,	-1 },
};

struct stuffromoffset {
	signed char	op;
	unsigned char	opno;
	unsigned char	voice;
} opfromoffset[] = {
	{  1, 1,  1 }, {  2, 1,  2 }, {  3, 1,  3 },
	{  4, 2,  1 }, {  5, 2,  2 }, {  6, 2,  3 },
	{ -1, 0,  0 }, { -1, 0,  0 },
	{  7, 1,  4 }, {  8, 1,  5 }, {  9, 1,  6 },
	{ 10, 2,  4 }, { 11, 2,  5 }, { 12, 2,  6 },
	{ -1, 0,  0 }, { -1, 0,  0 },
	{ 13, 1,  7 },
	{ 14, 1, 11 },
	{ 15, 1,  9 },
	{ 16, 2,  7 },
	{ 17, 1,  8 },
	{ 18, 1, 10 },
};

void	read3() {
	struct stuffromoffset	*op;
	struct sound3	*snd;
	FILE	*fp;
	int	lineno, r, v, peropoff, pervoiceoff, voice;
	char	buf[16], *per, *p;

	extern char	*fngets();

	if ((fp = fopen(soundfile, "rb")) == NULL)
		return;

	snd = &sounds[0];
	lineno = 0;
	per = "error %d in file %s:  line %d = %s (%x,%x)";
	while (fngets(buf, 16, fp) != NULL) {
		lineno++;
		for (p = buf; *p && (*p == ' ' || *p == '\t'); p++);
		if (*p == '\0' || *p == '#')
			continue;
		if (strcmp(p, "ff") == 0) {
			snd++;
			continue;
		}
		r = 0;
		v = 0;
		if (sscanf(p, "%x,%x", &r, &v) != 2) {
			errorbox(per, 0, soundfile, lineno, buf, r, v);
			continue;
		}

		op = NULL;
		voice = -1;
		if (r >= WS && r < (WS+0x16)) {
			op = &opfromoffset[r - WS];
			peropoff = 4;
		}
		else if (r >= FBC && r < (FBC+0x9)) {
			voice = r - FBC;
			pervoiceoff = 2;
		}
		else if (r >= FNUMH && r < (FNUMH+0x9)) {    /* F high+block */
			voice = r - FNUMH;
			pervoiceoff = 1;
		}
		else if (r >= FNUML && r < (FNUML+0x9)) {    /* F-number low */
			voice = r - FNUML;
			pervoiceoff = 0;
		}
		else if (r >= SLRR && r < (SLRR+0x16)) {
			op = &opfromoffset[r - SLRR];
			peropoff = 3;
		}
		else if (r >= ARDR && r < (ARDR+0x16)) {
			op = &opfromoffset[r - ARDR];
			peropoff = 2;
		}
		else if (r >= KSLTL && r < (KSLTL+0x16)) {
			op = &opfromoffset[r - KSLTL];
			peropoff = 1;
		}
		else if (r >= AVEKM && r < (AVEKM+0x16)) {
			op = &opfromoffset[r - AVEKM];
			peropoff = 0;
		}
		else {
			errorbox(per, 1, soundfile, lineno, buf, r, v);
			continue;
		}

		if (voice != -1) {
			if (snd->voiceno != voice) {
				errorbox(per, 2, soundfile, lineno, buf, r, v);
				errorbox(" ...snd->voiceno=%d voice=%d",
						snd->voiceno, voice);
				continue;
			}
			snd->pervoice[pervoiceoff] = (unsigned char)v;
		}
		else if (op != NULL) {
			voice = snd->voiceno;
			if (op->opno == 1 && op->op != opvoicemap[voice].op1no){
				errorbox(per, 3, soundfile, lineno, buf, r, v);
				errorbox(" ...op->op=%d op1no=%d",
					op->op, opvoicemap[voice].op1no);
				continue;
			}
			if (op->opno == 2 && op->op != opvoicemap[voice].op2no){
				errorbox(per, 3, soundfile, lineno, buf, r, v);
				errorbox("op->op=%d op2no=%d",
					op->op, opvoicemap[voice].op2no);
				continue;
			}
			if (op->opno == 1)
				snd->perop1[peropoff] = (unsigned char)v;
			else if (op->opno == 2)
				snd->perop2[peropoff] = (unsigned char)v;
			else {
				errorbox(per, 4, soundfile, lineno, buf, r, v);
				continue;
			}
		}
		else {
			errorbox(per, 5, soundfile, lineno, buf, r, v);
			continue;
		}
	}
	fclose(fp);
}

void	write3() {
	char	*fmt;
	int	i, j, op1off, op2off;
	struct sound3	*sn;
	FILE	*fp;

/* return; */

	if ((fp = fopen(soundfile, "wb")) == NULL) {
		errorbox("Cannot open %s for writing", soundfile);
		return;
	}

	sn = &sounds[0];
	fmt = "%x,%x\r\n";
	for (i = 0; i < nsounds; i++) {
		for (j = 0; j < 3; j++) {
			fprintf(fp, fmt, voiceregnos[j] + sn->voiceno,
					sn->pervoice[j]);
		}
		op1off = opvoicemap[sn->voiceno].op1off;
		op2off = opvoicemap[sn->voiceno].op2off;
		for (j = 0; j < 5; j++) {
			fprintf(fp, fmt, opregnos[j] + op1off, sn->perop1[j]);
		if(op2off > 0)  /* take care of single operator case */
			fprintf(fp, fmt, opregnos[j] + op2off, sn->perop2[j]);
		}
		fprintf(fp, "ff\r\n");
		sn++;
	}
	fclose(fp);
}

void	choosesnd() {
	int	y, i, s, ns, c;
	int far	*scr;
	char	buf[LINELEN];
	extern void	editsnd(), center();


/* errorbox("Please do not use; Sound Editor incomplete"); */

/*	read3(); */   /* this is done in the main section now */
	y = SEYMESS;
	simplesetup(SEXMESS, y++, SEXWID, SEYWID, "Sounds Editor",
							BROWN, CYAN, BLUE);
	y++;
	color(BRIGHTWHITE, CYAN);
	center(buf, SEXWID-2,
		"Use \030\031 to choose the sound you wish to edit.");
	xyprintf(SEXMESS+1, y++, buf);
	center(buf, SEXWID-2,
		"<CR> to edit the sound, or <ESC> for main editor menu");
	xyprintf(SEXMESS+1, y++, buf);
/*	center(buf, SEXWID-2,
		"or <BS> or <ESC> to return to the main menu.");
	xyprintf(SEXMESS+1, y++, buf); /**/

	y = SEYMESS+5;
	color(BLUE, CYAN);
	for (i = 0; i < nsounds; i++)
		xyprintf(SEXMESS+15, y++, "%s (%s)",
					sounds[i].desc, sounds[i].name);

	s = 0;
	ns = 0;
	color(YELLOW, CYAN);
	xyprintf(SEXMESS+15, SEYMESS+5+ns, "%s (%s)",
				sounds[ns].desc, sounds[ns].name);
	while (1) {
		c = spgetch();
		if (c == 010 || c == 033) {
/*			write3(); */
			return;
		}
		else if (c == UPARROW || c == LEFTARROW || c == 'k') {
			if (s == 0)
				ns = nsm;
			else
				ns = s - 1;
		}
		else if (c == DOWNARROW || c == RIGHTARROW || c == 'j') {
			if (s == nsm)
				ns = 0;
			else
				ns = s + 1;
		}
		else if (c != '\n') {
			beep();
			continue;
		}
		color(BLUE, CYAN);
		xyprintf(SEXMESS+15, SEYMESS+5+s, "%s (%s)",
					sounds[s].desc, sounds[s].name);
		color(YELLOW, CYAN);
		xyprintf(SEXMESS+15, SEYMESS+5+ns, "%s (%s)",
					sounds[ns].desc, sounds[ns].name);
		s = ns;
		if (c == '\n') {
			scr = yankscr();
			editsnd(&sounds[s]);
			putscr(scr);
			freescr(scr);
		}
	}
}

void	restorefile() {

	extern int	fgetc(),fputc();
	FILE	*fpw,*fp;
	static char	oldsoundfile[] = "def3.adl";
	int	i;
	long	now, next, far *ptime;


	if ((fp = fopen(oldsoundfile, "rb")) == NULL) {
		errorbox("Cannot open %s for reading", oldsoundfile);
		return; }

	if ((fpw = fopen(soundfile, "wb")) == NULL) {
		errorbox("Cannot open %s for writing", soundfile);
		return;
	}
	while ((i=fgetc(fp)) != EOF)  fputc(i,fpw);
	fclose(fp);
	fclose(fpw);
	color(RED, CYAN);
	xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+4+1, "%s",
					"Default Sounds Restored.");
	ptime = (long far *)MK_FP(0x40, 0x6C);
	now = *ptime;
	next = now+30;
	while (*ptime < next) i=i+1;
	xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+4+1, "%s",
					"                         ");
	upflag=0;	/* make sound not updated */
	}

void	savesounds() {
	long	now, next, far *ptime;
	int	i;

			  write3();
			  color(RED, CYAN);
			  xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+4+1, "%s",
					"New Sounds Saved.");
			  ptime = (long far *)MK_FP(0x40, 0x6C);
			  now = *ptime;
			  next = now+30;
			  while (*ptime < next) i=i+1;
			  xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+4+1, "%s",
					"                 ");
	}

void	checkforupdate() {
	int far	*scr;
	char	c;
	if(upflag!=0) {
	   scr = yankscr();
  	   beep();
	   simplesetup(MSEXMESS+3, MSEYMESS+MYOFF+3, SEXWID-5, 5, 
		"Abort W/O Saving",
					WHITE, RED, BLINK+WHITE);
	   color(BLINK+WHITE, RED);
	   xyprintf(MSEXMESS+8, MSEYMESS+MYOFF+3+2, "%s",
		"Type S to save, any other key exits w/o saving new changes.");
	   c=spgetch();
	   putscr(scr);
	   freescr(scr);
	   if(c==0x53 || c==0x73)
	     savesounds();
	  }
	}

void	sndedit() {
	int	y, i, s, ns, c;
	int far	*scr;
	char	buf[LINELEN];
	extern void	editsnd(), center(),sndhelp(),fdcpy();
	long	now, next, far *ptime;
	int nmainstrings=5;
	int nmsm=4;
	static char *mainstrings[] = {"Change Sound","Restore Default Sounds",
                 "Information/Help", "Save & Return to Main",
		"Abort and Return to Main"};


/* errorbox("Please do not use; Sound Editor incomplete"); */

	read3();
	upflag=0;		/* sound not updated */
	y = SEYMESS;
	simplesetup(MSEXMESS, y++, MSEXWID, MSEYWID, "Sounds Editor",
							BROWN, CYAN, BLUE);
	y++;
	color(BRIGHTWHITE, CYAN);

	center(buf, MSEXWID-2,
		"Use \030\031 to choose,");
	xyprintf(MSEXMESS+1, y++, buf);
	center(buf, MSEXWID-2,
		"Use <CR> to Activate.");
	xyprintf(MSEXMESS+1, y++, buf);

	color(BRIGHTWHITE, CYAN);

	y = SEYMESS+MYOFF;
	color(BLUE, CYAN);
	for (i = 0; i < nmainstrings; i++)
		xyprintf(MSEXMESS+MXOFF, y++, "%s",
					mainstrings[i]);

	s = 0;
	ns = 0;
	color(YELLOW, CYAN);
	xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+ns, "%s",
					mainstrings[s]);
	while (1) {
		c = spgetch();
		if (c == 0x08 || c == 0x1b) {
			checkforupdate();
			return;
		}
		else if (c == UPARROW || c == LEFTARROW || c == 'k') {
			if (s == 0)
				ns = nmsm;
			else
				ns = s - 1;
		}
		else if (c == DOWNARROW || c == RIGHTARROW || c == 'j') {
			if (s == nmsm)
				ns = 0;
			else
				ns = s + 1;
		}
		else if (c != '\n') {
			beep();
			continue;
		}
		color(BLUE, CYAN);
		xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+s, "%s",
					mainstrings[s]);
		color(YELLOW, CYAN);
		xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+ns, "%s",
					mainstrings[ns]);
		s = ns;
		if (c == '\n') {
			if(s==0) {
			scr = yankscr();
			choosesnd();	/* goto the main sound edit menu */
			putscr(scr);
			freescr(scr);
			color(BLUE, CYAN); /* make current menu item 'save' */
			xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+s, "%s",
					mainstrings[s]);
			s=3;
			ns=3;
			color(YELLOW, CYAN);
			xyprintf(MSEXMESS+MXOFF, MSEYMESS+MYOFF+ns, "%s",
					mainstrings[ns]);
			}
			 else if(s==1) {
			  restorefile();
			 }  else if(s==2) {
			   scr = yankscr();
			   sndhelp();
			   putscr(scr);
			   freescr(scr);
		  	   }  else  if(s==3) {
			     savesounds();
			     return;
			     }  else  if(s==4) return;
		}
	}
}

extern void	connect(), feedback(),
		onoff(), lowhi(), multi(), ksl(), totlev(), ch4(), wssel();

struct chstuff {
	char	*what;
	unsigned char	regno;
	unsigned char	nbits;
	unsigned char	nshift;
	unsigned char	mask;
	void	(*chval)();
} pervoice[] = {
	{ "Connection",			2, 1, 0, 0x1,	connect },
	{ "Feedback",			2, 3, 1, 0xe,	feedback },
}, perop[] = {
	{ "Tremolo",			0, 1, 7, 0x80,	onoff	},
	{ "Vibrato",			0, 1, 6, 0x40,	onoff	},
	{ "Sustain",			0, 1, 5, 0x20,	onoff	},
	{ "Envelope Rate",		0, 1, 4, 0x10,	lowhi	},
	{ "Frequency Multiplier",	0, 4, 0, 0xf,	multi	},
	{ "Low-Pass Filter",		1, 2, 6, 0xc0,	ksl	},
	{ "Total Level",		1, 6, 0, 0x3f,	totlev	},
	{ "Attack Rate",		2, 4, 4, 0xf0,	ch4	},
	{ "Decay Rate",			2, 4, 0, 0xf,	ch4	},
	{ "Sustain Level",		3, 4, 4, 0xf0,	ch4	},
	{ "Release Rate",		3, 4, 0, 0xf,	ch4	},
	{ "Waveform Select",		4, 2, 0, 0x3,	wssel	},
};
static int	npervoice = (sizeof(pervoice)/sizeof(struct chstuff));
static int	nperop = (sizeof(perop)/sizeof(struct chstuff));
static char	*retsval;
static int	retival;
static int	setx, sety;
#define	FS_SET	0
#define	FS_RET	1

static void	chooses(strings, nstrings, x, pv)
	char	**strings;
	int	nstrings, x, *pv;
{
	int	starts[16], i, ni, nsm, c;
	char	*fmt = " %s ";

	starts[0] = x;
	for (i = 1; i < nstrings; i++)
		starts[i] = starts[i-1] + strlen(strings[i-1]) + 2;

	for (i = 0; i < nstrings; i++)
		xyprintf(starts[i], sety, fmt, strings[i]);

	i = *pv;
	ni = i;
	nsm = nstrings - 1;
	inverse();
	xyprintf(starts[i], sety, fmt, strings[i]);
	while (1) {
		c = spgetch();
		if (c == 010 || c == 033)
			return;
		else if (c == UPARROW || c == LEFTARROW || c=='k' || c=='h') {
			if (i == 0)
				ni = nsm;
			else
				ni = i - 1;
		}
		else if (c == DOWNARROW || c == RIGHTARROW || c=='j' || c=='l'){
			if (i == nsm)
				ni = 0;
			else
				ni = i + 1;
		}
		else if (c != '\n') {
			beep();
			continue;
		}
		normal();
		xyprintf(starts[i], sety, fmt, strings[i]);
		inverse();
		xyprintf(starts[ni], sety, fmt, strings[ni]);
		i = ni;
		if (c == '\n') {
			*pv = i;
			return;
		}
	}
}

static void	xbar(x, v, nv, mkval)
	int	x, v, nv;
	char	*(*mkval)();
{
	int	i, non;

	cursor(x, sety + 1);
	sput((*mkval)(v));
	non = (v+1)*32/nv;
	for (i = 0; i < non; i++)
		cput(BLOCK);
	for (; i < 32; i++)
		cput(FUZZ2);
}

static void	choosev(nval, x, pv, mkval)
	int	nval, x, *pv;
	char	*(*mkval)();
{
	int	i, ni, c;

	i = *pv;
	ni = i;
	xbar(x, i, nval, mkval);
	while (1) {
		c = spgetch();
		if (c == 010 || c == 033)
			return;
		else if (c == UPARROW || c == LEFTARROW || c=='k' || c=='h') {
			if (i == 0)
				beep();
			else
				ni = i - 1;
		}
		else if (c == DOWNARROW || c == RIGHTARROW || c=='j' || c=='l'){
			if (i == nval-1)
				beep();
			else
				ni = i + 1;
		}
		else if (c != '\n') {
			beep();
			continue;
		}
		xbar(x, ni, nval, mkval);
		i = ni;
		if (c == '\n') {
			*pv = i;
		        upflag=1;	/* a parameter has been updated */
			return;
		}
	}
}

static void	connect(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	*ps;
	int	ival,oldival;
	static char	*strings[] = { "FM", "Additive" };

	p = &pervoice[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		if (percflag) {
			retsval = "None";
			return;
		}
		retsval = strings[retival];
		savepos();
		color(LIGHTCYAN, RED);
		if (ival == 0)
			xyprintf(SEXMESS+50, SEYMESS+4,
						"Modulator       Carrier");
		else
			xyprintf(SEXMESS+50, SEYMESS+4,
						"    Osc#1         Osc#2");
		restpos();
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	ps = "Choose method of synthesis: ";
	xyprintf(setx, sety, ps);
	oldival=ival;
	chooses(strings, 2, setx + strlen(ps), &ival);
	if(ival!=oldival) upflag=1;  /* has this been updated? */
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static void	feedback(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	static char	*strings[] = { "0", "pi/16", "pi/8", "pi/4",
					"pi/2", "pi", "2*pi", "4*pi" };
	int	ival,oldival;

	p = &pervoice[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		retsval = strings[retival];
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	
	xyprintf(setx, sety, "Choose feedback modulation factor: ");
	sety++;
	oldival=ival;
	chooses(strings, 8, setx + 10, &ival);
	if(ival!=oldival) upflag=1;
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static void	onoff(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	buf[32], *pp;
	int	ival,oldival;
	static char	*strings[] = { "off", "on" };

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		retsval = strings[retival];
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	pp = "Set %s on or off: ";
	if (i == 0)
		sprintf(buf, pp, "tremolo");
	else if (i == 1)
		sprintf(buf, pp, "vibrato");
	else if (i == 2)
		sprintf(buf, pp, "operator sustain");
	else
		buf[0] = '\0';
	xyprintf(setx, sety, buf);
	oldival=ival;
	chooses(strings, 2, setx + strlen(buf), &ival);
	if(ival!=oldival) upflag=1;
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static void	lowhi(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	*ps;
	int	ival,oldival;
	static char	*strings[] = { "low", "high" };

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		retsval = strings[retival];
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	ps = "Choose envelope generator rate: ";
	xyprintf(setx, sety, ps);
	oldival=ival;
	chooses(strings, 2, setx + strlen(ps), &ival);
	if(ival!=oldival) upflag=1;
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static char	*fval(v)
	int	v;
{
	static char	buf[16];

	if (v == 0)
		strcpy(buf, "1/2 ");
	else
		sprintf(buf, "%-3d ", v);
	return(buf);
}

static void	multi(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	*ps, *ps2;
	int	ival;

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		ps = fval(ival);
		for (ps2 = &ps[strlen(ps)-1]; *ps2 == ' ' && ps2 > ps; ps2--);
		*++ps2 = '\0';
		retsval = ps;
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	xyprintf(setx, sety, "Choose frequency multiplier: ");
	cursor(setx + 8, sety+1);
	sput("Currently =");
	choosev(16, setx + 20, &ival, fval);
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static void	ksl(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	*ps;
	int	ival,oldival;
	static char	*strings[] = { "0", "1.5", "3", "6" };
	static char	buf[16];

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		sprintf(buf, "%s dB/oct", strings[retival]);
		retsval = buf;
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	ps = "Choose low pass filter slope (dB/octave): ";
	xyprintf(setx, sety, ps);
	oldival=ival;
	chooses(strings, 4, setx + strlen(ps), &ival);
	if(ival!=oldival) upflag=1;
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static char	*tval(v)
	int	v;
{
	static char	buf[16];
	int	a;

	a = 63 - v;
	a *= 300;
	a /= 4;
	sprintf(buf, "%4d  dB ", a);
	buf[4] = buf[3];
	buf[3] = buf[2];
	buf[2] = '.';
	return(buf);
}

static void	totlev(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	*pp;
	int	ival;

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		pp = tval(ival);
		pp[strlen(pp)-1] = '\0';
		retsval = pp;
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	xyprintf(setx, sety, "Choose total level (attenuation): ");
	cursor(setx + 8, sety+1);
	sput("Currently =");
	choosev(64, setx + 20, &ival, tval);
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static char	*rval(v)
	int	v;
{
	static char	buf[8];

	sprintf(buf, "%-3d", v);
	return(buf);
}

static void	ch4(i, flag, s)
	int	i;
	unsigned char	*s;
{
	struct chstuff	*p;
	int	ival, ii;
	static char	buf[8];
	static char	*types[] = {
		"Attack Rate, this increases the envelope rise time.",
		"Decay Rate, the rate of decay toward the sustain level.",
		"Sustain Level, fraction of total level held if sustain is on.",
		"Release Rate, the rate of decay after sustain is released."
	};

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		sprintf(buf, "%d", retival);
		retsval = buf;
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	ii = i - 7;
	if (ii < 0 || ii > 3)
		xyprintf(setx, sety, "Choose value: ");
	else
		xyprintf(setx, sety, "Choose %s", types[ii]);
	cursor(setx + 8, sety+1);
	sput("Currently =");
	choosev(16, setx + 20, &ival, rval);
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static void	wssel(i, flag, s)
	int	i, flag;
	unsigned char	*s;
{
	struct chstuff	*p;
	char	*ps;
	int	ival,oldival;
	static char	*strings[] = { "sine", "half-sine",
						"abs-sine", "pulse-sine" };

	p = &perop[i];
	ival = ((*s)&(p->mask))>>p->nshift;
	if (flag == FS_RET) {
		retival = ival;
		retsval = strings[retival];
		return;
	}
	else if (flag != FS_SET) {
		beep();
		return;
	}
	ps = "Choose wave-form: ";
	xyprintf(setx, sety, ps);
	oldival=ival;
	chooses(strings, 4, setx + strlen(ps), &ival);
	if(ival!=oldival) upflag=1;
	*s &= ~p->mask;
	*s |= (ival<<p->nshift)&p->mask;
}

static void	outitem(row, col, s)
	int	row, col;
	struct sound3	*s;
{
	char	buf[LINELEN];
	int	x;

	if (col == 0) {
		(*pervoice[row].chval)(row, FS_RET,
					     &s->pervoice[pervoice[row].regno]);
		sprintf(buf, " %10s ", retsval);
		x = SEXMESS+15;
	}
	else if (col == 1) {
		(*perop[row].chval)(row, FS_RET, &s->perop1[perop[row].regno]);
		sprintf(buf, " %12s ", retsval);
		x = SEXMESS+46;
	}
	else if (col == 2) {
		if (opvoicemap[s->voiceno].op2no == -1)
			return;
		(*perop[row].chval)(row, FS_RET, &s->perop2[perop[row].regno]);
		sprintf(buf, " %12s ", retsval);
		x = SEXMESS+60;
	}
	else {
		errorbox("Internal confusion:  outitem(%d, %d)", row, col);
	}
	cursor(x, SEYMESS+5 + row);
	sput(buf);
}

static void	changeitem(row, col, s)
	int	row, col;
	struct sound3	*s;
{
	int	y;

	y = SEYMESS+6 + row;
	if (row > 6)
		y -= 6;
	simplesetup(SEXMESS+3, y++, SEXWID-5, 4, "Change Value",
							BROWN, CYAN, BLUE);
	setx = SEXMESS+5;
	sety = y;
	if (col == 0)
		(*pervoice[row].chval)(row, FS_SET,
					     &s->pervoice[pervoice[row].regno]);
	else if (col == 1)
		(*perop[row].chval)(row, FS_SET, &s->perop1[perop[row].regno]);
	else if (col == 2)
		(*perop[row].chval)(row, FS_SET, &s->perop2[perop[row].regno]);
	else
		errorbox("Internal confusion:  setitem(%d, %d)", row, col);
	color(RED, WHITE);
	outitem(row, col, s);
}

void	editsnd(s)
	struct sound3	*s;
{
	char	buf[LINELEN];
	int	i, y, row, col, ocol, nrow, ncol, c;
	int far	*scr;
	extern void	sndhelp();

	y = SEYMESS;
	simplesetup(SEXMESS, y++, SEXWID, SEYWID, "Editing Sound",
						GREEN, RED, BRIGHTWHITE);
	cursor(SEXMESS+2, y);
	sput("Current Sound:  ");
	fcolor(YELLOW);
	sput(s->name);
	fcolor(BRIGHTWHITE);
	cursor(SEXMESS+45, y++);
	sput("Type <BS> for previous menu.");
	cursor(SEXMESS+45, y++);
	sput("Type <F10> for instructions.");

	if (opvoicemap[s->voiceno].op2no == -1)
		percflag = 1;
	else
		percflag = 0;

	y = SEYMESS+3;
	fcolor(LIGHTCYAN);
	xyprintf(SEXMESS+29, y++, "Operator");
	xyprintf(SEXMESS+29, y++, "Characteristics:");

	for (i = 0; i < nperop; i++) {
		cursor(SEXMESS+22, y++);
		fcolor(LIGHTGREEN);
		sprintf(buf, "%22s: ", perop[i].what);
		sput(buf);
		fcolor(BRIGHTWHITE);
		outitem(i, 1, s);
		outitem(i, 2, s);
	}

	y = SEYMESS+3;
	fcolor(LIGHTCYAN);
	xyprintf(SEXMESS+2, y++, "Voice");
	xyprintf(SEXMESS+2, y++, "Characteristics:");
	for (i = 0; i < npervoice; i++) {
		fcolor(LIGHTGREEN);
		xyprintf(SEXMESS+3, y++, "%10s:  ", pervoice[i].what);
		fcolor(BRIGHTWHITE);
		outitem(i, 0, s);
	}

	if (percflag) {
		ocol = 1;
		col = 1;
	}
	else {
		ocol = 2;
		col = 0;
	}
	row = 0;

	ncol = col;
	nrow = row;
	color(RED, WHITE);
	outitem(row, col, s);
	while (1) {
		c = spgetch();
		if (c == 010 || c == 033)
			return;
		else if (c == UPARROW || c == 'k') {
			if (row == 0) {
				if (col == 0)
					nrow = npervoice - 1;
				else
					nrow = nperop - 1;
			}
			else
				nrow = row - 1;
			ncol = col;
		}
		else if (c == DOWNARROW || c == 'j') {
			if (col == 0) {
				if (row == npervoice - 1)
					nrow = 0;
				else
					nrow = row + 1;
			}
			else {
				if (row == nperop - 1)
					nrow = 0;
				else
					nrow = row + 1;
			}
			ncol = col;
		}
		else if (c == RIGHTARROW || c == 'l') {
			if (row >= npervoice) {
				if (col == ocol)
					ncol = 1;
				else
					ncol = col + 1;
			}
			else {
				if (col == ocol)
					ncol = 0;
				else
					ncol = col + 1;
			}
			nrow = row;
		}
		else if (c == LEFTARROW || c == 'h') {
			if (row >= npervoice) {
				if (col == 1)
					ncol = ocol;
				else
					ncol = col - 1;
			}
			else {
				if (col == 0)
					ncol = ocol;
				else
					ncol = col - 1;
			}
			nrow = row;
		}
		else if (c == 'H') {
			scr = yankscr();
			sndhelp();
			putscr(scr);
			freescr(scr);
			continue;
		}
		else if (c != '\n') {
			beep();
			continue;
		}
		if (percflag && nrow == 0 && ncol == 0)
			continue;
		color(BRIGHTWHITE, RED);
		outitem(row, col, s);
		color(RED, WHITE);
		outitem(nrow, ncol, s);
		row = nrow;
		col = ncol;
		if (c == '\n') {
			scr = yankscr();
			changeitem(row, col, s);
			putscr(scr);
			outitem(row, col, s);
			freescr(scr);
			continue;
		}
	}
}

static void	sndhelp() {
	extern int	fileout();

	simplesetup(RGXMESS, RGYMESS, RGXWID, RGYWID,
			"Sound Editor Information", YELLOW, CYAN, BLUE);
	fileout("moag.inf", 10, RGXMESS+1, RGYMESS+1, RGXWID-2, RGYWID-2,
								BLUE, CYAN, 4);
}
