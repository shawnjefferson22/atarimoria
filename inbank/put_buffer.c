/* Dump IO to buffer                                    -RAK-   */
//void __fastcall__ put_buffer(char *out_str, int8u row, int8u col)
{
  vtype tmp_str;

  strcpy(tmp_str, out_str);
  bank_vbxe_cputsxy(col, row, tmp_str, 0x0F, INBANK_RETBANK);
}