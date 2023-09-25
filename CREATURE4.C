/* source/creature.c: handle monster movement and attacks

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("MON4BANK")
#pragma data-name("MON4BANK")
#pragma rodata-name("MON4BANK")


/* Prototypes of functions copied into this bank */
int8 __fastcall__ creature3_bit_pos(int32u *test);
int8u __fastcall__ creature3_is_a_vowel(char ch);
void __fastcall__ creature3_lite_spot(int8u y, int8u x);
void __fastcall__ creature3_move_rec(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ creature3_take_hit(int8u damage, char *hit_from);
int8u __fastcall__ creature3_in_bounds(int8u y, int8u x);
int8u __fastcall__ creature3_check_mon_lite(int8u y, int8u x);
int8u __fastcall__ creature3_distance(int8u y1, int8u x1, int8u y2, int8u x2);
int8u __fastcall__ creature3_popm(void);
int16u __fastcall__ creature3_max_hp(int8u *array);
int16u __fastcall__ creature3_pdamroll(int8u *array);
int16u __fastcall__ creature3_damroll(int8u num, int8u sides);
int8u __fastcall__ creature3_los(int8u fromY, int8u fromX, int8u toY, int8u toX);
void __fastcall__ teleport_to(int8u ny, int8u nx);
int8u __fastcall__ summon_undead(int8u *y, int8u *x);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MON4NUM

#undef  INBANK_MOD
#define INBANK_MOD			creature3



int8 __fastcall__ creature3_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

int8u __fastcall__ creature3_is_a_vowel(char ch)
#include "inbank\is_a_vowel.c"

void __fastcall__ creature3_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

void __fastcall__ creature3_move_rec(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\move_rec.c"

void __fastcall__ creature3_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"

int8u __fastcall__ creature3_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"


/* Makes sure a new creature gets lit up.			-CJS- */
int8u __fastcall__ creature3_check_mon_lite(int8u y, int8u x)
{
  int8u monptr;

  //monptr = cave[y][x].cptr;
  vbxe_bank(VBXE_CAVBANK);
  monptr = (CAVE_ADR(y,x))->cptr;

  if (monptr <= 1)
    return FALSE;
  else {
    bank_update_mon(monptr, MON4NUM);
    vbxe_bank(VBXE_MONBANK);
    return m_list[monptr].ml;
  }
}

int8u __fastcall__ creature3_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

int8u __fastcall__ creature3_popm(void)
#include "inbank\popm.c"

/* Gives Max hit points					-RAK-	*/
int16u __fastcall__ creature3_max_hp(int8u *array)
{
  return(array[0] * array[1]);
}


int16u __fastcall__ creature3_pdamroll(int8u *array)
#include "inbank\pdamroll.c"

int16u __fastcall__ creature3_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ creature3_los(int8u fromY, int8u fromX, int8u toY, int8u toX)
#include "inbank\los.c"


/* Places creature adjacent to given location		-RAK-	*/
/* Rats and Flys are fun!					 */
int8u __fastcall__ multiply_monster(int8u y, int8u x, int16u cr_index, int8u monptr)
{
  register int8u i, j, k;
  register cave_type *c_ptr;
  int8u result;
  int8u cptr;


  i = 0;
  do {
    j = y - 2 + randint(3);
    k = x - 2 + randint(3);

    /* don't create a new creature on top of the old one, that causes invincible/invisible creatures to appear */
    if (creature3_in_bounds(j, k) && ((j != y) || (k != x))) {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(j,k);

	  if ((c_ptr->fval <= MAX_OPEN_SPACE) && (c_ptr->tptr == 0) && (c_ptr->cptr != 1)) {
	    if (c_ptr->cptr > 1) {     /* Creature there already?	*/
	      cptr = c_ptr->cptr;					// save the cptr
		  vbxe_bank(VBXE_MONBANK);
		  /* Some critters are cannibalistic! */
		  if ((c_list[cr_index].cmove & CM_EATS_OTHER)
		      /* Check the experience level -CJS- */
		      && c_list[cr_index].mexp >= c_list[m_list[cptr].mptr].mexp) {
		    /* It ate an already processed monster.Handle normally.*/
		    if (monptr < cptr)
			  bank_delete_monster(cptr, MON4NUM);
		      /* If it eats this monster, an already processed mosnter will take its place, causing all kinds of havoc.
			     Delay the kill a bit. */
		    else
			  bank_fix1_delete_monster(cptr, MON4NUM);

		    /* in case compact_monster() is called,it needs monptr */
		    hack_monptr = monptr;
		    /* Place_monster() may fail if monster list full.  */
		    result = bank_place_monster(j, k, cr_index, FALSE, MON4NUM);
		    hack_monptr = -1;
		    if (!result)
			  return FALSE;
		    ++mon_tot_mult;
		    return (creature3_check_mon_lite(j, k));
		  }
		}
	    else {  /* All clear,  place a monster  */
		  /* in case compact_monster() is called,it needs monptr */
		  hack_monptr = monptr;
		  /* Place_monster() may fail if monster list full.  */
		  result = bank_place_monster(j, k, cr_index, FALSE, MON4NUM);
		  hack_monptr = -1;
		  if (!result)
		    return FALSE;
		  ++mon_tot_mult;
		  return (creature3_check_mon_lite(j, k));
		}
	  }
	}

    ++i;
  } while (i <= 18);

  return FALSE;
}


/* Creatures can cast spells too.  (Dragon Breath)	-RAK-	*/
/* cast_spell = true if creature changes position	*/
/* took_turn  = true if creature casts a spell		*/
void __fastcall__ mon_cast_spell(int8u monptr, int8u *took_turn)
{
  int32u i;
  int8u y, x, chance, thrown_spell, r1;
  register int8u k;
  int spell_choice[30];
  //vtype cdesc, outval, ddesc;
  vtype ddesc;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int16u mon;


  //if (death)						// FIXME: freeing up three bytes in MON4BANK
  //  return;

  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];
  mon = m_ptr->mptr;

  chance = (r_ptr->spells & CS_FREQ);
  /* 1 in x chance of casting spell */
  if (randint(chance) != 1)
    *took_turn	= FALSE;
  /* Must be within certain range */
  else if (m_ptr->cdis > MAX_SPELL_DIS)
    *took_turn	= FALSE;
  /* Must have unobstructed Line-Of-Sight */
  else if (!creature3_los(char_row, char_col, m_ptr->fy, m_ptr->fx))
    *took_turn	= FALSE;
  else {	/* Creature is going to cast a spell */
    *took_turn  = TRUE;
    /* Check to see if monster should be lit. */
    bank_update_mon(monptr, MON4NUM);
    /* Describe the attack */

    vbxe_bank(VBXE_MONBANK);							// make sure monster bank is in -SJ
    if (m_ptr->ml)
	  sprintf(out_val1, "The %s ", r_ptr->name);
    else
	  strcpy(out_val1, "It ");

    /* For "DIED_FROM" string	 */
    if (CM_WIN & r_ptr->cmove)
	  sprintf(ddesc, "The %s", r_ptr->name);
    else if (creature3_is_a_vowel(r_ptr->name[0]))
	  sprintf (ddesc, "an %s", r_ptr->name);
    else
	  sprintf(ddesc, "a %s", r_ptr->name);
    /* End DIED_FROM		       */

    /* Extract all possible spells into spell_choice */
    i = (r_ptr->spells & ~CS_FREQ);
    k = 0;
    while(i != 0) {
	  spell_choice[k] = creature3_bit_pos(&i);
	  ++k;
	}

    /* Choose a spell to cast */
    thrown_spell = spell_choice[randint(k) - 1];
    ++thrown_spell;
    /* all except teleport_away() and drain mana spells always disturb */
    if (thrown_spell > 6 && thrown_spell != 17)
	  bank_disturb(1, 0, MON4NUM);
    /* save some code/data space here, with a small time penalty */
    if ((thrown_spell < 14 && thrown_spell > 6) || (thrown_spell == 16)) {
	  strcat(out_val1, "casts a spell.");
	  bank_msg_print(out_val1, MON4NUM);
	}

    /* Cast the spell. */
    switch(thrown_spell) {
	  case 5:	 /* Teleport Short */
	    bank_teleport_away(monptr, 5, MON4NUM);
	    break;
	  case 6:	 /*Teleport Long */
	    bank_teleport_away(monptr, MAX_SIGHT, MON4NUM);
	    break;
	  case 7:	 /*Teleport To	 */
	    teleport_to(m_ptr->fy, m_ptr->fx);
	    break;
	  case 8:	 /*Light Wound	 */
	    if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else
	      creature3_take_hit(creature3_damroll(3, 8), ddesc);
	    break;
	  case 9:	 /*Serious Wound */
	    if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else
	      creature3_take_hit(creature3_damroll(8, 8), ddesc);
	    break;
	  case 10:  /*Hold Person	  */
	    if (py.flags.free_act)
	      bank_msg_print("You are unaffected.", MON4NUM);
	    else if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else if (py.flags.paralysis > 0)
	      py.flags.paralysis += 2;
	    else
	      py.flags.paralysis = randint(5)+4;
	    break;
	  case 11:  /*Cause Blindness*/
	    if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else if (py.flags.blind > 0)
	      py.flags.blind += 6;
	    else
	      py.flags.blind += 12 + randint(3);
	    break;
	  case 12:  /*Cause Confuse */
	    if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else if (py.flags.confused > 0)
	      py.flags.confused += 2;
	    else
	      py.flags.confused = randint(5) + 3;
	    break;
	  case 13:  /*Cause Fear	  */
	    if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else if (py.flags.afraid > 0)
	      py.flags.afraid += 2;
	    else
	      py.flags.afraid = randint(5) + 3;
	    break;
	  case 14:  /*Summon Monster*/
	    strcat(out_val1, "magically summons a monster!");
	    bank_msg_print(out_val1, MON4NUM);
	    y = char_row;
	    x = char_col;
	    /* in case compact_monster() is called,it needs monptr */
	    hack_monptr = monptr;
	    bank_summon_monster(&y, &x, FALSE, MON4NUM);
	    hack_monptr = -1;

	    vbxe_bank(VBXE_CAVBANK);
	    bank_update_mon((CAVE_ADR(y,x))->cptr, MON4NUM);
	    break;
	  case 15:  /*Summon Undead*/
	    strcat(out_val1, "magically summons an undead!");
	    bank_msg_print(out_val1, MON4NUM);
	    y = char_row;
	    x = char_col;
	    /* in case compact_monster() is called,it needs monptr */
	    hack_monptr = monptr;
	    summon_undead(&y, &x);
	    hack_monptr = -1;
	    bank_update_mon((CAVE_ADR(y,x))->cptr, MON4NUM);
	    break;
	  case 16:  /*Slow Person	 */
	    if (py.flags.free_act)
	      bank_msg_print("You are unaffected.", MON4NUM);
	    else if (bank_player_saves(MON4NUM))
	      bank_msg_print("You resist the effects of the spell.", MON4NUM);
	    else if (py.flags.slow > 0)
	      py.flags.slow += 2;
	    else
	      py.flags.slow = randint(5) + 3;
	    break;
	  case 17:  /*Drain Mana	 */
	    if (py.misc.cmana > 0) {
	      bank_disturb(1, 0, MON4NUM);
	      sprintf(out_val2, "%sdraws psychic energy from you!", out_val1);
	      bank_msg_print(out_val2, MON4NUM);
	      if (m_ptr->ml) {
		    sprintf(out_val2, "%sappears healthier.", out_val1);
		    bank_msg_print(out_val2, MON4NUM);
		  }
	      r1 = (randint(r_ptr->level) >> 1) + 1;
	      if (r1 > py.misc.cmana) {
		    r1 = py.misc.cmana;
		    py.misc.cmana = 0;
		    py.misc.cmana_frac = 0;
		  }
	      else
		    py.misc.cmana -= r1;
	      bank_prt_cmana(MON4NUM);
	      m_ptr->hp += 6*(r1);
	    }
	    break;
	  case 20:  /*Breath Light */
	    strcat(out_val1, "breathes lightning.");
	    bank_msg_print(out_val1, MON4NUM);
	    bank_breath(GF_LIGHTNING, char_row, char_col, (m_ptr->hp / 4), ddesc, monptr, MON4NUM);
	    break;
	  case 21:  /*Breath Gas	 */
	    strcat(out_val1, "breathes gas.");
	    bank_msg_print(out_val1, MON4NUM);
	    bank_breath(GF_POISON_GAS, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr, MON4NUM);
	    break;
	  case 22:  /*Breath Acid	 */
	    strcat(out_val1, "breathes acid.");
	    bank_msg_print(out_val1, MON4NUM);
	    bank_breath(GF_ACID, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr, MON4NUM);
	    break;
	  case 23:  /*Breath Frost */
	    strcat(out_val1, "breathes frost.");
	    bank_msg_print(out_val1, MON4NUM);
	    bank_breath(GF_FROST, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr, MON4NUM);
	    break;
	  case 24:  /*Breath Fire	 */
	    strcat(out_val1, "breathes fire.");
	    bank_msg_print(out_val1, MON4NUM);
	    bank_breath(GF_FIRE, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr, MON4NUM);
	    break;
	  default:
	    strcat(out_val1, "cast unknown spell.");
	    bank_msg_print(out_val1, MON4NUM);
	}
    /* End of spells				       */

    if (m_ptr->ml) {
      vbxe_bank(VBXE_GENBANK);
	  c_recall[mon].r_spells |= 1L << (thrown_spell-1);
	  if ((c_recall[mon].r_spells & CS_FREQ) != CS_FREQ)
	    c_recall[mon].r_spells++;
	  if (death && c_recall[mon].r_deaths < MAX_SHORT)
	    c_recall[mon].r_deaths++;
	}
  }
}


/* Teleport player to spell casting creature		-RAK-	*/
void __fastcall__ teleport_to(int8u ny, int8u nx)
{
  int8u dis, ctr, y, x;
  register int8u i, j;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  dis = 1;
  ctr = 0;
  do {
    y = ny + (randint(2*dis+1) - (dis + 1));
    x = nx + (randint(2*dis+1) - (dis + 1));
    ++ctr;
    if (ctr > 9) {
	  ctr = 0;
	  ++dis;
	}
	c_ptr = CAVE_ADR(y, x);
  //} while (!creature3_in_bounds(y, x) || (cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr >= 2));
  } while (!creature3_in_bounds(y, x) || (c_ptr->fval >= MIN_CLOSED_SPACE) || (c_ptr->cptr >= 2));

  creature3_move_rec(char_row, char_col, y, x);
  for (i = char_row-1; i <= char_row+1; ++i)
    for (j = char_col-1; j <= char_col+1; ++j) {
	  //c_ptr = &cave[i][j];
	  //c_ptr->tl = FALSE;
	  vbxe_bank(VBXE_CAVBANK);
	  (CAVE_ADR(i, j))->flags &= ~CAVE_TL;
	  creature3_lite_spot(i, j);
    }

  creature3_lite_spot(char_row, char_col);
  char_row = y;
  char_col = x;
  bank_check_view(MON4NUM);
  /* light creatures */
  bank_creatures(FALSE, MON4NUM);
}


/* Places undead adjacent to given location		-RAK-	*/
int8u __fastcall__ summon_undead(int8u *y, int8u *x)
{
  register cave_type *cave_ptr;
  register int8u i, j, k;
  int16u l, m;
  int8u ctr, summon;


  i = 0;
  summon = FALSE;

  vbxe_bank(VBXE_MONBANK);
  l = m_level[MAX_MONS_LEVEL];

  /* Find an appropriate monster */
  do {
    m = randint(l) - 1;
    ctr = 0;
    do {
	  if (c_list[m].cdefense & CD_UNDEAD) {
	    ctr = 20;
	    l = 0;
	  }
	  else {
	    ++m;
	    if (m > l)
		  ctr = 20;
	    else
		  ++ctr;
	  }
	} while (ctr <= 19);
  } while(l != 0);

  /* Place the monster on the map */
  do {
    j = *y - 2 + randint(3);
    k = *x - 2 + randint(3);
    if (creature3_in_bounds(j, k)) {
      vbxe_bank(VBXE_CAVBANK);
	  cave_ptr = CAVE_ADR(j, k);

	  if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
	    /* Place_monster() should always return TRUE here.  */
	    if (!bank_place_monster(j, k, m, FALSE, MON4NUM))
		  return FALSE;

	    summon = TRUE;
	    i = 9;
	    *y = j;
	    *x = k;
	  }
	}
    ++i;
  } while(i <= 9);

  return(summon);
}


/* Choose correct directions for monster movement	-RAK-	*/
void __fastcall__ get_moves(int8u monptr, int8u *mm)
{
  int16 y, ay, x, ax;
  int8u move_val;


  vbxe_bank(VBXE_MONBANK);

  y = (int16) m_list[monptr].fy - (int16) char_row;
  x = (int16) m_list[monptr].fx - (int16) char_col;
  if (y < 0) {
    move_val = 8;
    ay = -y;
  }
  else {
    move_val = 0;
    ay = y;
  }
  if (x > 0) {
    move_val += 4;
    ax = x;
  }
  else
    ax = -x;

  /* this has the advantage of preventing the diamond maneuvre, also faster */
  if (ay > (ax << 1))
    move_val += 2;
  else if (ax > (ay << 1))
    ++move_val;

  switch(move_val) {
    case 0:
      mm[0] = 9;
      if (ay > ax) {
	    mm[1] = 8;
	    mm[2] = 6;
	    mm[3] = 7;
	    mm[4] = 3;
	  }
      else {
	    mm[1] = 6;
	    mm[2] = 8;
	    mm[3] = 3;
	    mm[4] = 7;
	  }
      break;
    case 1: case 9:
      mm[0] = 6;
      if (y < 0) {
	    mm[1] = 3;
	    mm[2] = 9;
	    mm[3] = 2;
	    mm[4] = 8;
	  }
      else {
	    mm[1] = 9;
	    mm[2] = 3;
	    mm[3] = 8;
	    mm[4] = 2;
	  }
      break;
    case 2: case 6:
      mm[0] = 8;
      if (x < 0) {
	    mm[1] = 9;
	    mm[2] = 7;
	    mm[3] = 6;
	    mm[4] = 4;
	  }
      else {
	    mm[1] = 7;
	    mm[2] = 9;
	    mm[3] = 4;
	    mm[4] = 6;
	  }
      break;
    case 4:
      mm[0] = 7;
      if (ay > ax) {
	    mm[1] = 8;
	    mm[2] = 4;
	    mm[3] = 9;
	    mm[4] = 1;
	  }
      else {
	    mm[1] = 4;
	    mm[2] = 8;
	    mm[3] = 1;
	    mm[4] = 9;
	  }
      break;
    case 5: case 13:
      mm[0] = 4;
      if (y < 0) {
	    mm[1] = 1;
	    mm[2] = 7;
	    mm[3] = 2;
	    mm[4] = 8;
	  }
      else {
	    mm[1] = 7;
	    mm[2] = 1;
	    mm[3] = 8;
	    mm[4] = 2;
	  }
      break;
    case 8:
      mm[0] = 3;
      if (ay > ax) {
	    mm[1] = 2;
	    mm[2] = 6;
	    mm[3] = 1;
	    mm[4] = 9;
	  }
      else {
	    mm[1] = 6;
	    mm[2] = 2;
	    mm[3] = 9;
	    mm[4] = 1;
	  }
      break;
    case 10: case 14:
      mm[0] = 2;
      if (x < 0) {
	    mm[1] = 3;
	    mm[2] = 1;
	    mm[3] = 6;
	    mm[4] = 4;
	  }
      else {
	    mm[1] = 1;
	    mm[2] = 3;
	    mm[3] = 4;
	    mm[4] = 6;
	  }
      break;
    case 12:
      mm[0] = 1;
      if (ay > ax) {
	    mm[1] = 2;
	    mm[2] = 4;
	    mm[3] = 3;
	    mm[4] = 7;
	  }
      else {
	    mm[1] = 4;
	    mm[2] = 2;
	    mm[3] = 7;
	    mm[4] = 3;
	  }
      break;
  }

  vbxe_restore_bank();
}
