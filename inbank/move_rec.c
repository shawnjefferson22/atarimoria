/* Moves creature record from one space to another	-RAK-	*/
//void __fastcall__ move_rec(int8u y1, int8u x1, int8u y2, int8u x2)
{
  register cave_type *oldloc, *newloc;
  int8u tmp;


  vbxe_bank(VBXE_CAVBANK);

  /* this always works correctly, even if y1==y2 and x1==x2 */
  /*tmp = cave[y1][x1].cptr;
  cave[y1][x1].cptr = 0;
  cave[y2][x2].cptr = tmp;*/

  oldloc = CAVE_ADR(y1, x1);
  newloc = CAVE_ADR(y2, x2);

  tmp = oldloc->cptr;
  oldloc->cptr = 0;
  newloc->cptr = tmp;

  vbxe_restore_bank();
}
