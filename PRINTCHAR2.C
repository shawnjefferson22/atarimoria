/* source/misc3.c: misc code for maintaining the dungeon, printing player info

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("PRINTBANK")
#pragma rodata-name("PRINTBANK")


/* Local prototypes, so we don't have to bank for these functions */
void __fastcall__ printchar2_put_buffer(char *out_str, int8 row, int8 col);
int8 __fastcall__ print_todis_adj(void);
int8u __fastcall__  print_stat_adj(int8u stat);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		PRINTNUM


void __fastcall__ printchar2_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

int8 __fastcall__ print_todis_adj(void)
#include "inbank\todis_adj.c"

int8u __fastcall__ print_stat_adj(int8u stat)
#include "inbank\stat_adj.c"


/* Returns a rating of x depending on y                 -JWT-   */
char * __fastcall__ likert(int x, int8u y)
{
  switch((signed char)(x/y))
    {
      case -3: case -2: case -1: return("Very Bad");
      case 0: case 1:            return("Bad");
      case 2:                    return("Poor");
      case 3: case 4:            return("Fair");
      case  5:                   return("Good");
      case 6:                    return("Very Good");
      case 7: case 8:            return("Excellent");
      default:                   return("Superb");
      }
}


/* Prints ratings on certain abilities                  -RAK-   */
void put_misc3(void)
{
  int8 xbth, xbthb, xfos, xsrh, xstl, xdis, xsave, xdev;
  vtype xinfra;
  register struct misc *p_ptr;


  vbxe_bank(VBXE_SPLBANK);

  bank_clear_from(14, PRINTNUM);
  p_ptr = &py.misc;
  xbth  = p_ptr->bth + p_ptr->ptohit*BTH_PLUS_ADJ + (class_level_adj[p_ptr->pclass][CLA_BTH] * p_ptr->lev);
  xbthb = p_ptr->bthb + p_ptr->ptohit*BTH_PLUS_ADJ + (class_level_adj[p_ptr->pclass][CLA_BTHB] * p_ptr->lev);
  /* this results in a range from 0 to 29 */
  xfos  = 40 - p_ptr->fos;
  if (xfos < 0)  xfos = 0;
    xsrh  = p_ptr->srh;
  /* this results in a range from 0 to 9 */
  xstl  = p_ptr->stl + 1;
  xdis  = p_ptr->disarm + 2*print_todis_adj() + print_stat_adj(A_INT) + (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
  xsave = p_ptr->save + print_stat_adj(A_WIS) + (class_level_adj[p_ptr->pclass][CLA_SAVE] * p_ptr->lev / 3);
  xdev  = p_ptr->save + print_stat_adj(A_INT) + (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3);

  sprintf(xinfra, "%d feet", py.flags.see_infra*10);

  printchar2_put_buffer("(Miscellaneous Abilities)", 15, 25);
  printchar2_put_buffer("Fighting    :", 16, 1);
  printchar2_put_buffer(likert (xbth, 12), 16, 15);
  printchar2_put_buffer("Bows/Throw  :", 17, 1);
  printchar2_put_buffer(likert (xbthb, 12), 17, 15);
  printchar2_put_buffer("Saving Throw:", 18, 1);
  printchar2_put_buffer(likert (xsave, 6), 18, 15);

  printchar2_put_buffer("Stealth     :", 16, 28);
  printchar2_put_buffer(likert (xstl, 1), 16, 42);
  printchar2_put_buffer("Disarming   :", 17, 28);
  printchar2_put_buffer(likert (xdis, 8), 17, 42);
  printchar2_put_buffer("Magic Device:", 18, 28);
  printchar2_put_buffer(likert (xdev, 6), 18, 42);

  printchar2_put_buffer("Perception  :", 16, 55);
  printchar2_put_buffer(likert (xfos, 3), 16, 69);
  printchar2_put_buffer("Searching   :", 17, 55);
  printchar2_put_buffer(likert (xsrh, 6), 17, 69);
  printchar2_put_buffer("Infra-Vision:", 18, 55);
  printchar2_put_buffer(xinfra, 18, 69);

  vbxe_restore_bank();
}


