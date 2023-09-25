/* Interbank calls
 *
 * These are moria functions that are called from banking code in low memory, from one cartridge bank to another.
 * Should only be called from code defined in setup.c
 */

#ifndef INTERBANK_H
#define INTERBANK_H

#include "externs.h"
#include "types.h"


// cast.c
void __fastcall__ cast(void);

// caveplace.c
void place_boundary(void);
void __fastcall__ place_streamer(int8u fval, int8u treas_chance);
void __fastcall__ place_open_door(int8u y, int8u x);
void __fastcall__ place_broken_door(int8u y, int8u x);
void __fastcall__ place_closed_door(int8u y, int8u x);
void __fastcall__ place_locked_door(int8u y, int8u x);
void __fastcall__ place_stuck_door(int8u y, int8u x);
void __fastcall__ place_secret_door(int8u y, int8u x);
void __fastcall__ place_door(int8u y, int8u x);
void __fastcall__ place_up_stairs(int8u y, int8u x);
void __fastcall__ place_down_stairs(int8u y, int8u x);
void __fastcall__ place_stairs(int8u typ, int8u num, int8u walls);
void __fastcall__ place_gold(int8u y, int8u x);
void __fastcall__ place_trap(int8u y, int8u x, int8u subval);
void __fastcall__ place_object(int8u y, int8u x);
void __fastcall__ vault_trap(int8u y, int8u x, int8u yd, int8u xd, int8u num);
void __fastcall__ random_object(int8u y, int8u x, int16u num);
void __fastcall__ place_rubble(int8u y, int8u x);
int16u __fastcall__ get_obj_num(int8u level);

// create1.c
void choose_race(void);
void print_history(void);
void get_history(void);
void get_sex(void);
void get_ahw(void);
void get_class(void);
void create_character(void);
void get_name(void);
void change_name(void);
void __fastcall__ create1_put_buffer(char *out_str, int8 row, int8 col);
int16u __fastcall__ create1_randnor(int16u mean, int16u stand);

// create2.c
void __fastcall__ create2_calc_spells(int8u stat);
void __fastcall__ create2_calc_mana(int8u stat);
void get_all_stats(void);
void get_stats(void);
void __fastcall__ create2_set_use_stat(int8u stat);
void init_calc_spells_prayers(void);

// creature1.c
void __fastcall__ alloc_monster(int8u num, int8u dis, int8u slp);
int8u __fastcall__ summon_monster(int8u *y, int8u *x, int8u slp);
void place_win_monster(void);
int8u __fastcall__ mon1_place_monster(int8u y, int8u x, int16u z, int8u slp);
void __fastcall__ vault_monster(int8u y, int8u x, int8u num);
int8u __fastcall__ mon1_compact_monsters(void);
int8 __fastcall__ mon_take_hit(int8u monptr, int16 dam);
void __fastcall__ update_mon(int8 monptr);
void __fastcall__ delete_monster(int8u j);
void __fastcall__ fix1_delete_monster(int8u j);
void __fastcall__ fix2_delete_monster(int8u j);
int16u __fastcall__ get_mons_num(int8u level);

// creature2.c
void __fastcall__ creatures(int8u attack);

// creature3.c
void __fastcall__ make_attack(int8u monptr);

// creature4.c
int8u __fastcall__ multiply_monster(int8u y, int8u x, int16u cr_index, int8u monptr);
void __fastcall__ mon_cast_spell(int8u monptr, int8u *took_turn);
int8u __fastcall__ summon_undead(int8u *y, int8u *x);
void __fastcall__ get_moves(int8u monptr, int8u *mm);

// creature5.c
int32u __fastcall__ monster_death(int8u y, int8u x, int32u flags);

// death1.c
void exit_game(void);
void display_scores(void);

// dungeon1.c
void __fastcall__ dungeon(void);
int8u __fastcall__ twall(int8u y, int8u x, int8u t1, int8u t2);

// dungeon2.c
void __fastcall__ dungeon2_disturb(int8u s, int8u l);
void __fastcall__ dungeon_check_flags(void);

// dungeon3.c
void __fastcall__ find_run(void);
void __fastcall__ dungeon3_end_find(void);
void __fastcall__ check_view(void);
void __fastcall__ move_light(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ move_char(int8u dir, int8u do_pickup);
void __fastcall__ find_init(int8u dir);
void __fastcall__ search(int8u y, int8u x, int8u chance);
void __fastcall__ light_room(int8u y, int8u x);

// dungeon4.c
void __fastcall__ rest(void);
void __fastcall__ go_up(void);
void __fastcall__ go_down(void);
void __fastcall__ openobject(void);
void __fastcall__ examine_book(void);
void __fastcall__ jamdoor(void);
void __fastcall__ refill_lamp(void);

// dungeon5.c
void __fastcall__ hit_trap(int8u y, int8u x);
void __fastcall__ carry(int8u y, int8u x, int8u pickup);
void __fastcall__ teleport(int8u dis);
void __fastcall__ scribe_object(void);

// dungeon6.c
void __fastcall__ closeobject(void);
void __fastcall__ bash(void);
void __fastcall__ chest_trap(int8u y, int8u x);
void __fastcall__ disarm_trap(void);

// dungeon7.c
void __fastcall__ throw_object(void);
void __fastcall__ set_options(void);
void __fastcall__ where_locate(void);

// dungeon8.c
void __fastcall__ py_attack(int8u y, int8u x);
void __fastcall__ gain_spells(void);

// eat1.c
void __fastcall__ eat(void);

// generate1.c
void generate_cave(void);

// generate2.c
void prep_cave(void);
void blank_cave(void);
void tlink(void);
void mlink(void);
void town_gen(void);

// generate3.c
void __fastcall__ build_type1(int8u yval, int8u xval);
void __fastcall__ build_type2(int8u yval, int8u xval);
void __fastcall__ build_type3(int8u yval, int8u xval);

// guild.c
void __fastcall__ enter_guild(void);

// help.c
void __fastcall__ ident_char(void);
void __fastcall__ display_help(void);
void __fastcall__ display_wizard_help(void);

// io.c
void __fastcall__ put_buffer(char *out_str, int8 row, int8 col);
char __fastcall__ inkey(void);
void __fastcall__ erase_line(int8u row, int8u col);
//void __fastcall__ clear_screen(void);
void __fastcall__ clear_from(int8u row);
void __fastcall__ print(char ch, int8u row, int8u col);
void __fastcall__ print_symbol(int8u row, int8u col);
void __fastcall__ print_symbol_highlight(int8u row, int8u col);
void __fastcall__ print_self(int8u row, int8u col);
void __fastcall__ count_msg_print(char *p);
void __fastcall__ prt(char *str_buff, int8u row, int8u col);
void __fastcall__ msg_print(char *str_buff);
int8u __fastcall__ get_com(char *prompt, char *command);
int8u __fastcall__ get_string(char *in_str, int8u row, int8u column, int8u slen);
void __fastcall__ screen_map(void);
void __fastcall__ prt_map(void);
void __fastcall__ pretty_print(char *text, int8u line);

// init.c
void moria_init(void);
void char_inven_init(void);
void init_m_level(void);
void init_t_level(void);
void store_init(void);
void magic_init(void);
void __fastcall__ init_seeds(int32u seed);

// inventory1.c
int8u __fastcall__ get_item(int8u *com_val, char *pmt, int8u i, int8u j, char *mask, char *message);
void __fastcall__ inven_destroy(int8u item_val);
void __fastcall__ identify(int8u *item);
void __fastcall__ inven_drop(int8u item_val, int8u drop_all);
void __fastcall__ inven1_show_equip(int8u weight);
void __fastcall__ inven1_takeoff(int8u item_val, int8 posn);

// inventory2.c
void __fastcall__ inven_command(char command);

// look.c
void __fastcall__ look(void);

// magictreasure1.c
void __fastcall__ magic_treasure(int8u x, int8u level);
void __fastcall__ magic_treasure_1(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed);

// magictreasure2.c
void __fastcall__ magic_treasure_2(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed);

// misc1.c
void __fastcall__ check_strength(void);
void __fastcall__ misc1_calc_bonuses(void);
void __fastcall__ calc_hitpoints(void);
void __fastcall__ py_bonuses(inven_type *t_ptr, int8 factor);
void __fastcall__ compact_objects(void);
int8u __fastcall__ misc1_delete_object(int8u y, int8u x);
void __fastcall__ desc_remain(int8u item_val);
void __fastcall__ set_use_stat(int8u stat);
int8u __fastcall__ dec_stat(int8u stat);
void __fastcall__ lose_exp(int32 amount);
void __fastcall__ misc1_invcopy(inven_type *to, int16u from_index);

// misc2.c
void __fastcall__ corrode_gas(char *kb_str);
void __fastcall__ poison_gas(int8u dam, char *kb_str);
void __fastcall__ fire_dam(int8u dam, char *kb_str);
void __fastcall__ cold_dam(int8u dam, char *kb_str);
void __fastcall__ light_dam(int8u dam, char *kb_str);
void __fastcall__ acid_dam(int8u dam, char *kb_str);
int8u __fastcall__ tot_dam(inven_type *i_ptr, int8u tdam, int8u monster);
int8u __fastcall__ critical_blow(int16u weight, int8u plus, int8u dam, int8u attack_type);
int8u __fastcall__ aggravate_monster(int8u dis_affect);
void __fastcall__ teleport_away(int8u monptr, int8u dis);
int8u __fastcall__ player_saves(void);

// objdes.c
void __fastcall__ objdes(char *out_val, inven_type *i_ptr, int8u pref);

// potions.c
void quaff(void);
int8u __fastcall__ potion1_find_range(int8u item1, int8u item2, int8u *j, int8u *k);

// prayer.c
void __fastcall__ pray(void);

// PrintChar1.c
void __fastcall__ cnv_stat(int8u stat, char *out_val);
void __fastcall__ prt_stat(int8u stat);
void __fastcall__ prt_field(char *info, int8u row, int8u column);
void __fastcall__ prt_lnum(char *header, int32 num, int8u row, int8u column);
void __fastcall__ prt_num(char *header, int num, int8u row, int8u column);
void __fastcall__ prt_long(int32 num, int8u row, int8u column);
void __fastcall__ prt_int(int num, int8u row, int8u column);
char* __fastcall__ title_string(void);
void prt_title(void);
void prt_level(void);
void prt_cmana(void);
void prt_mhp(void);
void prt_chp(void);
void prt_pac(void);
void prt_gold(void);
void prt_depth(void);
void prt_hunger(void);
void prt_blind(void);
void prt_confused(void);
void prt_afraid(void);
void prt_poisoned(void);
void prt_state(void);
void prt_speed (void);
void prt_study(void);
void prt_winner(void);
void prt_stat_block(void);
void put_character(void);
void put_stats(void);
void put_misc1(void);
void put_misc2(void);
void display_char(void);
void __fastcall__ prt_experience(void);
void __fastcall__ printchar_put_buffer(char *out_str, int8 row, int8 col);
void printchar_clear_screen(void);
void __fastcall__ print_spells(int8u *spell, int8u num, int8u comment, int8 nonconsec);

// printchar2.c
char * __fastcall__ likert(int x, int8u y);
void put_misc3(void);

// quest1.c
int8u __fastcall__ quest_start(int8u chance);
void __fastcall__ quest_end(void);

// quest2.c
void __fastcall__ show_quests(void);
void __fastcall__ eval_quest_mon(int16u mon);
void __fastcall__ eval_quest_level(int8u level);
void __fastcall__ eval_quest_item(void);
void __fastcall__ compact_quests(void);

// recall.c
int8u __fastcall__ bool_roff_recall(int16u mon_num);
char __fastcall__ roff_recall(int16u mon_num);

// scrolls1.c
void __fastcall__ read_scroll(void);

// spells1.c
int8u __fastcall__ sleep_monsters1(int8u y, int8u x);
int8u __fastcall__ detect_treasure(void);
int8u __fastcall__ detect_object(void);
int8u __fastcall__ detect_trap(void);
int8u __fastcall__ detect_sdoor(void);
int8u __fastcall__ detect_invisible(void);
int8u __fastcall__ light_area(int8u y, int8u x);
int8u __fastcall__ unlight_area(int8u y, int8u x);
void __fastcall__ map_area(void);
int8u __fastcall__ ident_spell(void);
int8u __fastcall__ remove_curse(void);
void __fastcall__ warding_glyph(void);
int8u __fastcall__ protect_evil(void);
void __fastcall__ create_food(void);
int8u __fastcall__ td_destroy(void);
int8u __fastcall__ recharge(int8u num);
int8u __fastcall__ mass_genocide(void);
int8u __fastcall__ genocide(void);

// spells2.c
void __fastcall__ light_line(int8u dir, int8u y, int8u x);
void __fastcall__ starlite(int8u y, int8u x);
void __fastcall__ fire_bolt(int8u typ, int8u dir, int8u y, int8u x, int8u dam, char *bolt_typ);
void __fastcall__ fire_ball(int8u typ, int8u dir, int8u y, int8u x, int8u dam_hp, char *descrip);
void __fastcall__ breath(int8u typ, int8u y, int8u x, int8u dam_hp, char *ddesc, int8u monptr);

// spells3.c
void __fastcall__ destroy_area(int8u y, int8u x);
int8u __fastcall__ dispel_creature(int8u cflag, int8u damage);
int8u __fastcall__ disarm_all(int8u dir, int8u y, int8u x);
int8u __fastcall__ hp_monster(int8u dir, int8u y, int8u x, int8 dam);
int8u __fastcall__ drain_life(int8u dir, int8u y, int8u x);
int8u __fastcall__ speed_monster(int8u dir, int8u y, int8u x, int8 spd);
int8u __fastcall__ confuse_monster(int8u dir, int8u y, int8u x);
int8u __fastcall__ sleep_monster(int8u dir, int8u y, int8u x);
int8u __fastcall__ td_destroy2(int8u dir, int8u y, int8u x);
int8u __fastcall__ poly_monster(int8u dir, int8u y, int8u x);
int8u __fastcall__ clone_monster(int8u dir, int8u y, int8u x);

// spells4.c
int8u __fastcall__ wall_to_mud(int8u dir, int8u y, int8u x);
int8u __fastcall__ build_wall(int8u dir, int8u y, int8u x);
int8u __fastcall__ speed_monsters(int8u spd);
int8u __fastcall__ sleep_monsters2(void);
int8u __fastcall__ mass_poly(void);
void __fastcall__ earthquake(void);
int8u __fastcall__ turn_undead(void);
int8u __fastcall__ detect_monsters(void);
int8u __fastcall__ teleport_monster(int8u dir, int8u y, int8u x);
int8u __fastcall__ detect_evil(void);

// staffs.c
void __fastcall__ use(void);

// store1.c
int32 __fastcall__ item_value(inven_type *i_ptr);
int32 __fastcall__ sell_price(int8u snum, int32 *max_sell, int32 *min_sell, inven_type *item);
void store_carry(int8u store_num, int8 *ipos, inven_type *t_ptr);
void store_destroy(int8u store_num, int8u item_val, int8u one_of);
void store_maint(void);

// store2.c
void __fastcall__ display_store(int8u store_num, int8u cur_top);
int8u __fastcall__ store2_inven_carry(inven_type *i_ptr);

void __fastcall__ enter_store(int8u store_num);
int8u __fastcall__ general_store(int8u element);
int8u __fastcall__ armory(int8u element);
int8u __fastcall__ weaponsmith(int8u element);
int8u __fastcall__ temple(int8u element);
int8u __fastcall__ alchemist(int8u element);
int8u __fastcall__ magic_shop(int8u element);

// store3.c
int8u __fastcall__ purchase_haggle(int8u store_num, int32 *price, inven_type *item);
int8u __fastcall__ sell_haggle(int8u store_num, int32 *price, inven_type *item);
void __fastcall__ store3_prt_comment1(void);
void __fastcall__ store3_prt_comment4(void);

// tavern1.c
void __fastcall__ tavern_init(void);
void __fastcall__ enter_tavern(void);

// tavern2.c
int8u __fastcall__ tavern_gossip(int8u chance, int8u barkeep);

// wands.c
void __fastcall__ aim(void);

// wizard.c
int8u __fastcall__ enter_wiz_mode(void);
void __fastcall__ wizard_commands(char com_val);

// wizard2.c
void __fastcall__ wizard_display_player(void);


#endif