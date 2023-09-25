/* Returns a character's adjustment to damage            -JWT-   */
//int8 __fastcall__ todam_adj(void)
{
  int8u stat;


  stat = py.stats.use_stat[A_STR];
  if      (stat <   4)  return(-2);
  else if (stat <   5)  return(-1);
  else if (stat <  16)  return( 0);
  else if (stat <  17)  return( 1);
  else if (stat <  18)  return( 2);
  else if (stat <  94)  return( 3);
  else if (stat < 109)  return( 4);
  else if (stat < 117)  return( 5);
  else                  return( 6);
}