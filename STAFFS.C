/* source/staffs.c: staff code

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


#pragma code-name("STAFFSBANK")
#pragma data-name("STAFFSBANK")
#pragma rodata-name("STAFFSBANK")


/* Local Prototypes */
int8u __fastcall__ staffs_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8 __fastcall__ staffs_bit_pos(int32u *test);
int8u __fastcall__ staffs_stat_adj(int8u stat);
int8 __fastcall__ staffs_object_offset(inven_type *t_ptr);
int8u __fastcall__ staffs_known1_p(inven_type *i_ptr);
void __fastcall__ staffs_sample(inven_type *i_ptr);
void __fastcall__ staffs_desc_charges(int8u item_val);
int8u __fastcall__ staffs_cure_confusion(void);
int8u __fastcall__ staffs_cure_blindness(void);
int8u __fastcall__ staffs_cure_poison(void);
int8u __fastcall__ staffs_hp_player(int16 num);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		STAFFSNUM

#undef  INBANK_MOD
#define INBANK_MOD			staffs


int8u __fastcall__ staffs_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

int8 __fastcall__ staffs_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

int8u __fastcall__ staffs_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

int8 __fastcall__ staffs_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

int8u __fastcall__ staffs_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

void __fastcall__ staffs_sample(inven_type *i_ptr)
#include "inbank\sample.c"

/* Describe number of remaining charges.		-RAK-	*/
void __fastcall__ staffs_desc_charges(int8u item_val)
{
  int8u rem_num;

  //if (known2_p(&inventory[item_val])) {
  if ((inventory[item_val].ident & ID_KNOWN2)) {
    rem_num = inventory[item_val].p1;
    sprintf(out_val1, "You have %d charges remaining.", rem_num);
    bank_msg_print(out_val1, STAFFSNUM);
  }
}

int8u __fastcall__ staffs_cure_confusion(void)
#include "inbank\cure_confusion.c"

int8u __fastcall__ staffs_cure_blindness(void)
#include "inbank\cure_blindness.c"

/* Cure poisoning					-RAK-	*/
int8u __fastcall__ staffs_cure_poison(void)
#include "inbank\cure_poison.c"

/* Change players hit points in some manner		-RAK-	*/
int8u __fastcall__ staffs_hp_player(int16 num)
#include "inbank\hp_player.c"


/* Use a staff.	-RAK- */
void __fastcall__ use(void)
{
  int32u i;
  int8u j, k, item_val, chance, y, x;
  int8u ident;
  register inven_type *i_ptr;
  int8u ca;


  free_turn_flag = TRUE;
  if (inven_ctr == 0)
    bank_msg_print("But you are not carrying anything.", STAFFSNUM);
  else if (!staffs_find_range(TV_STAFF, TV_NEVER, &j, &k))
    bank_msg_print("You are not carrying any staffs.", STAFFSNUM);
  else if (bank_get_item(&item_val, "Use which staff?", j, k, CNIL, "", STAFFSNUM)) {
    free_turn_flag = FALSE;

    vbxe_bank(VBXE_SPLBANK);
    ca = class_level_adj[py.misc.pclass][CLA_DEVICE];			// save class adjustment
    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[item_val];

    chance = py.misc.save + staffs_stat_adj(A_INT) - i_ptr->level - 5 + (ca * py.misc.lev / 3);
    if (py.flags.confused > 0)
	  chance = chance / 2;
    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1))
	  chance = USE_DEVICE; 		/* Give everyone a slight chance */
    if (chance <= 0)
      chance = 1;
    if (randint(chance) < USE_DEVICE)
	  bank_msg_print("You failed to use the staff properly.", STAFFSNUM);
    else if (i_ptr->p1 > 0)	{
	  i = i_ptr->flags;
	  ident = FALSE;
	  (i_ptr->p1)--;
	  while (i != 0) {
	    j = staffs_bit_pos(&i) + 1;
	    /* Staffs. */
	    switch(j) {
		case 1:
		  ident = bank_light_area(char_row, char_col, STAFFSNUM);
		  break;
		case 2:
		  ident = bank_detect_sdoor(STAFFSNUM);
		  break;
		case 3:
		  ident = bank_detect_trap(STAFFSNUM);
		  break;
		case 4:
		  ident = bank_detect_treasure(STAFFSNUM);
		  break;
		case 5:
		  ident = bank_detect_object(STAFFSNUM);
		  break;
		case 6:
		  bank_teleport(100, STAFFSNUM);
		  ident = TRUE;
		  break;
		case 7:
		  ident = TRUE;
		  bank_earthquake(STAFFSNUM);
		  break;
		case 8:
		  ident = FALSE;
		  for (k = 0; k < randint(4); ++k) {
		    y = char_row;
		    x = char_col;
		    ident |= bank_summon_monster(&y, &x, FALSE, STAFFSNUM);
		  }
		  break;
		case 10:
		  ident = TRUE;
		  bank_destroy_area(char_row, char_col, STAFFSNUM);
		  break;
		case 11:
		  ident = TRUE;
		  bank_starlite(char_row, char_col);
		  break;
		case 12:
		  ident = bank_speed_monsters(1);
		  break;
		case 13:
		  ident = bank_speed_monsters(-1);
		  break;
		case 14:
		  ident = bank_sleep_monsters2(STAFFSNUM);
		  break;
		case 15:
		  ident = staffs_hp_player(randint(8));
		  break;
		case 16:
		  ident = bank_detect_invisible(STAFFSNUM);
		  break;
		case 17:
		  if (py.flags.fast == 0)
		    ident = TRUE;
		  py.flags.fast += randint(30) + 15;
		  break;
		case 18:
		  if (py.flags.slow == 0)
		    ident = TRUE;
		  py.flags.slow += randint(30) + 15;
		  break;
		case 19:
		  ident = bank_mass_poly();
		  break;
		case 20:
		  if (bank_remove_curse(STAFFSNUM)) {
		    if (py.flags.blind < 1)
			  bank_msg_print("The staff glows blue for a moment..", STAFFSNUM);
		    ident = TRUE;
		  }
		  break;
		case 21:
		  ident = bank_detect_evil(STAFFSNUM);
		  break;
		case 22:
		  if ((staffs_cure_blindness()) || (staffs_cure_poison()) || (staffs_cure_confusion()))
		    ident = TRUE;
		  break;
		case 23:
		  ident = bank_dispel_creature(CD_EVIL, 60, STAFFSNUM);
		  break;
		case 25:
		  ident = bank_unlight_area(char_row, char_col, STAFFSNUM);
		  break;
		case 32:
		  /* store bought flag */
		  break;
		default:
		  bank_msg_print("Internal error in staffs()", STAFFSNUM);
		  break;
        }
	    /* End of staff actions. */
     }

     if (ident) {
	    if (!staffs_known1_p(i_ptr)) {
		  /* round half-way case up */
		  py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;
		  bank_prt_experience(STAFFSNUM);

		  bank_identify(&item_val, STAFFSNUM);
		  i_ptr = &inventory[item_val];
		}
	  }
	  else if (!staffs_known1_p(i_ptr))
	    staffs_sample(i_ptr);
	  staffs_desc_charges(item_val);
	}
    else {
	  bank_msg_print("The staff has no charges left.", STAFFSNUM);
      if (!(i_ptr->ident & ID_KNOWN2))				// in-line known2_p code -SJ
		  i_ptr->ident |= ID_EMPTY;					// in-line add_inscribe code -SJ
	}
  }
}

