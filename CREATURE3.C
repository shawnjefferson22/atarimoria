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


#pragma code-name("MON3BANK")
#pragma data-name("MON3BANK")
#pragma rodata-name("MON3BANK")


/* Prototypes of functions copied into this bank */
int8u __fastcall__ creature2_is_a_vowel(char ch);
int8u __fastcall__ creature2_damroll(int8u num, int8u sides);
int8u __fastcall__ creature2_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type);
void __fastcall__ creature2_take_hit(int8u damage, char *hit_from);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MON3NUM

#undef  INBANK_MOD
#define INBANK_MOD			creature2


int8u __fastcall__ creature2_is_a_vowel(char ch)
#include "inbank\is_a_vowel.c"

/* generates damage for 2d6 style dice rolls */
int8u __fastcall__ creature2_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ creature2_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type)
#include "inbank\test_hit.c"

void __fastcall__ creature2_take_hit(int8u damage, char *hit_from)
#include "inbank\take_hit.c"


/* Make an attack on the player (chuckle.)		-RAK-	*/
void __fastcall__ make_attack(int8u monptr)
{
  int8u attype, adesc, adice, asides;
  int8u i, j, damage, flag, attackn, notice, visible;
  int32 gold;
  int8u *attstr;
  //vtype cdesc, tmp_str, ddesc;		tmp_str replaced with out_val1, ddesc replaced with out_val2 -SJ
  static vtype cdesc;
  register creature_type *r_ptr;
  register monster_type *m_ptr;
  register inven_type *i_ptr;
  int16 p1;								// temporary p1 value
  int16u mon;							// temporary monster number
  int16u cdef;							// temporary cdefense holder


  if (death)  /* don't beat a dead body! */
    return;

  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];
  mon = m_ptr->mptr;					// save the monster number -SJ

  if (!m_ptr->ml)
    strcpy(cdesc, "It ");
  else
    sprintf(cdesc, "The %s ", r_ptr->name);
  /* For "DIED_FROM" string	   */
  if (CM_WIN & r_ptr->cmove)
    sprintf(out_val2, "The %s", r_ptr->name);
  else if (creature2_is_a_vowel (r_ptr->name[0]))
    sprintf(out_val2, "an %s", r_ptr->name);
  else
    sprintf (out_val2, "a %s", r_ptr->name);
  /* End DIED_FROM		   */

  attackn = 0;
  attstr = r_ptr->damage;
  while ((*attstr != 0) && !death) {
    attype = monster_attacks[*attstr].attack_type;
    adesc = monster_attacks[*attstr].attack_desc;
    adice = monster_attacks[*attstr].attack_dice;
    asides = monster_attacks[*attstr].attack_sides;
    ++attstr;
    flag = FALSE;
    if ((py.flags.protevil > 0) && (r_ptr->cdefense & CD_EVIL) && ((py.misc.lev + 1) > r_ptr->level)) {
	  if (m_ptr->ml) {
	    vbxe_bank(VBXE_GENBANK);
	    c_recall[mon].r_cdefense |= CD_EVIL;
	    vbxe_restore_bank();
      }
	  attype = 99;
	  adesc = 99;
	}

    //p_ptr = &py.misc;
    switch(attype) {
	case 1:	      /*Normal attack  */
	  if (creature2_test_hit(60, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 2:	      /*Lose Strength*/
	  if (creature2_test_hit(-3, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 3:	      /*Confusion attack*/
	  if (creature2_test_hit(10, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 4:	      /*Fear attack    */
	  if (creature2_test_hit(10, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 5:	      /*Fire attack    */
	  if (creature2_test_hit(10, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 6:	      /*Acid attack    */
	  if (creature2_test_hit(0, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 7:	      /*Cold attack    */
	  if (creature2_test_hit(10, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 8:	      /*Lightning attack*/
	  if (creature2_test_hit(10, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 9:	      /*Corrosion attack*/
	  if (creature2_test_hit(0, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 10:      /*Blindness attack*/
	  if (creature2_test_hit(2, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 11:      /*Paralysis attack*/
	  if (creature2_test_hit(2, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 12:      /*Steal Money    */
	  if ((creature2_test_hit(5, r_ptr->level, 0, py.misc.lev, CLA_MISC_HIT)) && (py.misc.au > 0))
	    flag = TRUE;
	  break;
	case 13:      /*Steal Object   */
	  if ((creature2_test_hit(2, r_ptr->level, 0, py.misc.lev, CLA_MISC_HIT)) && (inven_ctr > 0))
	    flag = TRUE;
	  break;
	case 14:      /*Poison	       */
	  if (creature2_test_hit(5, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 15:      /*Lose dexterity*/
	  if (creature2_test_hit(0, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 16:      /*Lose constitution*/
	  if (creature2_test_hit(0, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 17:      /*Lose intelligence*/
	  if (creature2_test_hit(2, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 18:      /*Lose wisdom*/
	  if (creature2_test_hit(2, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 19:      /*Lose experience*/
	  if (creature2_test_hit(5, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 20:      /*Aggravate monsters*/
	  flag = TRUE;
	  break;
	case 21:      /*Disenchant	  */
	  if (creature2_test_hit(20, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 22:      /*Eat food	  */
	  if (creature2_test_hit(5, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 23:      /*Eat light	  */
	  if (creature2_test_hit(5, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 24:      /*Eat charges	  */
	  if ((creature2_test_hit(15, r_ptr->level, 0, py.misc.pac+py.misc.ptoac, CLA_MISC_HIT)) && (inven_ctr > 0))	/* check to make sure an object exists */
	    flag = TRUE;
	  break;
	case 99:
	  flag = TRUE;
	  break;
	default:
	  break;
    }
    if (flag) {
	  /* can not strcat to cdesc because the creature may have multiple attacks */
	  bank_disturb(1, 0, MON3NUM);
	  strcpy(out_val1, cdesc);
	  switch(adesc) {
	    case 1: strcat(out_val1, "hits you."); break;
	    case 2: strcat(out_val1, "bites you."); break;
	    case 3: strcat(out_val1, "claws you."); break;
	    case 4: strcat(out_val1, "stings you."); break;
	    case 5: strcat(out_val1, "touches you."); break;
	    //case 6: msg_print(strcat(out_val1, "kicks you.")); break;				// not implemented in original source -SJ
	    case 7: strcat(out_val1, "gazes at you."); break;
	    case 8: strcat(out_val1, "breathes on you."); break;
	    case 9: strcat(out_val1, "spits on you."); break;
	    case 10: strcat(out_val1,"makes a horrible wail.");break;
	    //case 11: msg_print(strcat(out_val1, "embraces you.")); break;			// not implemented in original source -SJ
	    case 12: strcat(out_val1, "crawls on you."); break;
	    case 13: strcat(out_val1, "releases a cloud of spores."); break;
	    case 14: strcat(out_val1, "begs you for money."); break;
	    //case 15: bank_msg_print("You've been slimed!", MON3NUM); break;
	    case 15: strcpy(out_val1, "You've been slimed!"); break;
	    case 16: strcat(out_val1, "crushes you."); break;
	    case 17: strcat(out_val1, "tramples you."); break;
	    case 18: strcat(out_val1, "drools on you."); break;
	    case 19:
	      switch(randint(9)) {
			case 1: strcat(out_val1, "insults you!"); break;
			case 2: strcat(out_val1, "insults your mother!"); break;
			case 3: strcat(out_val1, "gives you the finger!"); break;
			case 4: strcat(out_val1, "humiliates you!"); break;
			case 5: strcat(out_val1, "wets on your leg!"); break;
			case 6: strcat(out_val1, "defiles you!"); break;
			case 7: strcat(out_val1, "dances around you!");break;
			case 8: strcat(out_val1, "makes obscene gestures!"); break;
			case 9: strcat(out_val1, "moons you!!!"); break;
		  }
	      break;
	    case 99: strcat(out_val1, "is repelled."); break;
	    default: break;
	  }
      bank_msg_print(out_val1, MON3NUM);

	  notice = TRUE;
	  /* always fail to notice attack if creature invisible, set notice
	     and visible here since creature may be visible when attacking
	     and then teleport afterwards (becoming effectively invisible) */

	  vbxe_bank(VBXE_MONBANK);
	  if (!m_ptr->ml) {
	    visible = FALSE;
	    notice = FALSE;
	  }
	  else
	    visible = TRUE;

	  damage = creature2_damroll(adice, asides);
	  switch(attype) {
	    case 1:    /*Normal attack	*/
	      /* round half-way case down */
	      damage -= (int16u)((py.misc.pac+py.misc.ptoac) * damage) / 200;			// FIXME: byte overflow maybe? -SJ

	      //sprintf(dbgstr, "%d", damage);									// FIXME: debugging
	      //bank_prt(dbgstr, 23, 0, MON3NUM);
	      //cgetc();

	      creature2_take_hit(damage, out_val2);
	      break;
	    case 2:    /*Lose Strength*/
	      creature2_take_hit(damage, out_val2);
	      if (py.flags.sustain_str)
		    bank_msg_print("You feel weaker for a moment, but it passes.", MON3NUM);
	      else if (randint(2) == 1) {
		  	bank_msg_print("You feel weaker.", MON3NUM);
		    bank_dec_stat(A_STR, MON3NUM);
		  }
	      else
		    notice = FALSE;
	      break;
	    case 3:    /*Confusion attack*/
	      creature2_take_hit(damage, out_val2);
	      if (randint(2) == 1) {
		    if (py.flags.confused < 1) {
		      bank_msg_print("You feel confused.", MON3NUM);
		      py.flags.confused += randint(r_ptr->level);
		    }
		    else
		      notice = FALSE;
		    py.flags.confused += 3;
		  }
	      else
		    notice = FALSE;
	      break;
	    case 4:    /*Fear attack	*/
	      creature2_take_hit(damage, out_val2);
	      if (bank_player_saves(MON3NUM))
		    bank_msg_print("You resist the effects!", MON3NUM);
	      else if (py.flags.afraid < 1) {
		    bank_msg_print("You are suddenly afraid!", MON3NUM);
		    py.flags.afraid += 3 + randint(r_ptr->level);
		  }
	      else {
		    py.flags.afraid += 3;
		    notice = FALSE;
		  }
	      break;
	    case 5:    /*Fire attack	*/
	      bank_msg_print("You are enveloped in flames!", MON3NUM);
	      bank_fire_dam(damage, out_val2, MON3NUM);
	      break;
	    case 6:    /*Acid attack	*/
	      bank_msg_print("You are covered in acid!", MON3NUM);
	      bank_acid_dam(damage, out_val2, MON3NUM);
	      break;
	    case 7:    /*Cold attack	*/
	      bank_msg_print("You are covered with frost!", MON3NUM);
	      bank_cold_dam(damage, out_val2, MON3NUM);
	      break;
	    case 8:    /*Lightning attack*/
	      bank_msg_print("Lightning strikes you!", MON3NUM);
	      bank_light_dam(damage, out_val2, MON3NUM);
	      break;
	    case 9:    /*Corrosion attack*/
	      bank_msg_print("A stinging red gas swirls about you.", MON3NUM);
	      bank_corrode_gas(out_val2, MON3NUM);
	      creature2_take_hit(damage, out_val2);
	      break;
	    case 10:	/*Blindness attack*/
	      creature2_take_hit(damage, out_val2);
	      if (py.flags.blind < 1) {
		    py.flags.blind += 10 + randint((int)r_ptr->level);
		    bank_msg_print("Your eyes begin to sting.", MON3NUM);
		  }
	      else {
		    py.flags.blind += 5;
		    notice = FALSE;
		  }
	      break;
	    case 11:	/*Paralysis attack*/
	      creature2_take_hit(damage, out_val2);
	      if (bank_player_saves(MON3NUM))
	        bank_msg_print("You resist the effects!", MON3NUM);
	      else if (py.flags.paralysis < 1) {
		    if (py.flags.free_act)
		      bank_msg_print("You are unaffected.", MON3NUM);
		    else {
		      py.flags.paralysis = randint(r_ptr->level) + 3;
		      bank_msg_print("You are paralyzed.", MON3NUM);
		    }
		  }
	      else
		    notice = FALSE;
	      break;
	    case 12:	/*Steal Money	  */
	      if ((py.flags.paralysis < 1) && (randint(124) < py.stats.use_stat[A_DEX]))
		    bank_msg_print("You quickly protect your money pouch!", MON3NUM);
	      else {
		    gold = (py.misc.au/10) + randint(25);
		    if (gold > py.misc.au)
		      py.misc.au = 0;
		    else
		      py.misc.au -= gold;
		    bank_msg_print("Your purse feels lighter.", MON3NUM);
		    bank_prt_gold(MON3NUM);
		  }
	      if (randint(2) == 1) {
		    bank_msg_print("There is a puff of smoke!", MON3NUM);
		    bank_teleport_away(monptr, MAX_SIGHT, MON3NUM);
		  }
	      break;
	    case 13:	/*Steal Object	 */
	      if ((py.flags.paralysis < 1) && (randint(124) < py.stats.use_stat[A_DEX]))
		    bank_msg_print("You grab hold of your backpack!", MON3NUM);
	      else {
		    i = randint(inven_ctr) - 1;
		    bank_inven_destroy(i, MON3NUM);
		    bank_msg_print("Your backpack feels lighter.", MON3NUM);
		  }
	      if (randint(2) == 1) {
		    bank_msg_print("There is a puff of smoke!", MON3NUM);
		    bank_teleport_away(monptr, MAX_SIGHT, MON3NUM);
		  }
	      break;
	    case 14:	/*Poison	 */
	      creature2_take_hit(damage, out_val2);
	      bank_msg_print("You feel very sick.", MON3NUM);
	      py.flags.poisoned += randint(r_ptr->level)+5;
	      break;
	    case 15:	/*Lose dexterity */
	      creature2_take_hit(damage, out_val2);
	      if (py.flags.sustain_dex)
		    bank_msg_print("You feel clumsy for a moment, but it passes.", MON3NUM);
	      else {
		    bank_msg_print("You feel more clumsy.", MON3NUM);
		    bank_dec_stat(A_DEX, MON3NUM);
		  }
	      break;
	    case 16:	/*Lose constitution */
	      creature2_take_hit(damage, out_val2);
	      if (py.flags.sustain_con)
		    bank_msg_print("Your body resists the effects of the disease.", MON3NUM);
	      else {
		    bank_msg_print("Your health is damaged!", MON3NUM);
		    bank_dec_stat(A_CON, MON3NUM);
		  }
	      break;
	    case 17:	/*Lose intelligence */
	      creature2_take_hit(damage, out_val2);
	      bank_msg_print("You have trouble thinking clearly.", MON3NUM);
	      if (py.flags.sustain_int)
		    bank_msg_print("But your mind quickly clears.", MON3NUM);
	      else
		    bank_dec_stat(A_INT, MON3NUM);
	      break;
	    case 18:	/*Lose wisdom	   */
	      creature2_take_hit(damage, out_val2);
	      if (py.flags.sustain_wis)
		    bank_msg_print("Your wisdom is sustained.", MON3NUM);
	      else {
		    bank_msg_print("Your wisdom is drained.", MON3NUM);
		    bank_dec_stat(A_WIS, MON3NUM);
		  }
	      break;
	    case 19:	/*Lose experience  */
	      bank_msg_print("You feel your life draining away!", MON3NUM);
	      bank_lose_exp(damage + (py.misc.exp / 100)*MON_DRAIN_LIFE, MON3NUM);
	      break;
	    case 20:	/*Aggravate monster*/
	      bank_aggravate_monster(20, MON3NUM);
	      break;
	    case 21:	/*Disenchant	   */
	      flag = FALSE;
	      switch(randint(7)) {
		    case 1: i = INVEN_WIELD; break;
		    case 2: i = INVEN_BODY;	 break;
		    case 3: i = INVEN_ARM;	 break;
		    case 4: i = INVEN_OUTER; break;
		    case 5: i = INVEN_HANDS; break;
		    case 6: i = INVEN_HEAD;	 break;
		    case 7: i = INVEN_FEET;	 break;
		  }

	      vbxe_bank(VBXE_OBJBANK);
	      i_ptr = &inventory[i];
	      if (i_ptr->tohit > 0) {
		    i_ptr->tohit -= randint(2);
		    /* don't send it below zero */
		    if (i_ptr->tohit < 0)
		      i_ptr->tohit = 0;
		    flag = TRUE;
		  }
	      if (i_ptr->todam > 0) {
		    i_ptr->todam -= randint(2);
		    /* don't send it below zero */
		    if (i_ptr->todam < 0)
		      i_ptr->todam = 0;
		    flag = TRUE;
		  }
	      if (i_ptr->toac > 0) {
		    i_ptr->toac  -= randint(2);
		    /* don't send it below zero */
		    if (i_ptr->toac < 0)
		      i_ptr->toac = 0;
		    flag = TRUE;
		  }
	      if (flag) {
		    bank_msg_print("There is a static feeling in the air.", MON3NUM);
		    bank_calc_bonuses(MON3NUM);
		  }
	      else
		    notice = FALSE;
	      break;
	    case 22:	/*Eat food	   */
	      if (bank_find_range(TV_FOOD, TV_NEVER, &i, &j, MON3NUM)) {
		    bank_inven_destroy(i, MON3NUM);
		    bank_msg_print ("It got at your rations!", MON3NUM);
		  }
	      else
		    notice = FALSE;
	      break;
	    case 23:	/*Eat light	   */
	      vbxe_bank(VBXE_OBJBANK);
	      i_ptr = &inventory[INVEN_LIGHT];
	      if (i_ptr->p1 > 0) {
		    i_ptr->p1 -= (250 + randint(250));
		    if (i_ptr->p1 < 1)  i_ptr->p1 = 1;
		    if (py.flags.blind < 1)
		      bank_msg_print("Your light dims.", MON3NUM);
		    else
		      notice = FALSE;
		  }
	      else
		    notice = FALSE;
	      break;
	    case 24:	/*Eat charges	  */
	      vbxe_bank(VBXE_OBJBANK);
	      i = randint(inven_ctr) - 1;
	      j = r_ptr->level;
	      i_ptr = &inventory[i];
	      if (((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND)) && (i_ptr->p1 > 0)) {
		    p1 = i_ptr->p1;								// save p1 value -SJ

		    vbxe_bank(VBXE_MONBANK);
		    m_ptr->hp += (int16) j*p1;					// monster gains hp from eating charges
		    vbxe_restore_bank();

		    i_ptr->p1 = 0;								// no charges left
		    //if (!known2_p(i_ptr))
		    if (!(i_ptr->ident & ID_KNOWN2))			// in-line known2_p code	-SJ
		      //add_inscribe(i_ptr, ID_EMPTY);
		      i_ptr->ident |= ID_EMPTY;					// in-lined add_inscribe code -SJ
		    bank_msg_print("Energy drains from your pack!", MON3NUM);
		  }
	      else
		    notice = FALSE;
	      break;
	    case 99:
	    default:
	      notice = FALSE;
	      break;
	  }

	  /* Moved here from mon_move, so that monster only confused if it
	     actually hits.  A monster that has been repelled has not hit
	     the player, so it should not be confused.  */
	  if (py.flags.confuse_monster && adesc != 99) {
	    bank_msg_print("Your hands stop glowing.", MON3NUM);
	    py.flags.confuse_monster = FALSE;
	    if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense))
		  sprintf(out_val1, "%sis unaffected.", cdesc);
	    else {
		  sprintf(out_val1, "%sappears confused.", cdesc);
		  m_ptr->confused = TRUE;
		}
	    bank_msg_print(out_val1, MON3NUM);
	    if (visible && !death && randint(4) == 1) {
		  cdef = r_ptr->cdefense;
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cdefense |= cdef & CD_NO_SLEEP;
		  vbxe_restore_bank();
		}
	  }

	  /* increase number of attacks if notice true, or if visible and had
	     previously noticed the attack (in which case all this does is
	     help player learn damage), note that in the second case do
	     not increase attacks if creature repelled (no damage done) */
	  vbxe_bank(VBXE_GENBANK);
	  if ((notice || (visible && c_recall[mon].r_attacks[attackn] != 0 && attype != 99)) && c_recall[mon].r_attacks[attackn] < MAX_UCHAR)
	    c_recall[mon].r_attacks[attackn]++;
	  if (death && c_recall[mon].r_deaths < MAX_SHORT)
	    c_recall[mon].r_deaths++;
	}
    else {
	  if ((adesc >= 1 && adesc <= 3) || (adesc == 6)) {
	    bank_disturb(1, 0, MON3NUM);
	    strcpy(out_val1, cdesc);
	    bank_msg_print(strcat(out_val1, "misses you."), MON3NUM);
	  }
	}
    if (attackn < MAX_MON_NATTACK-1)
	  ++attackn;
    else
	  break;
  }
}

