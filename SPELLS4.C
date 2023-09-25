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


#pragma code-name("SPELLS4BANK")
#pragma data-name("SPELLS4BANK")
#pragma rodata-name("SPELLS4BANK")


/* Local Prototypes */
int8u __fastcall__ spells4_panel_contains(int8u y, int8u x);
int8u __fastcall__ spells4_in_bounds(int8u y, int8u x);
void __fastcall__ spells4_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr);
int8u __fastcall__ spells4_mmove(int8u dir, int8u *y, int8u *x);
int8u __fastcall__ spells4_test_light(int8u y, int8u x);
void __fastcall__ spells4_lite_spot(int8u y, int8u x);
int8u __fastcall__ spells4_damroll(int8u num, int8u sides);
int8u __fastcall__ spells4_los(int8u fromY, int8u fromX, int8u toY, int8u toX);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		SPELLS4NUM

#undef  INBANK_MOD
#define INBANK_MOD			spells4


int8u __fastcall__ spells4_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ spells4_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

void __fastcall__ spells4_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
#include "inbank\monster_name.c"

int8u __fastcall__ spells4_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8u __fastcall__ spells4_test_light(int8u y, int8u x)
#include "inbank\test_light.c"

void __fastcall__ spells4_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ spells4_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ spells4_los(int8u fromY, int8u fromX, int8u toY, int8u toX)
#include "inbank\los.c"


/* Turn stone to mud, delete wall.			-RAK-	*/
int8u __fastcall__ wall_to_mud(int8u dir, int8u y, int8u x)
{
  int8u wall, dist;
  int16 i;
  vtype tmp_str;
  int8u flag;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u c, t;
  int16u mon;


  wall = FALSE;
  flag = FALSE;
  dist = 0;
  do {
    spells4_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    /* note, this ray can move through walls as it turns them to mud */
    if (dist == OBJ_BOLT_RANGE)
	  flag = TRUE;

    if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL)) {
	  flag = TRUE;
	  bank_twall(y, x, 1, 0, SPELLS4NUM);
	  if (spells4_test_light(y, x)) {
	    bank_msg_print("The wall turns into mud.", SPELLS4NUM);
	    wall = TRUE;
	  }
	}
    else if ((c_ptr->tptr != 0) && (c_ptr->fval >= MIN_CLOSED_SPACE)) {
	  flag = TRUE;
	  if (spells4_panel_contains(y, x) && spells4_test_light(y, x)) {
		t = c_ptr->tptr;
		vbxe_bank(VBXE_OBJBANK);
	    bank_objdes(tmp_str, &t_list[t], FALSE, SPELLS4NUM);

	    sprintf(out_val1, "The %s turns into mud.", tmp_str);
	    bank_msg_print(out_val1, SPELLS4NUM);
	    wall = TRUE;
	  }
	  bank_delete_object(y, x, SPELLS4NUM);
	}

    vbxe_bank(VBXE_CAVBANK);
    if (c_ptr->cptr > 1) {
	  c = c_ptr->cptr;
	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[c];
	  r_ptr = &c_list[m_ptr->mptr];
	  mon = m_ptr->mptr;

	  if (CD_STONE & r_ptr->cdefense) {
	    spells4_monster_name(out_val2, m_ptr, r_ptr);
	    i = bank_mon_take_hit(c, 100, SPELLS4NUM);
	    /* Should get these messages even if the monster is not visible. */
	    if (i >= 0) {
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[i].r_cdefense |= CD_STONE;

		  sprintf(out_val1, "%s dissolves!", out_val2);
		  bank_msg_print(out_val1, SPELLS4NUM);
		  bank_prt_experience(SPELLS4NUM);  /* print msg before calling prt_exp */
		}
	    else {
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cdefense |= CD_STONE;
		  sprintf(out_val1, "%s grunts in pain!", out_val2);
		  bank_msg_print(out_val1, SPELLS4NUM);
		}

	    flag = TRUE;
	  }
	}
  } while (!flag);

  return(wall);
}



/* Create a wall.					-RAK-	*/
int8u __fastcall__ build_wall(int8u dir, int8u y, int8u x)
{
  int8 i;
  int8u build, dist, flag;
  int16 damage;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  //vtype m_name, out_val;
  int8u c;


  build = FALSE;
  dist = 0;
  flag = FALSE;
  do {
    spells4_mmove(dir, &y, &x);
    ++dist;

    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else {
	  if (c_ptr->tptr != 0)
	    bank_delete_object(y, x, SPELLS4NUM);

	  if (c_ptr->cptr > 1) {
		c = c_ptr->cptr;

	    /* stop the wall building */
	    flag = TRUE;

	    vbxe_bank(VBXE_MONBANK);
	    m_ptr = &m_list[c];
	    r_ptr = &c_list[m_ptr->mptr];

        if (!(r_ptr->cmove & CM_PHASE)) {
		  /* monster does not move, can't escape the wall */
		  if (r_ptr->cmove & CM_ATTACK_ONLY)
		    damage = 3000; /* this will kill everything */
		  else
		    damage = spells4_damroll(4, 8);

		  spells4_monster_name(out_val2, m_ptr, r_ptr);
		  sprintf(out_val1, "%s wails out in pain!", out_val2);
		  bank_msg_print(out_val1, SPELLS4NUM);

		  i = bank_mon_take_hit(c, damage, SPELLS4NUM);
		  if (i >= 0) {
		    sprintf(out_val1, "%s is embedded in the rock.", out_val2);
		    bank_msg_print (out_val1, SPELLS4NUM);
		    bank_prt_experience(SPELLS4NUM);
		  }
		}
	    else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
		  /* must be an earth elemental or an earth spirit, or a Xorn increase its hit points */
		  m_ptr->hp += spells4_damroll(4, 8);
		}
	  }

	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr->fval  = MAGMA_WALL;
	  c_ptr->flags &= ~CAVE_FM;
	  /* Permanently light this wall if it is lit by player's lamp.  */
	  //c_ptr->pl = (c_ptr->tl || c_ptr->pl);
	  if ((c_ptr->flags & CAVE_TL) || (c_ptr->flags & CAVE_PL))
	    c_ptr->flags |= CAVE_PL;
	  else
	    c_ptr->flags &= ~CAVE_PL;

	  spells4_lite_spot(y, x);
	  ++i;								// CHECK: need this? -SJ
	  build = TRUE;
	}
  } while (!flag);

  return(build);
}


/* Change speed of any creature .			-RAK-	*/
/* NOTE: cannot slow a winning creature (BALROG)		 */
int8u __fastcall__ speed_monsters(int8u spd)
{
  register int8u i;
  int8u speed;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  //vtype out_val, m_name;


  speed = FALSE;
  vbxe_bank(VBXE_MONBANK);
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    r_ptr = &c_list[m_ptr->mptr];
    spells4_monster_name(out_val2, m_ptr, r_ptr);

    if ((m_ptr->cdis > MAX_SIGHT) || !spells4_los(char_row, char_col, m_ptr->fy, m_ptr->fx))
	  ;		/* do nothing */
    else if (spd > 0) {
	  m_ptr->cspeed += spd;
	  m_ptr->csleep = 0;
	  if (m_ptr->ml) {
	    speed = TRUE;
	    sprintf(out_val1, "%s starts moving faster.", out_val2);
	    bank_msg_print(out_val1, SPELLS4NUM);
	  }
	}
    else if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
	  m_ptr->cspeed += spd;
	  m_ptr->csleep = 0;
	  if (m_ptr->ml) {
	    sprintf(out_val1, "%s starts moving slower.", out_val2);
	    bank_msg_print(out_val1, SPELLS4NUM);
	    speed = TRUE;
	  }
	}
    else if (m_ptr->ml) {
	  m_ptr->csleep = 0;
	  sprintf(out_val1, "%s is unaffected.", out_val2);
	  bank_msg_print(out_val1, SPELLS4NUM);
	}
  }

  return(speed);
}


/* Sleep any creature .		-RAK-	*/
int8u __fastcall__ sleep_monsters2(void)
{
  register int8u i;
  int8u sleep;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int16u mon;


  sleep = FALSE;
  vbxe_bank(VBXE_MONBANK);
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    r_ptr = &c_list[m_ptr->mptr];
    mon = m_ptr->mptr;

    spells4_monster_name(out_val2, m_ptr, r_ptr);
    if ((m_ptr->cdis > MAX_SIGHT) || !spells4_los(char_row, char_col, m_ptr->fy, m_ptr->fx))
	  ;		/* do nothing */
    else if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
	  if (m_ptr->ml) {
	    if (r_ptr->cdefense & CD_NO_SLEEP) {
 		  vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cdefense |= CD_NO_SLEEP;
		}
	    sprintf(out_val1, "%s is unaffected.", out_val2);
	    bank_msg_print(out_val1, SPELLS4NUM);
	  }
	}
    else {
	  m_ptr->csleep = 500;
	  if (m_ptr->ml) {
	    sprintf(out_val1, "%s falls asleep.", out_val2);
	    bank_msg_print(out_val1, SPELLS4NUM);
	    sleep = TRUE;
	  }
	}
  }

  return(sleep);
}


/* Polymorph any creature that player can see.	-RAK-	*/
/* NOTE: cannot polymorph a winning creature (BALROG)		 */
int8u __fastcall__ mass_poly(void)
{
  register int8u i;
  int8u y, x, mass;
  register monster_type *m_ptr;
  register creature_type *r_ptr;


  mass = FALSE;
  vbxe_bank(VBXE_MONBANK);
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    if (m_ptr->cdis <= MAX_SIGHT) {
	  r_ptr = &c_list[m_ptr->mptr];
	  if ((r_ptr->cmove & CM_WIN) == 0) {
	    y = m_ptr->fy;
	    x = m_ptr->fx;
	    bank_delete_monster(i, SPELLS4NUM);
	    /* Place_monster() should always return TRUE here.  */
	    mass = bank_place_monster(y, x, randint(m_level[MAX_MONS_LEVEL]-m_level[0]) - 1 + m_level[0], FALSE, SPELLS4NUM);
	  }
	}
  }

  return(mass);
}


/* This is a fun one.  In a given block, pick some walls and	*/
/* turn them into open spots.  Pick some open spots and turn	 */
/* them into walls.  An "Earthquake" effect.	       -RAK-   */
void __fastcall__ earthquake(void)
{
  register int8u i, j;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  creature_type *r_ptr;
  int16 damage;
  int8u tmp;
  //vtype out_val, m_name;
  int8u c;
  int8 h;


  for (i = char_row-8; i <= char_row+8; ++i)
    for (j = char_col-8; j <= char_col+8; ++j)
      if (((i != char_row) || (j != char_col)) && spells4_in_bounds(i, j) && (randint(8) == 1)) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr = CAVE_ADR(i, j);
	    if (c_ptr->tptr != 0)
	      bank_delete_object(i, j, SPELLS4NUM);

	    vbxe_bank(VBXE_CAVBANK);
	    if (c_ptr->cptr > 1) {
		  c = c_ptr->cptr;

		  vbxe_bank(VBXE_MONBANK);
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];

	      if (!(r_ptr->cmove & CM_PHASE)) {
		    if(r_ptr->cmove & CM_ATTACK_ONLY)
		      damage = 3000; /* this will kill everything */
		    else
		      damage = spells4_damroll(4, 8);

		    spells4_monster_name(out_val2, m_ptr, r_ptr);
		    sprintf(out_val1, "%s wails out in pain!", out_val2);
		    bank_msg_print(out_val1, SPELLS4NUM);
		    h = bank_mon_take_hit(c, damage, SPELLS4NUM);
		    if (h >= 0) {
		      sprintf(out_val1, "%s is embedded in the rock.", out_val2);
		      bank_msg_print(out_val1, SPELLS4NUM);
		      bank_prt_experience(SPELLS4NUM);
		    }
		  }
	      else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
		  /* must be an earth elemental or an earth spirit, or a Xorn increase its hit points */
		    m_ptr->hp += spells4_damroll(4, 8);
		  }
	    }

        vbxe_bank(VBXE_CAVBANK);
	    if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL)) {
	      c_ptr->fval  = CORR_FLOOR;
	      c_ptr->flags &= ~CAVE_PL;
	      c_ptr->flags &= ~CAVE_FM;
	    }
	    else if (c_ptr->fval <= MAX_CAVE_FLOOR) {
	      tmp = randint(10);
	      if (tmp < 6)
		    c_ptr->fval  = QUARTZ_WALL;
	      else if (tmp < 9)
		    c_ptr->fval  = MAGMA_WALL;
	      else
		    c_ptr->fval  = GRANITE_WALL;

	      c_ptr->flags &= ~CAVE_FM;
	    }

	    spells4_lite_spot(i, j);
	}
}


/* Attempt to turn (confuse) undead creatures.	-RAK-	*/
//int8u __fastcall__ turn_undead(void)
void __fastcall__ turn_undead(void)
{
  register int8u i;
  //int8u turn_und;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int16u mon;


  //turn_und = FALSE;
  vbxe_bank(VBXE_MONBANK);
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    r_ptr = &c_list[m_ptr->mptr];
    mon = m_ptr->mptr;

    if ((m_ptr->cdis <= MAX_SIGHT) && (CD_UNDEAD & r_ptr->cdefense) && (spells4_los(char_row, char_col, m_ptr->fy, m_ptr->fx))) {
	  spells4_monster_name(out_val2, m_ptr, r_ptr);
	  if (((py.misc.lev+1) > r_ptr->level) || (randint(5) == 1)) {
	    if (m_ptr->ml) {
		  sprintf(out_val1, "%s runs frantically!", out_val2);
		  bank_msg_print(out_val1, SPELLS4NUM);
		  //turn_und = TRUE;
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cdefense |= CD_UNDEAD;
		}

	    m_ptr->confused = TRUE;
	  }
	  else if (m_ptr->ml) {
	    sprintf(out_val1, "%s is unaffected.", out_val2);
	    bank_msg_print(out_val1, SPELLS4NUM);
	  }
	}
  }

  //return(turn_und);
  return;
}


/* Display all creatures on the current panel		-RAK-	*/
int8u __fastcall__ detect_monsters(void)
{
  register int8u i;
  int8u detect;
  register monster_type *m_ptr;


  vbxe_bank(VBXE_MONBANK);
  detect = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    if (spells4_panel_contains(m_ptr->fy, m_ptr->fx) && ((CM_INVISIBLE & c_list[m_ptr->mptr].cmove) == 0)) {
	  m_ptr->ml = TRUE;
	  /* works correctly even if hallucinating */
	  bank_print(c_list[m_ptr->mptr].cchar, m_ptr->fy, m_ptr->fx, SPELLS4NUM);
	  detect = TRUE;
	}
  }

  if (detect) {
    bank_msg_print("You sense the presence of monsters!", SPELLS4NUM);
    bank_msg_print("", SPELLS4NUM);
    /* must unlight every monster just lighted */
    bank_creatures(FALSE, SPELLS4NUM);
  }

  return(detect);
}


/* Teleport all creatures in a given direction away	-RAK-	*/
int8u __fastcall__ teleport_monster(int8u dir, int8u y, int8u x)
{
  int8u flag, result, dist;
  register cave_type *c_ptr;
  int8u c;


  flag = FALSE;
  result = FALSE;
  dist = 0;
  do {
    spells4_mmove(dir, &y, &x);
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    c = c_ptr->cptr;
    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else if (c_ptr->cptr > 1) {
	  vbxe_bank(VBXE_MONBANK);
	  m_list[c].csleep = 0; /* wake it up */
	  bank_teleport_away(c, MAX_SIGHT, SPELLS4NUM);
	  result = TRUE;
	}
  } while (!flag);

  return(result);
}


/* Display evil creatures on current panel		-RAK-	*/
int8u __fastcall__ detect_evil(void)
{
  register int8u i;
  int8u flag;
  register monster_type *m_ptr;


  flag = FALSE;
  vbxe_bank(VBXE_MONBANK);
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    if (spells4_panel_contains(m_ptr->fy, m_ptr->fx) && (CD_EVIL & c_list[m_ptr->mptr].cdefense)) {
	  m_ptr->ml = TRUE;
	  /* works correctly even if hallucinating */
	  bank_print(c_list[m_ptr->mptr].cchar, m_ptr->fy, m_ptr->fx, SPELLS4NUM);
	  flag = TRUE;
	}
  }

  if (flag) {
    bank_msg_print("You sense the presence of evil!", SPELLS4NUM);
    bank_msg_print("", SPELLS4NUM);
    /* must unlight every monster just lighted */
    bank_creatures(FALSE, SPELLS4NUM);
  }

  return(flag);
}
