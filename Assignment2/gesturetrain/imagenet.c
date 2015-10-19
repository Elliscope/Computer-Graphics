#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pgmimage.h>
#include <backprop.h>
#include <string.h>

#define TARGET_HIGH 0.9
#define TARGET_LOW 0.1

/*** This is the target output encoding for a network with one output unit.
     It scans the image name, and if it's an image of down gesture then
     it sets the target unit to HIGH; otherwise it sets it to LOW.
     Remember, units are indexed starting at 1, so target unit 1
     is the one to change....  ***/

void load_target(IMAGE *img, BPNN *net)
{
  int num;
  char userid[40], gestures[40], photo[40];

  userid[0] = gestures[0] = photo[0] = '\0';

  /*** scan in the image features ***/
  sscanf(NAME(img), "%[^_]_%[^_]_%d.%[^_]",
    userid, gestures, &num, photo);

  if (!strcmp(gestures, "down")) {
    net->target[1] = TARGET_HIGH;  /* it's down, set target to HIGH */
  } else {
    net->target[1] = TARGET_LOW;   /* not down, set it to LOW */
  }
}

/***********************************************************************/
/********* You shouldn't need to change any of the code below.   *******/
/***********************************************************************/

void load_input_with_image(IMAGE *img, BPNN *net)
{
  double *units;
  int nr, nc, imgsize, i, j, k;

  nr = ROWS(img);
  nc = COLS(img);
  imgsize = nr * nc;;
  if (imgsize != net->input_n) {
    printf("LOAD_INPUT_WITH_IMAGE: This image has %d pixels,\n", imgsize);
    printf("   but your net has %d input units.  I give up.\n", net->input_n);
    exit (-1);
  }

  units = net->input_units;
  k = 1;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      units[k] = ((double) img_getpixel(img, i, j)) / 255.0;
      k++;
    }
  }
}
