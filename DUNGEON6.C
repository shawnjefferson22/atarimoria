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
#pragma code-name("DUNGEON6BANK")
#pragma rodata-name("DUNGEON6BANK")
#pragma data-name("DUNGEON6BANK")


/* Prototypes of functions copied into this bank */
//void __fastcall__ dungeon6_erase_line(int8u row, int8u col);
int8u __fastcall__ dungeon6_mmove(int8u dir, int8u *y, int8u *x);
int8u __fastcall__ dungeon6_get_com(char *prompt, char *command);
char __fastcall__ dungeon6_map_roguedir(char comval);
int8u __fastcall__ dungeon6_get_dir(char *prompt, int8u *dir);
void __fastcall__ dungeon6_lite_spot(int8u y, int8u x);
void __fastcall__ dungeon6_invcopy(inven_type *to, int16u from_index);
void __fastcall__ dungeon6_take_hit(int8u damage, char *hit_from);
int8u __fastcall__ dungeon6_pdamroll(int8u *array);
int8u __fastcall__ dungeon6_damroll(int8u num, int8u sides);
int8u __fastcall__ dungeon6_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type);
void __fastcall__ dungeon6_disturb(int8u s, int8u l);
void __fastcall__ dungeon6_search_off(void);
void __fastcall__ dungeon6_rest_off(void);
void __fastcall__ dungeon6_change_speed(int8 num);
void __fastcall__ py_bash(int8u y, int8u x);
void __fastcall__ dungeon6_known2(inven_type *i_ptr);
void __fastcall__ dungeon6_unsample(inven_type *i_ptr);
int8 __fastcall__ dungeon6_object_offset(inven_type *t_ptr);
int8 __fastcall__ dungeon6_todis_adj(void);
int8u __fastcall__ dungeon6_stat_adj(int8u stat);
int8u __fastcall__ dungeon6_no_light(void);
char __fastcall__ dungeon6_toupper(char c);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN6NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon6


/* Implement in cartridge bank to get out of main memory -SJ */
char __fastcall__ dungeon6_toupper(char c)
{
  if ((c > 96) && (c < 123))
    return(c - 32);
}


int8u __fastcall__ dungeon6_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8u __fastcall__ dungeon6_get_com(char *prompt, char *command)
#include "inbank\get_com.c"

char __fastcall__ dungeon6_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

int8u __fastcall__ dungeon6_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

void __fastcall__ dungeon6_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8 __fastcall__ dungeon6_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

void __fastcall__ dungeon6_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ dungeon6_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"

int8 __fastcall__ dungeon6_todis_adj(void)
#include "inbank\todis_adj.c"

int8u __fastcall__ dungeon6_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

int8u __fastcall__ dungeon6_no_light(void)
#include "inbank\no_light.c"

void __fastcall__ dungeon6_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ dungeon6_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type)
#include "inbank\test_hit.c"

void __fastcall__ dungeon6_disturb(int8u s, int8u l)
#include "inbank\disturb.c"

void __fastcall__ dungeon6_search_off(void)
#include "inbank\search_off.c"

void __fastcall__ dungeon6_rest_off(void)
#include "inbank\rest_off.c"

void __fastcall__ dungeon6_change_speed(int8 num)
#include "inbank\change_speed.c"

void __fastcall__ dungeon6_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"

int8u __fastcall__ dungeon6_pdamroll(int8u *array)
#include "inbank\pdamroll.c"

/* generates damage for 2d6 style dice rolls */
int8u __fastcall__ dungeon6_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"


/* Closes an open door.				-RAK-	*/
void __fastcall__ closeobject(void)
{
  int8u y, x, dir, no_object;
  //vtype out_val, m_name;				// use out_val1 and out_val2 -SJ
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  cave_type c;							// local cave storage


  y = char_row;
  x = char_col;
  if (dungeon6_get_dir(CNIL, &dir)) {
    dungeon6_mmove(dir, &y, &x);

    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    c.tptr = c_ptr->tptr;
    c.cptr = c_ptr->cptr;

    no_object = FALSE;
    if (c_ptr->tptr != 0) {									// cavbank is still in here, cptr valid
      vbxe_bank(VBXE_OBJBANK);
	  if (t_list[c.tptr].tval == TV_OPEN_DOOR)				// use local copy of cave spot
	    if (c.cptr == 0)
	      if (t_list[c.tptr].p1 == 0) {
		    dungeon6_invcopy(&t_list[c.tptr], OBJ_CLOSED_DOOR);
		    vbxe_bank(VBXE_CAVBANK);						// need cavebank to make assigment below
		    c_ptr->fval = BLOCKED_FLOOR;
		    dungeon6_lite_spot(y, x);
	      }
	      else
	        bank_msg_print("The door appears to be broken.", DUN6NUM);
	    else {
		  vbxe_bank(VBXE_MONBANK);
	      m_ptr = &m_list[c.cptr];
	      if (m_ptr->ml)
		    sprintf (out_val1, "The %s", c_list[m_ptr->mptr].name);
	      else
		    strcpy (out_val1, "Something");
	      sprintf(out_val2, "%s is in your way!", out_val1);
	      bank_msg_print(out_val2, DUN6NUM);
	    }
	  else
	    no_object = TRUE;
    }
    else
	  no_object = TRUE;

    if (no_object) {
	  bank_msg_print("I do not see anything you can close there.", DUN6NUM);
	  free_turn_flag = TRUE;
	}
  }
}


/* Bash open a door or chest				-RAK-	*/
/* Note: Affected by strength and weight of character

   For a closed door, p1 is positive if locked; negative if
   stuck. A disarm spell unlocks and unjams doors!

   For an open door, p1 is positive for a broken door.

   A closed door can be opened - harder if locked. Any door might
   be bashed open (and thereby broken). Bashing a door is
   (potentially) faster! You move into the door way. To open a
   stuck door, it must be bashed. A closed door can be jammed
   (which makes it stuck if previously locked).

   Creatures can also open doors. A creature with open door
   ability will (if not in the line of sight) move though a
   closed or secret door with no changes. If in the line of
   sight, closed door are openned, & secret door revealed.
   Whether in the line of sight or not, such a creature may
   unlock or unstick a door.

   A creature with no such ability will attempt to bash a
   non-secret door. */
void __fastcall__ bash(void)
{
  int8u y, x, dir, tmp;
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  int8u t;							// local tptr storage


  y = char_row;
  x = char_col;
  if (dungeon6_get_dir(CNIL, &dir)) {
    if (py.flags.confused > 0) {
	  bank_msg_print("You are confused.", DUN6NUM);
	  do {
	    dir = randint(9);
	  } while (dir == 5);
	}
    dungeon6_mmove(dir, &y, &x);
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if (c_ptr->cptr > 1) {
	  if (py.flags.afraid > 0)
	    bank_msg_print("You are afraid!", DUN6NUM);
	  else
	    py_bash(y, x);
	}
    else if (c_ptr->tptr != 0) {
	  t = c_ptr->tptr;
	  vbxe_bank(VBXE_OBJBANK);
	  t_ptr = &t_list[t];
	  if (t_ptr->tval == TV_CLOSED_DOOR) {
	    bank_count_msg_print("You smash into the door!", DUN6NUM);
	    tmp = py.stats.use_stat[A_STR] + py.misc.wt / 2;
	    /* Use (roughly) similar method as for monsters. */
	    if (randint(tmp*(20+abs(t_ptr->p1))) < 10*(tmp-abs(t_ptr->p1))) {
		  bank_msg_print("The door crashes open!", DUN6NUM);
		  dungeon6_invcopy(&t_list[t], OBJ_OPEN_DOOR);
		  t_ptr->p1 = 1 - randint(2); /* 50% chance of breaking door */

		  vbxe_bank(VBXE_CAVBANK);
		  c_ptr->fval = CORR_FLOOR;
		  if (py.flags.confused == 0)
		    bank_move_char(dir, FALSE, DUN6NUM);
		  else
		    dungeon6_lite_spot(y, x);
		}
	    else if (randint(150) > py.stats.use_stat[A_DEX]) {
		  bank_msg_print("You are off-balance.", DUN6NUM);
		  py.flags.paralysis = 1 + randint(2);
		}
	    else if (command_count == 0)
		  bank_msg_print("The door holds firm.", DUN6NUM);
	  }
	  else if (t_ptr->tval == TV_CHEST) {
	    if (randint(10) == 1) {
		  bank_msg_print("You have destroyed the chest.", DUN6NUM);
		  bank_msg_print("and its contents!", DUN6NUM);
		  t_ptr->index = OBJ_RUINED_CHEST;
		  t_ptr->flags = 0;
		}
	    else if ((CH_LOCKED & t_ptr->flags) && (randint(10) == 1)) {
		  bank_msg_print("The lock breaks open!", DUN6NUM);
		  t_ptr->flags &= ~CH_LOCKED;
		}
	    else
		  bank_count_msg_print("The chest holds firm.", DUN6NUM);
	  }
	  else
	    /* Can't give free turn, or else player could try directions
	       until he found invisible creature */
	    bank_msg_print("You bash it, but nothing interesting happens.", DUN6NUM);
	}
    else {
	  if (c_ptr->fval < MIN_CAVE_WALL)
	    bank_msg_print("You bash at empty space.", DUN6NUM);
	  else
	    /* same message for wall as for secret door */
	    bank_msg_print("You bash it, but nothing interesting happens.", DUN6NUM);
	}
  }
}


/* Chests have traps too.				-RAK-	*/
/* Note: Chest traps are based on the FLAGS value		 */
void __fastcall__ chest_trap(int8u y, int8u x)
{
  register int8u i;
  int8u j, k;
  register inven_type *t_ptr;


  vbxe_bank(VBXE_CAVBANK);
  i = (CAVE_ADR(y, x))->tptr;

  vbxe_bank(VBXE_OBJBANK);
  t_ptr = &t_list[i];

  if (CH_LOSE_STR & t_ptr->flags) {
    bank_msg_print("A small needle has pricked you!", DUN6NUM);
    if (!py.flags.sustain_str) {
	  bank_dec_stat(A_STR, DUN6NUM);
	  dungeon6_take_hit(dungeon6_damroll(1, 4), "a poison needle");
	  bank_msg_print("You feel weakened!", DUN6NUM);
	}
    else
	  bank_msg_print("You are unaffected.", DUN6NUM);
  }
  if (CH_POISON & t_ptr->flags) {
    bank_msg_print("A small needle has pricked you!", DUN6NUM);
    dungeon6_take_hit(dungeon6_damroll(1, 6), "a poison needle");
    py.flags.poisoned += 10 + randint(20);
  }
  if (CH_PARALYSED & t_ptr->flags) {
    bank_msg_print("A puff of yellow gas surrounds you!", DUN6NUM);
    if (py.flags.free_act)
	  bank_msg_print("You are unaffected.", DUN6NUM);
    else {
	  bank_msg_print("You choke and pass out.", DUN6NUM);
	  py.flags.paralysis = 10 + randint(20);
	}
  }
  if (CH_SUMMON & t_ptr->flags) {
    for (i = 0; i < 3; ++i) {
	  j = y;
	  k = x;
	  bank_summon_monster(&j, &k, FALSE, DUN6NUM);
	}
  }
  if (CH_EXPLODE & t_ptr->flags) {
    bank_msg_print("There is a sudden explosion!", DUN6NUM);
    bank_delete_object(y, x, DUN6NUM);
    dungeon6_take_hit(dungeon6_damroll(5, 8), "an exploding chest");
  }
}


/* Make a bash attack on someone.				-CJS-
   Used to be part of bash above. */
void __fastcall__ py_bash(int8u y, int8u x)
{
  int8u monster;
  int16u avg_max_hp;
  register int16u base_tohit, k;
  register creature_type *c_ptr;
  register monster_type *m_ptr;
  //vtype m_name, out_val;


  vbxe_bank(VBXE_CAVBANK);
  monster = (CAVE_ADR(y, x))->cptr;

  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monster];
  c_ptr = &c_list[m_ptr->mptr];
  m_ptr->csleep = 0;
  /* Does the player know what he's fighting?	   */
  if (!m_ptr->ml)
    strcpy(out_val1, "it");
  else
    sprintf(out_val1, "the %s", c_ptr->name);

  vbxe_bank(VBXE_OBJBANK);
  base_tohit = py.stats.use_stat[A_STR] + inventory[INVEN_ARM].weight/2 + py.misc.wt/10;

  vbxe_restore_bank();
  if (!m_ptr->ml) {
	vbxe_bank(VBXE_SPLBANK);
    base_tohit = (base_tohit / 2) - (py.stats.use_stat[A_DEX]*(BTH_PLUS_ADJ-1)) - (py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTH] / 2);
  }

  vbxe_bank(VBXE_MONBANK);
  if (dungeon6_test_hit(base_tohit, py.misc.lev, py.stats.use_stat[A_DEX], c_ptr->ac, CLA_BTH)) {
    sprintf(out_val2, "You hit %s.", out_val1);
    bank_msg_print(out_val2, DUN6NUM);

    vbxe_bank(VBXE_OBJBANK);
    k = dungeon6_pdamroll(inventory[INVEN_ARM].damage);
    k = bank_critical_blow((inventory[INVEN_ARM].weight / 4 + py.stats.use_stat[A_STR]), 0, k, CLA_BTH, DUN6NUM);
    k += py.misc.wt/60 + 3;
    //if (k < 0) k = 0;

    /* See if we done it in.				     */
    if (bank_mon_take_hit(monster, k, DUN6NUM) >= 0) {
	  sprintf(out_val2, "You have slain %s.", out_val1);
	  bank_msg_print(out_val2, DUN6NUM);
	  bank_prt_experience(DUN6NUM);
	}
    else {
	  out_val1[0] = dungeon6_toupper(out_val1[0]); 		/* Capitalize */

	  vbxe_bank(VBXE_MONBANK);
	  /* Can not stun Balrog */
	  avg_max_hp = (c_ptr->cdefense & CD_MAX_HP ? c_ptr->hd[0] * c_ptr->hd[1] : (c_ptr->hd[0] * (c_ptr->hd[1] + 1)) >> 1);
	  if ((100 + randint(400) + randint(400)) > (m_ptr->hp + avg_max_hp)) {
	    m_ptr->stunned += randint(3) + 1;
	    if (m_ptr->stunned > 24)
	      m_ptr->stunned = 24;
	    sprintf(out_val2, "%s appears stunned!", out_val1);
	  }
	  else
	    sprintf(out_val2, "%s ignores your bash!", out_val1);
	  bank_msg_print(out_val2, DUN6NUM);
	}
  }
  else {
    sprintf(out_val2, "You miss %s.", out_val1);
    bank_msg_print(out_val2, DUN6NUM);
  }

  if (randint(150) > py.stats.use_stat[A_DEX]) {
    bank_msg_print("You are off balance.", DUN6NUM);
    py.flags.paralysis = 1 + randint(2);
  }
}


/* Disarms a trap					-RAK-	*/
void disarm_trap(void)
{
  int8u y, x, level, tmp, dir, no_disarm;
  register int8u tot, i;
  register cave_type *c_ptr;
  register inven_type *i_ptr;
  monster_type *m_ptr;
  //vtype m_name, out_val;
  int8u tptr, cptr;


  y = char_row;
  x = char_col;
  if (dungeon6_get_dir(CNIL, &dir)) {
    dungeon6_mmove(dir, &y, &x);
    //c_ptr = &cave[y][x];
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    tptr = c_ptr->tptr;					// get cave values -SJ
    cptr = c_ptr->cptr;					// get cave values -SJ

    no_disarm = FALSE;
    vbxe_bank(VBXE_OBJBANK);
    //if (c_ptr->cptr > 1 && c_ptr->tptr != 0 && (t_list[c_ptr->tptr].tval == TV_VIS_TRAP || t_list[c_ptr->tptr].tval == TV_CHEST)) {
    if (cptr > 1 && tptr != 0 && (t_list[tptr].tval == TV_VIS_TRAP || t_list[tptr].tval == TV_CHEST)) {
	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[cptr];
	  if (m_ptr->ml)
	    sprintf (out_val1, "The %s", c_list[m_ptr->mptr].name);
	  else
	    strcpy (out_val1, "Something");
	  sprintf(out_val2, "%s is in your way!", out_val1);
	  bank_msg_print(out_val2, DUN6NUM);
	}
    else if (tptr != 0) {
	  vbxe_bank(VBXE_SPLBANK);
	  tot = py.misc.disarm + 2*dungeon6_todis_adj() + dungeon6_stat_adj(A_INT) + (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
	  if ((py.flags.blind > 0) || (dungeon6_no_light()))
	    tot = tot / 10;
	  if (py.flags.confused > 0)
	    tot = tot / 10;
	  if (py.flags.image > 0)
	    tot = tot / 10;

	  vbxe_bank(VBXE_OBJBANK);
	  i_ptr = &t_list[tptr];
	  i = i_ptr->tval;
	  level = i_ptr->level;
	  if (i == TV_VIS_TRAP) {		    /* Floor trap    */
        if ((tot + 100 - level) > randint(100)) {
		  bank_msg_print("You have disarmed the trap.", DUN6NUM);
		  py.misc.exp += i_ptr->p1;
		  bank_delete_object(y, x, DUN6NUM);
		  /* make sure we move onto the trap even if confused */
		  tmp = py.flags.confused;
		  py.flags.confused = 0;
		  bank_move_char(dir, FALSE, DUN6NUM);
		  py.flags.confused = tmp;
		  bank_prt_experience(DUN6NUM);
		}
	    /* avoid randint(0) call */
	    else if ((tot > 5) && (randint(tot) > 5))
		  bank_count_msg_print("You failed to disarm the trap.", DUN6NUM);
	    else {
		  bank_msg_print("You set the trap off!", DUN6NUM);
		  /* make sure we move onto the trap even if confused */
		  tmp = py.flags.confused;
		  py.flags.confused = 0;
		  bank_move_char(dir, FALSE, DUN6NUM);
		  py.flags.confused += tmp;
		}
	  }
	  else if (i == TV_CHEST) {
	    //if (!known2_p(i_ptr)) {
		if (!(i_ptr->ident & ID_KNOWN2)) {							// in-lined !known2_p(i_ptr) -SJ
		  bank_msg_print("I don't see a trap.", DUN6NUM);
		  free_turn_flag = TRUE;
		}
	    else if (CH_TRAPPED & i_ptr->flags) {
		  if ((tot - level) > randint(100)) {
		    i_ptr->flags &= ~CH_TRAPPED;
		    if (CH_LOCKED & i_ptr->flags)
			  i_ptr->name2 = SN_LOCKED;
		    else
			  i_ptr->name2 = SN_DISARMED;
		    bank_msg_print("You have disarmed the chest.", DUN6NUM);
		    dungeon6_known2(i_ptr);
		    py.misc.exp += level;
		    bank_prt_experience(DUN6NUM);
		  }
		  else if ((tot > 5) && (randint(tot) > 5))
		    bank_count_msg_print("You failed to disarm the chest.", DUN6NUM);
		  else {
		    bank_msg_print("You set a trap off!", DUN6NUM);
		    dungeon6_known2(i_ptr);
		    chest_trap(y, x);
		  }
		}
	    else {
		  bank_msg_print("The chest was not trapped.", DUN6NUM);
		  free_turn_flag = TRUE;
		}
	  }
	  else
	    no_disarm = TRUE;
	}
    else
	  no_disarm = TRUE;

    if (no_disarm) {
	  bank_msg_print("I do not see anything to disarm there.", DUN6NUM);
	  free_turn_flag = TRUE;
	}
  }
}



