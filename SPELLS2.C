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


#pragma code-name("SPELLS2BANK")
#pragma data-name("SPELLS2BANK")
#pragma rodata-name("SPELLS2BANK")


/* Local Prototypes */
int8u __fastcall__ spells2_in_bounds(int8u y, int8u x);
int8u __fastcall__ spells2_panel_contains(int8u y, int8u x);
int8u __fastcall__ spells2_distance(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ spells2_lite_spot(int8u y, int8u x);
int8u __fastcall__ spells2_los(int8u fromY, int8u fromX, int8u toY, int8u toX);
void __fastcall__ spells2_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr);
void __fastcall__ spells2_lower_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr);
int8u __fastcall__ spells2_mmove(int8u dir, int8u *y, int8u *x);
int8u __fastcall__ spells2_damroll(int8u num, int8u sides);
void __fastcall__ light_line(int8u dir, int8u y, int8u x);
void __fastcall__ get_flags(int8u typ, int32u *weapon_type, int8u *harm_type, int8u __fastcall__ (**destroy)(inven_type *item));


#undef  INBANK_RETBANK
#define INBANK_RETBANK		SPELLS2NUM

#undef  INBANK_MOD
#define INBANK_MOD			spells2


int8u __fastcall__ spells2_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

int8u __fastcall__ spells2_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ spells2_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

void __fastcall__ spells2_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ spells2_los(int8u fromY, int8u fromX, int8u toY, int8u toX)
#include "inbank\los.c"

void __fastcall__ spells2_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
#include "inbank\monster_name.c"

void __fastcall__ spells2_lower_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
#include "inbank\lower_monster_name.c"

int8u __fastcall__ spells2_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8u __fastcall__ spells2_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"


/* Leave a line of light in given dir, blue light can sometimes	*/
/* hurt creatures.				       -RAK-   */
void __fastcall__ light_line(int8u dir, int8u y, int8u x)
{
  int8 i;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u dist, flag;
  int8u c;
  int16u mon;


  //dist = -1;
  dist = 255;					// will rollover to 0, since it's now an int8u -SJ
  flag = FALSE;
  do {
    /* put mmove at end because want to light up current spot */
    ++dist;
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else {
	  if (!(c_ptr->flags & CAVE_PL) && !(c_ptr->flags & CAVE_TL)) {
	    /* set pl so that lite_spot will work */
	    c_ptr->flags |= CAVE_PL;
	    if (c_ptr->fval == LIGHT_FLOOR) {
		  if (spells2_panel_contains(y, x))
		    bank_light_room(y, x, SPELLS2NUM);
		}
	    else
		  spells2_lite_spot(y, x);
	  }

	  /* set pl in case tl was true above */
      vbxe_bank(VBXE_CAVBANK);
	  c_ptr->flags |= CAVE_PL;
	  if (c_ptr->cptr > 1) {
		c = c_ptr->cptr;
		vbxe_bank(VBXE_MONBANK);
	    m_ptr = &m_list[c];
	    r_ptr = &c_list[m_ptr->mptr];
	    mon = m_ptr->mptr;

	    /* light up and draw monster */
	    bank_update_mon(c, SPELLS2NUM);
	    spells2_monster_name(out_val1, m_ptr, r_ptr);

	    if (CD_LIGHT & r_ptr->cdefense) {
		  if (m_ptr->ml) {
			vbxe_bank(VBXE_GENBANK);
		    c_recall[mon].r_cdefense |= CD_LIGHT;
		    vbxe_restore_bank();
	      }
		  i = bank_mon_take_hit(c, spells2_damroll(2, 8), SPELLS2NUM);
		  if (i >= 0) {
		    sprintf(out_val2, "%s shrivels away in the light!", out_val1);
		    bank_msg_print(out_val2, SPELLS2NUM);
		    bank_prt_experience(SPELLS2NUM);
		  }
		  else {
		    sprintf(out_val2, "%s cringes from the light!", out_val1);
		    bank_msg_print(out_val2, SPELLS2NUM);
		  }
		}
	  }
	}

    spells2_mmove(dir, &y, &x);
  } while (!flag);
}


/* Light line in all directions				-RAK-	*/
void __fastcall__ starlite(int8u y, int8u x)
{
  register int8u i;

  if (py.flags.blind < 1)
    bank_msg_print("The end of the staff bursts into a blue shimmering light.", SPELLS2NUM);
  for (i = 1; i <= 9; ++i)
    if (i != 5)
      light_line(i, y, x);
}


int8u __fastcall__ spells2_set_lightning_destroy(inven_type *item)
{
  if ((item->tval == TV_RING) || (item->tval == TV_WAND) || (item->tval == TV_SPIKE))
    return(TRUE);
  else
    return(FALSE);
}


#pragma warn(unused-param, push, off)
int8u __fastcall__ set_null(inven_type *item)
{
  return(FALSE);
}
#pragma warn(unused-param, pop)


int8u __fastcall__ set_acid_destroy(inven_type *item)
{
  switch(item->tval)
    {
    case TV_ARROW: case TV_BOW: case TV_HAFTED: case TV_POLEARM:
    case TV_BOOTS: case TV_GLOVES: case TV_CLOAK: case TV_HELM:
    case TV_SHIELD: case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
      if (item->flags & TR_RES_ACID)
	return FALSE;
      else
	return TRUE;
    case TV_STAFF:
    case TV_SCROLL1: case TV_SCROLL2: case TV_FOOD: case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
      return(TRUE);
    }
  return(FALSE);
}


int8u __fastcall__ set_fire_destroy(inven_type *item)
{
  switch(item->tval)
    {
    case TV_ARROW: case TV_BOW: case TV_HAFTED: case TV_POLEARM:
    case TV_BOOTS: case TV_GLOVES: case TV_CLOAK: case TV_SOFT_ARMOR:
      if (item->flags & TR_RES_FIRE)
	return FALSE;
      else
	return TRUE;
    case TV_STAFF: case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1:
    case TV_POTION2: case TV_FLASK: case TV_FOOD: case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
      return(TRUE);
    }
  return(FALSE);
}


int8u __fastcall__ spells2_set_frost_destroy(inven_type *item)
{
  if ((item->tval == TV_POTION1) || (item->tval == TV_POTION2) || (item->tval == TV_FLASK))
    return(TRUE);
  return(FALSE);
}


/* Return flags for given type area affect		-RAK-	*/
void __fastcall__ get_flags(int8u typ, int32u *weapon_type, int8u *harm_type, int8u __fastcall__ (**destroy)(inven_type *item))
{
  switch(typ) {
    case GF_MAGIC_MISSILE:
      *weapon_type = 0;
      *harm_type   = 0;
      *destroy	   = set_null;
      break;
    case GF_LIGHTNING:
      *weapon_type = CS_BR_LIGHT;
      *harm_type   = CD_LIGHT;
      *destroy	   = spells2_set_lightning_destroy;
      break;
    case GF_POISON_GAS:
      *weapon_type = CS_BR_GAS;
      *harm_type   = CD_POISON;
      *destroy	   = set_null;
      break;
    case GF_ACID:
      *weapon_type = CS_BR_ACID;
      *harm_type   = CD_ACID;
      *destroy	   = set_acid_destroy;
      break;
    case GF_FROST:
      *weapon_type = CS_BR_FROST;
      *harm_type   = CD_FROST;
      *destroy	   = spells2_set_frost_destroy;
      break;
    case GF_FIRE:
      *weapon_type = CS_BR_FIRE;
      *harm_type   = CD_FIRE;
      *destroy	   = set_fire_destroy;
      break;
    case GF_HOLY_ORB:
      *weapon_type = 0;
      *harm_type   = CD_EVIL;
      *destroy	   = set_null;
      break;
    default:
      bank_msg_print("ERROR in get_flags()\n", SPELLS2NUM);
    }
}


/* Shoot a bolt in a given direction			-RAK-	*/
void __fastcall__ fire_bolt(int8u typ, int8u dir, int8u y, int8u x, int8u dam, char *bolt_typ)
{
  int8u i, oldy, oldx, dist, flag;
  int32u weapon_type;
  int8u harm_type;
  int8u __fastcall__ (*dummy)(inven_type *item);
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u c;
  int8 h;
  int16u mon;


  flag = FALSE;
  get_flags(typ, &weapon_type, &harm_type, &dummy);
  oldy = y;
  oldx = x;
  dist = 0;
  do {
    spells2_mmove(dir, &y, &x);
    ++dist;
    spells2_lite_spot(oldy, oldx);
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);
    if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	  flag = TRUE;
    else {
	  c = c_ptr->cptr;
	  if (c > 1) {
	    flag = TRUE;
	    vbxe_bank(VBXE_MONBANK);
	    m_ptr = &m_list[c];
	    r_ptr = &c_list[m_ptr->mptr];
	    mon = m_ptr->mptr;

	    /* light up monster and draw monster, temporarily set pl so that update_mon() will work */
	    vbxe_bank(VBXE_CAVBANK);
	    i = c_ptr->flags;					// CHECK: save only PL? -SJ
	    c_ptr->flags |= CAVE_PL;
	    bank_update_mon(c, SPELLS2NUM);

	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr->flags = i;					// CHECK: restore only PL? -SJ

        vbxe_bank(VBXE_MONBANK);
        spells2_lower_monster_name(out_val1, m_ptr, r_ptr);
        sprintf(out_val2, "The %s strikes %s.", bolt_typ, out_val1);
	    bank_msg_print(out_val2, SPELLS2NUM);
	    if (harm_type & r_ptr->cdefense) {
		  dam = dam*2;
		  if (m_ptr->ml) {
		    vbxe_bank(VBXE_GENBANK);
		    c_recall[mon].r_cdefense |= harm_type;
		    vbxe_restore_bank();
	      }
		}
	    else if (weapon_type & r_ptr->spells) {
		  dam = dam / 4;
		  if (m_ptr->ml) {
		    vbxe_bank(VBXE_GENBANK);
		    c_recall[mon].r_spells |= weapon_type;
		    vbxe_restore_bank();
	      }
		}

	    spells2_monster_name(out_val1, m_ptr, r_ptr);
	    h = bank_mon_take_hit(c, dam, SPELLS2NUM);
	    if (h >= 0) {
		  sprintf(out_val2, "%s dies in a fit of agony.", out_val1);
		  bank_msg_print(out_val2, SPELLS2NUM);
		  bank_prt_experience(SPELLS2NUM);
		}
	    else if (dam > 0) {
		  sprintf(out_val2, "%s screams in agony.", out_val1);
		  bank_msg_print(out_val2, SPELLS2NUM);
		}
	  }
	  else if (spells2_panel_contains(y, x) && (py.flags.blind < 1)) {
	    bank_print('*', y, x, SPELLS2NUM);
      }
	}

    oldy = y;
    oldx = x;
  } while (!flag);
}


/* Shoot a ball in a given direction.  Note that balls have an	*/
/* area affect.					      -RAK-   */
void __fastcall__ fire_ball(int8u typ, int8u dir, int8u y, int8u x, int8u dam_hp, char *descrip)
{
  register int8u i, j;
  int8u dam, max_dis, thit, tkill, tmp;
  int8u oldy, oldx, dist, flag, harm_type;
  int32u weapon_type;
  int8u __fastcall__ (*destroy)(inven_type *item);
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  creature_type *r_ptr;
  int8u c, t;
  int8 h;
  int16u mon;


  thit	 = 0;
  tkill	 = 0;
  max_dis = 2;
  get_flags(typ, &weapon_type, &harm_type, &destroy);
  flag = FALSE;
  oldy = y;
  oldx = x;
  dist = 0;
  do {
    spells2_mmove(dir, &y, &x);
    ++dist;
    spells2_lite_spot(oldy, oldx);
    if (dist > OBJ_BOLT_RANGE)
	  flag = TRUE;
    else {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(y, x);
	  c = c_ptr->cptr;					// save cptr -SJ
	  if ((c_ptr->fval >= MIN_CLOSED_SPACE) || (c_ptr->cptr > 1)) {
	    flag = TRUE;
	    if (c_ptr->fval >= MIN_CLOSED_SPACE) {
		  y = oldy;
		  x = oldx;
		}

	    /* The ball hits and explodes. */
	    /* The explosion.			   */
	    for (i = y-max_dis; i <= y+max_dis; ++i)
		  for (j = x-max_dis; j <= x+max_dis; ++j)
		    if (spells2_in_bounds(i, j) && (spells2_distance(y, x, i, j) <= max_dis) && spells2_los(y, x, i, j)) {
		      vbxe_bank(VBXE_CAVBANK);
		      c_ptr = CAVE_ADR(i, j);
		      t = c_ptr->tptr;

		      vbxe_bank(VBXE_OBJBANK);
		      if ((t != 0) && (*destroy)(&t_list[t]))
			    bank_delete_object(i, j, SPELLS2NUM);

			  vbxe_bank(VBXE_CAVBANK);
		      if (c_ptr->fval <= MAX_OPEN_SPACE) {
			    if (c_ptr->cptr > 1) {
				  c = c_ptr->cptr;
				  vbxe_bank(VBXE_MONBANK);
			      m_ptr = &m_list[c];
			      r_ptr = &c_list[m_ptr->mptr];
			      mon = m_ptr->mptr;

			      /* lite up creature if visible, temp set pl so that update_mon works */
			      vbxe_bank(VBXE_CAVBANK);
			      tmp = c_ptr->flags;					// CHECK: save just PL?
			      c_ptr->flags |= CAVE_PL;
			      bank_update_mon(c, SPELLS2NUM);

				  vbxe_bank(VBXE_MONBANK);
			      ++thit;
			      dam = dam_hp;
			      if (harm_type & r_ptr->cdefense) {
				    dam = dam*2;
				    if (m_ptr->ml) {
					  vbxe_bank(VBXE_GENBANK);
				      c_recall[mon].r_cdefense |= harm_type;
				      vbxe_restore_bank();
					}
				  }
			      else if (weapon_type & r_ptr->spells) {
				    dam = dam / 4;
				    if (m_ptr->ml) {
					  vbxe_bank(VBXE_GENBANK);
				      c_recall[mon].r_spells |= weapon_type;
				      vbxe_restore_bank();
					}
				  }

			      dam = (dam/(spells2_distance(i, j, y, x)+1));
			      h = bank_mon_take_hit(c, dam, SPELLS2NUM);
			      if (h >= 0)
				    ++tkill;
				  vbxe_bank(VBXE_CAVBANK);
			      c_ptr->flags = tmp;					// CHECK: restore just PL?
			    }
			    else if (spells2_panel_contains(i, j) && (py.flags.blind < 1))
			      bank_print('*', i, j, SPELLS2NUM);
			  }
		    }

	    for (i = (y - 2); i <= (y + 2); ++i)
		  for (j = (x - 2); j <= (x + 2); ++j)
		    if (spells2_in_bounds(i, j) && spells2_panel_contains(i, j) && (spells2_distance(y, x, i, j) <= max_dis))
		      spells2_lite_spot(i, j);

	      /* End  explosion. */
	    if (thit == 1) {
		  sprintf(out_val1, "The %s envelops a creature!", descrip);
		  bank_msg_print(out_val1, SPELLS2NUM);
		}
	    else if (thit > 1) {
		  sprintf(out_val1, "The %s envelops several creatures!", descrip);
		  bank_msg_print(out_val1, SPELLS2NUM);
		}

	    if (tkill == 1)
		  bank_msg_print("There is a scream of agony!", SPELLS2NUM);
	    else if (tkill > 1)
		  bank_msg_print("There are several screams of agony!", SPELLS2NUM);
	    //if (tkill >= 0)
		bank_prt_experience(SPELLS2NUM);
	      /* End ball hitting. */
	  }
	  else if (spells2_panel_contains(y, x) && (py.flags.blind < 1)) {
	    bank_print('*', y, x, SPELLS2NUM);
	  }

	  oldy = y;
	  oldx = x;
	}
  } while (!flag);
}


/* Breath weapon works like a fire_ball, but affects the player. */
/* Note the area affect.			      -RAK-   */
void __fastcall__ breath(int8u typ, int8u y, int8u x, int8u dam_hp, char *ddesc, int8u monptr)
{
  register int8u i, j;
  int8u dam, max_dis, harm_type;
  int32u weapon_type;
  int32u treas;
  int32u tmp;
  int8u __fastcall__ (*destroy)(inven_type *item);
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  creature_type *r_ptr;
  int8u c, t;
  int16u mon;


  max_dis = 2;
  get_flags(typ, &weapon_type, &harm_type, &destroy);
  for (i = y-2; i <= y+2; ++i)
    for (j = x-2; j <= x+2; ++j)
      if (spells2_in_bounds(i, j) && (spells2_distance(y, x, i, j) <= max_dis) && spells2_los(y, x, i, j)) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr = CAVE_ADR(i, j);
	    t = c_ptr->tptr;

	    vbxe_bank(VBXE_OBJBANK);
	    if ((t != 0) &&  (*destroy)(&t_list[t]))
	      bank_delete_object(i, j, SPELLS2NUM);

	    vbxe_bank(VBXE_CAVBANK);
	    if (c_ptr->fval <= MAX_OPEN_SPACE) {
	      /* must test status bit, not py.flags.blind here, flag could have been set by a previous
	         monster, but the breath should still be visible until the blindness takes effect */
	      if (spells2_panel_contains(i, j) && !(py.flags.status & PY_BLIND))
		    bank_print('*', i, j, SPELLS2NUM);

	      if (c_ptr->cptr > 1) {
			c = c_ptr->cptr;
		    vbxe_bank(VBXE_MONBANK);
		    m_ptr = &m_list[c];
		    r_ptr = &c_list[m_ptr->mptr];
		    mon = m_ptr->mptr;

		    dam = dam_hp;
		    if (harm_type & r_ptr->cdefense)
		      dam = dam*2;
		    else if (weapon_type & r_ptr->spells)
		      dam = (dam / 4);
		    dam = (dam/(spells2_distance(i, j, y, x)+1));

		  /* can not call mon_take_hit here, since player does not get experience for kill */
		    m_ptr->hp = m_ptr->hp - dam;
		    m_ptr->csleep = 0;
		    if (m_ptr->hp < 0) {
		      treas = bank_monster_death(m_ptr->fy, m_ptr->fx, r_ptr->cmove, SPELLS2NUM);

		      if (m_ptr->ml) {
				vbxe_bank(VBXE_GENBANK);
			    tmp = (c_recall[mon].r_cmove & CM_TREASURE) >> CM_TR_SHIFT;
			    if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT))
			      treas = (treas & ~CM_TREASURE)|(tmp << CM_TR_SHIFT);
			    c_recall[mon].r_cmove = treas | (c_recall[mon].r_cmove & ~CM_TREASURE);
			    vbxe_restore_bank();
			  }

		      /* It ate an already processed monster.Handle normally.*/
		      if (monptr < c)
			    bank_delete_monster(c, SPELLS2NUM);
		        /* If it eats this monster, an already processed monster will take its place,
		           causing all kinds of havoc. Delay the kill a bit. */
		      else
			    bank_fix1_delete_monster(c, SPELLS2NUM);
		    }
		  }
	      else if (c_ptr->cptr == 1) {
		    dam = (dam_hp/(spells2_distance(i, j, y, x)+1));
		    /* let's do at least one point of damage */
		    /* prevents randint(0) problem with poison_gas, also */
		    if (dam == 0)
		      dam = 1;
		    switch(typ) {
		      case GF_LIGHTNING: 	bank_light_dam(dam, ddesc, SPELLS2NUM); break;
		      case GF_POISON_GAS: 	bank_poison_gas(dam, ddesc, SPELLS2NUM); break;
		      case GF_ACID: 		bank_acid_dam(dam, ddesc, SPELLS2NUM); break;
		      case GF_FROST:  	 	bank_cold_dam(dam, ddesc, SPELLS2NUM); break;
		      case GF_FIRE: 		bank_fire_dam(dam, ddesc, SPELLS2NUM); break;
		    }
		  }
	    }
	  }

  for (i = (y - 2); i <= (y + 2); ++i)
    for (j = (x - 2); j <= (x + 2); ++j)
      if (spells2_in_bounds(i, j) && spells2_panel_contains(i, j) && (spells2_distance(y, x, i, j) <= max_dis))
	    spells2_lite_spot(i, j);
}
