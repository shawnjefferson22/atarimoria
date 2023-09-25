/* Cure players blindness				-RAK-	*/
//int8u __fastcall__ cure_blindness(void)
{
  int8u cure = FALSE;

  if (py.flags.blind > 1) {
    py.flags.blind = 1;
    cure = TRUE;
  }
  return(cure);
}
