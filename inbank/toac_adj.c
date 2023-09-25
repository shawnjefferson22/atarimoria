/* Returns a character's adjustment to armor class	 -JWT-	 */
//int8 __fastcall__ create1_toac_adj(void)
{
  int8u stat;


  stat = py.stats.use_stat[A_DEX];
  if	  (stat <   4)	return(-4);
  else if (stat ==  4)	return(-3);
  else if (stat ==  5)	return(-2);
  else if (stat ==  6)	return(-1);
  else if (stat <  15)	return( 0);
  else if (stat <  18)	return( 1);
  else if (stat <  59)	return( 2);
  else if (stat <  94)	return( 3);
  else if (stat < 117)	return( 4);
  else			return( 5);
}
