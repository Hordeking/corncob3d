
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <malloc.h>

extern int LOADSOADL();
extern void VOICEON();
extern void VOICEOFF();
extern int ADLINIT();



main()
{
int	exitcode,result,i,ii;
int 	freqp[12],curvoice,basep[12];
unsigned char akey;
unsigned char kbuf[256];
unsigned char far *farmemptr;

printf("hello\n");

farmemptr=farmalloc(20000L);
if(farmemptr==NULL) {printf("Error allocating Far RAM for SOADL"); exit(0); }

exitcode= 0xff & LOADSOADL(farmemptr);
if(exitcode != 0) {printf("Error loading SOADL.EXE driver\n"); exit(0);}

ADLINIT();		/* Initialize the sound driver */


printf("About to turn the engine on...");
ADLWRT(0x40,16);
VOICEON(0,0x308);
printf("have turned the engine on...");
sleep(1);
for(i=0;i<500;i++) {
   VOICEON(0,0x300+ (int) (8. * cos((float) i*3.14/500.)));

   for(ii=0;ii<100;ii++) { result=(int) 10.*sin((float) i)*cos((float) i);}

   if(kbhit()) break;
}
getch();
VOICEOFF(0,0x200);
exit(0);
/*
*/
for(i=0;i<12;i++) {
freqp[i]=5;
basep[i]=80;
}
curvoice=0;
ADLWRT(0x40,16);	/* Make engine sound nice */
clrscrn();



while ( 1==1 ) {
akey=getch();
if( (akey>0x30) && (akey < 0x3a)) { 
        freqp[curvoice]=(int) akey;
        VOICEOFF(curvoice,freqp[curvoice]*basep[curvoice]/10+basep[curvoice]);
        VOICEON(curvoice,freqp[curvoice]*basep[curvoice]/10+basep[curvoice]);
}

if( akey == 0x76 )
  {
     poscurs(12,0);
     printf("                                 ");
     poscurs(12,0);
     printf("Enter the desired voice : ");
     curvoice=atoi(gets(kbuf));
  }

if( akey == 0x62 )
  {
     poscurs(12,0);
     printf("                                 ");
     poscurs(12,0);
     printf("Enter the base value : ");
     basep[curvoice]=atoi(gets(kbuf));
     if(basep[curvoice] == 0) basep[curvoice]=0x200;
  } 

if(akey == 0x6f)
        VOICEOFF(curvoice,freqp[curvoice]*basep[curvoice]/10+basep[curvoice]);

if(akey == 0x64) ADLWRT(0x40,4);

if(akey == 0x1b)
        break;

poscurs(0,0);
printf("Curvoice : %d    ",curvoice);
}

for(i=0;i<12;i++) {
        VOICEOFF(i,freqp[i]*basep[i]/10+basep[i]);
}
     poscurs(23,0);


}
