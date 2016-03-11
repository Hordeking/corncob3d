/* Prototypes are listed alphabetically */

Symbol *Append_New_Symbol (Symbol *);
int    End_Of_File (int, int);
int    Find_Next_Event (int);
UBYTE1 Get_Figure (int);
Symbol *Get_Next_Event (Symbol *);
Symbol *Get_Next_Measure (int, int, int);
Symbol *Get_Next_Note (Symbol *);
int    Group_Notes (Symbol *);
int    Init_Key_Sig (int);
Symbol *Insert_New_Element (Symbol *);
int    Make_Measures (int);
int    Read_Input_File (Global_Info *);
Symbol *Rest_Fill (Symbol *, int, int);
int    Set_Durations (Symbol *);
int    Set_Pitches (Symbol *);
int    Set_Up (int, UBYTE1, int);
int    SetUp_Start (Global_Info *);
Symbol *Split_Note (Symbol *, int);
void   *Struct_Alloc (int);


/* This was taken from <stdio.h> */
#ifndef _FILE_DEFINED
#define  FILE    struct _iobuf
#define _FILE_DEFINED
#endif

Symbol *Write_Measure (Symbol *, int, FILE *, int, BYTE4 *);


