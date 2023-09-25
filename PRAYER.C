/* source/prayer.c: code for priest spells

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("PRAYERBANK")
#pragma data-name("PRAYERBANK")
#pragma rodata-name("PRAYERBANK")


/* Local Prototypes */
void __fastcall__ prayer_erase_line(int8u row, int8u col);
int8u __fastcall__ prayer_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8u __fastcall__ prayer_get_dir(char *prompt, int8u *dir);
char __fastcall__ prayer_map_roguedir(char comval);
int8u __fastcall__ prayer_no_light(void);
int8u __fastcall__ prayer_hp_player(int16 num);
int8u __fastcall__ prayer_remove_fear(void);
int8u __fastcall__ prayer_cure_poison(void);
int8u __fastcall__ prayer_damroll(int8u num, int8u sides);
int8u __fastcall__ prayer_slow_poison(void);
int8 __fastcall__ prayer_bit_pos(int32u *test);
int8u __fastcall__ prayer_get_check(char *prompt);
int8u __fastcall__ prayer_spell_chance(int8u spell);
int8u __fastcall__ prayer_stat_adj(int8u stat);
int8u __fastcall__ prayer_get_spell(int8u *spell, int8u num, int8 *sn, int8u *sc, char *prompt, int8u first_spell);
int8 __fastcall__ prayer_cast_spell(char *prompt, int8u item_val, int8 *sn, int8u *sc);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		PRAYERNUM

#undef	INBANK_MOD
#define INBANK_MOD			prayer


void __fastcall__ prayer_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

/* Finds range of item in inventory list		-RAK-	*/
int8u __fastcall__ prayer_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

/* map rogue_like direction commands into numbers */
char __fastcall__ prayer_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

/* Prompts for a direction				-RAK-	*/
/* Direction memory added, for repeated commands.  -CJS */
int8u __fastcall__ prayer_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

/* Returns true if player has no light			-RAK-	*/
int8u __fastcall__ prayer_no_light(void)
#include "inbank\no_light.c"

/* Change players hit points in some manner		-RAK-	*/
int8u __fastcall__ prayer_hp_player(int16 num)
#include "inbank\hp_player.c"

/* Cure the players fear				-RAK-	*/
/* No need to return anything in this bank -SJ 	*/
int8u __fastcall__ prayer_remove_fear(void)
#include "inbank\remove_fear.c"

/* Cure poisoning					-RAK-	*/
int8u __fastcall__ prayer_cure_poison(void)
#include "inbank\cure_poison.c"

/* generates damage for 2d6 style dice rolls */
int8u __fastcall__ prayer_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

/* Slow Poison	-RAK-	*/
int8u __fastcall__ prayer_slow_poison(void)
#include "inbank\slow_poison.c"

/* Returns position of first set bit			-RAK-	*/
/*     and clears that bit */
int8 __fastcall__ prayer_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

/* Adjustment for wisdom/intelligence				-JWT-	*/
int8u __fastcall__ prayer_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

/* Used to verify a choice - user gets the chance to abort choice.  -CJS- */
int8u __fastcall__ prayer_get_check(char *prompt)
#include "inbank\get_check.c"

/* Returns spell chance of failure for spell		-RAK-	*/
int8u __fastcall__ prayer_spell_chance(int8u spell)
#include "inbank\spell_chance.c"

/* Returns spell pointer				-RAK-	*/
int8u __fastcall__ prayer_get_spell(int8u *spell, int8u num, int8 *sn, int8u *sc, char *prompt, int8u first_spell)
#include "inbank\get_spell.c"

/* Return spell number and failure chance		-RAK-	*/
/* returns -1 if no spells in book
   returns 1 if choose a spell in book to cast
   returns 0 if don't choose a spell, i.e. exit with an escape */
int8 __fastcall__ prayer_cast_spell(char *prompt, int8u item_val, int8 *sn, int8u *sc)
#include "inbank\cast_spell.c"


/* Pray like HELL.					-RAK-	*/
void __fastcall__ pray(void)
{
  int8u i, j, item_val, dir;
  int8u chance;
  int8 result, choice;
  register spell_type *s_ptr;
  register inven_type *i_ptr;
  char spellname[20];


  free_turn_flag = TRUE;
  vbxe_bank(VBXE_SPLBANK);

  if (py.flags.blind > 0)
    bank_msg_print("You can't see to read your prayer!", PRAYERNUM);
  else if (prayer_no_light())
    bank_msg_print("You have no light to read by.", PRAYERNUM);
  else if (py.flags.confused > 0)
    bank_msg_print("You are too confused.", PRAYERNUM);
  else if (class[py.misc.pclass].spell != PRIEST)
    bank_msg_print("Pray hard enough and your prayers may be answered.", PRAYERNUM);
  else if (inven_ctr == 0)
    bank_msg_print("But you are not carrying anything!", PRAYERNUM);
  else if (!prayer_find_range(TV_PRAYER_BOOK, TV_NEVER, &i, &j))
    bank_msg_print("You are not carrying any Holy Books!", PRAYERNUM);
  else if (bank_get_item(&item_val, "Use which Holy Book?", i, j, CNIL, "", PRAYERNUM)) {
    result = prayer_cast_spell("Recite which prayer?", item_val, &choice, &chance);
    if (result < 0)
	  bank_msg_print("You don't know any prayers in that book.", PRAYERNUM);
    else if (result > 0) {
	  vbxe_bank(VBXE_SPLBANK);
	  s_ptr = &magic_spell[py.misc.pclass-1][choice];
	  free_turn_flag = FALSE;

	  if (randint(100) < chance)
	    bank_msg_print("You lost your concentration!", PRAYERNUM);
	  else {
	    /* Prayers.	*/
	    switch(choice+1) {
		  case 1:
		    bank_detect_evil(PRAYERNUM);
		    break;
		  case 2:
		    prayer_hp_player(prayer_damroll(3, 3));
		    break;
  		  case 3:
		    //bless(randint(12)+12);
		    py.flags.blessed += randint(12)+12;					// in-line bless code -SJ
		    break;
		  case 4:
		    prayer_remove_fear();
		    break;
		  case 5:
		    bank_light_area(char_row, char_col, PRAYERNUM);
		    break;
		  case 6:
		    bank_detect_trap(PRAYERNUM);
		    break;
		  case 7:
		    bank_detect_sdoor(PRAYERNUM);
		    break;
		  case 8:
		    prayer_slow_poison();
		    break;
		  case 9:
		    if (prayer_get_dir(CNIL, &dir))
		      bank_confuse_monster(dir, char_row, char_col, PRAYERNUM);
		    break;
		  case 10:
		    bank_teleport(py.misc.lev*3, PRAYERNUM);
		    break;
		  case 11:
		    prayer_hp_player(prayer_damroll(4, 4));
		    break;
		  case 12:
		    //bless(randint(24)+24);
		    py.flags.blessed += randint(24)+24;					// in-line bless code -SJ
		    break;
		  case 13:
		    bank_sleep_monsters1(char_row, char_col, PRAYERNUM);
		    break;
		  case 14:
		    bank_create_food(PRAYERNUM);
		    break;
		  case 15:
		    vbxe_bank(VBXE_OBJBANK);
		    for (i = 0; i < INVEN_ARRAY_SIZE; ++i) {
		      i_ptr = &inventory[i];
		      /* only clear flag for items that are wielded or worn */
		      if (i_ptr->tval >= TV_MIN_WEAR && i_ptr->tval <= TV_MAX_WEAR)
			    i_ptr->flags &= ~TR_CURSED;
		    }
		    break;
		  case 16:
		    py.flags.resist_heat += randint(10) + 10;
		    py.flags.resist_cold += randint(10) + 10;
		    break;
		  case 17:
		    prayer_cure_poison();
		    break;
		  case 18:
		    if (prayer_get_dir(CNIL, &dir)) {
			  strcpy(spellname, "Black Sphere");
		      bank_fire_ball(GF_HOLY_ORB, dir, char_row, char_col, prayer_damroll(3, 6)+py.misc.lev, spellname, PRAYERNUM);
			}
		    break;
		  case 19:
		    prayer_hp_player(prayer_damroll(8, 4));
		    break;
		  case 20:
		    //detect_inv2(randint(24)+24);
		    py.flags.detect_inv += randint(24)+24;					// in-line detect_inv2 code -SJ
		    break;
		  case 21:
		    bank_protect_evil(PRAYERNUM);
		    break;
		  case 22:
		    bank_earthquake(PRAYERNUM);
		    break;
		  case 23:
		    bank_map_area(PRAYERNUM);
		    break;
		  case 24:
		    prayer_hp_player(prayer_damroll(16, 4));
		    break;
		  case 25:
		    bank_turn_undead();
		    break;
		  case 26:
		    //bless(randint(48)+48);
		    py.flags.blessed += randint(48)+48;					// in-line bless code -SJ
		    break;
		  case 27:
		    bank_dispel_creature(CD_UNDEAD, (3*py.misc.lev), PRAYERNUM);
		    break;
		  case 28:
		    prayer_hp_player(200);
		    break;
		  case 29:
		    bank_dispel_creature(CD_EVIL, (3*py.misc.lev), PRAYERNUM);
		    break;
		  case 30:
		    bank_warding_glyph(PRAYERNUM);
		    break;
		  case 31:
		    bank_dispel_creature(CD_EVIL, (4*py.misc.lev), PRAYERNUM);
		    prayer_remove_fear();
		    prayer_cure_poison();
		    prayer_hp_player(1000);
		    break;
		  default:
		    break;
		}
	    /* End of prayers. */

	    if (!free_turn_flag) {
	  	  if ((spell_worked & (1L << choice)) == 0) {
			vbxe_bank(VBXE_SPLBANK);
		    py.misc.exp += s_ptr->sexp << 2;
		    bank_prt_experience(PRAYERNUM);
		    spell_worked |= (1L << choice);
		  }
		}
	  }

      vbxe_bank(VBXE_SPLBANK);
	  if (!free_turn_flag) {
	    if (s_ptr->smana > py.misc.cmana) {
		  bank_msg_print("You faint from fatigue!", PRAYERNUM);
		  py.flags.paralysis = randint((5 * (s_ptr->smana - py.misc.cmana)));
		  py.misc.cmana = 0;
		  py.misc.cmana_frac = 0;
		  if (randint(3) == 1) {
		    bank_msg_print("You have damaged your health!", PRAYERNUM);
		    bank_dec_stat(A_CON, PRAYERNUM);
		  }
		}
	    else
		  py.misc.cmana -= s_ptr->smana;

	    bank_prt_cmana(PRAYERNUM);
	  }
	}
  }
}
