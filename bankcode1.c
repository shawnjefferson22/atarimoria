#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <string.h>
#include "interbank.h"
#include "returnbank.h"
#include "types.h"
#include "vbxetext.h"
#include "externs.h"


/* Banking Code, in Cart Bank 7D and copied to ram under OS */
#pragma code-name("BCODE1")



int8u __fastcall__ bank_get_com(char *prompt, char *command, int8u bank)
{
  int8u ret;
  char p[80];


  strcpy(p, prompt);
  PRINTBANK;
  ret = get_com(p, command);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_print_symbol(int8u row, int8u col, int8u bank)
{
  PRINTBANK;
  print_symbol(row, col);

  return_bank(bank);
  return;
}


void __fastcall__ bank_print_self(int8u row, int8u col, int8u bank)
{
  PRINTBANK;
  print_self(row, col);

  return_bank(bank);
  return;
}


void __fastcall__ bank_put_misc1(void)
{
  CHPRINT1BANK;
  put_misc1();
  CREATE1BANK;

  return;
}


void __fastcall__ bank_put_stats(void)
{
  CHPRINT1BANK;
  put_stats();
  CREATE1BANK;

  return;
}


void __fastcall__ bank_prt_depth(int8u bank)
{
  CHPRINT1BANK;
  prt_depth();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_title(int8u bank)
{
  CHPRINT1BANK;
  prt_title();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_level(int8u bank)
{
  CHPRINT1BANK;
  prt_level();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_cmana(int8u bank)
{
  CHPRINT1BANK;
  prt_cmana();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_mhp(int8u bank)
{
  CHPRINT1BANK;
  prt_mhp();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_chp(int8u bank)
{
  CHPRINT1BANK;
  prt_chp();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_pac(int8u bank)
{
  CHPRINT1BANK;
  prt_pac();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_gold(int8u bank)
{
  CHPRINT1BANK;
  prt_gold();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_hunger(int8u bank)
{
  CHPRINT1BANK;
  prt_hunger();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_blind(int8u bank)
{
  CHPRINT1BANK;
  prt_blind();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_confused(int8u bank)
{
  CHPRINT1BANK;
  prt_confused();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_afraid(int8u bank)
{
  CHPRINT1BANK;
  prt_afraid();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_poisoned(int8u bank)
{
  CHPRINT1BANK;
  prt_poisoned();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_state(int8u bank)
{
  CHPRINT1BANK;
  prt_state();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_speed(int8u bank)
{
  CHPRINT1BANK;
  prt_speed();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_experience(int8u bank)
{
  CHPRINT1BANK;
  prt_experience();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_study(int8u bank)
{
  CHPRINT1BANK;
  prt_study();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_winner(int8u bank)
{
  CHPRINT1BANK;
  prt_winner();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_stat(int8u stat, int8u bank)
{
  CHPRINT1BANK;
  prt_stat(stat);

  return_bank(bank);
  return;
}


/*void __fastcall__ bank_create2_set_use_stat(int8u i)
{
  CREATE2BANK;
  create2_set_use_stat(i);
  CREATE1BANK;
  return;
}*/


void __fastcall__ bank_put_misc2(void)
{
  CHPRINT1BANK;
  put_misc2();
  CREATE1BANK;

  return;
}

void __fastcall__ bank_put_misc3(void)
{
  PRINTBANK;
  put_misc3();
  CREATE1BANK;

  return;
}


void __fastcall__ bank_display_char(int8u bank)
{
  CHPRINT1BANK;
  display_char();

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_get_string(char *in_str, int8u row, int8u column, int8u slen, int8u bank)
{
  int8u ret;

  PRINTBANK;
  ret = get_string(in_str, row, column, slen);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_change_name(int8u bank)
{
  CREATE1BANK;
  change_name();

  return_bank(bank);
  return;
}


void __fastcall__ bank_init_calc_spells_prayers(void)
{
  CREATE2BANK;
  init_calc_spells_prayers();
  INITBANK;

  return;
}


void __fastcall__ bank_calc_spells(int8u stat, int8u bank)
{
  CREATE2BANK;
  create2_calc_spells(stat);

  return_bank(bank);
  return;
}


void __fastcall__ bank_calc_mana(int8u stat, int8u bank)
{
  CREATE2BANK;
  create2_calc_mana(stat);

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_stat_block(int8u bank)
{
  CHPRINT1BANK;
  prt_stat_block();

  return_bank(bank);
  return;
}


/* Always called from the same bank */
void __fastcall__ bank_prep_cave(void)
{
  GEN2BANK;
  prep_cave();
  GEN1BANK;

  return;
}


void __fastcall__ bank_generate_cave(int8u bank)
{
  GEN1BANK;
  generate_cave();

  return_bank(bank);
  return;
}


void __fastcall__ bank_town_gen(void)
{
  GEN2BANK;
  town_gen();
  GEN1BANK;

  return;
}


void __fastcall__ bank_build_type1(int8u yval, int8u xval)
{
  GEN3BANK;
  build_type1(yval, xval);
  GEN1BANK;
  return;
}


void __fastcall__ bank_build_type2(int8u yval, int8u xval)
{
  GEN3BANK;
  build_type2(yval, xval);
  GEN1BANK;
  return;
}


void __fastcall__ bank_build_type3(int8u yval, int8u xval)
{
  GEN3BANK;
  build_type3(yval, xval);
  GEN1BANK;
  return;
}


void __fastcall__ bank_place_secret_door(int8u y, int8u x, int8u bank)
{
  PLACE1BANK;
  place_secret_door(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_locked_door(int8u y, int8u x, int8u bank)
{
  PLACE1BANK;
  place_locked_door(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_door(int8u y, int8u x, int8u bank)
{
  PLACE1BANK;
  place_door(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_object(int8u y, int8u x, int8u bank)
{
  PLACE1BANK;
  place_object(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_magic_treasure(int8u x, int8u level, int8u bank)
{
  MAGICT1BANK;
  magic_treasure(x, level);

  return_bank(bank);
  return;
}


void __fastcall__ bank_magic_treasure_2(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed)
{
  MAGICT2BANK;
  magic_treasure_2(t_ptr, level, special, chance, cursed);
  MAGICT1BANK;

  return;
}


void __fastcall__ bank_vault_monster(int8u y, int8u x, int8u num)
{
  MON1BANK;
  vault_monster(y, x, num);
  GEN3BANK;

  return;
}


/* Pass 0 for place_up_stairs and 1 for place_down_stairs.      */
/* This calls either place_down_stairs or place_up_stairs.      */
void __fastcall__ bank_place_updown_stairs(int8u y, int8u x, int8u dir, int8u bank)
{
  PLACE1BANK;
  if (dir)
    place_down_stairs(y, x);
  else
    place_up_stairs(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_stairs(int8u typ, int8u num, int8u walls, int8u bank)
{
  PLACE1BANK;
  place_stairs(typ, num, walls);

  return_bank(bank);
  return;
}


void __fastcall__ bank_vault_trap(int8u y, int8u x, int8u yd, int8u xd, int8u num)
{
  PLACE1BANK;
  vault_trap(y, x, yd, xd, num);
  GEN3BANK;

  return;
}


void __fastcall__ bank_random_object(int8u y, int8u x, int16u num, int8u bank)
{
  PLACE1BANK;
  random_object(y, x, num);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_streamer(int8u fval, int8u treas_chance)
{
  PLACE1BANK;
  place_streamer(fval, treas_chance);
  GEN1BANK;

  return;
}


void __fastcall__ bank_alloc_monster(int8u num, int8u dis, int8u slp, int8u bank)
{
  MON1BANK;
  alloc_monster(num, dis, slp);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_summon_monster(int8u *y, int8u *x, int8u slp, int8u bank)
{
  int8u ret;

  MON1BANK;
  ret = summon_monster(y, x, slp);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_place_trap(int8u y, int8u x, int8u subval, int8u bank)
{
  PLACE1BANK;
  place_trap(y, x, subval);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_rubble(int8u y, int8u x, int8u bank)
{
  PLACE1BANK;
  place_rubble(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_gold(int8u y, int8u x, int8u bank)
{
  PLACE1BANK;
  place_gold(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_place_win_monster(void)
{
  MON1BANK;
  place_win_monster();
  GEN1BANK;

  return;
}


void __fastcall__ bank_screen_map(int8u bank)
{
  PRINTBANK;
  screen_map();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_map(int8u bank)
{
  PRINTBANK;
  prt_map();

  return_bank(bank);
  return;
}


void __fastcall__ bank_store_maint(int8u bank)			// maybe only used from store1.c?
{
  STORE1BANK;
  store_maint();

  return_bank(bank);
  return;
}


void __fastcall__ bank_store_destroy(int8u store_num, int8u item_val, int8u one_of, int8u bank)
{
  STORE1BANK;
  store_destroy(store_num, item_val, one_of);

  return_bank(bank);
  return;
}


int32 __fastcall__ bank_item_value(inven_type *i_ptr, int8u bank)
{
  int32 ret;

  STORE1BANK;
  ret = item_value(i_ptr);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_store_carry(int8u store_num, int8 *ipos, inven_type *t_ptr, int8u bank)
{
  STORE1BANK;
  store_carry(store_num, ipos, t_ptr);

  return_bank(bank);
  return;
}


void __fastcall__ bank_objdes(char *out_val, inven_type *i_ptr, int8u pref, int8u bank)
{
  OBJDESBANK;
  objdes(out_val, i_ptr, pref);

  return_bank(bank);
  return;
}


void __fastcall__ bank_enter_store(int8u store_num)
{
  STORE2BANK;
  enter_store(store_num);
  DUN5BANK;

  return;
}


int8u __fastcall__ bank_inven_carry(inven_type *i_ptr, int8u bank)
{
  int8u ret;

  STORE2BANK;
  ret = store2_inven_carry(i_ptr);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_prt_comment1(int8u bank)
{
  STORE3BANK;
  store3_prt_comment1();

  return_bank(bank);
  return;
}


void __fastcall__ bank_prt_comment4(int8u bank)
{
  STORE3BANK;
  store3_prt_comment4();

  return_bank(bank);
  return;
}


void __fastcall__ bank_check_strength(int8u bank)
{
  MISC1BANK;
  check_strength();

  return_bank(bank);
  return;
}


void __fastcall__ bank_calc_bonuses(int8u bank)
{
  MISC1BANK;
  misc1_calc_bonuses();

  return_bank(bank);
  return;
}


void __fastcall__ bank_calc_hitpoints(int8u bank)
{
  MISC1BANK;
  calc_hitpoints();

  return_bank(bank);
  return;
}


void __fastcall__ bank_py_bonuses(inven_type *t_ptr, int8 factor, int8u bank)
{
  MISC1BANK;
  py_bonuses(t_ptr, factor);

  return_bank(bank);
  return;
}


void __fastcall__ bank_set_use_stat(int8u stat, int8u bank)
{
  MISC1BANK;
  set_use_stat(stat);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_dec_stat(int8u stat, int8u bank)
{
  int8u ret;


  MISC1BANK;
  ret = dec_stat(stat);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_desc_remain(int8u item_val, int8u bank)
{
  MISC1BANK;
  desc_remain(item_val);

  return_bank(bank);
  return;
}


void __fastcall__ bank_lose_exp(int32 amount, int8u bank)
{
  MISC1BANK;
  lose_exp(amount);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_purchase_haggle(int8u store_num, int32 *price, inven_type *item, int8u bank)
{
  int8u ret;

  STORE3BANK;
  ret = purchase_haggle(store_num, price, item);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_sell_haggle(int8u store_num, int32 *price, inven_type *item, int8u bank)
{
  int8u ret;

  STORE3BANK;
  ret = sell_haggle(store_num, price, item);

  return_bank(bank);
  return(ret);
}


int32 __fastcall__ bank_sell_price(int8u snum, int32 *max_sell, int32 *min_sell, inven_type *item, int8u bank)
{
  int32 ret;

  STORE1BANK;
  ret = sell_price(snum, max_sell, min_sell, item);

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_get_item(int8u *com_val, char *pmt, int8u i, int8u j, char *mask, char *message, int8u bank)
{
  int8u ret;
  char p[80];								// local copy of prompt
  char msg[80];								// local copy of message


  strcpy(p, pmt);							// copy strings locally
  strcpy(msg, message);

  INVEN1BANK;
  ret = get_item(com_val, p, i, j, mask, msg);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_inven_destroy(int8u item_val, int8u bank)
{
  INVEN1BANK;
  inven_destroy(item_val);

  return_bank(bank);
  return;
}


void __fastcall__ bank_identify(int8u *item, int8u bank)
{
  INVEN1BANK;
  identify(item);

  return_bank(bank);
  return;
}


void __fastcall__ bank_inven_drop(int8u item_val, int8u drop_all, int8u bank)
{
  INVEN1BANK;
  inven_drop(item_val, drop_all);

  return_bank(bank);
  return;
}


void __fastcall__ bank_inven_command(char command, int8u bank)
{
  INVEN2BANK;
  inven_command(command);

  return_bank(bank);
  return;
}


void __fastcall__ bank_compact_objects(int8u bank)
{
  MISC1BANK;
  compact_objects();

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_delete_object(int8u y, int8u x, int8u bank)
{
  int8u ret;

  MISC1BANK;
  ret = misc1_delete_object(y, x);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_show_equip(int8u weight, int8u bank)
{
  INVEN1BANK;
  inven1_show_equip(weight);

  return_bank(bank);
  return;
}


void __fastcall__ bank_takeoff(int8u item_val, int8 posn, int8u bank)
{
  INVEN1BANK;
  inven1_takeoff(item_val, posn);

  return_bank(bank);
  return;
}


void __fastcall__ bank_dungeon_check_flags(void)
{
  DUN2BANK;
  dungeon_check_flags();
  DUN1BANK;

  return;
}


void __fastcall__ bank_find_run(void)
{
  DUN3BANK;
  find_run();
  DUN1BANK;

  return;
}


void __fastcall__ bank_end_find(int8u bank)
{
  DUN3BANK;
  dungeon3_end_find();

  return_bank(bank);
  return;
}


void __fastcall__ bank_check_view(int8u bank)
{
  DUN3BANK;
  check_view();

  return_bank(bank);
  return;
}

void __fastcall__ bank_move_light(int8u y1, int8u x1, int8u y2, int8u x2, int8u bank)
{
  DUN3BANK;
  move_light(y1, x1, y2, x2);

  return_bank(bank);
  return;
}

void __fastcall__ bank_move_char(int8u dir, int8u do_pickup, int8u bank)
{
  DUN3BANK;
  move_char(dir, do_pickup);

  return_bank(bank);
  return;
}


void __fastcall__ bank_find_init(int8u dir, int8u bank)
{
  DUN3BANK;
  find_init(dir);

  return_bank(bank);
  return;
}


void __fastcall__ bank_search(int8u y, int8u x, int8u chance, int8u bank)
{
  DUN3BANK;
  search(y, x, chance);

  return_bank(bank);
  return;
}


void __fastcall__ bank_rest(void)
{
  DUN4BANK;
  rest();
  DUN1BANK;
  return;
}


void __fastcall__ bank_go_up(void)
{
  DUN4BANK;
  go_up();
  DUN1BANK;
  return;
}


void __fastcall__ bank_go_down(void)
{
  DUN4BANK;
  go_down();
  DUN1BANK;
  return;
}


void __fastcall__ bank_jamdoor(void)
{
  DUN4BANK;
  jamdoor();
  DUN1BANK;
  return;
}


void __fastcall__ bank_openobject(void)
{
  DUN4BANK;
  openobject();
  DUN1BANK;
  return;
}


void __fastcall__ bank_examine_book(void)
{
  DUN4BANK;
  examine_book();
  DUN1BANK;
  return;
}


void __fastcall__ bank_refill_lamp(void)
{
  DUN4BANK;
  refill_lamp();
  DUN1BANK;
  return;
}


void __fastcall__ bank_hit_trap(int8u y, int8u x, int8u bank)
{
  DUN5BANK;
  hit_trap(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_carry(int8u y, int8u x, int8u pickup, int8u bank)
{
  DUN5BANK;
  carry(y, x, pickup);

  return_bank(bank);
  return;
}


void __fastcall__ bank_teleport(int8u dis, int8u bank)
{
  DUN5BANK;
  teleport(dis);

  return_bank(bank);
  return;
}


void __fastcall__ bank_scribe_object(void)
{
  DUN5BANK;
  scribe_object();
  DUN1BANK;

  return;
}


void __fastcall__ bank_closeobject(void)
{
  DUN6BANK;
  closeobject();
  DUN1BANK;
  return;
}


void __fastcall__ bank_bash(void)
{
  DUN6BANK;
  bash();
  DUN1BANK;
  return;
}


void __fastcall__ bank_eat(void)
{
  EAT1BANK;
  eat();
  DUN1BANK;
  return;
}


void __fastcall__ bank_corrode_gas(char *kb_str, int8u bank)
{
  MISC2BANK;
  corrode_gas(kb_str);

  return_bank(bank);
  return;
}


void __fastcall__ bank_poison_gas(int8u dam, char *kb_str, int8u bank)
{
  MISC2BANK;
  poison_gas(dam, kb_str);

  return_bank(bank);
  return;
}


void __fastcall__ bank_fire_dam(int8u dam, char *kb_str, int8u bank)
{
  MISC2BANK;
  fire_dam(dam, kb_str);

  return_bank(bank);
  return;
}


void __fastcall__ bank_cold_dam(int8u dam, char *kb_str, int8u bank)
{
  MISC2BANK;
  cold_dam(dam, kb_str);

  return_bank(bank);
  return;
}

void __fastcall__ bank_light_dam(int8u dam, char *kb_str, int8u bank)
{
  MISC2BANK;
  light_dam(dam, kb_str);

  return_bank(bank);
  return;
}


void __fastcall__ bank_acid_dam(int8u dam, char *kb_str, int8u bank)
{
  MISC2BANK;
  acid_dam(dam, kb_str);

  return_bank(bank);
  return;
}


void __fastcall__ bank_chest_trap(int8u y, int8u x, int8u bank)
{
  DUN6BANK;
  chest_trap(y, x);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_compact_monsters(int8u bank)
{
  int8u ret;


  MON1BANK;
  ret = mon1_compact_monsters();

  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_tot_dam(inven_type *i_ptr, int8u tdam, int16u monster, int8u bank)
{
  int8u ret;

  MISC2BANK;
  ret = tot_dam(i_ptr, tdam, monster);


  return_bank(bank);
  return(ret);
}


int8u __fastcall__ bank_critical_blow(int16u weight, int8u plus, int8u dam, int8u attack_type, int8u bank)
{
  int8u ret;

  MISC2BANK;
  ret = critical_blow(weight, plus, dam, attack_type);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_throw_object(void)
{
  DUN7BANK;
  throw_object();
  DUN1BANK;

  return;
}


void __fastcall__ bank_disarm_trap(void)
{
  DUN6BANK;
  disarm_trap();
  DUN1BANK;

  return;
}


void __fastcall__ bank_set_options(void)
{
  DUN7BANK;
  set_options();
  DUN1BANK;

  return;
}


void __fastcall__ bank_where_locate(void)
{
  DUN7BANK;
  where_locate();
  DUN1BANK;

  return;
}


void __fastcall__ bank_ident_char(void)
{
  HELP1BANK;
  ident_char();
  DUN1BANK;

  return;
}


void __fastcall__ bank_display_help(void)
{
  HELP1BANK;
  display_help();
  DUN1BANK;

  return;
}


void __fastcall__ bank_exit_game(void)
{
  DEATH1BANK;
  exit_game();
  DUN1BANK;

  return;
}


int8 __fastcall__ bank_mon_take_hit(int8u monptr, int16u dam, int8u bank)
{
  int8 ret;

  MON1BANK;
  ret = mon_take_hit(monptr, dam);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_disturb(int8u s, int8u l, int8u bank)
{
  DUN2BANK;
  dungeon2_disturb(s, l);

  return_bank(bank);
  return;
}


void __fastcall__ bank_update_mon(int8 monptr, int8u bank)
{
  MON1BANK;
  update_mon(monptr);

  return_bank(bank);
  return;
}


void __fastcall__ bank_delete_monster(int8u j, int8u bank)
{
  MON1BANK;
  delete_monster(j);

  return_bank(bank);
  return;
}


void __fastcall__ bank_fix1_delete_monster(int8u j, int8u bank)
{
  MON1BANK;
  fix1_delete_monster(j);

  return_bank(bank);
  return;
}


void __fastcall__ bank_fix2_delete_monster(int8u j, int8u bank)
{
  MON1BANK;
  fix2_delete_monster(j);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_aggravate_monster(int8u dis_affect, int8u bank)
{
  int8u ret;

  MISC2BANK;
  ret = aggravate_monster(dis_affect);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_teleport_away(int8u monptr, int8u dis, int8u bank)
{
  MISC2BANK;
  teleport_away(monptr, dis);

  return_bank(bank);
  return;
}


int8u __fastcall__ bank_player_saves(int8u bank)
{
  int8u ret;

  MISC2BANK;
  ret = player_saves();

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_creatures(int8u attack, int8u bank)
{
  MON2BANK;
  creatures(attack);

  return_bank(bank);
  return;
}


void __fastcall__ bank_py_attack(int8u y, int8u x, int8u bank)
{
  DUN8BANK;
  py_attack(y, x);

  return_bank(bank);
  return;
}


void __fastcall__ bank_quaff(void)
{
  POTIONBANK;
  quaff();
  DUN1BANK;

  return;
}


void __fastcall__ bank_print_spells(int8u *spell, int8u num, int8u comment, int8 nonconsec, int8u bank)
{
  CHPRINT1BANK;
  print_spells(spell, num, comment, nonconsec);

  return_bank(bank);
  return;
}


void __fastcall__ bank_make_attack(int8u monptr)
{
  MON3BANK;
  make_attack(monptr);
  MON2BANK;

  return;
}


void __fastcall__ bank_display_scores(void)
{
  DEATH1BANK;
  display_scores();
  DUN1BANK;

  return;
}


int8u __fastcall__ bank_multiply_monster(int8u y, int8u x, int16u cr_index, int8u monptr, int8u bank)
{
  int8u ret;


  MON4BANK;
  ret = multiply_monster(y, x, cr_index, monptr);

  return_bank(bank);
  return(ret);
}


void __fastcall__ bank_gain_spells(void)
{
  DUN8BANK;
  gain_spells();
  DUN1BANK;

  return;
}


void __fastcall__ bank_mon_cast_spell(int8u monptr, int8u *took_turn, int8u bank)
{
  MON4BANK;
  mon_cast_spell(monptr, took_turn);

  return_bank(bank);
  return;
}

