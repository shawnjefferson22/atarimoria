/* source/tavern.c: tavern code, entering, command interpreter, buying, selling

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "vbxetext.h"


extern quest_type quests[QUEST_SLOTS];					// quests, only four allowed at one time (17x4=68 bytes)


#pragma code-name("QUEST2BANK")
#pragma rodata-name("QUEST2BANK")
#pragma data-name("QUEST2BANK")



/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ quest2_erase_line(int8u row, int8u col);
int8u __fastcall__ quest2_is_a_vowel(char ch);
void __fastcall__ quest2_invcopy(inven_type *to, int16u from_index);
void __fastcall__ quest2_item_name(int16u i, char *str);
void __fastcall__ quest2_pluralize(char *out_str, char *in_str);
void __fastcall__ eval_quest_item(void);
void __fastcall__ display_quests(void);
void __fastcall__ compact_quests(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		QUEST2NUM

#undef  INBANK_MOD
#define INBANK_MOD			quest2



void __fastcall__ quest2_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

int8u __fastcall__ quest2_is_a_vowel(char ch)
#include "inbank\is_a_vowel.c"

void __fastcall__ quest2_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"


/* Pluralizes a monster name */
void __fastcall__ quest2_pluralize(char *out_str, char *in_str)
{
  register int8u len;
  char *str;


  len = strlen(in_str)-1;					// get length, real end of string
  strcpy(out_str, in_str);					// copy the string as-is


  /* Special Cases */
  str = &out_str[len-5];
  if (strcmp(str, "Coins") == 0)
    return;

  if (strcmp(out_str, "Yeti") == 0)
    return;

  if (strcmp(out_str, "Swordman") == 0) {
    strcpy(out_str, "Swordmen");
    return;
  }

  /* Ends with "s" */
  if (out_str[len] == 's') {
    strcpy(out_str, in_str);
    strcat(out_str, "es");
    return;
  }

  /* Ends with "y" */
  if (out_str[len] == 'y') {
    out_str[len] = '\0';
    strcat(out_str, "ies");
    return;
  }

  /* Ends with "ch" */
  if (out_str[len-1] == 'c' && out_str[len] == 'h') {
    strcat(out_str, "es");
    return;
  }

  /* All the rest, just add an "s" */
  strcat(out_str, "s");
  return;
}


/* Extracts an item name and strips off all extraneous information. -SJ */
void __fastcall__ quest2_item_name(int16u i, char *str)
{
  register int8u j, l;
  inven_type item;


  quest2_invcopy(&item, i);
  item.ident = ID_STOREBOUGHT;
  bank_objdes(str, &item, TRUE, QUEST2NUM);

  l = strlen(str)-1;
  for (j = l; j > 0; --j)				// strip off extra stuff from name
    if (str[j] == '(' || str[j] == '[' || str[j] == '{') {
      str[j-1] = '\0';
      break;
	}

  if (str[l] == '.')				// strip off period
    str[l] = '\0';

  return;
}


/* Just killed a monster, is it a quest item? -SJ */
void __fastcall__ eval_quest_mon(int16u mon)
{
  register int8u i;


  /* Any monster quests? */
  for(i=0; i<QUEST_SLOTS; ++i) {
	if (quests[i].type == QUEST_NONE)
	  return;

	//if (quests[i].status != QST_INPROG)
	//  continue;

    switch(quests[i].type) {
	  case QUEST_MON:
	    if (quests[i].p1 == mon) {
		  quests[i].status = QST_COMPLETE;
		}
	    break;

	  case QUEST_MONS:
	    if (quests[i].p1 == mon)
		  quests[i].p2++;

	    if (quests[i].p2 > (py.misc.max_dlv * randint(10)+1)) {
	      quests[i].status = QST_COMPLETE;
	      quests[i].gold = quests[i].gold * quests[i].p2;	// gold per monster killed
	      quests[i].exp  = quests[i].exp * quests[i].p2;	// experience per monster killed
	    }

	    break;
    }
  }

  return;
}


/* Just reached a dungeon level, is it a quest item? -SJ */
void __fastcall__ eval_quest_level(int8u level)
{
  register int8u i;


  /* Any quests? */
  for(i=0; i<QUEST_SLOTS; ++i) {
	if (quests[i].type == QUEST_NONE)
	  return;

	if (quests[i].status != QST_INPROG)
	  continue;

    if (quests[i].type == QUEST_LEVEL) {
	  if (level >= quests[i].p1) {
		quests[i].status = QST_COMPLETE;
	  }
    }
  }

  return;
}


/* do we have the quest item? -SJ */
void __fastcall__ eval_quest_item(void)
{
  register int8u i, j;


  /* Any quests? */
  for(i=0; i<QUEST_SLOTS; ++i) {
	if (quests[i].type == QUEST_NONE)
	  return;

	if (quests[i].status != QST_INPROG)
	  continue;

	vbxe_bank(VBXE_OBJBANK);
    if (quests[i].type == QUEST_ITEM) {
	  for(j=0; j<INVEN_ARRAY_SIZE; ++j) {
		if ((inventory[j].index == quests[i].p1) && !(inventory[j].ident & ID_STOREBOUGHT))
		  quests[i].status = QST_COMPLETE;
	  }
    }
  }
  vbxe_restore_bank();

  return;
}


/* Display Quests and allow the player to give up on a quest. -SJ */
void __fastcall__ show_quests(void)
{
  int8u i;
  char c;
  int8u exit_flag;


  /* No quests? */
  if (quests[0].type == QUEST_NONE) {
    bank_prt("You have no quests.", 0, 0, QUEST2NUM);
    return;
  }

  /* Re-evaluate any item quests, since inventory changes */
  eval_quest_item();

  /* Main quest display loop */
  bank_vbxe_savescreen(QUEST2NUM);
  exit_flag = FALSE;
  do {
    display_quests();

    c = cgetc();
    switch(c) {
	  case 'D':
	  case 'd':
		bank_get_com("Drop Quest #?", &c, QUEST2NUM);
		if ((c > 48) && (c < 53)) {
		  i = c - 49;
		  quests[i].type = QUEST_NONE;
		  compact_quests();

	      if (quests[0].type == QUEST_NONE)
	        exit_flag = TRUE;
	      else {
	        bank_vbxe_restorescreen(QUEST2NUM);
		    display_quests();
	  	  }
		}
	    break;

	  case ESCAPE:
	    exit_flag = TRUE;
		break;
    }
  } while(!exit_flag);

  bank_vbxe_restorescreen(QUEST2NUM);
  return;
}


void __fastcall__ display_quests(void)
{
  register int8u i;
  int8u current_line;


  current_line = 1;

  for (i = 0; i<QUEST_SLOTS; ++i) { 				// Print the quests
    if (quests[i].type != QUEST_NONE) {
	  if (quests[i].status == QST_COMPLETE)
	    strcpy(out_val2, "COMPLETE");
	  else
	    strcpy(out_val2, "IN PROGRESS");

      sprintf(out_val1, "Quest %d: %s exp:%ld gold:%ld item:%d", i+1, out_val2, quests[i].exp, quests[i].gold, quests[i].item);  // FIXME
      bank_prt(out_val1, current_line, 0, QUEST2NUM);
	  ++current_line;

	  switch(quests[i].type) {
	    case QUEST_MON:
	      vbxe_bank(VBXE_MONBANK);
		  sprintf(out_val2, "%s %s", quest2_is_a_vowel(c_list[quests[i].p1].name[0]) ? "an" : "a", c_list[quests[i].p1].name);
	      sprintf(out_val1, "  Object: slay %s.", out_val2);
	      bank_prt(out_val1, current_line, 0, QUEST2NUM);
	      ++current_line;
	      break;

	    case QUEST_MONS:
	      vbxe_bank(VBXE_MONBANK);
		  strcpy(out_val1, c_list[quests[i].p1].name);						// using out_val1 temporarily here
		  quest2_pluralize(out_val2, out_val1);
	      sprintf(out_val1, "  Object: slay multiple %s.", out_val2);
	      bank_prt(out_val1, current_line, 0, QUEST2NUM);
	      ++current_line;

	      sprintf(out_val1, "  Number killed: %d", quests[i].p2);
	      bank_prt(out_val1, current_line, 0, QUEST2NUM);
	      ++current_line;
	      break;

	    case QUEST_ITEM:
	      quest2_item_name(quests[i].p1, out_val2);

	      sprintf(out_val1, "  Object: find %s.", out_val2);
		  bank_prt(out_val1, current_line, 0, QUEST2NUM);
	      ++current_line;
	      break;

	    case QUEST_LEVEL:
	      sprintf(out_val1, "  Object: reach %d feet in the mines.", (quests[i].p1 * 50));
		  bank_prt(out_val1, current_line, 0, QUEST2NUM);
	      ++current_line;
	      break;
   	  }

	  quest2_erase_line(current_line, 0);
   	  ++current_line;		// extra line between quest descriptions
	}
  }

  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", current_line-1, 0, QUEST2NUM);
  bank_prt("d/ESC", current_line-1, 74, QUEST2NUM);

  return;
}


void __fastcall__ compact_quests(void)
{
  int8u i;


  for(i=0; i<QUEST_SLOTS-1; ++i) {
    if (quests[i].type == QUEST_NONE) {
      memcpy(&quests[i], &quests[i+1], sizeof(quest_type));
      quests[i+1].type = QUEST_NONE;
	}
  }

  return;
}