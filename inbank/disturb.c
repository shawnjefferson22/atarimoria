#undef INBANK_NAME
#undef INBANK_FUNCHELPER
#undef INBANK_FUNCHELPER1

#define INBANK_NAME(name)   name
#define INBANK_FUNCHELPER	INBANK_NAME(INBANK_MOD)##_search_off
#define INBANK_FUNCHELPER1  INBANK_NAME(INBANK_MOD)##_rest_off

/* Something happens to dungeon1_disturb the player.		-CJS-
   The first arg indicates a major dungeon1_disturbance, which affects search.
   The second arg indicates a light change. */
//void __fastcall__ disturb(int8u s, int8u l)
{
  command_count = 0;
  if (s && (py.flags.status & PY_SEARCH))
    //search_off();
    INBANK_FUNCHELPER();
  if (py.flags.rest != 0)
    //dungeon1_rest_off();
    INBANK_FUNCHELPER1();
  if (l || find_flag) {
      find_flag = FALSE;
      bank_check_view(INBANK_RETBANK);
  }
  //flush();
  while (kbhit()) cgetc();		// flush code -SJ
}
