/* Returns true if player has no light			-RAK-	*/
//int8u __fastcall__ no_light(void)
{
  register cave_type *c_ptr;
  int8u light;


  vbxe_bank(VBXE_CAVBANK);						// bank in cave, no need to restore bank
  c_ptr = CAVE_ADR(char_row, char_col);
  if (!(c_ptr->flags & CAVE_TL) && !(c_ptr->flags & CAVE_PL))
    light = TRUE;
  else
    light = FALSE;

  vbxe_restore_bank();
  return(light);
}
