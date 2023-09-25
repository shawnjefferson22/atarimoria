/* source/create.c: create a player character

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("CREATE2BANK")
#pragma rodata-name("CREATE2BANK")



/* Local prototypes, duplicated in CREATEBANK */
void __fastcall__ create2_set_use_stat(int8u stat);
int8 __fastcall__ create2_todam_adj(void);
int8 __fastcall__ create2_tohit_adj(void);
int8 __fastcall__ create2_toac_adj(void);
int8 __fastcall__ create2_con_adj(void);
int8u __fastcall__ create2_modify_stat (int8u stat, int8 amount);
void __fastcall__ create2_calc_bonuses(void);
void __fastcall__ create2_calc_spells(int8u stat);
void __fastcall__ create2_calc_mana(int8u stat);
void __fastcall__ create2_calc_hitpoints(void);
int8u __fastcall__ create2_stat_adj(int8u stat);
void __fastcall__ create2_change_stat(int8u stat, int8 amount);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		CREATE2NUM

#undef  INBANK_MOD
#define INBANK_MOD			create2



int8u __fastcall__ create2_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

int8 __fastcall__ create2_con_adj(void)
#include "inbank\con_adj.c"

void __fastcall__ create2_change_stat(int8u stat, int8 amount)
#include "inbank\change_stat.c"

int8u __fastcall__ create2_modify_stat (int8u stat, int8 amount)                                // amount was int
#include "inbank\modify_stat.c"

int8 __fastcall__ create2_todam_adj(void)
#include "inbank\todam_adj.c"

int8 __fastcall__ create2_tohit_adj(void)
#include "inbank\tohit_adj.c"

int8 __fastcall__ create2_toac_adj(void)
#include "inbank\toac_adj.c"



/* Generates character's stats                          -JWT-   */
void get_stats(void)
{
  register int8u i, tot;
  int8u dice[18];


  do {
    tot = 0;
    for (i = 0; i < 18; ++i) {
	  dice[i] = randint (3 + i % 3);  /* Roll 3,4,5 sided dice once each */
	  tot += dice[i];
	}
  } while (tot <= 42 || tot >= 54);

  for (i = 0; i < 6; ++i)
    py.stats.max_stat[i] = 5 + dice[3*i] + dice[3*i+1] + dice[3*i+2];
}


/* generate all stats and modify for race. needed in a separate module so
   looping of character selection would be allowed     -RGM- */
void get_all_stats(void)
{
  register int8u j;


  vbxe_bank(VBXE_SPLBANK);

  get_stats();
  create2_change_stat(A_STR, race[py.misc.prace].str_adj);
  create2_change_stat(A_INT, race[py.misc.prace].int_adj);
  create2_change_stat(A_WIS, race[py.misc.prace].wis_adj);
  create2_change_stat(A_DEX, race[py.misc.prace].dex_adj);
  create2_change_stat(A_CON, race[py.misc.prace].con_adj);
  create2_change_stat(A_CHR, race[py.misc.prace].chr_adj);
  for (j = 0; j < 6; ++j) {
    py.stats.cur_stat[j] = py.stats.max_stat[j];
    create2_set_use_stat(j);
  }

  py.misc.srh    = race[py.misc.prace].srh;
  py.misc.bth    = race[py.misc.prace].bth;
  py.misc.bthb   = race[py.misc.prace].bthb;
  py.misc.fos    = race[py.misc.prace].fos;
  py.misc.stl    = race[py.misc.prace].stl;
  py.misc.save   = race[py.misc.prace].bsav;
  py.misc.hitdie = race[py.misc.prace].bhitdie;
  py.misc.lev    = 1;
  py.misc.ptodam = create2_todam_adj();
  py.misc.ptohit = create2_tohit_adj();
  py.misc.ptoac  = 0;
  py.misc.pac    = create2_toac_adj();
  py.misc.expfact = race[py.misc.prace].b_exp;
  py.flags.see_infra = race[py.misc.prace].infra;
}


/* Set the value of the stat which is actually used.     -CJS- */
void __fastcall__ create2_set_use_stat(int8u stat)
{
  py.stats.use_stat[stat] = create2_modify_stat(stat, py.stats.mod_stat[stat]);

  if (stat == A_STR)
    {
      py.flags.status |= PY_STR_WGT;
      create2_calc_bonuses();
    }
  else if (stat == A_DEX)
    create2_calc_bonuses();
  else if (stat == A_INT && class[py.misc.pclass].spell == MAGE)
    {
      create2_calc_spells(A_INT);
      create2_calc_mana(A_INT);
    }
  else if (stat == A_WIS && class[py.misc.pclass].spell == PRIEST)
    {
      create2_calc_spells(A_WIS);
      create2_calc_mana(A_WIS);
    }
  else if (stat == A_CON)
    create2_calc_hitpoints();
}


/* Recalculate the effect of all the stuff we use.                -CJS- */
void __fastcall__ create2_calc_bonuses(void)
{
  int32u item_flags;
  int8u old_dis_ac;
  register inven_type *i_ptr;
  register int i;


  if (py.flags.slow_digest)
    py.flags.food_digested++;
  if (py.flags.regenerate)
    py.flags.food_digested -= 3;
  py.flags.see_inv     = FALSE;
  py.flags.teleport    = FALSE;
  py.flags.free_act    = FALSE;
  py.flags.slow_digest = FALSE;
  py.flags.aggravate   = FALSE;
  py.flags.sustain_str = FALSE;
  py.flags.sustain_int = FALSE;
  py.flags.sustain_wis = FALSE;
  py.flags.sustain_con = FALSE;
  py.flags.sustain_dex = FALSE;
  py.flags.sustain_chr = FALSE;
  py.flags.fire_resist = FALSE;
  py.flags.acid_resist = FALSE;
  py.flags.cold_resist = FALSE;
  py.flags.regenerate  = FALSE;
  py.flags.lght_resist = FALSE;
  py.flags.ffall       = FALSE;

  old_dis_ac = py.misc.dis_ac;
  py.misc.ptohit  = create2_tohit_adj();       	/* Real To Hit   */
  py.misc.ptodam  = create2_todam_adj();       	/* Real To Dam   */
  py.misc.ptoac   = create2_toac_adj();        	/* Real To AC    */
  py.misc.pac     = 0;               			/* Real AC       */
  py.misc.dis_th  = py.misc.ptohit;  			/* Display To Hit        */
  py.misc.dis_td  = py.misc.ptodam;  			/* Display To Dam        */
  py.misc.dis_ac  = 0;           				/* Display AC            */
  py.misc.dis_tac = py.misc.ptoac;   			/* Display To AC         */


  vbxe_bank(VBXE_OBJBANK);                              // bank in inventory/object list

  for (i = INVEN_WIELD; i < INVEN_LIGHT; ++i) {
    i_ptr = &inventory[i];
    if (i_ptr->tval != TV_NOTHING) {
	  py.misc.ptohit += i_ptr->tohit;
	  if (i_ptr->tval != TV_BOW)            /* Bows can't damage. -CJS- */
	    py.misc.ptodam += i_ptr->todam;
	  py.misc.ptoac  += i_ptr->toac;
	  py.misc.pac += i_ptr->ac;
	  if (i_ptr->ident & ID_KNOWN2) {
	    py.misc.dis_th  += i_ptr->tohit;
	    if (i_ptr->tval != TV_BOW)
		  py.misc.dis_td  += i_ptr->todam; /* Bows can't damage. -CJS- */
	    py.misc.dis_tac += i_ptr->toac;
	    py.misc.dis_ac += i_ptr->ac;
	  }
	  else if (! (TR_CURSED & i_ptr->flags))
	    /* Base AC values should always be visible, as long as the item
	       is not cursed.  */
	    py.misc.dis_ac += i_ptr->ac;
	}
  }
  py.misc.dis_ac += py.misc.dis_tac;

  if (weapon_heavy)
    py.misc.dis_th += (py.stats.use_stat[A_STR] * 15 - inventory[INVEN_WIELD].weight);

  /* Add in temporary spell increases   */
  if (py.flags.invuln > 0) {
    py.misc.pac += 100;
    py.misc.dis_ac += 100;
  }
  if (py.flags.blessed > 0) {
    py.misc.pac    += 2;
    py.misc.dis_ac += 2;
  }
  if (py.flags.detect_inv > 0)
    py.flags.see_inv = TRUE;

  /* can't print AC here because might be in a store */
  if (old_dis_ac != py.misc.dis_ac)
    py.flags.status |= PY_ARMOR;

  item_flags = 0;
  i_ptr = &inventory[INVEN_WIELD];
  for (i = INVEN_WIELD; i < INVEN_LIGHT; ++i) {
    item_flags |= i_ptr->flags;
    ++i_ptr;
  }

  if (TR_SLOW_DIGEST & item_flags)
    py.flags.slow_digest = TRUE;
  if (TR_AGGRAVATE & item_flags)
    py.flags.aggravate = TRUE;
  if (TR_TELEPORT & item_flags)
    py.flags.teleport = TRUE;
  if (TR_REGEN & item_flags)
    py.flags.regenerate = TRUE;
  if (TR_RES_FIRE & item_flags)
    py.flags.fire_resist = TRUE;
  if (TR_RES_ACID & item_flags)
    py.flags.acid_resist = TRUE;
  if (TR_RES_COLD & item_flags)
    py.flags.cold_resist = TRUE;
  if (TR_FREE_ACT & item_flags)
    py.flags.free_act = TRUE;
  if (TR_SEE_INVIS & item_flags)
    py.flags.see_inv = TRUE;
  if (TR_RES_LIGHT & item_flags)
    py.flags.lght_resist = TRUE;
  if (TR_FFALL & item_flags)
    py.flags.ffall = TRUE;

  i_ptr = &inventory[INVEN_WIELD];
  for (i = INVEN_WIELD; i < INVEN_LIGHT; ++i) {
    if (TR_SUST_STAT & i_ptr->flags)
	  switch(i_ptr->p1) {
	    case 1: py.flags.sustain_str = TRUE; break;
	    case 2: py.flags.sustain_int = TRUE; break;
	    case 3: py.flags.sustain_wis = TRUE; break;
	    case 4: py.flags.sustain_con = TRUE; break;
	    case 5: py.flags.sustain_dex = TRUE; break;
	    case 6: py.flags.sustain_chr = TRUE; break;
	    default: break;
	  }
      ++i_ptr;
  }

  if (py.flags.slow_digest)
    py.flags.food_digested--;
  if (py.flags.regenerate)
    py.flags.food_digested += 3;

  vbxe_restore_bank();                                  // restore the previous VBXE bank
}


/* calculate number of spells player should have, and learn forget spells
   until that number is met -JEW- */
void __fastcall__ create2_calc_spells(int8u stat)
{
  register int8u i, j;
  int32u spell_flag, mask;
  int offset;
  int num_allowed, new_spells, num_known, levels;
  vtype tmp_str;
  char *p;
  spell_type *msp_ptr;


  vbxe_bank(VBXE_SPLBANK);

  msp_ptr = &magic_spell[py.misc.pclass-1][0];
  if (stat == A_INT) {
    p = "spell";
    offset = SPELL_OFFSET;
  }
  else {
    p = "prayer";
    offset = PRAYER_OFFSET;
  }

  /* check to see if know any spells greater than level, eliminate them */
  for (i = 31, mask = 0x80000000uL; mask; mask >>= 1, --i)
    if (mask & spell_learned) {
	  if (msp_ptr[i].slevel > py.misc.lev) {
	    spell_learned &= ~mask;
	    spell_forgotten |= mask;
	    sprintf(tmp_str, "You have forgotten the %s of %s.", p, spell_names[i+offset]);
	    bank_msg_print(tmp_str, CREATE2NUM);
	  }
	  else
	    break;
    }

  /* calc number of spells allowed */
  levels = py.misc.lev - class[py.misc.pclass].first_spell_lev + 1;
  switch(create2_stat_adj(stat)) {
    case 0:                 num_allowed = 0; break;
    case 1: case 2: case 3: num_allowed = 1 * levels; break;
    case 4: case 5:         num_allowed = 3 * levels / 2; break;
    case 6:                 num_allowed = 2 * levels; break;
    case 7:                 num_allowed = 5 * levels / 2; break;
  }

  num_known = 0;
  for (mask = 0x1; mask; mask <<= 1)
    if (mask & spell_learned)
      ++num_known;
  new_spells = num_allowed - num_known;

  if (new_spells > 0) {
    /* remember forgotten spells while forgotten spells exist of new_spells positive,
       remember the spells in the order that they were learned */
    for (i = 0; (spell_forgotten && new_spells && (i < num_allowed) && (i < 32)); ++i) {
	  /* j is (i+1)th spell learned */
	  j = spell_order[i];
	  /* shifting by amounts greater than number of bits in long gives
	     an undefined result, so don't shift for unknown spells */
	  if (j == 99)
	    mask = 0x0;
	  else
	    mask = 1L << j;
	  if (mask & spell_forgotten) {
	    if (msp_ptr[j].slevel <= py.misc.lev) {
		  --new_spells;
		  spell_forgotten &= ~mask;
		  spell_learned |= mask;
		  sprintf(tmp_str, "You have remembered the %s of %s.", p, spell_names[j+offset]);
	      bank_msg_print(tmp_str, CREATE2NUM);
		}
	    else
		  ++num_allowed;
	  }
	}

    if (new_spells > 0) {
	  /* determine which spells player can learn */
	  /* must check all spells here, in gain_spell() we actually check
	     if the books are present */
	  spell_flag = 0x7FFFFFFFL & ~spell_learned;

	  mask = 0x1;
	  i = 0;
	  for (j = 0, mask = 0x1; spell_flag; mask <<= 1, ++j)
	    if (spell_flag & mask) {
		  spell_flag &= ~mask;
		  if (msp_ptr[j].slevel <= py.misc.lev)
		    ++i;
	    }

	  if (new_spells > i)
	    new_spells = i;
	}
  }
  else if (new_spells < 0) {
    /* forget spells until new_spells zero or no more spells know, spells
	   are forgotten in the opposite order that they were learned */
    for (i = 31; new_spells && spell_learned; --i) {
	  /* j is the (i+1)th spell learned */
	  j = spell_order[i];
	  /* shifting by amounts greater than number of bits in long gives
	     an undefined result, so don't shift for unknown spells */
	  if (j == 99)
	    mask = 0x0;
	  else
	    mask = 1L << j;
	  if (mask & spell_learned) {
	    spell_learned &= ~mask;
	    spell_forgotten |= mask;
	    new_spells++;
	    sprintf(tmp_str, "You have forgotten the %s of %s.", p, spell_names[j+offset]);
	    bank_msg_print(tmp_str, CREATE2NUM);
	  }
	}

    new_spells = 0;
  }

  if (new_spells != py.flags.new_spells) {
    if (new_spells > 0 && py.flags.new_spells == 0) {
	  sprintf(tmp_str, "You can learn some new %ss now.", p);
	  bank_msg_print(tmp_str, CREATE2NUM);
	}

    py.flags.new_spells = new_spells;
    py.flags.status |= PY_STUDY;
  }
}


/* Gain some mana if you know at least one spell        -RAK-   */
void __fastcall__ create2_calc_mana(int8u stat)
{
  register int8u new_mana, levels;
  //register struct misc *p_ptr;
  register int32 value;


  //p_ptr = &py.misc;
  if (spell_learned != 0)
    {
      levels = py.misc.lev - class[py.misc.pclass].first_spell_lev + 1;
      switch(create2_stat_adj(stat))
	{
	case 0:         new_mana = 0; break;
	case 1: case 2: new_mana = 1 * levels; break;
	case 3:         new_mana = 3 * levels / 2; break;
	case 4:         new_mana = 2 * levels; break;
	case 5:         new_mana = 5 * levels / 2; break;
	case 6:         new_mana = 3 * levels; break;
	case 7:         new_mana = 4 * levels; break;
	}
      /* increment mana by one, so that first level chars have 2 mana */
      if (new_mana > 0)
	    ++new_mana;

      /* mana can be zero when creating character */
      if (py.misc.mana != new_mana)
	{
	  if (py.misc.mana != 0)
	    {
	      /* change current mana proportionately to change of max mana,
		 divide first to avoid overflow, little loss of accuracy */
	      value = (((long)py.misc.cmana << 16) + py.misc.cmana_frac) / py.misc.mana * new_mana;
	      py.misc.cmana = value >> 16;
	      py.misc.cmana_frac = value & 0xFFFF;
	    }
	  else
	    {
	      py.misc.cmana = new_mana;
	      py.misc.cmana_frac = 0;
	    }
	  py.misc.mana = new_mana;
	  /* can't print mana here, may be in store or inventory mode */
	  py.flags.status |= PY_MANA;
	}
    }
  else if (py.misc.mana != 0)
    {
      py.misc.mana = 0;
      py.misc.cmana = 0;
      /* can't print mana here, may be in store or inventory mode */
      py.flags.status |= PY_MANA;
    }
}


/* Calculate the players hit points */
void __fastcall__ create2_calc_hitpoints(void)
{
  register int8u hitpoints;
  //register struct misc *p_ptr;
  register int32 value;


  //p_ptr = &py.misc;
  hitpoints = player_hp[py.misc.lev-1] + (create2_con_adj() * py.misc.lev);
  /* always give at least one point per level + 1 */
  if (hitpoints < (py.misc.lev + 1))
    hitpoints = py.misc.lev + 1;

  if (py.flags.status & PY_HERO)
    hitpoints += 10;
  if (py.flags.status & PY_SHERO)
    hitpoints += 20;

  /* mhp can equal zero while character is being created */
  if ((hitpoints != py.misc.mhp) && (py.misc.mhp != 0))
    {
      /* change current hit points proportionately to change of mhp,
	 divide first to avoid overflow, little loss of accuracy */
      value = (((long)py.misc.chp << 16) + py.misc.chp_frac) / py.misc.mhp * hitpoints;
      py.misc.chp = value >> 16;
      py.misc.chp_frac = value & 0xFFFF;
      py.misc.mhp = hitpoints;

      /* can't print hit points here, may be in store or inventory mode */
      py.flags.status |= PY_HP;
    }
}


/* To avoid a bunch of banking, this code is moved here from INITBANK */
void init_calc_spells_prayers(void)
{
  vbxe_bank(VBXE_SPLBANK);                              // for class struct

  if (class[py.misc.pclass].spell == MAGE)
	{                                       /* Magic realm   */
	  bank_vbxe_clear(CREATE2NUM);

	  create2_calc_spells(A_INT);
	  create2_calc_mana(A_INT);
	}
  else if (class[py.misc.pclass].spell == PRIEST)
	{                                       /* Clerical realm*/
	  create2_calc_spells(A_WIS);
	  bank_vbxe_clear(CREATE2NUM);
	  create2_calc_mana(A_WIS);
	}

  return;
}
