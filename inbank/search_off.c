#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_change_speed

//void __fastcall__ search_off(void)
{
  bank_check_view(INBANK_RETBANK);
  //dungeon1_change_speed(-1);
  INBANK_FUNCHELPER(-1);
  py.flags.status &= ~PY_SEARCH;
  bank_prt_state(INBANK_RETBANK);
  bank_prt_speed(INBANK_RETBANK);
  py.flags.food_digested--;
}