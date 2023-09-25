/* Tests a spot for light or field mark status		-RAK-	*/
//int8u __fastcall__ test_light(int8u y, int8u x)
{
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);
  cave_ptr = CAVE_ADR(y, x);

  if ((cave_ptr->flags & CAVE_PL) || (cave_ptr->flags & CAVE_TL) || (cave_ptr->flags & CAVE_FM)) {
	vbxe_restore_bank();
    return(TRUE);
  }
  else {
	vbxe_restore_bank();
    return(FALSE);
  }
}
