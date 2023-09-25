/* source/creature.c: handle monster movement and attacks

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


#pragma code-name("MON2BANK")
#pragma data-name("MON2BANK")
#pragma rodata-name("MON2BANK")


/* Prototypes of functions copied into this bank */
void __fastcall__ creature1_lite_spot(int8u y, int8u x);
void __fastcall__ creature1_move_rec(int8u y1, int8u x1, int8u y2, int8u x2);
int8u __fastcall__ creature1_damroll(int8u num, int8u sides);
int8u __fastcall__ creature1_distance(int8u y1, int8u x1, int8u y2, int8u x2);
int8u __fastcall__ creature1_mmove(int8u dir, int8u *y, int8u *x);
int8u __fastcall__ creature1_in_bounds(int8u y, int8u x);
void __fastcall__ creature1_invcopy(inven_type *to, int16u from_index);
int8 __fastcall__ movement_rate(int8 speed);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MON2NUM

#undef  INBANK_MOD
#define INBANK_MOD			creature1


void __fastcall__ creature1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

void __fastcall__ creature1_move_rec(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\move_rec.c"

int8u __fastcall__ creature1_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ creature1_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

int8u __fastcall__ creature1_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

int8u __fastcall__ creature1_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

void __fastcall__ creature1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"


/* Given speed,	 returns number of moves this turn.	-RAK-	  */
/* NOTE: Player must always move at least once per iteration, */
/*	 a slowed player is handled by moving monsters faster	  */
int8 __fastcall__ movement_rate(int8 speed)
{
  if (speed > 0) {
    if (py.flags.rest != 0)
	  return 1;
    else
	  return speed;
  }
  else {
    /* speed must be negative here */
    return ((turn % (2 - speed)) == 0);
  }
}


/* Make the move if possible, five choices		-RAK-	*/
void __fastcall__ make_move(int8u monptr, int8u *mm, int32u *rcmove)
{
  int8u i, newy, newx, do_turn, do_move, stuck_door;
  int32u movebits;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register inven_type *t_ptr;
  cave_type c;								// local copy of cave location
  int16 hp;									// local copy of monster hp


  i = 0;
  do_turn = FALSE;
  do_move = FALSE;

  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  movebits = c_list[m_ptr->mptr].cmove;
  hp = m_ptr->hp;

  do {
    vbxe_bank(VBXE_MONBANK);
    /* Get new position		*/
    newy = m_ptr->fy;
    newx = m_ptr->fx;
    creature1_mmove(mm[i], &newy, &newx);

    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(newy, newx);						// get the new cave location of monster
    c.cptr = c_ptr->cptr;								// make a local copy to avoid banking
    c.tptr = c_ptr->tptr;
    c.fval = c_ptr->fval;

    if (c_ptr->fval != BOUNDARY_WALL) {
	  /* Floor is open?		   */
	  if (c_ptr->fval <= MAX_OPEN_SPACE)
	    do_move = TRUE;
	  /* Creature moves through walls? */
	  else if (movebits & CM_PHASE) {
	    do_move = TRUE;
	    *rcmove |= CM_PHASE;
	  }
	  /* Creature can open doors? */
	  else if (c_ptr->tptr != 0) {
	    vbxe_bank(VBXE_OBJBANK);
	    t_ptr = &t_list[c.tptr];
        if (movebits & CM_OPEN_DOOR) {     /* Creature can open doors. */
		  stuck_door = FALSE;
		  if (t_ptr->tval == TV_CLOSED_DOOR) {
		    do_turn = TRUE;
		    if (t_ptr->p1 == 0)				/* Closed doors	*/
			  do_move = TRUE;
		    else if (t_ptr->p1 > 0) {	    /* Locked doors	*/
			  if (randint((hp+1)*(50+t_ptr->p1)) < 40*(hp-10-t_ptr->p1))
			    t_ptr->p1 = 0;
			}
		    else if (t_ptr->p1 < 0) {      	/* Stuck doors	*/
			  if (randint((hp+1)*(50-t_ptr->p1)) < 40*(hp-10+t_ptr->p1)) {
			    bank_msg_print("You hear a door burst open!", MON2NUM);
			    bank_disturb(1, 0, MON2NUM);
			    stuck_door = TRUE;
			    do_move = TRUE;
			  }
			}
		  }
		  else if (t_ptr->tval == TV_SECRET_DOOR) {
		    do_turn = TRUE;
		    do_move = TRUE;
		  }

		  if (do_move) {
		    creature1_invcopy(t_ptr, OBJ_OPEN_DOOR);
		    if (stuck_door) /* 50% chance of breaking door */
			  t_ptr->p1 = 1 - randint(2);

		    vbxe_bank(VBXE_CAVBANK);
		    c_ptr->fval = CORR_FLOOR;

		    creature1_lite_spot(newy, newx);
		    *rcmove |= CM_OPEN_DOOR;
		    do_move = FALSE;
		  }
		}
	    else {     /* Creature can not open doors, must bash them   */
		  if (t_ptr->tval == TV_CLOSED_DOOR) {
		    do_turn = TRUE;
		    if (randint((hp+1)*(80+abs(t_ptr->p1))) < 40*(hp-20-abs(t_ptr->p1))) {
			  creature1_invcopy(t_ptr, OBJ_OPEN_DOOR);
			  /* 50% chance of breaking door */
			  t_ptr->p1 = 1 - randint(2);

		      vbxe_bank(VBXE_CAVBANK);
			  c_ptr->fval = CORR_FLOOR;

			  creature1_lite_spot(newy, newx);
			  bank_msg_print("You hear a door burst open!", MON2NUM);
			  bank_disturb(1, 0, MON2NUM);
			}
		  }
		}
	  }
	  /* Glyph of warding present? */
	  vbxe_bank(VBXE_OBJBANK);
	  if (do_move && (c.tptr != 0) && (t_list[c.tptr].tval == TV_VIS_TRAP) && (t_list[c.tptr].subval == 99)) {
	    vbxe_bank(VBXE_MONBANK);
	    if (randint(OBJ_RUNE_PROT) < c_list[m_ptr->mptr].level) {
		  if ((newy==char_row) && (newx==char_col))
		    bank_msg_print("The rune of protection is broken!", MON2NUM);
		    bank_delete_object(newy, newx, MON2NUM);
		}
	    else {
		  do_move = FALSE;
		  /* If the creature moves only to attack, */
		  /* don't let it move if the glyph prevents */
		  /* it from attacking */
	  	  if (movebits & CM_ATTACK_ONLY)
		    do_turn = TRUE;
		}
	  }

	  /* Creature has attempted to move on player?	   */
	  vbxe_bank(VBXE_MONBANK);
	  if (do_move)
	    if (c.cptr == 1) {
		   /* if the monster is not lit, must call update_mon, it may be faster than character,
		      and hence could have just moved next to character this same turn */
		  if (!m_ptr->ml)
		    bank_update_mon(monptr, MON2NUM);
		  bank_make_attack(monptr);
		  do_move = FALSE;
		  do_turn = TRUE;
	    }
	    /* Creature is attempting to move on other creature?	   */
	    else if ((c.cptr > 1) && ((newy != m_ptr->fy) || (newx != m_ptr->fx))) {
		  /* Creature eats other creatures?	 */
		  if ((movebits & CM_EATS_OTHER) && (c_list[m_ptr->mptr].mexp >= c_list[m_list[c.cptr].mptr].mexp)) {
		    if (m_list[c_ptr->cptr].ml)
		      *rcmove |= CM_EATS_OTHER;
		      /* It ate an already processed monster. Handle normally. */
		      if (monptr < c.cptr)
		        bank_delete_monster(c.cptr, MON2NUM);
		      /* If it eats this monster, an already processed monster will take its place,
		         causing all kinds of havoc.  Delay the kill a bit. */
		      else
		        bank_fix1_delete_monster(c.cptr, MON2NUM);
		  }
		  else
		    do_move = FALSE;
	    }

	  /* Creature has been allowed move.	 */
	  if (do_move) {
	    /* Pick up or eat an object	       */
	    if (movebits & CM_PICKS_UP) {
		  vbxe_bank(VBXE_CAVBANK);
		  //c_ptr = CAVE_ADR(newy, newx);				//&cave[newy][newx];
		  //c.tptr = c_ptr->tptr;
		  c.tptr = (CAVE_ADR(newy, newx))->tptr;

	      vbxe_bank(VBXE_OBJBANK);
		  if ((c.tptr != 0) && (t_list[c.tptr].tval <= TV_MAX_OBJECT)) {
		    *rcmove |= CM_PICKS_UP;
		    bank_delete_object(newy, newx, MON2NUM);
		  }
		}

	    /* Move creature record		       */
	    vbxe_bank(VBXE_MONBANK);
	    creature1_move_rec(m_ptr->fy, m_ptr->fx, newy, newx);
	    if (m_ptr->ml) {
		  m_ptr->ml = FALSE;
		  creature1_lite_spot(m_ptr->fy, m_ptr->fx);
		}

		vbxe_bank(VBXE_MONBANK);

        //sprintf(dbgstr, "%c %d,%d new:%d,%d", c_list[m_ptr->mptr].cchar, m_ptr->fx, m_ptr->fy, newx, newy);
  	    //bank_prt(dbgstr, 23, 0, MON2NUM);
  	    //cgetc();

	    m_ptr->fy = newy;
	    m_ptr->fx = newx;
	    m_ptr->cdis = creature1_distance(char_row, char_col, newy, newx);
	    do_turn = TRUE;
	  }
	}
    ++i;
    /* Up to 5 attempts at moving, give up.	  */
  } while ((!do_turn) && (i < 5));
}


/* Move the critters about the dungeon			-RAK-	*/
void __fastcall__ mon_move(int8u monptr, int32u *rcmove)
{
  register int8u i, j;
  int8u k, move_test, dir;
  register creature_type *r_ptr;
  register monster_type *m_ptr;
  int8u mm[9];
  int16 rest_val;
  int8u fx, fy, fval;
  int8 x;
  int16u mon;


  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];
  mon = m_ptr->mptr;

  fx = m_ptr->fx;
  fy = m_ptr->fy;

  /* Does the critter multiply? */
  /* rest could be negative, to be safe, only use mod with positive values. */
  rest_val = abs(py.flags.rest);
  if ((r_ptr->cmove & CM_MULTIPLY) && (MAX_MON_MULT >= mon_tot_mult) && ((rest_val % MON_MULT_ADJ) == 0)) {
    k = 0;
    vbxe_bank(VBXE_CAVBANK);
    for (i = fy-1; i <= fy+1; ++i)
	  for (j = fx-1; j <= fx+1; ++j)
	    //if (in_bounds(i, j) && (cave[i][j].cptr > 1))
	    if (creature1_in_bounds(i, j) && ((CAVE_ADR(i,j))->cptr > 1))
	      ++k;

    //vbxe_restore_bank();

    /* can't call randint with a value of zero, increment counter to allow creature multiplication */
    if (k == 0)
	  ++k;
    if ((k < 4) && (randint(k*MON_MULT_ADJ) == 1)) {
	  vbxe_bank(VBXE_MONBANK);
      if (bank_multiply_monster(m_ptr->fy, m_ptr->fx, m_ptr->mptr, monptr, MON2NUM))
        *rcmove |= CM_MULTIPLY;
	}
  }
  move_test = FALSE;

  vbxe_bank(VBXE_CAVBANK);
  fval = (CAVE_ADR(fy, fx))->fval;
  vbxe_bank(VBXE_MONBANK);

  /* if in wall, must immediately escape to a clear area */
  //if (!(r_ptr->cmove & CM_PHASE) && (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL)) {
  if (!(r_ptr->cmove & CM_PHASE) && (fval >= MIN_CAVE_WALL)) {
      /* If the monster is already dead, don't kill it again!
	 This can happen for monsters moving faster than the player.  They
	 will get multiple moves, but should not if they die on the first
	 move.  This is only a problem for monsters stuck in rock.  */
    if (m_ptr->hp < 0)
	  return;

    k = 0;
    dir = 1;
    /* note direction of for loops matches direction of keypad from 1 to 9*/
    /* do not allow attack against the player */
    /* Must cast fy-1 to signed int, so that a nagative value of i will fail the comparison.  */
	vbxe_bank(VBXE_CAVBANK);
    for (x = fy+1; x >= fy-1; --x)					// changed to x, as it's int8.  max y is 44 -SJ
	  for (j = fx-1; j <= fx+1; ++j) {
	    //if ((dir != 5) && (cave[i][j].fval <= MAX_OPEN_SPACE) && (cave[i][j].cptr != 1))
	    if ((dir != 5) && ((CAVE_ADR(x,j))->fval <= MAX_OPEN_SPACE) && ((CAVE_ADR(x,j))->cptr != 1))
	      mm[++k] = dir;
	    ++dir;
	  }

    if (k != 0) {
	  /* put a random direction first */
	  dir = randint (k) - 1;
	  i = mm[0];
	  mm[0] = mm[dir];
	  mm[dir] = i;
	  make_move(monptr, mm, rcmove);
	  /* this can only fail if mm[0] has a rune of protection */
	}

    /* if still in a wall, let it dig itself out, but also apply some more damage */
    //if (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL) {
	vbxe_bank(VBXE_MONBANK);
	fx = m_ptr->fx;
	fy = m_ptr->fy;
	vbxe_bank(VBXE_CAVBANK);
    if ((CAVE_ADR(fy, fx))->fval >= MIN_CAVE_WALL) {
	  /* in case the monster dies, may need to call fix1_delete_monster() instead of delete_monsters() */
	  hack_monptr = monptr;
	  x = bank_mon_take_hit(monptr, creature1_damroll(8, 8), MON2NUM);
	  hack_monptr = -1;
	  if (x >= 0) {
	    bank_msg_print("You hear a scream muffled by rock!", MON2NUM);
	    bank_prt_experience(MON2NUM);
	  }
	  else {
	    bank_msg_print ("A creature digs itself out from the rock!", MON2NUM);
	    bank_twall(fy, fx, 1, 0, MON2NUM);
	  }
	}
    return;  /* monster movement finished */
  }
  /* Creature is confused?  Chance it becomes un-confused  */
  else if (m_ptr->confused) {
    mm[0] = randint(9);
    mm[1] = randint(9);
    mm[2] = randint(9);
    mm[3] = randint(9);
    mm[4] = randint(9);
    /* don't move him if he is not supposed to move! */
    if (!(r_ptr->cmove & CM_ATTACK_ONLY))
	  make_move(monptr, mm, rcmove);

    if (randint(8) == 1)
	  m_ptr->confused = FALSE;
    move_test = TRUE;
  }
  /* Creature may cast a spell */
  else if (r_ptr->spells != 0)
    bank_mon_cast_spell(monptr, &move_test, MON2NUM);

  if (!move_test) {
    /* 75% random movement */
    if ((r_ptr->cmove & CM_75_RANDOM) && (randint(100) < 75)) {
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  *rcmove |= CM_75_RANDOM;
	  make_move(monptr, mm, rcmove);
	}
    /* 40% random movement */
    else if ((r_ptr->cmove & CM_40_RANDOM) && (randint(100) < 40)) {
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  *rcmove |= CM_40_RANDOM;
	  make_move(monptr, mm, rcmove);
	}
    /* 20% random movement */
    else if ((r_ptr->cmove & CM_20_RANDOM) && (randint(100) < 20)) {
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  *rcmove |= CM_20_RANDOM;
	  make_move(monptr, mm, rcmove);
	}
    /* Normal movement */
    else if (r_ptr->cmove & CM_MOVE_NORMAL) {
	  if (randint(200) == 1) {
	    mm[0] = randint(9);
	    mm[1] = randint(9);
	    mm[2] = randint(9);
	    mm[3] = randint(9);
	    mm[4] = randint(9);
	  }
	  else
	    bank_get_moves(monptr, mm);

	  *rcmove |= CM_MOVE_NORMAL;
	  make_move(monptr, mm, rcmove);
	}
    /* Attack, but don't move */
    else if (r_ptr->cmove & CM_ATTACK_ONLY) {
	  if (m_ptr->cdis < 2) {
	    bank_get_moves(monptr, mm);
	    make_move(monptr, mm, rcmove);
	  }
	  else
	    /* Learn that the monster does does not move when it should have moved, but didn't.  */
	    *rcmove |= CM_ATTACK_ONLY;
	}
    else if ((r_ptr->cmove & CM_ONLY_MAGIC) && (m_ptr->cdis < 2)) {
	  vbxe_bank(VBXE_GENBANK);
	  /* A little hack for Quylthulgs, so that one will eventually notice that they have no physical attacks.  */
	  if (c_recall[mon].r_attacks[0] < MAX_UCHAR)
	    c_recall[mon].r_attacks[0]++;
	  /* Another little hack for Quylthulgs, so that one can eventually learn their speed.  */
	  if (c_recall[mon].r_attacks[0] > 20)
	    c_recall[mon].r_cmove |= CM_ONLY_MAGIC;
	}
  }
}


/* Creatures movement and attacking are done from here	-RAK-	*/
void __fastcall__ creatures(int8u attack)
{
  register int8u i;
  register int8 k;
  register monster_type *m_ptr;
  //recall_type *r_ptr;
  int32u notice;
  int32u rcmove;					// CHECK: monster recall
  int8u wake, ignore;
  int8u fval;						// save fval from cave
  int8u fx, fy;
  //vtype cdesc;
  int16u mon;


  /* Process the monsters  */
  for (i = mfptr - 1; i >= MIN_MONIX && !death; --i) {
    vbxe_bank(VBXE_MONBANK);
    m_ptr = &m_list[i];
    fy = m_ptr->fy;
    fx = m_ptr->fx;

    vbxe_bank(VBXE_CAVBANK);
    fval = (CAVE_ADR(fy, fx))->fval;							    //cave[m_ptr->fy][m_ptr->fx].fval
    vbxe_restore_bank();

    /* Get rid of an eaten/breathed on monster.  Note: Be sure not to
	   process this monster. This is necessary because we can't delete
	   monsters while scanning the m_list here. */
    if (m_ptr->hp < 0) {
	  bank_fix2_delete_monster(i, MON2NUM);
	  continue;
	}

    m_ptr->cdis = creature1_distance(char_row, char_col, m_ptr->fy, m_ptr->fx);

    if (attack) {  /* Attack is argument passed to CREATURE*/
	  k = movement_rate(m_ptr->cspeed);

	  //sprintf(dbgstr, "%c - speed: %d", c_list[m_ptr->mptr].cchar, k);
	  //bank_prt(dbgstr, 23, 0, MON2NUM);
	  //cgetc();

	  if (k <= 0)
	    bank_update_mon(i, MON2NUM);
	  else
	    while (k > 0) {
		  --k;
		  wake = FALSE;
		  ignore = FALSE;
		  rcmove = 0;

		  vbxe_bank(VBXE_MONBANK);

		  if (m_ptr->ml || (m_ptr->cdis <= c_list[m_ptr->mptr].aaf)
		      /* Monsters trapped in rock must be given a turn also, so that they will die/dig out immediately.  */
		      || ((!(c_list[m_ptr->mptr].cmove & CM_PHASE)) && fval >= MIN_CAVE_WALL)) {
		    if (m_ptr->csleep > 0)
		      if (py.flags.aggravate)
			    m_ptr->csleep = 0;
		      else if ((py.flags.rest == 0 && py.flags.paralysis < 1) || (randint(50) == 1)) {
			    notice = (int32u) randint(1024);
			    if (notice*notice*notice <= (1L << (29 - (int32u) py.misc.stl))) {
			      m_ptr->csleep -= (100 / m_ptr->cdis);
			      if (m_ptr->csleep > 0)
				    ignore = TRUE;
			      else {
				    wake = TRUE;
				    /* force it to be exactly zero */
				    m_ptr->csleep = 0;
				  }
			    }
			  }

		    if (m_ptr->stunned != 0) {
			  /* NOTE: Balrog = 100*100 = 10000, it always recovers instantly */
			  if (randint(5000) < c_list[m_ptr->mptr].level * c_list[m_ptr->mptr].level)
			    m_ptr->stunned = 0;
			  else
			    m_ptr->stunned--;

			  if (m_ptr->stunned == 0) {
			    if (!m_ptr->ml)
			      strcpy(out_val1, "It ");
			    else
			      sprintf(out_val1, "The %s ", c_list[m_ptr->mptr].name);
			    bank_msg_print(strcat(out_val1, "recovers and glares at you."), MON2NUM);
			  }
		    }

		    if ((m_ptr->csleep == 0) && (m_ptr->stunned == 0))
		      mon_move(i, &rcmove);
		  }

		  bank_update_mon(i, MON2NUM);

		  if (m_ptr->ml) {
			vbxe_bank(VBXE_MONBANK);
			mon = m_ptr->mptr;
			vbxe_bank(VBXE_GENBANK);
		    if (wake) {
			  if (c_recall[mon].r_wake < MAX_UCHAR)
			    c_recall[mon].r_wake++;
		    }
		    else if (ignore) {
			  if (c_recall[mon].r_ignore < MAX_UCHAR)
			    c_recall[mon].r_ignore++;
		    }
		    c_recall[mon].r_cmove |= rcmove;
		  }
	    }
	  }
      else
	    bank_update_mon(i, MON2NUM);

      vbxe_bank(VBXE_MONBANK);						// make sure monbank is in -SJ

      /* Get rid of an eaten/breathed on monster.  This is necessary because we can't delete monsters
         while scanning the m_list here.  This monster may have been killed during mon_move(). */
      if (m_ptr->hp < 0) {
	    bank_fix2_delete_monster(i, MON2NUM);
	    continue;
	  }
  }
  /* End processing monsters */
}

