
#include <stdio.h>
/* #include <dos.h> */
main(int argc, char *argv[])
{
FILE	*input_file;
char	towerfname[]="username.dat";
char	buffer[81];
int	i;

	  input_file=fopen(towerfname,"r");
	  if(input_file == NULL) {
             puts("Error: cannot open username.dat file");
             exit(1);}


	    if(fgets(buffer,80,input_file) != NULL) {
	printf("\nmcpywmsg MACRO");
	printf("\ncpywmsg	db	%d",buffer[0]-0x50); 
	for (i=1; i < 80; i++) {
	  if(buffer[i] < 0x20) break;
	printf("\n	db	%d",buffer[i]-0x50); }
	printf("\n	db	%d,%d",0x0d-0x50,0x0a-0x50);  }

	    if(fgets(buffer,80,input_file) != NULL) {
	for (i=0; i < 80; i++) {
	  if(buffer[i] < 0x20) break;
	printf("\n	db	%d",buffer[i]-0x50); }
	printf("\n	db	%d,%d,%d",0x0d-0x50,0x0a-0x50,0-0x50);
	printf("\nENDM"); }


	    if(fgets(buffer,80,input_file) != NULL) {
	    
	printf("\nmlettercheck1 MACRO");
	printf("\nlettercheck1	db	%d",buffer[0]-0x50);
	printf("\nENDM");
	printf("\nmlettercheck2 MACRO");
	printf("\nlettercheck2	db	%d",buffer[2]-0x50);
	printf("\nENDM");
	printf("\nmusername MACRO");
	printf("\nIF userflag");
	printf("\nusername	db	%d",buffer[0]-0x50); 
	for (i=1; i < 40; i++) {
	  if(buffer[i] < 0x20) break;
	printf("\n	db	%d",buffer[i]-0x50); }
	printf("\n	db	%d,%d",0x0d-0x50,0x0a-0x50); }

	    if(fgets(buffer,80,input_file) != NULL) {
	for (i=0; i < 40; i++) {
	  if(buffer[i] < 0x20) break;
	printf("\n	db	%d",buffer[i]-0x50); }
	printf("\n	db	%d,%d",0x0d-0x50,0x0a-0x50); }

	    if(fgets(buffer,80,input_file) != NULL) {
	for (i=0; i < 40; i++) {
	  if(buffer[i] < 0x20) break;
	printf("\n	db	%d",buffer[i]-0x50); }
	printf("\n	db	%d,%d",0x0d-0x50,0x0a-0x50); }

	    if(fgets(buffer,80,input_file) != NULL) {
	for (i=0; i < 40; i++) {
	  if(buffer[i] < 0x20) break;
	printf("\n	db	%d",buffer[i]-0x50); } }
	printf("\n	db	%d,%d,%d",0x0d-0x50,0x0a-0x50,0-0x50);
	printf("\nENDIF\n");
	printf("\nENDM\n");
}

