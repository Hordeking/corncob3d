#include	<stdio.h>
#define	NTILESX		8
#define	NTILESY		8
#define	MAXTILEX	NTILESX-1
#define	MAXTILEY	NTILESY-1
#define	TILESZX		32
#define	TILESZY		32
#define	NBYTEOBJ	23
#define	NOBJTILE	150
#define	NPARATILE	(NBYTEOBJ*NOBJTILE+15)/16
#define	NPARALINE	NPARATILE*NTILESX
#define	NHEADERBYTES	16
#define	UNIVERSEX	-128
#define	UNIVERSEY	-128

extern long	getobjx();
extern long	getobjy();
extern long	getobjz();
extern long	getdist();

#define	OZ	720L
struct coords {
	long	ox;
	long	oy;
	int	ncloseplanes;
} startcoords[9];

struct objheader {
	char	cksum[4];
	int	tilexid;
	int	tileyid;
	unsigned int	nobjintile;
	int	reserved[3];
};

struct cobject {
	int	xyhigh;
	int	xlow;
	int	ylow;
	int	zlow;
	char	yaw;
	char	pitch;
	char	roll;
	int	objattr;
	int	xvel;
	int	yvel;
	int	zvel;
	int	objtime;
	int	objtype;
};

main() {
	char	towerfname[] = "3univ.dat";
	int	i, objnum, itilex, itiley;
	int	totalmaxplns, totalnobj;
	long	distance, starthere[3];
	struct cobject	thisobj;
	struct objheader	thisheader;
	FILE	*input_file;
	extern long	startcx(), startcy();

	input_file = fopen(towerfname, "rb");
	if (input_file == NULL) {
		puts("Error: cannot open 3univ.dat file");
		exit(1);
	}

	i = 0;
	for (itiley = 0; itiley < 3; itiley++) {
		for (itilex = 0; itilex < 3; itilex++) {
			startcoords[i].ox = startcx(itilex);
			startcoords[i].oy = startcy(itiley);
			i++;
		}
	}

	totalmaxplns = 0;
	totalnobj = 0;
	for (itiley = 0; itiley < NTILESY; itiley++) {
	    for (itilex = 0; itilex < NTILESX; itilex++) {
		if (fread(&thisheader, sizeof(thisheader), 1, input_file) != 1){
			printf("Unexpected hdr EOF \n");
			exit(1);
		}
		if ((thisheader.tilexid != itilex) ||
					    (thisheader.tileyid != itiley)) {
			printf("(%d,%d)", itilex, itiley);
			printf(": (%d,%d) ", thisheader.tilexid,
							thisheader.tileyid);
			printf(" Bogus tile header! \n");
			exit(1);
		}

		for (objnum = 0; objnum < thisheader.nobjintile; objnum++) {
			if (fread(&thisobj, sizeof(thisobj), 1, input_file)!=1){
				printf("Unexpected obj EOF \n");
				exit(1);
			}

			totalnobj++;

			/* is this object ain't a plane
			 * or if so, if is it's wrecked, then shine it.
			 */
			if ((thisobj.objtype != 2) || (thisobj.objattr&0x100))
				continue;

			totalmaxplns++;
			for (i = 0; i < 9; i++) {
				distance = getdist(itilex, itiley,
						&thisobj, &startcoords[i]);
				if (distance != -1)
					startcoords[i].ncloseplanes++;
			}
		}
	    }
	}

	if (fread(starthere, sizeof(long), 3, input_file) != 3) {
		printf("Unexpected startcoord EOF \n");
		exit(1);
	}

 	for (i = 0; i < 9; i++) {
		printf("location %d  x=%9ld  y=%9ld    ncloseplanes=%d\n",
					i, startcoords[i].ox, startcoords[i].oy,
					startcoords[i].ncloseplanes);
	}
	printf("Total number of objects in universe : %d \n",
	    totalnobj);
	printf("Total number of non-wrecked airplanes in universe : %d \n",
	    totalmaxplns);
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
	struct coords	*thatstart;
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
