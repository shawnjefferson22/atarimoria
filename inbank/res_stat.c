/* Restore a stat.  Return TRUE only if this actually makes a difference. */
//int8u __fastcall__ res_stat(int8u stat)
{
  int8 i;


  i = py.stats.max_stat[stat] - py.stats.cur_stat[stat];
  if (i) {
    py.stats.cur_stat[stat] += i;
    bank_set_use_stat(stat, INBANK_RETBANK);
    bank_prt_stat(stat, INBANK_RETBANK);
    return TRUE;
  }
  return FALSE;
}
