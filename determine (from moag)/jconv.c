/*
 * jdmain.c
 *
 * Copyright (C) 1991, 1992, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a trivial test user interface for the JPEG decompressor.
 * It should work on any system with Unix- or MS-DOS-style command lines.
 *
 * Two different command line styles are permitted, depending on the
 * compile-time switch TWO_FILE_COMMANDLINE:
 *      djpeg [options]  inputfile outputfile
 *      djpeg [options]  [inputfile]
 * In the second style, output is always to standard output, which you'd
 * normally redirect to a file or pipe to some other program.  Input is
 * either from a named file or from standard input (typically redirected).
 * The second style is convenient on Unix but is unhelpful on systems that
 * don't support pipes.  Also, you MUST use the first style if your system
 * doesn't do binary I/O to stdin/stdout.
 */

#include "jinclude.h"
#ifdef INCLUDES_ARE_ANSI
#include <stdlib.h>             /* to declare exit() */
#endif
#ifdef NEED_SIGNAL_CATCHER
#include <signal.h>             /* to declare signal() */
#endif

#ifdef THINK_C
#include <console.h>            /* command-line reader for Macintosh */
#endif

#ifdef DONT_USE_B_MODE          /* define mode parameters for fopen() */
#define READ_BINARY     "r"
#define WRITE_BINARY    "w"
#else
#define READ_BINARY     "rb"
#define WRITE_BINARY    "wb"
#endif

#ifndef EXIT_FAILURE            /* define exit() codes if not provided */
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_SUCCESS
#ifdef VMS
#define EXIT_SUCCESS  1         /* VMS is very nonstandard */
#else
#define EXIT_SUCCESS  0
#endif
#endif


#include "jversion.h"           /* for version message */


/*
 * PD version of getopt(3).
 */

#include "egetopt.c"


/*
 * This list defines the known output image formats
 * (not all of which need be supported by a given version).
 * You can change the default output format by defining DEFAULT_FMT;
 * indeed, you had better do so if you undefine PPM_SUPPORTED.
 */

typedef enum {
	FMT_GIF,                /* GIF format */
	FMT_PPM,                /* PPM/PGM (PBMPLUS formats) */
	FMT_RLE,                /* RLE format */
	FMT_TARGA,              /* Targa format */
	FMT_TIFF                /* TIFF format */
} IMAGE_FORMATS;


#ifndef DEFAULT_FMT             /* so can override from CFLAGS in Makefile */
#define DEFAULT_FMT     FMT_PPM
#endif

static IMAGE_FORMATS requested_fmt;


/*
 * This routine gets control after the input file header has been read.
 * It must determine what output file format is to be written,
 * and make any other decompression parameter changes that are desirable.
 */

METHODDEF void
d_ui_method_selection (decompress_info_ptr cinfo)
{
  /* select output file format */
  /* Note: jselwxxx routine may make additional parameter changes,
   * such as forcing color quantization if it's a colormapped format.
   */
   jselwvga(cinfo);
}


/*
 * Signal catcher to ensure that temporary files are removed before aborting.
 * NB: for Amiga Manx C this is actually a global routine named _abort();
 * see -Dsignal_catcher=_abort in CFLAGS.  Talk about bogus...
 */

#ifdef NEED_SIGNAL_CATCHER

static external_methods_ptr emethods; /* for access to free_all */

GLOBAL void
signal_catcher (int signum)
{
  emethods->trace_level = 0;    /* turn off trace output */
  (*emethods->free_all) ();     /* clean up memory allocation & temp files */
  exit(EXIT_FAILURE);
}

#endif


LOCAL void
usage (char * progname)
/* complain about bad command line */
{
  fprintf(stderr, "\nusage: jv ");
  fprintf(stderr, "[-b] [-g] [-1] [-D] [-d] [-m mem]");
#ifdef TWO_FILE_COMMANDLINE
  fprintf(stderr, " jpegfile.jpg outfile.vga\n");
#else
  fprintf(stderr, " [inputfile]\n");
#endif
  fprintf(stderr, "\nThis program converts a 256 color JFIF (A type of JPEG) file");
  fprintf(stderr, "\ninto .VGA files suitable for use with 256PAINT \n");
  exit(EXIT_FAILURE);
}


/*
 * The main program.
 */

GLOBAL int
jconv (char *infname, char *outfname)
{
  struct decompress_info_struct cinfo;
  struct decompress_methods_struct dc_methods;
  struct external_methods_struct e_methods;
  int c;


  /* Initialize the system-dependent method pointers. */
  cinfo.methods = &dc_methods;
  cinfo.emethods = &e_methods;
  jselerror(&e_methods);        /* error/trace message routines */
  jselmemmgr(&e_methods);       /* memory allocation routines */
  dc_methods.d_ui_method_selection = d_ui_method_selection;

  /* Now OK to enable signal catcher. */
#ifdef NEED_SIGNAL_CATCHER
  emethods = &e_methods;
  signal(SIGINT, signal_catcher);
#ifdef SIGTERM                  /* not all systems have SIGTERM */
  signal(SIGTERM, signal_catcher);
#endif
#endif

  /* Set up default JPEG parameters. */
  j_d_defaults(&cinfo, TRUE);
  requested_fmt = DEFAULT_FMT;  /* set default output file format */

  /* Select the input and output files */


  if ((cinfo.input_file = fopen(infname, READ_BINARY)) == NULL) {
    fprintf(stderr, "jconv: can't open %s\n", infname);
    exit(EXIT_FAILURE);
  }
  if ((cinfo.output_file = fopen(outfname, WRITE_BINARY)) == NULL) {
    fprintf(stderr, "jconv: can't open %s\n", outfname);
    exit(EXIT_FAILURE);
  }

  
  /* Set up to read a JFIF or baseline-JPEG file. */
  /* A smarter UI would inspect the first few bytes of the input file */
  /* to determine its type. */
#ifdef JFIF_SUPPORTED
  jselrjfif(&cinfo);
#else
  You shoulda defined JFIF_SUPPORTED.   /* deliberate syntax error */
#endif

  /* Do it to it! */
  jpeg_decompress(&cinfo);

  /* All done. */
  return 0;                     /* suppress no-return-value warnings */
}
