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

#include  "cflags.h"
#include  "insbank.h"
#include  "snddef.h"
#include  "sndtype.h"

#include  <fcntl.h>
#include  <sys\types.h>
#include  <sys\stat.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#ifdef TURBOC
   #include <mem.h>
#else
   #include <malloc.h>
   #include <memory.h>
#endif

#define  My_Free(x)    free(x)

static Rol_File  glb_rol;
static Event     evnt_recs [NR_VOICES];       /* input data structure */
static void *allocptrs[NR_VOICES*5];
static numallocs;

void My_Free1(void *ptr)
{
//	printf("\nFreeing ptr %p",ptr);
	free(ptr);
}

void * My_Alloc(int x) {
	extern int ramalloced;
	ramalloced+=x;
	allocptrs[numallocs]=malloc(x);
	if(allocptrs[numallocs]==NULL) {
		printf("\n\n\n Out of RAM Panic!!!! \n\n\n");
		exit(137);
	}
	numallocs++;
	return(allocptrs[numallocs-1]);
}

void nullem()
{
	numallocs=0;
}

void freeem()
{
	int i;
	for(i=0;i<numallocs;i++)
	  My_Free(allocptrs[i]);
}

/********************************* FILE IO *********************************/

/*-------------------------------------------------------------------------
Read enough volume changes to fill a buffer. */

Fill_Volume_Buffer (evnt, fileID)
   Event *evnt;
   int fileID;
{
   int read_size, i;
   lseek (fileID, evnt->vol_seek, SEEK_SET);
   read_size = (evnt->vol_count <= MAX_STOCK) ? evnt->vol_count : MAX_STOCK;
   read_size *= sizeof (VolumeEvent);

   i = read (fileID, (char *) evnt->vol_data, read_size);
   if (i != read_size) {
	  return 0;
   }

   evnt->vol_ptr = 0;
   evnt->vol_seek += (long) read_size;
   return 1;
}


/*-------------------------------------------------------------------------
Read enough instrument changes to fill a buffer. */

Fill_Instr_Buffer (evnt, fileID)
   Event *evnt;
   int fileID;
{
   int read_size, i;
   lseek (fileID, evnt->instr_seek, SEEK_SET);
   read_size = (evnt->instr_count <= MAX_STOCK) ? evnt->instr_count : MAX_STOCK;
   read_size *= sizeof (InstrumEvent);

   i = read (fileID, (char *) evnt->instr_data, read_size);
   if (i != read_size) {
	  return 0;
   }

   evnt->instr_ptr = 0;
   evnt->instr_seek += (long) read_size;
   return 1;
}


/*-------------------------------------------------------------------------
Read enough pitch changes to fill a buffer. */

Fill_Pitch_Buffer (evnt, fileID)
   Event *evnt;
   int fileID;
{
   int read_size, i;
   lseek (fileID, evnt->pitch_seek, SEEK_SET);
   read_size = (evnt->pitch_count <= MAX_STOCK) ? evnt->pitch_count : MAX_STOCK;
   read_size *= sizeof (PitchEvent);

   i = read (fileID, (char *) evnt->pitch_data, read_size);
   if (i != read_size) {
	  return 0;
   }

   evnt->pitch_ptr = 0;
   evnt->pitch_seek += (long) read_size;
   return 1;
}


/*-------------------------------------------------------------------------
Read enough tempo changes to fill one buffer. */

Fill_Tempo_Buffer (evnt, fileID)
   Event *evnt;
   int fileID;
{
   int read_size, i;
   lseek (fileID, evnt->tempo_seek, SEEK_SET);
   read_size = (evnt->tempo_count <= MAX_STOCK) ? evnt->tempo_count : MAX_STOCK;
   read_size *= sizeof (TempoEvent);

   i = read (fileID, (char *) evnt->tempo_data, read_size);
   if (i != read_size) {
	  return 0;
   }

   evnt->tempo_ptr = 0;
   evnt->tempo_seek += (long) read_size;
   return 1;
}


/*-------------------------------------------------------------------------
Read enough notes to fill one buffer. */

Fill_Note_Buffer (evnt, fileID, voice)
   Event *evnt;
   int fileID;
   int voice;
{
   NoteEvent *ndata = evnt->note_data;
   int read_size, i;

   lseek (fileID, evnt->note_seek, SEEK_SET);

   read_size = NOTE_STOCK * sizeof (NoteEvent);
   i = read (fileID, (char *) ndata, read_size);
   /* Cannot compare i to read_size as number of notes is not known */
   if (i < 0) {
	  return 0;
   }

   evnt->note_seek += (long) read_size;
   evnt->note_ptr = 0;
   return 1;
}




/*-------------------------------------------------------------------------
Read the header section of the .ROL file. */

Read_Rol_Header (rol)
   Rol_File *rol;
{
   int result;
   result = read (rol->ID, (char *) &rol->rol_header, sizeof (Rol_Header));
   if (result != sizeof (Rol_Header)) return 0;
	  else return 1;
}


/*-------------------------------------------------------------------------
Load tempo data into the first voice and then copy it to the other voices.
Resets the file pointer to the start of the next data section. */

Load_Tempo (events, fileID, count)
   Event *events;
   int fileID;
   long *count;
{
   int i;
   Event *evnt = &events [0];

   lseek (fileID, *count, SEEK_SET);
   i = read (fileID, (char *) &evnt->tempo_count, sizeof(int));
   *count += (long) sizeof(int);
   if (i != sizeof(int)) return 0;
   evnt->tempo_seek = *count;

   if (evnt->tempo_count > 0) {
	  i = (evnt->tempo_count <= MAX_STOCK) ? evnt->tempo_count : MAX_STOCK;
	  evnt->tempo_data = (TempoEvent *) My_Alloc (i * sizeof (TempoEvent));

	  if (!Fill_Tempo_Buffer (evnt, fileID)) return 0;

	  for (i=1; i < NR_VOICES; i++) {
		 events [i].tempo_data = evnt->tempo_data;
		 /* 18-jan-88, only allow voice 0 access to tempo data */
		 events [i].tempo_count = 0 /* evnt->tempo_count */;
		 events [i].tempo_seek = evnt->tempo_seek;
		 events [i].tempo_ptr = 0;
	  }
	  *count += (long) (evnt->tempo_count * sizeof (TempoEvent));
   }
   else *count += 6;
   lseek (fileID, *count, SEEK_SET);

   return 1;
}


/*-------------------------------------------------------------------------
Load note data for the current voice set file pointer to start of next data
section. */

Load_Notes (fileID, evnt, count, voice)
	int fileID;
	Event *evnt;
	long *count;
	int voice;
{
   int i;
   Event dummy;
   Time time;
   unsigned char buffer [NOTE_STOCK * sizeof (NoteEvent)];

   i = read (fileID, (char *) &evnt->end_time, sizeof(int));
   if (i < 0) return 0;
   *count += (long) sizeof(int);

   evnt->now_time = 0;
   evnt->note_seek = *count;
   evnt->note_data = (NoteEvent *) My_Alloc (NOTE_STOCK * sizeof (NoteEvent));

   if (!Fill_Note_Buffer (evnt, fileID, voice)) return 0;

   /* Read any remaining notes to find end of section */
   memcpy (&dummy, evnt, sizeof (Event));
   dummy.note_data = (NoteEvent *) buffer;
   memcpy (dummy.note_data, evnt->note_data, NOTE_STOCK * sizeof (NoteEvent));
   time = 0;

   DO
	  /* calculate total time for this buffer */
	  for (i=0; time < evnt->end_time && i < NOTE_STOCK; i++)
		 time += dummy.note_data [i].duree;
	  *count += (long) (i * sizeof (NoteEvent));

	  /* read another buffer if necessary */
	  if (time < dummy.end_time) {
		 i = Fill_Note_Buffer (&dummy, fileID, voice);
		 if (i < 0) return 0;
	  }
   WHILE (time < dummy.end_time);

   *count += (long) FILLER_SIZE;
   lseek (fileID, *count, SEEK_SET);

   return 1;
}

/*-------------------------------------------------------------------------
Load the volume data for the current voice and set the file pointer to the
beginning of the next data section. */

Load_Volume (fileID, evnt, count)
	int fileID;
	Event *evnt;
	long *count;
{
   int i;

   i = read (fileID, (char *) &evnt->vol_count, sizeof(int));
   *count += (long) sizeof(int);
   if (i < 0) return 0;

   evnt->vol_seek = *count;
   i = (evnt->vol_count <= MAX_STOCK) ? evnt->vol_count : MAX_STOCK;
   evnt->vol_data = (VolumeEvent *) My_Alloc (i * sizeof (VolumeEvent));

   if (!Fill_Volume_Buffer (evnt, fileID)) return;

   *count += (long) (FILLER_SIZE + evnt->vol_count * sizeof (VolumeEvent));
   lseek (fileID, *count, SEEK_SET);

   return 1;
}


/*-------------------------------------------------------------------------
Read instrument change data for the current voice. */

Load_Instr (fileID, evnt, count)
	int fileID;
	Event *evnt;
	long *count;
{
   int i;

   i = read (fileID, (char *) &evnt->instr_count, sizeof(int));
   *count += (long) sizeof(int);
   if (i < 0) return 0;

   evnt->instr_seek = *count;
   i = (evnt->instr_count <= MAX_STOCK) ? evnt->instr_count : MAX_STOCK;
   evnt->instr_data = (InstrumEvent *) My_Alloc (i*sizeof (InstrumEvent));

   if (!Fill_Instr_Buffer (evnt, fileID)) return;

   *count += (long) (FILLER_SIZE + evnt->instr_count * sizeof (InstrumEvent));
   lseek (fileID, *count, SEEK_SET);

   return 1;
}


/*-------------------------------------------------------------------------
Read pitch bend info for the current voice. */

Load_Pitch (fileID, evnt, count)
	int fileID;
	Event *evnt;
	long *count;
{
   int i;

   i = read (fileID, (char *) &evnt->pitch_count, sizeof(int));
   *count += (long) sizeof(int);
   if (i < 0) return 0;

   evnt->pitch_seek = *count;
   i = (evnt->pitch_count <= MAX_STOCK) ? evnt->pitch_count : MAX_STOCK;
   evnt->pitch_data = (PitchEvent *) My_Alloc (i*sizeof (PitchEvent));

   if (!Fill_Pitch_Buffer (evnt, fileID)) return;

   *count += (long) (FILLER_SIZE + evnt->pitch_count * sizeof (PitchEvent));
   lseek (fileID, *count, SEEK_SET);

   return 1;
}


/*-------------------------------------------------------------------------
Initial routine to read in the file.  */

Load_File (rol)
   Rol_File *rol;
{
   int i, ok;
   long count, n;

   memcpy (&glb_rol, rol, sizeof(Rol_File));

   ok = Read_Rol_Header (rol);
   if (!ok) return 0;
   count = (long) sizeof (Rol_Header);

   if (!Load_Tempo (evnt_recs, rol->ID, &count)) return 0;

   count += (long) FILLER_SIZE;
   n = lseek (rol->ID, count, SEEK_SET);
   if (n < 0) return 0;

   for (i=0; i < NR_VOICES; i++) {
	  if (!Load_Notes (rol->ID, &evnt_recs [i], &count, i))
		 return 0;
	  if (!Load_Instr (rol->ID, &evnt_recs [i], &count))
		 return 0;
	  if (!Load_Volume (rol->ID, &evnt_recs [i], &count))
		 return 0;
	  if (!Load_Pitch (rol->ID, &evnt_recs [i], &count))
		 return 0;
   }
   return 1;
}



/**************************** EVENT MANAGEMENT *****************************

The routines Get_X_Event, where X is a data type used in the .ROL file, get
the next piece of data for type X.  A limited amount of data is stored in a
buffer, and when all data has been read from the buffer, the buffer is
refreshed.  The amount of data stored in a buffer is determined by MAX_STOCK
and NOTE_STOCK, which are defined in "snddef.h".

The routines Undo_X_Event replace the data (or rather, it changes pointers to
point to the previous piece of data) that was removed by the corresponding
Get_X_Event routine.  The undo routines can be safely called only once for
every call to the corresponding Get_X_Event.  The Undo routines are used to
replace an event when the sound driver signals that its buffer is full.
*/


/*-------------------------------------------------------------------------*/
void * Get_Instr_Event (evnt, voice)
   Event *evnt;
   int voice;
{
   int n;
   if (evnt->instr_ptr >= MAX_STOCK)
	  if (!Fill_Instr_Buffer (evnt, glb_rol.ID, voice)) return (NULL);
   n = evnt->instr_ptr;
   evnt->instr_ptr++;
   evnt->instr_count--;
   return ((UBYTE1 *) &evnt->instr_data [n]);
}

Undo_Instr_Event (evnt)
   Event *evnt;
{
   evnt->instr_ptr--;
   evnt->instr_count++;
}

/*-------------------------------------------------------------------------*/
void * Get_Pitch_Event (evnt, voice)
   Event *evnt;
   int voice;
{
   int n;
   if (evnt->pitch_ptr >= MAX_STOCK)
	  if (!Fill_Pitch_Buffer (evnt, glb_rol.ID, voice)) return (NULL);
   n = evnt->pitch_ptr;
   evnt->pitch_ptr++;
   evnt->pitch_count--;
   return ((UBYTE1 *) &evnt->pitch_data [n]);
}

Undo_Pitch_Event (evnt)
   Event *evnt;
{
   evnt->pitch_ptr--;
   evnt->pitch_count++;
}

/*-------------------------------------------------------------------------*/
void * Get_Tempo_Event (evnt, voice)
   Event *evnt;
   int voice;
{
   int n;
   if (evnt->tempo_ptr >= MAX_STOCK)
	  if (!Fill_Tempo_Buffer (evnt, glb_rol.ID, voice)) return (NULL);
   n = evnt->tempo_ptr;
   evnt->tempo_ptr++;
   evnt->tempo_count--;
   return ((UBYTE1 *) &evnt->tempo_data [n]);
}

Undo_Tempo_Event (evnt)
   Event *evnt;
{
   evnt->tempo_ptr--;
   evnt->tempo_count++;
}

/*-------------------------------------------------------------------------*/
void * Get_Vol_Event (evnt, voice)
   Event *evnt;
   int voice;
{
   int n;
   if (evnt->vol_ptr >= MAX_STOCK)
	  if (!Fill_Volume_Buffer (evnt, glb_rol.ID, voice)) return (NULL);
   n = evnt->vol_ptr;
   evnt->vol_ptr++;
   evnt->vol_count--;
   return ((UBYTE1 *) &evnt->vol_data [n]);
}

Undo_Vol_Event (evnt)
   Event *evnt;
{
   evnt->vol_ptr--;
   evnt->vol_count++;
}


/*-------------------------------------------------------------------------*/
void * Get_Note_Event (evnt, voice)
   Event *evnt;
   int voice;
{
   int n;
   if (evnt->note_ptr >= NOTE_STOCK)
	  if (!Fill_Note_Buffer (evnt, glb_rol.ID, voice)) return (NULL);
   n = evnt->note_ptr;
   evnt->now_time += evnt->note_data [n].duree;
   evnt->note_ptr++;
   return ((UBYTE1 *) &evnt->note_data [n]);
}

Undo_Note_Event (evnt)
   Event *evnt;
{
   evnt->note_ptr--;
   evnt->now_time -= evnt->note_data [evnt->note_ptr].duree;
}


/*-------------------------------------------------------------------------
For the given voice, find which type of event should be the next to be
processed.  Places the time of the next event in *soonest and returns
the event type code. */

int Voice_Event_Search (voice, soonest)
   int voice, *soonest;
{
   Event *evnt = &evnt_recs [voice];
   int n, event;
   int times [NR_EVENTS];

   /* Set up "times", an array containing the times of the upcoming events.
	  -1 indicates no (more) events. */
   for (n=0; n < NR_EVENTS; n++) times [n] = -1;
   times [EOF_EVENT] = -1;
   if (evnt->now_time < evnt->end_time)
	  times [NOTE_EVENT] = evnt->now_time;
   if (evnt->tempo_count)
	  times [TEMPO_EVENT]  = evnt->tempo_data [evnt->tempo_ptr].time;
   if (evnt->instr_count)
	  times [INSTR_EVENT] = evnt->instr_data [evnt->instr_ptr].time;
   if (evnt->pitch_count)
	  times [PITCH_EVENT] = evnt->pitch_data [evnt->pitch_ptr].time;
   if (evnt->vol_count)
	  times [VOL_EVENT] = evnt->vol_data [evnt->vol_ptr].time;

   /* find the smallest value > 0 in the array */
   for (event=0, n=1; n < NR_EVENTS; n++)
	  if (times [n] >= 0)
		 if (times [event] < 0 || times [n] < times [event]) event = n;

   *soonest = times [event];

   return (event);
}


/*-------------------------------------------------------------------------
Find the next event. Places the voice number of the next event in *voice
and returns the event type code. */

int Next_Event_Search (voice)
   int *voice;
{
   int n, i;
   int times [NR_VOICES], events [NR_VOICES];
   long ptimes [NR_VOICES];

   for (n=0; n < NR_VOICES; n++)
	  events [n] = Voice_Event_Search (n, &times [n]);

   /*** find the smallest value(s) > 0 in the array */
   for (i=0, n=1; n < NR_VOICES; n++)
	  if (times [n] >= 0)
		 if (times [i] < 0 || times [n] < times [i]) i = n;

   /** remove events so only the earliest event(s) remain */
   for (n=0; n < NR_VOICES; n++)
	  if (times [n] > times [i]) times [n] = -1;

   /* Make an event table which takes priorities into account.
	  Lower event numbers have higher priority. */
   for (n=0; n < NR_VOICES; n++) {
	  if (times [n] < 0)
		 ptimes [n] = -1L;
	  else
		 ptimes [n] = (times [n] + 1) * events [n];
   }

   /* find the smallest value >= 0 in the time array */
   for (*voice=0, n=1; n < NR_VOICES; n++)
	  if (ptimes [n] >= 0)
		 if (ptimes [*voice] < 0 || ptimes [n] < ptimes [*voice]) *voice = n;

   return (events [*voice]);
}

/*-------------------------------------------------------------------------
For the passed voice, find the next type of data to be sent to the sound
driver.  Places a pointer to the data area in **buffer and returns the
type of event. */

int Find_Next_Event (voice, buffer)
   int *voice;
   void **buffer;
{
   Event *evnt;
   int next_event;

   next_event = Next_Event_Search (voice);      /* this sets *voice */
   evnt = &evnt_recs [*voice];

   switch (next_event) {
	  case NOTE_EVENT:
		 *buffer = Get_Note_Event (evnt, *voice);
		 break;
	  case INSTR_EVENT:
		 *buffer = Get_Instr_Event (evnt, *voice);
		 break;
	  case TEMPO_EVENT:
		 *buffer = Get_Tempo_Event (evnt, *voice);
		 break;
	  case PITCH_EVENT:
		 *buffer = Get_Pitch_Event (evnt, *voice);
		 break;
	  case VOL_EVENT:
		 *buffer = Get_Vol_Event (evnt, *voice);
		 break;
	  case EOF_EVENT:
		 break;
   }
   return (next_event);
}


/*-------------------------------------------------------------------------
Replace the passed event back into the event "queue".  This will be called
when the sound driver buffer is full and the most recent event cannot be
sent. */

Undo_Event (event, voice)
   int event;
{
   Event *evnt = &evnt_recs [voice];

   switch (event) {
	  case NOTE_EVENT:
		 Undo_Note_Event (evnt);
		 break;
	  case INSTR_EVENT:
		 Undo_Instr_Event (evnt);
		 break;
	  case TEMPO_EVENT:
		 Undo_Tempo_Event (evnt);
		 break;
	  case PITCH_EVENT:
		 Undo_Pitch_Event (evnt);
		 break;
	  case VOL_EVENT:
		 Undo_Vol_Event (evnt);
		 break;
	  case EOF_EVENT:
		 break;
   }
}
