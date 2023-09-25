/* Pauses for user response before returning            -RAK-   */
//void __fastcall__ pause_line(int8u prt_line)
{
  bank_prt("[Press any key to continue.]", prt_line, 23, INBANK_RETBANK);
  cgetc();
  bank_vbxe_cleartoeol(0, prt_line, INBANK_RETBANK);		// eraseline code
  //erase_line(prt_line, 0);
}