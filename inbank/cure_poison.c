//* Cure poisoning					-RAK-	*/
//int8u __fastcall__ cure_poison(void)
{
  int8u cure = FALSE;


  if (py.flags.poisoned > 1) {
    py.flags.poisoned = 1;
    cure = TRUE;
  }
  return(cure);
}