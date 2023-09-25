/* source/externs.h: declarations for global variables and initialized data

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifndef EXTERNS_H
#define EXTERNS_H

#include "vbxebank.h"                   // VBXE banking functions
#include "constant.h"
#include "types.h"


// Cartridge Banking Defines
#define STARTUPBANK				asm("sta $D57F")
#define INITBANK                asm("sta $D50A")
#define CREATE1BANK             asm("sta $D50B")
#define CREATE2BANK             asm("sta $D50C")
#define PRINTBANK               asm("sta $D50D")
#define CHPRINT1BANK            asm("sta $D50E")
#define GEN1BANK                asm("sta $D50F")
#define GEN2BANK                asm("sta $D510")
#define GEN3BANK                asm("sta $D511")
#define MON1BANK                asm("sta $D512")
#define MON2BANK				asm("sta $D513")
#define MON3BANK				asm("sta $D514")
#define MON4BANK				asm("sta $D515")
#define PLACE1BANK              asm("sta $D516")
#define HELP1BANK	            asm("sta $D517")
#define STORE1BANK				asm("sta $D518")
#define STORE2BANK				asm("sta $D519")
#define STORE3BANK				asm("sta $D51A")
#define OBJDESBANK				asm("sta $D51B")
#define INVEN1BANK				asm("sta $D51C")
#define INVEN2BANK				asm("sta $D51D")
#define DUN1BANK				asm("sta $D51E")
#define DUN2BANK				asm("sta $D51F")
#define DUN3BANK				asm("sta $D520")
#define DUN4BANK				asm("sta $D521")
#define DUN5BANK				asm("sta $D522")
#define DUN6BANK				asm("sta $D523")
#define DUN7BANK				asm("sta $D524")
#define DUN8BANK				asm("sta $D525")
#define MAGICT1BANK             asm("sta $D526")
#define MAGICT2BANK             asm("sta $D527")
#define POTIONBANK				asm("sta $D528")
#define SCROLLBANK				asm("sta $D529")
#define SPELLS1BANK				asm("sta $D52A")
#define SPELLS2BANK				asm("sta $D52B")
#define SPELLS3BANK				asm("sta $D52C")
#define SPELLS4BANK				asm("sta $D52D")
#define DEATH1BANK				asm("sta $D52E")
#define EAT1BANK				asm("sta $D52F")
#define MISC1BANK				asm("sta $D530")
#define MISC2BANK				asm("sta $D531")
#define WIZ1BANK				asm("sta $D532")
#define WIZ2BANK				asm("sta $D533")
#define STAFFSBANK				asm("sta $D534")
#define PRAYERBANK				asm("sta $D535")
#define CASTBANK				asm("sta $D536")
#define LOOKBANK				asm("sta $D537")
#define RECALLBANK				asm("sta $D538")
#define MON5BANK				asm("sta $D539")
#define WANDBANK				asm("sta $D53A")
#define GUILD1BANK				asm("sta $D53B")
#define TAVERN1BANK				asm("sta $D53C")
#define TAVERN2BANK				asm("sta $D53D")
#define QUEST1BANK				asm("sta $D53F")
#define QUEST2BANK				asm("sta $D540")
#define CHARSAVE0BANK			asm("sta $D560")
#define CHARSAVE1BANK			asm("sta $D561")
#define RECALLSAVEBANK			asm("sta $D567")
#define GAMESAVE0BANK			asm("sta $D568")
#define GAMESAVE1BANK			asm("sta $D569")
#define GAMESAVE2BANK			asm("sta $D56A")
#define GAMESAVE3BANK			asm("sta $D56B")
#define GAMESAVE4BANK			asm("sta $D56C")
#define HIGHSCOREBANK			asm("sta $D570")
#define BANKCODE2BANK			asm("sta $D57C")
#define BANKCODE1BANK			asm("sta $D57D")
#define BANKCODE0BANK			asm("sta $D57E")

#define STARTUPNUM				0x7F
#define INITNUM                 0x0A
#define CREATE1NUM              0x0B
#define CREATE2NUM              0x0C
#define PRINTNUM                0x0D
#define CHPRINT1NUM             0x0E
#define GEN1NUM                 0x0F
#define GEN2NUM                 0x10
#define GEN3NUM                 0x11
#define MON1NUM                 0x12
#define MON2NUM                 0x13
#define MON3NUM					0x14
#define MON4NUM					0x15
#define PLACE1NUM               0x16
#define HELP1NUM                0x17
#define STORE1NUM				0x18
#define STORE2NUM				0x19
#define STORE3NUM				0x1A
#define OBJDESNUM				0x1B
#define INVEN1NUM				0x1C
#define INVEN2NUM				0x1D
#define DUN1NUM					0x1E
#define DUN2NUM					0x1F
#define DUN3NUM					0x20
#define DUN4NUM					0x21
#define DUN5NUM					0x22
#define DUN6NUM					0x23
#define DUN7NUM					0x24
#define DUN8NUM					0x25
#define MAGICT1NUM              0x26
#define MAGICT2NUM              0x27
#define POTIONNUM				0x28
#define SCROLLNUM				0x29
#define SPELLS1NUM				0x2A
#define SPELLS2NUM				0x2B
#define SPELLS3NUM				0x2C
#define SPELLS4NUM				0x2D
#define DEATH1NUM				0x2E
#define EAT1NUM					0x2F
#define MISC1NUM				0x30
#define MISC2NUM				0x31
#define WIZ1NUM					0x32
#define WIZ2NUM					0x33
#define STAFFSNUM				0x34
#define PRAYERNUM				0x35
#define CASTNUM					0x36
#define LOOKNUM					0x37
#define RECALLNUM				0x38
#define MON5NUM					0x39
#define WANDNUM					0x3A
#define GUILD1NUM				0x3B
#define TAVERN1NUM				0x3C
#define TAVERN2NUM				0x3D
#define QUEST1NUM				0x3F
#define QUEST2NUM				0x40

#define CHARSAVE0NUM			0x60
#define CHARSAVE1NUM			0x61
#define RECALLSAVENUM			0x67
#define GAMESAVE0NUM			0x68
#define GAMESAVE1NUM			0x69
#define GAMESAVE2NUM			0x6A
#define GAMESAVE3NUM			0x6B
#define GAMESAVE4NUM			0x6C
#define HIGHSCORENUM			0x70




/* Map symbol defines */
#define SYM_PLAYER	'@'
//#define SYM_WALL	'#'
#define SYM_WALL	0x10
#define SYM_SEAM	0x7F
#define SYM_FLOOR	'.'


/* Joystick control */
#define	STICK0		*(unsigned char *) 0x278
#define STRIG0		*(unsigned char *) 0x284
#define RTCLOK0		*(unsigned char *) 0x14
#define RTCLOK1		*(unsigned char *) 0x13
#define RTCLOK2		*(unsigned char *) 0x12
extern int8u joystick_roguedir[16];
extern char joystick_command[16];


/* DEBUGGING FIXME */
extern char dbgstr[80];

/* Strings storage in main memory, used in place of stack storage in various functions -SJ */
extern bigvtype out_val1, out_val2;				// moved here from functions below (placed in BSS in lowram)
extern char msg_print_str[VTYPESIZ];			// buffer array in BSS for message print functions (used in bankcode mainly)

extern char text[5 * 76];						// text to be printed, 5 lines, 384 bytes, in GENRAM for pretty print function


/* horrible hack: needed because compact_monster() can be called from deep
   within creatures() via place_monster() and summon_monster() */
extern int8 hack_monptr;						// can be int8 as MAX_MALLOC is 125 -SJ

extern vtype died_from;

/* These are options, set with set_options command -CJS- */
extern int8u find_cut;                  /* Cut corners on a run */
extern int8u find_examine;              /* Check corners on a run */
extern int8u find_prself;               /* Print yourself on a run (slower) */
extern int8u find_bound;                /* Stop run when the map shifts */
extern int8u prompt_carry_flag; 		/* Prompt to pick something up */
extern int8u show_weight_flag;  		/* Display weights in inventory */
extern int8u highlight_seams;           /* Highlight magma and quartz */
extern int8u find_ignore_doors; 		/* Run through open doors */
extern int8u display_counts;            /* Display rest/repeat counts */
extern int8u classic_moria;				/* Classic Moria or Atari Enhanced? */

/* global flags */
extern int8u new_level_flag;    		/* Next level when true  */
extern int8u teleport_flag;     		/* Handle teleport traps  */
extern int8u player_light;      		/* Player carrying light */
#pragma zpsym("player_light")
extern int8u find_flag; 				/* Used in MORIA */
extern int8u free_turn_flag;    		/* Used in MORIA */
extern int8u weapon_heavy;      		/* Flag if the weapon too heavy -CJS- */
extern int8u pack_heavy;                /* Flag if the pack too heavy -CJS- */
extern int8u doing_inven;        		/* Track inventory commands */
extern int8u screen_change;     		/* Screen changes (used in inven_commands) */

extern int8u character_generated;       /* don't save score until char gen finished*/
extern int8u character_saved;    		/* prevents save on kill after save_char() */
extern int8u command_count;     		/* Repetition of commands. -CJS- */
extern int8u default_dir;       		/* Use last direction in repeated commands */
extern int8u noscore;           		/* Don't score this game. -CJS- */
extern int16u randes_seed;    			/* For encoding colors */
extern int16u town_seed;            	/* Seed for town genera*/
extern int8u dun_level; 				/* Cur dungeon level   */
#pragma zpsym("dun_level")
extern int16 missile_ctr;       		/* Counter for missiles */
extern int8u msg_flag;  				/* Set with first msg  */
extern vtype old_msg[MAX_SAVE_MSG];     /* Last messages -CJS- */
extern int8u last_msg;                  /* Where in the array is the last */
extern int8u death;     				/* True if died       */
extern int32 turn;      				/* Cur trun of game    */
extern int8u wizard;    				/* Wizard flag        */
//extern int8u to_be_wizard;
extern int8u total_winner;
extern int32 max_score;
extern int8u guild_member;				/* Guild member? Atari Enhancement -SJ */

extern int8u cur_height, cur_width;     /* Cur dungeon size    */
#pragma zpsym("cur_height")
#pragma zpsym("cur_width")

/*  Following are calculated from max dungeon sizes             */
extern int8u max_panel_rows, max_panel_cols;
extern int8  panel_row, panel_col;
extern int8 panel_row_min, panel_row_max;
extern int16 panel_col_min, panel_col_max;
extern int16 panel_col_prt;
extern int8 panel_row_prt;
#pragma zpsym("max_panel_rows")
#pragma zpsym("max_panel_cols")
#pragma zpsym("panel_row")
#pragma zpsym("panel_col")
#pragma zpsym("panel_row_min")
#pragma zpsym("panel_row_max")
#pragma zpsym("panel_col_min")
#pragma zpsym("panel_col_max")

/* Following are all floor definitions */
extern cave_type cave[MAX_HEIGHT][MAX_WIDTH];
extern int16u crow_lookup[MAX_HEIGHT];				/* cave row memory address lookup table */

#define CAVE_ADR(y, x)		(cave_type *)(crow_lookup[y] + ((int16u)(x) * sizeof(cave_type)))


/* Following are player variables */
extern player_type py;
extern char *player_title[MAX_CLASS][MAX_PLAYER_LEVEL];
extern race_type race[MAX_RACES];
extern background_type background[MAX_BACKGROUND];
extern int32u player_exp[MAX_PLAYER_LEVEL];
extern int16u player_hp[MAX_PLAYER_LEVEL];

extern int8u char_row;
extern int8u char_col;
#pragma zpsym("char_row")
#pragma zpsym("char_col")

extern int8u rgold_adj[MAX_RACES][MAX_RACES];
extern class_type class[MAX_CLASS];
extern int8 class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

/* Warriors don't have spells, so there is no entry for them. */
extern spell_type magic_spell[MAX_CLASS-1][31];
extern char *spell_names[62];
extern int32u spell_learned;    /* Bit field for spells learnt -CJS- */
extern int32u spell_worked;     /* Bit field for spells tried -CJS- */
extern int32u spell_forgotten;  /* Bit field for spells forgotten -JEW- */
extern int8u spell_order[32];   /* remember order that spells are learned in */
extern int16u player_init[MAX_CLASS][5];

/* Following are store definitions */
extern owner_type owners[MAX_OWNERS];
extern store_type store[MAX_STORES];
extern int16u store_choice[MAX_STORES][STORE_CHOICES];

/* Following are treasure arrays and variables */
extern treasure_type object_list[MAX_OBJECTS];
extern int8u object_ident[OBJECT_IDENT_SIZE];
extern int16u t_level[MAX_OBJ_LEVEL+1];
extern inven_type t_list[MAX_TALLOC];
extern inven_type inventory[INVEN_ARRAY_SIZE];
extern char *special_names[SN_ARRAY_SIZE];
extern int16u sorted_objects[MAX_DUNGEON_OBJ];
extern int8u inven_ctr;         				/* Total different obj's */
extern int16u inven_weight;      				/* Cur carried weight */
extern int8u equip_ctr; 						/* Cur equipment ctr */
extern int8u tcptr;     						/* Cur treasure heap ptr */
#pragma zpsym("tcptr")

/* Following are creature arrays and variables */
extern creature_type c_list[MAX_CREATURES];
extern monster_type m_list[MAX_MALLOC];
extern int16u m_level[MAX_MONS_LEVEL+1];
extern m_attack_type monster_attacks[N_MONS_ATTS];
extern recall_type c_recall[MAX_CREATURES];     /* Monster memories. -CJS- */
extern monster_type blank_monster;      		/* Blank monster values */
extern int8u mfptr;     						/* Cur free monster ptr, was int16 -SJ */
#pragma zpsym("mfptr")
extern int8u mon_tot_mult;      				/* # of repro's of creature */
#pragma zpsym("mon_tot_mult")

/* Following are arrays for descriptive pieces */
extern char *colors[MAX_COLORS];
extern char *mushrooms[MAX_MUSH];
extern char *woods[MAX_WOODS];
extern char *metals[MAX_METALS];
extern char *rocks[MAX_ROCKS];
extern char *amulets[MAX_AMULETS];
extern char *syllables[MAX_SYLLABLES];
extern char titles[MAX_TITLES][10];

extern int8u blows_table[7][6];

extern int16u normal_table[NORMAL_TABLE_SIZE];

/* Initialized data which had to be moved from some other file */
/* Since these get modified, macrsrc.c must be able to access them */
/* Otherwise, game cannot be made restartable */
/* dungeon.c */
extern char last_command;  /* Memory of previous command. */
/* moria1.c */
/* Track if temporary light about player.  */
extern int8u light_flag;

/* Store Comments */
/* Moved into SPELLBANK - storecmt.c */
extern char *comment1[14];
extern char *comment2a[3];
extern char *comment2b[16];
extern char *comment3a[3];
extern char *comment3b[15];
extern char *comment4a[5];
extern char *comment4b[5];
extern char *comment5[10];
extern char *comment6[5];

/* Defines for place_stairs functions */
#define UPSTAIRS        0
#define DOWNSTAIRS      1

/* High Scores */
extern int8u score_entries_ram;			        	/* Number of high score entries */
extern high_scores scores_ram[SCOREFILE_SIZE];		/* High Scores in main RAM      */

extern int8u score_entries_cart;			        /* Number of high score entries */
extern high_scores scores_cart[SCOREFILE_SIZE];		/* High Scores in cart bank     */

/* Monster Memories */
extern recall_type save_c_recall[MAX_CREATURES];	/* Monster memories in cart bank */

/* Tavern variables */
extern int8u tavern_got_gossip;						// already received gossip for today?

/* Quest variables */
extern quest_type quests[QUEST_SLOTS];				// quests


/*									 */
/* Character and Game Save Variables */
/*									 */
extern int8u save_find_cut;
extern int8u save_find_examine;
extern int8u save_find_bound;
extern int8u save_find_prself;
extern int8u save_prompt_carry_flag;
extern int8u save_show_weight_flag;
extern int8u save_highlight_seams;
extern int8u save_find_ignore_doors;
extern int8u save_display_counts;
extern int8u save_classic_moria;
extern int8u save_death;
extern int8u save_total_winner;
extern int8u save_noscore;
extern int32 save_turn;
extern int16u save_randes_seed;
extern int16u save_town_seed;
extern int16 save_missile_ctr;
extern vtype save_old_msg[MAX_SAVE_MSG];
extern int8u save_last_msg;
extern vtype save_died_from;
extern int32 save_max_score;
extern int8u save_guild_member;

extern player_type save_py;
extern int16u save_player_hp[MAX_PLAYER_LEVEL];
extern int32u save_spell_learned;
extern int32u save_spell_worked;
extern int32u save_spell_forgotten;
extern int8u save_spell_order[32];

extern int8u save_inven_ctr;
extern int16u save_inven_weight;
extern int8u save_equip_ctr;
extern inven_type save_inventory[INVEN_ARRAY_SIZE];
extern int8u save_object_ident[OBJECT_IDENT_SIZE];

extern store_type save_store[MAX_STORES];

extern char save_cave0[0x2000];			// 44x132x4 = 0x5AC0
extern char save_cave1[0x2000];
extern char save_cave2[0x1AC0];
extern int8u save_dun_level;
extern int8u save_max_panel_rows, save_max_panel_cols;
extern int8u save_cur_height, save_cur_width;
extern int8u save_char_row;
extern int8u save_char_col;

extern int8u save_tcptr;
extern inven_type save_t_list[MAX_TALLOC];

extern int8u save_mfptr;
extern int8u save_mon_tot_mult;
extern monster_type save_m_list[MAX_MALLOC];

extern quest_type save_quests[QUEST_SLOTS];

// inbank/los.c zeropage variables
extern int8 los_dx, los_dy;
extern int8 los_sx, los_sy;
extern int8 los_err, los_e2;
#pragma zpsym("los_dx")
#pragma zpsym("los_dy")
#pragma zpsym("los_sx")
#pragma zpsym("los_sy")
#pragma zpsym("los_err")
#pragma zpsym("los_e2")



/* Functions that are in low memory, no need to bank, any cartridge code can call them */
// main.c
void main_loop(void);

// irqhand.s
void IntHandlerInit(void);

// flashcart.c
void __fastcall__ flash_erase_bank(unsigned char bank);
void __fastcall__ flash_prog_bank(unsigned char bank, void *dst, void *src, unsigned int count);

// rnd.c
int16u __fastcall__ get_rnd_seed(void);
void __fastcall__ set_rnd_seed(int16u seedval);
//int32 __fastcall__ rnd(void);
void __fastcall__ set_seed(int16u seed);
void __fastcall__ reset_seed(void);
int16u __fastcall__ randint(int16u maxval);

// save.c
void __fastcall__ copy_highscores_to_ram(void);
void __fastcall__ write_recall_to_cart(void);
void __fastcall__ write_highscores_to_cart(void);
int8u __fastcall__ get_char(int8u *generate);
void __fastcall__ save_char(void);


/* Banking functions, code in setup.c 					  */
/* These should be the only way funtions call out of bank */

void 	__fastcall__ bank_acid_dam(int8u dam, char *kb_str, int8u bank);
int8u   __fastcall__ bank_aggravate_monster(int8u dis_affect, int8u bank);
void	__fastcall__ bank_aim(void);
void 	__fastcall__ bank_alloc_monster(int8u num, int8u dis, int8u slp, int8u bank);
void 	__fastcall__ bank_bash(void);
int8u 	__fastcall__ bank_bool_roff_recall(int16u mon_num);
void 	__fastcall__ bank_breath(int8u typ, int8u y, int8u x, int8u dam_hp, char *ddesc, int8u monptr, int8u bank);
void 	__fastcall__ bank_build_type1(int8u yval, int8u xval);
void 	__fastcall__ bank_build_type2(int8u yval, int8u xval);
void 	__fastcall__ bank_build_type3(int8u yval, int8u xval);
int8u 	__fastcall__ bank_build_wall(int8u dir, int8u y, int8u x);
void 	__fastcall__ bank_calc_bonuses(int8u bank);
void 	__fastcall__ bank_calc_hitpoints(int8u bank);
void 	__fastcall__ bank_calc_mana(int8u stat, int8u bank);
void 	__fastcall__ bank_calc_spells(int8u stat, int8u bank);
void 	__fastcall__ bank_carry(int8u y, int8u x, int8u pickup, int8u bank);
void	__fastcall__ bank_cast(void);
void 	__fastcall__ bank_change_name(int8u bank);
void 	__fastcall__ bank_check_strength(int8u bank);
void 	__fastcall__ bank_check_view(int8u bank);
void 	__fastcall__ bank_chest_trap(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_clear_from(unsigned char y, int8u bank);
int8u 	__fastcall__ bank_clone_monster(int8u dir, int8u y, int8u x);
void 	__fastcall__ bank_closeobject(void);
void 	__fastcall__ bank_cold_dam(int8u dam, char *kb_str, int8u bank);
int8u 	__fastcall__ bank_compact_monsters(int8u bank);
void 	__fastcall__ bank_compact_objects(int8u bank);
void 	__fastcall__ bank_compact_quests(void);
int8u 	__fastcall__ bank_confuse_monster(int8u dir, int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_corrode_gas(char *kb_str, int8u bank);
void 	__fastcall__ bank_count_msg_print(char *p, int8u bank);
void 	__fastcall__ bank_create_character(void);
void 	__fastcall__ bank_create_food(int8u bank);
void    __fastcall__ bank_creatures(int8u attack, int8u bank);
int8u 	__fastcall__ bank_critical_blow(int16u weight, int8u plus, int8u dam, int8u attack_type, int8u bank);
int8u   __fastcall__ bank_dec_stat(int8u stat, int8u bank);
int8u	__fastcall__ bank_delete_object(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_delete_monster(int8u j, int8u bank);
void 	__fastcall__ bank_desc_remain(int8u item_val, int8u bank);
void 	__fastcall__ bank_destroy_area(int8u y, int8u x, int8u bank);
int8u 	__fastcall__ bank_detect_evil(int8u bank);
int8u 	__fastcall__ bank_detect_invisible(int8u bank);
int8u 	__fastcall__ bank_detect_monsters(void);
int8u 	__fastcall__ bank_detect_treasure(int8u bank);
int8u 	__fastcall__ bank_detect_object(int8u bank);
int8u 	__fastcall__ bank_detect_trap(int8u bank);
int8u 	__fastcall__ bank_detect_sdoor(int8u bank);
int8u 	__fastcall__ bank_disarm_all(int8u dir, int8u y, int8u x);
void 	__fastcall__ bank_disarm_trap(void);
int8u 	__fastcall__ bank_dispel_creature(int8u cflag, int8u damage, int8u bank);
void 	__fastcall__ bank_display_char(int8u bank);
void 	__fastcall__ bank_display_help(void);
void    __fastcall__ bank_display_scores(void);
void 	__fastcall__ bank_display_store(int8u store_num, int8u cur_top, int8u bank);
void 	__fastcall__ bank_display_wizard_help(void);
void    __fastcall__ bank_disturb(int8u s, int8u l, int8u bank);
int8u 	__fastcall__ bank_drain_life(int8u dir, int8u y, int8u x);
void 	__fastcall__ bank_dungeon_check_flags(void);
void 	__fastcall__ bank_earthquake(int8u bank);
void 	__fastcall__ bank_eat(void);
void 	__fastcall__ bank_end_find(int8u bank);
void 	__fastcall__ bank_enter_guild(void);
void 	__fastcall__ bank_enter_store(int8u store_num);
void	__fastcall__ bank_enter_tavern(void);
int8u 	__fastcall__ bank_enter_wiz_mode(void);
void 	__fastcall__ bank_eval_quest_mon(int16u mon, int8u bank);
void 	__fastcall__ bank_eval_quest_level(int8u level, int8u bank);
void 	__fastcall__ bank_eval_quest_item(int8u bank);
void 	__fastcall__ bank_examine_book(void);
void 	__fastcall__ bank_exit_game(void);
void 	__fastcall__ bank_find_init(int8u dir, int8u bank);
int8u 	__fastcall__ bank_find_range(int8u item1, int8u item2, int8u *j, int8u *k, int8u bank);
void 	__fastcall__ bank_find_run(void);
void 	__fastcall__ bank_fire_ball(int8u typ, int8u dir, int8u y, int8u x, int8u dam_hp, char *descrip, int8u bank);
void    __fastcall__ bank_fire_bolt(int8u typ, int8u dir, int8u y, int8u x, int8u dam, char *bolt_typ, int8u bank);
void 	__fastcall__ bank_fire_dam(int8u dam, char *kb_str, int8u bank);
void    __fastcall__ bank_fix1_delete_monster(int8u j, int8u bank);
void    __fastcall__ bank_fix2_delete_monster(int8u j, int8u bank);
void 	__fastcall__ bank_gain_spells(void);
void 	__fastcall__ bank_generate_cave(int8u bank);
int8u 	__fastcall__ bank_genocide(int8u bank);
void 	__fastcall__ bank_get_all_stats(void);
int8u	__fastcall__ bank_get_com(char *prompt, char *command, int8u bank);
int8u 	__fastcall__ bank_get_item(int8u *com_val, char *pmt, int8u i, int8u j, char *mask, char *message, int8u bank);
int16u 	__fastcall__ bank_get_mons_num(int8u level, int8u bank);
void 	__fastcall__ bank_get_moves(int8u monptr, int8u *mm);
int16u  __fastcall__ bank_get_obj_num(int8u level, int8u bank);
int8u 	__fastcall__ bank_get_string(char *in_str, int8u row, int8u column, int8u slen, int8u bank);
void 	__fastcall__ bank_go_down(void);
void 	__fastcall__ bank_go_up(void);
void 	__fastcall__ bank_hit_trap(int8u y, int8u x, int8u bank);
int8u 	__fastcall__ bank_hp_monster(int8u dir, int8u y, int8u x, int8 dam, int8u bank);
void 	__fastcall__ bank_ident_char(void);
int8u 	__fastcall__ bank_ident_spell(int8u bank);
void 	__fastcall__ bank_identify(int8u *item, int8u bank);
void 	__fastcall__ bank_init_calc_spells_prayers(void);
int8u 	__fastcall__ bank_inven_carry(inven_type *i_ptr, int8u bank);
void 	__fastcall__ bank_inven_command(char command, int8u bank);
void 	__fastcall__ bank_inven_destroy(int8u item_val, int8u bank);
void 	__fastcall__ bank_inven_drop(int8u item_val, int8u drop_all, int8u bank);
int32 	__fastcall__ bank_item_value(inven_type *i_ptr, int8u bank);
void 	__fastcall__ bank_jamdoor(void);
int8u 	__fastcall__ bank_light_area(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_light_dam(int8u dam, char *kb_str, int8u bank);
void 	__fastcall__ bank_light_line(int8u dir, int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_light_room(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_look(void);
void    __fastcall__ bank_lose_exp(int32 amount, int8u bank);
void 	__fastcall__ bank_magic_treasure(int8u x, int8u level, int8u bank);
void 	__fastcall__ bank_magic_treasure_2(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed);
void    __fastcall__ bank_make_attack(int8u monptr);
void 	__fastcall__ bank_map_area(int8u bank);
int8u 	__fastcall__ bank_mass_genocide(int8u bank);
int8u 	__fastcall__ bank_mass_poly(void);
void 	__fastcall__ bank_mon_cast_spell(int8u monptr, int8u *took_turn, int8u bank);
int8    __fastcall__ bank_mon_take_hit(int8u monptr, int16u dam, int8u bank);
int32u 	__fastcall__ bank_monster_death(int8u y, int8u x, int32u flags, int8u bank);
void 	__fastcall__ bank_move_char(int8u dir, int8u do_pickup, int8u bank);
void 	__fastcall__ bank_move_light(int8u y1, int8u x1, int8u y2, int8u x2, int8u bank);
void 	__fastcall__ bank_msg_print(char *str_buff, int8u bank);
int8u 	__fastcall__ bank_multiply_monster(int8u y, int8u x, int16u cr_index, int8u monptr, int8u bank);
void 	__fastcall__ bank_objdes(char *out_val, inven_type *i_ptr, int8u pref, int8u bank);
void 	__fastcall__ bank_openobject(void);
void 	__fastcall__ bank_place_door(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_place_gold(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_place_locked_door(int8u y, int8u x, int8u bank);
int8u 	__fastcall__ bank_place_monster(int8u y, int8u x, int16u z, int8u slp, int8u bank);
void 	__fastcall__ bank_place_object(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_place_rubble(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_place_secret_door(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_place_stairs(int8u typ, int8u num, int8u walls, int8u bank);
void 	__fastcall__ bank_place_streamer(int8u fval, int8u treas_chance);
void 	__fastcall__ bank_place_trap(int8u y, int8u x, int8u subval, int8u bank);
void 	__fastcall__ bank_place_updown_stairs(int8u y, int8u x, int8u dir, int8u bank);
void 	__fastcall__ bank_place_win_monster(void);
int8u   __fastcall__ bank_player_saves(int8u bank);
void 	__fastcall__ bank_poison_gas(int8u dam, char *kb_str, int8u bank);
int8u 	__fastcall__ bank_poly_monster(int8u dir, int8u y, int8u x, int8u bank);
void	__fastcall__ bank_pray(void);
void 	__fastcall__ bank_pretty_print(char *text, int8u line, int8u bank);
void 	__fastcall__ bank_prep_cave(void);
void 	__fastcall__ bank_print(char ch, int8u row, int8u col, int8u bank);
void 	__fastcall__ bank_print_self(int8u row, int8u col, int8u bank);
void    __fastcall__ bank_print_spells(int8u *spell, int8u num, int8u comment, int8 nonconsec, int8u bank);
void 	__fastcall__ bank_print_symbol(int8u row, int8u col, int8u bank);
void 	__fastcall__ bank_print_symbol_highlight(int8u row, int8u col, int8u bank);
int8u 	__fastcall__ bank_protect_evil(int8u bank);
void 	__fastcall__ bank_prt(char *str_buff, int8u row, int8u col, int8u bank);
void 	__fastcall__ bank_prt_afraid(int8u bank);
void 	__fastcall__ bank_prt_blind(int8u bank);
void 	__fastcall__ bank_prt_chp(int8u bank);
void 	__fastcall__ bank_prt_cmana(int8u bank);
void 	__fastcall__ bank_prt_comment1(int8u bank);
void 	__fastcall__ bank_prt_comment4(int8u bank);
void 	__fastcall__ bank_prt_confused(int8u bank);
void 	__fastcall__ bank_prt_depth(int8u bank);
void 	__fastcall__ bank_prt_depth(int8u bank);
void 	__fastcall__ bank_prt_experience(int8u bank);
void 	__fastcall__ bank_prt_gold(int8u bank);
void 	__fastcall__ bank_prt_hunger(int8u bank);
void 	__fastcall__ bank_prt_level(int8u bank);
void 	__fastcall__ bank_prt_map(int8u bank);
void 	__fastcall__ bank_prt_mhp(int8u bank);
void 	__fastcall__ bank_prt_pac(int8u bank);
void 	__fastcall__ bank_prt_poisoned(int8u bank);
void 	__fastcall__ bank_prt_speed (int8u bank);
void 	__fastcall__ bank_prt_stat(int8u stat, int8u bank);
void 	__fastcall__ bank_prt_stat_block(int8u bank);
void 	__fastcall__ bank_prt_state(int8u bank);
void 	__fastcall__ bank_prt_study(int8u bank);
void 	__fastcall__ bank_prt_title(int8u bank);
void 	__fastcall__ bank_prt_winner(int8u bank);
int8u 	__fastcall__ bank_purchase_haggle(int8u store_num, int32 *price, inven_type *item, int8u bank);
void 	__fastcall__ bank_put_character(void);
void 	__fastcall__ bank_put_misc1(void);
void 	__fastcall__ bank_put_misc2(void);
void 	__fastcall__ bank_put_misc3(void);
void 	__fastcall__ bank_put_stats(void);
void    __fastcall__ bank_py_attack(int8u y, int8u x, int8u bank);
void 	__fastcall__ bank_py_bonuses(inven_type *t_ptr, int8 factor, int8u bank);
void	__fastcall__ bank_quaff(void);
int8u 	__fastcall__ bank_quest_start(int8u chance);
void 	__fastcall__ bank_quest_end(void);
void 	__fastcall__ bank_random_object(int8u y, int8u x, int16u num, int8u bank);
void 	__fastcall__ bank_read_scroll(void);
int8u 	__fastcall__ bank_recharge(int8u num, int8u bank);
void 	__fastcall__ bank_refill_lamp(void);
int8u 	__fastcall__ bank_remove_curse(int8u bank);
void 	__fastcall__ bank_rest(void);
char 	__fastcall__ bank_roff_recall(int16u mon_num, int8u bank);
void 	__fastcall__ bank_screen_map(int8u bank);
void 	__fastcall__ bank_scribe_object(void);
void 	__fastcall__ bank_search(int8u y, int8u x, int8u chance, int8u bank);
int8u 	__fastcall__ bank_sell_haggle(int8u store_num, int32 *price, inven_type *item, int8u bank);
int32 	__fastcall__ bank_sell_price(int8u snum, int32 *max_sell, int32 *min_sell, inven_type *item, int8u bank);
void 	__fastcall__ bank_set_options(void);
void 	__fastcall__ bank_set_use_stat(int8u stat, int8u bank);
void 	__fastcall__ bank_show_equip(int8u weight, int8u bank);
void 	__fastcall__ bank_show_quests(int8u bank);
int8u 	__fastcall__ bank_sleep_monster(int8u dir, int8u y, int8u x, int8u bank);
int8u 	__fastcall__ bank_sleep_monsters1(int8u y, int8u x, int8u bank);
int8u 	__fastcall__ bank_sleep_monsters2(int8u bank);
int8u 	__fastcall__ bank_speed_monster(int8u dir, int8u y, int8u x, int8 spd, int8u bank);
int8u 	__fastcall__ bank_speed_monsters(int8u spd);
void 	__fastcall__ bank_starlite(int8u y, int8u x);
void 	__fastcall__ bank_store_carry(int8u store_num, int8 *ipos, inven_type *t_ptr, int8u bank);
void 	__fastcall__ bank_store_destroy(int8u store_num, int8u item_val, int8u one_of, int8u bank);
void 	__fastcall__ bank_store_maint(int8u bank);
int8u 	__fastcall__ bank_summon_monster(int8u *y, int8u *x, int8u slp, int8u bank);
int8u 	__fastcall__ bank_summon_undead(int8u *y, int8u *x, int8u bank);
void 	__fastcall__ bank_takeoff(int8u item_val, int8 posn, int8u bank);
int8u 	__fastcall__ bank_tavern_gossip(int8u chance, int8u barkeep);
void 	__fastcall__ bank_tavern_init(void);
int8u 	__fastcall__ bank_td_destroy(int8u bank);
int8u 	__fastcall__ bank_td_destroy2(int8u dir, int8u y, int8u x);
void 	__fastcall__ bank_teleport(int8u dis, int8u bank);
void    __fastcall__ bank_teleport_away(int8u monptr, int8u dis, int8u bank);
int8u 	__fastcall__ bank_teleport_monster(int8u dir, int8u y, int8u x, int8u bank);
void    __fastcall__ bank_throw_object(void);
int8u 	__fastcall__ bank_tot_dam(inven_type *i_ptr, int8u tdam, int16u monster, int8u bank);
void 	__fastcall__ bank_town_gen(void);
void	__fastcall__ bank_turn_undead(void);
int8u 	__fastcall__ bank_twall(int8u y, int8u x, int8u t1, int8u t2, int8u bank);
int8u 	__fastcall__ bank_unlight_area(int8u y, int8u x, int8u bank);
void    __fastcall__ bank_update_mon(int8 monptr, int8u bank);
void 	__fastcall__ bank_use(void);
void 	__fastcall__ bank_vault_monster(int8u y, int8u x, int8u num);
void 	__fastcall__ bank_vault_trap(int8u y, int8u x, int8u yd, int8u xd, int8u num);
void 	__fastcall__ bank_vbxe_clear(int8u bank);
void 	__fastcall__ bank_vbxe_cleartoeol(unsigned char x, unsigned char y, int8u bank);
void 	__fastcall__ bank_vbxe_cputcxy(int8u x, int8u y, char c, int8u atr, int8u bank);
void 	__fastcall__ bank_vbxe_cputsxy(unsigned char x, unsigned char y, char *str, unsigned char atr, int8u bank);
void 	__fastcall__ bank_vbxe_restorescreen(int8u bank);
void 	__fastcall__ bank_vbxe_savescreen(int8u bank);
int8u 	__fastcall__ bank_wall_to_mud(int8u dir, int8u y, int8u x, int8u bank);
void    __fastcall__ bank_warding_glyph(int8u bank);
void 	__fastcall__ bank_where_locate(void);
void 	__fastcall__ bank_wizard_commands(char com_val);
void 	__fastcall__ bank_wizard_display_player(void);


#endif
