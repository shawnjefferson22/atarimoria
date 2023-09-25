/* source/guild.c: guild code, entering, command interpreter, buying, selling

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


#pragma code-name("GUILD1BANK")
#pragma rodata-name("GUILD1BANK")


#define GUILD_NUM_SERVICES 	6

#define GUILD_CUREPOISON 	0
#define GUILD_HEALWOUNDS 	1
#define GUILD_REMOVECURSE	2
#define GUILD_IDENTIFY 		3
#define GUILD_RECHARGE 		4
#define GUILD_TRAIN 		5

const int32u guild_service_cost[GUILD_NUM_SERVICES] =
{100L, 200L, 200L, 100L, 500L, 1000L};

char *guild_reject[5] = {
"Come back when you've proven yourself in battle!",
"Brave the mines and if you don't die, visit us again.",
"We only accept those who've proven themselves worthy!",
"We need members who aren't likely to die in the mines!",
"Return when you have learned something!"
};



/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ guild1_put_buffer(char *out_str, int8 row, int8 col);
void __fastcall__ guild1_erase_line(int8u row, int8u col);
int8u __fastcall__ guild1_cure_poison(void);
int8u __fastcall__ guild1_chr_adj(void);
int8u __fastcall__ guild1_inc_stat(int8u stat);
int8u __fastcall__ guild1_ask(int32u price);
void __fastcall__ guild_display_commands(void);
void __fastcall__ guild_train(void);
void __fastcall__ guild1_draw_cave(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		GUILD1NUM

#undef  INBANK_MOD
#define INBANK_MOD		guild1


void __fastcall__ guild1_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

void __fastcall__ guild1_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

/* Cure poisoning					-RAK-	*/
int8u __fastcall__ guild1_cure_poison(void)
#include "inbank\cure_poison.c"


/* Adjustment for charisma				-RAK-	*/
/* Percent decrease or increase in price of goods		 */
int8u __fastcall__ guild1_chr_adj(void)
{
  register int8u charisma;


  charisma = py.stats.use_stat[A_CHR];				// py is in BSS -SJ
  if (charisma > 117)
    return(90);
  else if (charisma > 107)
    return(92);
  else if (charisma > 87)
    return(94);
  else if (charisma > 67)
    return(96);
  else if (charisma > 18)
    return(98);
  else
    switch(charisma) {
      case 18:	return(100);
      case 17:	return(101);
      case 16:	return(102);
      case 15:	return(103);
      case 14:	return(104);
      case 13:	return(106);
      case 12:	return(108);
      case 11:	return(110);
      case 10:	return(112);
      case 9:  return(114);
      case 8:  return(116);
      case 7:  return(118);
      case 6:  return(120);
      case 5:  return(122);
      case 4:  return(125);
      case 3:  return(130);
      default: return(100);
    }
}


/* Increases a stat by one randomized level	-RAK- */
/* Function in this bank does not print the stat! -SJ */
int8u __fastcall__ guild1_inc_stat(int8u stat)
{
  int8u tmp_stat, gain;


  tmp_stat = py.stats.cur_stat[stat];
  if (tmp_stat < 118) {
    if (tmp_stat < 18)
	  ++tmp_stat;
    else if (tmp_stat < 116) {
	  /* stat increases by 1/6 to 1/3 of difference from max */
	  gain = ((118 - tmp_stat)/3 + 1) >> 1;
	  tmp_stat += randint(gain) + gain;
	}
    else
	  ++tmp_stat;

    py.stats.cur_stat[stat] = tmp_stat;
    if (tmp_stat > py.stats.max_stat[stat])
	  py.stats.max_stat[stat] = tmp_stat;

    bank_set_use_stat(stat, GUILD1NUM);
    //bank_prt_stat(stat, GUILD1NUM);
    return TRUE;
  }
  else
    return FALSE;
}


/* Gets the players answer and subtracts the gold -SJ */
int8u __fastcall__ guild1_ask(int32u price)
{
  char c;


  c = cgetc();
  if (c == 'y' || c == 'Y') {
	if (py.misc.au < price) {
  	  /* Not enough money! */
      bank_msg_print("You don't have enough gold!", GUILD1NUM);
      bank_msg_print("", GUILD1NUM);
      return(FALSE);
  	}
  	else {
  	  py.misc.au -= price;
  	  return(TRUE);
  	}
  }

  return(FALSE);
}


/* Displays the set of commands	-SJ	*/
void __fastcall__ guild_display_commands(void)
{
  guild1_put_buffer("Guild Services:", 19, 10);
  guild1_put_buffer(" p) Cure Poison.                h) Heal Wounds.", 20, 10);
  guild1_put_buffer(" c) Remove Curse.               i) Identify an Item.", 21, 10);
  guild1_put_buffer(" r) Recharge Wand/Staff.        t) Training.", 22, 10);
  guild1_put_buffer("ESC) Exit from Building.", 23, 10);
}


/* Displays players gold -RAK-	*/
void __fastcall__ guild1_prt_gold(void)
{
  sprintf(out_val1, "Gold Remaining : %ld", py.misc.au);			// py is in BSS -SJ
  bank_prt(out_val1, 17, 17, GUILD1NUM);
}


/* Entering the guild -SJ */
void __fastcall__ enter_guild(void)
{
  char command;
  int16u price;
  int8u exit_flag = FALSE;
  int8u redraw = FALSE;


  /* High enough level? */
  /*if (py.misc.lev < 3) {
	vbxe_bank(VBXE_SPLBANK);
    sprintf(out_val1, "The %s's Guild. %s", class[py.misc.pclass].title, guild_reject[randint(5)-1]);
    bank_msg_print(out_val1, GUILD1NUM);
    //vbxe_restore_bank();
    return;
  }*/

  /* Display the Guild */
  bank_vbxe_clear(GUILD1NUM);

  vbxe_bank(VBXE_SPLBANK);
  sprintf(out_val1, "The %s's Guild", class[py.misc.pclass]);
  guild1_put_buffer(out_val1, 3, 40-(strlen(out_val1)/2));
  guild1_prt_gold();

  /* Member? */
  if (!guild_member) {
    guild1_put_buffer("Welcome! I see you are not a member of our guild yet.", 5, 2);
    guild1_put_buffer("It costs 1000 gold to join before you can use any guild services.", 6, 2);
    guild1_put_buffer("Do you wish to join? [y/n]", 7, 2);

    if (guild1_ask(1000)) {			// Ask the player for $1000
  	  guild_member = TRUE;
  	  redraw = TRUE;
	}
    else {
      guild1_draw_cave();
      return;
    }
  }
  else {
    /* FIXME: Add some random greetings */
    sprintf(out_val1, "Welcome back %s! How can we help you?", (py.misc.male ? "Brother" : "Sister"));
    guild1_put_buffer(out_val1, 5, 2);
  }

  /* Guild main loop */
  guild_display_commands();
  do {
    if (redraw) {
      guild1_erase_line(5, 2);
      guild1_erase_line(6, 2);
      guild1_erase_line(7, 2);
	  guild1_prt_gold();
	  redraw = FALSE;
	}

    /* clear the msg flag just like we do in dungeon.c */
    msg_flag = FALSE;
    if (bank_get_com("", &command, GUILD1NUM)) {
      switch(command) {
		case 'p':						// cure poison
		  price = guild_service_cost[GUILD_CUREPOISON];
		  price = price * (int32u) guild1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold to rid you of poison.", price);
		  bank_prt(out_val1, 5, 2, GUILD1NUM);
		  bank_prt("Do you accept? [y/n]", 7, 2, GUILD1NUM);

		  if (guild1_ask(price))		// ask for money
		    guild1_cure_poison();

		  redraw = TRUE;
          break;
		case 'h':						// heal wounds
		  price = guild_service_cost[GUILD_HEALWOUNDS];
		  price = price * (int32u) guild1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold to heal your wounds.", price);
		  bank_prt(out_val1, 5, 2, GUILD1NUM);
		  bank_prt("Do you accept? [y/n]", 7, 2, GUILD1NUM);

		  if (guild1_ask(price)) {
	        py.misc.chp = py.misc.mhp;
	        py.misc.chp_frac = 0;
  	      }

		  redraw = TRUE;
		  break;
		case 'c':						// remove curse
		  price = guild_service_cost[GUILD_REMOVECURSE];
		  price = price * (int32u) guild1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold to lift a curse.", price);
		  bank_prt(out_val1, 5, 2, GUILD1NUM);
		  bank_prt("Do you accept? [y/n]", 7, 2, GUILD1NUM);

		  if (guild1_ask(price)) {
		    bank_remove_curse(GUILD1NUM);
  	      }

		  redraw = TRUE;
		  break;
		case 'i':						// identify item
		  price = guild_service_cost[GUILD_IDENTIFY];
		  price = price * (int32u) guild1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold to identify an item.", price);
		  bank_prt(out_val1, 5, 2, GUILD1NUM);
		  bank_prt("Do you accept? [y/n]", 7, 2, GUILD1NUM);

		  if (guild1_ask(price)) {
		    bank_ident_spell(GUILD1NUM);
  	      }

		  redraw = TRUE;
          break;
		case 'r':						// recharge
		  price = guild_service_cost[GUILD_RECHARGE];
		  price = price * (int32u) guild1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold to recharge a wand or staff.", price);
		  bank_prt(out_val1, 5, 2, GUILD1NUM);
		  bank_prt("Do you accept? [y/n]", 7, 2, GUILD1NUM);

		  if (guild1_ask(price)) {
		    bank_recharge(100, GUILD1NUM);
  	      }

		  redraw = TRUE;
		  break;
		case 't':						// training
		  price = guild_service_cost[GUILD_TRAIN];
		  price = price * (int32u) guild1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold for training.", price);
		  bank_prt(out_val1, 5, 2, GUILD1NUM);
		  bank_prt("Do you accept? [y/n]", 7, 2, GUILD1NUM);

		  if (guild1_ask(price)) {
		    guild_train();
  	      }

		  redraw = TRUE;
		  break;
		default:
		  break;
      }
	}
	else
	  exit_flag = TRUE;
  } while (!exit_flag);

  /* Can't save and restore the screen because inven_command does that. */
  guild1_draw_cave();
}


/* Draws entire screen					-RAK-	*/
void __fastcall__ guild1_draw_cave(void)
{
  //clear_screen();
  bank_vbxe_clear(GUILD1NUM);
  bank_prt_stat_block(GUILD1NUM);
  bank_prt_map(GUILD1NUM);
  bank_prt_depth(GUILD1NUM);
}


void __fastcall__ guild_train(void)
{
  char c;
  int8u ret;
  int8u exit_flag = FALSE;


  /* Erase the middle section */
  for(ret=5; ret<12; ++ret) {
    guild1_erase_line(ret, 0);
  }

  guild1_put_buffer("Train for which?", 5, 10);
  guild1_put_buffer("a) Strength", 7, 12);
  guild1_put_buffer("b) Intelligence", 8, 12);
  guild1_put_buffer("c) Wisdom", 9, 12);
  guild1_put_buffer("d) Dexterity", 10, 12);
  guild1_put_buffer("e) Constitution", 11, 12);
  //guild1_put_buffer("e) Fighting", 7, 44);
  //guild1_put_buffer("f) Bows/Throwing", 8, 44);
  //guild1_put_buffer("g) Stealth", 9, 44);
  //guild1_put_buffer("h) Disarming Traps", 10, 44);
  //guild1_put_buffer("f) Searching", 11, 44);

  do {
    c = cgetc();
	exit_flag = TRUE;
    switch (c) {
      case 'a':
      case 'A':
        ret = guild1_inc_stat(A_STR);
        if (ret) {
		  bank_msg_print("After much training, you feel stronger!", GUILD1NUM);
		  bank_msg_print("", GUILD1NUM);
	    }
		break;
      case 'b':
      case 'B':
        ret = guild1_inc_stat(A_INT);
        if (ret) {
		  bank_msg_print("After much studying, you feel smarter!", GUILD1NUM);
		  bank_msg_print("", GUILD1NUM);
	    }
		break;
      case 'c':
      case 'C':
        ret = guild1_inc_stat(A_WIS);
        if (ret) {
		  bank_msg_print("After much contemplation, you feel wiser!", GUILD1NUM);
		  bank_msg_print("", GUILD1NUM);
	    }
		break;
      case 'd':
      case 'D':
        ret = guild1_inc_stat(A_DEX);
        if (ret) {
		  bank_msg_print("After much training, you feel more nimble!", GUILD1NUM);
		  bank_msg_print("", GUILD1NUM);
		}
		break;
      case 'e':
      case 'E':
        ret = guild1_inc_stat(A_CON);
        if (ret) {
		  bank_msg_print("After much training, you feel more hale!", GUILD1NUM);
		  bank_msg_print("", GUILD1NUM);
	    }
		break;
	  default:
	    exit_flag = FALSE;
	    break;
	}
  } while (!exit_flag);

  if (!ret) {
    bank_msg_print("You train hard, but you cannot get any better.", GUILD1NUM);
    bank_msg_print("", GUILD1NUM);
  }

  for(ret=5; ret<12; ++ret) {
    guild1_erase_line(ret, 10);
  }

  return;
}
