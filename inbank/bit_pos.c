/* Returns position of first set bit			-RAK-	*/
/*     and clears that bit */
//int8 __fastcall__ bit_pos(int32u *test)
{
  register int8u i;
  int32u mask = 0x1;


  for (i = 0; i < sizeof(*test)*8; ++i) {
    if (*test & mask) {
      *test &= ~mask;
      return(i);
    }
    mask <<= 1;
  }

  /* no one bits found */
  return(-1);
}
