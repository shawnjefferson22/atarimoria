/* Prompts (optional) and returns ord value of input char       */
/* Function returns false if <ESCAPE> is input  */
//int8u __fastcall__ get_com(char *prompt, char *command)
{
  int8u res;


  if (prompt)
    bank_prt(prompt, 0, 0, INBANK_RETBANK);

  *command = cgetc();
  if (*command == ESCAPE)
    res = FALSE;
  else
    res = TRUE;

  //dungeon1_erase_line(MSG_LINE, 0);
  bank_vbxe_cleartoeol(0, MSG_LINE, INBANK_RETBANK);
  return(res);
}