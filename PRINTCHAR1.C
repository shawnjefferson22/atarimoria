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
#include "vbxetext.h"


#pragma code-name("CHPRINT1BANK")
#pragma rodata-name("CHPRINT1BANK")
#pragma data-name("CHPRINT1BANK")



/* Local prototypes in this cart bank */
void __fastcall__ printchar_put_buffer(char *out_str, int8 row, int8 col);
int8u __fastcall__ printchar_stat_adj(int8u stat);
int8u __fastcall__ printchar_spell_chance(int8u spell);
void printchar_clear_screen(void);
void __fastcall__ gain_level(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		CHPRINT1NUM

#undef	INBANK_MOD
#define INBANK_MOD			printchar


char *stat_names[] = { "STR : ", "INT : ", "WIS : ", "DEX : ", "CON : ", "CHR : " };

#define BLANK_LENGTH    24
char blank_string[] = "                        ";



/* Dump IO to buffer                                    -RAK-   */
void __fastcall__ printchar_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"


/* Converts stat num into string                        -RAK-   */
void __fastcall__ cnv_stat(int8u stat, char *out_val)
{
  register int8u part1, part2;


  if (stat > 18) {
    part1 = 18;
    part2 = stat - 18;
    if (part2 == 100)
	  strcpy(out_val, "18/100");
    else
	  sprintf(out_val, " %2d/%02d", part1, part2);
  }
  else
    sprintf(out_val, "%6d", stat);
}


/* Print character stat in given row, column            -RAK-   */
void __fastcall__ prt_stat(int8u stat)
{
  stat_type out_val1;


  cnv_stat(py.stats.use_stat[stat], out_val1);
  printchar_put_buffer(stat_names[stat], 6+stat, STAT_COLUMN);
  printchar_put_buffer(out_val1, 6+stat, STAT_COLUMN+6);
}


/* Print character info in given row, column            -RAK-   */
/* the longest title is 13 characters, so only pad to 13 */
void __fastcall__ prt_field(char *info, int8u row, int8u column)
{
  printchar_put_buffer(&blank_string[BLANK_LENGTH-13], row, column);
  printchar_put_buffer(info, row, column);
}


/* Print long number with header at given row, column */
void __fastcall__ prt_lnum(char *header, int32 num, int8u row, int8u column)
{
  vtype out_val;


  sprintf(out_val, "%s: %6ld", header, num);
  printchar_put_buffer(out_val, row, column);
}


/* Print number with header at given row, column        -RAK-   */
void __fastcall__ prt_num(char *header, int num, int8u row, int8u column)
{
  vtype out_val;


  sprintf(out_val, "%s: %6d", header, num);
  printchar_put_buffer(out_val, row, column);
}


/* Print long number at given row, column */
void __fastcall__ prt_long(int32 num, int8u row, int8u column)
{
  vtype out_val;


  sprintf(out_val, "%6ld", num);
  printchar_put_buffer(out_val, row, column);
}


/* Print number at given row, column    -RAK-   */
void __fastcall__ prt_int(int num, int8u row, int8u column)
{
  vtype out_val;


  sprintf(out_val, "%6d", num);
  printchar_put_buffer(out_val, row, column);
}


char* __fastcall__ title_string(void)
{
  register char *p;


  if (py.misc.lev < 1)
    p = "Babe in arms";
  else if (py.misc.lev <= MAX_PLAYER_LEVEL)
    p = player_title[py.misc.pclass][py.misc.lev-1];
  else if (py.misc.male)
    p = "**KING**";
  else
    p = "**QUEEN**";
  return p;
}


/* Prints title of character                            -RAK-   */
void prt_title(void)
{
  prt_field(title_string(), 4, STAT_COLUMN);
}


/* Prints level                                         -RAK-   */
void prt_level(void)
{
  prt_int((int)py.misc.lev, 13, STAT_COLUMN+6);
}


/* Prints players current mana points.           -RAK-  */
void prt_cmana(void)
{
  prt_int(py.misc.cmana, 15, STAT_COLUMN+6);
}


/* Prints Max hit points                                -RAK-   */
void prt_mhp(void)
{
  prt_int(py.misc.mhp, 16, STAT_COLUMN+6);
}


/* Prints players current hit points                    -RAK-   */
void prt_chp(void)
{
  prt_int(py.misc.chp, 17, STAT_COLUMN+6);
}


/* prints current AC                                    -RAK-   */
void prt_pac(void)
{
  prt_int(py.misc.dis_ac, 19, STAT_COLUMN+6);
}


/* Prints current gold                                  -RAK-   */
void prt_gold(void)
{
  prt_long(py.misc.au, 20, STAT_COLUMN+6);
}


/* Prints depth in stat area                            -RAK-   */
void prt_depth(void)
{
  vtype depths;
  int16u depth;


  depth = dun_level*50;
  if (depth == 0)
    strcpy(depths, "Town level");
  else
    sprintf(depths, "%d feet", depth);
  bank_prt(depths, 23, 65, CHPRINT1NUM);
}


/* Prints status of hunger                              -RAK-   */
void prt_hunger(void)
{
  if (PY_WEAK & py.flags.status)
    printchar_put_buffer("Weak  ", 23, 0);
  else if (PY_HUNGRY & py.flags.status)
    printchar_put_buffer("Hungry", 23, 0);
  else
    printchar_put_buffer(&blank_string[BLANK_LENGTH-6], 23, 0);
}


/* Prints Blind status                                  -RAK-   */
void prt_blind(void)
{
  if (PY_BLIND & py.flags.status)
    printchar_put_buffer("Blind", 23, 7);
  else
    printchar_put_buffer(&blank_string[BLANK_LENGTH-5], 23, 7);
}


/* Prints Confusion status                              -RAK-   */
void prt_confused(void)
{
  if (PY_CONFUSED & py.flags.status)
    printchar_put_buffer("Confused", 23, 13);
  else
    printchar_put_buffer(&blank_string[BLANK_LENGTH-8], 23, 13);
}


/* Prints Fear status                                   -RAK-   */
void prt_afraid(void)
{
  if (PY_FEAR & py.flags.status)
    printchar_put_buffer("Afraid", 23, 22);
  else
    printchar_put_buffer(&blank_string[BLANK_LENGTH-6], 23, 22);
}


/* Prints Poisoned status                               -RAK-   */
void prt_poisoned(void)
{
  if (PY_POISONED & py.flags.status)
    printchar_put_buffer("Poisoned", 23, 29);
  else
    printchar_put_buffer(&blank_string[BLANK_LENGTH-8], 23, 29);
}


/* Prints Searching, Resting, Paralysis, or 'count' status      -RAK-   */
void prt_state(void)
{
  char tmp[16];


  py.flags.status &= ~PY_REPEAT;
  if (py.flags.paralysis > 1)
    printchar_put_buffer ("Paralysed", 23, 38);
  else if (PY_REST & py.flags.status)
    {
      if (py.flags.rest < 0)
	(void) strcpy (tmp, "Rest *");
      else if (display_counts)
	(void) sprintf (tmp, "Rest %-5d", py.flags.rest);
      else
	(void) strcpy (tmp, "Rest");
      printchar_put_buffer (tmp, 23, 38);
    }
  else if (command_count > 0)
    {
      if (display_counts)
	(void) sprintf (tmp, "Repeat %-3d", command_count);
      else
	(void) strcpy (tmp, "Repeat");
      py.flags.status |= PY_REPEAT;
      printchar_put_buffer (tmp, 23, 38);
      if (PY_SEARCH & py.flags.status)
	printchar_put_buffer ("Search", 23, 38);
    }
  else if (PY_SEARCH & py.flags.status)
    printchar_put_buffer("Searching", 23, 38);
  else          /* "repeat 999" is 10 characters */
    printchar_put_buffer(&blank_string[BLANK_LENGTH-10], 23, 38);
}


/* Prints the speed of a character.                     -CJS- */
void prt_speed(void)
{
  register int8 i;

  i = py.flags.speed;
  if (PY_SEARCH & py.flags.status)   /* Search mode. */
    i--;
  if (i > 1)
    printchar_put_buffer ("Very Slow", 23, 49);
  else if (i == 1)
    printchar_put_buffer ("Slow     ", 23, 49);
  else if (i == 0)
    printchar_put_buffer (&blank_string[BLANK_LENGTH-9], 23, 49);
  else if (i == -1)
    printchar_put_buffer ("Fast     ", 23, 49);
  else
    printchar_put_buffer ("Very Fast", 23, 49);
}


/* Prints experience					-RAK-	*/
void __fastcall__ prt_experience(void)
{
  if (py.misc.exp > MAX_EXP)
    py.misc.exp = MAX_EXP;

  if (py.misc.lev < MAX_PLAYER_LEVEL) {
	vbxe_bank(VBXE_SPLBANK);
    while ((player_exp[py.misc.lev-1] * (int32)py.misc.expfact / 100) <= py.misc.exp)
      gain_level();
    vbxe_restore_bank();
  }

  if (py.misc.exp > py.misc.max_exp)
    py.misc.max_exp = py.misc.exp;

  prt_long(py.misc.exp, 14, STAT_COLUMN+6);
}


void prt_study(void)
{
  py.flags.status &= ~PY_STUDY;
  if (py.flags.new_spells == 0)
    printchar_put_buffer (&blank_string[BLANK_LENGTH-5], 23, 59);
  else
    printchar_put_buffer ("Study", 23, 59);
}


/* Prints winner status on display                      -RAK-   */
void prt_winner(void)
{
  if (noscore & 0x2) {
    if (wizard)
	  printchar_put_buffer("Is wizard  ", 22, 0);
    else
	  printchar_put_buffer("Was wizard ", 22, 0);
  }
  //else if (noscore & 0x1)
  //  printchar_put_buffer("Resurrected", 22, 0);
  //else if (noscore & 0x4)
  //  printchar_put_buffer ("Duplicate", 22, 0);
  else if (total_winner)
    printchar_put_buffer("*Winner*   ", 22, 0);
}


/* Prints character-screen info                         -RAK-   */
void prt_stat_block(void)
{
  register int32u status;
  register int8u i;


  vbxe_bank(VBXE_SPLBANK);									// need SPLBANK for race/class and titles

  prt_field(race[py.misc.prace].trace, 2, STAT_COLUMN);
  prt_field(class[py.misc.pclass].title, 3, STAT_COLUMN);
  prt_field(title_string(), 4, STAT_COLUMN);
  for (i = 0; i < 6; i++)
    prt_stat (i);
  prt_num ("LEV ", (int)py.misc.lev, 13, STAT_COLUMN);
  prt_lnum("EXP ", py.misc.exp, 14, STAT_COLUMN);
  prt_num ("MANA", py.misc.cmana, 15, STAT_COLUMN);
  prt_num ("MHP ", py.misc.mhp, 16, STAT_COLUMN);
  prt_num ("CHP ", py.misc.chp, 17, STAT_COLUMN);
  prt_num ("AC  ", py.misc.dis_ac, 19, STAT_COLUMN);
  prt_lnum("GOLD", py.misc.au, 20, STAT_COLUMN);
  prt_winner();
  status = py.flags.status;
  if ((PY_HUNGRY|PY_WEAK) & status)
    prt_hunger();
  if (PY_BLIND & status)
    prt_blind();
  if (PY_CONFUSED & status)
    prt_confused();
  if (PY_FEAR & status)
    prt_afraid();
  if (PY_POISONED & status)
    prt_poisoned();
  if ((PY_SEARCH|PY_REST) & status)
    prt_state ();
  /* if speed non zero, print it, modify speed if Searching */
  if (py.flags.speed - ((PY_SEARCH & status) >> 8) != 0)
    prt_speed ();
  /* display the study field */
  prt_study();

  vbxe_restore_bank();
}


/* Prints the following information on the screen.      -JWT-   */
void put_character(void)
{
  printchar_clear_screen();
  printchar_put_buffer ("Name        :", 2, 1);
  printchar_put_buffer ("Race        :", 3, 1);
  printchar_put_buffer ("Sex         :", 4, 1);
  printchar_put_buffer ("Class       :", 5, 1);
  if (character_generated) {
    vbxe_bank(VBXE_SPLBANK);

    printchar_put_buffer (py.misc.name, 2, 15);
    printchar_put_buffer (race[py.misc.prace].trace, 3, 15);
    printchar_put_buffer ((py.misc.male ? "Male" : "Female"), 4, 15);
    printchar_put_buffer (class[py.misc.pclass].title, 5, 15);

    vbxe_restore_bank();
  }
}


/* Prints the following information on the screen.      -JWT-   */
void put_stats(void)
{
  register int8u i;
  vtype buf;


  for (i = 0; i < 6; ++i)
    {
      cnv_stat(py.stats.use_stat[i], buf);
      printchar_put_buffer (stat_names[i], 2+i, 61);
      printchar_put_buffer (buf, 2+i, 66);
      if (py.stats.max_stat[i] > py.stats.cur_stat[i])
	  {
	    cnv_stat(py.stats.max_stat[i], buf);
	    printchar_put_buffer (buf, 2+i, 73);
	  }
    }
  prt_num("+ To Hit    ", py.misc.dis_th,  9, 1);
  prt_num("+ To Damage ", py.misc.dis_td, 10, 1);
  prt_num("+ To AC     ", py.misc.dis_tac, 11, 1);
  prt_num("  Total AC  ", py.misc.dis_ac, 12, 1);
}


/* Prints age, height, weight, and SC                   -JWT-   */
void put_misc1(void)
{
  prt_num("Age          ", (int)py.misc.age, 2, 38);
  prt_num("Height       ", (int)py.misc.ht, 3, 38);
  prt_num("Weight       ", (int)py.misc.wt, 4, 38);
  prt_num("Social Class ", (int)py.misc.sc, 5, 38);
}


/* Prints the following information on the screen.      -JWT-   */
void put_misc2(void)
{
  vbxe_bank(VBXE_SPLBANK);

  prt_num("Level      ", (int)py.misc.lev, 9, 29);
  prt_lnum("Experience ", py.misc.exp, 10, 29);
  prt_lnum("Max Exp    ", py.misc.max_exp, 11, 29);
  if (py.misc.lev == MAX_PLAYER_LEVEL)
    bank_prt("Exp to Adv.: ******", 12, 29, CHPRINT1NUM);
  else
    prt_lnum("Exp to Adv.", (int32)(player_exp[py.misc.lev-1] * py.misc.expfact / 100), 12, 29);
  prt_lnum("Gold       ", py.misc.au, 13, 29);
  prt_num("Max Hit Points ", py.misc.mhp, 9, 52);
  prt_num("Cur Hit Points ", py.misc.chp, 10, 52);
  prt_num("Max Mana       ", py.misc.mana, 11, 52);
  prt_num("Cur Mana       ", py.misc.cmana, 12, 52);

  vbxe_restore_bank();
}


/* Used to display the character on the screen.         -RAK-   */
void display_char(void)
{
  put_character();
  put_misc1();
  put_stats();
  put_misc2();
  bank_put_misc3();
}


/* Clears screen */
void printchar_clear_screen(void)
{
  if (msg_flag)
    bank_msg_print("", CHPRINT1NUM);

  bank_vbxe_clear(CHPRINT1NUM);
}


/* Increases hit points and level			-RAK-	*/
void __fastcall__ gain_level(void)
{
  int32 dif_exp, need_exp;
  //vtype out_val;


  py.misc.lev++;
  sprintf(out_val1, "Welcome to level %d.", py.misc.lev);
  bank_msg_print(out_val1, CHPRINT1NUM);
  bank_calc_hitpoints(CHPRINT1NUM);

  vbxe_bank(VBXE_SPLBANK);
  need_exp = player_exp[py.misc.lev-1] * py.misc.expfact / 100;
  if (py.misc.exp > need_exp) {
    /* lose some of the 'extra' exp when gain a level */
    dif_exp = py.misc.exp - need_exp;
    py.misc.exp = need_exp + (dif_exp / 2);
  }
  prt_level();
  prt_title();
  if (class[py.misc.pclass].spell == MAGE) {
    bank_calc_spells(A_INT, CHPRINT1NUM);
    bank_calc_mana(A_INT, CHPRINT1NUM);
  }
  else if (class[py.misc.pclass].spell == PRIEST) {
    bank_calc_spells(A_WIS, CHPRINT1NUM);
    bank_calc_mana(A_WIS, CHPRINT1NUM);
  }

  vbxe_restore_bank();
}


/* Adjustment for wisdom/intelligence				-JWT-	*/
int8u __fastcall__ printchar_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

/* Returns spell chance of failure for spell		-RAK-	*/
int8u __fastcall__ printchar_spell_chance(int8u spell)
#include "inbank\spell_chance.c"


/* Print list of spells					-RAK-	*/
/* if nonconsec is -1: spells numbered consecutively from 'a' to 'a'+num
                  >=0: spells numbered by offset from nonconsec */
void __fastcall__ print_spells(int8u *spell, int8u num, int8u comment, int8 nonconsec)
{
  register int8u i, j;
  register spell_type *s_ptr;
  int8u offset;
  char *p;
  char spell_char;


  vbxe_bank(VBXE_SPLBANK);

  offset = (class[py.misc.pclass].spell==MAGE ? SPELL_OFFSET : PRAYER_OFFSET);
  //erase_line(1, col);
  bank_vbxe_cleartoeol(0, 1, CHPRINT1NUM);		// eraseline code, in-line -SJ
  printchar_put_buffer("Name", 1, 5);
  printchar_put_buffer("Lv Mana Fail", 1, 35);

  /* only show the first 22 choices */
  if (num > 22)
    num = 22;

  for (i = 0; i < num; ++i) {
    j = spell[i];
    s_ptr = &magic_spell[py.misc.pclass-1][j];
    if (comment == FALSE)
	  p = "";
    else if ((spell_forgotten & (1L << j)) != 0)
	  p = " forgotten";
    else if ((spell_learned & (1L << j)) == 0)
	  p = " unknown";
    else if ((spell_worked & (1L << j)) == 0)
	  p = " untried";
    else
	  p = "";

    /* determine whether or not to leave holes in character choices, nonconsec -1 when learning
       spells, consec offset>=0 when asking which spell to cast */
    if (nonconsec == -1)
	spell_char = 'a' + i;
      else
	spell_char = 'a' + j - nonconsec;

    sprintf(out_val1, "  %c) %-30s%2d %4d %3d%%%s", spell_char, spell_names[j+offset], s_ptr->slevel, s_ptr->smana,
		    printchar_spell_chance(j), p);
    bank_prt(out_val1, 2+i, 0, CHPRINT1NUM);
  }
}

