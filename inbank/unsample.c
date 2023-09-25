#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_object_offset


/*	Remove an automatically generated inscription.	-CJS- */
//void __fastcall__ unsample(inven_type *i_ptr)
{
  int16 offset;
  int8u indexx;


  /* used to clear ID_DAMD flag, but I think it should remain set */
  i_ptr->ident &= ~(ID_MAGIK|ID_EMPTY);
  if ((offset = INBANK_FUNCHELPER(i_ptr)) < 0)
    return;
  offset <<= 6;
  indexx = i_ptr->subval & (ITEM_SINGLE_STACK_MIN - 1);
  object_ident[offset + indexx] &= ~OD_TRIED;
}
