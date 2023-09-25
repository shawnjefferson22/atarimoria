#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_stat_adj

/* Returns spell chance of failure for spell		-RAK-	*/
//int8u __fastcall__ spell_chance(int8u spell)
{
  register spell_type *s_ptr;
  int16 chance;
  int8u stat;
  int8u ret;


  vbxe_bank(VBXE_SPLBANK);
  s_ptr = &magic_spell[py.misc.pclass-1][spell];
  chance = (int16) (s_ptr->sfail - 3*(py.misc.lev - s_ptr->slevel));

  if (class[py.misc.pclass].spell == MAGE)
    stat = A_INT;
  else
    stat = A_WIS;

  //chance -= 3 * (cast_stat_adj(stat)-1);
  chance -= 3 * (INBANK_FUNCHELPER(stat)-1);

  if (s_ptr->smana > py.misc.cmana)
    chance += 5 * (s_ptr->smana - py.misc.cmana);
  if (chance > 95)
    chance = 95;
  else if (chance < 5)
    chance = 5;

  ret = (int8u) chance;
  return(ret);
}
