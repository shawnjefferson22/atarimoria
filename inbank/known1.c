#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_object_offset


/* Remove "Secret" symbol for identity of object			*/
//void __fastcall__ known1(inven_type *i_ptr)
{
  int16 offset;
  int8u indexx;


  if ((offset = INBANK_FUNCHELPER(i_ptr)) < 0)
    return;
  offset <<= 6;
  indexx = i_ptr->subval & (ITEM_SINGLE_STACK_MIN - 1);
  object_ident[offset + indexx] |= OD_KNOWN1;
  /* clear the tried flag, since it is now known */
  object_ident[offset + indexx] &= ~OD_TRIED;
}
