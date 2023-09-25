/* source/dungeon.c: the main command interpreter, updating player status

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>



/* Put code, rodata and data in Dungeon1 bank */
#pragma code-name("DUNGEON1BANK")
#pragma rodata-name("DUNGEON1BANK")
#pragma data-name("DUNGEON1BANK")

 /* Command Count Lookup table, replaces valid_countcommand function */
						//  00   C-A   C-B   C-C   C-D   C-E   C-F   C-G   C-H   C-I   C-J   C-K   C-L   C-M   C-N   C-O
char valid_cc[128] =     { 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00,   // 15
  						//  C-P   C-Q   C-R   C-S   C-T   C-U   C-V   C-W   C-X   C-Y   C-Z   ESC   U-A   D-A   L-A   R-A
  						   0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,   // 31
  						//  SPC   !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
  						   0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,   // 47
  						//  0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
  						   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,   // 63
  						//  @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
  						   0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x31, 0x01, 0x01, 0x01, 0x31, 0x01, 0x31,   // 79
  						//  P     Q     R     S     T     U     V     W     X     Y     Z	  [     \     ]     ^     _
  						   0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // 95
  						//  DIA   a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
  						   0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01,   // 111
  						//  p     q     r     s     t     u     v     w     x     y     z     SPD   |     ARR   TRI  TRI
						   0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // 127


/* Prototypes of functions copied into this bank */
void __fastcall__ dungeon1_put_buffer(char *out_str, int8u row, int8u col);
void __fastcall__ dungeon1_erase_line(int8u row, int8u col);
void __fastcall__ dungeon1_pause_line(int8u prt_line);
int8u __fastcall__ dungeon1_get_check(char *prompt);
void __fastcall__ dungeon1_disturb(int8u s, int8u l);
void __fastcall__ dungeon1_rest_off(void);
void __fastcall__ dungeon1_search_on(void);
void __fastcall__ dungeon1_search_off(void);
void __fastcall__ dungeon1_change_speed(int8 num);
int8u __fastcall__ dungeon1_get_com(char *prompt, char *command);
char __fastcall__ dungeon1_map_roguedir(char comval);
int8u __fastcall__ dungeon1_get_dir(char *prompt, int8u *dir);
int8u __fastcall__ dungeon1_mmove(int8u dir, int8u *y, int8u *x);
void __fastcall__ dungeon1_lite_spot(int8u y, int8u x);
int8u __fastcall__ dungeon1_test_light(int8u y, int8u x);
void __fastcall__ do_command(char com_val);
//int8u __fastcall__ valid_countcommand(char c);
void __fastcall__ tunnel(int8u dir);
int8u __fastcall__ twall(int8u y, int8u x, int8u t1, int8u t2);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN1NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon1


void __fastcall__ dungeon1_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

void __fastcall__ dungeon1_put_buffer(char *out_str, int8u row, int8u col)
#include "inbank\put_buffer.c"

void __fastcall__ dungeon1_pause_line(int8u prt_line)
#include "inbank\pause_line.c"

int8u __fastcall__ dungeon1_get_check(char *prompt)
#include "inbank\get_check.c"

void __fastcall__ dungeon1_disturb(int8u s, int8u l)
#include "inbank\disturb.c"


/* Search Mode enhancement -RAK- */
/* Only used in this bank  -SJ */
void __fastcall__ dungeon1_search_on(void)
{
  dungeon1_change_speed(1);
  py.flags.status |= PY_SEARCH;
  bank_prt_state(DUN1NUM);
  bank_prt_speed(DUN1NUM);
  py.flags.food_digested++;
}

void __fastcall__ dungeon1_search_off(void)
#include "inbank\search_off.c"

void __fastcall__ dungeon1_rest_off(void)
#include "inbank\rest_off.c"

void __fastcall__ dungeon1_change_speed(int8 num)
#include "inbank\change_speed.c"

int8u __fastcall__ dungeon1_get_com(char *prompt, char *command)
#include "inbank\get_com.c"

char __fastcall__ dungeon1_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

int8u __fastcall__ dungeon1_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

int8u __fastcall__ dungeon1_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

void __fastcall__ dungeon1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ dungeon1_test_light(int8u y, int8u x)
#include "inbank\test_light.c"


/* Moria game module					-RAK-					 */
/* The code in this section has gone through many revisions, and */
/* some of it could stand some more hard work.	-RAK-	       	 */
/* It has had a bit more hard work.			-CJS- 				 */

void __fastcall__ dungeon(void)
{
  register int8u find_count, i;
  char command;						/* Last command		 */
  register inven_type *i_ptr;
  char tmp[8];
  int8u last_joy = FALSE;			// last command was joystick direction movement
  int8u rtclok0_save = 128;			// last value of rtclok0


  /* Main procedure for dungeon.			-RAK-	*/
  /* Note: There is a lot of preliminary magic going on here at first*/

  /* Check light status for setup */
  vbxe_bank(VBXE_OBJBANK);						// need objbank for inventory -SJ
  i_ptr = &inventory[INVEN_LIGHT];
  if (i_ptr->p1 > 0)							// i_ptr is in objbank -SJ
    player_light = TRUE;
  else
    player_light = FALSE;

  /* Check for a maximum level */
  if (dun_level > py.misc.max_dlv)
    py.misc.max_dlv = dun_level;

  /* Reset flags and initialize variables  */
  command_count = 0;
  find_count = 0;
  new_level_flag = FALSE;
  find_flag	= FALSE;
  teleport_flag = FALSE;
  mon_tot_mult = 0;

  vbxe_bank(VBXE_CAVBANK);						// need cavebank for this -SJ
  cave[char_row][char_col].cptr = 1;			// put us in this cave space -SJ

  /* Ensure we display the panel. Used to do this with a global var. -CJS- */
  panel_row = panel_col = -1;					// forces a new panel

  /* Light up the area around character	   */
  bank_check_view(DUN1NUM);

  /* must do this after panel_row/col set to -1, because search_off() will
     call check_view(), and so the panel_* variables must be valid before
     search_off() is called */
  if (py.flags.status & PY_SEARCH)
    dungeon1_search_off();
  /* Light,  but do not move critters */
  bank_creatures(FALSE, DUN1NUM);
  /* Print the depth */
  bank_prt_depth(DUN1NUM);

  /* Loop until dead,  or new level		*/
  do {
    /* Increment turn counter			*/
    ++turn;

    /* turn over the store contents every, say, 1000 turns */
    if ((dun_level != 0) && ((turn % STORE_MAINT_FREQ) == 0))
	  bank_store_maint(DUN1NUM);

    /* Check for creature generation		*/
    if (randint(MAX_MALLOC_CHANCE) == 1)
	  bank_alloc_monster(1, MAX_SIGHT, FALSE, DUN1NUM);

    /* Check light status */
    vbxe_bank(VBXE_OBJBANK);					// need objbank for this -SJ
    i_ptr = &inventory[INVEN_LIGHT];
    if (player_light)
	  if (i_ptr->p1 > 0) {
	    i_ptr->p1--;
	    if (i_ptr->p1 == 0) {
		  player_light = FALSE;
		  bank_msg_print("Your light has gone out!", DUN1NUM);
		  dungeon1_disturb(0, 1);
		  /* unlight creatures */
		  bank_creatures(FALSE, DUN1NUM);
	    }
	    else if ((i_ptr->p1 < 40) && (randint(5) == 1) && (py.flags.blind < 1)) {
		  dungeon1_disturb(0, 0);
		  bank_msg_print("Your light is growing faint.", DUN1NUM);
	    }
	  }
	  else {
	    player_light = FALSE;
	    dungeon1_disturb(0, 1);
	    /* unlight creatures */
	    bank_creatures(FALSE, DUN1NUM);
	  }
      else if (i_ptr->p1 > 0) {
	    i_ptr->p1--;
	    player_light = TRUE;
	    dungeon1_disturb(0, 1);
	    /* light creatures */
	    bank_creatures(FALSE, DUN1NUM);
	  }

	/* Update counters and messages */
	bank_dungeon_check_flags();

    /* Check the state of the monster list, and delete some monsters if
	   the monster list is nearly full.  This helps to avoid problems in
	   creature.c when monsters try to multiply.  Compact_monsters() is
	   much more likely to succeed if called from here, than if called
	   from within creatures().  */
    if (MAX_MALLOC - mfptr < 10)
	  bank_compact_monsters(DUN1NUM);

    if ((py.flags.paralysis < 1) &&	     /* Accept a command?     */
	    (py.flags.rest == 0) && (!death)) {
	    /* Accept a command and execute it */
	  do {
	    if (py.flags.status & PY_REPEAT)
		  bank_prt_state(DUN1NUM);
	    default_dir = FALSE;
	    free_turn_flag = FALSE;

	    if (find_flag) {
		  bank_find_run();
		  --find_count;
		  if (find_count == 0)
		    bank_end_find(DUN1NUM);
		}
	    else if (doing_inven)
		  bank_inven_command(doing_inven, DUN1NUM);
	    else {
		  /* move the cursor to the players character */
		  if (command_count > 0) {
		    msg_flag = FALSE;
		    default_dir = TRUE;
		  }
		  else {
		    msg_flag = FALSE;

			/* Joystick movement -SJ */
		    while (!kbhit()) {								// While no key pressed
			  if (!last_joy) {								// debounce the joystick after typed commands
			    while (STICK0 != 15);
			    last_joy = FALSE;
		  	  }

			  while (abs(RTCLOK0 - rtclok0_save) < 3); 			// waste some time if we got back here too fast
			  rtclok0_save = RTCLOK0;						// save the current jiffy

			  if (STICK0 != 15) {							// if joystick moved
			    last_joy = TRUE;
				i = 0;										// reset counted command
			    command = joystick_command[STICK0];			// get the direction as command character
			    if (STRIG0 == 0)							// joystick button down does tunnel
			      command = CTRL(command);
				break;
		      }
			}

		    if (kbhit()) {									// User pressed a key -SJ
		      command = cgetc();
			  last_joy = FALSE;

		      i = 0;
		      /* Get a count for a command. */
		      if (command >= '0' && command <= '9') {			// only roguelike commands -SJ
			    bank_prt("Repeat count:", 0, 0, DUN1NUM);
			    if (command == '#')
			      command = '0';
			    i = 0;
			    while (TRUE) {
			      if (command == DELETE) {
			  	    i = i / 10;
			  	    sprintf(tmp, "%d", i);
				    bank_prt(tmp, 0, 14, DUN1NUM);
				  }
			      else if (command >= '0' && command <= '9') {
			        if (i > 99)
			          ;
				      //bell();
				    else {
				      i = i * 10 + command - '0';
				      sprintf(tmp, "%d", i);
				      bank_prt(tmp, 0, 14, DUN1NUM);
				    }
				  }
			      else
				    break;

		          command = cgetc();
			    }

			    if (i == 0) {
			      i = 99;
			      sprintf (tmp, "%d", i);
			      bank_prt(tmp, 0, 14, DUN1NUM);
			    }

			    /* a special hack to allow numbers as commands */
			    if (command == ' ') {
			      bank_prt("Command:", 0, 20, DUN1NUM);
			      command = cgetc();
			    }
			  }
		    }

		    /* Another way of typing control codes -CJS- */
		    /*if (command == '^') {
			  if (command_count > 0)
			    bank_prt_state(DUN1NUM);
			  if(dungeon1_get_com("Control-", &command)) {
			    if (command >= 'A' && command <= 'Z')
				  command -= 'A' - 1;
			    else if (command >= 'a' && command <= 'z')
				  command -= 'a' - 1;
			    else {
			      bank_msg_print("Type ^ <letter> for a control char", DUN1NUM);
				  command = ' ';
			    }
			  }
			  else
			    command = ' ';
		    }*/

		    if (i > 0) {
			  //if (!valid_countcommand(command)) {
			  if (!valid_cc[command]) {
			    free_turn_flag = TRUE;
			    bank_msg_print ("Invalid command with a count.", DUN1NUM);
			    command = ' ';
			  }
			  else {
			    command_count = i;
			    bank_prt_state(DUN1NUM);
			  }
			}
		  }

		  /* Flash the message line. */
		  dungeon1_erase_line(MSG_LINE, 0);

		  do_command(command);
		  /* Find is counted differently, as the command changes. */
		  if (find_flag) {
		    find_count = command_count - 1;
		    command_count = 0;
		  }
		  else if (free_turn_flag)
		    command_count = 0;
		  else if (command_count)
		    --command_count;
		}
	    /* End of commands				     */
	  } while (free_turn_flag && !new_level_flag);
	}

    /* Teleport?		       */
    if (teleport_flag)
      bank_teleport(100, DUN1NUM);

    /* Move the creatures	       */
    if (!new_level_flag)
      bank_creatures(TRUE, DUN1NUM);

    /* Exit when new_level_flag is set   */
  } while (!new_level_flag);
}


void __fastcall__ do_command(char com_val)
{
  int8u dir_val, do_pickup;
  //int8u y;
  int8u x, i, j;
  //vtype out_val, tmp_str;				// use out_val1 and out_val2 -SJ


  /* hack for move without pickup.  Map '-' to a movement command. */
  if (com_val == '-') {
    do_pickup = FALSE;
    i = command_count;
    if (dungeon1_get_dir(CNIL, &dir_val)) {
	  command_count = i;
	  switch (dir_val) {
	    case 1:    com_val = 'b';	 break;
	    case 2:    com_val = 'j';	 break;
	    case 3:    com_val = 'n';	 break;
	    case 4:    com_val = 'h';	 break;
	    case 6:    com_val = 'l';	 break;
	    case 7:    com_val = 'y';	 break;
	    case 8:    com_val = 'k';	 break;
	    case 9:    com_val = 'u';	 break;
	    default:   com_val = '~';	 break;
	  }
	}
    else
	  com_val = ' ';
  }
  else
    do_pickup = TRUE;

  switch(com_val) {
    case 'Q':			/* (Q)uit		(^K)ill */
      //flush();
      while(kbhit()) cgetc();											// flush code -SJ
      if (dungeon1_get_check("Do you really want to quit?")) {
	    new_level_flag = TRUE;
	    death = TRUE;
	    strcpy(died_from, "Quitting");
	  }
      free_turn_flag = TRUE;
      break;
    case CTRL('P'):		/* (^P)revious message. */
      if (command_count > 0) {
	    i = command_count;
	    if (i > MAX_SAVE_MSG)
	      i = MAX_SAVE_MSG;
	    command_count = 0;
	  }
      else if (last_command != CTRL('P'))
	    i = 1;
      else
	    i = MAX_SAVE_MSG;
      j = last_msg;
      if (i > 1) {
	    bank_vbxe_savescreen(DUN1NUM);
	    x = i;
	    while (i > 0) {
	      --i;
	      bank_prt(old_msg[j], i, 0, DUN1NUM);
	      if (j == 0)
		    j = MAX_SAVE_MSG-1;
	      else
		    --j;
	    }
	    dungeon1_erase_line (x, 0);
	    dungeon1_pause_line(x);
	    bank_vbxe_restorescreen(DUN1NUM);
	  }
      else {
	    /* Distinguish real and recovered messages with a '>'. -CJS- */
	    dungeon1_put_buffer(">", 0, 0);
	    bank_prt(old_msg[j], 0, 1, DUN1NUM);
	  }
      free_turn_flag = TRUE;
      break;
    case CTRL('W'):	/* (^W)izard mode */
      if (wizard) {
	    wizard = FALSE;
	    bank_msg_print("Wizard mode off.", DUN1NUM);
	  }
      else if (bank_enter_wiz_mode())
	    bank_msg_print("Wizard mode on.", DUN1NUM);
      bank_prt_winner(DUN1NUM);
      free_turn_flag = TRUE;
      break;
    case CTRL('X'):	/* e(^X)it and save */
      if (total_winner) {
       bank_msg_print("You are a Total Winner, your character must be retired.", DUN1NUM);
	   bank_msg_print("Use 'Q' when you are ready to quit.", DUN1NUM);
	  }
      else {
	    strcpy(died_from, "(saved)");
	    bank_msg_print ("Saving game...", DUN1NUM);
	    save_char();						// can't fail, in main RAM returns cart to this bank
	    character_saved = TRUE;				// character saved (don't show char stats and equipment)
	    bank_exit_game();					// doesn't return -SJ
	    //while(1);							// FIXME: back to init?
	  }
      free_turn_flag = TRUE;
      break;
    case '=':		/* (=) set options */
      bank_vbxe_savescreen(DUN1NUM);
      bank_set_options();
      bank_vbxe_restorescreen(DUN1NUM);
      free_turn_flag = TRUE;
      break;
    case '[':		/* ({) inscribe an object    */			// was { but doesn't exist on Atari
      bank_scribe_object();
      free_turn_flag = TRUE;
      break;
    case ESCAPE:	/* (ESC)   do nothing. */
    case ' ':		/* (space) do nothing. */
      free_turn_flag = TRUE;
      break;
    case 'b':		/* (b) down, left	(1) */
      bank_move_char(1, do_pickup, DUN1NUM);
      break;
    case 0x1D:
    case 'j':		/* (j) down		(2) */
      bank_move_char(2, do_pickup, DUN1NUM);
      break;
    case 'n':		/* (n) down, right	(3) */
      bank_move_char(3, do_pickup, DUN1NUM);
      break;
    case 0x1E:
    case 'h':		/* (h) left		(4) */
      bank_move_char(4, do_pickup, DUN1NUM);
      break;
    case 0x1F:
    case 'l':		/* (l) right		(6) */
      bank_move_char(6, do_pickup, DUN1NUM);
      break;
    case 'y':		/* (y) up, left		(7) */
      bank_move_char(7, do_pickup, DUN1NUM);
      break;
    case 0x1C:
    case 'k':		/* (k) up		(8) */
      bank_move_char(8, do_pickup, DUN1NUM);
      break;
    case 'u':		/* (u) up, right	(9) */
      bank_move_char(9, do_pickup, DUN1NUM);
      break;
    case 'B':		/* (B) run down, left	(. 1) */
      bank_find_init(1, DUN1NUM);
      break;
    case 'J':		/* (J) run down		(. 2) */
      bank_find_init(2, DUN1NUM);
      break;
    case 'N':		/* (N) run down, right	(. 3) */
      bank_find_init(3, DUN1NUM);
      break;
    case 'H':		/* (H) run left		(. 4) */
      bank_find_init(4, DUN1NUM);
      break;
    case 'L':		/* (L) run right	(. 6) */
      bank_find_init(6, DUN1NUM);
      break;
    case 'Y':		/* (Y) run up, left	(. 7) */
      bank_find_init(7, DUN1NUM);
      break;
    case 'K':		/* (K) run up		(. 8) */
      bank_find_init(8, DUN1NUM);
      break;
    case 'U':		/* (U) run up, right	(. 9) */
      bank_find_init(9, DUN1NUM);
      break;
    case '/':		/* (/) identify a symbol */
      bank_ident_char();
      free_turn_flag = TRUE;
      break;
    case '.':		/* (.) stay in one place (5) */
      bank_move_char(5, do_pickup, DUN1NUM);
      if (command_count > 1) {
	    --command_count;
	    bank_rest();
	  }
      break;
    case '<':		/* (<) go down a staircase */
      bank_go_up();
      break;
    case '>':		/* (>) go up a staircase */
      bank_go_down();
      break;
    case '?':		/* (?) help with commands */
      bank_vbxe_savescreen(DUN1NUM);
      bank_display_help();
      bank_vbxe_restorescreen(DUN1NUM);
      free_turn_flag = TRUE;
      break;
    case 'f':		/* (f)orce		(B)ash */
      bank_bash();
      break;
    case 'C':		/* (C)haracter description */
      bank_vbxe_savescreen(DUN1NUM);
      bank_change_name(DUN1NUM);
      bank_vbxe_restorescreen(DUN1NUM);
      free_turn_flag = TRUE;
      break;
    case 'D':		/* (D)isarm trap */
      bank_disarm_trap();
      break;
    case 'E':		/* (E)at food */
      bank_eat();
      break;
    case 'F':		/* (F)ill lamp */
      bank_refill_lamp();
      break;
    case 'G':		/* (G)ain magic spells */
      bank_gain_spells();
      break;
    case 'V':		/* (V)iew scores */
      if (last_command != 'V')
		i = TRUE;
      else
		i = FALSE;
      bank_vbxe_savescreen(DUN1NUM);
      bank_display_scores();
      bank_vbxe_restorescreen(DUN1NUM);
      free_turn_flag = TRUE;
      break;
    case 'W':		/* (W)here are we on the map	(L)ocate on map */
      bank_where_locate();
      free_turn_flag = TRUE;
      break;
    case 'R':		/* (R)est a while */
      bank_rest();
      break;
    case '#':		/* (#) search toggle	(S)earch toggle */
      if (py.flags.status & PY_SEARCH)
	    dungeon1_search_off();
      else
	    dungeon1_search_on();
      free_turn_flag = TRUE;
      break;
    case CTRL('B'):		/* (^B) tunnel down left	(T 1) */
      tunnel(1);
      break;
    case CTRL('J'):		/* (^J) tunnel down		(T 2) */
      tunnel(2);
      break;
    case CTRL('N'):		/* (^N) tunnel down right	(T 3) */
      tunnel(3);
      break;
    case CTRL('H'):		/* (^H) tunnel left		(T 4) */
      tunnel(4);
      break;
    case CTRL('L'):		/* (^L) tunnel right		(T 6) */
      tunnel(6);
      break;
    case CTRL('Y'):		/* (^Y) tunnel up left		(T 7) */
      tunnel(7);
      break;
    case CTRL('K'):		/* (^K) tunnel up		(T 8) */
      tunnel(8);
      break;
    case CTRL('U'):		/* (^U) tunnel up right		(T 9) */
      tunnel(9);
      break;
    case 'z':		/* (z)ap a wand		(a)im a wand */
      bank_aim();
      break;
    case 'M':
      bank_screen_map(DUN1NUM);
      free_turn_flag = TRUE;
      break;
    case 'P':		/* (P)eruse a book	(B)rowse in a book */
      bank_examine_book();
      free_turn_flag = TRUE;
      break;
    case 'c':		/* (c)lose an object */
      bank_closeobject();
      break;
    case 'd':		/* (d)rop something */
      bank_inven_command('d', DUN1NUM);
      break;
    case 'e':		/* (e)quipment list */
      bank_inven_command('e', DUN1NUM);
      break;
    case 't':		/* (t)hrow something	(f)ire something */
      bank_throw_object();
      break;
    case 'i':		/* (i)nventory list */
      bank_inven_command('i', DUN1NUM);
      break;
    case 'S':		/* (S)pike a door	(j)am a door */
      bank_jamdoor();
      break;
    case 'x':		/* e(x)amine surrounds	(l)ook about */
      bank_look();
      free_turn_flag = TRUE;
      break;
    case 'm':		/* (m)agic spells */
      bank_cast();
      break;
    case 'o':		/* (o)pen something */
      bank_openobject();
      break;
    case 'p':		/* (p)ray */
      bank_pray();
      break;
    case 'q':		/* (q)uaff */
      bank_quaff();
      break;
    case 'r':		/* (r)ead */
      bank_read_scroll();
      break;
    case 's':		/* (s)earch for a turn */
      bank_search(char_row, char_col, py.misc.srh, DUN1NUM);
      break;
    case 'T':		/* (T)ake off something	(t)ake off */
      bank_inven_command('t', DUN1NUM);
      break;
    case 'Z':		/* (Z)ap a staff	(u)se a staff */
      bank_use();
      break;
    case 'v':		/* (v)ersion of game */
      //helpfile(MORIA_VER);						// FIXME
      free_turn_flag = TRUE;
      break;
    case 'w':		/* (w)ear or wield */
      bank_inven_command('w', DUN1NUM);
      break;
    case 'X':		/* e(X)change weapons	e(x)change */
      bank_inven_command('x', DUN1NUM);
      break;
    case 'a':		/* Assess Quests */
      bank_show_quests(DUN1NUM);
      break;
    default:
      if (wizard) {
	    bank_wizard_commands(com_val);
	  }
      else {
	    bank_prt("Type '?' for help.", 0, 0, DUN1NUM);
	    free_turn_flag = TRUE;
      }
  } // end of switch statement

  last_command = com_val;
}


/* Check whether this command will accept a count.     -CJS-  */
/*int8u __fastcall__ valid_countcommand(char c)
{
  switch(c) {						// CHECK: maybe a lookup table?
    case 'Q':
    case CTRL('W'):
    case CTRL('X'):
    case '=':
    case '{':
    case '/':
    case '<':
    case '>':
    case '?':
    case 'C':
    case 'E':
    case 'F':
    case 'G':
    case 'V':
    case '#':
    case 'z':
    case 'P':
    case 'c':
    case 'd':
    case 'e':
    case 't':
    case 'i':
    case 'x':
    case 'm':
    case 'p':
    case 'q':
    case 'r':
    case 'T':
    case 'Z':
    case 'v':
    case 'w':
    case 'W':
    case 'X':
    case CTRL('A'):
    case '\\':
    case CTRL('I'):
    case '*':
    case ':':
    case CTRL('T'):
    case CTRL('E'):
    case CTRL('F'):
    case CTRL('S'):
    case CTRL('Q'):
      return FALSE;
    case CTRL('P'):
    case ESCAPE:
    case ' ':
    case '-':
    case 'b':
    case 'f':
    case 'j':
    case 'n':
    case 'h':
    case 'l':
    case 'y':
    case 'k':
    case 'u':
    case '.':
    case 'B':
    case 'J':
    case 'N':
    case 'H':
    case 'L':
    case 'Y':
    case 'K':
    case 'U':
    case 'D':
    case 'R':
    case CTRL('Y'):
    case CTRL('K'):
    case CTRL('U'):
    case CTRL('L'):
    case CTRL('N'):
    case CTRL('J'):
    case CTRL('B'):
    case CTRL('H'):
    case 'S':
    case 'o':
    case 's':
    case CTRL('D'):
    case CTRL('G'):
    case '+':
      return TRUE;
    default:
      return FALSE;
    }
}*/


/* Tunnels through rubble and walls			-RAK-	*/
/* Must take into account: secret doors,  special tools		  */
void __fastcall__ tunnel(int8u dir)
{
  register int8u i;
  register int8 tabil;
  //register cave_type *c_ptr;
  register inven_type *i_ptr;
  register monster_type *m_ptr;
  int8u y, x;
  //vtype out_val, m_name;
  vtype m_name;
  cave_type c;


  if ((py.flags.confused > 0) &&    /* Confused? */
      (randint(4) > 1))		        /* 75% random movement */
    dir = randint(9);

  y = char_row;
  x = char_col;
  dungeon1_mmove(dir, &y, &x);

  vbxe_bank(VBXE_CAVBANK);
  //c_ptr = &cave[y][x];
  //memcpy(&c, &cave[y][x], sizeof(cave_type));						// make a local copy of cave location
  memcpy(&c, CAVE_ADR(y, x), sizeof(cave_type));					// make a local copy of cave location


  /* Compute the digging ability of player; based on strength, and type of tool used */
  tabil = py.stats.use_stat[A_STR];
  vbxe_bank(VBXE_OBJBANK);									// need objbank for inventory
  i_ptr = &inventory[INVEN_WIELD];

  /* Don't let the player tunnel somewhere illegal, this is necessary to
     prevent the player from getting a free attack by trying to tunnel
     somewhere where it has no effect.  */
  if (c.fval < MIN_CAVE_WALL && (c.tptr == 0 || (t_list[c.tptr].tval != TV_RUBBLE && t_list[c.tptr].tval != TV_SECRET_DOOR))) {
    if (c.tptr == 0) {
	  bank_msg_print ("Tunnel through what?  Empty air?!?", DUN1NUM);
	  free_turn_flag = TRUE;
	}
    else {
	  bank_msg_print("You can't tunnel through that.", DUN1NUM);
	  free_turn_flag = TRUE;
	}
    return;
  }

  if (c.cptr > 1) {
    vbxe_bank(VBXE_MONBANK);									// need monster bank
    m_ptr = &m_list[c.cptr];
    if (m_ptr->ml)
      sprintf (m_name, "The %s", c_list[m_ptr->mptr].name);
    else
	  strcpy (m_name, "Something");
    sprintf(out_val1, "%s is in your way!", m_name);
    bank_msg_print(out_val1, DUN1NUM);

    /* let the player attack the creature */
    if (py.flags.afraid < 1)
	  bank_py_attack(y, x, DUN1NUM);
    else
	  bank_msg_print("You are too afraid!", DUN1NUM);
  }
  else if (i_ptr->tval != TV_NOTHING) {
    if (TR_TUNNEL & i_ptr->flags)
	  tabil += 25 + i_ptr->p1*50;
    else {
	  tabil += (i_ptr->damage[0]*i_ptr->damage[1]) + i_ptr->tohit + i_ptr->todam;
	  /* divide by two so that digging without shovel isn't too easy */
	  tabil >>= 1;
	}

    /* If this weapon is too heavy for the player to wield properly, then also make it harder to dig with it.  */
    if (weapon_heavy) {
	  tabil += (py.stats.use_stat[A_STR] * 15) - i_ptr->weight;
	  if (tabil < 0)
	    tabil = 0;
	}

    /* Regular walls; Granite, magma intrusion, quartz vein  */
    /* Don't forget the boundary walls, made of titanium (255)*/
    switch(c.fval) {
	  case GRANITE_WALL:
	    i = randint(1200) + 80;
	    if (twall(y, x, tabil, i))
	      bank_msg_print("You have finished the tunnel.", DUN1NUM);
	    else
	      bank_count_msg_print("You tunnel into the granite wall.", DUN1NUM);
	    break;
	  case MAGMA_WALL:
	    i = randint(600) + 10;
	    if (twall(y, x, tabil, i))
	      bank_msg_print("You have finished the tunnel.", DUN1NUM);
	    else
	      bank_count_msg_print("You tunnel into the magma intrusion.",DUN1NUM);
	    break;
	  case QUARTZ_WALL:
	    i = randint(400) + 10;
	    if (twall(y, x, tabil, i))
	      bank_msg_print("You have finished the tunnel.", DUN1NUM);
	    else
	      bank_count_msg_print("You tunnel into the quartz vein.", DUN1NUM);
	    break;
	  case BOUNDARY_WALL:
	    bank_msg_print("This seems to be permanent rock.", DUN1NUM);
	    break;
	  default:
	    /* Is there an object in the way?  (Rubble and secret doors)*/
	    if (c.tptr != 0) {  /* Rubble. */
	      if (t_list[c.tptr].tval == TV_RUBBLE) {
	        if (tabil > randint(180)) {
	          bank_delete_object(y, x, DUN1NUM);
	          bank_msg_print("You have removed the rubble.", DUN1NUM);
	          if (randint(10) == 1) {
	  	        bank_place_object(y, x, DUN1NUM);
			    if (dungeon1_test_light(y, x))
			      bank_msg_print("You have found something!", DUN1NUM);
			  }
		      dungeon1_lite_spot(y, x);
		    }
		    else
		      bank_count_msg_print("You dig in the rubble.", DUN1NUM);
		  }
	      /* Secret doors.*/
	      else if (t_list[c.tptr].tval == TV_SECRET_DOOR) {
		    bank_count_msg_print("You tunnel into the granite wall.", DUN1NUM);
		    bank_search(char_row, char_col, py.misc.srh, DUN1NUM);
		  }
	    }
	    break;
	}
  }
  else
    bank_msg_print("You dig with your hands, making no progress.", DUN1NUM);
}


/* Tunneling through real wall: 10, 11, 12		-RAK-	*/
/* Used by TUNNEL and WALL_TO_MUD				 */
int8u __fastcall__ twall(int8u y, int8u x, int8u t1, int8u t2)
{
  register int8u i, j;
  register cave_type *c_ptr;
  register int8u res, found;
  int8u fval, flags;


  res = FALSE;
  if (t1 > t2) {
    vbxe_bank(VBXE_CAVBANK);
    c_ptr = CAVE_ADR(y, x);

    if (c_ptr->flags & CAVE_LR) {
	  /* should become a room space, check to see whether it should be LIGHT_FLOOR or DARK_FLOOR */
	  found = FALSE;
	  for (i = y-1; i <= y+1; ++i)
	    for (j = x-1; j <= x+1; ++j) {
		  fval = (CAVE_ADR(i, j))->fval;		// CHECK: optimization, could use a pointer to avoid two lookups? -SJ
		  flags = (CAVE_ADR(i, j))->flags;
	      if (fval <= MAX_CAVE_ROOM) {
	        c_ptr->fval = fval;
	        //c_ptr->pl = cave[i][j].pl;
	        if (flags & CAVE_PL)				// CHECK: is this right? -SJ
	          c_ptr->flags |= CAVE_PL;			// set it
	        else
	          c_ptr->flags &= ~CAVE_PL;			// clear it
	        found = TRUE;
	        break;
	      }
	    }

	  if (!found) {
	    c_ptr->fval = CORR_FLOOR;
	    c_ptr->flags &= ~CAVE_PL;
	  }
	}
    else {
	  /* should become a corridor space */
	  c_ptr->fval = CORR_FLOOR;
	  c_ptr->flags &= ~CAVE_PL;
	}

    c_ptr->flags &= ~CAVE_FM;
    //if (dungeon1_panel_contains(y, x))
    if ((y >= panel_row_min) && (y <= panel_row_max) && (x >= panel_col_min) && (x <= panel_col_max))
	  if (((c_ptr->flags & CAVE_TL) || (c_ptr->flags & CAVE_PL)) && c_ptr->tptr != 0)
	    bank_msg_print("You have found something!", DUN1NUM);

    dungeon1_lite_spot(y, x);
    res = TRUE;
  }

  return(res);
}




