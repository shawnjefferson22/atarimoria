/* Returns a character's adjustment to hit.		 -JWT-	 */
//int8 __fastcall__ create1_tohit_adj(void)
{
  int8u total, stat;


  stat = py.stats.use_stat[A_DEX];
  if	  (stat <   4)	total = -3;
  else if (stat <   6)	total = -2;
  else if (stat <   8)	total = -1;
  else if (stat <  16)	total =	 0;
  else if (stat <  17)	total =	 1;
  else if (stat <  18)	total =	 2;
  else if (stat <  69)	total =	 3;
  else if (stat < 118)	total =	 4;
  else			total =	 5;
  stat = py.stats.use_stat[A_STR];
  if	  (stat <   4)	total -= 3;
  else if (stat <   5)	total -= 2;
  else if (stat <   7)	total -= 1;
  else if (stat <  18)	total -= 0;
  else if (stat <  94)	total += 1;
  else if (stat < 109)	total += 2;
  else if (stat < 117)	total += 3;
  else			total += 4;
  return(total);
}