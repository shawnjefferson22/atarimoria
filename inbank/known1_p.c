#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_object_offset


//int8u __fastcall__ known1_p(inven_type *i_ptr)
{
  int16 offset;
  int8u indexx;


  /* Items which don't have a 'color' are always known1, so that they can
     be carried in order in the inventory.  */
  if ((offset = INBANK_FUNCHELPER(i_ptr)) < 0)
    return OD_KNOWN1;
  //if (store_bought_p(i_ptr))
  if(i_ptr->ident & ID_STOREBOUGHT)                                                             // removed call to store_bought_p
    return OD_KNOWN1;
  offset <<= 6;
  indexx = i_ptr->subval & (ITEM_SINGLE_STACK_MIN - 1);
  return(object_ident[offset + indexx] & OD_KNOWN1);											// object ident in BSS in main ram
}