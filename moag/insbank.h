/*
Definition and type file to be used with the new instrument bank file.
Function prototypes are at the end of this file.
BANK_WRITE_ACCESS must be defined if you wish to update a bank.

Dec-88, DJG, MSA
*/

#define BANK_FILLER_SIZE    8

#define BANK_SIG            "ADLIB-"
#define BANK_SIG_LEN        6

#define BANK_OK             0
#define BANK_NOT_FOUND      -1
#define BAD_BANK_FILE       -2
#define NOT_DEFINED         -3
#define CREATE_ERROR        -4
#define READ_WRITE_ERR      -5

#pragma pack(1)

/* Instrument bank file header */
typedef struct {
   char      majorVersion;
   char      minorVersion;
	char	  sig[ BANK_SIG_LEN];	/* signature: "ADLIB-" */
   unsigned  nrDefined;        /* number of list entries used */
   unsigned  nrEntry;          /* number of total entries in list */
   long      offsetIndex;      /* offset in file of start of list of names */
   long      offsetTimbre;     /* offset in file of start of data */
   char	  filler[ BANK_FILLER_SIZE];      /* must be zero */
} BankHeader;


/* Bank descriptor: */
typedef
	struct {
		int		bank_id;
		BankHeader head;
	} BankRec, * BankPtr;



/* Name of instrument with a pointer to its position in the file */
typedef struct {
   unsigned nrReference;
   char  usedFlag;             /* 0 if not used */
   char  TimbreName [9];       /* max 8 char's + NULL */
} BankEntry;


/* Operator structure */
typedef struct {
   int  ksl;
   int  freqMult;
   int  feedBack;              /* used by operator 0 only */
   int  attack;
   int  sustLevel;
   int  sustain;
   int  decay;
   int  release;
   int  output;
   int  am;
   int  vib;
   int  ksr;
   int  fm;                    /* used by operator 0 only */
} Operator;


/* Timbre structure (old .INS file structure) */
typedef struct {
   char      mode;             /* 0=melodic, 1=percussive */
   char      percVoice;        /* if mode=1, voice number to be used */
   Operator  op0;
   Operator  op1;
   int       wave0;            /* waveform for operator 0 */
   int       wave1;            /* waveform for operator 1 */
} Timbre;

typedef struct {
	char	mode;
	char	percVoice;
	char	param[ 13 * 2];
	char	wave0;
	char	wave1;
	} PackedTimbre;

#pragma pack()

#define  DEFAULT_BANK    "standard"
#define  BANK_EXTENSION  "bnk"


#define MAJ_VERSION	1
#define MIN_VERSION	0


/***************************************************************************
				 FUNCTION PROTOTYPES FOR PUBLIC ROUTINES
****************************************************************************/

/*
Routines for read only access to bank
*/
int Open_Bank (char *fname, int writeFlag, BankPtr bkp);
int Close_Bank (BankPtr bkp);
int Read_Timbre_def (char *ins_name, Timbre *timbre, BankPtr bkp);
int Seek_Nth_Timbre (unsigned n, BankPtr bkp);

/*
Routines for read/write access to bank
BANK_WRITE_ACCESS must be defined in order to user these routines.
*/
int Open_New_Bank (char *name, BankPtr bkp);
int Write_Timbre (char *ins_name, Timbre *timbre, BankPtr bkp);
int Delete_Timbre (char *ins_name, BankPtr bkp);
int Read_timbre_name (unsigned n, char *name, BankPtr bkp);

