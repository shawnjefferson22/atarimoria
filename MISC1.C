#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("MISC1BANK")
#pragma rodata-name("MISC1BANK")


/* prototypes of functions moved into this cartridge bank */
int8 __fastcall__ misc1_todam_adj(void);
int8 __fastcall__ misc1_tohit_adj(void);
int8 __fastcall__ misc1_toac_adj(void);
int8 __fastcall__ misc1_con_adj(void);
int16u __fastcall__ misc1_weight_limit(void);
void __fastcall__ misc1_change_speed(int8 num);
void __fastcall__ misc1_calc_bonuses(void);
int8u __fastcall__ misc1_loc_symbol(int8u y, int8u x);
int8u __fastcall__ misc1_distance(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ misc1_pusht(int8u x);
void __fastcall__ misc1_lite_spot(int8u y, int8u x);
int8u __fastcall__ misc1_test_light(int8u y, int8u x);
int8u __fastcall__ misc1_delete_object(int8u y, int8u x);
void __fastcall__ misc1_invcopy(inven_type *to, int16u from_index);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MISC1NUM

#undef  INBANK_MOD
#define INBANK_MOD			misc1


int8 __fastcall__ misc1_todam_adj(void)
#include "inbank\todam_adj.c"

int8 __fastcall__ misc1_tohit_adj(void)
#include "inbank\tohit_adj.c"

int8 __fastcall__ misc1_toac_adj(void)
#include "inbank\toac_adj.c"

int8u __fastcall__ modify_stat (int8u stat, int8 amount)
#include "inbank\modify_stat.c"

void __fastcall__ misc1_pusht(int8u x)
#include "inbank\pusht.c"

void __fastcall__ misc1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ misc1_test_light(int8u y, int8u x)
#include "inbank\test_light.c"


/* Computes current weight limit			-RAK-	*/
int16u __fastcall__ misc1_weight_limit(void)
{
  register int16u weight_cap;


  weight_cap = py.stats.use_stat[A_STR] * PLAYER_WEIGHT_CAP + py.misc.wt;
  if (weight_cap > 3000)  weight_cap = 3000;
  return(weight_cap);
}


void __fastcall__ misc1_change_speed(int8 num)
#include "inbank\change_speed.c"

int8 __fastcall__ misc1_con_adj(void)
#include "inbank\con_adj.c"

void __fastcall__ misc1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ misc1_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"


/* Recalculate the effect of all the stuff we use.                -CJS- */
void __fastcall__ misc1_calc_bonuses(void)
{
  register int32u item_flags;
  int8u old_dis_ac;
  register struct flags *p_ptr;
  register struct misc *m_ptr;
  register inven_type *i_ptr;
  register int i;


  p_ptr = &py.flags;
  m_ptr = &py.misc;
  if (p_ptr->slow_digest)
    p_ptr->food_digested++;
  if (p_ptr->regenerate)
    p_ptr->food_digested -= 3;
  p_ptr->see_inv     = FALSE;
  p_ptr->teleport    = FALSE;
  p_ptr->free_act    = FALSE;
  p_ptr->slow_digest = FALSE;
  p_ptr->aggravate   = FALSE;
  p_ptr->sustain_str = FALSE;
  p_ptr->sustain_int = FALSE;
  p_ptr->sustain_wis = FALSE;
  p_ptr->sustain_con = FALSE;
  p_ptr->sustain_dex = FALSE;
  p_ptr->sustain_chr = FALSE;
  p_ptr->fire_resist = FALSE;
  p_ptr->acid_resist = FALSE;
  p_ptr->cold_resist = FALSE;
  p_ptr->regenerate  = FALSE;
  p_ptr->lght_resist = FALSE;
  p_ptr->ffall       = FALSE;

  old_dis_ac = m_ptr->dis_ac;
  m_ptr->ptohit  = misc1_tohit_adj();       /* Real To Hit   		*/
  m_ptr->ptodam  = misc1_todam_adj();       /* Real To Dam   		*/
  m_ptr->ptoac   = misc1_toac_adj();        /* Real To AC    		*/
  m_ptr->pac     = 0;               		/* Real AC       		*/
  m_ptr->dis_th  = m_ptr->ptohit;  			/* Display To Hit       */
  m_ptr->dis_td  = m_ptr->ptodam;  			/* Display To Dam       */
  m_ptr->dis_ac  = 0;           			/* Display AC           */
  m_ptr->dis_tac = m_ptr->ptoac;   			/* Display To AC        */


  vbxe_bank(VBXE_OBJBANK);                              // bank in inventory/object list

  for (i = INVEN_WIELD; i < INVEN_LIGHT; ++i)
    {
      i_ptr = &inventory[i];
      if (i_ptr->tval != TV_NOTHING)
	{
	  m_ptr->ptohit += i_ptr->tohit;
	  if (i_ptr->tval != TV_BOW)            /* Bows can't damage. -CJS- */
	    m_ptr->ptodam += i_ptr->todam;
	  m_ptr->ptoac  += i_ptr->toac;
	  m_ptr->pac += i_ptr->ac;
	  //if (known2_p(i_ptr))
	  if (i_ptr->ident & ID_KNOWN2)
	    {
	      m_ptr->dis_th  += i_ptr->tohit;
	      if (i_ptr->tval != TV_BOW)
		m_ptr->dis_td  += i_ptr->todam; /* Bows can't damage. -CJS- */
	      m_ptr->dis_tac += i_ptr->toac;
	      m_ptr->dis_ac += i_ptr->ac;
	    }
	  else if (! (TR_CURSED & i_ptr->flags))
	    /* Base AC values should always be visible, as long as the item
	       is not cursed.  */
	    m_ptr->dis_ac += i_ptr->ac;
	}
    }
  m_ptr->dis_ac += m_ptr->dis_tac;

  if (weapon_heavy)
    m_ptr->dis_th += (py.stats.use_stat[A_STR] * 15 -
		      inventory[INVEN_WIELD].weight);

  /* Add in temporary spell increases   */
  if (p_ptr->invuln > 0)
    {
      m_ptr->pac += 100;
      m_ptr->dis_ac += 100;
    }
  if (p_ptr->blessed > 0)
    {
      m_ptr->pac    += 2;
      m_ptr->dis_ac += 2;
    }
  if (p_ptr->detect_inv > 0)
    p_ptr->see_inv = TRUE;

  /* can't print AC here because might be in a store */
  if (old_dis_ac != m_ptr->dis_ac)
    p_ptr->status |= PY_ARMOR;

  item_flags = 0;
  i_ptr = &inventory[INVEN_WIELD];
  for (i = INVEN_WIELD; i < INVEN_LIGHT; ++i)
    {
      item_flags |= i_ptr->flags;
      ++i_ptr;
    }

  if (TR_SLOW_DIGEST & item_flags)
    p_ptr->slow_digest = TRUE;
  if (TR_AGGRAVATE & item_flags)
    p_ptr->aggravate = TRUE;
  if (TR_TELEPORT & item_flags)
    p_ptr->teleport = TRUE;
  if (TR_REGEN & item_flags)
    p_ptr->regenerate = TRUE;
  if (TR_RES_FIRE & item_flags)
    p_ptr->fire_resist = TRUE;
  if (TR_RES_ACID & item_flags)
    p_ptr->acid_resist = TRUE;
  if (TR_RES_COLD & item_flags)
    p_ptr->cold_resist = TRUE;
  if (TR_FREE_ACT & item_flags)
    p_ptr->free_act = TRUE;
  if (TR_SEE_INVIS & item_flags)
    p_ptr->see_inv = TRUE;
  if (TR_RES_LIGHT & item_flags)
    p_ptr->lght_resist = TRUE;
  if (TR_FFALL & item_flags)
    p_ptr->ffall = TRUE;

  i_ptr = &inventory[INVEN_WIELD];
  for (i = INVEN_WIELD; i < INVEN_LIGHT; ++i)
    {
      if (TR_SUST_STAT & i_ptr->flags)

	switch(i_ptr->p1)
	  {
	  case 1: p_ptr->sustain_str = TRUE; break;
	  case 2: p_ptr->sustain_int = TRUE; break;
	  case 3: p_ptr->sustain_wis = TRUE; break;
	  case 4: p_ptr->sustain_con = TRUE; break;
	  case 5: p_ptr->sustain_dex = TRUE; break;
	  case 6: p_ptr->sustain_chr = TRUE; break;
	  default: break;
	  }
      ++i_ptr;
    }

  if (p_ptr->slow_digest)
    p_ptr->food_digested--;
  if (p_ptr->regenerate)
    p_ptr->food_digested += 3;

  vbxe_restore_bank();                                  // restore the previous VBXE bank
}


/* Are we strong enough for the current pack and weapon?  -CJS-	 */
void __fastcall__ check_strength(void)
{
  register int i;
  register inven_type *i_ptr;


  vbxe_bank(VBXE_OBJBANK);

  i_ptr = &inventory[INVEN_WIELD];
  if (i_ptr->tval != TV_NOTHING
      && (py.stats.use_stat[A_STR]*15 < i_ptr->weight))
    {
      if (weapon_heavy == FALSE)
	{
	  bank_msg_print("You have trouble wielding such a heavy weapon.", MISC1NUM);
	  weapon_heavy = TRUE;
	  misc1_calc_bonuses();
	}
  }
  else if (weapon_heavy == TRUE)
  {
      weapon_heavy = FALSE;
      if (i_ptr->tval != TV_NOTHING)
	    bank_msg_print("You are strong enough to wield your weapon.", MISC1NUM);
      misc1_calc_bonuses();
  }

  i = misc1_weight_limit();
  if (i < inven_weight)
    i = inven_weight / (i+1);
  else
    i = 0;
  if (pack_heavy != i)
  {
      if (pack_heavy < i)
	    bank_msg_print("Your pack is so heavy that it slows you down.", MISC1NUM);
      else
	    bank_msg_print("You move more easily under the weight of your pack.", MISC1NUM);
        misc1_change_speed(i - pack_heavy);
        pack_heavy = i;
  }
  py.flags.status &= ~PY_STR_WGT;
  vbxe_restore_bank();

}


/* Calculate the players hit points */
void __fastcall__ calc_hitpoints(void)
{
  register int hitpoints;
  register struct misc *p_ptr;
  int32 value;


  p_ptr = &py.misc;

  vbxe_bank(VBXE_SPLBANK);					// player_hp in splbank
  hitpoints = player_hp[p_ptr->lev-1] + (misc1_con_adj() * p_ptr->lev);
  vbxe_restore_bank();

  /* always give at least one point per level + 1 */
  if (hitpoints < (p_ptr->lev + 1))
    hitpoints = p_ptr->lev + 1;

  if (py.flags.status & PY_HERO)
    hitpoints += 10;
  if (py.flags.status & PY_SHERO)
    hitpoints += 20;

  /* mhp can equal zero while character is being created */
  if ((hitpoints != p_ptr->mhp) && (p_ptr->mhp != 0)) {
    /* change current hit points proportionately to change of mhp, divide first to avoid overflow, little loss of accuracy */
    value = (((long)p_ptr->chp << 16) + p_ptr->chp_frac) / p_ptr->mhp * hitpoints;
    p_ptr->chp = value >> 16;
    p_ptr->chp_frac = value & 0xFFFF;
    p_ptr->mhp = hitpoints;

    /* can't print hit points here, may be in store or inventory mode */
    py.flags.status |= PY_HP;
  }
}


/* Set the value of the stat which is actually used.	 -CJS- */
void __fastcall__ set_use_stat(int8u stat)
{
  py.stats.use_stat[stat] = modify_stat(stat, py.stats.mod_stat[stat]);

  if (stat == A_STR) {
      py.flags.status |= PY_STR_WGT;
      misc1_calc_bonuses();
  }
  else if (stat == A_DEX)
    misc1_calc_bonuses();
  else if (stat == A_INT && class[py.misc.pclass].spell == MAGE) {
    bank_calc_spells(A_INT, MISC1NUM);
    bank_calc_mana(A_INT, MISC1NUM);
  }
  else if (stat == A_WIS && class[py.misc.pclass].spell == PRIEST) {
    bank_calc_spells(A_WIS, MISC1NUM);
    bank_calc_mana(A_WIS, MISC1NUM);
  }
  else if (stat == A_CON)
    calc_hitpoints();
}


/* Decreases a stat by one randomized level		-RAK-	*/
int8u __fastcall__ dec_stat(int8u stat)
{
  register int8u tmp_stat, loss;


  tmp_stat = py.stats.cur_stat[stat];
  if (tmp_stat > 3) {
    if (tmp_stat < 19)
	  --tmp_stat;
    else if (tmp_stat < 117) {
	  loss = (((118 - tmp_stat) >> 1) + 1) >> 1;
	  tmp_stat += -randint(loss) - loss;
	  if (tmp_stat < 18)
	    tmp_stat = 18;
    }
    else
	  --tmp_stat;

     py.stats.cur_stat[stat] = tmp_stat;
     set_use_stat(stat);
     bank_prt_stat(stat, MISC1NUM);
     return TRUE;
  }
  else
    return FALSE;
}


/* Boost a stat artificially (by wearing something). If the display argument
   is TRUE, then increase is shown on the screen. */
void __fastcall__ bst_stat(int8u stat, int8 amount)
{
  py.stats.mod_stat[stat] += amount;

  set_use_stat(stat);
  /* can not call prt_stat() here, may be in store, may be in inven_command */
  py.flags.status |= (PY_STR << stat);
}


/* Player bonuses					-RAK-	*/
/* When an item is worn or taken off, this re-adjusts the player */
/* bonuses.  Factor=1 : wear; Factor=-1 : removed		 */
/* Only calculates properties with cumulative effect.  Properties that
   depend on everything being worn are recalculated by calc_bonuses() -CJS- */
void __fastcall__ py_bonuses(inven_type *t_ptr, int8 factor)
{
  register int8 amount;
  register int8u i;


  amount = t_ptr->p1 * factor;
  if (t_ptr->flags & TR_STATS) {
    for(i = 0; i < 6; ++i)
	  if ((1 << i) & t_ptr->flags)
	    bst_stat(i, amount);
  }
  if (TR_SEARCH & t_ptr->flags) {
      py.misc.srh += amount;
      py.misc.fos -= amount;
  }
  if (TR_STEALTH & t_ptr->flags)
    py.misc.stl += amount;
  if (TR_SPEED & t_ptr->flags)
    misc1_change_speed(-amount);
  if ((TR_BLIND & t_ptr->flags) && (factor > 0))
    py.flags.blind += 1000;
  if ((TR_TIMID & t_ptr->flags) && (factor > 0))
    py.flags.afraid += 50;
  if (TR_INFRA & t_ptr->flags)
    py.flags.see_infra += amount;
}


/* Returns symbol for given row, column                 -RAK-   */
/*int8u __fastcall__ misc1_loc_symbol(int8u y, int8u x)
{
  register cave_type *cave_ptr;         // in VBXE CAVBANK
  cave_type c;                                          // temporary store cave data in stack


  vbxe_bank(VBXE_CAVBANK);
  cave_ptr = &cave[y][x];

  c.cptr = cave_ptr->cptr;              // copy the cave data locally
  c.tptr = cave_ptr->tptr;
  c.fval = cave_ptr->fval;
  c.flags = cave_ptr->flags;

  vbxe_bank(VBXE_MONBANK);                      // bank in the monster list
  if ((c.cptr == 1) && (!find_flag || find_prself))
    return SYM_PLAYER;
  else if (py.flags.status & PY_BLIND)
    return ' ';
  else if ((py.flags.image > 0) && (randint (12) == 1))
    return randint (95) + 31;
  else if ((c.cptr > 1) && (m_list[c.cptr].ml))
    return c_list[m_list[c.cptr].mptr].cchar;
  else if (!(c.flags & CAVE_PL) && !(c.flags & CAVE_TL) && !(c.flags & CAVE_FM))
    return ' ';

  vbxe_bank(VBXE_OBJBANK);              // bank in the object list
  if ((c.tptr != 0) && (t_list[c.tptr].tval != TV_INVIS_TRAP))
    return t_list[c.tptr].tchar;
  else if (c.fval <= MAX_CAVE_FLOOR)
    return SYM_FLOOR;
  else if (c.fval == GRANITE_WALL || c.fval == BOUNDARY_WALL || highlight_seams == FALSE)
    return SYM_WALL;
  else
    return SYM_SEAM;
}*/


/* Deletes object from given location                   -RAK-   */
int8u __fastcall__ misc1_delete_object(int8u y, int8u x)
{
  //register int8u delete;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);
  if (c_ptr->fval == BLOCKED_FLOOR)
    c_ptr->fval = CORR_FLOOR;
  misc1_pusht(c_ptr->tptr);

  vbxe_bank(VBXE_CAVBANK);
  c_ptr->tptr = 0;
  c_ptr->flags &= ~CAVE_FM;

  misc1_lite_spot(y, x);
  if (misc1_test_light(y, x))
    return(TRUE);
  else
    return(FALSE);
}


/* If too many objects on floor level, delete some of them-RAK- */
void __fastcall__ compact_objects(void)
{
  register int8u i, j;
  register int8u ctr;
  int8u chance;
  int8 cur_dis;
  //register cave_type *cave_ptr;
  int8u tptr;


  bank_msg_print("Compacting objects...", MISC1NUM);

  ctr = 0;
  cur_dis = 66;
  do {
    for (i = 0; i < cur_height; ++i)
	  for (j = 0; j < cur_width; ++j) {
        vbxe_bank(VBXE_CAVBANK);
	    //cave_ptr = &cave[i][j];
		tptr = (CAVE_ADR(i, j))->tptr;

	    if ((tptr != 0) && (misc1_distance(i, j, char_row, char_col) > cur_dis)) {
		  vbxe_bank(VBXE_OBJBANK);
		  switch(t_list[tptr].tval) {
		    case TV_VIS_TRAP:
		      chance = 15;
		      break;
		    case TV_INVIS_TRAP:
		    case TV_RUBBLE:
		    case TV_OPEN_DOOR: case TV_CLOSED_DOOR:
		      chance = 5;
		      break;
		    case TV_UP_STAIR: case TV_DOWN_STAIR:
		    case TV_STORE_DOOR:
		      /* stairs, don't delete them */
		      /* shop doors, don't delete them */
		      chance = 0;
		      break;
		    case TV_SECRET_DOOR: /* secret doors */
		      chance = 3;
		      break;
		    default:
		      chance = 10;
		  }
		  if (randint (100) <= chance)
		  {
		    misc1_delete_object(i, j);
		    ++ctr;
		  }
	    }
	  }
      if (ctr == 0) cur_dis -= 6;
  } while (ctr <= 0);

  if (cur_dis < 66) bank_prt_map(MISC1NUM);
}


/* Describe amount of item remaining.			-RAK-	*/
void __fastcall__ desc_remain(int8u item_val)
{
  //bigvtype out_val, tmp_str;				// use out_val1 -SJ
  register inven_type *i_ptr;


  vbxe_bank(VBXE_OBJBANK);

  i_ptr = &inventory[item_val];
  i_ptr->number--;
  bank_objdes(out_val1, i_ptr, TRUE, MISC1NUM);
  i_ptr->number++;
  /* the string already has a dot at the end. */
  sprintf(out_val2, "You have %s", out_val1);
  bank_msg_print(out_val2, MISC1NUM);

  vbxe_restore_bank();
}


/* Lose experience -RAK-	*/
void __fastcall__ lose_exp(int32 amount)
{
  register int8u i;
  register class_type *c_ptr;


  if (amount > py.misc.exp)
    py.misc.exp = 0;
  else
    py.misc.exp -= amount;
  bank_prt_experience(MISC1NUM);

  vbxe_bank(VBXE_SPLBANK);			// player_exp and class are in splbank

  i = 0;
  while ((player_exp[i] * py.misc.expfact / 100) <= py.misc.exp)
    ++i;
  /* increment i once more, because level 1 exp is stored in player_exp[0] */
  ++i;

  if (py.misc.lev != i) {
    py.misc.lev = i;

    calc_hitpoints();
    c_ptr = &class[py.misc.pclass];
    if (c_ptr->spell == MAGE) {
	  bank_calc_spells(A_INT, MISC1NUM);
	  bank_calc_mana(A_INT, MISC1NUM);
	}
    else if (c_ptr->spell == PRIEST) {
	  bank_calc_spells(A_WIS, MISC1NUM);
	  bank_calc_mana(A_WIS, MISC1NUM);
	}
    bank_prt_level(MISC1NUM);
    bank_prt_title(MISC1NUM);
  }

  vbxe_restore_bank();
}
