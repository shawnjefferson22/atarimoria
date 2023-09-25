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
#pragma code-name("DUNGEON5BANK")
#pragma rodata-name("DUNGEON5BANK")
#pragma data-name("DUNGEON5BANK")


/* Prototypes of functions copied into this bank */
int8u __fastcall__ dungeon5_get_check(char *prompt);
void __fastcall__ dungeon5_change_speed(int8 num);
void __fastcall__ dungeon5_end_find(void);
void __fastcall__ dungeon5_lite_spot(int8u y, int8u x);
int16u __fastcall__ dungeon5_damroll(int8u num, int8u sides);
int16u __fastcall__ dungeon5_pdamroll(int8u *array);
void __fastcall__ dungeon5_search_off(void);
void __fastcall__ dungeon5_rest_off(void);
void __fastcall__ dungeon5_disturb(int8u s, int8u l);
int8u __fastcall__ dungeon5_test_hit(int8u bth, int8u level, int8 pth, int8u ac, int8u attack_type);
void __fastcall__ dungeon5_take_hit(int8u damage, char *hit_from);
int8 __fastcall__ dungeon5_object_offset(inven_type *t_ptr);
int8u __fastcall__ dungeon5_known1_p(inven_type *i_ptr);
int8u __fastcall__ dungeon5_inven_check_num (inven_type *t_ptr);
int16u __fastcall__ dungeon5_weight_limit(void);
int8u __fastcall__ inven_check_weight(inven_type *i_ptr);
int8u __fastcall__ dungeon5_distance(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ dungeon5_move_rec(int8u y1, int8u x1, int8u y2, int8u x2);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN5NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon5


int8u __fastcall__ dungeon5_get_check(char *prompt)
#include "inbank\get_check.c"

void __fastcall__ dungeon5_change_speed(int8 num)
#include "inbank\change_speed.c"


void __fastcall__ dungeon5_end_find(void)
{
  if (find_flag) {
    find_flag = FALSE;
    bank_move_light(char_row, char_col, char_row, char_col, DUN5NUM);
  }
}

void __fastcall__ dungeon5_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

void __fastcall__ dungeon5_change_trap(int8u y, int8u x)
#include "inbank\change_trap.c"

int16u __fastcall__ dungeon5_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int16u __fastcall__ dungeon5_pdamroll(int8u *array)
#include "inbank\pdamroll.c"

void __fastcall__ dungeon5_search_off(void)
#include "inbank\search_off.c"

void __fastcall__ dungeon5_rest_off(void)
#include "inbank\rest_off.c"

void __fastcall__ dungeon5_disturb(int8u s, int8u l)
#include "inbank\disturb.c"

int8u __fastcall__ dungeon5_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

void __fastcall__ dungeon5_move_rec(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\move_rec.c"

int8u __fastcall__ dungeon5_test_hit(int8u bth, int8u level, int8 pth, int8u ac, int8u attack_type)
#include "inbank\test_hit.c"

void __fastcall__ dungeon5_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"

int8 __fastcall__ dungeon5_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

int8u __fastcall__ dungeon5_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"


/* this code must be identical to the inven_carry() code below */
int8u __fastcall__ dungeon5_inven_check_num (inven_type *t_ptr)
{
  register int8u i;
  int8u ret;


  vbxe_bank(VBXE_OBJBANK);

  ret = FALSE;
  if (inven_ctr < INVEN_WIELD)
    ret = TRUE;
  else if (t_ptr->subval >= ITEM_SINGLE_STACK_MIN) {
	ret = FALSE;
    for (i = 0; i < inven_ctr; ++i)
      if (inventory[i].tval == t_ptr->tval && inventory[i].subval == t_ptr->subval &&      // inventory is in OBJBANK
	  /* make sure the number field doesn't overflow */
	  ((int)inventory[i].number + (int)t_ptr->number < 256) &&
	  /* they always stack (subval < 192), or else they have same p1 */
	  ((t_ptr->subval < ITEM_GROUP_MIN) || (inventory[i].p1 == t_ptr->p1))
	  /* only stack if both or neither are identified */
	  && (dungeon5_known1_p(&inventory[i]) == dungeon5_known1_p(t_ptr)))
	    ret = TRUE;
  }

  vbxe_restore_bank();
  return(ret);
}


/* Computes current weight limit			-RAK-	*/
int16u __fastcall__ dungeon5_weight_limit(void)
{
  register int16u weight_cap;


  weight_cap = py.stats.use_stat[A_STR] * PLAYER_WEIGHT_CAP + py.misc.wt;
  if (weight_cap > 3000) weight_cap = 3000;
  return(weight_cap);
}


/* return FALSE if picking up an object would change the players speed */
int8u __fastcall__ inven_check_weight(inven_type *i_ptr)
{
  register int16u i, new_inven_weight;


  i = dungeon5_weight_limit();
  new_inven_weight = i_ptr->number*i_ptr->weight + inven_weight;
  if (i < new_inven_weight)
    i = new_inven_weight / (i + 1);
  else
    i = 0;

  if (pack_heavy != i)
    return FALSE;
  else
    return TRUE;
}


/* Player hit a trap.	(Chuckle)			-RAK-	*/
void __fastcall__ hit_trap(int8u y, int8u x)
{
  int8u i, num, dam;
  int8u ty, tx;
  register inven_type *t_ptr;
  //bigvtype tmp;						// use out_val1


  dungeon5_end_find();
  dungeon5_change_trap(y, x);

  vbxe_bank(VBXE_CAVBANK);				// get and store tptr
  i = (CAVE_ADR(y, x))->tptr;

  vbxe_bank(VBXE_OBJBANK);
  t_ptr = &t_list[i];
  dam = (int8u) dungeon5_pdamroll(t_ptr->damage);			// traps should only be max int8u -SJ

  switch(t_ptr->subval) {
    case 1:  /* Open pit*/
      bank_msg_print("You fell into a pit!", DUN5NUM);
      if (py.flags.ffall)
	    bank_msg_print("You gently float down.", DUN5NUM);
      else {
	    bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	    dungeon5_take_hit(dam, out_val1);
	  }
      break;
    case 2: /* Arrow trap*/
      if (dungeon5_test_hit(125, 0, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT)) {
	    bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	    dungeon5_take_hit(dam, out_val1);
	    bank_msg_print("An arrow hits you.", DUN5NUM);
	  }
      else
	    bank_msg_print("An arrow barely misses you.", DUN5NUM);
      break;
    case 3: /* Covered pit*/
      bank_msg_print("You fell into a covered pit.", DUN5NUM);
      if (py.flags.ffall)
	    bank_msg_print("You gently float down.", DUN5NUM);
      else {
	    bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	    dungeon5_take_hit(dam, out_val1);
      }
      bank_place_trap(y, x, 0, DUN5NUM);
      break;
    case 4: /* Trap door*/
      bank_msg_print("You fell through a trap door!", DUN5NUM);
      new_level_flag = TRUE;
      ++dun_level;
      if (py.flags.ffall)
	    bank_msg_print("You gently float down.", DUN5NUM);
      else {
	    bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	    dungeon5_take_hit(dam, out_val1);
	  }
      /* Force the messages to display before starting to generate the next level.  */
      bank_msg_print("", DUN5NUM);
      break;
    case 5: /* Sleep gas*/
      if (py.flags.paralysis == 0) {
	    bank_msg_print("A strange white mist surrounds you!", DUN5NUM);
	    if (py.flags.free_act)
	      bank_msg_print("You are unaffected.", DUN5NUM);
	    else {
	      bank_msg_print("You fall asleep.", DUN5NUM);
	      py.flags.paralysis += randint(10) + 4;
	    }
	  }
      break;
    case 6: /* Hid Obj*/
      bank_delete_object(y, x, DUN5NUM);
      bank_place_object(y, x, DUN5NUM);
      bank_msg_print("Hmmm, there was something under this rock.", DUN5NUM);
      break;
    case 7:  /* STR Dart*/
      if (dungeon5_test_hit(125, 0, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT)) {
	    if (!py.flags.sustain_str) {
	      bank_dec_stat(A_STR, DUN5NUM);
	      bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	      dungeon5_take_hit(dam, out_val1);
	      bank_msg_print("A small dart weakens you!", DUN5NUM);
	    }
	    else
	      bank_msg_print("A small dart hits you.", DUN5NUM);
	  }
      else
	    bank_msg_print("A small dart barely misses you.", DUN5NUM);
      break;
    case 8: /* Teleport*/
      teleport_flag = TRUE;
      bank_msg_print("You hit a teleport trap!", DUN5NUM);
      /* Light up the teleport trap, before we teleport away.  */
      bank_move_light(y, x, y, x, DUN5NUM);
      break;
    case 9: /* Rockfall*/
      dungeon5_take_hit(dam, "a falling rock");
      bank_delete_object(y, x, DUN5NUM);
      bank_place_rubble(y, x, DUN5NUM);
      bank_msg_print("You are hit by falling rock.", DUN5NUM);
      break;
    case 10: /* Corrode gas*/
      /* Makes more sense to print the message first, then damage an object.  */
      bank_msg_print("A strange red gas surrounds you.", DUN5NUM);
      strcpy(out_val1, "corrosion gas");			// need to copy the string out of cart bank
      bank_corrode_gas(out_val1, DUN5NUM);
      //corrode_gas("corrosion gas");
      break;
    case 11: /* Summon mon*/
      bank_delete_object(y, x, DUN5NUM);	/* Rune disappears.    */
      num = 2 + randint (3);
      for (i = 0; i < num; ++i) {
	    ty = y;
	    tx = x;
	    bank_summon_monster(&ty, &tx, FALSE, DUN5NUM);
	  }
      break;
    case 12: /* Fire trap*/
      bank_msg_print("You are enveloped in flames!", DUN5NUM);
      strcpy(out_val1, "a fire trap");				// copy string out of cart bank to global var
      bank_fire_dam(dam, out_val1, DUN5NUM);
      //fire_dam(dam, "a fire trap");
      break;
    case 13: /* Acid trap*/
      bank_msg_print("You are splashed with acid!", DUN5NUM);
      strcpy(out_val1, "an acid trap");				// copy string out of cart bank to global var
      bank_acid_dam(dam, out_val1, DUN5NUM);
      //acid_dam(dam, "an acid trap");
      break;
    case 14: /* Poison gas*/
      bank_msg_print("A pungent green gas surrounds you!", DUN5NUM);
      strcpy(out_val1, "a poison gas trap");		// copy string out of cart bank to global var
      bank_poison_gas(dam, out_val1, DUN5NUM);
      //poison_gas(dam, "a poison gas trap");
      break;
    case 15: /* Blind Gas */
      bank_msg_print("A black gas surrounds you!", DUN5NUM);
      py.flags.blind += randint(50) + 50;
      break;
    case 16: /* Confuse Gas*/
      bank_msg_print("A gas of scintillating colors surrounds you!", DUN5NUM);
      py.flags.confused += randint(15) + 15;
      break;
    case 17: /* Slow Dart*/
      if (dungeon5_test_hit(125, 0, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT)) {
	    bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	    dungeon5_take_hit(dam, out_val1);
	    bank_msg_print("A small dart hits you!", DUN5NUM);
	    if (py.flags.free_act)
	      bank_msg_print("You are unaffected.", DUN5NUM);
	    else
	      py.flags.slow += randint(20) + 10;
	  }
      else
	    bank_msg_print("A small dart barely misses you.", DUN5NUM);
      break;
    case 18: /* CON Dart*/
      if (dungeon5_test_hit(125, 0, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT)) {
	    if (!py.flags.sustain_con) {
	      bank_dec_stat(A_CON, DUN5NUM);
	      bank_objdes(out_val1, t_ptr, TRUE, DUN5NUM);
	      dungeon5_take_hit(dam, out_val1);
	      bank_msg_print("A small dart saps your health!", DUN5NUM);
	    }
	    else
	      bank_msg_print("A small dart hits you.", DUN5NUM);
	  }
      else
	    bank_msg_print("A small dart barely misses you.", DUN5NUM);
      break;
    case 19: /*Secret Door*/
      break;
    case 99: /* Scare Mon*/
      break;

      /* Town level traps are special,	the stores.	*/
    case 101: /* General    */
      bank_enter_store(0);
      break;
    case 102: /* Armory	    */
      bank_enter_store(1);
      break;
    case 103: /* Weaponsmith*/
      bank_enter_store(2);
      break;
    case 104: /* Temple	    */
      bank_enter_store(3);
      break;
    case 105: /* Alchemy    */
      bank_enter_store(4);
      break;
    case 106: /* Magic-User */
      bank_enter_store(5);
      break;
    case 107: /* Pub */						// only accessible with enhancements on -SJ
      bank_enter_tavern();
      break;
    case 108: /* Guild */					// only accessible with enhancements on -SJ
      bank_enter_guild();
      break;

    default:
      bank_msg_print("Unknown trap value.", DUN5NUM);
      break;
    }
}


/* Player is on an object.  Many things can happen based -RAK-	*/
/* on the TVAL of the object.  Traps are set off, money and most */
/* objects are picked up.  Some objects, such as open doors, just*/
/* sit there.						       */
void __fastcall__ carry(int8u y, int8u x, int8u pickup)
{
  register int8u locn, i;
  //bigvtype out_val, tmp_str;				// use out_val1 and out_val2
  register inven_type *i_ptr;
  register int8u t;


  vbxe_bank(VBXE_CAVBANK);
  t = cave[y][x].tptr;						// make a local copy of tptr

  vbxe_bank(VBXE_OBJBANK);
  i_ptr = &t_list[t];
  //i = t_list[t].tval;
  i = i_ptr->tval;

  if (i <= TV_MAX_PICK_UP) {
      dungeon5_end_find();
      /* There's GOLD in them thar hills!      */
      if (i == TV_GOLD) {
	    py.misc.au += i_ptr->cost;
	    bank_objdes(out_val1, i_ptr, TRUE, DUN5NUM);
	    sprintf(out_val2, "You have found %ld gold pieces worth of %s", i_ptr->cost, out_val1);
	    bank_prt_gold(DUN5NUM);
	    bank_delete_object(y, x, DUN5NUM);
	    bank_msg_print(out_val2, DUN5NUM);
	  }
      else {
	    if (dungeon5_inven_check_num(i_ptr))	   // Too many objects?
	    {			    						   /* Okay,  pick it up */
	      if (pickup && prompt_carry_flag) {
		    bank_objdes(out_val1, i_ptr, TRUE, DUN5NUM);
		    /* change the period to a question mark */
		    out_val1[strlen(out_val1)-1] = '?';
		    sprintf(out_val2, "Pick up %s", out_val1);
		    pickup = dungeon5_get_check(out_val2);
		  }
	      /* Check to see if it will change the players speed. */
	      if (pickup && !inven_check_weight(i_ptr)) {
		    bank_objdes(out_val1, i_ptr, TRUE, DUN5NUM);
		    /* change the period to a question mark */
		    out_val1[strlen(out_val1)-1] = '?';
		    sprintf(out_val2, "Exceed your weight limit to pick up %s", out_val1);
		    pickup = dungeon5_get_check(out_val2);
		  }
	      /* Attempt to pick up an object.	       */
	      if (pickup) {
		    locn = bank_inven_carry(i_ptr, DUN5NUM);
		    bank_objdes(out_val1, &inventory[locn], TRUE, DUN5NUM);
		    sprintf(out_val2, "You have %s (%c)", out_val1, locn+'a');
		    bank_msg_print(out_val2, DUN5NUM);
		    bank_delete_object(y, x, DUN5NUM);
		  }
	    }
	    else {
	      bank_objdes(out_val1, i_ptr, TRUE, DUN5NUM);
	      sprintf(out_val2, "You can't carry %s", out_val1);
	      bank_msg_print(out_val2, DUN5NUM);
	    }
	}
    }
  /* OOPS!				   */
  else if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP || i == TV_STORE_DOOR)
    hit_trap(y, x);
}


/* Teleport the player to a new location		-RAK-	*/
void __fastcall__ teleport(int8u dis)
{
  register int8u y, x, i, j;
  cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  do {
    y = randint(cur_height) - 1;
    x = randint(cur_width) - 1;

    //while (dungeon5_distance(y, x, char_row, char_col) > dis) {
    while ((dungeon5_distance(y, x, char_row, char_col) > dis) && ((y > cur_height) || (x > cur_width))) {
	  y += ((char_row-y)/2);
	  x += ((char_col-x)/2);
	}

	c_ptr = CAVE_ADR(y, x);
  } while ((c_ptr->fval >= MIN_CLOSED_SPACE) || (c_ptr->cptr >= 2));

  dungeon5_move_rec(char_row, char_col, y, x);
  for (i = char_row-1; i <= char_row+1; ++i)
    for (j = char_col-1; j <= char_col+1; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  (CAVE_ADR(i,j))->flags &= ~CAVE_TL;
	  dungeon5_lite_spot(i, j);
    }

  dungeon5_lite_spot(char_row, char_col);
  char_row = y;
  char_col = x;
  bank_check_view(DUN5NUM);
  bank_creatures(FALSE, DUN5NUM);
  teleport_flag = FALSE;
}


/* Add a comment to an object description.		-CJS- */
void __fastcall__ scribe_object(void)
{
  int8u item_val, j;
  //vtype out_val, tmp_str;


  if (inven_ctr > 0 || equip_ctr > 0) {
    if (bank_get_item(&item_val, "Which one? ", 0, inven_ctr-2, CNIL, CNIL, DUN5NUM)) {
    //if (bank_get_item(&item_val, "Which one? ", 0, INVEN_ARRAY_SIZE, CNIL, CNIL, DUN5NUM)) {
	  bank_objdes(out_val1, &inventory[item_val], TRUE, DUN5NUM);
	  sprintf(out_val2, "Inscribing %s", out_val1);
	  bank_msg_print(out_val2, DUN5NUM);

	  vbxe_bank(VBXE_OBJBANK);

	  if (inventory[item_val].inscrip[0] != '\0')
	    sprintf(out_val2, "Replace %s New inscription:", inventory[item_val].inscrip);
	  else
	    strcpy(out_val2, "Inscription: ");
	  j = 78 - strlen(out_val1);
	  if (j > 24)
	    j = 12;
	  bank_prt(out_val2, 0, 0, DUN5NUM);
	  if (bank_get_string(out_val2, 0, strlen(out_val2), j, DUN5NUM)) {
	    //inscribe(&inventory[item_val], out_val);
	    strcpy(inventory[item_val].inscrip, out_val2);					// in-line inscribe code -SJ

        strcpy(out_val1, "Inscription: ");
	    strcat(out_val1, out_val2);
	    bank_prt(out_val1, 0, 0, DUN5NUM);								// reprint inscription to user -SJ
      }
	}
  }
  else
    bank_msg_print("You are not carrying anything to inscribe.", DUN5NUM);
}

