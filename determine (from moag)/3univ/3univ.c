#include <stdio.h>
#include <stdlib.h>
/* #include <dos.h> */
#define	ntilesx	8
#define	ntilesy 8
#define maxtilex ntilesx-1
#define maxtiley ntilesy-1
#define tileszx	32
#define tileszy 32
#define nbyteobj 23
#define nobjtile 150
#define nparatile (nbyteobj*nobjtile+15)/16
#define nparaline nparatile*ntilesx
#define nheaderbytes 16
#define universex -128
#define universey -128

long	getobjx();
long	getobjy();
long	getobjz();
long	getdist();
void	typeobj();

main(int argc, char *argv[])
{

struct  markedtile {
        int tilex;
        int tiley;
        int maxnobj;
        };

struct	coords {
	long ox;
	long oy;
	long oz;
	};

struct	objheader {
        char cksum[4]; 
	int tilexid;
	int tileyid;
	unsigned int nobjintile;
        int reserved[3];
	};

struct	cobject {
	int xyhigh;
	int xlow;
     	int ylow;
	int zlow;
	char yaw;
        char pitch;
	char roll;
	int objattr;
	int xvel;
	int yvel;
	int zvel;
	int objtime;
	int objtype;
};

FILE	*input_file;
char	towerfname[]="3univ.dat";
char	buffer[81];
int	i,nskip;
int	itilex,itiley;
int	objnum;
int	maxplns,totalmaxplns,ncloseplanes;
int	nobjectives,ncobjectives;
int	totalnobj;

struct	cobject thisobj;
struct	objheader thisheader;
struct	markedtile maxtile;
struct	markedtile maxplnstile;
struct	coords startcoords;


long	objx,objy,objz;
long	distance;

	  input_file=fopen(towerfname,"rb");
	  if(input_file == NULL) {
             puts("Error: cannot open 3univ.dat file");
             exit(1);
	  }
		/* First try reading the starting coords */
		/* These should be the last 12 bytes in the file */
		if(fseek(input_file,-12,SEEK_END) != 0 )
                   { printf("Error in seeking file"); exit(1); }
                  if (fread(&startcoords,sizeof(startcoords),
                      1,input_file) != 1)
                    { printf("Unexpected startcoord EOF \n"); exit(1); }
		if(fseek(input_file,0,SEEK_SET) != 0 )
                   { printf("Error in seeking file"); exit(1); }

        maxtile.maxnobj=0;
        maxtile.tilex=0;
        maxtile.tiley=0;
        maxplnstile.maxnobj=0;
        maxplnstile.tilex=0;
        maxplnstile.tiley=0;
	totalmaxplns=0;
	totalnobj=0;
	ncloseplanes=0;
	for (itiley=0; itiley < ntilesy; itiley++)
          {
  	  for (itilex=0; itilex < ntilesx; itilex++)
            {
	    maxplns=0;
	    if (fread(&thisheader,sizeof(thisheader),1,input_file) != 1)
              { printf("Unexpected hdr EOF \n"); exit(1); }
	    if((thisheader.tilexid != itilex) ||
                   (thisheader.tileyid != itiley))
	      {
 	        printf("(%d,%d)",itilex,itiley);
                printf(": (%d,%d) ",thisheader.tilexid,thisheader.tileyid);
                printf(" Bogus tile header! \n");
                exit(1);
              }


              if (thisheader.nobjintile > 0 )
              {
  	        for (objnum=0; objnum < thisheader.nobjintile; objnum++)
                  { 

                  if (fread(&thisobj,sizeof(thisobj),1,input_file) != 1)
                    { printf("Unexpected obj EOF \n"); exit(1); }

		  totalnobj++;

						  /* is this object a plane? */
						/* and if so, is it wrecked? */
		  if( (thisobj.objtype == 2) &&
                        ((thisobj.objattr & 0x100) == 0) )
		    { 
                    maxplns++; totalmaxplns++;
		    distance=getdist(itilex,itiley,thisobj,startcoords);
		    if (distance != -1) ncloseplanes++;
                    }

		  if( (thisobj.objtype == 43) | (thisobj.objtype == 0)) {
		    if( (thisobj.objattr & 0x200) == 0) {
		      nobjectives++;
		      printf("\nIncomplete Mission objective in tile (%d,%d)"
	                ,itilex,itiley); }
                    else
                      { ncobjectives++; }
		    }

		  if( thisobj.objtype == 53) {
		    if( (thisobj.objattr & 0x200) == 0) {
		      nobjectives++;
		      printf("\nIncomplete Mission objective in tile (%d,%d)"
	                ,itilex,itiley); }
                    else
                      { ncobjectives++; }
		    }

		  if( thisobj.objtype == 1) {
		    if( (thisobj.objattr & 0x200) == 0) {
		      nobjectives++;
		      printf("\nIncomplete Mission objective in tile (%d,%d)"
	                ,itilex,itiley); }
                    else
                      { ncobjectives++; }
		    }

                  }
              }


/*	      typeobj(itilex,itiley,thisobj); */

              if(maxtile.maxnobj < thisheader.nobjintile) 
                {
		   maxtile.maxnobj=thisheader.nobjintile;
		   maxtile.tilex=itilex;
		   maxtile.tiley=itiley;
                }

              if(maxplnstile.maxnobj < maxplns) 
                {
		   maxplnstile.maxnobj=maxplns;
		   maxplnstile.tilex=itilex;
		   maxplnstile.tiley=itiley;
                }

	    }
	  }

                  if (fread(&startcoords,sizeof(startcoords),
                      1,input_file) != 1)
                    { printf("Unexpected startcoord EOF \n"); exit(1); }

	printf("\n");
	printf(" Starting Coords: %ld %ld\n",startcoords.ox,startcoords.oy);
	printf(" Number of currently accessible healthy planes : %d\n"
              ,ncloseplanes);

	printf("Total number of objects in universe : %d \n",
           totalnobj);
	printf("Max number of objects in tile (%d,%d) : %d \n",
           maxtile.tilex,maxtile.tiley,maxtile.maxnobj);
	printf("Total number of non-wrecked airplanes in universe : %d \n",
           totalmaxplns);
	printf("Max number of non-wrecked airplanes in tile (%d,%d) : %d \n",
           maxplnstile.tilex,maxplnstile.tiley,maxplnstile.maxnobj);
	printf("Number of mission objectives remaining in universe : %d\n",
           nobjectives);
	printf("Number of completed mission objectives in universe : %d\n",
           ncobjectives);
}

long	getobjx(curtilex,curtiley,thatobj)

struct	cobject {
	int xyhigh;
	int xlow;
     	int ylow;
	int zlow;
	char yaw;
        char pitch;
	char roll;
	int objattr;
	int xvel;
	int yvel;
	int zvel;
	int objtime;
	int objtype;
};

struct cobject thatobj;
int curtilex,curtiley;

 {
long temp;
   temp= curtilex*tileszx+universex;
   temp= temp + (thatobj.xyhigh & (tileszx-1));
   temp= (temp << 16) + thatobj.xlow;
 return(temp); }

long	getobjy(curtilex,curtiley,thatobj)

struct	cobject {
	int xyhigh;
	int xlow;
     	int ylow;
	int zlow;
	char yaw;
        char pitch;
	char roll;
	int objattr;
	int xvel;
	int yvel;
	int zvel;
	int objtime;
	int objtype;
};

struct cobject thatobj;
int curtilex,curtiley;

 {
long temp;
   temp= curtiley*tileszy+universey;
   temp= temp + ((thatobj.xyhigh >> 5) & (tileszy-1));
   temp= (temp << 16) + thatobj.ylow;
 return(temp); }

long	getobjz(curtilex,curtiley,thatobj)

struct	cobject {
	int xyhigh;
	int xlow;
     	int ylow;
	int zlow;
	char yaw;
        char pitch;
	char roll;
	int objattr;
	int xvel;
	int yvel;
	int zvel;
	int objtime;
	int objtype;
};

struct cobject thatobj;
int curtilex,curtiley;

 {
long temp;
   temp= ((thatobj.xyhigh >> 10) & (tileszx-1));
   temp= (temp << 16) + thatobj.zlow;
 return(temp); }


long	getdist(curtilex,curtiley,thatobj,thatstart)

struct	cobject {
	int xyhigh;
	int xlow;
     	int ylow;
	int zlow;
	char yaw;
        char pitch;
	char roll;
	int objattr;
	int xvel;
	int yvel;
	int zvel;
	int objtime;
	int objtype;
};

struct	coords {
	long ox;
	long oy;
	long oz;
	};

struct cobject thatobj;
struct coords thatstart;
int curtilex,curtiley;

 {
long deltax,deltay,deltaz,temp;
   deltax= getobjx(curtilex,curtiley,thatobj) - thatstart.ox;
   deltay= getobjy(curtilex,curtiley,thatobj) - thatstart.oy;
   deltaz= getobjz(curtilex,curtiley,thatobj) - thatstart.oz;
   temp= labs(deltax) << 1;
   temp= temp | (labs(deltay) << 1);
   temp= temp | (labs(deltaz) << 1);
   temp= (temp >> 16) & 65535;
   if((temp & 0x0fff0) != 0) return(-1);      /* obviously not a near object */

   if(temp == 0) {		/* However, this obj must be real close! */
     temp= (deltax*deltax)+(deltay*deltay)+(deltaz*deltaz); 
     return(temp); }
			/* so object neither near nor far, must be med */
   deltax=labs(deltax);
   deltay=labs(deltay);
   deltaz=labs(deltaz);
   temp= (deltax >> 4) * (deltax >> 4);
   temp= temp + (deltay >> 4) * (deltay >> 4);
   temp= temp + (deltaz >> 4) * (deltaz >> 4);
   return(temp); }

void	typeobj(curtilex,curtiley,thatobj)

struct	cobject {
	int xyhigh;
	int xlow;
     	int ylow;
	int zlow;
	char yaw;
        char pitch;
	char roll;
	int objattr;
	int xvel;
	int yvel;
	int zvel;
	int objtime;
	int objtype;
};

struct cobject thatobj;
int curtilex,curtiley;

 {
	    printf("( %d, %d)  %ld %ld %ld\n",
curtilex,curtiley
,getobjx(curtilex,curtiley,thatobj)
,getobjy(curtilex,curtiley,thatobj)
,getobjz(curtilex,curtiley,thatobj)
);
  
  }
