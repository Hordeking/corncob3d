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
   File: csoundc.c
   C interface to memory resident sound driver.
*/

#include  "cflags.h"

#include <dos.h>

#ifdef TURBOC
   #include <mem.h>
#else
   #include <memory.h>
   /* This pragma statement if for QuickC, which will generate a run-time
	  error in GetSoundDrvVersion() because of the far pointer calculation. */
   #pragma  check_pointer (off)
#endif

#define  DOS_FUNCTION_CALL         0x21
#define  SOUND_DRIVER_INT          101
#define  DOS_GET_VECTOR_FUNCTION   0x35

/* The sound driver code starts with the following signature and can be
   used to test if the sound driver is installed.  The version number is
   a two byte value which precedes the signature and is a hex readable
   number: version 1.5 will be 0x150.  */

static char adlib[] = {"SOUND-DRIVER-AD-LIB"};

int GetSoundDrvVersion ()
{
   char far * far *int_ptr;
   char far *sig_ptr;
   char str [40];
   int n, version = 0;

   /* 404 is the address of interrupt vector 101. */
   int_ptr = (char (far * far *)) 404;
   sig_ptr = *int_ptr;
   sig_ptr -= (sizeof (adlib) + 4);

   /* Copy signature to a local buffer. */
   for (n=0; n < strlen (adlib)+2; n++) str [n] = sig_ptr [n];

   if (!memcmp (adlib, &str [2], strlen (adlib)))
	  memcpy (&version, str, sizeof (int));

   return (version);
}

#ifndef TURBOC
   /* Returns pointer checking to its previous status. */
   #pragma  check_pointer ()
#endif

SoundCall (fn_num, stack)
   int fn_num, stack;
{
   union REGS inregs, outregs;
   struct SREGS segregs;
   int result;
   char far *c = (char *) &stack;

   inregs.x.si = fn_num;
   inregs.x.bx = FP_OFF (c);
   segregs.es = FP_SEG (c);
   result = int86x (SOUND_DRIVER_INT, &inregs, &outregs, &segregs);
   return (result);
}

#if 0
main ()
{
   int n;
   n = GetSoundDrvVersion();
   if (!n)
	  printf ("Sound driver not installed.\n");
   else
	  printf ("Sound driver version %x.%x\n", n >> 8, n & 0xff);
}
#endif
