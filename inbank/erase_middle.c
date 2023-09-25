/* Erase the interaction area -SJ*/
//void __fastcall__ erase_middle(void)
{
  int8u i;

  for (i=5; i<18; ++i)
    bank_vbxe_cleartoeol(0, i, INBANK_RETBANK);
    //erase_line(i, 0);
}