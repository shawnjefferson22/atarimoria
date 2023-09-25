#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <string.h>
#include "interbank.h"
#include "returnbank.h"
#include "types.h"
#include "vbxetext.h"
#include "externs.h"



/* Banking Code, in Cart Bank 7E and copied to main ram */
#pragma code-name("BCODE")


void __fastcall__ bank_moria_init(void)
{
  INITBANK;
  moria_init();                                         // never returns, no need to bank back in
  return;
}


void __fastcall__ bank_vbxe_cputcxy(int8u x, int8u y, char c, int8u atr, int8u bank)
{
  PRINTBANK;
  vbxe_cputcxy(x, y, c, atr);

  return_bank(bank);
  return;
}


void __fastcall__ bank_vbxe_cputsxy(unsigned char x, unsigned char y, char *str, unsigned char atr, int8u bank)
{
  //strcpy(msg_print_str, str);
  PRINTBANK;
  vbxe_cputsxy(x, y, str, atr);

  return_bank(bank);
  return;
}


void __fastcall__ bank_vbxe_clear(int8u bank)
{
  PRINTBANK;
  vbxe_clear();

  return_bank(bank);
  return;
}


void __fastcall__ bank_vbxe_cleartoeol(unsigned char x, unsigned char y, int8u bank)
{
  PRINTBANK;
  vbxe_cleartoeol(x, y);

  return_bank(bank);
  return;
}


void __fastcall__ bank_vbxe_savescreen(int8u bank)
{
  PRINTBANK;
  vbxe_savescreen();

  return_bank(bank);
  return;
}


void __fastcall__ bank_vbxe_restorescreen(int8u bank)
{
  PRINTBANK;
  vbxe_restorescreen();

  return_bank(bank);
  return;
}


void __fastcall__ bank_clear_from(unsigned char y, int8u bank)
{
  PRINTBANK;
  vbxe_cleartobot(y);

  return_bank(bank);
  return;
}


void __fastcall__ bank_msg_print(char *str_buff, int8u bank)
{
  strcpy(msg_print_str, str_buff);						// need to copy the string to main memory

  PRINTBANK;
  msg_print(msg_print_str);

  return_bank(bank);
  return;
}

void __fastcall__ bank_count_msg_print(char *p, int8u bank)
{
  strcpy(msg_print_str, p);								// need to copy the string to main memory

  PRINTBANK;
  count_msg_print(msg_print_str);

  return_bank(bank);
  return;
}

void __fastcall__ bank_print(char ch, int8u row, int8u col, int8u bank)
{
  PRINTBANK;
  print(ch, row, col);

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt(char *str_buff, int8u row, int8u col, int8u bank)
{
  strcpy(msg_print_str, str_buff);						// need to copy the string to main memory, only 80 chars big enough?

  PRINTBANK;
  prt(msg_print_str, row, col);

  return_bank(bank);
  return;
}





