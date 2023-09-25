/* Cure players confusion				-RAK-	*/
//int8u __fastcall__ cure_confusion(void)
{
  int8u cure = FALSE;


  if (py.flags.confused > 1) {
    py.flags.confused = 1;
    cure = TRUE;
  }
  return(cure);
}
