/* Dump IO to buffer -RAK-    */
/* Allows to choose color -SJ */
//void __fastcall__ put_buffer_color(char *out_str, int8u row, int8u col, int8u color)
{
  vtype tmp_str;


  // truncate the string, to make sure that it won't go past right edge of screen
  if (col > 79)
    col = 79;
  strncpy (tmp_str, out_str, 79 - col);
  tmp_str[79 - col] = '\0';

  bank_vbxe_cputsxy(col, row, tmp_str, color, INBANK_RETBANK);
}


