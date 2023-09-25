/* Basic bresenham line algorithm -SJ */
//int8u __fastcall__ los(int8u fromY, int8u fromX, int8u toY, int8u toX)
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


