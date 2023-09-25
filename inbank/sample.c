#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_object_offset


/* Somethings been sampled -CJS- */
//void __fastcall__ sample(inven_type *i_ptr)
{
  int16 offset;
  int8u indexx;

  if ((offset = INBANK_FUNCHELPER(i_ptr)) < 0)
    return;
  offset <<= 6;
  indexx = i_ptr->subval & (ITEM_SINGLE_STACK_MIN - 1);
  object_ident[offset + indexx] |= OD_TRIED;
}
