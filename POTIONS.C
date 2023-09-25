/* source/potions.c: code for potions

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("POTIONBANK")
#pragma data-name("POTIONBANK")
#pragma rodata-name("POTIONBANK")


/* Local Prototypes */
int8u __fastcall__ potion1_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8 __fastcall__ potion1_object_offset(inven_type *t_ptr);
int8u __fastcall__ potion1_known1_p(inven_type *i_ptr);
void __fastcall__ potion1_sample(inven_type *i_ptr);
int8 __fastcall__ potion1_bit_pos(int32u *test);
int8u __fastcall__ potion1_damroll(int8u num, int8u sides);
int8u __fastcall__ potion1_hp_player(int16 num);
int8u __fastcall__ potion1_res_stat(int8u stat);
int8u __fastcall__ inc_stat(int8u stat);
int8u __fastcall__ potion1_cure_confusion(void);
int8u __fastcall__ potion1_cure_blindness(void);
int8u __fastcall__ potion1_cure_poison(void);
void __fastcall__ potion1_lose_str(void);
void __fastcall__ potion1_lose_int(void);
void __fastcall__ potion1_lose_wis(void);
void __fastcall__ potion1_lose_chr(void);
void __fastcall__ potion1_lose_exp(int32 amount);
int8u __fastcall__ potion1_slow_poison(void);
int8u __fastcall__ potion1_remove_fear(void);
int8u __fastcall__ potion1_restore_level(void);
void __fastcall__ potion1_add_food(int16 num);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		POTIONNUM

#undef  INBANK_MOD
#define INBANK_MOD			potion1


int8u __fastcall__ potion1_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

int8 __fastcall__ potion1_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

int8u __fastcall__ potion1_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

void __fastcall__ potion1_sample(inven_type *i_ptr)
#include "inbank\sample.c"

int8 __fastcall__ potion1_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

int8u __fastcall__ potion1_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ potion1_hp_player(int16 num)
#include "inbank\hp_player.c"

int8u __fastcall__ potion1_cure_confusion(void)
#include "inbank\cure_confusion.c"

int8u __fastcall__ potion1_cure_blindness(void)
#include "inbank\cure_blindness.c"

/* Cure poisoning					-RAK-	*/
int8u __fastcall__ potion1_cure_poison(void)
#include "inbank\cure_poison.c"

int8u __fastcall__ potion1_res_stat(int8u stat)
#include "inbank\res_stat.c"


/* Increases a stat by one randomized level	-RAK- */
/* Only used in this bank -SJ                     */
int8u __fastcall__ inc_stat(int8u stat)
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

    bank_set_use_stat(stat, POTIONNUM);
    bank_prt_stat(stat, POTIONNUM);
    return TRUE;
  }
  else
    return FALSE;
}


void __fastcall__ potion1_lose_str(void)
#include "inbank\lose_str.c"

/* Lose an intelligence point.				-RAK-	*/
void __fastcall__ potion1_lose_int(void)
{
  if (!py.flags.sustain_int) {
    bank_dec_stat(A_INT, POTIONNUM);
    bank_msg_print("You become very dizzy.", POTIONNUM);
  }
  else
    bank_msg_print("You become dizzy for a moment, it passes.", POTIONNUM);
}


/* Lose a wisdom point.					-RAK-	*/
void __fastcall__ potion1_lose_wis(void)
{
  if (!py.flags.sustain_wis) {
    bank_dec_stat(A_WIS, POTIONNUM);
    bank_msg_print("You feel very naive.", POTIONNUM);
  }
  else
    bank_msg_print("You feel naive for a moment, it passes.", POTIONNUM);
}


/* Lose a charisma point.				-RAK-	*/
void __fastcall__ potion1_lose_chr(void)
{
  if (!py.flags.sustain_chr) {
    bank_dec_stat(A_CHR, POTIONNUM);
    bank_msg_print("Your skin starts to itch.", POTIONNUM);
  }
  else
    bank_msg_print("Your skin starts to itch, but feels better now.", POTIONNUM);
}


/* Lose experience					-RAK-	*/
void __fastcall__ potion1_lose_exp(int32 amount)
{
  register int8u i;


  if (amount > py.misc.exp)
    py.misc.exp = 0;
  else
    py.misc.exp -= amount;
  bank_prt_experience(POTIONNUM);

  i = 0;
  vbxe_bank(VBXE_SPLBANK);
  while ((player_exp[i] * py.misc.expfact / 100) <= py.misc.exp)
    ++i;
  /* increment i once more, because level 1 exp is stored in player_exp[0] */
  ++i;

  if (py.misc.lev != i) {
    py.misc.lev = i;

    bank_calc_hitpoints(POTIONNUM);
    if (class[py.misc.pclass].spell == MAGE) {
	  bank_calc_spells(A_INT, POTIONNUM);
	  bank_calc_mana(A_INT, POTIONNUM);
	}
    else if (class[py.misc.pclass].spell == PRIEST) {
	  bank_calc_spells(A_WIS, POTIONNUM);
	  bank_calc_mana(A_WIS, POTIONNUM);
	}

    bank_prt_level(POTIONNUM);
    bank_prt_title(POTIONNUM);
  }

  vbxe_restore_bank();
}


/* Slow Poison	-RAK-	*/
int8u __fastcall__ potion1_slow_poison(void)
#include "inbank\slow_poison.c"

/* Cure the players fear				-RAK-	*/
int8u __fastcall__ potion1_remove_fear(void)
#include "inbank\remove_fear.c"


/* Restores any drained experience			-RAK-	*/
int8u __fastcall__ potion1_restore_level(void)
{
  register int8u restore;


  restore = FALSE;
  if (py.misc.max_exp > py.misc.exp) {
    restore = TRUE;
    bank_msg_print("You feel your life energies returning.", POTIONNUM);
    /* this while loop is not redundant, ptr_exp may reduce the exp level */
    while (py.misc.exp < py.misc.max_exp) {
	  py.misc.exp = py.misc.max_exp;
	  bank_prt_experience(POTIONNUM);
	}
  }

  return(restore);
}


/* Add to the players food time				-RAK-	*/
void __fastcall__ potion1_add_food(int16 num)
{
  register int16 extra, penalty;


  if (py.flags.food < 0)
    py.flags.food = 0;

  py.flags.food += (int16) num;
  if (py.flags.food > PLAYER_FOOD_MAX) {
    bank_msg_print("You are bloated from overeating.", POTIONNUM);

    /* Calculate how much of num is responsible for the bloating.
	   Give the player food credit for 1/50, and slow him for that many
	   turns also.  */
    extra = py.flags.food - PLAYER_FOOD_MAX;
    if (extra > num)
	  extra = num;

    penalty = extra / 50;

    py.flags.slow += penalty;
    if (extra == num)
	  py.flags.food = py.flags.food - num + penalty;
    else
	  py.flags.food = PLAYER_FOOD_MAX + penalty;
  }
  else if (py.flags.food > PLAYER_FOOD_FULL)
    bank_msg_print("You are full.", POTIONNUM);
}


/* Potions for the quaffing	-RAK- */
void quaff(void)
{
  int32u i, l;
  int32 m, scale;
  int8u j, k, item_val;
  int8u ident;
  register inven_type *i_ptr;


  free_turn_flag = TRUE;
  if (inven_ctr == 0)
    bank_msg_print("But you are not carrying anything.", POTIONNUM);
  else if (!potion1_find_range(TV_POTION1, TV_POTION2, &j, &k))
    bank_msg_print("You are not carrying any potions.", POTIONNUM);
  else if (bank_get_item(&item_val, "Quaff which potion?", j, k, CNIL, "", POTIONNUM)) {
    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[item_val];
    i = i_ptr->flags;

    free_turn_flag = FALSE;
    ident = FALSE;
    if (i == 0) {
	  bank_msg_print ("You feel less thirsty.", POTIONNUM);
	  ident = TRUE;
	}
    else while (i != 0) {
	  j = potion1_bit_pos(&i) + 1;
	  if (i_ptr->tval == TV_POTION2)
	    j += 32;

	  /* Potions */
	  switch(j) {
	    case 1:
	      if (inc_stat(A_STR)) {
		    bank_msg_print("Wow! What bulging muscles!", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 2:
	      ident = TRUE;
	      potion1_lose_str();
	      break;
	    case 3:
	      if (potion1_res_stat(A_STR)) {
		    bank_msg_print("You feel warm all over.", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 4:
	      if (inc_stat(A_INT)) {
		    bank_msg_print("Aren't you brilliant!", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 5:
	      ident = TRUE;
	      potion1_lose_int();
	      break;
	    case 6:
	      if (potion1_res_stat(A_INT)) {
		    bank_msg_print("You have have a warm feeling.", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 7:
	      if (inc_stat(A_WIS)) {
		    bank_msg_print("You suddenly have a profound thought!", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 8:
	      ident = TRUE;
	      potion1_lose_wis();
	      break;
	    case 9:
	      if (potion1_res_stat(A_WIS)) {
		    bank_msg_print("You feel your wisdom returning.", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 10:
	      if (inc_stat(A_CHR)) {
		    bank_msg_print("Gee, ain't you cute!", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 11:
	      ident = TRUE;
	      potion1_lose_chr();
	      break;
	    case 12:
	      if (potion1_res_stat(A_CHR)) {
		    bank_msg_print("You feel your looks returning.", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 13:
	      ident = potion1_hp_player(potion1_damroll(2, 7));
	      break;
	    case 14:
	      ident = potion1_hp_player(potion1_damroll(4, 7));
	      break;
	    case 15:
	      ident = potion1_hp_player(potion1_damroll(6, 7));
	      break;
	    case 16:
	      ident = potion1_hp_player(1000);
	      break;
	    case 17:
	      if (inc_stat(A_CON)) {
		    bank_msg_print("You feel tingly for a moment.", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 18:
	      if (py.misc.exp < MAX_EXP) {
		    l = (py.misc.exp / 2) + 10;
		    if (l > 100000L)  l = 100000L;
		    py.misc.exp += l;
		    bank_msg_print("You feel more experienced.", POTIONNUM);
		    bank_prt_experience(POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 19:
	      if (!py.flags.free_act) {
		    /* paralysis must == 0, otherwise could not drink potion */
		    bank_msg_print("You fall asleep.", POTIONNUM);
		    py.flags.paralysis += randint(4) + 4;
		    ident = TRUE;
		  }
	      break;
	    case 20:
	      if (py.flags.blind == 0) {
		    bank_msg_print("You are covered by a veil of darkness.", POTIONNUM);
		    ident = TRUE;
		  }
	      py.flags.blind += randint(100) + 100;
	      break;
	    case 21:
	      if (py.flags.confused == 0) {
		    bank_msg_print("Hey! This is good stuff! *Hick!*", POTIONNUM);
		    ident = TRUE;
		  }
	      py.flags.confused += randint(20) + 12;
	      break;
	    case 22:
	      if (py.flags.poisoned == 0) {
		    bank_msg_print("You feel very sick.", POTIONNUM);
		    ident = TRUE;
		  }
	      py.flags.poisoned += randint(15) + 10;
	      break;
	    case 23:
	      if (py.flags.fast == 0)
		    ident = TRUE;
	      py.flags.fast += randint(25) + 15;
	      break;
	    case 24:
	      if (py.flags.slow == 0)
		    ident = TRUE;
	      py.flags.slow += randint(25) + 15;
	      break;
	    case 26:
	      if (inc_stat(A_DEX)) {
		    bank_msg_print("You feel more limber!", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 27:
	      if (potion1_res_stat(A_DEX)) {
		    bank_msg_print("You feel less clumsy.", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 28:
	      if (potion1_res_stat(A_CON)) {
		    bank_msg_print("You feel your health returning!", POTIONNUM);
		    ident = TRUE;
		  }
	      break;
	    case 29:
	      ident = potion1_cure_blindness();
	      break;
	    case 30:
	      ident = potion1_cure_confusion();
	      break;
	    case 31:
	      ident = potion1_cure_poison();
	      break;
	    case 34:
	      if (py.misc.exp > 0) {
		    bank_msg_print("You feel your memories fade.", POTIONNUM);
		    /* Lose between 1/5 and 2/5 of your experience */
		    m = py.misc.exp / 5;
		    if (py.misc.exp > MAX_SHORT) {
		      scale = MAX_LONG / py.misc.exp;
		      m += (randint((int)scale) * py.misc.exp) / (scale * 5);
		    }
		    else
		      m += randint((int)py.misc.exp) / 5;
		    potion1_lose_exp(m);
		    ident = TRUE;
		  }
	      break;
	    case 35:
	      potion1_cure_poison();
	      if (py.flags.food > 150)
	        py.flags.food = 150;
	      py.flags.paralysis = 4;
	      bank_msg_print("The potion makes you vomit!", POTIONNUM);
	      ident = TRUE;
	      break;
	    case 36:
	      if (py.flags.invuln == 0)
		    ident = TRUE;
	      py.flags.invuln += randint(10) + 10;
	      break;
	    case 37:
	      if (py.flags.hero == 0)
		    ident = TRUE;
	      py.flags.hero += randint(25) + 25;
	      break;
	    case 38:
	      if (py.flags.shero == 0)
		    ident = TRUE;
	      py.flags.shero += randint(25) + 25;
	      break;
	    case 39:
	      ident = potion1_remove_fear();
	      break;
	    case 40:
	      ident = potion1_restore_level();
	      break;
	    case 41:
	      if (py.flags.resist_heat == 0)
		    ident = TRUE;
	      py.flags.resist_heat += randint(10) + 10;
	      break;
	    case 42:
	      if (py.flags.resist_cold == 0)
		    ident = TRUE;
	      py.flags.resist_cold += randint(10) + 10;
	      break;
	    case 43:
	      if (py.flags.detect_inv == 0)
		    ident = TRUE;
	      //detect_inv2(randint(12)+12);
	      py.flags.detect_inv += randint(12)+12;				// in-line detect_inv2 -SJ
	      break;
	    case 44:
	      ident = potion1_slow_poison();
	      break;
	    case 45:
	      ident = potion1_cure_poison();
	      break;
	    case 46:
	      if (py.misc.cmana < py.misc.mana) {
		    py.misc.cmana = py.misc.mana;
		    ident = TRUE;
		    bank_msg_print("Your feel your head clear.", POTIONNUM);
		    bank_prt_cmana(POTIONNUM);
		  }
	      break;
	    case 47:
	      if (py.flags.tim_infra == 0) {
		    bank_msg_print("Your eyes begin to tingle.", POTIONNUM);
		    ident = TRUE;
		  }
	      py.flags.tim_infra += 100 + randint(100);
	      break;
	    default:
	      bank_msg_print("Internal error in potion()", POTIONNUM);
	      break;
	    }
	  /* End of Potions. */
	}


    vbxe_bank(VBXE_OBJBANK);
    if (ident) {
	  if (!potion1_known1_p(i_ptr)) {
	    /* round half-way case up */
	    py.misc.exp += ((int32) i_ptr->level + ((int32) py.misc.lev >> 1)) / (int32) py.misc.lev;
	    bank_prt_experience(POTIONNUM);

	    bank_identify(&item_val, POTIONNUM);
	    i_ptr = &inventory[item_val];
	  }
	}
    else if (!potion1_known1_p(i_ptr))
	  potion1_sample(i_ptr);

    potion1_add_food(i_ptr->p1);
    bank_desc_remain(item_val, POTIONNUM);
    bank_inven_destroy(item_val, POTIONNUM);
  }
}

