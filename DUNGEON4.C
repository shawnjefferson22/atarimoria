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
#pragma code-name("DUNGEON4BANK")
#pragma rodata-name("DUNGEON4BANK")
#pragma data-name("DUNGEON4BANK")


/* Prototypes of functions copied into this bank */
void __fastcall__ dungeon4_pause_line(int8u prt_line);
void __fastcall__ dungeon4_change_speed(int8 num);
void __fastcall__ dungeon4_search_off(void);
int8u __fastcall__ dungeon4_no_light(void);
int8u __fastcall__ dungeon4_get_string(char *in_str, int8u row, int8u column, int8u slen);
char __fastcall__ dungeon4_map_roguedir(char comval);
int8u __fastcall__ dungeon4_get_dir(char *prompt, int8u *dir);
int8u __fastcall__ dungeon4_mmove(int8u dir, int8u *y, int8u *x);
int8 __fastcall__ dungeon4_todis_adj(void);
int8u __fastcall__ dungeon4_stat_adj(int8u stat);
int8 __fastcall__ dungeon4_bit_pos(int32u *test);
int8u __fastcall__ dungeon4_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
void __fastcall__ dungeon4_invcopy(inven_type *to, int16u from_index);
void __fastcall__ dungeon4_known2(inven_type *i_ptr);
void __fastcall__ dungeon4_unsample(inven_type *i_ptr);
int8 __fastcall__ dungeon4_object_offset(inven_type *t_ptr);
void __fastcall__ dungeon4_lite_spot(int8u y, int8u x);
int8u __fastcall__ dungeon4_isprint(char c);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN4NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon4


/* Finds range of item in inventory list		-RAK-	*/
int8u __fastcall__ dungeon4_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"


/* Implemented this in the cartridge bank instead of main ram -SJ */
int8u __fastcall__ dungeon4_isprint(char c)
{
  switch(c) {
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
      return(FALSE);
    default:
      return(TRUE);
  }
}


void __fastcall__ dungeon4_pause_line(int8u prt_line)
#include "inbank\pause_line.c"

void __fastcall__ dungeon4_change_speed(int8 num)
#include "inbank\change_speed.c"

void __fastcall__ dungeon4_search_off(void)
#include "inbank\search_off.c"

int8u __fastcall__ dungeon4_no_light(void)
#include "inbank\no_light.c"

int8u __fastcall__ dungeon4_get_com(char *prompt, char *command)
#include "inbank\get_com.c"

char __fastcall__ dungeon4_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

int8u __fastcall__ dungeon4_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

int8u __fastcall__ dungeon4_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8 __fastcall__ dungeon4_todis_adj(void)
#include "inbank\todis_adj.c"

int8u __fastcall__ dungeon4_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

void __fastcall__ dungeon4_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

void __fastcall__ dungeon4_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ dungeon4_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"

int8 __fastcall__ dungeon4_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

/* Lights up given location				-RAK-	*/
void __fastcall__ dungeon4_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"


/* Resting allows a player to safely restore his hp	-RAK-	*/
void __fastcall__ rest(void)
{
  int16 rest_num;
  //vtype rest_str;					// use out_val1 instead


  if (command_count > 0) {
    rest_num = command_count;
    command_count = 0;
  }
  else {
    bank_prt("Rest for how long? ", 0, 0, DUN4NUM);
    rest_num = 0;
    if (dungeon4_get_string(out_val1, 0, 19, 5)) {
	  if (out_val1[0] == '*')
	    rest_num = -MAX_SHORT;
	  else
	    rest_num = atoi(out_val1);
	}
  }

  /* check for reasonable value, must be positive number in range of a
     short, or must be -MAX_SHORT */
  if ((rest_num == -MAX_SHORT) || (rest_num > 0) && (rest_num < MAX_SHORT)) {
    if (py.flags.status & PY_SEARCH)
	  dungeon4_search_off();
    py.flags.rest = rest_num;
    py.flags.status |= PY_REST;
    bank_prt_state(DUN4NUM);
    py.flags.food_digested--;
    bank_prt("Press any key to stop resting...", 0, 0, DUN4NUM);
  }
  else {
    if (rest_num != 0)
	  bank_msg_print ("Invalid rest count.", DUN4NUM);
      //dungeon2_erase_line(MSG_LINE, 0);
      bank_vbxe_cleartoeol(0, MSG_LINE, DUN4NUM);				// eraseline code, in-line -SJ
      free_turn_flag = TRUE;
    }
}


/* Gets a string terminated by <RETURN>         */
/* Function returns false if <ESCAPE> is input  */
int8u __fastcall__ dungeon4_get_string(char *in_str, int8u row, int8u column, int8u slen)
{
  register int8u start_col, end_col;
  register char i;
  register char *p;
  int8u flag, aborted;


  aborted = FALSE;
  flag  = FALSE;

  //vbxe_clearxy(column, row, slen);					// clear on screen
  for(i=0; i<slen; ++i)
    bank_vbxe_cputcxy(column+i, row, ' ', 0x00, DUN4NUM);

  start_col = column;
  end_col = column + slen - 1;
  if (end_col > 79) {
    slen = 80 - column;
    end_col = 79;
  }
  p = in_str;
  do {
    i = cgetc();
    switch(i) {
	  case ESCAPE:
	    aborted = TRUE;
	    break;
	  case RETURN:
        flag = TRUE;
        break;
      case DELETE:
	    if (column > start_col) {
		  --column;
		  bank_vbxe_cputcxy(column, row, ' ', 0x00, DUN4NUM);		// clear one space
		  *--p = '\0';
	    }
	    break;
	  default:
	    if (!dungeon4_isprint(i) || column > end_col)
		  ;
	    else {
		  bank_vbxe_cputcxy(column, row, i, 0x1F, DUN4NUM);			// output in yellow
		  *p++ = i;
		  ++column;
	    }
	    break;
	}
  }
  while ((!flag) && (!aborted));

  if (aborted)
    return(FALSE);

  /* Remove trailing blanks     */
  while (p > in_str && p[-1] == ' ')
    --p;

  *p = '\0';
  return(TRUE);
}


/* Go up one level					-RAK-	*/
void __fastcall__ go_up(void)
{
  register int8u t;
  int8u no_stairs = FALSE;


  vbxe_bank(VBXE_CAVBANK);
  t = cave[char_row][char_col].tptr;

  vbxe_bank(VBXE_OBJBANK);
  if (t != 0)
    if (t_list[t].tval == TV_UP_STAIR) {
	  --dun_level;
	  new_level_flag = TRUE;
	  bank_msg_print("You enter a maze of up staircases.", DUN4NUM);
	  bank_msg_print("You pass through a one-way door.", DUN4NUM);
    }
    else
      no_stairs = TRUE;
  else
    no_stairs = TRUE;

  if (no_stairs) {
    bank_msg_print("I see no up staircase here.", DUN4NUM);
    free_turn_flag = TRUE;
  }
}


/* Go down one level					-RAK-	*/
void __fastcall__ go_down(void)
{
  register int8u t;
  int8u no_stairs = FALSE;


  vbxe_bank(VBXE_CAVBANK);
  t = cave[char_row][char_col].tptr;

  vbxe_bank(VBXE_OBJBANK);
  if (t != 0)
    if (t_list[t].tval == TV_DOWN_STAIR) {
	  ++dun_level;
	  new_level_flag = TRUE;
	  bank_msg_print("You enter a maze of down staircases.", DUN4NUM);
	  bank_msg_print("You pass through a one-way door.", DUN4NUM);
    }
    else
      no_stairs = TRUE;
  else
    no_stairs = TRUE;

  if (no_stairs) {
    bank_msg_print("I see no down staircase here.", DUN4NUM);
    free_turn_flag = TRUE;
  }
}


/* Returns position of first set bit			-RAK-	*/
/*     and clears that bit */
int8 __fastcall__ dungeon4_bit_pos(int32u *test)
#include "inbank\bit_pos.c"


/* Examine a Book					-RAK-	*/
void __fastcall__ examine_book(void)
{
  int32u j;
  int8u i, k;
  int8u item_val;
  register int8u flag, s;
  int8u spell_index[31];
  register inven_type *i_ptr;


  vbxe_bank(VBXE_SPLBANK);					// class array is in splbank
  s = class[py.misc.pclass].spell;			// temporary storage

  if (!dungeon4_find_range(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &i, &k))
    bank_msg_print("You are not carrying any books.", DUN4NUM);
  else if (py.flags.blind > 0)
    bank_msg_print("You can't see to read your spell book!", DUN4NUM);
  else if (dungeon4_no_light())
    bank_msg_print("You have no light to read by.", DUN4NUM);
  else if (py.flags.confused > 0)
    bank_msg_print("You are too confused.", DUN4NUM);
  else if (bank_get_item(&item_val, "Which Book?", i, k, CNIL, "", DUN4NUM)) {
    flag = TRUE;
    vbxe_bank(VBXE_OBJBANK);						// need objbank for inventory
    i_ptr = &inventory[item_val];
    //if (class[py.misc.pclass].spell == MAGE) {
    if (s == MAGE) {
	  if (i_ptr->tval != TV_MAGIC_BOOK)
	    flag = FALSE;
	}
    //else if (class[py.misc.pclass].spell == PRIEST) {
    else if (s == PRIEST) {
	  if (i_ptr->tval != TV_PRAYER_BOOK)
	    flag = FALSE;
	}
    else
	  flag = FALSE;

    if (!flag)
	  bank_msg_print("You do not understand the language.", DUN4NUM);
    else {
	  i = 0;
	  vbxe_bank(VBXE_OBJBANK);
	  j = inventory[item_val].flags;

      vbxe_bank(VBXE_SPLBANK);				// need spell bank for spell_index and magic_spell
	  while (j) {
	    k = dungeon4_bit_pos(&j);
	    if (magic_spell[py.misc.pclass-1][k].slevel < 99) {
		  spell_index[i] = k;
		  ++i;
		}
	  }

	  bank_vbxe_savescreen(DUN4NUM);
	  bank_print_spells(spell_index, i, TRUE, -1, DUN4NUM);
	  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", i+2, 0, DUN4NUM);
	  dungeon4_pause_line(0);
	  bank_vbxe_restorescreen(DUN4NUM);
	}
  }
}


/* Jam a closed door					-RAK-	*/
void __fastcall__ jamdoor(void)
{
  int8u y, x, dir;
  int8u i, j;
  //register cave_type *c_ptr;
  register inven_type *t_ptr, *i_ptr;
  //char tmp_str[80];			// use out_val1 instead
  cave_type c;


  free_turn_flag = TRUE;
  y = char_row;
  x = char_col;
  if (dungeon4_get_dir(CNIL, &dir)) {
    dungeon4_mmove(dir, &y, &x);
    vbxe_bank(VBXE_CAVBANK);
    memcpy(&c, &cave[y][x], sizeof(cave_type));				// make local copy

    if (c.tptr != 0) {
	  vbxe_bank(VBXE_OBJBANK);
	  t_ptr = &t_list[c.tptr];
	  if (t_ptr->tval == TV_CLOSED_DOOR)
	    if (c.cptr == 0) {
		  if (dungeon4_find_range(TV_SPIKE, TV_NEVER, &i, &j)) {
		    free_turn_flag = FALSE;
		    bank_count_msg_print("You jam the door with a spike.", DUN4NUM);
		    if (t_ptr->p1 > 0)
		      t_ptr->p1 = -t_ptr->p1;	// Make locked to stuck.
		    /* Successive spikes have a progressively smaller effect.
		       Series is: 0 20 30 37 43 48 52 56 60 64 67 70 ... */
		    t_ptr->p1 -= 1 + 190 / (10 - t_ptr->p1);
		    i_ptr = &inventory[i];
		    if (i_ptr->number > 1) {
			  i_ptr->number--;
			  inven_weight -= i_ptr->weight;
		    }
		    else
		      bank_inven_destroy(i, DUN4NUM);
		  }
		  else
		    bank_msg_print("But you have no spikes.", DUN4NUM);
	    }
	    else {
		  free_turn_flag = FALSE;
		  vbxe_bank(VBXE_MONBANK);
		  sprintf(out_val1, "The %s is in your way!", c_list[m_list[c.cptr].mptr].name);
		  bank_msg_print(out_val1, DUN4NUM);
	    }
	    else if (t_ptr->tval == TV_OPEN_DOOR)
	      bank_msg_print("The door must be closed first.", DUN4NUM);
	    else
	      bank_msg_print("That isn't a door!", DUN4NUM);
	}
    else
	  bank_msg_print("That isn't a door!", DUN4NUM);
  }
}


/* Refill the players lamp				-RAK-	*/
void __fastcall__ refill_lamp(void)
{
  int8u i, j;
  register int8u k;
  register inven_type *i_ptr;


  free_turn_flag = TRUE;
  vbxe_bank(VBXE_OBJBANK);
  k = inventory[INVEN_LIGHT].subval;
  if (k != 0)
    bank_msg_print("But you are not using a lamp.", DUN4NUM);
  else if (!dungeon4_find_range(TV_FLASK, TV_NEVER, &i, &j))
    bank_msg_print("You have no oil.", DUN4NUM);
  else {
    free_turn_flag = FALSE;
    i_ptr = &inventory[INVEN_LIGHT];
    i_ptr->p1 += inventory[i].p1;
    if (i_ptr->p1 > OBJ_LAMP_MAX) {
	  i_ptr->p1 = OBJ_LAMP_MAX;
	  bank_msg_print ("Your lamp overflows, spilling oil on the ground.", DUN4NUM);
	  bank_msg_print("Your lamp is full.", DUN4NUM);
	}
    else if (i_ptr->p1 > OBJ_LAMP_MAX/2)
	  bank_msg_print("Your lamp is more than half full.", DUN4NUM);
    else if (i_ptr->p1 == OBJ_LAMP_MAX/2)
	  bank_msg_print("Your lamp is half full.", DUN4NUM);
    else
	  bank_msg_print("Your lamp is less than half full.", DUN4NUM);
    bank_desc_remain(i, DUN4NUM);
    bank_inven_destroy(i, DUN4NUM);
  }
}



/* Opens a closed door or closed chest.		-RAK-	*/
void __fastcall__ openobject(void)
{
  int8u y, x, i, dir;
  int8u flag, no_object;
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  //register struct misc *p_ptr;
  register monster_type *m_ptr;
  //vtype m_name, out_val;					// use out_val1 and out_val2
  cave_type c;


  y = char_row;
  x = char_col;
  if (dungeon4_get_dir(CNIL, &dir)) {
    dungeon4_mmove(dir, &y, &x);
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = &cave[y][x];
    memcpy(&c, c_ptr, sizeof(cave_type));

    no_object = FALSE;
    vbxe_bank(VBXE_OBJBANK);
    if (c.cptr > 1 && c.tptr != 0 && (t_list[c.tptr].tval == TV_CLOSED_DOOR || t_list[c.tptr].tval == TV_CHEST)) {
	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[c.cptr];
	  if (m_ptr->ml)
	    sprintf(out_val1, "The %s", c_list[m_ptr->mptr].name);
	  else
	    strcpy(out_val1, "Something");
	  sprintf(out_val2, "%s is in your way!", out_val1);
	  bank_msg_print(out_val2, DUN4NUM);
	}
    else if (c.tptr != 0)
	  /* Closed door */
	  if (t_list[c.tptr].tval == TV_CLOSED_DOOR) {
	    t_ptr = &t_list[c.tptr];
	    if (t_ptr->p1 > 0) {	 /* It's locked.	*/
		  vbxe_bank(VBXE_SPLBANK);
		  i = py.misc.disarm + 2*dungeon4_todis_adj() + dungeon4_stat_adj(A_INT) +
		      (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
		  vbxe_restore_bank();   // back to the object bank

		  if (py.flags.confused > 0)
		    bank_msg_print("You are too confused to pick the lock.", DUN4NUM);
		  else if ((i-t_ptr->p1) > randint(100)) {
		    bank_msg_print("You have picked the lock.", DUN4NUM);
		    py.misc.exp++;
		    bank_prt_experience(DUN4NUM);
		    t_ptr->p1 = 0;
		  }
		  else
		    bank_count_msg_print("You failed to pick the lock.", DUN4NUM);
	    }
	    else if (t_ptr->p1 < 0)	 /* It's stuck	  */
	      bank_msg_print("It appears to be stuck.", DUN4NUM);

	    if (t_ptr->p1 == 0) {
		  dungeon4_invcopy(&t_list[c.tptr], OBJ_OPEN_DOOR);

		  vbxe_bank(VBXE_CAVBANK);
		  c_ptr->fval = CORR_FLOOR;
		  dungeon4_lite_spot(y, x);
		  command_count = 0;
	    }
	  }
      /* Open a closed chest.		     */
	  else if (t_list[c.tptr].tval == TV_CHEST) {
		vbxe_bank(VBXE_SPLBANK);
	    i = py.misc.disarm + 2*dungeon4_todis_adj() + dungeon4_stat_adj(A_INT) +
	        (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
	    vbxe_restore_bank();

	    t_ptr = &t_list[c.tptr];
	    flag = FALSE;
	    if (CH_LOCKED & t_ptr->flags)
	      if (py.flags.confused > 0)
		    bank_msg_print("You are too confused to pick the lock.", DUN4NUM);
	      else if ((i-(int)t_ptr->level) > randint(100)) {
		    bank_msg_print("You have picked the lock.", DUN4NUM);
		    flag = TRUE;
		    py.misc.exp += t_ptr->level;
		    bank_prt_experience(DUN4NUM);
          }
	      else
		    bank_count_msg_print("You failed to pick the lock.", DUN4NUM);
	    else
	      flag = TRUE;

	    if (flag) {
		  t_ptr->flags &= ~CH_LOCKED;
		  t_ptr->name2 = SN_EMPTY;
		  dungeon4_known2(t_ptr);
		  t_ptr->cost = 0;
	    }
	    flag = FALSE;

	    /* Was chest still trapped?	 (Snicker)   */
	    if ((CH_LOCKED & t_ptr->flags) == 0) {
		  bank_chest_trap(y, x, DUN4NUM);
		  if (c.tptr != 0)
		    flag = TRUE;
	    }
	    /* Chest treasure is allocated as if a creature   */
	    /* had been killed.				   */
	    if (flag) {
		/* clear the cursed chest/monster win flag, so that people
		   can not win by opening a cursed chest */
		  t_list[c.tptr].flags &= ~TR_CURSED;
		  bank_monster_death(y, x, t_list[c.tptr].flags, DUN4NUM);
		  t_list[c.tptr].flags = 0;
	    }
	  }
	  else
	    no_object = TRUE;
    else
	  no_object = TRUE;

    if (no_object) {
	  bank_msg_print("I do not see anything you can open there.", DUN4NUM);
	  free_turn_flag = TRUE;
	}
  }
}

