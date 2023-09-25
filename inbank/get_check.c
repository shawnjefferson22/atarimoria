/* Used to verify a choice - user gets the chance to abort choice.  -CJS- */
//int8u __fastcall__ get_check(char *prompt)
{
  int8u res;
  int8u x;


  bank_prt(prompt, 0, 0, INBANK_RETBANK);

  x = strlen(prompt);                   // give us the string length of the prompt instead. -SJ
  if (x > 73) x = 73;
  bank_prt(" [y/n]", 0, x,  INBANK_RETBANK);

  do {
    res = cgetc();
  } while(res == ' ');

  //erase_line(0, 0);
  bank_vbxe_cleartoeol(0, 0, INBANK_RETBANK);

  if (res == 'Y' || res == 'y')
    return TRUE;
  else
    return FALSE;
}