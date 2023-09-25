#undef INBANK_NAME
#undef INBANK_FUNCHELPER
#undef INBANK_FUNCHELPER1
#undef INBANK_FUNCHELPER2

#define INBANK_NAME(name)  	name
#define INBANK_FUNCHELPER  	INBANK_NAME(INBANK_MOD)##_bit_pos
#define INBANK_FUNCHELPER1  INBANK_NAME(INBANK_MOD)##_get_spell
#define INBANK_FUNCHELPER2  INBANK_NAME(INBANK_MOD)##_get_check


/* Return spell number and failure chance		-RAK-	*/
/* returns -1 if no spells in book
   returns 1 if choose a spell in book to cast
   returns 0 if don't choose a spell, i.e. exit with an escape */
//int8 __fastcall__ cast_spell(char *prompt, int8u item_val, int8 *sn, int8u *sc)
{
  int32u j;
  int8u i;
  int8u spell[31], first_spell;
  int8 result, k;
  register spell_type *s_ptr;


  result = -1;
  i = 0;
  vbxe_bank(VBXE_OBJBANK);
  j = inventory[item_val].flags;
  //first_spell = cast_bit_pos(&j);
  first_spell = INBANK_FUNCHELPER(&j);

  /* set j again, since bit_pos modified it */
  j = inventory[item_val].flags & spell_learned;

  vbxe_bank(VBXE_SPLBANK);
  s_ptr = magic_spell[py.misc.pclass-1];
  while(j) {
    //k = cast_bit_pos(&j);
    k = INBANK_FUNCHELPER(&j);
    if (s_ptr[k].slevel <= py.misc.lev) {
	  spell[i] = k;
	  ++i;
	}
  }

  if (i > 0) {
    //result = cast_get_spell(spell, i, sn, sc, prompt, first_spell);
    result = INBANK_FUNCHELPER1(spell, i, sn, sc, prompt, first_spell);

    vbxe_bank(VBXE_SPLBANK);
    if (result && magic_spell[py.misc.pclass-1][*sn].smana > py.misc.cmana) {
	  if (class[py.misc.pclass].spell == MAGE)
	    //result = cast_get_check("You summon your limited strength to cast this one! Confirm?");
	    result = INBANK_FUNCHELPER2("You summon your limited strength to cast this one! Confirm?");
	  else
	    //result = cast_get_check("The gods may think you presumptuous for this! Confirm?");
	    result = INBANK_FUNCHELPER2("The gods may think you presumptuous for this! Confirm?");
	}
  }

  return(result);
}
