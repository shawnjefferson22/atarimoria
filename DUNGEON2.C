/* source/dungeon.c: the main command interpreter, updating player status

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>


/* Put code, rodata and data in Dungeon1 bank */
#pragma code-name("DUNGEON2BANK")
#pragma rodata-name("DUNGEON2BANK")
#pragma data-name("DUNGEON2BANK")

/* Prototypes of functions copied into this bank */
void __fastcall__ dungeon2_change_speed(int8 num);
void __fastcall__ dungeon2_disturb(int8u s, int8u l);
void __fastcall__ dungeon2_search_off(void);
void __fastcall__ dungeon2_rest_off(void);
int8 __fastcall__ dungeon2_con_adj(void);
char* __fastcall__ dungeon2_describe_use(int8u i);
void __fastcall__ dungeon2_take_hit(int8u damage, char *hit_from);
void __fastcall__ regenhp(int16u percent);
void __fastcall__ regenmana(int16u percent);
int8u __fastcall__ enchanted(inven_type *t_ptr);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN2NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon2


void __fastcall__ dungeon2_change_speed(int8 num)
#include "inbank\change_speed.c"

void __fastcall__ dungeon2_disturb(int8u s, int8u l)
#include "inbank\disturb.c"

void __fastcall__ dungeon2_search_off(void)
#include "inbank\search_off.c"

void __fastcall__ dungeon2_rest_off(void)
#include "inbank\rest_off.c"

int8 __fastcall__ dungeon2_con_adj(void)
#include "inbank\con_adj.c"

void __fastcall__ dungeon2_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"

char* __fastcall__ dungeon2_describe_use(int8u i)
#include "inbank\describe_use.c"


/* Checks all the player flags, code moved from dungeon function in dungeon1 bank to here -SJ */
void __fastcall__ dungeon_check_flags(void)
{
  int16u regen_amount;	    		/* Regenerate hp and mana*/
  int8u i;
  register struct inven_type *i_ptr;
  //register struct flags *f_ptr;


  /* init pointers. */
  //f_ptr = &py.flags;							// both of these are in lowmen -SJ


    /* Update counters and messages			*/
    /* Check food status	       */
    regen_amount = PLAYER_REGEN_NORMAL;
    if (py.flags.food < PLAYER_FOOD_ALERT) {
	  if (py.flags.food < PLAYER_FOOD_WEAK) {
	    if (py.flags.food < 0)
		  regen_amount = 0;
	    else if (py.flags.food < PLAYER_FOOD_FAINT)
		  regen_amount = PLAYER_REGEN_FAINT;
	    else if (py.flags.food < PLAYER_FOOD_WEAK)
		  regen_amount = PLAYER_REGEN_WEAK;
	    if ((PY_WEAK & py.flags.status) == 0) {
		  py.flags.status |= PY_WEAK;
		  bank_msg_print("You are getting weak from hunger.", DUN2NUM);
		  dungeon2_disturb(0, 0);
		  bank_prt_hunger(DUN2NUM);
		}
	    if ((py.flags.food < PLAYER_FOOD_FAINT) && (randint(8) == 1)) {
		  py.flags.paralysis += randint(5);
		  bank_msg_print("You faint from the lack of food.", DUN2NUM);
		  dungeon2_disturb(1, 0);
		}
	  }
	  else if ((PY_HUNGRY & py.flags.status) == 0) {
	    py.flags.status |= PY_HUNGRY;
	    bank_msg_print("You are getting hungry.", DUN2NUM);
	    dungeon2_disturb(0, 0);
	    bank_prt_hunger(DUN2NUM);
	  }
	}

    /* Food consumption	*/
    /* Note: Speeded up characters really burn up the food!  */
    if (py.flags.speed < 0)
	  py.flags.food -= py.flags.speed*py.flags.speed;
    py.flags.food -= py.flags.food_digested;
    if (py.flags.food < 0) {
	  dungeon2_take_hit(-py.flags.food/16, "starvation");   /* -CJS- */
	  dungeon2_disturb(1, 0);
	}

    /* Regenerate	       */
    if (py.flags.regenerate)
      regen_amount = regen_amount * 3 / 2;
    if ((py.flags.status & PY_SEARCH) || py.flags.rest != 0)
	  regen_amount = regen_amount * 2;
    if ((py.flags.poisoned < 1) && (py.misc.chp < py.misc.mhp))
	  regenhp(regen_amount);
    if (py.misc.cmana < py.misc.mana)
	  regenmana(regen_amount);

    /* Blindness	       */
    if (py.flags.blind > 0) {
	  if ((PY_BLIND & py.flags.status) == 0) {
	    py.flags.status |= PY_BLIND;
	    bank_prt_map(DUN2NUM);
	    bank_prt_blind(DUN2NUM);
	    dungeon2_disturb(0, 1);
	    /* unlight creatures */
	    bank_creatures(FALSE, DUN2NUM);
	  }
	  py.flags.blind--;
	  if (py.flags.blind == 0) {
        py.flags.status &= ~PY_BLIND;
        bank_prt_blind(DUN2NUM);
	    bank_prt_map(DUN2NUM);
	    /* light creatures */
	    dungeon2_disturb(0, 1);
	    bank_creatures(FALSE, DUN2NUM);
	    bank_msg_print("The veil of darkness lifts.", DUN2NUM);
	  }
	}

    /* Confusion	       */
    if (py.flags.confused > 0) {
	  if ((PY_CONFUSED & py.flags.status) == 0) {
	    py.flags.status |= PY_CONFUSED;
	    bank_prt_confused(DUN2NUM);
	  }
	  py.flags.confused--;
	  if (py.flags.confused == 0) {
        py.flags.status &= ~PY_CONFUSED;
        bank_prt_confused(DUN2NUM);
	    bank_msg_print("You feel less confused now.", DUN2NUM);
	    if (py.flags.rest != 0)
		  dungeon2_rest_off();
	  }
	}

    /* Afraid		       */
    if (py.flags.afraid > 0) {
	  if ((PY_FEAR & py.flags.status) == 0) {
	    if ((py.flags.shero+py.flags.hero) > 0)
		  py.flags.afraid = 0;
	    else {
		  py.flags.status |= PY_FEAR;
		  bank_prt_afraid(DUN2NUM);
		}
	  }
	  else if ((py.flags.shero+py.flags.hero) > 0)
	    py.flags.afraid = 1;
	  py.flags.afraid--;
	  if (py.flags.afraid == 0) {
        py.flags.status &= ~PY_FEAR;
	    bank_prt_afraid(DUN2NUM);
	    bank_msg_print("You feel bolder now.", DUN2NUM);
	    dungeon2_disturb(0, 0);
	  }
	}

    /* Poisoned	       */
    if (py.flags.poisoned > 0) {
	  if ((PY_POISONED & py.flags.status) == 0) {
	    py.flags.status |= PY_POISONED;
	    bank_prt_poisoned(DUN2NUM);
	  }
	  py.flags.poisoned--;
	  if (py.flags.poisoned == 0) {
	    py.flags.status &= ~PY_POISONED;
	    bank_prt_poisoned(DUN2NUM);
	    bank_msg_print("You feel better.", DUN2NUM);
	    dungeon2_disturb(0, 0);
	  }
	  else {
	    switch(dungeon2_con_adj()) {
		  case -4:  i = 4;  break;
		  case -3:
		  case -2:  i = 3;  break;
		  case -1:  i = 2;  break;
		  case 0:	  i = 1;  break;
		  case 1: case 2: case 3:
		    i = ((turn % 2) == 0);
		    break;
		  case 4: case 5:
		    i = ((turn % 3) == 0);
		    break;
		  case 6:
		    i = ((turn % 4) == 0);
		    break;
		}
	    dungeon2_take_hit(i, "poison");
	    dungeon2_disturb(1, 0);
	  }
	}

    /* Fast		       */
    if (py.flags.fast > 0) {
	  if ((PY_FAST & py.flags.status) == 0) {
	      py.flags.status |= PY_FAST;
	      dungeon2_change_speed(-1);
	      bank_msg_print("You feel yourself moving faster.", DUN2NUM);
	      dungeon2_disturb(0, 0);
	  }
	  py.flags.fast--;
	  if (py.flags.fast == 0) {
	    py.flags.status &= ~PY_FAST;
	    dungeon2_change_speed(1);
	    bank_msg_print("You feel yourself slow down.", DUN2NUM);
	    dungeon2_disturb(0, 0);
	  }
	}

    /* Slow		       */
    if (py.flags.slow > 0) {
	  if ((PY_SLOW & py.flags.status) == 0) {
	    py.flags.status |= PY_SLOW;
	    dungeon2_change_speed(1);
	    bank_msg_print("You feel yourself moving slower.", DUN2NUM);
	    dungeon2_disturb (0, 0);
	  }
	  py.flags.slow--;
	  if (py.flags.slow == 0) {
        py.flags.status &= ~PY_SLOW;
        dungeon2_change_speed(-1);
        bank_msg_print("You feel yourself speed up.", DUN2NUM);
	    dungeon2_disturb(0, 0);
	  }
	}

    /* Resting is over?      */
    if (py.flags.rest > 0) {
	  py.flags.rest--;
	  if (py.flags.rest == 0)		      /* Resting over */
	    dungeon2_rest_off();
	}
    else if (py.flags.rest < 0) {
	  /* Rest until reach max mana and max hit points.  */
	  py.flags.rest++;
	  if ((py.misc.chp == py.misc.mhp && py.misc.cmana == py.misc.mana) || py.flags.rest == 0)
	    dungeon2_rest_off();
	}

    /* Check for interrupts to find or rest. */
    if ((command_count > 0 || find_flag || py.flags.rest != 0) && kbhit()) {
	  cgetc();
	  dungeon2_disturb(0, 0);
	}

    /* Hallucinating?	 (Random characters appear!)*/
    if (py.flags.image > 0) {
	  bank_end_find(DUN2NUM);
	  py.flags.image--;
	  if (py.flags.image == 0)
	    bank_prt_map(DUN2NUM);	 /* Used to draw entire screen! -CJS- */
	}

    /* Paralysis	       */
    if (py.flags.paralysis > 0) {
	  /* when paralysis true, you can not see any movement that occurs */
	  py.flags.paralysis--;
	  dungeon2_disturb(1, 0);
	}

    /* Protection from evil counter*/
    if (py.flags.protevil > 0) {
	  py.flags.protevil--;
	  if (py.flags.protevil == 0)
	    bank_msg_print("You no longer feel safe from evil.", DUN2NUM);
	}

    /* Invulnerability	*/
    if (py.flags.invuln > 0) {
	  if ((PY_INVULN & py.flags.status) == 0) {
	    py.flags.status |= PY_INVULN;
	    dungeon2_disturb(0, 0);
	    py.misc.pac += 100;
	    py.misc.dis_ac += 100;
	    bank_prt_pac(DUN2NUM);
	    bank_msg_print("Your skin turns into steel!", DUN2NUM);
	  }
	  py.flags.invuln--;
	  if (py.flags.invuln == 0) {
	    py.flags.status &= ~PY_INVULN;
	    dungeon2_disturb(0, 0);
	    py.misc.pac -= 100;
	    py.misc.dis_ac -= 100;
	    bank_prt_pac(DUN2NUM);
	    bank_msg_print("Your skin returns to normal.", DUN2NUM);
	  }
	}

    /* Heroism       */
    if (py.flags.hero > 0) {
	  if ((PY_HERO & py.flags.status) == 0) {
	    py.flags.status |= PY_HERO;
	    dungeon2_disturb(0, 0);
	    py.misc.mhp += 10;
	    py.misc.chp += 10;
	    py.misc.bth += 12;
	    py.misc.bthb+= 12;
	    bank_msg_print("You feel like a HERO!", DUN2NUM);
	    bank_prt_mhp(DUN2NUM);
	    bank_prt_chp(DUN2NUM);
	  }
	  py.flags.hero--;
	  if (py.flags.hero == 0) {
	    py.flags.status &= ~PY_HERO;
	    dungeon2_disturb(0, 0);
	    py.misc.mhp -= 10;
	    if (py.misc.chp > py.misc.mhp) {
		  py.misc.chp = py.misc.mhp;
		  py.misc.chp_frac = 0;
		  bank_prt_chp(DUN2NUM);
		}
	    py.misc.bth -= 12;
	    py.misc.bthb-= 12;
	    bank_msg_print("The heroism wears off.", DUN2NUM);
	    bank_prt_mhp(DUN2NUM);
	  }
	}

    /* Super Heroism */
    if (py.flags.shero > 0) {
	  if ((PY_SHERO & py.flags.status) == 0) {
	    py.flags.status |= PY_SHERO;
	    dungeon2_disturb (0, 0);
	    py.misc.mhp += 20;
	    py.misc.chp += 20;
	    py.misc.bth += 24;
	    py.misc.bthb+= 24;
	    bank_msg_print("You feel like a SUPER HERO!", DUN2NUM);
	    bank_prt_mhp(DUN2NUM);
	    bank_prt_chp(DUN2NUM);
	  }
	  py.flags.shero--;
	  if (py.flags.shero == 0) {
	    py.flags.status &= ~PY_SHERO;
	    dungeon2_disturb(0, 0);
	    py.misc.mhp -= 20;
	    if (py.misc.chp > py.misc.mhp) {
		  py.misc.chp = py.misc.mhp;
		  py.misc.chp_frac = 0;
		  bank_prt_chp(DUN2NUM);
		}
	    py.misc.bth -= 24;
	    py.misc.bthb-= 24;
	    bank_msg_print("The super heroism wears off.", DUN2NUM);
	    bank_prt_mhp(DUN2NUM);
	  }
	}

    /* Blessed       */
    if (py.flags.blessed > 0) {
	  if ((PY_BLESSED & py.flags.status) == 0) {
	    py.flags.status |= PY_BLESSED;
	    dungeon2_disturb (0, 0);
	    py.misc.bth += 5;
	    py.misc.bthb+= 5;
	    py.misc.pac += 2;
	    py.misc.dis_ac+= 2;
	    bank_msg_print("You feel righteous!", DUN2NUM);
	    bank_prt_pac(DUN2NUM);
	  }
	  py.flags.blessed--;
	  if (py.flags.blessed == 0) {
	    py.flags.status &= ~PY_BLESSED;
	    dungeon2_disturb (0, 0);
	    py.misc.bth -= 5;
	    py.misc.bthb-= 5;
	    py.misc.pac -= 2;
	    py.misc.dis_ac -= 2;
	    bank_msg_print("The prayer has expired.", DUN2NUM);
	    bank_prt_pac(DUN2NUM);
	  }
	}

    /* Resist Heat   */
    if (py.flags.resist_heat > 0)	{
	  py.flags.resist_heat--;
	  if (py.flags.resist_heat == 0)
	    bank_msg_print("You no longer feel safe from flame.", DUN2NUM);
	}

    /* Resist Cold   */
    if (py.flags.resist_cold > 0) {
	  py.flags.resist_cold--;
	  if (py.flags.resist_cold == 0)
	    bank_msg_print("You no longer feel safe from cold.", DUN2NUM);
	}

    /* Detect Invisible      */
    if (py.flags.detect_inv > 0) {
	  if ((PY_DET_INV & py.flags.status) == 0) {
	    py.flags.status |= PY_DET_INV;
	    py.flags.see_inv = TRUE;
	    /* light but don't move creatures */
	    bank_creatures(FALSE, DUN2NUM);
	  }
	  py.flags.detect_inv--;
	  if (py.flags.detect_inv == 0) {
	    py.flags.status &= ~PY_DET_INV;
	    /* may still be able to see_inv if wearing magic item */
	    bank_calc_bonuses(DUN2NUM);
	    /* unlight but don't move creatures */
	    bank_creatures(FALSE, DUN2NUM);
	  }
	}

    /* Timed infra-vision    */
    if (py.flags.tim_infra > 0) {
	  if ((PY_TIM_INFRA & py.flags.status) == 0) {
	    py.flags.status |= PY_TIM_INFRA;
	    py.flags.see_infra++;
	    /* light but don't move creatures */
	    bank_creatures(FALSE, DUN2NUM);
	  }
	  py.flags.tim_infra--;
	  if (py.flags.tim_infra == 0) {
	    py.flags.status &= ~PY_TIM_INFRA;
	    py.flags.see_infra--;
	    /* unlight but don't move creatures */
	    bank_creatures(FALSE, DUN2NUM);
	  }
	}

	/* Word-of-Recall  Note: Word-of-Recall is a delayed action	 */
    if (py.flags.word_recall > 0)
	  if (py.flags.word_recall == 1) {
	    new_level_flag = TRUE;
	    py.flags.paralysis++;
	    py.flags.word_recall = 0;
	    if (dun_level > 0) {
		  dun_level = 0;
		  bank_msg_print("You feel yourself yanked upwards!", DUN2NUM);
	    }
	    else if (py.misc.max_dlv != 0) {
		  dun_level = py.misc.max_dlv;
		  bank_msg_print("You feel yourself yanked downwards!", DUN2NUM);
	    }
	  }
	  else
	    py.flags.word_recall--;

    /* Random teleportation  */
    if ((py.flags.teleport) && (randint(100) == 1)) {
	  dungeon2_disturb(0, 0);
	  bank_teleport(40, DUN2NUM);
	}

    /* See if we are too weak to handle the weapon or pack.  -CJS- */
    if (py.flags.status & PY_STR_WGT)
	  bank_check_strength(DUN2NUM);

    if (py.flags.status & PY_STUDY)
	  bank_prt_study(DUN2NUM);
    if (py.flags.status & PY_SPEED) {
	  py.flags.status &= ~PY_SPEED;
	  bank_prt_speed(DUN2NUM);
	}

    if ((py.flags.status & PY_PARALYSED) && (py.flags.paralysis < 1)) {
	  bank_prt_state(DUN2NUM);
	  py.flags.status &= ~PY_PARALYSED;
	}
    else if (py.flags.paralysis > 0) {
	  bank_prt_state(DUN2NUM);
	  py.flags.status |= PY_PARALYSED;
	}
    else if (py.flags.rest != 0)
	  bank_prt_state(DUN2NUM);

    if ((py.flags.status & PY_ARMOR) != 0) {
	  bank_prt_pac(DUN2NUM);
	  py.flags.status &= ~PY_ARMOR;
	}

    if ((py.flags.status & PY_STATS) != 0) {
	  for (i = 0; i < 6; ++i)
	    if ((PY_STR << i) & py.flags.status)
	      bank_prt_stat(i, DUN2NUM);
	  py.flags.status &= ~PY_STATS;
	}

    if (py.flags.status & PY_HP) {
	  bank_prt_mhp(DUN2NUM);
	  bank_prt_chp(DUN2NUM);
	  py.flags.status &= ~PY_HP;
	}

    if (py.flags.status & PY_MANA) {
	  bank_prt_cmana(DUN2NUM);
	  py.flags.status &= ~PY_MANA;
	}

    /* Allow for a slim chance of detect enchantment -CJS- */
    /* for 1st level char, check once every 2160 turns
	 for 40th level char, check once every 416 turns */
    if (((turn & 0xF) == 0) && (py.flags.confused == 0) && (randint((int)(10 + 750 / (5 + py.misc.lev))) == 1)) {
	  //vtype tmp_str; // use out_val1 -SJ

	  for (i = 0; i < INVEN_ARRAY_SIZE; ++i) {
	    if (i == inven_ctr)
		  i = 22;

		vbxe_bank(VBXE_OBJBANK);
	    i_ptr = &inventory[i];
	    /* if in inventory, succeed 1 out of 50 times, if in equipment list, success 1 out of 10 times */
	    if ((i_ptr->tval != TV_NOTHING) && enchanted(i_ptr) && (randint(i < 22 ? 50 : 10) == 1)) {
		  sprintf(out_val1, "There's something about what you are %s...", dungeon2_describe_use(i));
		  dungeon2_disturb(0, 0);
		  bank_msg_print(out_val1, DUN2NUM);
		  i_ptr->ident |= ID_MAGIK;								// inline add_inscribe code -SJ
		}
	  }
	}

	return;
}


/* Regenerate hit points				-RAK-	*/
void __fastcall__ regenhp(int16u percent)
{
  register int32 new_chp, new_chp_frac;
  int16 old_chp;


  old_chp = py.misc.chp;
  new_chp = ((long)py.misc.mhp) * percent + PLAYER_REGEN_HPBASE;				// 100 * 197 + 1442 = 21142
  py.misc.chp += new_chp >> 16;	/* div 65536 */								// divide by 65536
  /* check for overflow */
  if (py.misc.chp < 0 && old_chp > 0)
    py.misc.chp = MAX_SHORT;													// 32767
  new_chp_frac = (new_chp & 0xFFFF) + py.misc.chp_frac; /* mod 65536 */		//
  if (new_chp_frac >= 0x10000L) {
    py.misc.chp_frac = new_chp_frac - 0x10000L;
    py.misc.chp++;
  }
  else
    py.misc.chp_frac = new_chp_frac;

  /* must set frac to zero even if equal */
  if (py.misc.chp >= py.misc.mhp) {
    py.misc.chp = py.misc.mhp;
    py.misc.chp_frac = 0;
  }
  if (old_chp != py.misc.chp)
    bank_prt_chp(DUN2NUM);
}


/* Regenerate mana points				-RAK-	*/
void __fastcall__ regenmana(int16u percent)
{
  register int32 new_mana, new_mana_frac;
  int16 old_cmana;


  old_cmana = py.misc.cmana;
  new_mana = ((long)py.misc.mana) * percent + PLAYER_REGEN_MNBASE;
  py.misc.cmana += new_mana >> 16;  /* div 65536 */
  /* check for overflow */
  if (py.misc.cmana < 0 && old_cmana > 0)
    py.misc.cmana = MAX_SHORT;
  new_mana_frac = (new_mana & 0xFFFF) + py.misc.cmana_frac; /* mod 65536 */
  if (new_mana_frac >= 0x10000L) {
    py.misc.cmana_frac = new_mana_frac - 0x10000L;
    py.misc.cmana++;
  }
  else
    py.misc.cmana_frac = new_mana_frac;

  /* must set frac to zero even if equal */
  if (py.misc.cmana >= py.misc.mana) {
    py.misc.cmana = py.misc.mana;
    py.misc.cmana_frac = 0;
  }
  if (old_cmana != py.misc.cmana)
    bank_prt_cmana(DUN2NUM);
}


/* Is an item an enchanted weapon or armor and we don't know?  -CJS- */
/* only returns true if it is a good enchantment */
int8u __fastcall__ enchanted(inven_type *t_ptr)
{
  if (t_ptr->tval < TV_MIN_ENCHANT || t_ptr->tval > TV_MAX_ENCHANT || t_ptr->flags & TR_CURSED)
    return FALSE;
  //if (known2_p(t_ptr))
  if (t_ptr->ident & ID_KNOWN2)						// known2_p is simple, inline it -SJ
    return FALSE;
  if (t_ptr->ident & ID_MAGIK)
    return FALSE;
  if (t_ptr->tohit > 0 || t_ptr->todam > 0 || t_ptr->toac > 0)
    return TRUE;
  if ((0x4000107fL & t_ptr->flags) && t_ptr->p1 > 0)
    return TRUE;
  if (0x07ffe980L & t_ptr->flags)
    return TRUE;

  return FALSE;
}


