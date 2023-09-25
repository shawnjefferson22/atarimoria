/* source/death.c: code executed when player dies

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and diout_val1ibuted for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* Must read this before externs.h, as some global declarations use FILE. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("DEATH1BANK")
#pragma rodata-name("DEATH1BANK")
#pragma data-name("DEATH1BANK")


/* Prototypes of functions copied into this bank */
void __fastcall__ death1_put_buffer(char *out_str, int8u row, int8u col);
void __fastcall__ death1_erase_line(int8u row, int8u col);
void __fastcall__ death1_pause_line(int8u prt_line);
char* __fastcall__ death1_title_string(void);
int8 __fastcall__ death1_object_offset(inven_type *t_ptr);
void __fastcall__ death1_known1(inven_type *i_ptr);
void __fastcall__ death1_known2(inven_type *i_ptr);
void __fastcall__ death1_unsample(inven_type *i_ptr);
void __fastcall__ death1_restore_level(void);
void __fastcall__ death1_show_inven(int8u r1, int8u r2);
void __fastcall__ display_scores(void);
void __fastcall__ highscores(void);
void __fastcall__ end_game(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DEATH1NUM

#undef  INBANK_MOD
#define INBANK_MOD			death1


void __fastcall__ death1_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

/* Dump IO to buffer                                    -RAK-   */
void __fastcall__ death1_put_buffer(char *out_str, int8u row, int8u col)
#include "inbank\put_buffer.c"

/* Pauses for user response before returning            -RAK-   */
void __fastcall__ death1_pause_line(int8u prt_line)
#include "inbank\pause_line.c"


char* __fastcall__ death1_title_string(void)
{
  register char *p;


  vbxe_bank(VBXE_SPLBANK);
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


int8 __fastcall__ death1_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

void __fastcall__ death1_known1(inven_type *i_ptr)
#include "inbank\known1.c"

void __fastcall__ death1_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ death1_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"


/* Centers a out_string within a 31 character out_string		-JWT-	 */
char* __fastcall__ center_out_string(char *centered_out_val1, char *in_out_val1)
{
  register int8u i, j;


  i = strlen(in_out_val1);
  j = 15 - i/2;
  sprintf(centered_out_val1, "%*s%s%*s", j, "", in_out_val1, 31 - i - j, "");
  return centered_out_val1;
}


/* Restores any drained experience			-RAK-	*/
/* Since we copied this function here, it's pared down to only what's needed in death1.c -SJ */
void __fastcall__ death1_restore_level(void)
{
  //int8u restore;
  //register struct misc *m_ptr;


  //restore = FALSE;
  //m_ptr = &py.misc;
  if (py.misc.max_exp > py.misc.exp) {
    //restore = TRUE;
    //msg_print("You feel your life energies returning.");
    /* this while loop is not redundant, ptr_exp may reduce the exp level */
    //while (py.misc.exp < py.misc.max_exp) {
	py.misc.exp = py.misc.max_exp;
	  //bank_prt_experience(DEATH1NUM);
	//}
  }
  //return(restore);
  return;
}


void __fastcall__ display_scores(void)
{
  char input;
  int8u i, y;
  int8u r, c;
  char race_str[11];
  char class_str[20];


  input = 0;
  bank_vbxe_clear(DEATH1NUM);
  bank_prt("Rank  Points Name              Sex Race       Class  Lvl Killed By", 0, 0, DEATH1NUM);
  bank_prt("[Press any key to continue.]", 23, 23, DEATH1NUM);

  vbxe_bank(VBXE_GENBANK);
  y = 2;
  for(i=0; i<score_entries_ram; ++i) {
    vbxe_bank(VBXE_GENBANK);						// get race/class from highscore list
    r = scores_ram[i].race;
    c = scores_ram[i].class;

    vbxe_bank(VBXE_SPLBANK);						// get race and title strings
    strcpy(race_str, race[r].trace);
    strcpy(class_str, class[c].title);

    vbxe_bank(VBXE_GENBANK);

    /* Put twenty scores on each page, on lines 2 through 21. */
	sprintf(out_val1, "%-4d%8ld %-19.19s %c %-10.10s %-7.7s%3d %-22.22s", i+1, scores_ram[i].points, scores_ram[i].name,
	        scores_ram[i].sex, race_str, class_str, scores_ram[i].lev, scores_ram[i].died_from);
	bank_prt(out_val1, y, 0, DEATH1NUM);

    ++y;
    if ((y > 21) && (i <= score_entries_ram)) {
      input = cgetc();
      if (input == ESCAPE)
	    break;

      y = 2;
      bank_vbxe_clear(DEATH1NUM);
      bank_prt("Rank  Points Name              Sex Race       Class  Lvl Killed By", 0, 0, DEATH1NUM);
      bank_prt("[Press any key to continue.]", 23, 23, DEATH1NUM);
    }
  }

  if (input != ESCAPE)
    input = cgetc();
}


/* Prints the gravestone of the character		-RAK-	 */
void __fastcall__ print_tomb(void)
{
  //vtype out_val1, out_val2;
  register int8u i;
  //char day[11];
  register char *p;


  //clear_screen();
  bank_vbxe_clear(DEATH1NUM);
  death1_put_buffer("_______________________", 1, 15);
  death1_put_buffer("/", 2, 14);
  death1_put_buffer("\\         ___", 2, 38);
  death1_put_buffer("/", 3, 13);
  death1_put_buffer("\\ ___   /   \\      ___", 3, 39);
  death1_put_buffer("/            RIP            \\   \\  :   :     /   \\", 4, 12);
  death1_put_buffer("/", 5, 11);
  death1_put_buffer("\\  : _;,,,;_    :   :", 5, 41);
  sprintf(out_val1, "/%s\\,;_          _;,,,;_", center_out_string(out_val2, py.misc.name));
  death1_put_buffer(out_val1, 6, 10);
  death1_put_buffer("|               the               |   ___", 7, 9);
  if (!total_winner)
    p = death1_title_string();
  else
    p = "Magnificent";
  sprintf(out_val1, "| %s |  /   \\", center_out_string(out_val2, p));
  death1_put_buffer(out_val1, 8, 9);
  death1_put_buffer("|", 9, 9);
  death1_put_buffer("|  :   :", 9, 43);
  if (!total_winner) {
	vbxe_bank(VBXE_SPLBANK);
    p = class[py.misc.pclass].title;
  }
  else if (py.misc.male)
    p = "*King*";
  else
    p = "*Queen*";
  sprintf(out_val1,"| %s | _;,,,;_   ____", center_out_string(out_val2, p));
  death1_put_buffer(out_val1, 10, 9);
  sprintf(out_val1, "Level : %d", (int) py.misc.lev);
  sprintf(out_val1,"| %s |          /    \\", center_out_string(out_val2, out_val1));
  death1_put_buffer(out_val1, 11, 9);
  sprintf(out_val1, "%ld Exp", py.misc.exp);
  sprintf(out_val1,"| %s |          :    :", center_out_string(out_val2, out_val1));
  death1_put_buffer(out_val1, 12, 9);
  sprintf(out_val1, "%ld Au", py.misc.au);
  sprintf(out_val1,"| %s |          :    :", center_out_string(out_val2, out_val1));
  death1_put_buffer(out_val1, 13, 9);
  sprintf(out_val1, "Died on Level : %d", dun_level);
  sprintf(out_val1,"| %s |         _;,,,,;_", center_out_string(out_val2, out_val1));
  death1_put_buffer(out_val1, 14, 9);
  death1_put_buffer("|            killed by            |", 15, 9);
  p = died_from;
  i = strlen(p);
  p[i] = '.';  /* add a trailing period */
  p[i+1] = '\0';
  sprintf(out_val1, "| %s |", center_out_string(out_val2, p));
  death1_put_buffer(out_val1, 16, 9);
  p[i] = '\0';	 /* out_val1ip off the period */
  //date(day);
  sprintf(out_val1, "| %s |", center_out_string(out_val2, " "));		// FIXME?
  death1_put_buffer(out_val1, 17, 9);
  death1_put_buffer("*|   *     *     *    *   *     *  | *", 18, 8);
  death1_put_buffer("________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______", 19, 0);

  //flush();
  while(kbhit()) cgetc();			// inline flush code -SJ

  death1_put_buffer("(ESC to abort, any other key to print character record)", 23, 0);
  if (cgetc() != ESCAPE) {
    vbxe_bank(VBXE_OBJBANK);
    for (i = 0; i < INVEN_ARRAY_SIZE; ++i) {
	  death1_known1(&inventory[i]);
	  death1_known2(&inventory[i]);
	}
    bank_calc_bonuses(DEATH1NUM);

	//clear_screen();
	bank_vbxe_clear(DEATH1NUM);
	bank_display_char(DEATH1NUM);
	death1_put_buffer("Type ESC to skip the inventory:", 23, 0);
	if (cgetc() != ESCAPE) {
	  bank_vbxe_clear(DEATH1NUM);
	  bank_msg_print("You are using:", DEATH1NUM);
	  bank_show_equip(TRUE, DEATH1NUM);
	  bank_msg_print("", DEATH1NUM);
	  bank_msg_print("You are carrying:", DEATH1NUM);
	  bank_clear_from(1, DEATH1NUM);
	  death1_show_inven(0, inven_ctr-2);
	  bank_msg_print("", DEATH1NUM);
	}
  }
}


/* Calculates the total number of points earned		-JWT-	 */
int32 __fastcall__ total_points(void)
{
  int32 total;
  int8u i;


  total = py.misc.max_exp + (100 * py.misc.max_dlv);
  total += py.misc.au / 100;
  vbxe_bank(VBXE_OBJBANK);
  for (i = 0; i < INVEN_ARRAY_SIZE; ++i)
    total += bank_item_value(&inventory[i], DEATH1NUM);
  total += dun_level*50;

  /* Don't ever let the score decrease from one save to the next.  */
  if (max_score > total)
    return max_score;

  return total;
}


/* Change the player into a King!			-RAK-	 */
void kingly(void)
{
  register char *p;


  /* Change the character attributes.		 */
  dun_level = 0;
  strcpy(died_from, "Ripe Old Age");
  death1_restore_level();
  py.misc.lev += MAX_PLAYER_LEVEL;
  py.misc.au += 250000L;
  py.misc.max_exp += 5000000L;
  py.misc.exp = py.misc.max_exp;

  /* Let the player know that he did good.	 */
  //clear_screen();
  bank_vbxe_clear(DEATH1NUM);
  death1_put_buffer("#", 1, 34);
  death1_put_buffer("#####", 2, 32);
  death1_put_buffer("#", 3, 34);
  death1_put_buffer(",,,  $$$  ,,,", 4, 28);
  death1_put_buffer(",,=$   \"$$$$$\"   $=,,", 5, 24);
  death1_put_buffer(",$$        $$$        $$,", 6, 22);
  death1_put_buffer("*>         <*>         <*", 7, 22);
  death1_put_buffer("$$         $$$         $$", 8, 22);
  death1_put_buffer("\"$$        $$$        $$\"", 9, 22);
  death1_put_buffer("\"$$       $$$       $$\"", 10, 23);
  p = "*#########*#########*";
  death1_put_buffer(p, 11, 24);
  death1_put_buffer(p, 12, 24);
  death1_put_buffer("Veni, Vidi, Vici!", 15, 26);
  death1_put_buffer("I came, I saw, I conquered!", 16, 21);
  if (py.misc.male)
    death1_put_buffer("All Hail the Mighty King!", 17, 22);
  else
    death1_put_buffer("All Hail the Mighty Queen!", 17, 22);

  //flush();
  while(kbhit()) cgetc();			// flush code inline -SJ
  death1_pause_line(23);
}


/* Handles the gravestone end top-twenty routines	-RAK-	 */
void exit_game(void)
{
  /* What happens upon dying. -RAK-	*/
  bank_msg_print("", DEATH1NUM);
  while(kbhit()) cgetc();			// flush code inline -SJ
  /* If the game has been saved, then save sets turn back to -1, which inhibits the printing of the tomb. */
  if ((turn >= 0) && (!character_saved)) {
    if (total_winner)
	  kingly();
    print_tomb();
  }

  //if (character_generated && !character_saved)
  //  save_char ();							// FIXME: need to save? Will let player just turn off bad game -SJ

  write_recall_to_cart();					// save monster recall to cart

  if (!noscore) {
    highscores();
    display_scores();
  }

  //death1_erase_line(23, 0);

  bank_vbxe_clear(DEATH1NUM);				// clear the screen
  end_game();								// end game credits

  while(1);									// loop forever
}


/* Displays inventory items from r1 to r2	-RAK-	*/
/* Designed to keep the display as far to the right as possible.  The  -CJS-
   parameter col gives a column at which to start, but if the display does
   not fit, it may be moved left.  The return value is the left edge used. */
/* If mask is non-zero, then only display those items which have a non-zero
   entry in the mask array.  */
/* Re-write of this function to just assume screen is cleared already -SJ */
/* Removed mask and weight parameter, for death1.c -SJ */
void __fastcall__ death1_show_inven(int8u r1, int8u r2)
{
  register int8u i;
  int8u total_weight, current_line;


  current_line = 1;
  vbxe_bank(VBXE_OBJBANK);

  for (i = r1; i <= r2; ++i) { 				// Print the items
    bank_objdes(out_val1, &inventory[i], TRUE, DEATH1NUM);
    sprintf(out_val2, " %c) %s  ", 'a'+i, out_val1);
    bank_prt(out_val2, current_line, 0, DEATH1NUM);

	vbxe_bank(VBXE_OBJBANK);				// if objdes destroys VBXE bank CHECK
    total_weight = inventory[i].weight*inventory[i].number;
    sprintf(out_val1, "%3d.%d lb", (total_weight) / 10, (total_weight) % 10);
    bank_prt(out_val1, current_line, 71, DEATH1NUM);

    ++current_line;
  }

  return;
}


void __fastcall__ highscores(void)
{
  int8u i;
  int32 points;
  char *tmp;
  high_scores entry;


  points = total_points();					// calculate player points

  vbxe_bank(VBXE_GENBANK);					// bank GENRAM bank in

  /* is the highscore list full already? */
  if (score_entries_ram == SCOREFILE_SIZE)
    score_entries_ram = 99;

  /* add entry to end of list */
  scores_ram[score_entries_ram].points = points;
  scores_ram[score_entries_ram].mhp = py.misc.mhp;
  scores_ram[score_entries_ram].chp = py.misc.chp;
  scores_ram[score_entries_ram].dun_level = dun_level;
  scores_ram[score_entries_ram].lev = py.misc.lev;
  scores_ram[score_entries_ram].max_dlv = py.misc.max_dlv;
  scores_ram[score_entries_ram].sex = (py.misc.male ? 'M' : 'F');
  scores_ram[score_entries_ram].race = py.misc.prace;
  scores_ram[score_entries_ram].class = py.misc.pclass;
  strcpy(scores_ram[score_entries_ram].name, py.misc.name);

  tmp = died_from;
  if ('a' == *tmp) {
    if ('n' == *(++tmp)) {
	  ++tmp;
	}
    while (' ' == *tmp) {			// isspace -SJ
	  ++tmp;
	}
  }
  strcpy(scores_ram[score_entries_ram].died_from, tmp);

  /* increment number of entries */
  ++score_entries_ram;

  /* Sort the new entry */
  i = score_entries_ram - 1;
  while(i >= 1) {
    if (scores_ram[i].points > scores_ram[i-1].points) {
      memcpy(&entry, &scores_ram[i-1], sizeof(high_scores));
      memcpy(&scores_ram[i-1], &scores_ram[i], sizeof(high_scores));
	  memcpy(&scores_ram[i], &entry, sizeof(high_scores));
	}
	else
	  break;

	--i;
  }

  /* Don't write to highscore table if just saved */
  if (strcmp(died_from, "(saved)") != 0)
    write_highscores_to_cart();
  return;
}


/* End Game credits and text -SJ */
void __fastcall__ end_game(void)
{
  death1_put_buffer("*********************", 3, 29);
  death1_put_buffer("**   Umoria 5.5    **", 4, 29);
  death1_put_buffer("*********************", 5, 29);
  death1_put_buffer("Copyright (c) 1985 Robert Alan Koeneke", 7, 21);
  death1_put_buffer("Copyright (c) 1989-94 James E. Wilson", 8, 21);
  death1_put_buffer("Programmers : Robert Alan Koeneke / University of Oklahoma", 10, 11);
  death1_put_buffer("              Jimmey Wayne Todd   / University of Oklahoma", 11, 11);
  death1_put_buffer("UNIX Port   : James E. Wilson     / Cygnus Support", 12, 11);
  death1_put_buffer("Version 5.5 : David J. Grabiner   / Harvard University", 13, 11);
  death1_put_buffer("Atari 8-bit : Shawn Jefferson     / AtariAge", 14, 11);

  death1_put_buffer("Thanks for playing Atari Moria!",  20, 25);
  death1_put_buffer("(you can safely turn off your computer now)", 21, 19);
}



