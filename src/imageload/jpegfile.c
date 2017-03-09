#include <stdio.h>
#include <stdlib.h>
#include "jerror.h"
#include "jpeglib.h"
#include "jpegfile.h"

/* For error recovery */
#include <setjmp.h>

unsigned char* image;

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

/* Private JPEG error handler. */

METHODDEF(void) my_error_exit (j_common_ptr cinfo) {
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message.
     Could postpone this until after returning. */
  (*cinfo->err->output_message) (cinfo);
  if (image) {
        free(image);
        image = 0;
  }

  /* Return */
  longjmp(myerr->setjmp_buffer, 1);
}

unsigned char* jpg_load(char* fn, int* width, int* height, int* comps, int t_comps) {
  /* Struct for JPEG decompression parameters and pointers to
     JPEG library working space. */
  struct jpeg_decompress_struct cinfo;

  /* Private JPEG error handler. */
  struct my_error_mgr jerr;

  FILE* infile; 		/* Source file */
  JSAMPARRAY buffer;		/* Scanline buffer */
  int row_stride;               /* Scanline buffer size */
  /* unsigned char* image;          Output image */
  int image_size;		/* Output image size */

  if ((infile = fopen(fn, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", fn);
    return 0;
  }

  /* Set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
       Clean up the JPEG object, close the input file, and return. */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  /* Initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Define stdio source. */
  jpeg_stdio_src(&cinfo, infile);

  /* Read file parameters */
  (void) jpeg_read_header(&cinfo, TRUE);

  /* Set parameters for decompression if defaults need changing here. */

  (void) jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;
  image_size = cinfo.output_height * cinfo.output_width * t_comps;
  *height = cinfo.output_height;
  *width = cinfo.output_width;
  *comps = cinfo.output_components;
  /* Buffer will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
  image = (unsigned char*) malloc(image_size * sizeof(JSAMPLE));

  int max = t_comps;
  if (max < cinfo.output_components) max = cinfo.output_components;
  int i, j, i_index, b_index;
  /* Flip image vertically */
  int outstride = cinfo.output_width * t_comps;
  int offset = (cinfo.output_height - 1) * outstride;
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines. */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    i_index = b_index = 0;
    for (i = 0; i < cinfo.output_width; i++) {
        for (j = 0; j < max; j++) {
            if (j < t_comps) {
                if (j < cinfo.output_components) {
                    *(image + offset + i_index) = *(*buffer + b_index); /* Ptr to ptr to scanline. */
                    b_index++;
                }
                else *(image + offset + i_index) = 0xff; /* Pad with 1's (for alpha) */
                i_index++;
            } else b_index++; /* Skip unrequested components */
        }
    }
    offset -= outstride;
  }

  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

  /* May need to check to see whether any corrupt data warnings occurred
     (test whether jerr.pub.num_warnings is nonzero). */

  return image;
}
