/* Finds range of item in inventory list		-RAK-	*/
//int8u __fastcall__ find_range(int8u item1, int8u item2, int8u *j, int8u *k)
{
  register int8u i;
  register inven_type *i_ptr;
  int8u flag;


  i = 0;
  *j = 255;							// was -1, rewrote to avoid negative values
  *k = 255;
  flag = FALSE;

  vbxe_bank(VBXE_OBJBANK);
  i_ptr = &inventory[0];
  while (i < inven_ctr) {
    if (!flag) {
	  if ((i_ptr->tval == item1) || (i_ptr->tval == item2)) {
	    flag = TRUE;
	    *j = i;
	  }
	}
    else {
	  if ((i_ptr->tval != item1) && (i_ptr->tval != item2)) {
	    *k = i - 1;
	    break;
	  }
	}
    ++i;
    ++i_ptr;
  }

  if (flag && (*k == 255))
    *k = inven_ctr - 1;

  vbxe_restore_bank();
  return(flag);
}