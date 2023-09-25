/* Decreases players hit points and sets death flag if necessary*/
/*							 -RAK-	 */
//void __fastcall__ take_hit(int8u damage, char *hit_from)
{
  if (py.flags.invuln > 0)
    damage = 0;

  // FIXME: DEBUGGING
  //sprintf(dbgstr, "p:%-3d", damage);
  //bank_prt(dbgstr, 23, 0, INBANK_RETBANK);
  //bank_vbxe_cputsxy(0, 21, dbgstr, 0x0F, INBANK_RETBANK);
  // FIXME: DEBUGGING

  py.misc.chp -= damage;
  if (py.misc.chp < 0) {
    if (!death) {
	  death = TRUE;
	  strcpy(died_from, hit_from);
	  total_winner = FALSE;
	}

    new_level_flag = TRUE;
  }
  else
    bank_prt_chp(INBANK_RETBANK);
}