/* Returns a character's adjustment to disarm		 -RAK-	 */
//int8 __fastcall__ create1_todis_adj(void)
{
  int8u stat;


  stat = py.stats.use_stat[A_DEX];
  if	  (stat <   3)	return(-8);
  else if (stat ==  4)	return(-6);
  else if (stat ==  5)	return(-4);
  else if (stat ==  6)	return(-2);
  else if (stat ==  7)	return(-1);
  else if (stat <  13)	return( 0);
  else if (stat <  16)	return( 1);
  else if (stat <  18)	return( 2);
  else if (stat <  59)	return( 4);
  else if (stat <  94)	return( 5);
  else if (stat < 117)	return( 6);
  else					return( 8);
}
