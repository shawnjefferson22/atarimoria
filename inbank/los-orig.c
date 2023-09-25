int8u __fastcall__ los(int8u fromY, int8u fromX, int8u toY, int8u toX)

/* Original Moria code below. Had a lot of issues with type conversions and it seemed
   overly complicated, so I replaced it with a basic bresenham line algorithm.
   Hopefully I haven't introduced any major gameplay changes.  Seems to work in
   a very similar way. -SJ */

/* A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
   4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.

   Returns TRUE if a line of sight can be traced from x0, y0 to x1, y1.

   The LOS begins at the center of the tile [x0, y0] and ends at
   the center of the tile [x1, y1].  If los() is to return TRUE, all of
   the tiles this line passes through must be transparent, WITH THE
   EXCEPTIONS of the starting and ending tiles.

   We don't consider the line to be "passing through" a tile if
   it only passes across one corner of that tile. */

/* Because this function uses (short) ints for all calculations, overflow
   may occur if deltaX and deltaY exceed 90. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#if 0
{
  register int16 tmp, deltaX, deltaY;
  register int16 p_y, px;		// x & y position loop variables
  register int16 dy;		/* "fractional" y position	*/
  register int16 dx;		/* "fractional" y position	*/
  int16 scale2;			/* above scale factor / 2 */
  int16 scale;			/* above scale factor */
  int16 xSign;			/* sign of deltaX */
  int16 ySign;			/* sign of deltaY */
  int16 m;				/* slope or 1/slope of LOS */


  deltaX = (int16) toX - (int16) fromX;
  deltaY = (int16) toY - (int16) fromY;

  /* Adjacent? */
  if ((deltaX < 2) && (deltaX > -2) && (deltaY < 2) && (deltaY > -2))
    return TRUE;

  vbxe_bank(VBXE_CAVBANK);

  /* Handle the cases where deltaX or deltaY == 0. */
  if (deltaX == 0) {
    if (deltaY < 0) {
	  tmp = fromY;
	  fromY = toY;
	  toY = tmp;
	}

    for (p_y = fromY + 1; p_y < toY; ++p_y)
	  if ((CAVE_ADR((int8u) p_y, fromX))->fval >= MIN_CLOSED_SPACE)
	    return FALSE;
    return TRUE;
  }
  else if (deltaY == 0) {
    if (deltaX < 0)	{
	  tmp = fromX;
	  fromX = toX;
	  toX = tmp;
	}
    for (px = fromX + 1; px < toX; ++px)
	  if ((CAVE_ADR(fromY, (int8u) px))->fval >= MIN_CLOSED_SPACE)
	    return FALSE;
    return TRUE;
  }

  /* Now, we've eliminated all the degenerate cases.
     In the computations below, dy (or dx) and m are multiplied by a
     scale factor, scale = abs(deltaX * deltaY * 2), so that we can use
     integer arithmetic. */

  scale2 = abs(deltaX * deltaY);
  scale = scale2 << 1;
  xSign = (deltaX < 0) ? -1 : 1;
  ySign = (deltaY < 0) ? -1 : 1;

  /* Travel from one end of the line to the other, oriented along the longer axis. */

  if (abs(deltaX) >= abs(deltaY)) {
	/* We start at the border between the first and second tiles,
	   where the y offset = .5 * slope.  Remember the scale
	   factor.  We have:

	   m = deltaY / deltaX * 2 * (deltaY * deltaX)
	     = 2 * deltaY * deltaY. */

	dy = deltaY * deltaY;
	m = dy << 1;
	px = fromX + xSign;

	/* Consider the special case where slope == 1. */
	if (dy == scale2) {
	  p_y = fromY + ySign;
	  dy -= scale;
	}
	else
	  p_y = fromY;

	while (toX - px) {
      //bank_print_symbol_highlight((int8u)p_y, (int8u)px, INBANK_RETBANK);		// FIXME: debugging!

	  if ((CAVE_ADR((int8u)p_y, (int8u)px))->fval >= MIN_CLOSED_SPACE)
	    return FALSE;

	  dy += m;
	  if (dy < scale2)
	    px += xSign;
	  else if (dy > scale2) {
		p_y += ySign;
		if ((CAVE_ADR((int8u)p_y, (int8u)px))->fval >= MIN_CLOSED_SPACE)
		  return FALSE;
		px += xSign;
		dy -= scale;
	  }
	  else {
		/* This is the case, dy == scale2, where the LOS exactly meets the corner of a tile. */
		px += xSign;
		p_y += ySign;
		dy -= scale;
	  }
	}
	return TRUE;
  }
  else {
	dx = deltaX * deltaX;
	m = dx << 1;

	p_y = fromY + ySign;
	if (dx == scale2) {
	  px = fromX + xSign;
	  dx -= scale;
	}
	else
	  px = fromX;

	while (toY - p_y) {
      //bank_print_symbol_highlight((int8u)p_y, (int8u)px, INBANK_RETBANK);		// FIXME: debugging!

	  if ((CAVE_ADR((int8u)p_y, (int8u)px))->fval >= MIN_CLOSED_SPACE)
	    return FALSE;
	  dx += m;
	  if (dx < scale2)
	    p_y += ySign;
	  else if (dx > scale2) {
		px += xSign;
		if ((CAVE_ADR((int8u)p_y, (int8u)px))->fval >= MIN_CLOSED_SPACE)
		  return FALSE;
		p_y += ySign;
		dx -= scale;
	  }
	  else {
		px += xSign;
		p_y += ySign;
		dx -= scale;
	  }
	}
	return TRUE;
  }
}
#endif

#if 1

/* Basic bresenham line algorithm -SJ */
{
  //register int8 dx, dy;			// moved to zeropage.s
  //register int8 err, e2;
  //register int8 sx, sy;
  extern int8 los_dx, los_dy;
  extern int8 los_sx, los_sy;
  extern int8 los_err, los_e2;
  #pragma zpsym("los_dx")
  #pragma zpsym("los_dy")
  #pragma zpsym("los_sx")
  #pragma zpsym("los_sy")
  #pragma zpsym("los_err")
  #pragma zpsym("los_e2")


  /* X & Y distance and sign */
  los_sx = (fromX < toX ? 1 : -1);			// sign x
  los_sy = (fromY < toY ? 1 : -1);			// sign y

  if (los_sx > 0)							// if (sx) doesn't work, compiler bug? -SJ
    los_dx = toX - fromX;
  else
    los_dx = fromX - toX;

  if (los_sy > 0)
    los_dy = toY - fromY;
  else
    los_dy = fromY - toY;

  /* Adjacent to the player? */
  if ((los_dx < 2) && (los_dy < 2))
    return TRUE;

  los_err = (los_dx > los_dy ? los_dx : -los_dy)/2;

  vbxe_bank(VBXE_CAVBANK);
  while (1) {
    //bank_print_symbol_highlight(fromY, fromX, INBANK_RETBANK);		// FIXME: debugging!
    //sprintf(dbgstr, "%d,%d->%d,%d dy:%d dx:%d sy:%d: sx:%d err:%d e2:%d", fromY, fromX, toY, toX, dy, dx, sy, sx, err, e2);
    //bank_prt(dbgstr, 23, 0, INBANK_RETBANK);
    //cgetc();
	//vbxe_bank(VBXE_CAVBANK);

    los_e2 = los_err;
    if (los_e2 > -los_dx) {
	  los_err -= los_dy;
	  if (los_sx > 0)
	    ++fromX;
	  else
	    --fromX;
	}
    if (los_e2 < los_dy){
	  los_err += los_dx;
	  if (los_sy > 0)
	    ++fromY;
	  else
	    --fromY;
	}

    if ((fromX == toX) && (fromY == toY))   					// if we break before checking for wall,
      break;													// we should be able to see monsters in wall -SJ

    if ((CAVE_ADR(fromY, fromX))->fval >= MIN_CLOSED_SPACE)		// check for wall after one pass of loop, means we don't check
      return FALSE;												// player position -SJ
  }

  return TRUE;
}

#endif
