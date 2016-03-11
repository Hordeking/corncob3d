/*-------------------------------------------------------------------------
The following group of typedefs taken from Marc Savary's routines */

#pragma pack(1)

typedef int Time;

typedef
	struct {
		Time    time;
		float   tempo;
	} TempoEvent;

typedef
	struct {
		int     note;
		int     duree;
	} NoteEvent;


typedef
	struct {
		Time time;
		char instrumName[ 9];
		char filler;
		int timbreIndex;
	} InstrumEvent;

typedef
	struct {
		Time time;
		float volume;
	} VolumeEvent;


typedef
	struct {
		Time time;
		float pitch;
	} PitchEvent;

typedef
	struct {
		char instrumName[ 9];
		Timbre trec;            /* 19-Jan-89, DJG */
	} TimbreDef;

/*-------------------------------------------------------------------------*/

typedef struct {
   BYTE2   major, minor;
   char    filler1 [40];
   BYTE2   ticks_in_beat;
   BYTE2   beats_in_measure;
   BYTE2   axis_y, axis_x;
   char    filler2;
   char    music_mode;
   UBYTE1  filler3 [90+38+15];
   float   basic_tempo;
} Rol_Header;

typedef struct {
   char f_name [40];
   char out_name [40];
   int  ID;
   int  out_file_ID;
   Rol_Header  rol_header;
} Rol_File;

typedef struct {
   NoteEvent    *note_data;
   VolumeEvent  *vol_data;
   TempoEvent   *tempo_data;
   InstrumEvent *instr_data;
   PitchEvent   *pitch_data;
   long note_seek, vol_seek, tempo_seek, instr_seek, pitch_seek;
   int  now_time, end_time;
   int  vol_count, tempo_count, instr_count, pitch_count;
   int  note_ptr, vol_ptr, tempo_ptr, instr_ptr, pitch_ptr;
} Event;

#pragma pack()
