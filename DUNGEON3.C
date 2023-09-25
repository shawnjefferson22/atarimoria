/* source/dungeon.c: the main command interpreter, updating player status

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>


/* for find/running algorithm */
int8u find_openarea, find_breakright, find_breakleft, find_prevdir;					// in low memory, in BSS segment -SJ
int8u find_direction; /* Keep a record of which way we are going. */

/* Put code, rodata and data in Dungeon1 bank */
#pragma code-name("DUNGEON3BANK")
#pragma rodata-name("DUNGEON3BANK")
#pragma data-name("DUNGEON3BANK")

int8u cycle[] = { 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };				// in cartridge bank in data segment -SJ
int8 chome[] = { -1, 8, 9, 10, 7, -1, 11, 6, 5, 4 };


/* Prototypes of functions copied into this bank */
void __fastcall__ dungeon3_pause_line(int8u prt_line);
int8u __fastcall__ dungeon3_get_panel(int8u y, int8u x, int8u force);
void __fastcall__ find_run(void);
void __fastcall__ dungeon3_end_find(void);
int8u __fastcall__ dungeon3_loc_symbol(int8u y, int8u x);
int8u __fastcall__ dungeon3_no_light(void);
void __fastcall__ dungeon3_known2(inven_type *i_ptr);
void __fastcall__ dungeon3_unsample(inven_type *i_ptr);
int8 __fastcall__ dungeon3_object_offset(inven_type *t_ptr);
void __fastcall__ dungeon3_lite_spot(int8u y, int8u x);
void __fastcall__ sub1_move_light(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ sub3_move_light(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ move_light(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ light_room(int8u y, int8u x);
void __fastcall__ move_char(int8u dir, int8u do_pickup);
int8u __fastcall__ dungeon3_mmove(int8u dir, int8u *y, int8u *x);
void __fastcall__ dungeon3_move_rec(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ area_affect(int8u dir, int8u y, int8u x);
int8u __fastcall__ see_wall(int8u dir, int8u y, int8u x);
int8u see_nothing(int8u dir, int8u y, int8u x);
void __fastcall__ search(int8u y, int8u x, int8u chance);
void __fastcall__ dungeon3_change_trap(int8u y, int8u x);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN3NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon3


void __fastcall__ dungeon3_pause_line(int8u prt_line)
#include "inbank\pause_line.c"

int8u __fastcall__ dungeon3_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

void __fastcall__ dungeon3_move_rec(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\move_rec.c"

void __fastcall__ dungeon3_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ dungeon3_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"

int8 __fastcall__ dungeon3_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

/* Lights up given location				-RAK-	*/
void __fastcall__ dungeon3_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

void __fastcall__ dungeon3_change_trap(int8u y, int8u x)
#include "inbank\change_trap.c"


/* We need to reset the view of things.			-CJS- */
void __fastcall__ check_view(void)
{
  register int8u i, j;
  register cave_type *c_ptr, *d_ptr;


  vbxe_bank(VBXE_CAVBANK);									// need cave bank for this -SJ
  //c_ptr = &cave[char_row][char_col];
  c_ptr = CAVE_ADR(char_row, char_col);

  /* Check for new panel		   */
  if (dungeon3_get_panel(char_row, char_col, FALSE))
    bank_prt_map(DUN3NUM);

  /* Move the light source		   */
  move_light(char_row, char_col, char_row, char_col);

  vbxe_bank(VBXE_CAVBANK);
  /* A room of light should be lit.	 */
  if (c_ptr->fval == LIGHT_FLOOR) {
    if ((py.flags.blind < 1) && !(c_ptr->flags & CAVE_PL))
	  light_room(char_row, char_col);
  }
  /* In doorway of light-room?		   */
  else if ((c_ptr->flags & CAVE_LR) && (py.flags.blind < 1)) {
    for (i = (char_row - 1); i <= (char_row + 1); ++i)
	  for (j = (char_col - 1); j <= (char_col + 1); ++j) {
	    d_ptr = &cave[i][j];
	    if ((d_ptr->fval == LIGHT_FLOOR) && !(d_ptr->flags & CAVE_PL))
	      light_room(i, j);
	  }
  }

  vbxe_restore_bank();
}


/* Given an row (y) and col (x), this routine detects  -RAK-	*/
/* when a move off the screen has occurred and figures new borders.
   Force forcses the panel bounds to be recalculated, useful for 'W'here. */
int8u __fastcall__ dungeon3_get_panel(int8u y, int8u x, int8u force)
{
  register int8 prow, pcol;
  register int8u panel;


  prow = panel_row;
  pcol = panel_col;
  if (force || ((int8)y < panel_row_min + 2) || ((int8)y > panel_row_max - 2)) {
    prow = (((int8)y - SCREEN_HEIGHT/4)/(SCREEN_HEIGHT/2));
    if (prow > max_panel_rows)
	  prow = max_panel_rows;
    else if (prow < 0)
	  prow = 0;
  }

  if (force || ((int16)x < panel_col_min + 3) || ((int16)x > panel_col_max - 3)) {
    pcol = (((int16)x - SCREEN_WIDTH/4)/(SCREEN_WIDTH/2));
    if (pcol > max_panel_cols)
	  pcol = max_panel_cols;
    else if (pcol < 0)
	  pcol = 0;
  }

  if ((prow != panel_row) || (pcol != panel_col)) {
    panel_row = prow;
    panel_col = pcol;
    //panel_bounds();										// panel_bounds code below -SJ

    panel_row_min = panel_row*(SCREEN_HEIGHT/2);
    panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
    panel_row_prt = (int8)panel_row_min - 1;
    panel_col_min = panel_col*(SCREEN_WIDTH/2);
    panel_col_max = panel_col_min + SCREEN_WIDTH - 1;
    panel_col_prt = (int16)panel_col_min - 13;

    panel = TRUE;
    // stop movement if any //
    if (find_bound)
	  dungeon3_end_find();
  }
  else
    panel = FALSE;

  //sprintf(dbgstr, "pr:%d %d pc:%d %d prm:%d %d pcm:%d %d prp:%d pcp:%d panel:%d", panel_row, prow, panel_col, pcol, panel_row_min, panel_row_max, panel_col_min, panel_col_max,
  //		panel_row_prt, panel_col_prt, panel);
  //bank_prt(dbgstr, 23, 0, DUN3NUM);
  //cgetc();										// DEBUG: debugging panel code

  return(panel);
}


void __fastcall__ find_run(void)
{
  /* prevent infinite loops in find mode, will stop after moving 100 times */
  if (find_flag++ > 100) {
    bank_msg_print("You stop running to catch your breath.", DUN3NUM);
    dungeon3_end_find();
  }
  else
    move_char(find_direction, TRUE);
}


/* Switch off the run flag - and get the light correct. -CJS- */
void __fastcall__ dungeon3_end_find(void)
{
  if (find_flag) {
    find_flag = FALSE;
    move_light(char_row, char_col, char_row, char_col);
  }
}


/* Normal movement					*/
/* When FIND_FLAG,  light only permanent features	*/
void __fastcall__ sub1_move_light(int8u y1, int8u x1, int8u y2, int8u x2)
{
  register int8u i, j;
  register cave_type *c_ptr;
  int8u tval, top, left, bottom, right;
  int8u tptr;


  if (light_flag) {
	vbxe_bank(VBXE_CAVBANK);
    for (i = y1-1; i <= y1+1; ++i)	   /* Turn off lamp light */
	  for (j = x1-1; j <= x1+1; ++j)
	    cave[i][j].flags &= ~CAVE_TL;
    if (find_flag && !find_prself)
	  light_flag = FALSE;
  }
  else if (!find_flag || find_prself)
    light_flag = TRUE;

  for (i = y2-1; i <= y2+1; ++i)
    for (j = x2-1; j <= x2+1; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = &cave[i][j];
	  /* only light up if normal movement */
	  if (light_flag)
	    c_ptr->flags |= CAVE_TL;
	  if (c_ptr->fval >= MIN_CAVE_WALL)
	    c_ptr->flags |= CAVE_PL;
	  else if (!(c_ptr->flags & CAVE_FM) && c_ptr->tptr != 0) {
		tptr = c_ptr->tptr;
	    //tval = t_list[c_ptr->tptr].tval;				// can't have both banks in -SJ
	    vbxe_bank(VBXE_OBJBANK);
		tval = t_list[tptr].tval;

	    if ((tval >= TV_MIN_VISIBLE) && (tval <= TV_MAX_VISIBLE)) {
	      vbxe_bank(VBXE_CAVBANK);
	      c_ptr->flags |= CAVE_FM;
	    }
	  }
    }

  /* From uppermost to bottom most lines player was on.	 */
  if (y1 < y2) {
    top = y1 - 1;
    bottom = y2 + 1;
  }
  else {
    top = y2 - 1;
    bottom = y1 + 1;
  }
  if (x1 < x2) {
    left = x1 - 1;
    right = x2 + 1;
  }
  else {
    left = x2 - 1;
    right = x1 + 1;
  }
  for (i = top; i <= bottom; ++i)
    for (j = left; j <= right; ++j)   /* Leftmost to rightmost do*/
      bank_print_symbol(i, j, DUN3NUM);

  if (!find_flag || find_prself)				// DEBUG, debuging (not showing character with light on. :(
  	bank_print_self(y2, x2, DUN3NUM);
}


/* When blinded, move only the player symbol. */
/* With no light, movement becomes involved.  */
void __fastcall__ sub3_move_light(int8u y1, int8u x1, int8u y2, int8u x2)
{
  register int8u i, j;


  if (light_flag) {
	vbxe_bank(VBXE_CAVBANK);
    for (i = y1-1; i <= y1+1; ++i)
	  for (j = x1-1; j <= x1+1; ++j) {
	    cave[i][j].flags &= ~CAVE_TL;
	    bank_print_symbol(i, j, DUN3NUM);
	  }
      light_flag = FALSE;
  }
  else if (!find_flag || find_prself)
    bank_print_symbol(y1, x1, DUN3NUM);

  if (!find_flag || find_prself)
	bank_print_self(y2, x2, DUN3NUM);
}


/* Package for moving the character's light about the screen	 */
/* Four cases : Normal, Finding, Blind, and Nolight	 -RAK-	 */
void __fastcall__ move_light(int8u y1, int8u x1, int8u y2, int8u x2)
{
  if (py.flags.blind > 0 || !player_light)
    sub3_move_light(y1, x1, y2, x2);
  else
    sub1_move_light(y1, x1, y2, x2);
}


/* Room is lit, make it appear				-RAK-	*/
void __fastcall__ light_room(int8u y, int8u x)
{
  register int8u i, j, start_col, end_col;
  int8u tmp1, tmp2, start_row, end_row;
  register cave_type *c_ptr;
  int8u tval, tptr;


  tmp1 = (SCREEN_HEIGHT/2);
  tmp2 = (SCREEN_WIDTH /2);
  start_row = (y/tmp1)*tmp1;
  start_col = (x/tmp2)*tmp2;
  end_row = start_row + tmp1 - 1;
  end_col = start_col + tmp2 - 1;

  for (i = start_row; i <= end_row; ++i)
    for (j = start_col; j <= end_col; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(i,j);
	  if ((c_ptr->flags & CAVE_LR) && !(c_ptr->flags & CAVE_PL)) {
	    c_ptr->flags |= CAVE_PL;
	    if (c_ptr->fval == DARK_FLOOR)
	      c_ptr->fval = LIGHT_FLOOR;
	    if (!(c_ptr->flags & CAVE_FM) && c_ptr->tptr != 0) {
          tptr = c_ptr->tptr;
          vbxe_bank(VBXE_OBJBANK);
		  //tval = t_list[c_ptr->tptr].tval;
		  tval = t_list[tptr].tval;
		  if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE) {
		    vbxe_bank(VBXE_CAVBANK);
		    c_ptr->flags |= CAVE_FM;
	      }
	    }
	    bank_print_symbol(i, j, DUN3NUM);
	  }
    }
}


/* Moves player from one space to another.		-RAK-	*/
/* Note: This routine has been pre-declared; see that for argument*/
void __fastcall__ move_char(int8u dir, int8u do_pickup)
{
  int8 old_row, old_col;
  int8u old_find_flag;
  int8u y, x;
  register int8u i, j;
  //register cave_type *c_ptr, *d_ptr;
  register cave_type *d_ptr;
  cave_type c;


  if ((py.flags.confused > 0) &&    	// Confused?
      (randint(4) > 1) &&	    		// 75% random movement
      (dir != 5))		    			// Never random if sitting
  {
    dir = randint(9);
    dungeon3_end_find();
  }

  y = char_row;
  x = char_col;
  if (dungeon3_mmove(dir, &y, &x)) {
    vbxe_bank(VBXE_CAVBANK);
    //memcpy(&c, &cave[y][x], sizeof(cave_type));
    memcpy(&c, CAVE_ADR(y, x), sizeof(cave_type));
    //c_ptr = &cave[y][x];				  // change c_ptr-> to c. below -SJ

    /* if there is no creature, or an unlit creature in the walls then... */
    /* disallow attacks against unlit creatures in walls because moving into
  	   a wall is a free turn normally, hence don't give player free turns
	   attacking each wall in an attempt to locate the invisible creature,
	   instead force player to tunnel into walls which always takes a turn */

    vbxe_bank(VBXE_MONBANK);

    if ((c.cptr < 2) || (!m_list[c.cptr].ml && c.fval >= MIN_CLOSED_SPACE)) {
	  if (c.fval <= MAX_OPEN_SPACE) { 		// Open floor spot
        /* Make final assignments of char co-ords */
        old_row = char_row;
        old_col = char_col;
        char_row = y;
        char_col = x;

        /* Move character record (-1) */
        dungeon3_move_rec(old_row, old_col, char_row, char_col);

 	    /* Check for new panel */
	    if (dungeon3_get_panel(char_row, char_col, FALSE))
		  bank_prt_map(DUN3NUM);

	    /* Check to see if he should stop */
	    if (find_flag)
		  area_affect(dir, char_row, char_col);

	    /* Check to see if he notices something  */
	    /* fos may be negative if have good rings of searching */
	    if ((py.misc.fos <= 1) || (randint(py.misc.fos) == 1) || (py.flags.status & PY_SEARCH))
		  search(char_row, char_col, py.misc.srh);

	    /* A room of light should be lit. */
	    if (c.fval == LIGHT_FLOOR) {
		  if (!(c.flags & CAVE_PL) && !py.flags.blind)
		    light_room(char_row, char_col);
		}
	    /* In doorway of light-room?	       */
	    else if ((c.flags & CAVE_LR) && (py.flags.blind < 1))
		  for (i = (char_row - 1); i <= (char_row + 1); ++i)
		    for (j = (char_col - 1); j <= (char_col + 1); ++j) {
			  vbxe_bank(VBXE_CAVBANK);								// light_room destroys bank, so should leave this in the loop -SJ
		      //d_ptr = &cave[i][j];
		      d_ptr = CAVE_ADR(i, j);
		      if ((d_ptr->fval == LIGHT_FLOOR) && !(d_ptr->flags & CAVE_PL))
			    light_room(i, j);
		    }

	    /* Move the light source		       */
	    move_light(old_row, old_col, char_row, char_col);

	    /* An object is beneath him.	     */
	    if (c.tptr != 0) {
		  bank_carry(char_row, char_col, do_pickup, DUN3NUM);
		  /* if stepped on falling rock trap, and space contains
		     rubble, then step back into a clear area */

		  vbxe_bank(VBXE_OBJBANK);
		  if (t_list[c.tptr].tval == TV_RUBBLE) {
		    dungeon3_move_rec(char_row, char_col, old_row, old_col);
		    move_light(char_row, char_col, old_row, old_col);
		    char_row = old_row;
		    char_col = old_col;
		    /* check to see if we have stepped back onto another trap, if so, set it off */

		    vbxe_bank(VBXE_CAVBANK);
		    //c_ptr = &cave[char_row][char_col];
		    //memcpy(&c, &cave[char_row][char_col], sizeof(cave_type));
		    memcpy(&c, CAVE_ADR(char_row, char_col), sizeof(cave_type));

		    if (c.tptr != 0) {
			  vbxe_bank(VBXE_OBJBANK);
			  i = t_list[c.tptr].tval;
			  if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP || i == TV_STORE_DOOR)
			    bank_hit_trap(char_row, char_col, DUN3NUM);
			}
		  }
		}
	  }
	  else {	  // Can't move onto floor space
        if (!find_flag && (c.tptr != 0)) {
   	      vbxe_bank(VBXE_OBJBANK);
		  if (t_list[c.tptr].tval == TV_RUBBLE)
		    bank_msg_print("There is rubble blocking your way.", DUN3NUM);
		  else if (t_list[c.tptr].tval == TV_CLOSED_DOOR)
		    bank_msg_print("There is a closed door blocking your way.", DUN3NUM);
		}
	    else
		  dungeon3_end_find();
	    free_turn_flag = TRUE;
	  }
	}
    else {	  // Attacking a creature!
	  old_find_flag = find_flag;
	  dungeon3_end_find();

	  vbxe_bank(VBXE_MONBANK);
	  /* if player can see monster, and was in find mode, then nothing */
	  if (m_list[c.cptr].ml && old_find_flag) {
	    /* did not do anything this turn */
	    free_turn_flag = TRUE;
	  }
	  else {
	    if (py.flags.afraid < 1)		/* Coward?	*/
		  bank_py_attack(y, x, DUN3NUM);
	    else				/* Coward!	*/
		  bank_msg_print("You are too afraid!", DUN3NUM);
	  }
	}
  }
}


void __fastcall__ find_init(int8u dir)
{
  int8u row, col, deepleft, deepright;
  register int8u i, shortleft, shortright;


  row = char_row;
  col = char_col;
  if (!dungeon3_mmove(dir, &row, &col))
    find_flag = FALSE;
  else {
    find_direction = dir;
    find_flag = 1;
    find_breakright = find_breakleft = FALSE;
    find_prevdir = dir;
    if (py.flags.blind < 1) {
	  i = chome[dir];
	  deepleft = deepright = FALSE;
	  shortright = shortleft = FALSE;
	  if (see_wall(cycle[i+1], char_row, char_col)) {
	    find_breakleft = TRUE;
	    shortleft = TRUE;
	  }
	  else if (see_wall(cycle[i+1], row, col)) {
	    find_breakleft = TRUE;
	    deepleft = TRUE;
	  }
	  if (see_wall(cycle[i-1], char_row, char_col)) {
	    find_breakright = TRUE;
	    shortright = TRUE;
	  }
	  else if (see_wall(cycle[i-1], row, col)) {
	    find_breakright = TRUE;
	    deepright = TRUE;
	  }
	  if (find_breakleft && find_breakright) {
	    find_openarea = FALSE;
	    if (dir & 1) {		/* a hack to allow angled corridor entry */
		  if (deepleft && !deepright)
		    find_prevdir = cycle[i-1];
		  else if (deepright && !deepleft)
		    find_prevdir = cycle[i+1];
		}
	      /* else if there is a wall two spaces ahead and seem to be in a
		 corridor, then force a turn into the side corridor, must
		 be moving straight into a corridor here */
	    else if (see_wall(cycle[i], row, col)) {
		  if (shortleft && !shortright)
		    find_prevdir = cycle[i-2];
		  else if (shortright && !shortleft)
		    find_prevdir = cycle[i+2];
		}
	  }
	  else
	    find_openarea = TRUE;
	}
  }

  /* We must erase the player symbol '@' here, because sub3_move_light()
     does not erase the previous location of the player when in find mode
     and when find_prself is FALSE.  The player symbol is not draw at all
     in this case while moving, so the only problem is on the first turn
     of find mode, when the initial position of the character must be erased.
     Hence we must do the erasure here.  */
  if (!light_flag && !find_prself)
    //print(loc_symbol(char_row, char_col), char_row, char_col);
    bank_print_self(char_row, char_col, DUN3NUM);

  move_char(dir, TRUE);
  if (find_flag == FALSE)
    command_count = 0;
}


/* Determine the next direction for a run, or if we should stop.  -CJS- */
void __fastcall__ area_affect(int8u dir, int8u y, int8u x)
{
  int8u newdir, t, inv, check_dir, row, col;
  register int8 i, max;
  register int8u option, option2;
  register cave_type *c_ptr;
  cave_type c;


  if (py.flags.blind < 1) {
    option = 0;
    option2 = 0;
    dir = find_prevdir;
    max = (dir & 1) + 1;
    /* Look at every newly adjacent square. */
    for(i = -max; i <= max; ++i) {
	  newdir = cycle[chome[dir]+i];
	  row = y;
	  col = x;
	  if (dungeon3_mmove(newdir, &row, &col)) {
	    /* Objects player can see (Including doors?) cause a stop. */
	    vbxe_bank(VBXE_CAVBANK);
	    //c_ptr = &cave[row][col];
	    c_ptr = CAVE_ADR(row, col);
        memcpy(&c, c_ptr, sizeof(cave_type));						// need cave location local when cavbank is out

	    if (player_light || (c_ptr->flags & CAVE_TL) || (c_ptr->flags & CAVE_PL) || (c_ptr->flags & CAVE_FM)) {
		  if (c_ptr->tptr != 0) {
			vbxe_bank(VBXE_OBJBANK);								// bank in objbank for t_list
		    t = t_list[c.tptr].tval;
		    if (t != TV_INVIS_TRAP && t != TV_SECRET_DOOR && (t != TV_OPEN_DOOR || !find_ignore_doors)) {
			  dungeon3_end_find();
			  return;
			}
		  }

		  /* Also Creatures		*/
		  /* the monster should be visible since update_mon() checks
		     for the special case of being in find mode */
		  vbxe_bank(VBXE_MONBANK);
		  if (c.cptr > 1 && m_list[c.cptr].ml) {
		    dungeon3_end_find();
		    return;
		  }
		  inv = FALSE;
		}
	    else
		  inv = TRUE;	/* Square unseen. Treat as open. */

	    if (c.fval <= MAX_OPEN_SPACE || inv) {
		  if (find_openarea) {
		    /* Have we found a break? */
		    if (i < 0) {
			  if (find_breakright) {
			    dungeon3_end_find();
			    return;
			  }
			}
		    else if (i > 0) {
			  if (find_breakleft) {
			    dungeon3_end_find();
			    return;
			  }
			}
		  }
		  else if (option == 0)
		    option = newdir;	/* The first new direction. */
		  else if (option2 != 0) {
		    dungeon3_end_find();	/* Three new directions. STOP. */
		    return;
		  }
		  else if (option != cycle[chome[dir]+i-1]) {
		    dungeon3_end_find();	/* If not adjacent to prev, STOP */
		    return;
		  }
		  else {
		    /* Two adjacent choices. Make option2 the diagonal,
			  and remember the other diagonal adjacent to the first option. */
		    if ((newdir & 1) == 1) {
			  check_dir = cycle[chome[dir]+i-2];
			  option2 = newdir;
			}
		    else {
			  check_dir = cycle[chome[dir]+i+1];
			  option2 = option;
			  option = newdir;
			}
		  }
		}
	    else if (find_openarea) {
		  /* We see an obstacle. In open area, STOP if on a side previously open. */
		  if (i < 0) {
		    if (find_breakleft) {
			  dungeon3_end_find();
			  return;
			}
		    find_breakright = TRUE;
		  }
		  else if (i > 0) {
		    if (find_breakright) {
			  dungeon3_end_find();
			  return;
			}
		    find_breakleft = TRUE;
		  }
		}
	  }
	}

    if (find_openarea == FALSE) {	/* choose a direction. */
	  if (option2 == 0 || (find_examine && !find_cut)) {
	    /* There is only one option, or if two, then we always examine
		   potential corners and never cur known corners, so you step into the straight option. */
	    if (option != 0)
		  find_direction = option;
	    if (option2 == 0)
		  find_prevdir = option;
	    else
		  find_prevdir = option2;
	  }
	  else {
	    /* Two options! */
	    row = y;
	    col = x;
	    dungeon3_mmove(option, &row, &col);
	    if (!see_wall(option, row, col) || !see_wall(check_dir, row, col)) {
		  /* Don't see that it is closed off.  This could be a potential corner or an intersection. */
		  if (find_examine && see_nothing(option, row, col) && see_nothing(option2, row, col))
		    /* Can not see anything ahead and in the direction we are turning, assume that it is a potential corner. */
		  {
		    find_direction = option;
		    find_prevdir = option2;
		  }
		  else
		    /* STOP: we are next to an intersection or a room */
		    dungeon3_end_find();
		}
	    else if (find_cut) {
		  /* This corner is seen to be enclosed; we cut the corner. */
		  find_direction = option2;
		  find_prevdir = option2;
		}
	    else {
		  /* This corner is seen to be enclosed, and we deliberately
		     go the long way. */
		  find_direction = option;
		  find_prevdir = option2;
		}
	  }
	}
  }
}


/* Do we see a wall? Used in running.		-CJS- */
int8u __fastcall__ see_wall(int8u dir, int8u y, int8u x)
{
  char c;

  if (!dungeon3_mmove(dir, &y, &x))		/* check to see if movement there possible */
    return TRUE;
  else if ((c = dungeon3_loc_symbol(y, x)) == SYM_WALL || c == SYM_SEAM)
    return TRUE;
  else
    return FALSE;
}


/* Do we see anything? Used in running.		-CJS- */
int8u see_nothing(int8u dir, int8u y, int8u x)
{
  if (!dungeon3_mmove(dir, &y, &x))		/* check to see if movement there possible */
    return FALSE;
  else if (dungeon3_loc_symbol(y, x) == ' ')
    return TRUE;
  else
    return FALSE;
}


/* Returns symbol for given row, column			-RAK-	*/
int8u __fastcall__ dungeon3_loc_symbol(int8u y, int8u x)
{
  register cave_type *cave_ptr;			// in VBXE CAVBANK
  extern cave_type loc_c;        		// temporary storage, now in BSS, defined in io.c -SJ


  /* Save the cave data locally */
  vbxe_bank(VBXE_CAVBANK);
  cave_ptr = &cave[y][x];			// generated code is faster with a pointer here
  loc_c.cptr = cave_ptr->cptr;		// copy the cave data locally
  loc_c.tptr = cave_ptr->tptr;		// memcpy faster?
  loc_c.fval = cave_ptr->fval;
  loc_c.flags = cave_ptr->flags;

  /* Check monsters and player first, need to appear on top of other items */
  vbxe_bank(VBXE_MONBANK);
  if ((loc_c.cptr == 1) && (!find_flag || find_prself))
    return SYM_PLAYER;
  else if (py.flags.status & PY_BLIND)
    return ' ';
  else if ((py.flags.image > 0) && (randint (12) == 1))
    return randint (95) + 31;
  else if ((loc_c.cptr > 1) && (m_list[loc_c.cptr].ml))
    return c_list[m_list[loc_c.cptr].mptr].cchar;
  else if (!(loc_c.flags & CAVE_PL) && !(loc_c.flags & CAVE_TL) && !(loc_c.flags & CAVE_FM))  	// only print those parts that are lit
    return ' ';

  /* Check objects first then floor and walls */
  vbxe_bank(VBXE_OBJBANK);
  if ((loc_c.tptr != 0) && (t_list[loc_c.tptr].tval != TV_INVIS_TRAP))
    return t_list[loc_c.tptr].tchar;
  else if (loc_c.fval <= MAX_CAVE_FLOOR)
    return SYM_FLOOR;
  else if (loc_c.fval == GRANITE_WALL || loc_c.fval == BOUNDARY_WALL || highlight_seams == FALSE)
    return SYM_WALL;
  else
    return SYM_SEAM;
}


/* Returns true if player has no light			-RAK-	*/
int8u __fastcall__ dungeon3_no_light(void)
#include "inbank\no_light.c"


/* Searches for hidden things.			-RAK-	*/
void __fastcall__ search(int8u y, int8u x, int8u chance)
{
  register int8u i, j;
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  //register struct flags *p_ptr;		// just use struct directly -SJ
  //bigvtype tmp_str, tmp_str2;			// use out_val1 and out_val2 -SJ
  cave_type c;							// need a local copy -SJ


  //p_ptr = &py.flags;
  if (py.flags.confused > 0)
    chance = chance / 10;
  if ((py.flags.blind > 0) || dungeon3_no_light())
    chance = chance / 10;
  if (py.flags.image > 0)
    chance = chance / 10;

  for (i = (y - 1); i <= (y + 1); ++i)
    for (j = (x - 1); j <= (x + 1); ++j)
      if (randint(100) < chance) {	/* always in_bounds here */
	    vbxe_bank(VBXE_CAVBANK);					// need the cave bank
	    c_ptr = &cave[i][j];
		memcpy(&c, c_ptr, sizeof(cave_type));		// make a local copy

	    /* Search for hidden objects		   */
	    if (c.tptr != 0) {
		  vbxe_bank(VBXE_OBJBANK);					// need the objbank here
	      t_ptr = &t_list[c.tptr];
	      /* Trap on floor?		       */
	      if (t_ptr->tval == TV_INVIS_TRAP) {
		    bank_objdes(out_val2, t_ptr, TRUE, DUN3NUM);
		    sprintf(out_val1, "You have found %s", out_val2);
		    bank_msg_print(out_val1, DUN3NUM);
		    dungeon3_change_trap(i, j);
		    dungeon3_end_find();
		  }
	      /* Secret door?		       */
	      else if (t_ptr->tval == TV_SECRET_DOOR) {
		    bank_msg_print("You have found a secret door.", DUN3NUM);
		    dungeon3_change_trap(i, j);
		    dungeon3_end_find();
		  }
	      /* Chest is trapped?	       */
	      else if (t_ptr->tval == TV_CHEST) {
		    /* mask out the treasure bits */
		    if ((t_ptr->flags & CH_TRAPPED) > 1)
		      //if (!known2_p(t_ptr)) {
		      if (!(t_ptr->ident & ID_KNOWN2)) { 		// known2_p code in-line here -SJ
		        vbxe_bank(VBXE_OBJBANK);				// objbank must be in here for next call! -SJ
			    dungeon3_known2(t_ptr);
			    bank_msg_print("You have discovered a trap on the chest!", DUN3NUM);
		      }
		      else
		        bank_msg_print("The chest is trapped!", DUN3NUM);
		  }
	    }
	  }
}


