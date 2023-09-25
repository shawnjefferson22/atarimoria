/* Changes stats by given amount                                -JWT-   */
//void __fastcall__ change_stat(int8u stat, int8 amount)
{
  register int8 i;
  register int8u tmp_stat;


  tmp_stat = py.stats.max_stat[stat];
  if (amount < 0)
    for (i = 0; i > amount; --i) {
	  if (tmp_stat > 108)
	    --tmp_stat;
	  else if (tmp_stat > 88)
	    tmp_stat += -randint(6) - 2;
	  else if (tmp_stat > 18) {
	    tmp_stat += -randint(15) - 5;
	    if (tmp_stat < 18)
	      tmp_stat = 18;
	  }
	  else if (tmp_stat > 3)
	    --tmp_stat;
    }
  else
    for (i = 0; i < amount; ++i) {
	  if (tmp_stat < 18)
	    tmp_stat++;
	  else if (tmp_stat < 88)
	    tmp_stat += randint(15) + 5;
	  else if (tmp_stat < 108)
	    tmp_stat += randint(6) + 2;
	  else if (tmp_stat < 118)
	    ++tmp_stat;
    }

  py.stats.max_stat[stat] = tmp_stat;
}
