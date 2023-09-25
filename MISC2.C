#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("MISC2BANK")
#pragma rodata-name("MISC2BANK")


/* prototypes of functions moved into this cartridge bank */
void __fastcall__ misc2_take_hit(int8u damage, char *hit_from);
int8u __fastcall__ misc2_minus_ac(int32u typ_dam);
int16u __fastcall__ misc2_inven_damage(int8u __fastcall__ (*typ)(inven_type *item), int8u perc);
void __fastcall__ misc2_lite_spot(int8u y, int8u x);
void __fastcall__ misc2_move_rec(int8u y1, int8u x1, int8u y2, int8u x2);
int8u __fastcall__ misc2_distance(int8u y1, int8u x1, int8u y2, int8u x2);
int8u __fastcall__ misc2_stat_adj(int8u stat);
int8u __fastcall__ set_corrodes(inven_type *item);
int8u __fastcall__ set_flammable(inven_type *item);
int8u __fastcall__ set_frost_destroy(inven_type *item);
int8u __fastcall__ set_lightning_destroy(inven_type *item);
int8u __fastcall__ set_acid_affect(inven_type *item);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MISC2NUM


void __fastcall__ misc2_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"

void __fastcall__ misc2_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

void __fastcall__ misc2_move_rec(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\move_rec.c"

int8u __fastcall__ misc2_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

int8u __fastcall__ misc2_stat_adj(int8u stat)
#include "inbank\stat_adj.c"



/* AC gets worse					-RAK-	*/
/* Note: This routine affects magical AC bonuses so that stores	  */
/*	 can detect the damage.					 */
int8u __fastcall__ misc2_minus_ac(int32u typ_dam)
{
  register int8u i, j;
  int8u tmp[6], minus;
  register inven_type *i_ptr;
  //bigvtype out_val, tmp_str;						// use out_val1 and out_val2



  vbxe_bank(VBXE_OBJBANK);
  i = 0;
  if (inventory[INVEN_BODY].tval != TV_NOTHING) {
    tmp[i] = INVEN_BODY;
    ++i;
  }
  if (inventory[INVEN_ARM].tval != TV_NOTHING) {
    tmp[i] = INVEN_ARM;
    ++i;
  }
  if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
    tmp[i] = INVEN_OUTER;
    ++i;
  }
  if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
    tmp[i] = INVEN_HANDS;
    ++i;
  }
  if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
    tmp[i] = INVEN_HEAD;
    ++i;
  }
  /* also affect boots */
  if (inventory[INVEN_FEET].tval != TV_NOTHING) {
    tmp[i] = INVEN_FEET;
    ++i;
  }

  minus = FALSE;
  if (i > 0) {
    j = tmp[randint(i) - 1];
    i_ptr = &inventory[j];
    if (i_ptr->flags & typ_dam) {
	  bank_objdes(out_val1, &inventory[j], FALSE, MISC2NUM);
	  sprintf(out_val2, "Your %s resists damage!", out_val1);
	  bank_msg_print(out_val2, MISC2NUM);
	  minus = TRUE;
	}
    else if ((i_ptr->ac+i_ptr->toac) > 0) {
	  bank_objdes(out_val1, &inventory[j], FALSE, MISC2NUM);
	  sprintf(out_val2, "Your %s is damaged!", out_val1);
	  bank_msg_print(out_val2, MISC2NUM);

	  vbxe_bank(VBXE_OBJBANK);
	  i_ptr->toac--;
	  bank_calc_bonuses(MISC2NUM);
	  minus = TRUE;
	}
  }

  return(minus);
}


/* Destroys a type of item on a given percent chance	-RAK-	*/
int16u __fastcall__ misc2_inven_damage(int8u __fastcall__ (*typ)(inven_type *item), int8u perc)
{
  register int16u i, j;


  j = 0;
  vbxe_bank(VBXE_OBJBANK);
  for (i = 0; i < inven_ctr; ++i) {
    vbxe_bank(VBXE_OBJBANK);									// make sure object bank is in -SJ

    if ((*typ)(&inventory[i]) && (randint(100) < perc)) {
	  bank_inven_destroy(i, MISC2NUM);
	  ++j;
    }
  }

  return(j);
}


/* Corrode the unsuspecting person's armor		 -RAK-	 */
void __fastcall__ corrode_gas(char *kb_str)
{
  if (!misc2_minus_ac((int32u) TR_RES_ACID))
    misc2_take_hit(randint(8), kb_str);
  if (misc2_inven_damage(set_corrodes, 5) > 0)
    bank_msg_print("There is an acrid smell coming from your pack.", MISC2NUM);
}


/* Poison gas the idiot.				-RAK-	*/
void __fastcall__ poison_gas(int8u dam, char *kb_str)
{
  misc2_take_hit(dam, kb_str);
  py.flags.poisoned += 12 + randint(dam);
}


/* Burn the fool up.					-RAK-	*/
void __fastcall__ fire_dam(int8u dam, char *kb_str)
{
  if (py.flags.fire_resist)
    dam = dam / 3;
  if (py.flags.resist_heat > 0)
    dam = dam / 3;
  misc2_take_hit(dam, kb_str);
  if (misc2_inven_damage(set_flammable, 3) > 0)
    bank_msg_print("There is smoke coming from your pack!", MISC2NUM);
}


/* Freeze him to death.				-RAK-	*/
void __fastcall__ cold_dam(int8u dam, char *kb_str)
{
  if (py.flags.cold_resist)
    dam = dam / 3;
  if (py.flags.resist_cold > 0)
    dam = dam / 3;
  misc2_take_hit(dam, kb_str);
  if (misc2_inven_damage(set_frost_destroy, 5) > 0)
    bank_msg_print("Something shatters inside your pack!", MISC2NUM);
}


/* Lightning bolt the sucker away.			-RAK-	*/
void __fastcall__ light_dam(int8u dam, char *kb_str)
{
  if (py.flags.lght_resist)
    misc2_take_hit((dam / 3), kb_str);
  else
    misc2_take_hit(dam, kb_str);
  if (misc2_inven_damage(set_lightning_destroy, 3) > 0)
    bank_msg_print("There are sparks coming from your pack!", MISC2NUM);
}


/* Throw acid on the hapless victim			-RAK-	*/
void __fastcall__ acid_dam(int8u dam, char *kb_str)
{
  register int8u flag;


  flag = 0;
  if (misc2_minus_ac((int32u) TR_RES_ACID))
    flag = 1;
  if (py.flags.acid_resist)
    flag += 2;
  misc2_take_hit(dam / (flag + 1), kb_str);
  if (misc2_inven_damage(set_acid_affect, 3) > 0)
    bank_msg_print("There is an acrid smell coming from your pack!", MISC2NUM);
}


int8u __fastcall__ set_corrodes(inven_type *item)
{
  switch(item->tval) {
    case TV_SWORD: case TV_HELM: case TV_SHIELD: case TV_HARD_ARMOR:
    case TV_WAND:
      return (TRUE);
  }
  return(FALSE);
}


int8u __fastcall__ set_flammable(inven_type *item)
{
  switch(item->tval) {
    case TV_ARROW: case TV_BOW: case TV_HAFTED: case TV_POLEARM:
    case TV_BOOTS: case TV_GLOVES: case TV_CLOAK: case TV_SOFT_ARMOR:
      /* Items of (RF) should not be destroyed.  */
      if (item->flags & TR_RES_FIRE)
	    return FALSE;
      else
	   return TRUE;

    case TV_STAFF: case TV_SCROLL1: case TV_SCROLL2:
      return TRUE;
  }
  return(FALSE);
}


int8u __fastcall__ set_frost_destroy(inven_type *item)
{
  if ((item->tval == TV_POTION1) || (item->tval == TV_POTION2) || (item->tval == TV_FLASK))
    return(TRUE);
  return(FALSE);
}


int8u __fastcall__ set_lightning_destroy(inven_type *item)
{
  if ((item->tval == TV_RING) || (item->tval == TV_WAND) || (item->tval == TV_SPIKE))
    return(TRUE);
  else
    return(FALSE);
}


int8u __fastcall__ set_acid_affect(inven_type *item)
{
  switch(item->tval) {
    case TV_MISC: case TV_CHEST:
      return TRUE;
    case TV_BOLT: case TV_ARROW:
    case TV_BOW: case TV_HAFTED: case TV_POLEARM: case TV_BOOTS:
    case TV_GLOVES: case TV_CLOAK: case TV_SOFT_ARMOR:
      if (item->flags & TR_RES_ACID)
	    return (FALSE);
      else
	    return (TRUE);
  }
  return(FALSE);
}


/* Critical hits, Nasty way to die.			-RAK-	*/
int8u __fastcall__ critical_blow(int16u weight, int8u plus, int8u dam, int8u attack_type)
{
  register int8u critical;


  critical = dam;
  /* Weight of weapon, plusses to hit, and character level all	    */
  /* contribute to the chance of a critical			                */

  vbxe_bank(VBXE_SPLBANK);

  if (randint(5000) <= (int)(weight + 5 * plus + (class_level_adj[py.misc.pclass][attack_type] * py.misc.lev))) {
    weight += randint(650);
    if (weight < 400) {
	  critical = 2*dam + 5;
	  bank_msg_print("It was a good hit! (x2 damage)", MISC2NUM);
	}
    else if (weight < 700) {
	  critical = 3*dam + 10;
	  bank_msg_print("It was an excellent hit! (x3 damage)", MISC2NUM);
	}
    else if (weight < 900) {
	  critical = 4*dam + 15;
	  bank_msg_print("It was a superb hit! (x4 damage)", MISC2NUM);
	}
    else {
	  critical = 5*dam + 20;
	  bank_msg_print("It was a *GREAT* hit! (x5 damage)", MISC2NUM);
	}
  }

  vbxe_restore_bank();
  return(critical);
}


/* Special damage due to magical abilities of object	-RAK-	*/
int8u __fastcall__ tot_dam(inven_type *i_ptr, int8u tdam, int16u monster)
{
  int16u cdef;									// holder for monster cdefense


  vbxe_bank(VBXE_OBJBANK);
  if ((i_ptr->flags & TR_EGO_WEAPON) && (((i_ptr->tval >= TV_SLING_AMMO) && (i_ptr->tval <= TV_ARROW)) ||
       ((i_ptr->tval >= TV_HAFTED) && (i_ptr->tval <= TV_SWORD)) || (i_ptr->tval == TV_FLASK))) {
	vbxe_bank(VBXE_MONBANK);
    cdef = c_list[monster].cdefense;			// get monster cdefense

    vbxe_bank(VBXE_OBJBANK);   				    // bring bank OBJBANK

    /* Slay Dragon  */
    if ((cdef & CD_DRAGON) && (i_ptr->flags & TR_SLAY_DRAGON)) {
	  tdam = tdam * 4;

	  vbxe_bank(VBXE_GENBANK);
	  c_recall[monster].r_cdefense |= CD_DRAGON;
	  vbxe_restore_bank();
	}
    /* Slay Undead  */
    else if ((cdef & CD_UNDEAD) && (i_ptr->flags & TR_SLAY_UNDEAD)) {
	  tdam = tdam * 3;

	  vbxe_bank(VBXE_GENBANK);
	  c_recall[monster].r_cdefense |= CD_UNDEAD;
	  vbxe_restore_bank();
	}
    /* Slay Animal  */
    else if ((cdef & CD_ANIMAL) && (i_ptr->flags & TR_SLAY_ANIMAL)) {
	  tdam = tdam * 2;

	  vbxe_bank(VBXE_GENBANK);
	  c_recall[monster].r_cdefense |= CD_ANIMAL;
	  vbxe_restore_bank();
	}
    /* Slay Evil     */
    else if ((cdef & CD_EVIL) && (i_ptr->flags & TR_SLAY_EVIL)) {
	  tdam = tdam * 2;

	  vbxe_bank(VBXE_GENBANK);
	  c_recall[monster].r_cdefense |= CD_EVIL;
	  vbxe_restore_bank();
	}
    /* Frost	       */
    else if ((cdef & CD_FROST) && (i_ptr->flags & TR_FROST_BRAND)) {
	  tdam = tdam * 3 / 2;

	  vbxe_bank(VBXE_GENBANK);
	  c_recall[monster].r_cdefense |= CD_FROST;
	  vbxe_restore_bank();
	}
    /* Fire	      */
    else if ((cdef & CD_FIRE) && (i_ptr->flags & TR_FLAME_TONGUE)) {
	  tdam = tdam * 3 / 2;

	  vbxe_bank(VBXE_GENBANK);
	  c_recall[monster].r_cdefense |= CD_FIRE;
	  vbxe_restore_bank();
	}
  }

  return(tdam);
}


/* Get all the monsters on the level pissed off.	-RAK-	*/
int8u __fastcall__ aggravate_monster(int8u dis_affect)
{
  register int8u i, aggravate;
  register monster_type *m_ptr;


  vbxe_bank(VBXE_MONBANK);

  aggravate = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    m_ptr->csleep = 0;
    if ((m_ptr->cdis <= dis_affect) && (m_ptr->cspeed < 2)) {
	  m_ptr->cspeed++;
	  aggravate = TRUE;
	}
  }

  vbxe_restore_bank();

  if (aggravate)
    bank_msg_print("You hear a sudden stirring in the distance!", MISC2NUM);
  return(aggravate);
}


/* Move the creature record to a new location		-RAK-	*/
void __fastcall__ teleport_away(int8u monptr, int8u dis)
{
  register int8u yn, xn, ctr;
  register monster_type *m_ptr;
  int8u fy, fx;


  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  fy = m_ptr->fy;
  fx = m_ptr->fx;

  vbxe_bank(VBXE_CAVBANK);
  ctr = 0;
  do {
    do {
	  yn = fy + (randint(2*dis+1) - (dis + 1));
	  xn = fx + (randint(2*dis+1) - (dis + 1));
	//} while (!misc2_in_bounds(yn, xn));
	} while (!((yn > 0) && (yn < cur_height-1) && (xn > 0) && (xn < cur_width-1)));		// in_bounds code in-line -SJ

    ++ctr;
    if (ctr > 9) {
	  ctr = 0;
	  dis += 5;
	}
  } while ((cave[yn][xn].fval >= MIN_CLOSED_SPACE) || (cave[yn][xn].cptr != 0));

  misc2_move_rec(fy, fx, yn, xn);
  misc2_lite_spot(fy, fx);

  vbxe_bank(VBXE_MONBANK);
  m_ptr->fy = yn;
  m_ptr->fx = xn;
  /* this is necessary, because the creature is not currently visible in its new position */
  m_ptr->ml = FALSE;
  m_ptr->cdis = misc2_distance(char_row, char_col, yn, xn);
  bank_update_mon(monptr, MISC2NUM);
}


/* Saving throws for player character.		-RAK-	*/
int8u __fastcall__ player_saves(void)
{
  int8u temp;


  vbxe_bank(VBXE_SPLBANK);								// class_level_adj is in SPLBANK
  temp = class_level_adj[py.misc.pclass][CLA_SAVE];
  vbxe_restore_bank();

  if (randint(100) <= (py.misc.save + misc2_stat_adj(A_WIS) + (temp * py.misc.lev / 3)))
    return(TRUE);
  else
    return(FALSE);
}


