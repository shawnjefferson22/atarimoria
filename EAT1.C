/* source/eat.c: food code

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


/* Put code, rodata and data in EAT1 bank */
#pragma code-name("EAT1BANK")
#pragma rodata-name("EAT1BANK")
#pragma data-name("EAT1BANK")


/* Prototypes of functions copied into this bank */
int8u __fastcall__ eat1_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8 __fastcall__ eat1_bit_pos(int32u *test);
int8 __fastcall__ eat1_object_offset(inven_type *t_ptr);
int8u __fastcall__ eat1_known1_p(inven_type *i_ptr);
void __fastcall__ eat1_sample(inven_type *i_ptr);
void __fastcall__ eat1_identify(int8u *item);
void __fastcall__ eat1_known1(inven_type *i_ptr);
void __fastcall__ eat1_invcopy(inven_type *to, int16u from_index);
int8u __fastcall__ eat1_cure_confusion(void);
int8u __fastcall__ eat1_cure_blindness(void);
int8u __fastcall__ eat1_cure_poison(void);
void __fastcall__ eat1_lose_str(void);
void __fastcall__ eat1_lose_con(void);
int8u __fastcall__ eat1_hp_player(int16 num);
void __fastcall__ add_food(int16 num);
void __fastcall__ eat1_draw_cave(void);
void __fastcall__ eat1_take_hit(int8u damage, char *hit_from);
int8u __fastcall__ eat1_res_stat(int8u stat);
int8u __fastcall__ eat1_damroll(int8u num, int8u sides);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		EAT1NUM

#undef  INBANK_MOD
#define INBANK_MOD			eat1


int8u __fastcall__ eat1_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

int8 __fastcall__ eat1_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

int8 __fastcall__ eat1_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

int8u __fastcall__ eat1_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

void __fastcall__ eat1_sample(inven_type *i_ptr)
#include "inbank\sample.c"

int8u __fastcall__ eat1_cure_confusion(void)
#include "inbank\cure_confusion.c"

int8u __fastcall__ eat1_cure_blindness(void)
#include "inbank\cure_blindness.c"

int8u __fastcall__ eat1_cure_poison(void)
#include "inbank\cure_poison.c"

void __fastcall__ eat1_known1(inven_type *i_ptr)
#include "inbank\known1.c"

void __fastcall__ eat1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"


/* Somethings been identified					*/
/* extra complexity by CJS so that it can merge store/dungeon objects
   when appropriate */
void __fastcall__ eat1_identify(int8u *item)
{
  register int8u i, x1, x2;
  int8u j;
  register inven_type *i_ptr, *t_ptr;

  vbxe_bank(VBXE_OBJBANK);
  i_ptr = &inventory[*item];

  if (i_ptr->flags & TR_CURSED)
    //add_inscribe(i_ptr, ID_DAMD);
    i_ptr->ident |= ID_DAMD;						// inline add_inscribe code here -SJ

  if (!eat1_known1_p(i_ptr)) {
    eat1_known1(i_ptr);
    x1 = i_ptr->tval;
    x2 = i_ptr->subval;
    if (x2 < ITEM_SINGLE_STACK_MIN || x2 >= ITEM_GROUP_MIN)
	/* no merging possible */
	  ;
    else
	  for (i = 0; i < inven_ctr; ++i) {
	    t_ptr = &inventory[i];
	    if (t_ptr->tval == x1 && t_ptr->subval == x2 && i != *item && ((int)t_ptr->number + (int)i_ptr->number < 256)) {
		  /* make *item the smaller number */
		  if (*item > i) {
		    j = *item; *item = i; i = j;
		  }
	      bank_msg_print ("You combine similar objects from the shop and dungeon.", EAT1NUM);

		  inventory[*item].number += inventory[i].number;
		  --inven_ctr;
		  for (j = i; j < inven_ctr; ++j)
		    memcpy(&inventory[j], &inventory[j+1], sizeof(inven_type));
		  eat1_invcopy(&inventory[j], OBJ_NOTHING);
	    }
	  }
    }
}


void __fastcall__ eat1_lose_str(void)
#include "inbank\lose_str.c"

int8u __fastcall__ eat1_hp_player(int16 num)
#include "inbank\hp_player.c"

void __fastcall__ eat1_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"

int8u __fastcall__ eat1_res_stat(int8u stat)
#include "inbank\res_stat.c"

int8u __fastcall__ eat1_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"


/* Add to the players food time				-RAK-	*/
void __fastcall__ add_food(int16 num)
{
  int16 extra;
  int8u penalty;
  int8u old_slow;


  if (py.flags.food < 0)
    py.flags.food = 0;

  py.flags.food += num;
  if (py.flags.food > PLAYER_FOOD_MAX) {
    bank_msg_print("You are bloated from overeating.", EAT1NUM);

    /* Calculate how much of num is responsible for the bloating.
	   Give the player food credit for 1/50, and slow him for that many
	   turns also.  */
    extra = py.flags.food - PLAYER_FOOD_MAX;
    if (extra > num)
	  extra = num;

    penalty = (int8u) (extra / 50);

    old_slow = py.flags.slow;				// added to avoid rollover of int8u slow -SJ
    py.flags.slow += penalty;
    if (py.flags.slow < old_slow)
      py.flags.slow = 255;

    if (extra == num)
	  py.flags.food = py.flags.food - num + (int16) penalty;
	else
	  py.flags.food = PLAYER_FOOD_MAX + (int16) penalty;
  }
  else if (py.flags.food > PLAYER_FOOD_FULL)
    bank_msg_print("You are full.", EAT1NUM);
}


/* Lose a constitution point. -RAK-	*/
/* Only exists in this bank -SJ     */
void __fastcall__ eat1_lose_con(void)
{
  if (!py.flags.sustain_con) {
    bank_dec_stat(A_CON, EAT1NUM);
    bank_msg_print("You feel very sick.", EAT1NUM);
  }
  else
    bank_msg_print("You feel sick for a moment, it passes.", EAT1NUM);
}


/* Eat some food.					-RAK-	*/
void __fastcall__ eat(void)
{
  int32u i;
  int8u j, k;
  int8u item_val;
  register int8u ident;
  register inven_type *i_ptr;


  free_turn_flag = TRUE;
  if (inven_ctr == 0)
    bank_msg_print("But you are not carrying anything.", EAT1NUM);
  else if (!eat1_find_range(TV_FOOD, TV_NEVER, &j, &k))
    bank_msg_print("You are not carrying any food.", EAT1NUM);
  else if (bank_get_item(&item_val, "Eat what?", j, k, CNIL, "", EAT1NUM)) {
    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[item_val];
    free_turn_flag = FALSE;
    i = i_ptr->flags;
    ident = FALSE;
    while (i != 0) {
	  j = eat1_bit_pos(&i) + 1;
	  /* Foods */
	  switch(j) {
	    case 1:
	      py.flags.poisoned += randint(10) + i_ptr->level;
	      ident = TRUE;
	      break;
	    case 2:
	      py.flags.blind += randint(250) + 10*i_ptr->level + 100;
	      eat1_draw_cave();
	      bank_msg_print("A veil of darkness surrounds you.", EAT1NUM);
	      ident = TRUE;
	      break;
	    case 3:
	      py.flags.afraid += randint(10) + i_ptr->level;
	      bank_msg_print("You feel terrified!", EAT1NUM);
	      ident = TRUE;
	      break;
	    case 4:
	      py.flags.confused += randint(10) + i_ptr->level;
	      bank_msg_print("You feel drugged.", EAT1NUM);
	      ident = TRUE;
	      break;
	    case 5:
	      py.flags.image += randint(200) + 25*i_ptr->level + 200;
	      bank_msg_print("You feel drugged.", EAT1NUM);
	      ident = TRUE;
	      break;
	    case 6:
	      ident = eat1_cure_poison();
	      break;
	    case 7:
	      ident = eat1_cure_blindness();
	      break;
	    case 8:
	      if (py.flags.afraid > 1) {
		    py.flags.afraid = 1;
		    ident = TRUE;
		  }
	      break;
	    case 9:
	      ident = eat1_cure_confusion();
	      break;
	    case 10:
	      ident = TRUE;
	      eat1_lose_str();
	      break;
	    case 11:
	      ident = TRUE;
	      eat1_lose_con();
	      break;
		/* 12 through 15 are not used */
	    case 16:
	      if (eat1_res_stat(A_STR)) {
		    bank_msg_print("You feel your strength returning.", EAT1NUM);
		    ident = TRUE;
		  }
	      break;
	    case 17:
	      if (eat1_res_stat(A_CON)) {
		    bank_msg_print("You feel your health returning.", EAT1NUM);
		    ident = TRUE;
		  }
	      break;
	    case 18:
	      if (eat1_res_stat(A_INT)) {
		    bank_msg_print("Your head spins a moment.", EAT1NUM);
		    ident = TRUE;
		  }
	      break;
	    case 19:
	      if (eat1_res_stat(A_WIS)) {
		    bank_msg_print("You feel your wisdom returning.", EAT1NUM);
		    ident = TRUE;
		  }
	      break;
	    case 20:
	      if (eat1_res_stat(A_DEX))	{
		    bank_msg_print("You feel more dextrous.", EAT1NUM);
		    ident = TRUE;
		  }
	      break;
	    case 21:
	      if (eat1_res_stat(A_CHR)) {
		    bank_msg_print("Your skin stops itching.", EAT1NUM);
		    ident = TRUE;
		  }
	      break;
	    case 22:
	      ident = eat1_hp_player(randint(6));
	      break;
	    case 23:
	      ident = eat1_hp_player(randint(12));
	      break;
	    case 24:
	      ident = eat1_hp_player(randint(18));
	      break;
        /* 25 is not used */
	    case 26:
	      ident = eat1_hp_player(eat1_damroll(3, 12));
	      break;
	    case 27:
	      eat1_take_hit(randint(18), "poisonous food.");
	      ident = TRUE;
	      break;
		  /* 28 through 30 are not used */
	    default:
	      bank_msg_print("Internal error in eat()", EAT1NUM);
	      break;
	    }
	  /* End of food actions. */
	}

    vbxe_bank(VBXE_OBJBANK);
    if (ident) {
	  if (!eat1_known1_p(i_ptr)) {
	    /* use identified it, gain experience */
	    /* round half-way case up */
	    py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;
	    bank_prt_experience(EAT1NUM);

	    eat1_identify(&item_val);
	    vbxe_bank(VBXE_OBJBANK);
	    i_ptr = &inventory[item_val];
	  }
	}
    else if (!eat1_known1_p(i_ptr))
	  eat1_sample(i_ptr);

    vbxe_bank(VBXE_OBJBANK);
    add_food(i_ptr->p1);

    py.flags.status &= ~(PY_WEAK|PY_HUNGRY);
    bank_prt_hunger(EAT1NUM);
    bank_desc_remain(item_val, EAT1NUM);
    bank_inven_destroy(item_val, EAT1NUM);
  }
}


/* Draws entire screen					-RAK-	*/
void __fastcall__ eat1_draw_cave(void)
{
  //clear_screen();
  bank_vbxe_clear(EAT1NUM);
  bank_prt_stat_block(EAT1NUM);
  bank_prt_map(EAT1NUM);
  bank_prt_depth(EAT1NUM);
}
