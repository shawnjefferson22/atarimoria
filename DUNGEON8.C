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
#pragma code-name("DUNGEON8BANK")
#pragma rodata-name("DUNGEON8BANK")
#pragma data-name("DUNGEON8BANK")


/* used to calculate the number of blows the player gets in combat */
/* only used here, so moved from VBXE memory to this bank -SJ      */
int8u blows_table[7][6] = {
/* STR/W:          9  18  67 107 117 118   : DEX */
/* <2 */        {  1,  1,  1,  1,  1,  1 },
/* <3 */        {  1,  1,  1,  1,  2,  2 },
/* <4 */        {  1,  1,  1,  2,  2,  3 },
/* <5 */        {  1,  1,  2,  2,  3,  3 },
/* <7 */        {  1,  2,  2,  3,  3,  4 },
/* <9 */        {  1,  2,  2,  3,  4,  4 },
/* >9 */        {  2,  2,  3,  3,  4,  4 }
};


/* Prototypes of functions copied into this bank */
void __fastcall__ dungeon8_erase_line(int8u row, int8u col);
int8u __fastcall__ dungeon8_no_light(void);
int16u __fastcall__ dungeon8_pdamroll(int8u *array);
int16u __fastcall__ dungeon8_damroll(int8u num, int8u sides);
int8u __fastcall__ dungeon8_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type);
void __fastcall__ dungeon8_invcopy(inven_type *to, int16u from_index);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN8NUM

#undef  INBANK_MOD
#define INBANK_MOD			dungeon8


void __fastcall__ dungeon8_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

int8u __fastcall__ dungeon8_no_light(void)
#include "inbank\no_light.c"

int16u __fastcall__ dungeon8_pdamroll(int8u *array)
#include "inbank\pdamroll.c"

int16u __fastcall__ dungeon8_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

void __fastcall__ dungeon8_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ dungeon8_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type)
#include "inbank\test_hit.c"


/* Weapon weight VS strength and dexterity		-RAK-	*/
int8u __fastcall__ attack_blows(int16u weight, int8 *wtohit)
{
  register int8u adj_weight;
  register int8u str_index, dex_index, s, d;

  s = py.stats.use_stat[A_STR];
  d = py.stats.use_stat[A_DEX];
  if ((int16) s * 15 < weight) {
    *wtohit = (int8) ((int16) s * 15 - weight);
    return(1);
  }
  else {
    *wtohit = 0;
    if      (d <  1)  dex_index = 0;
    else if (d <  19) dex_index = 1;
    else if (d <  68) dex_index = 2;
    else if (d < 108) dex_index = 3;
    else if (d < 118) dex_index = 4;
    else		      dex_index = 5;

    adj_weight = (s * 10 / weight);

    if      (adj_weight < 2) str_index = 0;
    else if (adj_weight < 3) str_index = 1;
    else if (adj_weight < 4) str_index = 2;
    else if (adj_weight < 5) str_index = 3;
    else if (adj_weight < 7) str_index = 4;
    else if (adj_weight < 9) str_index = 5;
    else			         str_index = 6;

    return(blows_table[str_index][dex_index]);
  }
}


/* Player attacks a (poor, defenseless) creature	-RAK-	*/
void __fastcall__ py_attack(int8u y, int8u x)
{
  int8u blows, crptr;
  int16u monptr;
  int8 tot_tohit;
  register int8 k;
  int16 base_tohit;
  //vtype m_name, out_val;
  register inven_type *i_ptr;
  int8u ml;
  int16u cdef;


  vbxe_bank(VBXE_CAVBANK);
  crptr = (CAVE_ADR(y, x))->cptr;						//crptr = cave[y][x].cptr;

  vbxe_bank(VBXE_MONBANK);
  monptr = m_list[crptr].mptr;
  m_list[crptr].csleep = 0;								// wake up the monster
  ml = m_list[crptr].ml;								// save light status of creature

  /* Does the player know what he's fighting?	   */
  if (!ml)
    strcpy(out_val1, "it");
  else
    sprintf(out_val1, "the %s", c_list[monptr].name);

  vbxe_bank(VBXE_OBJBANK);
  i_ptr = &inventory[INVEN_WIELD];

  if (i_ptr->tval != TV_NOTHING)		/* Proper weapon */
    blows = attack_blows(i_ptr->weight, &tot_tohit);
  else {				  				/* Bare hands?   */
    blows = 2;
    tot_tohit = -3;
  }
  if ((i_ptr->tval >= TV_SLING_AMMO) && (i_ptr->tval <= TV_SPIKE))
    blows = 1;							/* Fix for arrows */

  tot_tohit += py.misc.ptohit;

  /* if creature not lit, make it more difficult to hit */
  if (ml)
    base_tohit = py.misc.bth;
  else {
    vbxe_bank(VBXE_SPLBANK);
    base_tohit = (py.misc.bth / 2) - (tot_tohit * (BTH_PLUS_ADJ-1)) - (py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTH] / 2);
  }

  //sprintf(dbgstr, "base_tohit: %d tohit:%d", base_tohit, tot_tohit);
  //bank_prt(dbgstr, 23, 0, DUN8NUM);
  //cgetc();

  /* Loop for number of blows, trying to hit the critter.	  */
  do {
	vbxe_bank(VBXE_MONBANK);
    if (dungeon8_test_hit(base_tohit, py.misc.lev, tot_tohit, c_list[monptr].ac, CLA_BTH)) {
	  sprintf(out_val2, "You hit %s.", out_val1);
	  bank_msg_print(out_val2, DUN8NUM);

	  vbxe_bank(VBXE_OBJBANK);
	  if (i_ptr->tval != TV_NOTHING) {
	    k = dungeon8_pdamroll(i_ptr->damage);
	    k = bank_tot_dam(i_ptr, k, monptr, DUN8NUM);
	    k = bank_critical_blow(i_ptr->weight, tot_tohit, k, CLA_BTH, DUN8NUM);
	  }
	  else {			      /* Bare hands!?  */
	    k = dungeon8_damroll(1, 1);
	    k = bank_critical_blow(1, 0, k, CLA_BTH, DUN8NUM);
	  }
	  k += py.misc.ptodam;
	  if (k < 0) k = 0;

      vbxe_bank(VBXE_MONBANK);
	  if (py.flags.confuse_monster) {
	    py.flags.confuse_monster = FALSE;
	    bank_msg_print("Your hands stop glowing.", DUN8NUM);
	    if ((c_list[monptr].cdefense & CD_NO_SLEEP) || (randint(MAX_MONS_LEVEL) < c_list[monptr].level))
		  sprintf(out_val2, "%s is unaffected.", out_val1);
	    else {
		  sprintf(out_val2, "%s appears confused.", out_val1);
		  m_list[crptr].confused = TRUE;
		}
	    bank_msg_print(out_val2, DUN8NUM);
	    if (m_list[crptr].ml && randint(4) == 1) {
		  cdef = c_list[monptr].cdefense;
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[monptr].r_cdefense |= cdef & CD_NO_SLEEP;
		  vbxe_restore_bank();
		}
	  }

	  /* See if we done it in. */
	  if (bank_mon_take_hit(crptr, k, DUN8NUM) >= 0) {
	    sprintf(out_val2, "You have slain %s.", out_val1);
	    bank_msg_print(out_val2, DUN8NUM);
	    bank_prt_experience(DUN8NUM);
	    //blows = 0;
	    blows = 1;						// fix to keep blows positive! -SJ
	  }

	  vbxe_bank(VBXE_OBJBANK);
	  if ((i_ptr->tval >= TV_SLING_AMMO) && (i_ptr->tval <= TV_SPIKE)) {   /* Use missiles up*/
	    i_ptr->number--;
	    inven_weight -= i_ptr->weight;
	    py.flags.status |= PY_STR_WGT;
	    if (i_ptr->number == 0) {
		  --equip_ctr;
		  bank_py_bonuses(i_ptr, -1, DUN8NUM);
		  dungeon8_invcopy(i_ptr, OBJ_NOTHING);
		  bank_calc_bonuses(DUN8NUM);
		}
	  }
	}
    else {
	  sprintf(out_val2, "You miss %s.", out_val1);
	  bank_msg_print(out_val2, DUN8NUM);
	}

    --blows;
  } while (blows >= 1);
}


/* gain spells when player wants to		- jw */
void __fastcall__ gain_spells(void)
{
  char query;
  int8u stat, diff_spells, new_spells;
  int8u spells[31], offset, last_known;
  register int8 i, j;
  int32u spell_flag, mask;
  //vtype tmp_str;
  register spell_type *msp_ptr;


  /* Priests don't need light because they get spells from their god,
     so only fail when can't see if player has MAGE spells.  This check
     is done below.  */
  if (py.flags.confused > 0) {
    bank_msg_print("You are too confused.", DUN8NUM);
    return;
  }

  vbxe_bank(VBXE_SPLBANK);

  new_spells = py.flags.new_spells;
  diff_spells = 0;

  msp_ptr = &magic_spell[py.misc.pclass-1][0];
  if (class[py.misc.pclass].spell == MAGE) {
    stat = A_INT;
    offset = SPELL_OFFSET;

    /* People with MAGE spells can't learn spells if they can't read their books. */
    if (py.flags.blind > 0) {
	  bank_msg_print("You can't see to read your spell book!", DUN8NUM);
	  return;
	}
    else if (dungeon8_no_light()) {
	  bank_msg_print("You have no light to read by.", DUN8NUM);
	  return;
	}
  }
  else {
    stat = A_WIS;
    offset = PRAYER_OFFSET;
  }

  for (last_known = 0; last_known < 32; ++last_known)
    if (spell_order[last_known] == 99)
      break;

  if (!new_spells) {
    sprintf(out_val1, "You can't learn any new %ss!", (stat == A_INT ? "spell" : "prayer"));
    bank_msg_print(out_val1, DUN8NUM);
    free_turn_flag = TRUE;
  }
  else {
    /* determine which spells player can learn */
    /* mages need the book to learn a spell, priests do not need the book */
    if (stat == A_INT) {
	  spell_flag = 0;

	  vbxe_bank(VBXE_OBJBANK);
	  for (i = 0; i < inven_ctr; ++i)
	    if (inventory[i].tval == TV_MAGIC_BOOK)
	      spell_flag |= inventory[i].flags;

	  vbxe_restore_bank();
	}
    else
	  spell_flag = 0x7FFFFFFF;

    /* clear bits for spells already learned */
    spell_flag &= ~spell_learned;

    mask = 0x1;
    i = 0;
    for (j = 0, mask = 0x1; spell_flag; mask <<= 1, ++j)
	  if (spell_flag & mask) {
	    spell_flag &= ~mask;
	    if (msp_ptr[j].slevel <= py.misc.lev) {
		  spells[i] = j;
		  ++i;
	    }
	  }

    if (new_spells > i) {
	  bank_msg_print("You seem to be missing a book.", DUN8NUM);
	  diff_spells = new_spells - i;
	  new_spells = i;
	}

    if (new_spells == 0)
	  ;
    else if (stat == A_INT) {
	  /* get to choose which mage spells will be learned */
	  bank_vbxe_savescreen(DUN8NUM);
	  bank_print_spells(spells, i, FALSE, -1, DUN8NUM);

	  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", i+2, 0, DUN8NUM);

	  while (new_spells && bank_get_com("Learn which spell?", &query, DUN8NUM)) {
	    j = query - 'a';
	    /* test j < 23 in case i is greater than 22, only 22 spells are actually shown on the screen, so limit choice to those */
	    if (j >= 0 && j < i && j < 22) {
		  --new_spells;
		  spell_learned |= 1L << spells[j];
		  spell_order[last_known++] = spells[j];
		  for (; j <= i-1; ++j)
		    spells[j] = spells[j+1];
		  --i;
		  if (i > 0) {									// avoid printing again if can't learn more spells -SJ
		    dungeon8_erase_line(j+1, 0);
		    bank_print_spells(spells, i, FALSE, -1, DUN8NUM);
	      }
		}
	  }
	  bank_vbxe_restorescreen(DUN8NUM);
	}
    else {
	  /* pick a prayer at random */
	  while (new_spells) {
	    j = randint(i) - 1;
	    spell_learned |= 1L << spells[j];
	    spell_order[last_known++] = spells[j];
	    sprintf(out_val1, "You have learned the prayer of %s.", spell_names[spells[j]+offset]);
	    bank_msg_print(out_val1, DUN8NUM);
	    for (; j <= i-1; ++j)
		  spells[j] = spells[j+1];
	    --i;
	    --new_spells;
	  }
	}

    py.flags.new_spells = new_spells + diff_spells;
    if (py.flags.new_spells == 0)
	  py.flags.status |= PY_STUDY;
      /* set the mana for first level characters when they learn their first spell */
      if (py.misc.mana == 0)
	    bank_calc_mana(stat, DUN8NUM);
  }
}
