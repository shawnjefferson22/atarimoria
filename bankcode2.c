#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <string.h>
#include "interbank.h"
#include "returnbank.h"
#include "types.h"
#include "vbxetext.h"
#include "externs.h"


/* Banking Code, in Cart Bank 7C and copied to ram under OS */
#pragma code-name("BCODE2")



/* Always called from the same bank */
void __fastcall__ bank_create_character(void)
{
  CREATE1BANK;
  create_character();
  INITBANK;
  return;
}


void __fastcall__ bank_put_character(void)
{
  CHPRINT1BANK;
  put_character();
  CREATE1BANK;
  return;
}


void __fastcall__ bank_get_all_stats(void)
{
  CREATE2BANK;
  get_all_stats();
  CREATE1BANK;
  return;
}


void __fastcall__ bank_read_scroll(void)
{
  SCROLLBANK;
  read_scroll();
  DUN1BANK;

  return;
}


int8u __fastcall__ bank_summon_undead(int8u *y, int8u *x, int8u bank)
{
  int8u ret;

  MON4BANK;
  ret = summon_undead(y, x);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_aim(void)
{
  WANDBANK;
  aim();
  DUN1BANK;

  return;
}


int8u __fastcall__ bank_ident_spell(int8u bank)
{
  int8u ret;

  SPELLS1BANK;
  ret = ident_spell();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_sleep_monsters1(int8u y, int8u x, int8u bank)
{
  int8u ret;

  SPELLS1BANK;
  ret = sleep_monsters1(y, x);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_detect_treasure(int8u bank)
{
  int8u ret;

  SPELLS1BANK;
  ret = detect_treasure();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_detect_object(int8u bank)
{
  int8u ret;

  SPELLS1BANK;
  ret = detect_object();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_detect_trap(int8u bank)
{
  int8u ret;

  SPELLS1BANK;
  ret = detect_trap();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_detect_sdoor(int8u bank)
{
  int8u ret;

  SPELLS1BANK;
  ret = detect_sdoor();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_remove_curse(int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = remove_curse();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_light_area(int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = light_area(y, x);

  return_bank(bank);
  return(ret);
}


int8u 	__fastcall__ bank_unlight_area(int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = unlight_area(y, x);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_map_area(int8u bank)
{
  SPELLS1BANK;
  map_area();

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_detect_invisible(int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = detect_invisible();

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_warding_glyph(int8u bank)
{
  SPELLS1BANK;
  warding_glyph();

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_protect_evil(int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = protect_evil();

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_create_food(int8u bank)
{
  SPELLS1BANK;
  create_food();

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_td_destroy(int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = td_destroy();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_recharge(int8u num, int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = recharge(num);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_mass_genocide(int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = mass_genocide();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_genocide(int8u bank)
{
  int8u ret;


  SPELLS1BANK;
  ret = genocide();

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_destroy_area(int8u y, int8u x, int8u bank)
{
  SPELLS3BANK;
  destroy_area(y, x);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_dispel_creature(int8u cflag, int8u damage, int8u bank)
{
  int8u ret;


  SPELLS3BANK;
  ret = dispel_creature(cflag, damage);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_enter_wiz_mode(void)
{
  int8u ret;


  WIZ1BANK;
  ret = enter_wiz_mode();
  DUN1BANK;

  return(ret);
}


void __fastcall__ bank_wizard_commands(char com_val)
{
  WIZ1BANK;
  wizard_commands(com_val);
  DUN1BANK;

  return;
}


void __fastcall__ bank_display_wizard_help(void)
{
  HELP1BANK;
  display_wizard_help();
  WIZ1BANK;

  return;
}


void __fastcall__ bank_wizard_display_player(void)
{
  WIZ2BANK;
  wizard_display_player();
  WIZ1BANK;

  return;
}


void __fastcall__ bank_light_room(int8u y, int8u x, int8u bank)
{
  DUN3BANK;
  light_room(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_light_line(int8u dir, int8u y, int8u x, int8u bank)
{
  SPELLS2BANK;
  light_line(dir, y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_fire_bolt(int8u typ, int8u dir, int8u y, int8u x, int8u dam, char *bolt_typ, int8u bank)
{
  SPELLS2BANK;
  fire_bolt(typ, dir, y, x, dam, bolt_typ);

  return_bank(bank);
  return;
}


void __fastcall__ bank_fire_ball(int8u typ, int8u dir, int8u y, int8u x, int8u dam_hp, char *descrip, int8u bank)
{
  SPELLS2BANK;
  fire_ball(typ, dir, y, x, dam_hp, descrip);

  return_bank(bank);
  return;
}


void __fastcall__ bank_breath(int8u typ, int8u y, int8u x, int8u dam_hp, char *ddesc, int8u monptr, int8u bank)
{
  SPELLS2BANK;
  breath(typ, y, x, dam_hp, ddesc, monptr);

  return_bank(bank);
  return;
}


int32u __fastcall__ bank_monster_death(int8u y, int8u x, int32u flags, int8u bank)
{
  int32u ret;


  MON5BANK;
  ret = monster_death(y, x, flags);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_teleport_monster(int8u dir, int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS4BANK;
  ret = teleport_monster(dir, y, x);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_starlite(int8u y, int8u x)
{
  SPELLS2BANK;
  starlite(y, x);
  STAFFSBANK;

  return;
}


int8u __fastcall__ bank_disarm_all(int8u dir, int8u y, int8u x)
{
  int8u ret;


  SPELLS3BANK;
  ret = disarm_all(dir, y, x);
  WANDBANK;

  return(ret);
}


int8u __fastcall__ bank_hp_monster(int8u dir, int8u y, int8u x, int8 dam, int8u bank)
{
  int8u ret;


  SPELLS3BANK;
  ret = hp_monster(dir, y, x, dam);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_drain_life(int8u dir, int8u y, int8u x)
{
  int8u ret;


  SPELLS3BANK;
  ret = drain_life(dir, y, x);
  WANDBANK;

  return(ret);
}


int8u __fastcall__ bank_speed_monster(int8u dir, int8u y, int8u x, int8 spd, int8u bank)
{
  int8u ret;


  SPELLS3BANK;
  ret = speed_monster(dir, y, x, spd);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_confuse_monster(int8u dir, int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS3BANK;
  ret = confuse_monster(dir, y, x);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_sleep_monster(int8u dir, int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS3BANK;
  ret = sleep_monster(dir, y, x);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_td_destroy2(int8u dir, int8u y, int8u x)
{
  int8u ret;


  SPELLS3BANK;
  ret = td_destroy2(dir, y, x);
  WANDBANK;

  return(ret);
}


int8u __fastcall__ bank_place_monster(int8u y, int8u x, int16u z, int8u slp, int8u bank)
{
  int8u ret;


  MON1BANK;
  ret = mon1_place_monster(y, x, z, slp);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_poly_monster(int8u dir, int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS3BANK;
  ret = poly_monster(dir, y, x);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_clone_monster(int8u dir, int8u y, int8u x)
{
  int8u ret;


  SPELLS3BANK;
  ret = clone_monster(dir, y, x);
  WANDBANK;

  return(ret);
}


int8u __fastcall__ bank_detect_evil(int8u bank)
{
  int8u ret;


  SPELLS4BANK;
  ret = detect_evil();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_twall(int8u y, int8u x, int8u t1, int8u t2, int8u bank)
{
  int8u ret;


  DUN1BANK;
  ret = twall(y, x, t1, t2);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_wall_to_mud(int8u dir, int8u y, int8u x, int8u bank)
{
  int8u ret;


  SPELLS4BANK;
  ret = wall_to_mud(dir, y, x);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_build_wall(int8u dir, int8u y, int8u x)
{
  int8u ret;


  SPELLS4BANK;
  ret = build_wall(dir, y, x);
  WANDBANK;

  return(ret);
}


void __fastcall__ bank_use(void)
{
  STAFFSBANK;
  use();
  DUN1BANK;

  return;
}


int8u __fastcall__ bank_speed_monsters(int8u spd)
{
  int8u ret;


  SPELLS4BANK;
  ret = speed_monsters(spd);
  STAFFSBANK;

  return(ret);
}


int8u __fastcall__ bank_sleep_monsters2(int8u bank)
{
  int8u ret;


  SPELLS4BANK;
  ret = sleep_monsters2();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_mass_poly(void)
{
  int8u ret;


  SPELLS4BANK;
  ret = mass_poly();
  STAFFSBANK;

  return(ret);
}


void __fastcall__ bank_earthquake(int8u bank)
{
  SPELLS4BANK;
  earthquake();

  return_bank(bank);
  return;
}


void __fastcall__ bank_turn_undead(void)
{
  SPELLS4BANK;
  turn_undead();
  PRAYERBANK;

  return;
}


void __fastcall__ bank_pray(void)
{
  PRAYERBANK;
  pray();
  DUN1BANK;

  return;
}


void __fastcall__ bank_cast(void)
{
  CASTBANK;
  cast();
  DUN1BANK;

  return;
}


int8u __fastcall__ bank_detect_monsters(void)
{
  int8u ret;


  SPELLS4BANK;
  ret = detect_monsters();
  CASTBANK;

  return(ret);
}


void __fastcall__ bank_look(void)
{
  LOOKBANK;
  look();
  DUN1BANK;

  return;
}


void __fastcall__ bank_print_symbol_highlight(int8u row, int8u col, int8u bank)
{
  PRINTBANK;
  print_symbol_highlight(row, col);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_bool_roff_recall(int16u mon_num)
{
  int8u ret;


  RECALLBANK;
  ret = bool_roff_recall(mon_num);
  HELP1BANK;

  return(ret);
}


char __fastcall__ bank_roff_recall(int16u mon_num, int8u bank)
{
  char ret;


  RECALLBANK;
  ret = roff_recall(mon_num);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_find_range(int8u item1, int8u item2, int8u *j, int8u *k, int8u bank)
{
  int8u ret;


  POTIONBANK;
  ret = potion1_find_range(item1, item2, j, k);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_get_moves(int8u monptr, int8u *mm)
{
  MON4BANK;
  get_moves(monptr, mm);
  MON2BANK;

  return;
}


void __fastcall__ bank_enter_guild(void)
{
  GUILD1BANK;
  enter_guild();
  DUN5BANK;

  return;
}


void __fastcall__ bank_enter_tavern(void)
{
  TAVERN1BANK;
  enter_tavern();
  DUN5BANK;

  return;
}


int8u __fastcall__ bank_tavern_gossip(int8u chance, int8u barkeep)
{
  int8u ret;


  TAVERN2BANK;
  ret = tavern_gossip(chance, barkeep);
  TAVERN1BANK;

  return(ret);
}


void __fastcall__ bank_tavern_init(void)
{
  TAVERN1BANK;
  tavern_init();
  GEN2BANK;
}


void __fastcall__ bank_pretty_print(char *text, int8u line, int8u bank)
{
  PRINTBANK;
  pretty_print(text, line);

  return_bank(bank);
  return;
}


int16u __fastcall__ bank_get_mons_num(int8u level, int8u bank)
{
  int16u ret;


  MON1BANK;
  ret = get_mons_num(level);

  return_bank(bank);
  return(ret);
}


int16u  __fastcall__ bank_get_obj_num(int8u level, int8u bank)
{
  int16u ret;


  PLACE1BANK;
  ret = get_obj_num(level);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_quest_start(int8u chance)
{
  int8u ret;


  QUEST1BANK;
  ret = quest_start(chance);
  TAVERN1BANK;

  return(ret);
}


void __fastcall__ bank_quest_end(void)
{
  QUEST1BANK;
  quest_end();
  TAVERN1BANK;

  return;
}


void __fastcall__ bank_show_quests(int8u bank)
{
  QUEST2BANK;
  show_quests();

  return_bank(bank);
  return;
}


void __fastcall__ bank_eval_quest_mon(int16u mon, int8u bank)
{
  QUEST2BANK;
  eval_quest_mon(mon);

  return_bank(bank);
  return;
}


void __fastcall__ bank_eval_quest_level(int8u level, int8u bank)
{
  QUEST2BANK;
  eval_quest_level(level);

  return_bank(bank);
  return;
}


void __fastcall__ bank_eval_quest_item(int8u bank)
{
  QUEST2BANK;
  eval_quest_item();

  return_bank(bank);
  return;
}


void __fastcall__ bank_compact_quests(void)
{
  QUEST2BANK;
  compact_quests();
  QUEST1BANK;

  return;
}


