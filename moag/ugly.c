#include	<stdio.h>
#include	<string.h>
#include	"moag.h"
#include	"texto.h"

extern void	outname();
extern struct rank	ranks[];

void	foutpname(pp)
	struct pilot	*pp;
{
	sput(ranks[pp->rank].rank);
	cput(' ');
	outname(pp);
}

void	outpname(pp)
	struct pilot	*pp;
{
	sput(ranks[pp->rank].abbrank);
	cput(' ');
	outname(pp);
}

void	outname(pp)
	struct pilot	*pp;
{
	int	len;
	char	*p, *p1, *pm;
	long	now;
	extern char	*ugly();
	extern int	timeflag;
	extern long	time();

	if ((p1 = ugly(pp->name, &len)) == NULL) {
		time(&now);
		if (now < pp->date)
			timeflag = TIMEFLAGVAL;
		sput(pp->name);
		return;
	}

	for (p = pp->name; p < p1; p++)
		cput(*p);
	blink();
	pm = &p1[len];
	for (;p < pm; p++)
		cput(*p);
	normal();
	for (; *p; p++)
		cput(*p);
	time(&now);
	if (now >= pp->date)
		timeflag = TIMEFLAGNOTVAL;
}

extern char	*uglylist[];
extern int	nugly; 

void	uglyinit() {
	int	i;
	char	*p;
	extern unsigned long	oneweek, twoweeks;

	twoweeks += 154780273UL;
	for (i = 0; i < nugly; i++) {
		for (p = uglylist[i]; *p; p++) {
			if (*p >= 'a' && *p <= 'm')
				*p += 13;
			else if (*p >= 'n' && *p <= 'z')
				*p = (char)((*p - 'n') + 'a');
			else if (*p >= 'A' && *p <= 'M')
				*p += 13;
			else if (*p >= 'N' && *p <= 'Z')
				*p = (char)((*p - 'N') + 'A');
		}
	}
	oneweek += 18436572UL;
}

char	*ugly(s, lenp)
	char	*s;
	int	*lenp;
{
	int	i;
	char	buf[LINELEN], *p1, *p2;

	for (p1 = buf, p2 = s; *p2; p1++, p2++) {
		if (*p2 >= 'A' && *p2 <= 'Z')
			*p1 = (char)(*p2 + 'a' - 'A');
		else
			*p1 = *p2;
	}
	*p1 = '\0';

	for (i = 0; i < nugly; i++) {
		if ((p1 = strstr(buf, uglylist[i])) != NULL)
			break;
	}
	if (p1 == NULL)
		return(NULL);
	*lenp = strlen(uglylist[i]);
	return(&s[p1 - buf]);
}
