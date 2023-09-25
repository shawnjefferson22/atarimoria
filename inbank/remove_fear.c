/* Cure the players fear				-RAK-	*/
//int8u __fastcall__ remove_fear(void)
{
  int8u result = FALSE;


  if (py.flags.afraid > 1) {
    py.flags.afraid = 1;
    result = TRUE;
  }

  return(result);
}
