/* copied this function into GEN1 bank, so don't need to do a bank switch to call it -SJ */
/* might be able to replace this with a memcpy and a few assignments. */
//void __fastcall__ invcopy(inven_type *to, int16u from_index)
{
  register treasure_type *from;


  vbxe_bank(VBXE_OBJBANK);                                      // bank in the object list to vbxe memory

  from = &object_list[from_index];
  to->index     = from_index;
  to->name2     = SN_NULL;
  to->inscrip[0] = '\0';
  to->flags     = from->flags;
  to->tval      = from->tval;
  to->tchar     = from->tchar;
  to->p1        = from->p1;
  to->cost      = from->cost;
  to->subval    = from->subval;
  to->number    = from->number;
  to->weight    = from->weight;
  to->tohit     = from->tohit;
  to->todam     = from->todam;
  to->ac        = from->ac;
  to->toac      = from->toac;
  to->damage[0] = from->damage[0];
  to->damage[1] = from->damage[1];
  to->level     = from->level;
  to->ident     = 0;

  vbxe_restore_bank();                                          // restore previous vbxe bank
}
