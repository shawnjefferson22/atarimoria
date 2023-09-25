/* source/variable.c: Global variables */

//char *copyright[5] = {
//"Copyright (c) 1989-92 James E. Wilson, Robert A. Keoneke",
//"",
//"This software may be copied and distributed for educational, research, and",
//"not for profit purposes provided that this copyright and statement are",
//"included in all such copies."};

#include <stdio.h>

#include "config.h"
#include "constant.h"
#include "types.h"


//* This code is placed in low memory *//
#pragma rodata-name("RODATA")
#pragma data-name("DATA")
#pragma bss-name("BSS")


/* DEBUGGING FIXME */
char dbgstr[80];

/* Joystick to Rogue directions lookup table */
								    //                      N   U   L       B   Y   H       J   K
							        //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
const int8u joystick_roguedir[16] = {  0,  0,  0,  0,  0,  3,  9,  6,  0,  1,  7,  4,  0,  2,  8,  0};
const char joystick_command[16]   = {' ',' ',' ',' ',' ','n','u','l',' ','b','y','h',' ','j','k',' '};

/* Strings storage in main memory, used in place of stack storage in various functions -SJ */
bigvtype out_val1, out_val2;				// moved here from functions below (placed in BSS in lowram)
char msg_print_str[VTYPESIZ];				// buffer array in BSS for message print functions (used in bankcode mainly)

/* Save the store's last increment value.  */
int16 last_store_inc;

/* a horrible hack: needed because compact_monster() can be called from
   creatures() via summon_monster() and place_monster() */
int8 hack_monptr = -1;

int8u weapon_heavy = FALSE;
int8u pack_heavy = FALSE;
vtype died_from;				/* What killed us */

int8u total_winner = FALSE;
int32u max_score = 0;
int16u randes_seed;             /* for restarting randes_state */
int16u town_seed;               /* for restarting town_seed */
//int8u cur_height,cur_width;     /* Cur dungeon size    */				// moved into zeropage -SJ
//int8u dun_level = 0;            /* Cur dungeon level   */				// moved into zeropage -SJ
int16 missile_ctr = 0;          /* Counter for missiles */
int8u msg_flag;                 /* Set with first msg  */
vtype old_msg[MAX_SAVE_MSG];    /* Last message       */
int8u last_msg = 0;             /* Where last is held. Used to be int16u, but shouldn't go higher than 160 (vtypesize) -SJ */
int8u death = FALSE;            /* True if died       */
int8u find_flag;                /* Used in MORIA for .(dir) */
int8u free_turn_flag;           /* Used in MORIA, do not move creatures  */
int8u command_count;            /* Gives repetition of commands. -CJS- */
int8u default_dir = FALSE;      /* Use last direction for repeated command */
int32 turn = -1;                /* Cur turn of game    */
int8u noscore = FALSE;          /* Don't log the game. -CJS- */
int8u character_generated = FALSE;
int8u character_saved = FALSE;
int8u wizard = FALSE;			/* Wizard mode */

int32u spell_learned = 0;       /* bit mask of spells learned */
int32u spell_worked = 0;        /* bit mask of spells tried and worked */
int32u spell_forgotten = 0;     /* bit mask of spells learned but forgotten */
int8u spell_order[32];          /* order spells learned/remembered/forgotten */

/* options set via the '=' command */
int8u find_cut = TRUE;
int8u find_examine = TRUE;
int8u find_bound = FALSE;
int8u find_prself = FALSE;
int8u prompt_carry_flag = FALSE;
int8u show_weight_flag = FALSE;
int8u highlight_seams = FALSE;
int8u find_ignore_doors = FALSE;
//int8u sound_beep_flag = TRUE;
int8u display_counts = TRUE;
int8u classic_moria = TRUE;

int8u doing_inven = FALSE;      /* Track inventory commands. -CJS- */
int8u screen_change = FALSE;    /* Track screen updates for inven_commands. */
char last_command = ' ';        /* Memory of previous command. */

/*  Following are calculated from max dungeon sizes             */
//int8u max_panel_rows, max_panel_cols;						// used to be int16 -SJ
//int8 panel_row, panel_col;      						    // needs to be int8, intialized to -1 in dungeon1
//int8 panel_row_min, panel_row_max;		   			    // and code in dungeon3 get_panel assumes unsigned
//int16 panel_col_min, panel_col_max;
int16 panel_col_prt;
int8 panel_row_prt;

/* these used to be in dungeon.c */
int8u new_level_flag;           /* Next level when true  */
int8u teleport_flag;            /* Handle teleport traps  */
//int8u player_light;             /* Player carrying light  in zp now -SJ*/
int8u light_flag = FALSE;       /* Track if temporary light about player.  */

/* from get_dir.c */
int8u prev_dir;					/* Direction memory. -CJS- */

/* Guild Variables */
int8u guild_member = FALSE;		/* Member of the guild? -SJ*/

/* Quest Variables */


/* The cave is in a different memory area, is not automatically cleared */
/* Will actually reside in VBXE memory */
#pragma bss-name("CAVERAM")

cave_type cave[MAX_HEIGHT][MAX_WIDTH];	// main cave structure
int16u crow_lookup[MAX_HEIGHT];			// cave row starting address lookup (built at init)


#pragma bss-name("GENRAM")

recall_type c_recall[MAX_CREATURES];    	/* Monster memories */		// 20 x 279 bytes = 5580 bytes 0x15CC (probably won't use -SJ)

int8u score_entries_ram;			        /* Number of high score entries */
high_scores scores_ram[SCOREFILE_SIZE];		/* High Scores in main RAM      */

char text[5 * 76];							// RAM for text to be printed, 5 lines, 384 bytes, used by pretty print function -SJ


#pragma bss-name("HIGHSCORES")

int8u score_entries_cart;			        /* Number of high score entries */
high_scores scores_cart[SCOREFILE_SIZE];	/* High Scores in cart bank     */

#pragma bss-name("RECALLSAVE")

recall_type save_c_recall[MAX_CREATURES];	/* Monster memories */

#pragma bss-name("CHARSAVE0")
#pragma data-name("CHARSAVE0")

int8u save_find_cut;
int8u save_find_examine;
int8u save_find_bound;
int8u save_find_prself;
int8u save_prompt_carry_flag;
int8u save_show_weight_flag;
int8u save_highlight_seams;
int8u save_find_ignore_doors;
int8u save_display_counts;
int8u save_classic_moria;

int8u save_death = 1;						// make player initially dead
int8u save_total_winner;
int8u save_noscore;
int32 save_turn;
int16u save_randes_seed;
int16u save_town_seed;
int16 save_missile_ctr;
vtype save_old_msg[MAX_SAVE_MSG];
int8u save_last_msg;
vtype save_died_from;
int32 save_max_score;

player_type save_py;
int16u save_player_hp[MAX_PLAYER_LEVEL];

int8u save_inven_ctr;
int16u save_inven_weight;
int8u save_equip_ctr;
inven_type save_inventory[INVEN_ARRAY_SIZE];
int8u save_object_ident[OBJECT_IDENT_SIZE];

int32u save_spell_learned;
int32u save_spell_worked;
int32u save_spell_forgotten;
int8u save_spell_order[32];

int8u save_guild_member;

quest_type save_quests[QUEST_SLOTS];


#pragma bss-name("CHARSAVE1")

store_type save_store[MAX_STORES];


#pragma bss-name("GAMESAVE0")

char save_cave0[0x2000];			// 44x66x4 = 0x2D60

#pragma bss-name("GAMESAVE1")

char save_cave1[0x2000];

#pragma bss-name("GAMESAVE2")

char save_cave2[0x1AC0];

int8u save_dun_level;
int8u save_max_panel_rows, save_max_panel_cols;
int8u save_cur_height, save_cur_width;
int8u save_char_row;
int8u save_char_col;

#pragma bss-name("GAMESAVE3")

int8u save_tcptr;
inven_type save_t_list[MAX_TALLOC];

#pragma bss-name("GAMESAVE4")

int8u save_mfptr;
int8u save_mon_tot_mult;
monster_type save_m_list[MAX_MALLOC];
