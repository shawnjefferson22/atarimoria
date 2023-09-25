/* Changes speed of monsters relative to player		-RAK-	 */
/* Note: When the player is sped up or slowed down, I simply */
/*	 change the speed of all the monsters.	This greatly	 */
/*	 simplified the logic.				                     */
//void __fastcall__ change_speed(int8 num)
{
  register int8u i;


  py.flags.speed += num;
  py.flags.status |= PY_SPEED;

  vbxe_bank(VBXE_MONBANK);						// monster list is in MONBANK
  for (i = mfptr - 1; i >= MIN_MONIX; --i)
      m_list[i].cspeed += num;
  vbxe_restore_bank();
}
