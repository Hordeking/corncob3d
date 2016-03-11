/*
	 Copyright Ad Lib Inc., 1990

	 This file is part of the Ad Lib Programmer's Manual product and is
	 subject to copyright laws.  As such, you may make copies of this file
	 only for the purpose of having backup copies.  This file may not be
	 redistributed in any form whatsoever.

	 If you find yourself in possession of this file without having received
	 it directly from Ad Lib Inc., then you are in violation of copyright
	 laws, which is a form of theft.
*/

/*
	Routines to deal with bank file (DJG, MSA).
	Dec-88

Public routines (read only access to bank):
int Open_Bank (fname, writeFlag, bkp)
int Close_Bank( bkp)
int Read_Timbre_def (ins_name, timbre, bkp)
int Seek_Nth_Timbre (n, bkp)

Public routines (read/write access to bank):
(BANK_WRITE_ACCESS must be defined in order to use these routines.)
int Open_New_Bank (name, bkp)
int Write_Timbre (ins_name, timbre, bkp)
int Delete_Timbre (ins_name, bkp)
int Read_timbre_name (n, name, bkp)

Local routines:
static int Header_Update( bkp)
static Seek_Nth_Entry (n, bkp)
static unsigned Get_Nth_Entry (n, entry, bkp)
static int  Entry_Search (ins_name, ref, bkp)
static int Expand_File (nrAdd, bkp)
static unsigned Insert_Entry (ins_name, bkp)
static int Delete_Entry( ins_name, bkp)
*/

#include  <dos.h>
#include  <fcntl.h>
#include  <sys\types.h>
#include  <sys\stat.h>
#include  <stdio.h>
#include  <string.h>
#include  "insbank.h"

#define  EXPAND_NR  32

/* The next line should be included if write access to the bank is desired. */
/* #define  BANK_WRITE_ACCESS */

extern int errno;			/* DOS error code */

/* Prototypes */
static unsigned Insert_Entry (char *ins_name, BankPtr bkp);
static unsigned Get_Nth_Entry (unsigned n, BankEntry * entry, BankPtr bkp);
static int  Entry_Search (char *, unsigned *, BankPtr);


/*------------------------------------------------------------------------
	Open a bank file.

	'fname' is the full path-name, including extension;
	'writeFlag' is true if the bank is opened for writing;
	'bkp' is the pointer to the bank descriptor, modified by this function.

	Return BANK_OK if no errors.
*/
int Open_Bank (fname, writeFlag, bkp)
	char *fname;
	int writeFlag;
	BankPtr bkp;
{
	int id;
	int wr;

	wr = writeFlag ? O_RDWR : O_RDONLY;
	id = open (fname, wr + O_BINARY, S_IWRITE);
	if (id != -1) {
		read (id, &bkp->head, sizeof (BankHeader));
		if( 0 != strncmp( bkp->head.sig, BANK_SIG, BANK_SIG_LEN)) {
			close( id);
			return BAD_BANK_FILE;
			}
		bkp->bank_id = id;
		return BANK_OK;
		}
	return BANK_NOT_FOUND;
}


#ifdef BANK_WRITE_ACCESS
/*
	Create an new empty bank of instrument.
*/
int Open_New_Bank( name, bkp)
	char * name;		/* full path & name */
	BankPtr bkp;
	{
	int id;
	BankHeader * hd;
	int olderr, res;

	id = open( name, O_BINARY | O_RDWR | O_CREAT | O_EXCL, S_IWRITE);
	if( -1 == id)
		return CREATE_ERROR;
	bkp->bank_id = id;

	hd = &bkp->head;
	hd->majorVersion = MAJ_VERSION;
	hd->minorVersion = MIN_VERSION;
	memmove( hd->sig, BANK_SIG, BANK_SIG_LEN);
	hd->nrDefined = 0;
	hd->nrEntry = 0;
	hd->offsetIndex = sizeof( BankHeader);
	hd->offsetTimbre = sizeof( BankHeader);
	memset( hd->filler, 0, BANK_FILLER_SIZE);
	res = Header_Update( bkp);
	if( res != BANK_OK) {
		olderr = errno;
		remove( name);
		errno = olderr;
		}
	return res;
	}
#endif


/*------------------------------------------------------------------------
Close a bank file. */

int Close_Bank( bkp)
	BankPtr bkp;
{
   int result;
   result = close( bkp->bank_id);
   bkp->bank_id = -1;
   return (result);
}

#ifdef BANK_WRITE_ACCESS
/*
	Return the name of the 'n'th timbre.
*/
int Read_timbre_name( n, name, bkp)
	unsigned n;			/* index of timbre */
	char * name;		/* output name */
	BankPtr bkp;
	{
	BankEntry entry;

	if( !Get_Nth_Entry( n, &entry, bkp))
		return NOT_DEFINED;
	strcpy( name, entry.TimbreName);
	return BANK_OK;
	}
#endif


/*------------------------------------------------------------------------
	Read an instrument record.
*/
int Read_Timbre_def (ins_name, timbre, bkp)
	char *ins_name;
	Timbre *timbre;
	BankPtr bkp;
{
	BankEntry entry;
	unsigned ptr;
	PackedTimbre pt;
	int * intP, i;

   if (!Entry_Search (ins_name, &ptr, bkp))
	  return NOT_DEFINED;

   Seek_Nth_Timbre (ptr, bkp);
   read ( bkp->bank_id, &pt, sizeof (PackedTimbre));

	timbre->mode = pt.mode;
	timbre->percVoice = pt.percVoice;
	for( i = 0, intP = (int *) &timbre->op0; i < 13 * 2; i++)
		*intP++ = pt.param[ i];
	timbre->wave0 = pt.wave0;
	timbre->wave1 = pt.wave1;

   return BANK_OK;
}


#ifdef BANK_WRITE_ACCESS
/*------------------------------------------------------------------------
Write an instrument record.
*/
int Write_Timbre (ins_name, timbre, bkp)
	char *ins_name;
	Timbre *timbre;
	BankPtr bkp;
{
   BankEntry entry;
   unsigned ptr;
   int result;
	PackedTimbre pt;
	int * intP, i;

	pt.mode = timbre->mode;
	pt.percVoice = timbre->percVoice;
	for( i = 0, intP = (int *) &timbre->op0; i < 13 * 2; i++)
		pt.param[ i] = *intP++;
	pt.wave0 = timbre->wave0;
	pt.wave1 = timbre->wave1;

   if (!Entry_Search (ins_name, &ptr, bkp)) {
	  ptr = Insert_Entry (ins_name, bkp);
	  bkp->head.nrDefined++;
	  }
   Seek_Nth_Timbre (ptr, bkp);
   result = write ( bkp->bank_id, &pt, sizeof (PackedTimbre));

   if (result == sizeof (PackedTimbre))
		result = Header_Update( bkp);
	else
		result = READ_WRITE_ERR;
   return result;
}
#endif

#ifdef BANK_WRITE_ACCESS
/*
	Remove the instrument 'ins_name'.  Returns NOT_DEFINED if inexistant.
*/
int Delete_Timbre( ins_name, bkp)
	char * ins_name;
	BankPtr bkp;
{
	return Delete_Entry( ins_name, bkp) ? BANK_OK : NOT_DEFINED;
}

#endif




#ifdef BANK_WRITE_ACCESS
/*------------------------------------------------------------------------
The header has been updated, write it to file. */

static int Header_Update( bkp)
	BankPtr bkp;
{
	int l;

   lseek ( bkp->bank_id, 0L, SEEK_SET);
   l = write ( bkp->bank_id, &bkp->head, sizeof (BankHeader));
	return l == sizeof( BankHeader) ? BANK_OK : READ_WRITE_ERR;
}
#endif

/*------------------------------------------------------------------------
Do a seek in list of sound names */

static Seek_Nth_Entry (n, bkp)
   unsigned n;
	BankPtr bkp;
{
   long l;

   l = bkp->head.offsetIndex + n * sizeof (BankEntry);
   lseek ( bkp->bank_id, l, SEEK_SET);
   return (1);
}




/*------------------------------------------------------------------------*/
/* Get a name from the list of sound names */
static unsigned Get_Nth_Entry (n, entry, bkp)
   unsigned n;
   BankEntry *entry;
	BankPtr bkp;
{
   int is_open;
   
	Seek_Nth_Entry (n, bkp);

   if (n >= bkp->head.nrEntry) return (0);

   read ( bkp->bank_id, (char *) entry, sizeof (BankEntry));

   if (!entry->usedFlag) return (0);
   else return (1);
}




/*------------------------------------------------------------------------
Do a file seek given a pointer to a timbre record. */
int Seek_Nth_Timbre (n, bkp)
   unsigned n;
   BankPtr bkp;
{
   long l;
   l = bkp->head.offsetTimbre + n * sizeof (PackedTimbre);
   lseek ( bkp->bank_id, l, SEEK_SET);
}


/*------------------------------------------------------------------------
Find an instrument in a bank file and do a seek so that the timbre record
is ready to be read. Returns 0 if not found, else returns 1 and reference
number in passed variable. */

static int  Entry_Search (ins_name, ref, bkp)
   char *ins_name;
   unsigned  *ref;
   BankPtr bkp;
{
   BankEntry entry;
   long top, bottom, mid;
   int diff;

   /* search for name */
   top = 0;
   bottom = bkp->head.nrDefined - 1;
   mid = bottom >> 1;
   do {
	  Get_Nth_Entry ((unsigned int) mid, &entry, bkp);
	  diff = strcmpi (ins_name, entry.TimbreName);
	  if (diff) {
		  if (diff < 0) bottom = mid - 1;
			 else top = mid + 1;
		  mid = (bottom + top) >> 1;
	  }
   } while (diff && top <= bottom);

   if (diff) return (0);
   *ref = entry.nrReference;
   return (1);
}


#ifdef BANK_WRITE_ACCESS
/*------------------------------------------------------------------------
There is no more space left in the section containing the instrument
names.  Expand the file.
*/
static int Expand_File (nrAdd, bkp)
   int nrAdd;
	BankPtr bkp;
{
   BankEntry entry;
   PackedTimbre timbre;
   long l, newSize;
   int i, xsize;
   unsigned new_ref;

   xsize = nrAdd * sizeof (BankEntry);

   /* calculate current end of file */
   l = bkp->head.offsetTimbre + bkp->head.nrEntry * sizeof (PackedTimbre);

	/* verify if enough space: */
	newSize = l + xsize + nrAdd * sizeof( PackedTimbre);
	if( -1 == chsize( bkp->bank_id, newSize))
		return READ_WRITE_ERR;

   /* move timbre data further in file */
   i = bkp->head.nrEntry;
   for (; i; i--) {
	  l -= sizeof (PackedTimbre);
	  lseek ( bkp->bank_id, l, SEEK_SET);
	  read ( bkp->bank_id, &timbre, sizeof (PackedTimbre));
	  lseek ( bkp->bank_id, l + xsize, SEEK_SET);
	  write ( bkp->bank_id, &timbre, sizeof (PackedTimbre));
   }

   /* set file pointer */
   new_ref = bkp->head.nrEntry;
   Seek_Nth_Entry (new_ref, bkp);

   /* initialize an empty entry */
   entry.usedFlag = 0;
   memset (entry.TimbreName, 0, 9);

   /* write empty entries */
   for (i=nrAdd; i; i--) {
	  entry.nrReference = new_ref;
	  write ( bkp->bank_id, &entry, sizeof (BankEntry));
	  new_ref++;
   }

   /* change and write header */
   bkp->head.offsetTimbre += xsize;
   bkp->head.nrEntry += nrAdd;
   return Header_Update( bkp);
}
#endif

#ifdef BANK_WRITE_ACCESS
/*------------------------------------------------------------------------
Insert an entry into the instrument list.  Returns the reference number
for the timbre data.
*/
static unsigned Insert_Entry (ins_name, bkp)
	char *ins_name;
	BankPtr bkp;
{
   BankEntry entry;
   int diff;
   unsigned new_ref;
	int n;

   if( bkp->head.nrDefined == bkp->head.nrEntry)
		Expand_File (EXPAND_NR, bkp);
   n = bkp->head.nrDefined - 1;

   /* Get reference number from next available record. */
   Get_Nth_Entry (n+1, &entry, bkp);
   new_ref = entry.nrReference;

   /* Starting at the bottom of the list, read in an entry.  If the new
	  entry does not go here, write the entry back in the next space. */
   do {
	  if (n != -1) {
		 Get_Nth_Entry (n, &entry, bkp);
		 diff = strcmpi (ins_name, entry.TimbreName);
	  }
	  else {
		 Seek_Nth_Entry (0, bkp);
		 diff = 1;
	  }

	  if (diff > 0) {
		 memset( entry.TimbreName, 0, 9);
		 strcpy (entry.TimbreName, ins_name);
		 entry.nrReference = new_ref;
		 entry.usedFlag = 1;
	  }

	  n--;
	  write ( bkp->bank_id, (char *) &entry, sizeof (BankEntry));
   } while (diff <= 0);

   return (new_ref);
}
#endif


#ifdef BANK_WRITE_ACCESS
/*
	Remove the entry 'ins_name'.  Shift the following entries up in order
	to preserve their order.
*/
static int Delete_Entry (ins_name, bkp)
	char * ins_name;
	BankPtr bkp;
	{
	int nbr, i, j;
	BankEntry entry, okEntry;

 	nbr = bkp->head.nrDefined;
	for( i = 0; i < nbr; i++) {
		Get_Nth_Entry( i, &entry, bkp);
		if( 0 == strcmpi( entry.TimbreName, ins_name))
			break;
		}
	if( i < nbr) {
		for( j = i +1; j < nbr; j++) {
			Get_Nth_Entry( j, &okEntry, bkp);
			Seek_Nth_Entry( j -1, bkp);
			write( bkp->bank_id, &okEntry, sizeof( BankEntry));
			}
		Seek_Nth_Entry( nbr -1, bkp);
		entry.usedFlag = 0;
		entry.TimbreName[ 0] = '-';
		write( bkp->bank_id, &entry, sizeof( BankEntry));

		bkp->head.nrDefined--;
		Header_Update( bkp);

		return 1;
		}
	return 0;
	}
#endif
