#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_unsample


/* Remove "Secret" symbol for identity of plusses			*/
//void __fastcall__ known2(inven_type *i_ptr)
{
  INBANK_FUNCHELPER(i_ptr);
  i_ptr->ident |= ID_KNOWN2;
}
