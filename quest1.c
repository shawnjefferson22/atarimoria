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



#pragma bss-name("BSS")

quest_type quests[QUEST_SLOTS];					// quests, only four allowed at one time (17x4=68 bytes)


#pragma code-name("QUEST1BANK")
#pragma rodata-name("QUEST1BANK")
#pragma data-name("QUEST1BANK")


const char *quest_source[QUEST_TYPES][QUEST_SOURCES] = {
	{"A familiar Dwarf", "A non-descript Human", "A pungent Halfling", "A sly Gnome", "A slim Elf", "A seedy Human", "A scruffy Human" },  	// QUEST_MON
	{"A rotund Dwarf", "A beautiful Elf", "A non-descript Human", "A slim Elf", "A wise Mage", "A scruffy Ranger", "A pungent Halfling" },  // QUEST_MONS
	{"A pompous Elf", "A non-descript Human", "A seedy Human", "A drunk Priest", "A nervous Rogue", "A sly Gnome", "A slim Elf" },			// QUEST_ITEM
	{"A wise Mage", "A mysterious Mage", "A severe Mage", "A drunk Priest", "A happy Priest", "A non-descript Human", "A scruffy Ranger"}	// QUEST_LEVEL
};

const char *quest_story[QUEST_TYPES][QUEST_STORIES] = {
	{"My father was slain in the mines by %s. I would like someone to hunt it down.", "I barely survived the mines myself, %s almost killed me! I would pay to have it slain!"},
	{"The mines are overrun by the %s, we'll pay you to kill as many as possible.", "The %s needs to be exterminated! They killed my entire family! I'll pay for each one slain."},
	{"My employeer wants %s and will pay whoever finds it for him.", "I was attacked and lost %s in the mines. I'm sure it can be found and I'll pay!"},
	{"My benefactor needs a map of the mines at %d feet and will pay handsomely!", "My master would like to know what's in the mines at %d feet and will pay greatly for the knowledge."}
};

const char *quest_done[QUEST_DONES] = {
	{"Excellent! I see you've completed the task I set you. "},
	{"Well, I didn't expect to see you again! Completed the quest I set you on too! "},
	{"The mines have left you somewhat worse for wear, but I see the task is complete. "},
	{"Took your time with the task we asked of you. We thought you'd given up. "}
};


/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ quest1_erase_line(int8u row, int8u col);
void __fastcall__ quest1_erase_middle(void);
int8u __fastcall__ quest1_is_a_vowel(char ch);
void __fastcall__ quest1_item_name(int16u i, char *str);
int16u __fastcall__ quest1_get_item(int8u level);
int8u __fastcall__ quest1_popt(void);
void __fastcall__ quest1_pusht(int8u x);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		QUEST1NUM

#undef  INBANK_MOD
#define INBANK_MOD			quest1



void __fastcall__ quest1_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

int8u __fastcall__ quest1_is_a_vowel(char ch)
#include "inbank\is_a_vowel.c"

void __fastcall__ quest1_erase_middle(void)
#include "inbank\erase_middle.c"

void __fastcall__ quest1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ quest1_popt(void)
#include "inbank\popt.c"

void __fastcall__ quest1_pusht(int8u x)
#include "inbank\pusht.c"


/* Extracts an item name and strips off all extraneous information. -SJ */
void __fastcall__ quest1_item_name(int16u i, char *str)
{
  register int8u j, l;
  inven_type item;


  quest1_invcopy(&item, i);
  item.ident = ID_STOREBOUGHT;
  bank_objdes(str, &item, TRUE, QUEST1NUM);

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


/* Time to go on a quest! -SJ */
int8u __fastcall__ quest_start(int8u chance)
{
  register int8u i;
  char c;


  /* Already got gossip today? (use the same flag for quests too) */
  if (tavern_got_gossip && (randint(20) > 2))			// small chance to hear gossip again
    return(FALSE);

  /* Already have enough quests? */
  for(i=0; i<QUEST_SLOTS; ++i) {
    if (quests[i].type == QUEST_NONE)
      break;
  }
  if (i == QUEST_SLOTS)				// no quest slots active
    return(FALSE);

  /* Always a slim chance */
  if (chance == 0)
    chance = 1;						// at least a small chance!

  /* Generate a quest? */
  if (0) {//chance < randint(100)) {		// no quest			// FIXME: testing, generate quests immediately
    return(FALSE);
  }
  else {							// here's a quest!
    quest1_erase_line(0, 0);		// erase top line
    quest1_erase_middle();			// and erase middle

	/* setup quest structure */
    quests[i].type = randint(QUEST_TYPES);
    quests[i].source = randint(QUEST_SOURCES)-1;
    quests[i].p1 = 0;
    quests[i].p2 = 0;
    quests[i].exp = (py.misc.lev * QUEST_EXPMULT);
    quests[i].gold = 0;
	quests[i].item = OBJ_NOTHING;

	/* Item reward? No item reward for item quests */
    if ((quests[i].type != QUEST_ITEM) && (randint(100) < QUEST_ITEM_CH))
	  quests[i].item = quest1_get_item(py.misc.max_dlv);

    strcpy(out_val1, quest_source[quests[i].type-1][quests[i].source]);
 	strcat(out_val1, " sits next to you and says:");
	bank_prt(out_val1, 5, 2, QUEST1NUM);

    /* which quest? */
    switch(quests[i].type) {
      case QUEST_MON:
		quests[i].p1 = bank_get_mons_num(py.misc.max_dlv+randint(QUEST_LEVADD), QUEST1NUM);
		quests[i].gold = (int32u) (py.misc.lev * (QUEST_AUMON * randint(3)));

		vbxe_bank(VBXE_MONBANK);
		sprintf(out_val1, "%s %s", quest1_is_a_vowel(c_list[quests[i].p1].name[0]) ? "an" : "a", c_list[quests[i].p1].name);

		vbxe_bank(VBXE_GENBANK);
		sprintf(text, quest_story[QUEST_MON-1][randint(QUEST_STORIES)-1], out_val1);
		bank_pretty_print(text, 7, QUEST1NUM);
        vbxe_restore_bank();
        break;

      case QUEST_MONS:
		quests[i].p1 = bank_get_mons_num(py.misc.max_dlv+randint(QUEST_LEVADD), QUEST1NUM);
		quests[i].gold = (int32u) (py.misc.lev * (QUEST_AUPERMON * randint(3)));

		vbxe_bank(VBXE_MONBANK);
		strcpy(out_val1, c_list[quests[i].p1].name);

		vbxe_bank(VBXE_GENBANK);
		sprintf(text, quest_story[QUEST_MONS-1][randint(QUEST_STORIES)-1], out_val1);
		bank_pretty_print(text, 7, QUEST1NUM);
        vbxe_restore_bank();
        break;

      case QUEST_ITEM:
        quests[i].p1 = quest1_get_item(py.misc.max_dlv+randint(QUEST_LEVADD));
        quests[i].gold = (int32u) (py.misc.lev * (QUEST_AUITEM * randint(3)));

	  	quest1_item_name(quests[i].p1, out_val1);

		vbxe_bank(VBXE_GENBANK);
		sprintf(text, quest_story[QUEST_ITEM-1][randint(QUEST_STORIES)-1], out_val1);
		bank_pretty_print(text, 7, QUEST1NUM);
        vbxe_restore_bank();
        break;

      case QUEST_LEVEL:
        quests[i].p1 = py.misc.max_dlv+randint(QUEST_LEVADD+2);
        quests[i].gold = (int32u) ((QUEST_AULEVEL * randint(2)) * quests[i].p1);

        vbxe_bank(VBXE_GENBANK);
		sprintf(text, quest_story[QUEST_LEVEL-1][randint(QUEST_STORIES)-1], (quests[i].p1 * 50));
		bank_pretty_print(text, 7, QUEST1NUM);
        vbxe_restore_bank();
        break;
	}

	/* Will the player accept the quest? */
    bank_prt("Do you accept this quest? [y/n]", 10, 2, QUEST1NUM);
    c = cgetc();
    if (c == 'y' || c == 'Y') {
	  quests[i].status = QST_INPROG;									// quest is in progress!
	  quest1_erase_line(10, 2);
	  bank_prt("Find me here when you've completed your quest.", 10, 2, QUEST1NUM);

	  //tavern_got_gossip = 1;											// no more quests or gossip today  FIXME
    }
  	else {
	  py.misc.frac_chr = 0;												// clear fractional charisma (more of a penalty?)
	  quests[i].type = QUEST_NONE;
	  quest1_erase_line(10, 2);
	  bank_prt("I guess I'll find someone braver to complete this quest.", 10, 2, QUEST1NUM);
  	}
  }

  bank_prt("press any key", 12, 33, QUEST1NUM);
  cgetc();

  quest1_erase_middle();
  return(TRUE);
}


/* End any quests? */
void __fastcall__ quest_end(void)
{
  register int8u i, j;


  /* Quickly re-evaluate item quests */
  bank_eval_quest_item(QUEST1NUM);

  /* Any quests? */
  for(i=0; i<QUEST_SLOTS; ++i) {
	if (quests[i].type == QUEST_NONE)
	  return;

    if (quests[i].status == QST_COMPLETE) {
	  quest1_erase_line(0, 0);		// erase top line
      quest1_erase_middle();		// and erase middle

      strcpy(out_val1, quest_source[quests[i].type-1][quests[i].source]);
	  strcat(out_val1, " sits next to you and says:");
	  bank_prt(out_val1, 5, 2, QUEST1NUM);

      vbxe_bank(VBXE_GENBANK);
	  strcpy(text, quest_done[randint(QUEST_DONES)-1]);
      sprintf(out_val1, "Here's %ld gold ", quests[i].gold);
      strcat(text, out_val1);

	  if (quests[i].item != OBJ_NOTHING) {
		quest1_item_name(quests[i].item, out_val1);

  	    vbxe_bank(VBXE_GENBANK);
  	    sprintf(out_val2, "and %s ", out_val1);
  	    strcat(text, out_val2);
  	  }

	  strcat(text, "for your effort.");
	  bank_pretty_print(text, 7, QUEST1NUM);
      vbxe_restore_bank();

      bank_prt("press any key", 12, 33, QUEST1NUM);
	  cgetc();

	  /* Give over the item */
	  if (quests[i].type == QUEST_ITEM) {
		vbxe_bank(VBXE_OBJBANK);
	    for(j=0; j<INVEN_ARRAY_SIZE; ++j)
		  if ((inventory[j].index == quests[i].p1) && !(inventory[j].ident & ID_STOREBOUGHT))
		    break;

		bank_inven_destroy(j, QUEST1NUM);
  	  }

	  /* Collect your reward! */
	  py.misc.au += quests[i].gold;
	  py.misc.exp += quests[i].exp;

	  if (quests[i].item != OBJ_NOTHING) {
        j = quest1_popt();
        vbxe_bank(VBXE_OBJBANK);
        quest1_invcopy(&t_list[j], quests[i].item);
        bank_magic_treasure(j, 100, QUEST1NUM);		// very good chance item is magical!
        t_list[j].ident |= (ID_KNOWN2 | ID_STOREBOUGHT);

        bank_inven_carry(&t_list[j], QUEST1NUM);	// carry the item if we have room
        quest1_pusht(j);							// delete the item from the t_list
      }

	  /* Mark quest slot empty */
	  quests[i].type = QUEST_NONE;
	  bank_compact_quests();

      quest1_erase_middle();
	}
  }

  return;
}


/* Returns an interesting item number, appropriate for the level. -SJ */
int16u __fastcall__ quest1_get_item(int8u level)
{
  int16u item;


  while(1) {
    item = bank_get_obj_num(level+randint(4), QUEST1NUM);

    vbxe_bank(VBXE_OBJBANK);
    switch(object_list[item].tval) {
	  case TV_BOW:
	  case TV_HAFTED:
	  case TV_POLEARM:
	  case TV_SWORD:
	  case TV_BOOTS:
	  case TV_GLOVES:
	  case TV_CLOAK:
	  case TV_HELM:
	  case TV_SHIELD:
	  case TV_HARD_ARMOR:
	  case TV_SOFT_ARMOR:
	  case TV_AMULET:
	  case TV_RING:
	  case TV_STAFF:
	  case TV_WAND:
	  case TV_SCROLL1:
	  case TV_SCROLL2:
	  case TV_POTION1:
	  case TV_POTION2:
	  case TV_MAGIC_BOOK:
	  case TV_PRAYER_BOOK:
	    return(item);
	    break;
    }
  };

  return(OBJ_NOTHING);
}
