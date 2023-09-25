/* source/scrolls.c: scroll code

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


#pragma code-name("SCROLLBANK")
#pragma data-name("SCROLLBANK")
#pragma rodata-name("SCROLLBANK")


/* Local Prototypes */
int8u __fastcall__ scroll1_no_light(void);
void __fastcall__ scroll1_lite_spot(int8u y, int8u x);
int8u __fastcall__ scroll1_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8u __fastcall__ scroll1_enchant(int8 *plusses, int8 limit);
void __fastcall__ scroll1_invcopy(inven_type *to, int16u from_index);
int8 __fastcall__ scroll1_object_offset(inven_type *t_ptr);
int8u __fastcall__ scroll1_known1_p(inven_type *i_ptr);
void __fastcall__ scroll1_sample(inven_type *i_ptr);
int8 __fastcall__ scroll1_bit_pos(int32u *test);
int8u __fastcall__ scroll1_popt(void);
int8u __fastcall__ trap_creation(void);
int8u __fastcall__ door_creation(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		SCROLLNUM

#undef  INBANK_MOD
#define INBANK_MOD			scroll1


/* Returns true if player has no light			-RAK-	*/
int8u __fastcall__ scroll1_no_light(void)
#include "inbank\no_light.c"

/* Lights up given location				-RAK-	*/
void __fastcall__ scroll1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

/* Finds range of item in inventory list		-RAK-	*/
int8u __fastcall__ scroll1_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"


/* Enchants a plus onto an item. -RAK-	*/
/* int16 limit; maximum bonus allowed; usually 10, but weapon's maximum damage
		when enchanting melee weapons to damage */
int8u __fastcall__ scroll1_enchant(int8 *plusses, int8 limit)
{
  int8 chance;
  int8u res;


  if (limit <= 0) /* avoid randint(0) call */
    return(FALSE);

  chance = 0;
  res = FALSE;
  if (*plusses > 0) {
    chance = *plusses;
    if (randint(100) == 1) 				/* very rarely allow enchantment over limit */
	  chance = randint(chance) - 1;
  }
  if (randint(limit) > chance) {
    *plusses += 1;
    res = TRUE;
  }

  return(res);
}


void __fastcall__ scroll1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8 __fastcall__ scroll1_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

int8u __fastcall__ scroll1_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

void __fastcall__ scroll1_sample(inven_type *i_ptr)
#include "inbank\sample.c"

int8 __fastcall__ scroll1_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

int8u __fastcall__ scroll1_popt(void)
#include "inbank\popt.c"


/* Scrolls for the reading				-RAK-	*/
void __fastcall__ read_scroll(void)
{
  int32u i;
  int8u j, k, item_val, y, x;
  int8u tmp[6], flag, used_up;
  //bigvtype out_val, tmp_str;
  register int8u ident, l;
  register inven_type *i_ptr;
  //register struct misc *m_ptr;


  free_turn_flag = TRUE;
  if (py.flags.blind > 0)
    bank_msg_print("You can't see to read the scroll.", SCROLLNUM);
  else if (scroll1_no_light())
    bank_msg_print("You have no light to read by.", SCROLLNUM);
  else if (py.flags.confused > 0)
    bank_msg_print("You are too confused to read a scroll.", SCROLLNUM);
  else if (inven_ctr == 0)
    bank_msg_print("You are not carrying anything!", SCROLLNUM);
  else if (!scroll1_find_range(TV_SCROLL1, TV_SCROLL2, &j, &k))
    bank_msg_print ("You are not carrying any scrolls!", SCROLLNUM);
  else if (bank_get_item(&item_val, "Read which scroll?", j, k, CNIL, "", SCROLLNUM)) {
    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[item_val];
    free_turn_flag = FALSE;
    used_up = TRUE;
    i = i_ptr->flags;
    ident = FALSE;

    while (i != 0) {
	  j = scroll1_bit_pos(&i) + 1;
	  if (i_ptr->tval == TV_SCROLL2)
	    j += 32;

	  /* Scrolls. */
	  switch(j) {
	    case 1:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING) {
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1, "Your %s glows faintly!", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);

		    vbxe_bank(VBXE_OBJBANK);
		    if (scroll1_enchant(&i_ptr->tohit, 10)) {
		      i_ptr->flags &= ~TR_CURSED;
		      bank_calc_bonuses(SCROLLNUM);
		    }
		    else
		      bank_msg_print("The enchantment fails.", SCROLLNUM);
		    ident = TRUE;
		  }
	      break;
	    case 2:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING) {
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1, "Your %s glows faintly!", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);

		    vbxe_bank(VBXE_OBJBANK);
		    if ((i_ptr->tval >= TV_HAFTED)&&(i_ptr->tval <= TV_DIGGING))
		      j = i_ptr->damage[0] * i_ptr->damage[1];
		    else /* Bows' and arrows' enchantments should not be limited by their low base damages */
		      j = 10;
		    if (scroll1_enchant(&i_ptr->todam, j)) {
		      i_ptr->flags &= ~TR_CURSED;
		      bank_calc_bonuses(SCROLLNUM);
		    }
		    else
		      bank_msg_print("The enchantment fails.", SCROLLNUM);
		    ident = TRUE;
		  }
	      break;
	    case 3:
	      k = 0;
	      l = 0;
	      if (inventory[INVEN_BODY].tval != TV_NOTHING)
		    tmp[k++] = INVEN_BODY;
	      if (inventory[INVEN_ARM].tval != TV_NOTHING)
		    tmp[k++] = INVEN_ARM;
	      if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		    tmp[k++] = INVEN_OUTER;
	      if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HANDS;
	      if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HEAD;
	      /* also enchant boots */
	      if (inventory[INVEN_FEET].tval != TV_NOTHING)
		    tmp[k++] = INVEN_FEET;

	      if (k > 0)  l = tmp[randint(k)-1];
	      if (TR_CURSED & inventory[INVEN_BODY].flags)
		    l = INVEN_BODY;
	      else if (TR_CURSED & inventory[INVEN_ARM].flags)
		    l = INVEN_ARM;
	      else if (TR_CURSED & inventory[INVEN_OUTER].flags)
		    l = INVEN_OUTER;
	      else if (TR_CURSED & inventory[INVEN_HEAD].flags)
		    l = INVEN_HEAD;
	      else if (TR_CURSED & inventory[INVEN_HANDS].flags)
		    l = INVEN_HANDS;
	      else if (TR_CURSED & inventory[INVEN_FEET].flags)
		    l = INVEN_FEET;

	      if (l > 0) {
		    i_ptr = &inventory[l];
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1, "Your %s glows faintly!", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);
		    if (scroll1_enchant(&i_ptr->toac, 10)) {
		      i_ptr->flags &= ~TR_CURSED;
		      bank_calc_bonuses(SCROLLNUM);
		    }
		    else
		      bank_msg_print("The enchantment fails.", SCROLLNUM);
		    ident = TRUE;
		  }
	      break;
	    case 4:
	      bank_msg_print("This is an identify scroll.", SCROLLNUM);
	      ident = TRUE;
	      used_up = bank_ident_spell(SCROLLNUM);

	      /* The identify may merge objects, causing the identify scroll
		     to move to a different place.	Check for that here.  It can
		     move arbitrarily far if an identify scroll was used on
		     another identify scroll, but it always moves down. */
		  vbxe_bank(VBXE_OBJBANK);													// CHECK: might still be in bank
	      while (i_ptr->tval != TV_SCROLL1 || i_ptr->flags != 0x00000008) {
		    --item_val;
		    i_ptr = &inventory[item_val];
		  }
	      break;
	    case 5:
	      if (bank_remove_curse(SCROLLNUM)) {
		    bank_msg_print("You feel as if someone is watching over you.", SCROLLNUM);
		    ident = TRUE;
		  }
	      break;
	    case 6:
	      ident = bank_light_area(char_row, char_col, SCROLLNUM);
	      break;
	    case 7:
	      for (k = 0; k < randint(3); ++k) {
		    y = char_row;
		    x = char_col;
		    ident |= bank_summon_monster(&y, &x, FALSE, SCROLLNUM);
		  }
	      break;
	    case 8:
	      bank_teleport(10, SCROLLNUM);
	      ident = TRUE;
	      break;
	    case 9:
	      bank_teleport(100, SCROLLNUM);
	      ident = TRUE;
	      break;
	    case 10:
	      dun_level += (-3) + 2*randint(2);				// might make dun_level wrap around -SJ
	      if (dun_level < 1)
		    dun_level = 1;
		  else if (dun_level > 200)						// check for wrap around -SJ
		    dun_level = 99;
	      new_level_flag = TRUE;
	      ident = TRUE;
	      break;
	    case 11:
	      if (py.flags.confuse_monster == 0) {
		    bank_msg_print("Your hands begin to glow.", SCROLLNUM);
		    py.flags.confuse_monster = TRUE;
		    ident = TRUE;
		  }
	      break;
	    case 12:
	      ident = TRUE;
	      bank_map_area(SCROLLNUM);
	      break;
	    case 13:
	      ident = bank_sleep_monsters1(char_row, char_col, SCROLLNUM);
	      break;
	    case 14:
	      ident = TRUE;
	      bank_warding_glyph(SCROLLNUM);
	      break;
	    case 15:
	      ident = bank_detect_treasure(SCROLLNUM);
	      break;
	    case 16:
	      ident = bank_detect_object(SCROLLNUM);
	      break;
	    case 17:
	      ident = bank_detect_trap(SCROLLNUM);
	      break;
	    case 18:
	      ident = bank_detect_sdoor(SCROLLNUM);
	      break;
	    case 19:
	      bank_msg_print("This is a mass genocide scroll.", SCROLLNUM);
	      ident = bank_mass_genocide(SCROLLNUM);
	      break;
	    case 20:
	      ident = bank_detect_invisible(SCROLLNUM);
	      break;
	    case 21:
	      ident = bank_aggravate_monster(20, SCROLLNUM);
	      if (ident)
		    bank_msg_print("There is a high pitched humming noise.", SCROLLNUM);
	      break;
	    case 22:
	      ident = trap_creation();
	      break;
	    case 23:
	      ident = bank_td_destroy(SCROLLNUM);
	      break;
	    case 24:
	      ident = door_creation();
	      break;
	    case 25:
	      bank_msg_print("This is a Recharge-Item scroll.", SCROLLNUM);
	      ident = TRUE;
	      used_up = bank_recharge(60, SCROLLNUM);
	      break;
	    case 26:
	      bank_msg_print("This is a genocide scroll.", SCROLLNUM);
	      ident = bank_genocide(SCROLLNUM);
	      break;
	    case 27:
	      ident = bank_unlight_area(char_row, char_col, SCROLLNUM);
	      break;
	    case 28:
	      ident = bank_protect_evil(SCROLLNUM);
	      break;
	    case 29:
	      ident = TRUE;
	      bank_create_food(SCROLLNUM);
	      break;
	    case 30:
	      ident = bank_dispel_creature(CD_UNDEAD, 60, SCROLLNUM);
	      break;
	    case 33:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING) {
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1, "Your %s glows brightly!", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);
		    flag = FALSE;

		    vbxe_bank(VBXE_OBJBANK);
		    for (k = 0; k < randint(2); ++k)
		      if (scroll1_enchant(&i_ptr->tohit, 10))
		        flag = TRUE;

		    if ((i_ptr->tval >= TV_HAFTED)&&(i_ptr->tval <= TV_DIGGING))
		      j = i_ptr->damage[0] * i_ptr->damage[1];
		    else /* Bows' and arrows' enchantments should not be limited by their low base damages */
		      j = 10;

		    for (k = 0; k < randint(2); ++k)
		      if (scroll1_enchant(&i_ptr->todam, j))
		        flag = TRUE;

		    if (flag) {
		      i_ptr->flags &= ~TR_CURSED;
		      bank_calc_bonuses(SCROLLNUM);
		    }
		    else
		      bank_msg_print("The enchantment fails.", SCROLLNUM);

		    ident = TRUE;
		  }
	      break;
	    case 34:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING) {
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1,"Your %s glows black, fades.", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);

		    vbxe_bank(VBXE_OBJBANK);
		    //unmagic_name(i_ptr);
		    i_ptr->name2 = SN_NULL;						// unmagic_name code in-line -SJ
		    i_ptr->tohit = -randint(5) - randint(5);
		    i_ptr->todam = -randint(5) - randint(5);

		    /* Must call py_bonuses() before set (clear) flags, and
		       must call calc_bonuses() after set (clear) flags, so that
		       all attributes will be properly turned off. */
		    bank_py_bonuses(i_ptr, -1, SCROLLNUM);
		    i_ptr->flags = TR_CURSED;
		    bank_calc_bonuses(SCROLLNUM);
		    ident = TRUE;
		  }
	      break;
	    case 35:
	      k = 0;
	      l = 0;
	      if (inventory[INVEN_BODY].tval != TV_NOTHING)
		    tmp[k++] = INVEN_BODY;
	      if (inventory[INVEN_ARM].tval != TV_NOTHING)
		    tmp[k++] = INVEN_ARM;
	      if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		    tmp[k++] = INVEN_OUTER;
	      if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HANDS;
	      if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HEAD;
	      /* also enchant boots */
	      if (inventory[INVEN_FEET].tval != TV_NOTHING)
		    tmp[k++] = INVEN_FEET;

	      if (k > 0) l = tmp[randint(k)-1];
	      if (TR_CURSED & inventory[INVEN_BODY].flags)
		    l = INVEN_BODY;
	      else if (TR_CURSED & inventory[INVEN_ARM].flags)
		    l = INVEN_ARM;
	      else if (TR_CURSED & inventory[INVEN_OUTER].flags)
		    l = INVEN_OUTER;
	      else if (TR_CURSED & inventory[INVEN_HEAD].flags)
		    l = INVEN_HEAD;
	      else if (TR_CURSED & inventory[INVEN_HANDS].flags)
		    l = INVEN_HANDS;
	      else if (TR_CURSED & inventory[INVEN_FEET].flags)
		    l = INVEN_FEET;

	      if (l > 0) {
		    i_ptr = &inventory[l];
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1,"Your %s glows brightly!", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);
		    flag = FALSE;

		    vbxe_bank(VBXE_OBJBANK);
		    for (k = 0; k < randint(2) + 1; ++k)
		      if (scroll1_enchant(&i_ptr->toac, 10))
		        flag = TRUE;

		    if (flag) {
		      i_ptr->flags &= ~TR_CURSED;
		      bank_calc_bonuses(SCROLLNUM);
		    }
		    else
		      bank_msg_print("The enchantment fails.", SCROLLNUM);

		    ident = TRUE;
		  }
	      break;
	    case 36:
	      if ((inventory[INVEN_BODY].tval != TV_NOTHING) && (randint(4) == 1))
		    k = INVEN_BODY;
	      else if ((inventory[INVEN_ARM].tval != TV_NOTHING) && (randint(3) ==1))
		    k = INVEN_ARM;
	      else if ((inventory[INVEN_OUTER].tval != TV_NOTHING) && (randint(3) ==1))
		    k = INVEN_OUTER;
	      else if ((inventory[INVEN_HEAD].tval != TV_NOTHING) && (randint(3) ==1))
		    k = INVEN_HEAD;
	      else if ((inventory[INVEN_HANDS].tval != TV_NOTHING) && (randint(3) ==1))
		    k = INVEN_HANDS;
	      else if ((inventory[INVEN_FEET].tval != TV_NOTHING) && (randint(3) ==1))
		    k = INVEN_FEET;
	      else if (inventory[INVEN_BODY].tval != TV_NOTHING)
	        k = INVEN_BODY;
	      else if (inventory[INVEN_ARM].tval != TV_NOTHING)
		    k = INVEN_ARM;
	      else if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		    k = INVEN_OUTER;
	      else if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		    k = INVEN_HEAD;
	      else if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		    k = INVEN_HANDS;
	      else if (inventory[INVEN_FEET].tval != TV_NOTHING)
		    k = INVEN_FEET;
	      else
		    k = 0;

	      if (k > 0) {
		    i_ptr = &inventory[k];
		    bank_objdes(out_val2, i_ptr, FALSE, SCROLLNUM);
		    sprintf(out_val1,"Your %s glows black, fades.", out_val2);
		    bank_msg_print(out_val1, SCROLLNUM);

		    vbxe_bank(VBXE_OBJBANK);
		    //unmagic_name(i_ptr);
		    i_ptr->name2 = SN_NULL;						// unmagic_name code in-line -SJ
		    i_ptr->flags = TR_CURSED;
		    i_ptr->toac = -randint(5) - randint(5);
		    bank_calc_bonuses(SCROLLNUM);
		    ident = TRUE;
		  }
	      break;
	    case 37:
	      ident = FALSE;
	      for (k = 0; k < randint(3); ++k) {
		    y = char_row;
		    x = char_col;
		    ident |= bank_summon_undead(&y, &x, SCROLLNUM);
		  }
	      break;
	    case 38:
	      ident = TRUE;
	      //bless(randint(12)+6);
	      py.flags.blessed += randint(12)+6;					// in-lined bless code -SJ
	      break;
	    case 39:
	      ident = TRUE;
	      //bless(randint(24)+12);
	      py.flags.blessed += randint(24)+12;					// in-lined bless code -SJ
	      break;
	    case 40:
	      ident = TRUE;
	      //bless(randint(48)+24);
	      py.flags.blessed += randint(48)+24;					// in-lined bless code -SJ
	      break;
	    case 41:
	      ident = TRUE;
	      if (py.flags.word_recall == 0)
		    py.flags.word_recall = 25 + randint(30);
	      bank_msg_print("The air about you becomes charged.", SCROLLNUM);
	      break;
	    case 42:
	      bank_destroy_area(char_row, char_col, SCROLLNUM);
	      ident = TRUE;
	      break;
	    default:
	      bank_msg_print("Internal error in scroll()", SCROLLNUM);
	      break;
	    }
	  /* End of Scrolls. */
	}

    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[item_val];
    if (ident) {
	  if (!scroll1_known1_p(i_ptr)) {
	    //m_ptr = &py.misc;
	    /* round half-way case up */
	    py.misc.exp += (i_ptr->level +(py.misc.lev >> 1)) / py.misc.lev;
	    bank_prt_experience(SCROLLNUM);

	    bank_identify(&item_val, SCROLLNUM);
	    i_ptr = &inventory[item_val];						// CHECK: objbank still in?
	  }
	}
    else if (!scroll1_known1_p(i_ptr))
	  scroll1_sample(i_ptr);

    if (used_up) {
	  bank_desc_remain(item_val, SCROLLNUM);
	  bank_inven_destroy(item_val, SCROLLNUM);
	}
  }
}


/* Surround the fool with traps (chuckle)		-RAK-	*/
int8u __fastcall__ trap_creation(void)
{
  int8u i, j, trap;
  register cave_type *c_ptr;
  int8u tptr;


  trap = TRUE;
  for (i = char_row-1; i <= char_row+1; ++i)
    for (j = char_col-1; j <= char_col+1; ++j) {
	  /* Don't put a trap under the player, since this can lead to
	     strange situations, e.g. falling through a trap door while
	     trying to rest, setting off a falling rock trap and ending
	     up under the rock.  */
	  if (i == char_row && j == char_col)
	    continue;

	  //c_ptr = &cave[i][j];
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(i, j);
	  if (c_ptr->fval <= MAX_CAVE_FLOOR) {
	    if (c_ptr->tptr != 0)
	      bank_delete_object(i, j, SCROLLNUM);

	    bank_place_trap(i, j, randint(MAX_TRAP)-1, SCROLLNUM);

	    /* don't let player gain exp from the newly created traps */
	    tptr = c_ptr->tptr;
	    vbxe_bank(VBXE_OBJBANK);
	    t_list[tptr].p1 = 0;
	    /* open pits are immediately visible, so call lite_spot */
	    scroll1_lite_spot(i, j);
	}
  }

  return(trap);
}


/* Surround the player with doors.			-RAK-	*/
int8u __fastcall__ door_creation(void)
{
  int8u i, j, door;
  int8u k;
  register cave_type *c_ptr;


  door = FALSE;
  for (i = char_row-1; i <= char_row+1; ++i)
    for (j = char_col-1; j <=  char_col+1; ++j)
      if ((i != char_row) || (j != char_col)) {
	    vbxe_bank(VBXE_CAVBANK);
	    //c_ptr = &cave[i][j];
	    c_ptr = CAVE_ADR(i,j);
	    if (c_ptr->fval <= MAX_CAVE_FLOOR) {
	      door = TRUE;
	      if (c_ptr->tptr != 0)
		    bank_delete_object(i, j, SCROLLNUM);

	      k = scroll1_popt();
	      c_ptr->fval = BLOCKED_FLOOR;
	      c_ptr->tptr = k;
	      scroll1_invcopy(&t_list[k], OBJ_CLOSED_DOOR);
	      scroll1_lite_spot(i, j);
	    }
	  }

  return(door);
}

