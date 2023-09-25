/* Slow Poison	-RAK-	*/
//int8u __fastcall__ slow_poison(void)
{
  int8u slow = FALSE;


  if (py.flags.poisoned > 0) {
    py.flags.poisoned = py.flags.poisoned / 2;
    if (py.flags.poisoned < 1)
      py.flags.poisoned = 1;
    slow = TRUE;
    bank_msg_print("The effect of the poison has been reduced.", INBANK_RETBANK);
  }

  return(slow);
}
