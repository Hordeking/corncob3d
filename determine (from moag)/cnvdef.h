
#define  FOURTH       5
#define  TRITONE      6
#define  FIFTH        7

#define  CLEF         1
#define  TIME_SIG     2
#define  KEY_SIG      3
#define  NOTE_4       4
#define  NOTE         5
#define  REST         6
#define  BAR_LINE     7
#define  OTHER_BARS   8
#define  FINAL_BAR    9
#define  TEMPO        10
#define  VOLUME       11
#define  NOTE_IRR     12
#define  REST_IRR     13
#define  NR_EVENTS    (VOLUME+1)            /* number of codes + 1 */

#define  SOL2         1
#define  FA4          3

#define  TYPE_BITS    0x0f
#define  DBL_ROND     1
#define  ROND         2
#define  BLANCHE      3
#define  NOIR         4
#define  CROCHE       5
#define  CROCHE2      6
#define  CROCHE3      7
#define  CROCHE4      8

#define  DOT_BITS     (3 << 4)
#define  DOT          (1 << 4)
#define  DBL_DOT      (2 << 4)

#define  PITCH_SHIFT  6
#define  PITCH_BITS   (7 << PITCH_SHIFT)
#define  OCTAVE_SHIFT 9
#define  OCTAVE_BITS  (7 << OCTAVE_SHIFT)
#define  ALT_SHIFT    12
#define  ALT_BITS     (7 << ALT_SHIFT)

#define  SHARP        1
#define  FLAT         2
#define  NATURAL      3
#define  DBL_SHARP    4
#define  DBL_FLAT     5

#define  GRP_BITS     3
#define  BEAM_FLAGS   0x07
#define  START_BEAM   1
#define  MID_BEAM     3
#define  END_BEAM     2
#define  EN_ST_BEAM   4

#define  TIE_FLAGS    (0x07 << 3)
#define  START_TIE    (1 << 3)
#define  MID_TIE      (3 << 3)
#define  END_TIE      (2 << 3)
#define  EN_ST_TIE    (4 << 3)

#define  IRR_FLAGS    (0x07 << 6)
#define  START_IRR    (1 << 6)
#define  MID_IRR      (3 << 6)
#define  END_IRR      (2 << 6)
#define  EN_ST_IRR    (4 << 6)

#define  NOTE_EVENT   1
#define  EOF_EVENT    0

#define  NR_VOICES    11

#define  BYTE1        char
#define  BYTE2        int
#define  BYTE4        long int
#define  UBYTE1       unsigned char
#define  UBYTE2       unsigned int
#define  UBYTE4       unsigned long int

#define  DO           do{
#define  WHILE        }while

#define  DIV_BY_2(n)      (!(n & 0x01))
#define  DIV_BY_3(n)      (!(n % 3))
#define  DIV_BY_4(n)      (!(n & 0x03))

#define  NOT_BAR_LINE(x)  (x->code < BAR_LINE || x->code > FINAL_BAR)
#define  NEXT(x)          x = x->next_sym
#define  PREV(x)          x = x->prev_sym

#define  Make_New_Symbol()  ((Symbol *) Struct_Alloc (sizeof(Symbol)))
#define  Make_New_Note()    ((Note *)   Struct_Alloc (sizeof(Note)))

#define  MAX_STOCK         64
