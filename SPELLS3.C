/* source/spells.c: player/creature spells, breaths, wands, scrolls, etc. code

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


#pragma code-name("SPELLS3BANK")
#pragma data-name("SPELLS3BANK")
#pragma rodata-name("SPELLS3BANK")


/* Local Prototypes */
int8u __fastcall__ spells3_in_bounds(int8u y, int8u x);
int8u __fastcall__ spells3_panel_contains(int8u y, int8u x);
int8u __fastcall__ spells3_distance(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ spells3_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr);
int8u __fastcall__ spells3_mmove(int8u dir, int8u *y, int8u *x);
int8u __fastcall__ spells3_los(int8u fromY, int8u fromX, int8u toY, int8u toX);
int8 __fastcall__ spells3_object_offset(inven_type *t_ptr);
void __fastcall__ spells3_known2(inven_type *i_ptr);
void __fastcall__ spells3_unsample(inven_type *i_ptr);
void __fastcall__ spells3_change_trap(int8u y, int8u x);
void __fastcall__ spells3_lite_spot(int8u y, int8u x);
void __fastcall__ replace_spot(int8u y, int8u x, int8u typ);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		SPELLS3NUM

#undef  INBANK_MOD
#define INBANK_MOD			spells3


int8u __fastcall__ spells3_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

int8u __fastcall__ spells3_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ spells3_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

void __fastcall__ spells3_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ spells3_los(int8u fromY, int8u fromX, int8u toY, int8u toX)
#include "inbank\los.c"

void __fastcall__ spells3_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
#include "inbank\monster_name.c"

int8u __fastcall__ spells3_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8 __fastcall__ spells3_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

void __fastcall__ spells3_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ spells3_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"

void __fastcall__ spells3_change_trap(int8u y, int8u x)
#include "inbank\change_trap.c"


/* Attempts to destroy a type of creature.  Success depends on	*/
/* the creatures level VS. the player's level		 -RAK-	 */
int8u __fastcall__ dispel_creature(int8u cflag, int8u damage)
{
  register int8u i;
  int8u dispel;
  int8 k;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int16u mon;


  dispel = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
	vbxe_bank(VBXE_MONBANK);
    m_ptr = &m_list[i];
    if ((m_ptr->cdis <= MAX_SIGHT) && (cflag & c_list[m_ptr->mptr].cdefense)) {
      if (spells3_los(char_row, char_col, m_ptr->fy, m_ptr->fx)) {
	    vbxe_bank(VBXE_MONBANK);
	    r_ptr = &c_list[m_ptr->mptr];
	    mon = m_ptr->mptr;

	    vbxe_bank(VBXE_GENBANK);
	    c_recall[mon].r_cdefense |= cflag;
	    vbxe_restore_bank();

	    spells3_monster_name(out_val2, m_ptr, r_ptr);

	    k = bank_mon_take_hit(i, randint(damage), SPELLS3NUM);
	    /* Should get these messages even if the monster is not visible.  */
	    if (k >= 0)
	      sprintf(out_val1, "%s dissolves!", out_val2);
	    else
	      sprintf(out_val1, "%s shudders.", out_val2);

	    bank_msg_print(out_val1, SPELLS3NUM);
	    dispel = TRUE;
	    if (k >= 0)
	      bank_prt_experience(SPELLS3NUM);
	  }
	}
  }

  return(dispel);
}


void __fastcall__ replace_spot(int8u y, int8u x, int8u typ)
{
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);
  switch(typ) {
    case 1: case 2: case 3:
      c_ptr->fval  = CORR_FLOOR;
      break;
    case 4: case 7: case 10:
      c_ptr->fval  = GRANITE_WALL;
      break;
    case 5: case 8: case 11:
      c_ptr->fval  = MAGMA_WALL;
      break;
    case 6: case 9: case 12:
      c_ptr->fval  = QUARTZ_WALL;
      break;
  }

  //c_ptr->pl = FALSE;
  //c_ptr->fm = FALSE;
  //c_ptr->lr = FALSE;  /* this is no longer part of a room */

  c_ptr->flags &= ~CAVE_PL;
  c_ptr->flags &= ~CAVE_FM;
  c_ptr->flags &= ~CAVE_LR;		/* this is no longer part of a room */

  if (c_ptr->tptr != 0)
    bank_delete_object(y, x, SPELLS3NUM);

  vbxe_bank(VBXE_CAVBANK);							// in case delete_object destroys vbxe bank -SJ
  if (c_ptr->cptr > 1)
    bank_delete_monster(c_ptr->cptr, SPELLS3NUM);
}


/* The spell of destruction.				-RAK-	*/
/* NOTE : Winning creatures that are deleted will be considered	 */
/*	  as teleporting to another level.  This will NOT win the game. */
void __fastcall__ destroy_area(int8u y, int8u x)
{
  register int8u i, j, k;
  int8u fval;



  if (dun_level > 0) {
    for (i = (y-15); i <= (y+15); ++i)					// could cause int rollover, but inbounds check should
	  for (j = (x-15); j <= (x+15); ++j)				// sort that out. -SJ
	    if (spells3_in_bounds(i, j)) {
		  vbxe_bank(VBXE_CAVBANK);
  	      fval = (CAVE_ADR(i,j))->fval;					// get the floor value -SJ

  	      if (fval != BOUNDARY_WALL) {
	        k = spells3_distance(i, j, y, x);
	        if (k == 0) /* clear player's spot, but don't put wall there */
		      replace_spot(i, j, 1);
	        else if (k < 13)
		      replace_spot(i, j, randint(6));
	        else if (k < 16)
		      replace_spot(i, j, randint(9));
	      }
		}
  }

  bank_msg_print("There is a searing blast of light!", SPELLS3NUM);
  py.flags.blind += 10 + randint(10);

  vbxe_restore_bank();
}


/* Disarms all traps/chests in a given direction	-RAK-	*/
int8u __fastcall__ disarm_all(int8u dir, int8u y, int8u x)
{
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  int8u disarm, dist;
  int8u t;


  disarm = FALSE;
  dist = 255;					// will rollover to 0 -SJ
  do {
    /* put mmove at end, in case standing on a trap */
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    t = c_ptr->tptr;
    /* note, must continue upto and including the first non open space,
       because secret doors have fval greater than MAX_OPEN_SPACE */
    if (t != 0) {
	  vbxe_bank(VBXE_OBJBANK);
	  t_ptr = &t_list[t];
	  if ((t_ptr->tval == TV_INVIS_TRAP) || (t_ptr->tval == TV_VIS_TRAP)) {
	    if (bank_delete_object(y, x, SPELLS3NUM))
		  disarm = TRUE;
	  }
	  else if (t_ptr->tval == TV_CLOSED_DOOR)
	    t_ptr->p1 = 0;  	/* Locked or jammed doors become merely closed. */
	  else if (t_ptr->tval == TV_SECRET_DOOR) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr->flags |= CAVE_FM;
	    spells3_change_trap(y, x);
	    disarm = TRUE;
	  }
	  else if ((t_ptr->tval == TV_CHEST) && (t_ptr->flags != 0)) {
	    bank_msg_print("Click!", SPELLS3NUM);
	    t_ptr->flags &= ~(CH_TRAPPED|CH_LOCKED);
	    disarm = TRUE;
	    t_ptr->name2 = SN_UNLOCKED;
	    spells3_known2(t_ptr);
	  }
	}

    vbxe_bank(VBXE_CAVBANK);
    spells3_mmove(dir, &y, &x);
  } while ((dist <= OBJ_BOLT_RANGE) && c_ptr->fval <= MAX_OPEN_SPACE);

  return(disarm);
}


/* Increase or decrease a creatures hit points		-RAK-	*/
int8u __fastcall__ hp_monster(int8u dir, int8u y, int8u x, int8 dam)
{
  int8 i;
  int8u flag, dist, monster;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  //vtype out_val, m_name;
  int8u c;


  monster = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;

    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  c = c_ptr->cptr;
	  flag = TRUE;

	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  spells3_monster_name(out_val2, m_ptr, r_ptr);
	  monster = TRUE;

	  i = bank_mon_take_hit(c, (int16)dam, SPELLS3NUM);
	  if (i >= 0) {
	    sprintf(out_val1, "%s dies in a fit of agony.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	    bank_prt_experience(SPELLS3NUM);
	  }
	  else if (dam > 0) {
	    sprintf(out_val1, "%s screams in agony.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	}
  } while (!flag);

  return(monster);
}


/* Drains life; note it must be living.		-RAK-	*/
int8u __fastcall__ drain_life(int8u dir, int8u y, int8u x)
{
  int8 i;
  int8u flag, dist, drain;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u c;
  int16u mon;


  drain = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  c = c_ptr->cptr;
	  flag = TRUE;

	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  mon = m_ptr->mptr;

	  if ((r_ptr->cdefense & CD_UNDEAD) == 0) {
	    drain = TRUE;
	    spells3_monster_name(out_val2, m_ptr, r_ptr);

	    i = bank_mon_take_hit(c, 75, SPELLS3NUM);
	    if (i >= 0) {
		  sprintf(out_val1, "%s dies in a fit of agony.", out_val2);
		  bank_msg_print(out_val1, SPELLS3NUM);
		  bank_prt_experience(SPELLS3NUM);
		}
	    else {
		  sprintf(out_val1, "%s screams in agony.", out_val2);
		  bank_msg_print(out_val1, SPELLS3NUM);
		}
	  }
	  else {
		vbxe_bank(VBXE_GENBANK);
	    c_recall[mon].r_cdefense |= CD_UNDEAD;
      }
    }
  } while (!flag);

  return(drain);
}


/* Increase or decrease a creatures speed		-RAK-	*/
/* NOTE: cannot slow a winning creature (BALROG)		 */
int8u __fastcall__ speed_monster(int8u dir, int8u y, int8u x, int8 spd)
{
  int8u flag, dist, speed;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u c;


  speed = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  flag = TRUE;
	  c = c_ptr->cptr;
	  vbxe_bank(VBXE_MONBANK);

	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  spells3_monster_name(out_val2, m_ptr, r_ptr);
	  if (spd > 0) {
	    m_ptr->cspeed += spd;
	    m_ptr->csleep = 0;
	    sprintf(out_val1, "%s starts moving faster.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	    speed = TRUE;
	  }
	  else if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
	    m_ptr->cspeed += spd;
	    m_ptr->csleep = 0;
	    sprintf(out_val1, "%s starts moving slower.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	    speed = TRUE;
	  }
	  else {
	    m_ptr->csleep = 0;
	    sprintf(out_val1, "%s is unaffected.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	}
  } while (!flag);

  return(speed);
}


/* Confuse a creature					-RAK-	*/
int8u __fastcall__ confuse_monster(int8u dir, int8u y, int8u x)
{
  int8u flag, dist, confuse;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u c;
  int16u mon;


  confuse = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  c = c_ptr->cptr;
	  vbxe_bank(VBXE_MONBANK);

	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  mon = m_ptr->mptr;

	  spells3_monster_name(out_val2, m_ptr, r_ptr);
	  flag = TRUE;
	  if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
	    if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cdefense |= CD_NO_SLEEP;
		  vbxe_restore_bank();
	    }
	    /* Monsters which resisted the attack should wake up. Monsters with inane resistence ignore the attack. */
	    if (!(CD_NO_SLEEP & r_ptr->cdefense))
		  m_ptr->csleep = 0;

	    sprintf(out_val1, "%s is unaffected.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	  else {
	    m_ptr->confused = TRUE;
	    confuse = TRUE;
	    m_ptr->csleep = 0;
	    sprintf(out_val1, "%s appears confused.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	}
  } while (!flag);

  return(confuse);
}


/* Sleep a creature.					-RAK-	*/
int8u __fastcall__ sleep_monster(int8u dir, int8u y, int8u x)
{
  int8u flag, dist, sleep;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u c;
  int16u mon;


  sleep = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
      c = c_ptr->cptr;
      vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  mon = m_ptr->mptr;

	  flag = TRUE;
	  spells3_monster_name(out_val2, m_ptr, r_ptr);
	  if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
	    if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
          vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cdefense |= CD_NO_SLEEP;
		}
	    sprintf(out_val1, "%s is unaffected.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	  else {
	    m_ptr->csleep = 500;
	    sleep = TRUE;
	    sprintf(out_val1, "%s falls asleep.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	}
  } while (!flag);

  return(sleep);
}


/* Destroy all traps and doors in a given direction	-RAK-	*/
int8u __fastcall__ td_destroy2(int8u dir, int8u y, int8u x)
{
  register int8u destroy2, dist;
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  int8u t;


  destroy2 = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    /* must move into first closed spot, as it might be a secret door */
    if (c_ptr->tptr != 0) {
	  t = c_ptr->tptr;
	  vbxe_bank(VBXE_OBJBANK);

	  t_ptr = &t_list[t];
	  if ((t_ptr->tval == TV_INVIS_TRAP) || (t_ptr->tval == TV_CLOSED_DOOR) || (t_ptr->tval == TV_VIS_TRAP)
	       || (t_ptr->tval == TV_OPEN_DOOR) || (t_ptr->tval == TV_SECRET_DOOR)) {
	    if (bank_delete_object(y, x, SPELLS3NUM)) {
		  bank_msg_print("There is a bright flash of light!", SPELLS3NUM);
		  destroy2 = TRUE;
		}
	  }
	  else if (t_ptr->tval == TV_CHEST) {
	    bank_msg_print("Click!", SPELLS3NUM);
	    t_ptr->flags &= ~(CH_TRAPPED|CH_LOCKED);
	    destroy2 = TRUE;
	    t_ptr->name2 = SN_UNLOCKED;
	    spells3_known2(t_ptr);
	  }
	}

	vbxe_bank(VBXE_CAVBANK);
  } while ((dist <= OBJ_BOLT_RANGE) || c_ptr->fval <= MAX_OPEN_SPACE);

  return(destroy2);
}


/* Polymorph a monster					-RAK-	*/
/* NOTE: cannot polymorph a winning creature (BALROG)		 */
int8u __fastcall__ poly_monster(int8u dir, int8u y, int8u x)
{
  int8u dist, flag, poly;
  register cave_type *c_ptr;
  register creature_type *r_ptr;
  register monster_type *m_ptr;
  //vtype out_val, m_name;
  int8u c;


  poly = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  c = c_ptr->cptr;

	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
	    flag = TRUE;
	    bank_delete_monster(c, SPELLS3NUM);
	    /* Place_monster() should always return TRUE here.  */
	    poly = bank_place_monster(y, x, randint(m_level[MAX_MONS_LEVEL]-m_level[0]) - 1 + m_level[0], FALSE, SPELLS3NUM);
	    /* don't test c_ptr->fm here, only pl/tl */

	    vbxe_bank(VBXE_CAVBANK);
	    if (poly && spells3_panel_contains(y, x) && ((c_ptr->flags & CAVE_TL) || (c_ptr->flags & CAVE_PL)))
		  poly = TRUE;
	  }
	  else {
	    spells3_monster_name(out_val2, m_ptr, r_ptr);
	    sprintf(out_val1, "%s is unaffected.", out_val2);
	    bank_msg_print(out_val1, SPELLS3NUM);
	  }
	}
  } while (!flag);

  return(poly);
}


/* Replicate a creature					-RAK-	*/
int8u __fastcall__ clone_monster(int8u dir, int8u y, int8u x)
{
  register cave_type *c_ptr;
  int8u dist, flag;
  int8u c;


  dist = 0;
  flag = FALSE;
  do {
    spells3_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  c = c_ptr->cptr;

	  vbxe_bank(VBXE_MONBANK);
	  m_list[c].csleep = 0;
	  /* monptr of 0 is safe here, since can't reach here from creatures */
	  return bank_multiply_monster(y, x, m_list[c].mptr, 0, SPELLS3NUM);
	}
  } while (!flag);

  return(FALSE);
}



