//void __fastcall__ rest_off(void)
{
  py.flags.rest = 0;
  py.flags.status &= ~PY_REST;
  bank_prt_state(INBANK_RETBANK);
  bank_msg_print("", INBANK_RETBANK);   // flush last message, or delete "press any key" message
  py.flags.food_digested++;
}