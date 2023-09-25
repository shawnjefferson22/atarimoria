/* source/magic.c: code for mage spells

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


#pragma code-name("CASTBANK")
#pragma data-name("CASTBANK")
#pragma rodata-name("CASTBANK")



/* Local Prototypes */
void __fastcall__ cast_erase_line(int8u row, int8u col);
int8u __fastcall__ cast_find_range(int8u item1, int8u item2, int8u *j, int8u *k);
int8u __fastcall__ cast_get_dir(char *prompt, int8u *dir);
char __fastcall__ cast_map_roguedir(char comval);
int8u __fastcall__ cast_no_light(void);
int8u __fastcall__ cast_hp_player(int16 num);
int8u __fastcall__ cast_cure_poison(void);
int8u __fastcall__ cast_damroll(int8u num, int8u sides);
int8 __fastcall__ cast_bit_pos(int32u *test);
int8u __fastcall__ cast_get_check(char *prompt);
int8u __fastcall__ cast_spell_chance(int8u spell);
int8u __fastcall__ cast_stat_adj(int8u stat);
int8u __fastcall__ cast_get_spell(int8u *spell, int8u num, int8 *sn, int8u *sc, char *prompt, int8u first_spell);
int8 __fastcall__ cast_cast_spell(char *prompt, int8u item_val, int8 *sn, int8u *sc);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		CASTNUM

#undef	INBANK_MOD
#define INBANK_MOD			cast


void __fastcall__ cast_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

int8u __fastcall__ cast_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

char __fastcall__ cast_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

int8u __fastcall__ cast_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

int8u __fastcall__ cast_no_light(void)
#include "inbank\no_light.c"

int8u __fastcall__ cast_hp_player(int16 num)
#include "inbank\hp_player.c"

int8u __fastcall__ cast_cure_poison(void)
#include "inbank\cure_poison.c"

int8u __fastcall__ cast_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8 __fastcall__ cast_bit_pos(int32u *test)
#include "inbank\bit_pos.c"

int8u __fastcall__ cast_stat_adj(int8u stat)
#include "inbank\stat_adj.c"

int8u __fastcall__ cast_get_check(char *prompt)
#include "inbank\get_check.c"

int8u __fastcall__ cast_spell_chance(int8u spell)
#include "inbank\spell_chance.c"

int8u __fastcall__ cast_get_spell(int8u *spell, int8u num, int8 *sn, int8u *sc, char *prompt, int8u first_spell)
#include "inbank\get_spell.c"

int8 __fastcall__ cast_cast_spell(char *prompt, int8u item_val, int8 *sn, int8u *sc)
#include "inbank\cast_spell.c"


/* Throw a magic spell					-RAK-	*/
void __fastcall__ cast(void)
{
  int8u i, j, item_val, dir;
  int8u chance;
  int8 result, choice;
  register inven_type *i_ptr;
  register spell_type *m_ptr;
  char spellname[25];


  free_turn_flag = TRUE;
  vbxe_bank(VBXE_SPLBANK);
  if (py.flags.blind > 0)
    bank_msg_print("You can't see to read your spell book!", CASTNUM);
  else if (cast_no_light())
    bank_msg_print("You have no light to read by.", CASTNUM);
  else if (py.flags.confused > 0)
    bank_msg_print("You are too confused.", CASTNUM);
  else if (class[py.misc.pclass].spell != MAGE)
    bank_msg_print("You can't cast spells!", CASTNUM);
  else if (!cast_find_range(TV_MAGIC_BOOK, TV_NEVER, &i, &j))
    bank_msg_print("But you are not carrying any spell-books!", CASTNUM);
  else if (bank_get_item(&item_val, "Use which spell-book?", i, j, CNIL, "", CASTNUM)) {
    result = cast_cast_spell("Cast which spell?", item_val, &choice, &chance);
    if (result < 0)
	  bank_msg_print("You don't know any spells in that book.", CASTNUM);
    else if (result > 0) {
	  vbxe_bank(VBXE_SPLBANK);
	  m_ptr = &magic_spell[py.misc.pclass-1][choice];
	  free_turn_flag = FALSE;

	  if (randint(100) < chance)
	    bank_msg_print("You failed to get the spell off!", CASTNUM);
	  else {
	    /* Spells.  */
	    switch(choice+1) {
		  case 1:
		    if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[0]);
		      bank_fire_bolt(GF_MAGIC_MISSILE, dir, char_row, char_col, cast_damroll(2, 6), spellname, CASTNUM);
			}
		    break;
		  case 2:
		    bank_detect_monsters();
		    break;
		  case 3:
		    bank_teleport(10, CASTNUM);
		    break;
		  case 4:
		    bank_light_area(char_row, char_col, CASTNUM);
		    break;
		  case 5:
		    cast_hp_player(cast_damroll(4, 4));
		    break;
		  case 6:
		    bank_detect_sdoor(CASTNUM);
		    bank_detect_trap(CASTNUM);
		    break;
		  case 7:
		    if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[6]);
		      bank_fire_ball(GF_POISON_GAS, dir, char_row, char_col, 12, spellname, CASTNUM);
			}
		    break;
		  case 8:
		    if (cast_get_dir(CNIL, &dir))
		      bank_confuse_monster(dir, char_row, char_col, CASTNUM);
		    break;
		  case 9:
		    if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[8]);
		      bank_fire_bolt(GF_LIGHTNING, dir, char_row, char_col, cast_damroll(4, 8), spellname, CASTNUM);
			}
		    break;
		  case 10:
		    bank_td_destroy(CASTNUM);
		    break;
		  case 11:
		    if (cast_get_dir(CNIL, &dir))
		      bank_sleep_monster(dir, char_row, char_col, CASTNUM);
		    break;
		  case 12:
		    cast_cure_poison();
		    break;
		  case 13:
		    bank_teleport(py.misc.lev*5, CASTNUM);
		    break;
		  case 14:
		    vbxe_bank(VBXE_OBJBANK);
		    for (i = 22; i < INVEN_ARRAY_SIZE; ++i) {
		      i_ptr = &inventory[i];
		      i_ptr->flags = (i_ptr->flags & ~TR_CURSED);
		    }
		    break;
		  case 15:
  	        if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[14]);
		      bank_fire_bolt(GF_FROST, dir, char_row, char_col, cast_damroll(6, 8), spellname, CASTNUM);
	   		}
		    break;
		  case 16:
		    if (cast_get_dir(CNIL, &dir))
		      bank_wall_to_mud(dir, char_row, char_col, CASTNUM);
		      break;
		  case 17:
		    bank_create_food(CASTNUM);
		    break;
		  case 18:
		    bank_recharge(20, CASTNUM);
		    break;
		  case 19:
		    bank_sleep_monsters1(char_row, char_col, CASTNUM);
		    break;
		  case 20:
		    if (cast_get_dir(CNIL, &dir))
		      bank_poly_monster(dir, char_row, char_col, CASTNUM);
		    break;
		  case 21:
		    bank_ident_spell(CASTNUM);
		    break;
		  case 22:
		    bank_sleep_monsters2(CASTNUM);
		    break;
		  case 23:
		    if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[22]);
		      bank_fire_bolt(GF_FIRE, dir, char_row, char_col, cast_damroll(9, 8), spellname, CASTNUM);
			}
		    break;
		  case 24:
		    if (cast_get_dir(CNIL, &dir))
		    bank_speed_monster(dir, char_row, char_col, -1, CASTNUM);
		    break;
		  case 25:
		    if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[24]);
		      bank_fire_ball(GF_FROST, dir, char_row, char_col, 48, spellname, CASTNUM);
			}
		    break;
		  case 26:
		    bank_recharge(60, CASTNUM);
		    break;
		  case 27:
		    if (cast_get_dir(CNIL, &dir))
		      bank_teleport_monster(dir, char_row, char_col, CASTNUM);
		    break;
		  case 28:
		    py.flags.fast += randint(20) + py.misc.lev;
		    break;
		  case 29:
		    if (cast_get_dir(CNIL, &dir)) {
			  vbxe_bank(VBXE_SPLBANK);
			  strcpy(spellname, spell_names[28]);
		      bank_fire_ball(GF_FIRE, dir, char_row, char_col, 72, spellname, CASTNUM);
			}
		    break;
		  case 30:
		    bank_destroy_area(char_row, char_col, CASTNUM);
		    break;
		  case 31:
		    bank_genocide(CASTNUM);
		    break;
		  default:
		    break;
		}
	    /* End of spells. */


	    if (!free_turn_flag) {
		  if ((spell_worked & (1L << choice)) == 0) {
    	    vbxe_bank(VBXE_SPLBANK);
		    py.misc.exp += m_ptr->sexp << 2;
		    spell_worked |= (1L << choice);
		    bank_prt_experience(CASTNUM);
		  }
		}
	  }

	  if (!free_turn_flag) {
		vbxe_bank(VBXE_SPLBANK);
	    if (m_ptr->smana > py.misc.cmana) {
		  bank_msg_print("You faint from the effort!", CASTNUM);
		  py.flags.paralysis = randint((5*(m_ptr->smana-py.misc.cmana)));
		  py.misc.cmana = 0;
		  py.misc.cmana_frac = 0;
		  if (randint(3) == 1) {
		    bank_msg_print("You have damaged your health!", CASTNUM);
		    bank_dec_stat(A_CON, CASTNUM);
		  }
		}
	    else
		  py.misc.cmana -= m_ptr->smana;
	    bank_prt_cmana(CASTNUM);
	  }
	}
  }
}
