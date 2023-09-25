/* UNIX Moria Version 5.x
   source/main.c: initialization, main() function and main loop
   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "interbank.h"
#include "returnbank.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


/* Local Prototypes */
void __fastcall__ get_char_spell_bank(void);
void __fastcall__ get_char_stores(void);
void __fastcall__ get_char_monster_bank(void);
void __fastcall__ save_char_spell_bank(void);
void __fastcall__ save_char_monster_bank(void);



/* Put this code in low memory, always there */
#pragma code-name("BCODE")

void __fastcall__ copy_highscores_to_ram(void)
{
  vbxe_bank(VBXE_GENBANK);
  HIGHSCOREBANK;

  score_entries_ram = score_entries_cart;					// copy number of highscore entries
  memcpy(&scores_ram, &scores_cart, sizeof(scores_cart));	// copy all high scores

  RECALLSAVEBANK;
  memcpy(&c_recall, &save_c_recall, sizeof(c_recall));		// copy monster recall to ram

  vbxe_restore_bank();
  INITBANK;													// only place it's called from
  return;
}


void __fastcall__ write_recall_to_cart(void)
{
  vbxe_bank(VBXE_GENBANK);

  flash_prog_bank(RECALLSAVENUM, &save_c_recall, &c_recall, sizeof(c_recall));		// save monster recall

  vbxe_restore_bank();
  DEATH1BANK;
  return;
}

void __fastcall__ write_highscores_to_cart(void)
{
  vbxe_bank(VBXE_GENBANK);
  flash_erase_bank(HIGHSCORENUM);
  flash_prog_bank(HIGHSCORENUM, &score_entries_cart, &score_entries_ram, sizeof(score_entries_ram)+sizeof(scores_ram));

  vbxe_restore_bank();
  DEATH1BANK;
  return;
}


int8u __fastcall__ get_char(int8u *generate)
{
  CHARSAVE0BANK;

  /* Is Character dead? */
  if (save_death) {
	*generate = TRUE;
	INITBANK;
    return(FALSE);
  }

  /* Get the Character variables from the spell bank, and also run the code
     from that bank as well to save main RAM. -SJ */
  vbxe_bank(VBXE_SPLBANK);										// run from SPELLBANK
  get_char_spell_bank();

  /* Don't have enough memory in object bank to copy this code there. :( -SJ*/
  CHARSAVE0BANK;												// bring back charsave0 (switched to 1 to get stores)
  vbxe_bank(VBXE_OBJBANK);  									// from INVENTORY BANK
  inven_ctr = save_inven_ctr;
  inven_weight = save_inven_weight;
  equip_ctr = save_equip_ctr;
  memcpy(&inventory, &save_inventory, sizeof(inventory));
  memcpy(&object_ident, &save_object_ident, sizeof(object_ident));

  /* Restore Cave */
  GAMESAVE0BANK;
  vbxe_bank(VBXE_CAVBANK);
  memcpy(&cave, &save_cave0, 0x2000);

  GAMESAVE1BANK;
  memcpy((void *)(((unsigned) &cave)+0x2000), &save_cave1, 0x2000);

  GAMESAVE2BANK;
  memcpy((void *)(((unsigned) &cave)+0x4000), &save_cave2, 0x1AC0);

  dun_level = save_dun_level;
  max_panel_rows = save_max_panel_rows;
  max_panel_cols = save_max_panel_cols;
  cur_height = save_cur_height;
  cur_width = save_cur_width;
  char_row = save_char_row;
  char_col = save_char_col;

  /* Restore Treasures */
  GAMESAVE3BANK;
  vbxe_bank(VBXE_OBJBANK);
  tcptr = save_tcptr;
  memcpy(&t_list, &save_t_list, sizeof(t_list));

  /* Restore Monsters */
  vbxe_bank(VBXE_MONBANK);
  GAMESAVE4BANK;
  mfptr = save_mfptr;
  mon_tot_mult = save_mon_tot_mult;
  memcpy(&m_list, &save_m_list, sizeof(m_list));

  *generate = FALSE;
  INITBANK;
  return(TRUE);
}


void __fastcall__ get_char_spell_bank(void)
{
  /* These variables are all in main memory */
  find_cut = save_find_cut;
  find_examine = save_find_examine;
  find_bound = save_find_bound;
  find_prself = save_find_prself;
  prompt_carry_flag = save_prompt_carry_flag;
  show_weight_flag = save_show_weight_flag;
  highlight_seams = save_highlight_seams;
  find_ignore_doors = save_find_ignore_doors;
  display_counts = save_display_counts;
  classic_moria = save_classic_moria;

  death = save_death;
  total_winner = save_total_winner;
  noscore = save_noscore;
  turn = save_turn;
  randes_seed = save_randes_seed;
  town_seed = save_town_seed;
  missile_ctr = save_missile_ctr;
  memcpy(&old_msg, &save_old_msg, sizeof(old_msg));
  last_msg = save_last_msg;
  strcpy(died_from, save_died_from);
  max_score = save_max_score;
  guild_member = save_guild_member;

  memcpy(&py, &save_py, sizeof(player_type));
  memcpy(&quests, &save_quests, sizeof(quests));

  /* These character variables come from SPLBANK, which we are already in. */
  memcpy(&player_hp, &save_player_hp, sizeof(player_hp));

  spell_learned = save_spell_learned;
  spell_worked = save_spell_worked;
  spell_forgotten = save_spell_forgotten;
  memcpy(&spell_order, &save_spell_order, sizeof(spell_order));

  /* Get Stores */
  CHARSAVE1BANK;
  memcpy(&store, &save_store, sizeof(store));
}


#pragma code-name("BCODE")

void __fastcall__ save_char(void)
{
  flash_erase_bank(CHARSAVE0NUM);

  /* Save Character to flashcart */
  vbxe_bank(VBXE_SPLBANK);
  save_char_spell_bank();

  vbxe_bank(VBXE_OBJBANK);  									// from INVENTORY BANK
  flash_prog_bank(CHARSAVE0NUM, &save_inven_ctr, &inven_ctr, sizeof(inven_ctr));
  flash_prog_bank(CHARSAVE0NUM, &save_inven_weight, &inven_weight, sizeof(inven_weight));
  flash_prog_bank(CHARSAVE0NUM, &save_equip_ctr, &equip_ctr, sizeof(equip_ctr));
  flash_prog_bank(CHARSAVE0NUM, &save_inventory, &inventory, sizeof(inventory));
  flash_prog_bank(CHARSAVE0NUM, &save_object_ident, &object_ident, sizeof(object_ident));

  if (death)						// character is dead, don't save cave
    return;

  flash_erase_bank(GAMESAVE0NUM);	// erase game save bank

  /* Save Cave */
  vbxe_bank(VBXE_CAVBANK);
  flash_prog_bank(GAMESAVE0NUM, &save_cave0, &cave, 0x2000);
  flash_prog_bank(GAMESAVE1NUM, &save_cave1, (void *)(((unsigned) &cave)+0x2000), 0x2000);
  flash_prog_bank(GAMESAVE2NUM, &save_cave2, (void *)(((unsigned) &cave)+0x4000), 0x1AC0);

  flash_prog_bank(GAMESAVE2NUM, &save_dun_level, &dun_level, sizeof(dun_level));
  flash_prog_bank(GAMESAVE2NUM, &save_max_panel_rows, &max_panel_rows, sizeof(max_panel_rows));
  flash_prog_bank(GAMESAVE2NUM, &save_max_panel_cols, &max_panel_cols, sizeof(max_panel_cols));
  flash_prog_bank(GAMESAVE2NUM, &save_cur_height, &cur_height, sizeof(cur_height));
  flash_prog_bank(GAMESAVE2NUM, &save_cur_width, &cur_width, sizeof(cur_width));
  flash_prog_bank(GAMESAVE2NUM, &save_char_row, &char_row, sizeof(char_row));
  flash_prog_bank(GAMESAVE2NUM, &save_char_col, &char_col, sizeof(char_col));

  /* Save Treasures */
  vbxe_bank(VBXE_OBJBANK);
  flash_prog_bank(GAMESAVE3NUM, &save_tcptr, &tcptr, sizeof(tcptr));
  flash_prog_bank(GAMESAVE3NUM, &save_t_list, &t_list, sizeof(t_list));

  /* Save Monsters */
  vbxe_bank(VBXE_MONBANK);
  //save_char_monster_bank();
  flash_prog_bank(GAMESAVE4NUM, &save_mfptr, &mfptr, sizeof(mfptr));
  flash_prog_bank(GAMESAVE4NUM, &save_mon_tot_mult, &mon_tot_mult, sizeof(mon_tot_mult));
  flash_prog_bank(GAMESAVE4NUM, &save_m_list, &m_list, sizeof(m_list));

  DUN1BANK;						// Either called from Dungeon1 bank, or main ram, so setting this here is ok
  return;
}


void __fastcall__ save_char_spell_bank(void)
{
  /* These variables are in main memory */
  flash_prog_bank(CHARSAVE0NUM, &save_find_cut, &find_cut, sizeof(find_cut));
  flash_prog_bank(CHARSAVE0NUM, &save_find_examine, &find_examine, sizeof(find_examine));
  flash_prog_bank(CHARSAVE0NUM, &save_find_bound, &find_bound, sizeof(find_bound));
  flash_prog_bank(CHARSAVE0NUM, &save_find_prself, &find_prself, sizeof(find_prself));
  flash_prog_bank(CHARSAVE0NUM, &save_prompt_carry_flag, &prompt_carry_flag, sizeof(prompt_carry_flag));
  flash_prog_bank(CHARSAVE0NUM, &save_show_weight_flag, &show_weight_flag, sizeof(show_weight_flag));
  flash_prog_bank(CHARSAVE0NUM, &save_highlight_seams, &highlight_seams, sizeof(highlight_seams));
  flash_prog_bank(CHARSAVE0NUM, &save_find_ignore_doors, &find_ignore_doors, sizeof(find_ignore_doors));
  flash_prog_bank(CHARSAVE0NUM, &save_display_counts, &display_counts, sizeof(display_counts));
  flash_prog_bank(CHARSAVE0NUM, &save_classic_moria, &classic_moria, sizeof(classic_moria));

  flash_prog_bank(CHARSAVE0NUM, &save_death, &death, sizeof(death));
  flash_prog_bank(CHARSAVE0NUM, &save_total_winner, &total_winner, sizeof(total_winner));
  flash_prog_bank(CHARSAVE0NUM, &save_noscore, &noscore, sizeof(noscore));
  flash_prog_bank(CHARSAVE0NUM, &save_missile_ctr, &missile_ctr, sizeof(missile_ctr));
  flash_prog_bank(CHARSAVE0NUM, &save_last_msg, &last_msg, sizeof(last_msg));
  flash_prog_bank(CHARSAVE0NUM, &save_turn, &turn, sizeof(turn));
  flash_prog_bank(CHARSAVE0NUM, &save_randes_seed, &randes_seed, sizeof(randes_seed));
  flash_prog_bank(CHARSAVE0NUM, &save_town_seed, &town_seed, sizeof(town_seed));
  flash_prog_bank(CHARSAVE0NUM, &save_max_score, &max_score, sizeof(max_score));
  flash_prog_bank(CHARSAVE0NUM, &save_old_msg, &old_msg, sizeof(old_msg));
  flash_prog_bank(CHARSAVE0NUM, &save_died_from, &died_from, sizeof(died_from));
  flash_prog_bank(CHARSAVE0NUM, &save_guild_member, &guild_member, sizeof(guild_member));

  flash_prog_bank(CHARSAVE0NUM, &save_py, &py, sizeof(player_type));
  flash_prog_bank(CHARSAVE0NUM, &save_quests, &quests, sizeof(quests));

  /* These variables are in SPLBANK, which we are already in. -SJ */
  flash_prog_bank(CHARSAVE0NUM, &save_player_hp, &player_hp, sizeof(player_hp));
  flash_prog_bank(CHARSAVE0NUM, &save_spell_learned, &spell_learned, sizeof(spell_learned));
  flash_prog_bank(CHARSAVE0NUM, &save_spell_worked, &spell_worked, sizeof(spell_worked));
  flash_prog_bank(CHARSAVE0NUM, &save_spell_forgotten, &spell_forgotten, sizeof(spell_forgotten));
  flash_prog_bank(CHARSAVE0NUM, &save_spell_order, &spell_order, sizeof(spell_order));

  /* Save stores in next flash bank */
  flash_prog_bank(CHARSAVE1NUM, &save_store, &store, sizeof(store));
}
