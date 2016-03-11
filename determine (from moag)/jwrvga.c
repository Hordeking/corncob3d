/*
 * jwrppm.c
 *
 * Copyright (C) 1991, 1992, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains routines to write output images in PPM format.
 * The PBMPLUS library is required (well, it will be in the real version).
 *
 * These routines may need modification for non-Unix environments or
 * specialized applications.  As they stand, they assume output to
 * an ordinary stdio stream.
 *
 * These routines are invoked via the methods put_pixel_rows, put_color_map,
 * and output_init/term.
 */

#include "jinclude.h"

/* Some real kludgy stuff here.  Some array space for the palette, and */
/*  The file size stuff for padding the VGA file with ridiculous space */

	unsigned char the_pal[768];
	unsigned long bogus_padsize;

/*
 * Haven't yet got around to making this work with text-format output,
 * hence cannot handle pixels wider than 8 bits.
 */

#ifndef EIGHT_BIT_SAMPLES
  Sorry, this code only copes with 8-bit JSAMPLEs. /* deliberate syntax err */
#endif


/*
 * Write the file header.
 */

METHODDEF void
output_init (decompress_info_ptr cinfo)
{
	register FILE * outfile = cinfo->output_file;
	unsigned char gline[4];


//  if (cinfo->out_color_space == CS_GRAYSCALE) {
 //   /* emit header for raw PGM format */
 //   fprintf(cinfo->output_file, "P5\n%ld %ld\n%d\n",
	    //cinfo->image_width, cinfo->image_height, 255);
  //} else if (cinfo->out_color_space == CS_RGB) {
    ///* emit header for raw PPM format */
    //fprintf(cinfo->output_file, "P6\n%ld %ld\n%d\n",
	    //cinfo->image_width, cinfo->image_height, 255);
  //} else {
    //ERREXIT(cinfo->emethods, "PPM output must be grayscale or RGB");
  //}

	printf("Welcome to Output_init");
	printf("The final_out_comps = %d \n",(int) cinfo->final_out_comps);
	if (cinfo->final_out_comps != 1)
    		ERREXIT(cinfo->emethods, "output must be 256 Color w/ Palette");
	printf(" The image_width and height are : %ld %ld \n",
			cinfo->image_width,cinfo->image_height);
	printf("The data precision is : %d",(int) cinfo->data_precision);
	
/* The following kludgy code puts the x y size into file in VGA format */

	*(int*) &gline[0]=cinfo->image_width-1;
	*(int*) &gline[2]=cinfo->image_height-1;
	putc(gline[0],outfile);
	putc(gline[1],outfile);
	putc(gline[2],outfile);
	putc(gline[3],outfile);

	bogus_padsize=((cinfo->image_height-1)/8+1)*8*cinfo->image_width;
	bogus_padsize-= cinfo->image_height*cinfo->image_width;
}


/*
 * Write some pixel data.
 */

METHODDEF void
put_pixel_rows (decompress_info_ptr cinfo, int num_rows,
		JSAMPIMAGE pixel_data)
{
  register FILE * outfile = cinfo->output_file;
  register JSAMPROW ptr0;
  register long col;
  register long width = cinfo->image_width;
  register int row;

printf("\n Writing %d rows of nice data ",num_rows);
    for (row = 0; row < num_rows; row++) {
      ptr0 = pixel_data[0][row];
      for (col = width; col > 0; col--) {
	putc(GETJSAMPLE(*ptr0), outfile);
	ptr0++;
      }
    }
}


/*
 * Write the color map.
 * For PPM output, we just demap the output data!
 */

METHODDEF void
put_color_map (decompress_info_ptr cinfo, int num_colors, JSAMPARRAY colormap)
{
  /* This example shows how you might write full-color RGB data (3 components)
   * to an output file in which the data is stored 3 bytes per pixel.
   */
  register FILE * outfile = cinfo->output_file;
  register JSAMPROW ptr0, ptr1, ptr2;
  register long col;
  register int icolor;
  
  if(cinfo->color_out_comps != 3 )
    ERREXIT(cinfo->emethods, "Must be a 256 color std VGA file with palette");
  if(num_colors != 256 )
    ERREXIT(cinfo->emethods, "Must be a 256 color file with palette");
    ptr0 = colormap[0];
    ptr1 = colormap[1];
    ptr2 = colormap[2];
  for (icolor = 0; icolor < 256; icolor++) {
	the_pal[icolor*3]= GETJSAMPLE(*ptr0);
	ptr0++;
	the_pal[icolor*3+1]= GETJSAMPLE(*ptr1);
	ptr1++;
	the_pal[icolor*3+2]= GETJSAMPLE(*ptr2);
	ptr2++;
  }
}


/*
 * Finish up at the end of the file.
 */

METHODDEF void
output_term (decompress_info_ptr cinfo)
{
  register FILE * outfile = cinfo->output_file;
	int i;

/* Put out silly .vga padding */
	for(i=0;i<bogus_padsize;i++)
		putc((char) 0,outfile);

/* Now put out 2 more silly bytes */
	putc((char) 0,outfile);
	putc((char) 0,outfile);

/* Put out palette */
	for(i=0;i<768;i++)
		putc(the_pal[i] >> 2,outfile);


	printf("\n Supposedly flushing the file now \n");
  /* No work except to make sure we wrote the output file OK */
  fflush(cinfo->output_file);
 if (ferror(cinfo->output_file))
    ERREXIT(cinfo->emethods, "Output file write error");
}


/*
 * The method selection routine for PPM format output.
 * This should be called from d_ui_method_selection if PPM output is wanted.
 */

GLOBAL void
jselwvga (decompress_info_ptr cinfo)
{
  cinfo->methods->output_init = output_init;
  cinfo->methods->put_color_map = put_color_map;
  cinfo->methods->put_pixel_rows = put_pixel_rows;
  cinfo->methods->output_term = output_term;

/* Force quantization on, for 256 color mapped mode */
    cinfo->quantize_colors = TRUE;
}

