/* Lose a strength point. -RAK-	*/
//void __fastcall__ lose_str(void)
{
  if (!py.flags.sustain_str) {
    bank_dec_stat(A_STR, INBANK_RETBANK);
    bank_msg_print("You feel very sick.", INBANK_RETBANK);
  }
  else
    bank_msg_print("You feel sick for a moment, it passes.", INBANK_RETBANK);
}
