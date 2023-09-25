//int8u __fastcall__ modify_stat(int8u stat, int8 amount)                                // amount was int
{
  register int8 i;
  int8u tmp_stat;
  int8u loop;


  tmp_stat = py.stats.cur_stat[stat];
  loop = (amount < 0 ? -amount : amount);
  for (i = 0; i < loop; ++i) {
    if (amount > 0) {
	  if (tmp_stat < 18)
	    ++tmp_stat;
	  else if (tmp_stat < 108)
	    tmp_stat += 10;
	  else
	    tmp_stat = 118;
	}
    else {
	  if (tmp_stat > 27)
	    tmp_stat -= 10;
	  else if (tmp_stat > 18)
	    tmp_stat = 18;
	  else if (tmp_stat > 3)
	    --tmp_stat;
	}
  }

  return tmp_stat;
}