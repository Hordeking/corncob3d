
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/* #include <dos.h> */
main(int argc, char *argv[])
{
FILE	*input_file;
char	towerfname[]="3.mak";
char	buffer[81];
int	i,xrv,lrv;
time_t	timer;

	time(&timer);
	  input_file=fopen(towerfname,"r");
	  if(input_file == NULL) {
             puts("Error: cannot open username.dat file");
             exit(1);}

	xrv=(timer&31)*10/32;
	lrv=(((((timer&127)*(timer&127))>>3)+timer&15)&31)*10/32;
	lrv=3;


/* skip two lines */
	    if(fgets(buffer,80,input_file) != NULL) { }
	    if(fgets(buffer,80,input_file) != NULL) { }
	printf("mxrv=%d\n",xrv);
	printf("mlrv=%d\n",lrv);

	while (fgets(buffer,80,input_file) != NULL) {
	      if(strlen(buffer) != 0) {
	        buffer[strlen(buffer)-1]='\0';
                puts(buffer);
	        }	
	     }
}
