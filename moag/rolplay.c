/* #define REGISTERED 1 /**/
/* #define heapprint 1 */
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
   File: RolPlay.c
   January, 1989, Dale Glowinski

   This file and RolRead.c make up a module which can be used to read and
   play a .ROL file.  The new instrument bank file (.BNK) is used.
   RolPlay.c contains mainly routines which interface with the card.  The
   file RolFile.c contains routines which read the file and manage the
   music file events.

   The routine main() (below) is an example of how to play a .ROL file.  In
   your own application, you must do the following:
	  1. Call SetUp_Melo (rol_file_name, bank_file_name)
	  2. Call Send_Melo() until the file has all been sent
	  3. Call Play_Status() until the song is finished
	  4. Call UnInit() to clean up
   In Step #2, you are waiting for space to be freed up in the sound driver
   buffer.  While waiting, you may go off and do something else.  How much
   else you can do in this time interval depends on the size of the sound
   driver and the speed of your machine.
   In Step #3, the sound driver has received all of the .ROL file and you
   are waiting for it to finish playing.  You may go off and do something
   for an extended period of time.  You cannot quit at this point because
   the buffers containing the instrument parameters must remain valid until
   the end of the song.

   In RolRead.c, the file input routines read the .ROL file and stores it
   in a manner which facilitates accessing the data in the order in which
   the events occur.  How much data is read at one time (and consequently
   how much space it will take) can be set by adjusting the #define's in
   stddef.h.  In the same file are routines to take the data from the
   input file buffers and feed it to the card output routines in RolPlay.c.

   The file InsBank.c contains routines to deal with the instrument bank.
   It is used to read an instrument from a bank file, but the file can be
   configured to write to the bank as well.

   Csound.asm contains two assembler routines: SoundCall and
   GetSoundDrvVersion.  SoundCall sends event data (note on, volume change,
   etc.) to the sound driver.  See snddef.h for the list of sound driver
   function numbers.  GetSoundDrvVersion determines if the sound driver is
   installed and returns its version number if so.

   To make the stand-alone executable version of this file, the following
   files must be linked together: rolplay.obj, rolread.obj, insbank.obj and
   csound.obj.  To use as a module in another program, remove the define of
   STAND_ALONE and rolplay.c will compile without the main() routine.
*/


#include  "cflags.h"
#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>
#include <alloc.h>
#include  "insbank.h"

#ifndef TURBOC
  #include <malloc.h>
#endif

/* This causes a local routine to be used to convert from floating point
   values to integers.  It is only of use for the range of values that are
   found in a .ROL file.  It is by no means a complete floating point routine
   library.  However, 15K of space is saved by not loading the compiler's
   floating point library. */
#define  NO_FLOAT_LIBRARY

#ifdef NO_FLOAT_LIBRARY
#define  float  UBYTE4
#endif

#include  "snddef.h"
#include  "sndtype.h"

#define MID_C               60

#define ErrNoSDSpace        -1
#define ErrFileNotFound     -2
#define ErrReadFile         -3
#define BankFileNotFound    -6
#define NoErr               0

#define  NOT_ALL_SENT       1
#define  ALL_SENT           0

#define  STILL_PLAYING             1
#define  DONE_PLAYING              0

#define  MELODIC_MODE       notPercusMode
#define  PERCUS_MODE        !notPercusMode

/* This causes main() to be included.  If you are going to use this module
   as part of another program, don't define this. */
/* #define STAND_ALONE */

//#define  My_Alloc(x)   malloc(x)
#define  My_Free(x)    free(x)


typedef struct Ins_List {
		struct Ins_List *next;
		char   instr_name [10];
		Timbre timbre;
} Instr_List;

static int         tickPerBeat;
static char        notPercusMode;
static int         basicTempo;
static int         active = 0;
static Rol_File    rol;
static int         current_voice;
static Instr_List  *instr_list;

BankRec            bnk_rec;
BankPtr            bnk_ptr = &bnk_rec;

	int ramalloced;
#ifdef heappring
void printheap() {
struct heapinfo hi;
	hi.ptr=NULL;
	printf("\n Printing contents of heap..............");
	while(heapwalk(&hi) == _HEAPOK)
		printf("%7u    %s\n",hi.size,hi.in_use ? "used" : "free");
	return;
	}
#endif

int startsong(songname)
   char    *songname;
{
   extern int GetSoundDrvVersion (void);
   int result, different;

   result = GetSoundDrvVersion();
   if (!result) {
	  printf ("Sound driver not installed, aborting application");
	  return(-1);
   }
//	  printf ("Using sound driver version %x.%x\n", result >> 8, result & 0xff);

   /* NOTE: User must specify .ROL extension in file name */
#ifdef REGISTERED
		 result = SetUp_Melo  (songname, "bnk835.bnk");
#else
		 result = SetUp_Melo  (songname, "ccmusic.bnk");
#endif
		 if (result != NoErr) return(-2);
	return(0);
}

int servicesong()
{
   int result;
			result = Send_Melo ();
			if(result == NOT_ALL_SENT)
				return(1);
			else
			   if(Play_Status() == STILL_PLAYING)
                                return(1);
			   else
				return(0);
}

void endsong()
{
int	i;
		i=SoundCall(5,0);
		UnInit ();
}

/*-------------------------------------------------------------------------*/
#ifdef STAND_ALONE
main (argc, argv)
   int     argc;
   char    *argv[];
{
   extern int GetSoundDrvVersion (void);
   int result, different;
   char fname [80];

   result = GetSoundDrvVersion();
   if (!result) {
	  printf ("Sound driver not installed, aborting application");
	  return;
   }
   else if (argc < 2)
	  printf ("Using sound driver version %x.%x\n", result >> 8, result & 0xff);

   /* NOTE: User must specify .ROL extension in file name */
		 result = SetUp_Melo  (argv [1], "standard.bnk");

		 if (result == NoErr) {

			/* Read the .ROL file and send it to the sound driver. */
			do result = Send_Melo ();
			   while (!kbhit() && result == NOT_ALL_SENT);

			/* Wait for song to finish (allocated space must remain valid) */
			do result = Play_Status();
			   while ((result == STILL_PLAYING) && !kbhit());
			if(kbhit()) getch();
		 }
		 else printf ("Error = %d\n", result);
		 UnInit ();
   }
   else {                    /* prompt user for file name */
	  printf ("\nFile name: ");
	  scanf ("%s", fname);

	  if (fname) {
		 result = SetUp_Melo  (fname, "standard.bnk");

		 if (result == NoErr) {

			/* Read the .ROL file and send it to the sound driver. */
			do result = Send_Melo ();
			   while (result == NOT_ALL_SENT);

			/* Wait for song to finish (allocated space must remain valid) */
			printf("Waiting for end\n");
			do result = Play_Status();
			   while (!kbhit() && result == STILL_PLAYING);
			if (kbhit()) getch ();
		 }
		 else printf ("Error = %d\n", result);
		 UnInit ();
	  }
   }
}
#endif

/*-------------------------------------------------------------------------*/
Timbre * Load_Timbre (instName)
	char *instName;
{
	int different, n;
	Instr_List *instr, *last = NULL;
#ifndef DYNAMIC
	int ins_count = 0;
	if (ins_count >= MAX_INSTR) return (NULL);
#endif

	/* check if the timbre is already in memory, return address if so */
	instr = instr_list;
	while (instr) {
	   different = strcmp (instr->instr_name, instName);
	   if (!different) return (&instr->timbre);
	   last = instr;
	   instr = instr->next;
	}

	instr = (Instr_List *) My_Alloc (sizeof(Instr_List));
	if (!instr) return (NULL);

	/* Read instrument from bank file */
	n = Read_Timbre_def (instName, &instr->timbre, bnk_ptr);
	if (n != BANK_OK) return (NULL);

	if (last) last->next = instr;
	   else instr_list = instr;
	instr->next = NULL;
	strcpy (instr->instr_name, instName);

#ifndef DYNAMIC
	ins_count++;
#endif
	return (&instr->timbre);
}


/*-------------------------------------------------------------------------*/
Turn_On_Driver ()
{
	Set_State_Drvr (1);
	active = 1;
}

Turn_Off_Driver ()
{
	Set_State_Drvr (0);
	active = 0;
}


/*-------------------------------------------------------------------------
Open an instrument bank file using the passed name. */

SetUp_Bank (bankName, bank_ptr)
   char *bankName;
   BankPtr bank_ptr;
{
   char bname [128];
   int n;

   strcpy (bname, bankName);
   if (!(strchr (bname, '.'))) strcat (bname, ".bnk");

   n = Open_Bank (bname, 0, bank_ptr);
   return (n);
}


/*-------------------------------------------------------------------------
Initialize the card and global variables. */

Init_Card ()
{
	Flush_Queues();
	Init_Snd_Drvr();
	Waveforms_On (1);
	Transpose_ToDrvr(0);
	Mode_ToDrvr (1);
}

Init_Vars ()
{
	instr_list = NULL;
	current_voice = -1;
	active = 0;
}


/*-------------------------------------------------------------------------
Free space allocated for instruments and close bank file. */

UnInit ()
{
	extern void freeem();
   Instr_List *instr = instr_list, *save;
#ifdef heapprint
   printheap();
getch();
#endif
	/* we can let freem free the memory so this won't have to? */
   while (instr) {
	  save = instr->next;
//	  My_Free (instr);
	  instr = save;
   }
   Close_Bank (bnk_ptr);
   close (rol.ID);
   freeem();
#ifdef heapprint
  printheap();
getch();
#endif
   /* Init_Card (); */
}


/*-------------------------------------------------------------------------
Convert a float to an integer.  The integer result is 128 times larger
than its real value.  This is to prevent numbers < 0 from being truncated
to zero.  */

#ifdef NO_FLOAT_LIBRARY

long Float_To_Int (f)
  UBYTE4 f;
{
   int exp, n;
   long bidon, mantissa;

   memcpy (&bidon, &f, sizeof(float));

   exp = (bidon >> 23) - 127;
   mantissa = (bidon & 0x7fffff) | 0x800000;
   n = (int) (mantissa >> (16 - exp));    /* mult of 128 */

   return (n);
}

#endif

/*-------------------------------------------------------------------------
Get everything ready to play a .ROL file. */

int SetUp_Melo (fileName, bankName)
	char * fileName, *bankName;
{
	extern void nullem();
	int i;

	Init_Card ();
	Init_Vars ();

	nullem();

	i = SetUp_Bank (bankName, bnk_ptr);
	if (i < 0) return (BankFileNotFound);

	strcpy (rol.f_name, fileName);
	rol.ID = open (rol.f_name, O_RDONLY | O_BINARY);
	if (rol.ID < 0) {
	   Close_Bank (bnk_ptr);
	   return (ErrFileNotFound);
	}

	/* Read file.  Puts header in rol structure. */
	if (!Load_File (&rol)) return (ErrReadFile);

	/* Set global variables using info from file header */
	tickPerBeat = rol.rol_header.ticks_in_beat;
	notPercusMode = rol.rol_header.music_mode;
	basicTempo = (int)(Float_To_Int ((UBYTE4) rol.rol_header.basic_tempo) >> 7);

	Ticks_ToDrvr (tickPerBeat);
	Start_Time_ToDrvr (0, tickPerBeat);
	Tempo_ToDrvr ((int) basicTempo, 0, tickPerBeat);

	i = (MELODIC_MODE) ? 0:1;
	Mode_ToDrvr (i);

	return (NoErr);
}


/*-------------------------------------------------------------------------
Send a pitch variation event to the card. */

int Send_Pitch (voice, pEvent)
	int  voice;
	PitchEvent  *pEvent;
{
	int int_pitch;

#ifdef NO_FLOAT_LIBRARY
   /* Rescale 0 to 2 into -100 to 100. */
   int_pitch = Float_To_Int ((UBYTE4) pEvent->pitch);
   int_pitch *= 100;
   int_pitch >>= 7;
   int_pitch -= 100;
#else
   /* Rescale 0 to 2 into -100 to 100. */
   int_pitch = (pEvent->pitch * 100) - 100;
#endif

   if (!Pitch_ToDrvr (0, int_pitch, 100, pEvent->time, tickPerBeat))
	  return ( ErrNoSDSpace);
   else
	  return (NoErr);
}


/*-------------------------------------------------------------------------
Send a change of volume event to the card. */

int Send_Volume (voice, vEvent)
   int voice;
   VolumeEvent *vEvent;
{
   int int_vol;

#ifdef NO_FLOAT_LIBRARY
   /* Rescale 0 to 1 into 0 to 100. */
   int_vol = Float_To_Int ((UBYTE4) vEvent->volume);

   if (!Volume_ToDrvr (int_vol, 128, vEvent->time, tickPerBeat))
	  return (ErrNoSDSpace);
   else
	  return (NoErr);
#else
   /* Rescale 0 to 1 into 0 to 100. */
   int_vol = (int) (vEvent->volume * (float) 100);

   if (!Volume_ToDrvr (int_vol, 100, vEvent->time, tickPerBeat))
	  return (ErrNoSDSpace);
   else
	 return (NoErr);
#endif
}
	

/*-------------------------------------------------------------------------
Get an instrument and send the parameters to the card. */

int Send_Instrument (voice, iEvent)
	int voice;
	InstrumEvent *iEvent;
{
	Timbre          *timbre;
					  	
	timbre = Load_Timbre (iEvent->instrumName);
	if (!Instr_ToDrvr (&timbre->op0, iEvent->time, tickPerBeat))
	   return (ErrNoSDSpace);
	return (NoErr);
}

				
/*-------------------------------------------------------------------------
Send a note to the card. */

int Send_Note (voice, nEvent)
	int voice;
	NoteEvent *nEvent;
{
	if (nEvent->note == 0) {
	   /* this note is a rest */
	   if (!Note_ToDrvr (2, 0, tickPerBeat, nEvent->duree, tickPerBeat))
				return( ErrNoSDSpace);
	}
	else {
	   if (!Note_ToDrvr (nEvent->note - MID_C, nEvent->duree, tickPerBeat,
					  nEvent->duree, tickPerBeat))
				return (ErrNoSDSpace);
	}
	return( NoErr);
}


/*-------------------------------------------------------------------------
Send a change of tempo event to the card. */

int Send_Tempo (tEvent)
	TempoEvent  *tEvent;
{
   int   theTempo;

#ifdef NO_FLOAT_LIBRARY
   long  n;
   n = Float_To_Int ((UBYTE4) tEvent->tempo);
   n *= basicTempo;
   theTempo = (n >> 7);
#else
   theTempo = basicTempo * tEvent->tempo;
#endif

	 if (!Tempo_ToDrvr (theTempo, tEvent->time, tickPerBeat))
		return (ErrNoSDSpace);
	 if (!tEvent->time && !active) {
		/* Before playing any notes, make the sound driver process any
		   data it may have received.  If this is not done, then the sound
		   driver may have so much data to process at one time that when it
		   does start, the first several notes may be played too quickly.
		   The tempo event must be the next-lowest priority before note
		   events (see snddef.h).  Otherwise, this sequence will not
		   execute at the appropriate moment.
		*/
		Turn_On_Driver();
		while (Play_Status());
		Turn_Off_Driver();
		Start_Time_ToDrvr (0, tickPerBeat);
	 }
	return( NoErr);
}


/*-------------------------------------------------------------------------
Gets events and sends them to the card until the sound driver buffer is full
or the entire .ROL file has been sent.  This routine should be called until
it returns ALL_SENT. */

int Send_Melo ()
{
   void *buffer;
   int event, voice, result;

   DO
	  event = Find_Next_Event (&voice, &buffer);
	  result = NoErr;

	  /* If in melodic mode, don't send info for voices 10 and 11 */
	  if (PERCUS_MODE || voice <= 8) {

		 /* Set active voice if necessary. */
		 if (voice != current_voice) {
			current_voice = voice;
			Voice_ToDrvr (current_voice);
		 }

		 if (buffer) {
			switch (event) {
			   case NOTE_EVENT:
				  result = Send_Note (voice, (NoteEvent *) buffer);
				  break;
			   case INSTR_EVENT:
				  result = Send_Instrument (voice, (InstrumEvent *) buffer);
				  break;
			   case TEMPO_EVENT:
				  result = Send_Tempo ((TempoEvent *) buffer);
				  break;
			   case PITCH_EVENT:
				  result = Send_Pitch (voice, (PitchEvent *) buffer);
				  break;
			   case VOL_EVENT:
				  result = Send_Volume (voice, (VolumeEvent *) buffer);
				  break;
			   case EOF_EVENT:
				  result = NoErr;
				  break;
			}
		 }
	  }
   WHILE (result == NoErr && event != EOF_EVENT);

   if (!active) Turn_On_Driver ();

   if (result != NoErr) {
	  /* If here, then the sound driver buffer was full.  This also means
		 that the last event was not sent to the card -> put it back in the
		 event queue.  Return a flag indicating that there is more data to
		 be sent.  */
	  Undo_Event (event, voice);
	  return (NOT_ALL_SENT);
   }

   return (ALL_SENT);
}


