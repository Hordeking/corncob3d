#include	<stdio.h>
#include	<string.h>
#include	"moag.h"

char	fileinf[80] = "towers.inf";

#define	NTOWERS		9
#define	MAXTPT		9	/* Max tiles per tower */

#define	NTILESX		8
#define	NTILESY		8
#define	TILESZX		32
#define	TILESZY		32
#define	UNIVERSEX	-128
#define	UNIVERSEY	-128

extern long	getobjx();
extern long	getobjy();
extern long	getobjz();
extern long	getdist();

#define	OZ	720L
struct pertowerinfo	ptos[NTOWERS];
unsigned char	ltiles[NTILESX][NTILESY] = {
	0, 0, 0, 3, 3, 6, 6, 6,
	0, 0, 0, 3, 3, 6, 6, 6,
	0, 0, 0, 3, 3, 6, 6, 6,
	1, 1, 1, 4, 4, 7, 7, 7,
	1, 1, 1, 4, 4, 7, 7, 7,
	2, 2, 2, 5, 5, 8, 8, 8,
	2, 2, 2, 5, 5, 8, 8, 8,
	2, 2, 2, 5, 5, 8, 8, 8
};
signed char	ttiles[NTOWERS][MAXTPT] = {
	 0,  1,  2,  8,  9, 10, 16, 17, 18,
	24, 25, 26, 32, 33, 34, -1, -1, -1,
	40, 41, 42, 48, 49, 50, 56, 57, 58,
	 3,  4, 11, 12, 19, 20, -1, -1, -1,
	27, 28, 35, 36, -1, -1, -1, -1, -1,
	43, 44, 51, 52, 59, 60, -1, -1, -1,
	 5,  6,  7, 13, 14, 15, 21, 22, 23,
	29, 30, 31, 37, 38, 39, -1, -1, -1,
	45, 46, 47, 53, 54, 55, 61, 62, 63
};

struct objheader {
	char	cksum[4];
	int	tilexid;
	int	tileyid;
	unsigned int	nobjintile;
	int	reserved[3];
};

struct cobject {
	int	xyhigh;
	unsigned int	xlow;
	unsigned int	ylow;
	unsigned int	zlow;
	char	yaw;
	char	pitch;
	char	roll;
	unsigned int	objattr;
	int	xvel;
	int	yvel;
	int	zvel;
	int	objtime;
	unsigned int	objtype;
};

struct myobj {
	long	x, y, z;
	unsigned int	type, attr;
	unsigned char	myattr;
};
#define	MYMISOBJ	0x40			/* Safe bit, could move over */
#define	MYMNUM		0x1f			/* <= 31 MOs per tower */
#define	MOGROUPD	((long)400000)		/* 3 km */

struct myhead {
	unsigned int	nobj;
#ifdef	TU
	struct myobj	*pob;
#else	/* !TU */
	struct myobj far	*pob;
#endif	/* !TU */
} mo[NTILESX*NTILESY];

int	threeuniv(i3, ofp, nobjectives, ncobjectives, fname, tflag)
	union un3	*i3;
	FILE	*ofp;
	int	*nobjectives, *ncobjectives, tflag;
	char	*fname;
{
	int	i, j, itilex, itiley, pflag, oflag, ntowers;
	unsigned int	objnum;
	long	starthere[3], d;
	struct cobject	thisobj, far * pp;
	struct objheader	thisheader;
#ifdef	TU
	struct myobj	*pob;
#else	/* !TU */
	struct myobj far	*pob;
#endif	/* !TU */

	extern long	startcx(), startcy();
	extern int	onetower, getobj();
#ifdef	TU
	extern void	*malloc();
#else	/* !TU */
	extern void far	*farmalloc();
#endif	/* !TU */
	extern unsigned int	coreleft();

	i = 0;
	for (itiley = 0; itiley < 3; itiley++) {
	    for (itilex = 0; itilex < 3; itilex++) {
		ptos[i].ox = startcx(itilex);
		ptos[i].oy = startcy(itiley);
		ptos[i].ncloseplanes = 0;
		ptos[i].nobjectives = 0;
		ptos[i].ncobjectives = 0;
		i++;
	    }
	}

	ntowers = 0;
	onetower = 0;
	*nobjectives = 0;
	*ncobjectives = 0;
	j = -1;
	for (itiley = 0; itiley < NTILESY; itiley++) {
	    for (itilex = 0; itilex < NTILESX; itilex++) {
		j++;
		if (tflag != 2) {
			if (fread(&thisheader, sizeof(thisheader), 1, i3->fp)
									!= 1) {
				errorbox("Bad read on header in %s", fname);
				return(0);
			}
			if (ofp != NULL &&
			    fwrite(&thisheader, sizeof(thisheader), 1, ofp)!=1){
				errorbox("Bad write on header in 3univ.dat");
				return(0);
			}
			if ((thisheader.tilexid != itilex) ||
			    (thisheader.tileyid != itiley)) {
				errorbox(
				   "(%d,%d): (%d,%d)  Bogus tile header in %s!",
					itilex, itiley,
					thisheader.tilexid, thisheader.tileyid,
					fname);
				return(0);
			}
		}
		else {
/*xyprintf(1, 1, "in tile %d:%d     ", itilex, itiley);			/**/
			pp = (struct cobject far *)i3->pp;
/*xyprintf(1, 2, "pointer=%Fp           ", pp);				/**/
			for (objnum = 0; objnum < 150; objnum++) {
				if (pp->objattr == 0)
					break;
				pp += sizeof(struct cobject);
			}
			objnum++;
			thisheader.nobjintile = objnum;
/*xyprintf(1, 3, "nobjs = %d      ", thisheader.nobjintile);		/**/
		}
		if (tflag == 1 || tflag == 2) {
			mo[j].nobj = thisheader.nobjintile;
			if (thisheader.nobjintile > 0) {
#ifdef	TU
				pob = (struct myobj *)malloc(
						thisheader.nobjintile*
							sizeof(struct myobj));
#else	/* !TU */
				pob = (struct myobj far *)farmalloc(
						(long)thisheader.nobjintile*
							sizeof(struct myobj));
#endif	/* !TU */
				if (pob == NULL) {
					errorbox(
				"Cannot alloc myobj pointer in tile %d   cl=%u",
								j, coreleft());
					/*
					 * SHOULD FARFREE FIRST !
					 * (here and elsewhere)
					 */
					return(0);
				}
			}
			else
				pob = NULL;
			mo[j].pob = pob;
		}

		for (objnum = 0; objnum < thisheader.nobjintile; objnum++) {
			if (!getobj(&thisobj, i3, tflag)) {
				errorbox("Bad read on object in %s", fname);
				return(0);
			}
			if (tflag == 2 && thisobj.objattr == 0)
				break;
			if (ofp != NULL &&
			    fwrite(&thisobj, sizeof(struct cobject), 1, ofp)
			    						!= 1) {
				errorbox("Bad write on object in 3univ.dat");
				return(0);
			}
#ifndef	REGISTERED
			thisobj.objtype ^= thisobj.xlow;
			thisobj.objtype ^= thisobj.ylow;
#endif	/* !REGISTERED */
			if (thisobj.objtype == 6)
				ntowers++;

			if (tflag == 1) {
				pob->type = thisobj.objtype;
				pob->attr = thisobj.objattr;
				pob->x = getobjx(itilex, &thisobj);
				pob->y = getobjy(itiley, &thisobj);
				pob->z = getobjz(&thisobj);
				pob->myattr = (unsigned char)0;
				pob++;
			}

			oflag = 0;
			if (nobjectives != NULL && (
			    (thisobj.objtype == 0)  ||
			    (thisobj.objtype == 58 &&
				((thisobj.objattr&0x7000) == 0x4000 ||
				 (thisobj.objattr&0x7000) == 0x7000)) ||
			    (thisobj.objtype == 53) ||
			    (thisobj.objtype == 43) ||
 ((thisobj.objtype == 7) && ((thisobj.objattr&0x0800) != 0))
						   )) {
				if ((thisobj.objattr&0x200) == 0) {
					oflag = 1;
					(*nobjectives)++;
				}
				else
					(*ncobjectives)++;
			}

			pflag = 1;
			if ((thisobj.objtype != 2) || (thisobj.objattr&0x100))
				pflag = 0;

			if (!oflag && !pflag)
				continue;

			for (i = 0; i < 9; i++) {
				d = getdist(itilex, itiley, &thisobj, &ptos[i]);
				if (pflag && d != -1)
					ptos[i].ncloseplanes++;
			}
			if (pflag)
				continue;
			ptos[ltiles[itilex][itiley]].nobjectives++;
		}
/*
xyprintf(1, 4, "Out of loop, objnum = %d     ", objnum);
beep();spgetch();
xyprintf(1, 1, "                                    ");
xyprintf(1, 2, "                                    ");
xyprintf(1, 3, "                                    ");
xyprintf(1, 4, "                                    ");
 */
	    }
	}

	if (ntowers == 1) {
		onetower = 1;
		for (i = 0; i < NTOWERS; i++) {
			if (i == 4)
				continue;
			ptos[i].ncloseplanes = 0;
			ptos[4].nobjectives += ptos[i].nobjectives;
			ptos[i].nobjectives = 0;
			ptos[4].ncobjectives += ptos[i].ncobjectives;
			ptos[i].ncobjectives = 0;
		}
	}

	if (tflag == 2)
		return(1);

	if (fread(starthere, sizeof(long), 3, i3->fp) != 3) {
		errorbox("Bad read on start coordinates in %s", fname);
		return(0);
	}
	if (ofp != NULL &&
	    fwrite(starthere, sizeof(long), 3, ofp) != 3) {
		errorbox("Bad write on start coordinates in 3univ.dat");
		return(0);
	}
	return(1);
}

int	getobj(op, pp, tflag)
	struct cobject	*op;
	union un3	*pp;
	int	tflag;
{
	if (tflag == 2) {
		*op = *(struct cobject far *)(pp->pp);
		pp->pp += sizeof(struct cobject);
		return(1);
	}
	else if (pp->fp != NULL)
		return(fread(op, sizeof(struct cobject), 1, pp->fp));
	else
		return(0);
}

long	getobjx(curtilex, thatobj)
	int	curtilex;
	struct cobject	*thatobj;
{
	long	temp;

	temp = curtilex*TILESZX+UNIVERSEX;
	temp = temp + (thatobj->xyhigh & (TILESZX-1));
	temp = (temp << 16) + thatobj->xlow;
	return(temp);
}

long	getobjy(curtiley, thatobj)
	int	curtiley;
	struct cobject	*thatobj;
{
	long	temp;

	temp = curtiley*TILESZY+UNIVERSEY;
	temp = temp + ((thatobj->xyhigh >> 5) & (TILESZY-1));
	temp = (temp << 16) + thatobj->ylow;
	return(temp);
}

long	getobjz(thatobj)
	struct cobject	*thatobj;
{
	long	temp;

	temp = ((thatobj->xyhigh >> 10) & (TILESZX-1));
	temp = (temp << 16) + thatobj->zlow;
	return(temp);
}

long	getdist(curtilex, curtiley, thatobj, thatstart)
	struct cobject	*thatobj;
	struct pertowerinfo	*thatstart;
	int	curtilex, curtiley;
{
	long	deltax, deltay, deltaz, temp;
	extern long	labs();

	deltax = getobjx(curtilex, thatobj) - thatstart->ox;
	deltay = getobjy(curtiley, thatobj) - thatstart->oy;
	deltaz = getobjz(thatobj) - OZ;
	temp = labs(deltax) << 1;
	temp = temp | (labs(deltay) << 1);
	temp = temp | (labs(deltaz) << 1);
	temp = (temp >> 16) & 65535;
	if ((temp & 0x0fff0) != 0)
		return(-1);      /* obviously not a near object */

	if (temp == 0) {	/* However, this obj must be real close! */
		temp = (deltax*deltax)+(deltay*deltay)+(deltaz*deltaz);
		return(temp);
	}
	/* so object neither near nor far, must be med */
	deltax = labs(deltax);
	deltay = labs(deltay);
	deltaz = labs(deltaz);
	temp =  (deltax >> 4) * (deltax >> 4);
	temp += (deltay >> 4) * (deltay >> 4);
	temp += (deltaz >> 4) * (deltaz >> 4);
	return(temp);
}

long	startcx(xp)
	int	xp;
{
	long	xs;

	xs = ((long)xp + 1L)*(64*65536L) + (-128*(65536L) + 0) +
			(-11*(65536L) - 1) + (-2*(65536L) + 3001);
	return(xs);
}

long	startcy(yp)
	int	yp;
{
	long	ys;

	ys = ((long)yp + 1L)*(64*65536L) + (-128*(65536L) + 0);
	return(ys);
}

/*
 *	Here is the list of 3 OBJECTS, and what they are and do...
 */

char ffield[] = "Force-field";
char fsfeld[] = "Small Force-field";
char flotsm[] = "Flotsm";
char ffence[] = "Fence";

#define	UC(x)	((unsigned char)(x))

#define	SHOWN	0x1
#define	ENEMY	0x2

struct oobj {
	char	*name;
	unsigned char	flag;
} oobjs[] = {

/*  0 */	"Headquarters",			UC(SHOWN|ENEMY),
/*  1 */	"Mind-bender",			UC(SHOWN|ENEMY),
/*  2 */	"Human-made plane",		UC(SHOWN),
/*  3 */	"Radar",			UC(SHOWN),
/*  4 */	"Radar Base",			UC(SHOWN),
/*  5 */	"Runway Piece",			UC(0),
/*  6 */	"Control Tower",		UC(SHOWN),
/*  7 */	"Ground Crewman",		UC(SHOWN),
/*  8 */	"Shard-2",			UC(0),
/*  9 */	"Shard-1",			UC(0),
/* 10 */	"Incendiary-blast",		UC(0),
/* 11 */	"KLA-Site",			UC(SHOWN|ENEMY),
/* 12 */	"Vehicle",			UC(SHOWN|ENEMY),
/* 13 */	"Vehicle Site",			UC(SHOWN|ENEMY),
/* 14 */	ffield,				UC(0	|ENEMY),
/* 15 */	ffield,				UC(0	|ENEMY),
/* 16 */	ffield,				UC(0	|ENEMY),
/* 17 */	ffield,				UC(0	|ENEMY),
/* 18 */	"Puff",				UC(0),
/* 19 */	"Boom",				UC(0),
/* 20 */	"Bomb",				UC(SHOWN),
/* 21 */	"Human-made Missile",		UC(SHOWN),
/* 22 */	"AAA Shell",			UC(SHOWN|ENEMY),
/* 23 */	"AAA Site",			UC(SHOWN|ENEMY),
/* 24 */	"KLA",				UC(SHOWN|ENEMY),
/* 25 */	flotsm,				UC(0),
/* 26 */	flotsm,				UC(0),
/* 27 */	"Fuel Dump",			UC(SHOWN|ENEMY),
/* 28 */	"Mystery Object",		UC(0),
/* 29 */	"Propeller",			UC(0),
/* 30 */	"Bullet",			UC(0),
/* 31 */	"Survey Marker",		UC(0),
/* 32 */	"Crater",			UC(SHOWN),
/* 33 */	"Fuel Dump -- Destroyed",	UC(SHOWN|ENEMY),
/* 34 */	"Tower Doorway",		UC(0),
/* 35 */	"Runway End piece 1",		UC(0),
/* 36 */	"Runway End piece 2",		UC(0),
/* 37 */	ffence,				UC(0	|ENEMY),
/* 38 */	ffence,				UC(0	|ENEMY),
/* 39 */	ffence,				UC(0	|ENEMY),
/* 40 */	"Barrage Balloon",		UC(SHOWN|ENEMY),
/* 41 */	"Bumble-Bee cloud",		UC(SHOWN|ENEMY),
/* 42 */	"Mortar",			UC(SHOWN|ENEMY),
/* 43 */	"Headquarters -- Destroyed",	UC(SHOWN|ENEMY),
/* 44 */	ffield,				UC(0	|ENEMY),
/* 45 */	ffield,				UC(0	|ENEMY),
/* 46 */	ffield,				UC(0	|ENEMY),
/* 47 */	ffield,				UC(0	|ENEMY),
/* 48 */	"Rescue-Van",			UC(SHOWN),
/* 49 */	"Pop-Up",			UC(SHOWN|ENEMY),
/* 50 */	"Saucer",			UC(SHOWN|ENEMY),
/* 51 */	"Saucer-Site",			UC(SHOWN|ENEMY),
/* 52 */	"TRFRU",			UC(SHOWN|ENEMY),
/* 53 */	"M-O Saucer",			UC(SHOWN|ENEMY),
/* 54 */	fsfeld,				UC(0	|ENEMY),
/* 55 */	fsfeld,				UC(0	|ENEMY),
/* 56 */	fsfeld,				UC(0	|ENEMY),
/* 57 */	"Alien",			UC(SHOWN|ENEMY),
/* 58 */	"Portal",			UC(SHOWN|ENEMY),
/* 59 */	fsfeld,				UC(0	|ENEMY),
/* 60 */	fsfeld,				UC(0	|ENEMY),

};

#define	NTYPEOBJ	(sizeof(oobjs)/sizeof(struct oobj))

FILE	*lfp;
long	ox, oy;
struct myhead	to[NTOWERS];

void	listobjs() {
	int	i, j, t, n, q, last, tstart, tlast, maxtilep;
#ifdef	TU
	struct myobj	*pob;
	struct myobj	*pob2;
#else	/* !TU */
	struct myobj far	*pob;
	struct myobj far	*pob2;
#endif	/* !TU */
	char	buf[LINELEN];
	extern void	objout(), sortem();
#ifdef	TU
	extern void	*malloc(), free();
#else	/* !TU */
	extern void	far *farmalloc(), farfree();
#endif	/* !TU */
	extern struct strip	strips[];
	extern int	onetower;

	if ((lfp = fopen(fileinf, "wb")) == NULL) {
		sprintf(buf, "Cannot open %s in threeuniv", fileinf);
		return;
	}
	if (onetower) {
		tstart = 4;
		tlast = 4;
		maxtilep = NTILESX*NTILESY;
		fprintf(lfp, "\014\r\n\014\r\n\014\r\n\014\r\n");
	}
	else {
		tstart = 0;
		tlast = NTOWERS-1;
		maxtilep = MAXTPT;
	}
	for (i = tstart; i <= tlast; i++) {
		if (!onetower)
			q = 3*(i%3) + i/3;
		to[i].nobj = 0;
		for (j = 0; j < maxtilep; j++) {
			if (!onetower) {
				t = ttiles[q][j];
				if (t < 0)
					continue;
			}
			else
				t = j;
			if (mo[t].nobj == 0)
				continue;
			pob = mo[t].pob;
			for (n = 0; n < mo[t].nobj; n++, pob++) {
				if (pob->type >= NTYPEOBJ)
					continue;
				if (!(oobjs[pob->type].flag&SHOWN))
					continue;
				to[i].nobj++;
			}
		}
	}
	for (i = tstart; i <= tlast; i++) {
		if (to[i].nobj == 0) {
			to[i].pob = NULL;
			continue;
		}
#ifdef	TU
		to[i].pob = (struct myobj *)malloc(
				(to[i].nobj*sizeof(struct myobj)));
#else	/* !TU */
		to[i].pob = (struct myobj far *)farmalloc(
				(long)(to[i].nobj*sizeof(struct myobj)));
#endif	/* !TU */
		if (to[i].pob == NULL) {
			errorbox("Cannot alloc myobj pointer in tower %d", i);
			fclose(lfp);
			return;
		}
		if (!onetower)
			q = 3*(i%3) + i/3;
		pob2 = to[i].pob;
		for (j = 0; j < maxtilep; j++) {
			if (!onetower) {
				t = ttiles[q][j];
				if (t < 0)
					continue;
			}
			else
				t = j;
			if (mo[t].nobj == 0)
				continue;
			pob = mo[t].pob;
			for (n = 0; n < mo[t].nobj; n++, pob++) {
				if (pob->type >= NTYPEOBJ)
					continue;
				if (!(oobjs[pob->type].flag&SHOWN))
					continue;
				*pob2++ = *pob;
			}
		}
	}
	for (i = tstart; i <= tlast; i++) {
		ox = ptos[i].ox;
		oy = ptos[i].oy;
		fprintf(lfp, "\014\r\n\\(Objects near %s %s:\\)\r\n\r\n",
						strips[i].s1, strips[i].s2);
		fprintf(lfp,
"\\(Object                    Quality:  Range/Dir:    Alt:\\)\r\n");
		fprintf(lfp,
"\\(                                     (Mi.)       (ft.)\\)\r\n");
		pob = to[i].pob;
		sortem(&to[i]);
		last = 0;
		for (n = 0; n < to[i].nobj; n++) {
			if ((pob->myattr&MYMNUM) != last)
				fprintf(lfp, "\r\n\r\n");
			objout(pob);
			last = (pob->myattr&MYMNUM);
			pob++;
		}
		fprintf(lfp, "\r\n\r\n");
	}
	fclose(lfp);
	for (i = 0; i < NTILESX*NTILESY; i++) {
		if (mo[i].nobj != 0)
#ifdef	TU
			free(mo[i].pob);
#else	/* !TU */
			farfree(mo[i].pob);
#endif	/* !TU */
	}
	for (i = tstart; i <= tlast; i++) {
		if (to[i].nobj != 0)
#ifdef	TU
			free(to[i].pob);
#else	/* !TU */
			farfree(to[i].pob);
#endif	/* !TU */
	}
}

int	cmpob(p1, p2)
	struct myobj	*p1, *p2;
{
	int	md;

	/* Same Group first */
	md = (int)(p1->myattr&MYMNUM) - (int)(p2->myattr&MYMNUM);
	if (md > 0)
		return(1);
	else if (md < 0)
		return(-1);

	/* Then friendlies first within a group */
	md = (oobjs[p1->type].flag&ENEMY) - (oobjs[p2->type].flag&ENEMY);
	if (md > 0)
		return(1);
	else if (md < 0)
		return(-1);

	/* Then non-mission objectives first */
	md = (int)(p1->myattr&MYMISOBJ) - (int)(p2->myattr&MYMISOBJ);
	if (md > 0)
		return(1);
	else if (md < 0)
		return(-1);

	/* If both are MO, completed ones first */
	if ((p1->myattr&MYMISOBJ) && (p2->myattr&MYMISOBJ)) {
		md = (p2->attr&0x200) - (p1->attr&0x200);
		if (md > 0)
			return(1);
		else if (md < 0)
			return(-1);
	} 

	/* Then by type */
	md = (int)(p1->type) - (int)(p2->type);
	if (md > 0)
		return(1);
	else if (md < 0)
		return(-1);

	/* Then by attribute */
	md = (int)(p1->attr&0x7000) - (int)(p2->attr&0x7000);
	if (md > 0)
		return(1);
	else if (md < 0)
		return(-1);

	return(0);
}

void	sortem(pto)
	struct myhead	*pto;
{
	int	n, i, q, j, mnum, mn;
#ifdef	TU
	struct myobj	*pob;
	struct myobj	*pob2;
#else	/* !TU */
	struct myobj far	*pob;
	struct myobj far	*pob2;
#endif	/* !TU */
	extern int	isclose();
	extern void	mysort();

	n = pto->nobj;
 	mnum = 1;
	/*
	 *	First, gather all friendlies near airstrip
	 */
	for (pob = pto->pob, i = 0; i < n; i++, pob++) {
		if (pob->type == 6) {
			mn = mnum++;
			pob->myattr |= mn;
			pob2 = pto->pob;
			for (j = 0; j < n; j++, pob2++) {
				if (oobjs[pob2->type].flag&ENEMY)
					continue;
				if (isclose(pob, pob2))
					pob2->myattr |= mn;
			}
			break;
		}
	}

	/*
	 *	Now, gather all objects near MO's
	 */
	for (pob = pto->pob, i = 0; i < n; i++, pob++) {
		switch (pob->type) {
		case 58:	/* Portal */
			q = pob->attr&0x7000;
			if (!(q == 0x4000 || q == 0x7000))
				continue;
		case 0:		/* Orb */
		case 43:	/* Destroyed Orb */
		case 53:	/* MO Saucer */
			pob->myattr |= MYMISOBJ;
			mn = pob->myattr&MYMNUM;
			if (mn == 0) {
				pob->myattr |= mnum;
				mn = mnum;
				mnum++;
			}

			pob2 = pto->pob;
			for (j = 0; j < n; j++, pob2++) {
				if (pob2->myattr&MYMNUM)
					continue;
				if (isclose(pob, pob2))
					pob2->myattr |= mn;
			}
		}
	}

	/*
	 *	Now, gather all remaining objs
	 */
	for (pob = pto->pob, i = 0; i < n; i++, pob++) {
		if (pob->myattr&MYMNUM)
			continue;
		pob->myattr |= mnum;
	}

	mysort(pto->pob, n, cmpob);
}

int	isclose(p1, p2)
#ifdef	TU
	struct myobj	*p1;
	struct myobj	*p2;
#else	/* !TU */
	struct myobj far	*p1;
	struct myobj far	*p2;
#endif	/* !TU */
{
	long	x, y;
	extern long	labs();

	x = labs(p2->x - p1->x);
	y = labs(p2->y - p1->y);
	if (x < MOGROUPD && y < MOGROUPD)
		return(1);
	else
		return(0);
}

void	mysort(p, n, f)
#ifdef	TU
	struct myobj	*p;
#else	/* !TU */
	struct myobj far	*p;
#endif	/* !TU */
	unsigned int	n;
	int	(*f)();
{
	struct myobj	*p1;
	unsigned int	nb;
	int	i;
	extern void	*malloc(), free(), qsort();
	extern unsigned int	coreleft();

	if (n == 0)
		return;
	nb = n*sizeof(struct myobj);
	if ((p1 = malloc(nb)) == NULL) {
		errorbox("Cannot alloc %u bytes to sort %d objs cl=%u",
						nb, n, coreleft());
		return;
	}
	for (i = 0; i < n; i++)
		p1[i] = p[i];
	qsort((void *)p1, n, sizeof(struct myobj), f);
	for (i = 0; i < n; i++)
		p[i] = p1[i];
	free(p1);
}

char	*misn_orbs[] = {
	"destroy this orb.",
	"blow up the nearby TRFRU/fuel dumps.",
	"eliminate the nearby KLA and AAA.",
	"kill all nearby alien ground vehicles.",
	"destroy this orb with a suicide plane.",
	"blast the nearby saucer sites.",
	"destroy all nearby alien mind-benders.",
	"kill the nearby congregation of aliens.",
};

char	*shell_types[] = {
	"vehicle site",
	"mortar",
	"KLA site",
	"AAA site",
	"barrage balloon",
	"bee-swarm",
	"fuel dump",
	"unknown type",
};

void	objout(pob)
#ifdef	TU
	struct myobj	*pob;
#else	/* !TU */
	struct myobj far	*pob;
#endif	/* !TU */
{
	struct oobj	*pb;
	long	x, y, z;
	int	q, qflag;
	char	*d, *p, buf[LINELEN], buf2[LINELEN];
	extern char	*cdist(), *cdir();

	if (pob->type >= (sizeof(oobjs)/sizeof(struct oobj)))
		return;
	pb = &oobjs[pob->type];
	if (!(pb->flag&SHOWN))
		return;

	qflag = 0;
	if (pb->flag&ENEMY)
		sprintf(buf, "Enemy ");
	else
		buf[0] = '\0';
	strcat(buf, pb->name);

	q = ((pob->attr&0x7000)>>12) + 1;
	switch (pob->type) {
	case 0:					/* Orb */
		if (q == 5)
			strcat(buf, " (Indestructible)");
		else if (pob->attr&0x800)
			strcat(buf, " (Gravity)");
		break;
	case 2:					/* Plane */
		switch (q) {
		case 1:
			p = "Corsair";
			break;
		case 2:
			p = "Dumpy";
			break;
		case 3:
			p = "Stealth";
			break;
		default:
			p = "Unknown type";
			break;
		}
		sprintf(buf2, " (%s)", p, pob->attr);
		strcat(buf, buf2);
		if (pob->attr&0x100)
			strcat(buf, " (Wrecked)");
		break;
	case 7:					/* Ground Crew */
		if (pob->attr&0x800)
			strcat(buf, " (Spy)");
		if (q == 2)
			strcat(buf, " (Dead)");
		break;
	case 12:				/* Vehicle */
		qflag = 1;
		if ((pob->attr&0xff) == 0x71)
			strcat(buf, " (Dead)");
		else if (pob->attr&0x800)
			strcat(buf, " (bumpers)");
		break;
	case 32:				/* Crater */
		if (q != 1)
			return;
		break;
	case 49:
		sprintf(buf2, " (%s)", shell_types[q-1]);
		strcat(buf, buf2);
		break;
	case 11:				/* KLA-Site */
	case 13:				/* Vehical Site */
	case 23:				/* AAA Site */
	case 24:				/* KLA */
	case 50:				/* Saucer */
	case 51:				/* Saucer-Site */
	case 53:				/* Saucer */
	case 58:				/* Portal */
		qflag = 1;
		break;
	default:
		break;
	}
	if (qflag)
		fprintf(lfp, "%-27.27s Type \\%c   ", buf, q + '0');
	else
		fprintf(lfp, "%-36.36s ", buf);

	x = pob->x - ox;
	y = pob->y - oy;
	p = cdir(-y, x);	/* Because of kevin's coordinates */
	x /= 16090;		/* Tenths of miles */
	y /= 16090;
	d = cdist(x, y);
	z = pob->z*10;
	z /= 305;		/* Feet */
	fprintf(lfp, "%4.4s %3s    %5ld", d, p, z);
	fprintf(lfp, "             (%d)", (pob->myattr&MYMNUM));
	fprintf(lfp, "\r\n");

	switch (pob->type) {
	case 0:		/* Orb */
	case 43:	/* Destroyed Orb */
		p = misn_orbs[(pob->attr&0x7000)>>12];
		break;
	case 58:	/* Portal */
		if (q == 5 || q == 8)
			p = "Fly through this portal.";
		else
			p = NULL;
		break;
	case 53:	/* Saucer */
		p = "Blast this saucer out of the sky.";
		break;
	case 7:	/* Spy */
		if (pob->attr&0x800)
		   p = "Get secret document from spy.";
                  else p = NULL;
		break;
	default:
		p = NULL;
		break;
	}
	if (p != NULL && (pob->attr&0x200) == 0)
		fprintf(lfp, "\\(         Mission Objective:  %s\\)\r\n", p);
}

char	*cdist(x, y)
	long	x, y;
{
	static char	buf[8];
	char	*p;
	extern long	labs();
	extern int	isqrt();

	sprintf(buf, "%d", isqrt((int)(labs(x)+labs(y)), x*x + y*y));
	for (p = buf; *p; p++);
	if (p == buf)
		return("0.0");
	*(p+1) = '\0';
	*p = *(p-1);
	*(p-1) = '.';
	return(buf);
}

int	isqrt(x1, r2)
	int	x1;
	long	r2;
{
	int	x0, n;

	if (r2 == 0)
		return(0);
	n = 0;
	do {
		x0 = x1;
		x1 = (int)(x0 + r2/x0)/2;
		n++;
	}
	while( (n < 100) && (x1 != x0));
	return(x1);
}

char	qs[4][2] = {
	'N', 'E',
	'S', 'E',
	'N', 'W',
	'S', 'W',
};

char	*cdir(x, y)
	long	x, y;
{
	char	*q, q1, q2;
	static char	buf[4];
	extern long	labs();

	if (x > 0) {
		if (y > 0)
			q = qs[0];
		else if (y < 0)
			q = qs[1];
		else
			return("E");
	}
	else if (x < 0) {
		if (y > 0)
			q = qs[2];
		else if (y < 0)
			q = qs[3];
		else
			return("W");
	}
	else {
		if (y > 0)
			return("N");
		else if (y < 0)
			return("S");
		else
			return("na");
	}
	x = labs(x);
	y = labs(y);
	q1 = q[0];
	q2 = q[1];
	if (y > x) {
		y *= 100;
		y /= x;
		if (y > 503) {
			buf[0] = q1;
			buf[1] = '\0';
		}
		else if (y > 150) {
			buf[0] = q1;
			buf[1] = q1;
			buf[2] = q2;
			buf[3] = '\0';
		}
		else {
			buf[0] = q1;
			buf[1] = q2;
			buf[2] = '\0';
		}
	}
	else {
		x *= 100;
		x /= y;
		if (x > 503) {
			buf[0] = q2;
			buf[1] = '\0';
		}
		else if (x > 150) {
			buf[0] = q2;
			buf[1] = q1;
			buf[2] = q2;
			buf[3] = '\0';
		}
		else {
			buf[0] = q1;
			buf[1] = q2;
			buf[2] = '\0';
		}
	}
	return(buf);
}
