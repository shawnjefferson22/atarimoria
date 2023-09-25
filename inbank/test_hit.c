/* Attacker's level and plusses,  defender's AC		-RAK-	*/
//int8u __fastcall__ test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type)
{
  register int16 i;
  register int8u die;


  bank_disturb(1, 0, INBANK_RETBANK);

  vbxe_bank(VBXE_SPLBANK);
  i = bth + pth * BTH_PLUS_ADJ + (level * class_level_adj[py.misc.pclass][attack_type]);
  vbxe_restore_bank();

  /* pth could be less than 0 if player wielding weapon too heavy for him */
  /* always miss 1 out of 20, always hit 1 out of 20 */
  die = randint(20);
  if ((die != 1) && ((die == 20) || ((i > 0) && (randint(i) > ac))))  /* normal hit */
    return TRUE;
  else
    return FALSE;
}
