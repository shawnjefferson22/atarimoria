/* Change players hit points in some manner		-RAK-	*/
//int8u __fastcall__ hp_player(int16 num)
{
  int8u res;


  res = FALSE;
  if (py.misc.chp < py.misc.mhp) {
    py.misc.chp += num;
    if (py.misc.chp > py.misc.mhp) {
	  py.misc.chp = py.misc.mhp;
	  py.misc.chp_frac = 0;
	}
    bank_prt_chp(INBANK_RETBANK);

    num = num / 5;
    if (num < 3) {
	  if (num == 0) bank_msg_print("You feel a little better.", INBANK_RETBANK);
	  else	        bank_msg_print("You feel better.", INBANK_RETBANK);
    }
    else {
	  if (num < 7) bank_msg_print("You feel much better.", INBANK_RETBANK);
	  else	       bank_msg_print("You feel very good.", INBANK_RETBANK);
    }
    res = TRUE;
  }
  return(res);
}