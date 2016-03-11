/* This causes malloc() and free() to be used.  If not defined, then static
   memory will be allocated at compile time. */
#define DYNAMIC

#define  NR_VOICES    11

#define  FILLER_SIZE 15

/* Number of events (other than note events) to read from the file at one
   time.  Also used to set storage buffer size. */
#define  MAX_STOCK         12

/* Number of note events to read from the file at one time.  Also used to
   set storage buffer size. */
#define  NOTE_STOCK        64

/* Maximum number of instrument definitions to be read and stored.  This is
   used only if DYNAMIC is not defined. */
#define  MAX_INSTR         35

/* The following indicates the priority of events.  The lower the number,
   the higher the priority.  Notes must have the lowest priority and tempo
   events must have the next lowest priority.  (See also SendTempo() in
   rolplay.c.)
*/
#define  EOF_EVENT    0
#define  INSTR_EVENT  1
#define  VOL_EVENT    2
#define  PITCH_EVENT  3
#define  TEMPO_EVENT  4
#define  NOTE_EVENT   5
#define  NR_EVENTS    6

#define  BYTE1        char
#define  BYTE2        int
#define  BYTE4        long int
#define  UBYTE1       unsigned char
#define  UBYTE2       unsigned int
#define  UBYTE4       unsigned long int

#define  DO           do{
#define  WHILE        }while

extern   int   SoundCall (int,...);

#define  Init_Snd_Drvr()           SoundCall(0)
#define  Start_Time_ToDrvr(a,b)    SoundCall(2,a,b)
#define  Set_State_Drvr(a)         SoundCall(3,a)
#define  Play_Status()             SoundCall(4)
#define  Flush_Queues()            SoundCall(5)
#define  Mode_ToDrvr(a)            SoundCall(6,a)
#define  Volume_ToDrvr(a,b,c,d)    SoundCall(8,a,b,c,d)
#define  Tempo_ToDrvr(a,b,c)       SoundCall(9,a,b,c)
#define  Transpose_ToDrvr(a)       SoundCall(10,a)
#define  Voice_ToDrvr(a)           SoundCall(12,a)
#define  Note_ToDrvr(a,b,c,d,e)    SoundCall(14,a,b,c,d,e)
#define  Note_NoDelay(a,b,c)       SoundCall(15,a,b,c)
#define  Instr_ToDrvr(a,b,c)       SoundCall(16,(Timbre far *)a,b,c)
#define  Pitch_ToDrvr(a,b,c,d,e)   SoundCall(17,a,b,c,d,e)
#define  Ticks_ToDrvr(a)           SoundCall(18,a)
#define  Direct_Note_On(a,b)       SoundCall(19,a,b)
#define  Direct_Note_Off(a)        SoundCall(20,a)
#define  Direct_Set_Ins(a)         SoundCall(21,(Timbre far *)a)
#define  Waveforms_On(a)           SoundCall(23,a)

