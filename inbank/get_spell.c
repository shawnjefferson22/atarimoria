#undef INBANK_NAME
#undef INBANK_FUNCHELPER
#undef INBANK_FUNCHELPER1

#define INBANK_NAME(name)  	name
#define INBANK_FUNCHELPER  	INBANK_NAME(INBANK_MOD)##_get_check
#define INBANK_FUNCHELPER1 INBANK_NAME(INBANK_MOD)##_spell_chance


/* Returns spell pointer				-RAK-	*/
//int8u __fastcall__ get_spell(int8u *spell, int8u num, int8 *sn, int8u *sc, char *prompt, int8u first_spell)
{
  register spell_type *s_ptr;
  int8u flag, redraw, offset, i;
  char choice;


  *sn = -1;
  flag = FALSE;
  sprintf(out_val2, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s", (char)(spell[0]+'a'-first_spell), (char)(spell[num-1]+'a'-first_spell), prompt);
  redraw = FALSE;
  vbxe_bank(VBXE_SPLBANK);
  offset = (class[py.misc.pclass].spell==MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

  while (flag == FALSE && bank_get_com(out_val2, &choice, INBANK_RETBANK)) {
    if (isupper(choice)) {
	  *sn = choice-'A'+first_spell;
	  /* verify that this is in spell[], at most 22 entries in spell[] */
	  for (i = 0; i < num; ++i)
	    if (*sn == spell[i])
	      break;
	    if (i == num)
	      *sn = -2;
	    else {
	      s_ptr = &magic_spell[py.misc.pclass-1][*sn];
	      //sprintf(out_val1, "Cast %s (%d mana, %d%% fail)?", spell_names[*sn+offset], s_ptr->smana, cast_spell_chance(*sn));
	      sprintf(out_val1, "Cast %s (%d mana, %d%% fail)?", spell_names[*sn+offset], s_ptr->smana, INBANK_FUNCHELPER1(*sn));
	      if (INBANK_FUNCHELPER(out_val1))			//cast_get_check
		    flag = TRUE;
	      else
		    *sn = -1;
	    }
	}
    else if (islower(choice)) {
	  *sn = choice-'a'+first_spell;
	  /* verify that this is in spell[], at most 22 entries in spell[] */
	  for (i = 0; i < num; ++i)
	    if (*sn == spell[i])
	      break;
	    if (i == num)
	      *sn = -2;
	    else
	      flag = TRUE;
	}
    else if (choice == '*') {
	  /* only do this drawing once */
	  if (!redraw) {
	    bank_vbxe_savescreen(INBANK_RETBANK);
	    redraw = TRUE;
	    bank_print_spells(spell, num, FALSE, first_spell, INBANK_RETBANK);

	    bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
		         "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
		         "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
		         "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", num+2, 0, INBANK_RETBANK);
	  }
	}
    else if (isalpha(choice))
	  *sn = -2;
    else {
	  *sn = -1;
	}

    if (*sn == -2) {
	  sprintf (out_val1, "You don't know that %s.", (offset == SPELL_OFFSET ? "spell" : "prayer"));
	  bank_msg_print(out_val1, INBANK_RETBANK);
	}
  }

  if (redraw)
    bank_vbxe_restorescreen(INBANK_RETBANK);

  //erase_line(MSG_LINE, 0);
  bank_vbxe_cleartoeol(0, MSG_LINE, INBANK_RETBANK);

  if (flag)
    //*sc = cast_spell_chance(*sn);
    *sc = INBANK_FUNCHELPER1(*sn);

  return(flag);
}