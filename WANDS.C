/* source/wands.c: wand code

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


#pragma code-name("WANDBANK")
#pragma data-name("WANDBANK")
#pragma rodata-name("WANDBANK")


/* Local Prototypes */
int8u __fastcall__ wands_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8u __fastcall__ wand_get_dir(char *prompt, int8u *dir);
char __fastcall__ wand_map_roguedir(char comval);
int8 __fastcall__ wand_bit_pos(int32u *test);
int8 __fastcall__ wand_object_offset(inven_type *t_ptr);
int8u __fastcall__ wand_known1_p(inven_type *i_ptr);
void __fastcall__ wand_sample(inven_type *i_ptr);
void __fastcall__ wand_desc_charges(int8u item_val);
int8u __fastcall__ wand_stat_adj(int8u stat);
int8u __fastcall__ wand_damroll(int8u num, int8u sides);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		WANDNUM

#undef	INBANK_MOD
#define INBANK_MOD			wand


int8u __fastcall__ wands_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

char __fastcall__ wand_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

int8u __fastcall__ wand_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

int8 __fastcall__ wand_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

int8u __fastcall__ wand_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

void __fastcall__ wand_sample(inven_type *i_ptr)
#include "inbank\sample.c"

int8 __fastcall__ wand_bit_pos(int32u *test)
#include "inbank\bit_pos.c"


/* Describe number of remaining charges.		-RAK-	*/
void __fastcall__ wand_desc_charges(int8u item_val)
{
  int8u rem_num;

  //if (known2_p(&inventory[item_val])) {
  if ((inventory[item_val].ident & ID_KNOWN2)) {
    rem_num = inventory[item_val].p1;
    sprintf(out_val1, "You have %d charges remaining.", rem_num);
    bank_msg_print(out_val1, WANDNUM);
  }
}


int8u __fastcall__ wand_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

int8u __fastcall__ wand_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"


/* Wands for the aiming. */
void __fastcall__ aim(void)
{
  int32u i;
  int8u item_val, j, k, l, chance, dir, ident;
  register inven_type *i_ptr;
  char spellname[25];					// to hold the spell name -SJ
  int8u ca;								// class_adj from spellbank -SJ


  free_turn_flag = TRUE;
  if (inven_ctr == 0)
    bank_msg_print("But you are not carrying anything.", WANDNUM);
  else if (!wands_find_range(TV_WAND, TV_NEVER, &j, &k))
    bank_msg_print("You are not carrying any wands.", WANDNUM);
  else if (bank_get_item(&item_val, "Aim which wand?", j, k, CNIL, "", WANDNUM)) {
    if (wand_get_dir(CNIL, &dir)) {
	  if (py.flags.confused > 0) {
	    bank_msg_print("You are confused.", WANDNUM);
	    do {
		  dir = randint(9);
		} while (dir == 5);
	  }

      free_turn_flag = FALSE;
	  ident = FALSE;

      vbxe_bank(VBXE_SPLBANK);
      ca = class_level_adj[py.misc.pclass][CLA_DEVICE];			// save class adjustment
      vbxe_bank(VBXE_OBJBANK);
      i_ptr = &inventory[item_val];

	  chance = py.misc.save + wand_stat_adj(A_INT) - i_ptr->level + (ca * py.misc.lev / 3);
	  if (py.flags.confused > 0)
	    chance = chance / 2;
	  if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1))
	    chance = USE_DEVICE; 			/* Give everyone a slight chance */
	  if (chance <= 0) chance = 1;
	  if (randint(chance) < USE_DEVICE)
	    bank_msg_print("You failed to use the wand properly.", WANDNUM);
	  else if (i_ptr->p1 > 0) {
	    i = i_ptr->flags;
	    (i_ptr->p1)--;
	    while (i != 0) {
		  j = wand_bit_pos(&i) + 1;
		  k = char_row;
		  l = char_col;

		  /* Wands */
		  switch(j) {
		    case 1:
		      bank_msg_print("A line of blue shimmering light appears.", WANDNUM);
		      bank_light_line(dir, char_row, char_col, WANDNUM);
		      ident = TRUE;
		      break;
		    case 2:
		      vbxe_bank(VBXE_SPLBANK);
		      strcpy(spellname, spell_names[8]);			// copy the spell name to the stack -SJ
		      vbxe_restore_bank();

		      bank_fire_bolt(GF_LIGHTNING, dir, k, l, wand_damroll(4, 8), spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 3:
		      vbxe_bank(VBXE_SPLBANK);
		      strcpy(spellname, spell_names[14]);			// copy the spell name to the stack -SJ
		      vbxe_restore_bank();

		      bank_fire_bolt(GF_FROST, dir, k, l, wand_damroll(6, 8), spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 4:
		      vbxe_bank(VBXE_SPLBANK);
		      strcpy(spellname, spell_names[22]);			// copy the spell name to the stack -SJ
		      vbxe_restore_bank();

		      bank_fire_bolt(GF_FIRE, dir, k, l, wand_damroll(9, 8), spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 5:
		      ident = bank_wall_to_mud(dir, k, l, WANDNUM);
		      break;
		    case 6:
		      ident = bank_poly_monster(dir, k, l, WANDNUM);
		      break;
		    case 7:
		      ident = bank_hp_monster(dir, k, l, -wand_damroll(4, 6), WANDNUM);
		      break;
		    case 8:
		      ident = bank_speed_monster(dir, k, l, 1, WANDNUM);
		      break;
		    case 9:
		      ident = bank_speed_monster(dir, k, l, -1, WANDNUM);
		      break;
		    case 10:
		      ident = bank_confuse_monster(dir, k, l, WANDNUM);
		      break;
		    case 11:
		      ident = bank_sleep_monster(dir, k, l, WANDNUM);
		      break;
		    case 12:
		      ident = bank_drain_life(dir, k, l);
		      break;
		    case 13:
		      ident = bank_td_destroy2(dir, k, l);
		      break;
		    case 14:
		      vbxe_bank(VBXE_SPLBANK);
		      strcpy(spellname, spell_names[0]);			// copy the spell name to the stack -SJ
		      vbxe_restore_bank();

		      bank_fire_bolt(GF_MAGIC_MISSILE, dir, k, l, wand_damroll(2, 6), spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 15:
		      ident = bank_build_wall(dir, k, l);
		      break;
		    case 16:
		      ident = bank_clone_monster(dir, k, l);
		      break;
		    case 17:
		      ident = bank_teleport_monster(dir, k, l, WANDNUM);
		      break;
		    case 18:
		      ident = bank_disarm_all(dir, k, l);
		      break;
		    case 19:
		      strcpy(spellname, "Lightning Ball");
		      bank_fire_ball(GF_LIGHTNING, dir, k, l, 32, spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 20:
		      strcpy(spellname, "Cold Ball");
		      bank_fire_ball(GF_FROST, dir, k, l, 48, spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 21:
		      vbxe_bank(VBXE_SPLBANK);
		      strcpy(spellname, spell_names[28]);			// copy the spell name to the stack -SJ
		      vbxe_restore_bank();

		      bank_fire_ball(GF_FIRE, dir, k, l, 72, spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 22:
		      vbxe_bank(VBXE_SPLBANK);
		      strcpy(spellname, spell_names[6]);			// copy the spell name to the stack -SJ
		      vbxe_restore_bank();

		      bank_fire_ball(GF_POISON_GAS, dir, k, l, 12, spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 23:
		      strcpy(spellname, "Acid Ball");
		      bank_fire_ball(GF_ACID, dir, k, l, 60, spellname, WANDNUM);
		      ident = TRUE;
		      break;
		    case 24:
		      i = 1L << (randint(23) - 1);
		      break;
		    default:
		      bank_msg_print("Internal error in wands()", WANDNUM);
		      break;
		  }
		  /* End of Wands. */
		}

        vbxe_bank(VBXE_OBJBANK);
	    if (ident) {
		  if (!wand_known1_p(i_ptr)) {
		    /* round half-way case up */
		    py.misc.exp += (i_ptr->level +(py.misc.lev >> 1)) / py.misc.lev;
		    bank_prt_experience(WANDNUM);

		    bank_identify(&item_val, WANDNUM);
		    i_ptr = &inventory[item_val];
		  }
		}
	    else if (!wand_known1_p(i_ptr))
		  wand_sample(i_ptr);

	    wand_desc_charges(item_val);
	  }
	  else {
	    bank_msg_print("The wand has no charges left.", WANDNUM);
	    //if (!known2_p(i_ptr))
	    if (!(i_ptr->ident & ID_KNOWN2))				// in-line known2_p code -SJ
		  //add_inscribe(i_ptr, ID_EMPTY);
		  i_ptr->ident |= ID_EMPTY;						// in-line add_inscribe code -SJ
	  }
	}
  }
}
