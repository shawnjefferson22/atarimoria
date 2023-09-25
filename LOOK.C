/* source/moria4.c: misc code, mainly to handle player commands

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("LOOKBANK")
#pragma data-name("LOOKBANK")
#pragma rodata-name("LOOKBANK")


/* Globally accessed variables: gl_nseen counts the number of places where
   something is seen. gl_rock indicates a look for rock or objects.

   The others map co-ords in the ray frame to dungeon co-ords.

   dungeon y = char_row	 + gl_fyx * (ray x)  + gl_fyy * (ray y)
   dungeon x = char_col	 + gl_fxx * (ray x)  + gl_fxy * (ray y) */

int16 gl_fxx, gl_fxy, gl_fyx, gl_fyy;
int8u gl_nseen, gl_noquery;
int8u gl_rock;

/* Intended to be indexed by dir/2, since is only relevant to horizontal or
   vertical directions. */
const int16 set_fxy[] = { 0,  1,  0,  0, -1 };
const int16 set_fxx[] = { 0,  0, -1,  1,  0 };
const int16 set_fyy[] = { 0,  0,  1, -1,  0 };
const int16 set_fyx[] = { 0,  1,  0,  0, -1 };
/* Map diagonal-dir/2 to a normal-dir/2. */
const int16 map_diag1[] = { 1, 3, 0, 2, 4 };
const int16 map_diag2[] = { 2, 1, 0, 4, 3 };

#define GRADF	2000	/* Any sufficiently big number will do */


/* Local Prototypes */
int8u __fastcall__ look_panel_contains(int8u y, int8u x);
int8u __fastcall__ look_is_a_vowel(char ch);
int8u __fastcall__ look_get_alldir(char *prompt, int8u *dir);
char __fastcall__ look_map_roguedir(char comval);
int8u __fastcall__ look_see(int16 x, int16 y, int8u *transparent);
int8u __fastcall__ look_ray(int16 y, int16 from, int16 to);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		LOOKNUM

#undef  INBANK_MOD
#define INBANK_MOD			look


int8u __fastcall__ look_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ look_is_a_vowel(char ch)
#include "inbank\is_a_vowel.c"

char __fastcall__ look_map_roguedir(char comval)
#include "inbank\map_roguedir.c"


/* Similar to get_dir, except that no memory exists, and it is		-CJS-
   allowed to enter the null direction. */
int8u __fastcall__ look_get_alldir(char *prompt, int8u *dir)
{
  char command;


  bank_prt(prompt, 0, 0, LOOKNUM);

  for(;;) {
    //if (!bank_get_com(prompt, &command, LOOKNUM)) {
	//  free_turn_flag = TRUE;
	//  return FALSE;
	//}

    /* Joystick movement -SJ */
	while (!kbhit()) {								// While no key pressed
	  if (STICK0 != 15) {							// if joystick moved
        command = joystick_command[STICK0];			// get the direction as command character
		break;
      }
      if (STRIG0 == 0) {							// press button to look in all dirs
	    command = '.';
	    break;
  	  }
	}

    if (kbhit()) {
 	  command = cgetc();
	  if (command == ESCAPE) {
	    free_turn_flag = TRUE;
	    return FALSE;
   	  }
	}

	bank_vbxe_cleartoeol(0, MSG_LINE, LOOKNUM);

    command = look_map_roguedir(command);
    if (command >= '1' && command <= '9') {
	  *dir = command - '0';
	  return TRUE;
	}
  }
}



/* An enhanced look, with peripheral vision. Looking all 8	-CJS-
   directions will see everything which ought to be visible. Can
   specify direction 5, which looks in all directions.

   For the purpose of hindering vision, each place is regarded as
   a diamond just touching its four immediate neighbours. A
   diamond is opaque if it is a wall, or shut door, or something
   like that. A place is visible if any part of its diamond is
   visible: i.e. there is a line from the view point to part of
   the diamond which does not pass through any opaque diamonds.

   Consider the following situation:

     @....			  			  X	  X   X	  X   X
     .##..			  			 / \ / \ / \ / \ / \
     .....			            X @ X . X . X 1 X . X
				                 \ / \ / \ / \ / \ /
				                  X	  X   X	  X   X
	   Expanded view, with	     / \ / \ / \ / \ / \
	   diamonds inscribed	    X . X # X # X 2 X . X
	   about each point,	     \ / \ / \ / \ / \ /
	   and some locations	      X	  X   X	  X   X
	   numbered.		         / \ / \ / \ / \ / \
				                X . X . X . X 3 X 4 X
				                 \ / \ / \ / \ / \ /
				                  X	  X   X	  X   X
	- Location 1 is fully visible.
	- Location 2 is visible, even though partially obscured.
	- Location 3 is invisible, but if either # were
	  transparent, it would be visible.
	- Location 4 is completely obscured by a single #.

   The function which does the work is look_ray. It sets up its
   own co-ordinate frame (global variables map back to the
   dungeon frame) and looks for everything between two angles
   specified from a central line. It is recursive, and each call
   looks at stuff visible along a line parallel to the center
   line, and a set distance away from it. A diagonal look uses
   more extreme peripheral vision from the closest horizontal and
   vertical directions; horizontal or vertical looks take a call
   for each side of the central line. */



/* Look at what we can see. This is a free move.

   Prompts for a direction, and then looks at every object in
   turn within a cone of vision in that direction. For each
   object, the cursor is moved over the object, a description is
   given, and we wait for the user to type something. Typing
   ESCAPE will abort the entire look.

   Looks first at real objects and monsters, and looks at rock
   types only after all other things have been seen.  Only looks at rock
   types if the highlight_seams option is set. */

void __fastcall__ look(void)
{
  register int8u i;
  int8u abort;
  int8u dir, dummy;


  if (py.flags.blind > 0)
    bank_msg_print("You can't see a damn thing!", LOOKNUM);
  else if (py.flags.image > 0)
    bank_msg_print("You can't believe what you are seeing! It's like a dream!", LOOKNUM);
  else if (look_get_alldir("Look which direction?", &dir)) {
    abort = FALSE;
    gl_nseen = 0;
    gl_rock = 0;
    gl_noquery = FALSE;	/* Have to set this up for the look_see */

    if (look_see(0, 0, &dummy))
	  abort = TRUE;
    else {
	  do {
	    abort = FALSE;
	    if (dir == 5) {
		  for (i = 1; i <= 4; ++i) {
		    gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		    gl_fxy = set_fxy[i]; gl_fyy = set_fyy[i];
		    if (look_ray(0, 2*GRADF-1, 1)) {
			  abort = TRUE;
			  break;
			}
		    gl_fxy = -gl_fxy;
		    gl_fyy = -gl_fyy;
		    if (look_ray(0, 2*GRADF, 2)) {
			  abort = TRUE;
			  break;
			}
		  }
		}
	    else if ((dir & 1) == 0) {	/* Straight directions */
		  i = dir >> 1;
		  gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		  gl_fxy = set_fxy[i]; gl_fyy = set_fyy[i];
		  if (look_ray(0, GRADF, 1))
		    abort = TRUE;
		  else {
		    gl_fxy = -gl_fxy;
		    gl_fyy = -gl_fyy;
		    abort = look_ray(0, GRADF, 2);
		  }
		}
	    else {
		  i = map_diag1[dir >> 1];
		  gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		  gl_fxy = -set_fxy[i]; gl_fyy = -set_fyy[i];
		  if (look_ray(1, 2*GRADF, GRADF))
		    abort = TRUE;
		  else {
		    i = map_diag2[dir >> 1];
		    gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		    gl_fxy = set_fxy[i]; gl_fyy = set_fyy[i];
		    abort = look_ray(1, 2*GRADF-1, GRADF);
		  }
		}
	  }
	  while (abort == FALSE && highlight_seams && (++gl_rock < 2));

	  if (abort)
	    bank_msg_print("--Aborting look--", LOOKNUM);
	  else {
	    if (gl_nseen) {
		  if (dir == 5)
		    bank_msg_print("That's all you see.", LOOKNUM);
		  else
		    bank_msg_print("That's all you see in that direction.", LOOKNUM);
		}
	    else if (dir == 5)
		  bank_msg_print("You see nothing of interest.", LOOKNUM);
	    else
		  bank_msg_print("You see nothing of interest in that direction.", LOOKNUM);
	  }
	}
  }
}


/* Look at everything within a cone of vision between two ray
   lines emanating from the player, and y or more places away
   from the direct line of view. This is recursive.

   Rays are specified by gradients, y over x, multiplied by
   2*GRADF. This is ONLY called with gradients between 2*GRADF
   (45 degrees) and 1 (almost horizontal).

   (y axis)/ angle from
     ^	  /	      ___ angle to
     |	 /	    ___
  ...|../.....___.................... parameter y (look at things in the
     | /   ___			      cone, and on or above this line)
     |/ ___
     @-------------------->   direction in which you are looking. (x axis)
     |
     | */
int8u __fastcall__ look_ray(int16 y, int16 from, int16 to)
{
  register int16 max_x, x;
  int8u transparent;


  /* from is the larger angle of the ray, since we scan towards the
     center line. If from is smaller, then the ray does not exist. */
  if (from <= to || y > MAX_SIGHT)
    return FALSE;
  /* Find first visible location along this line. Minimum x such
     that (2x-1)/x < from/GRADF <=> x > GRADF(2x-1)/from. This may
     be called with y=0 whence x will be set to 0. Thus we need a
     special fix. */
  x = GRADF * (2 * y - 1) / from + 1;
  if (x <= 0)
    x = 1;

  /* Find last visible location along this line.
     Maximum x such that (2x+1)/x > to/GRADF <=> x < GRADF(2x+1)/to */
  max_x = (GRADF * (2 * y + 1) - 1) / to;
  if (max_x > MAX_SIGHT)
    max_x = MAX_SIGHT;
  if (max_x < x)
    return FALSE;

  /* gl_noquery is a HACK to prevent doubling up on direct lines of
     sight. If 'to' is	greater than 1, we do not really look at
     stuff along the direct line of sight, but we do have to see
     what is opaque for the purposes of obscuring other objects. */
  if (y == 0 && to > 1 || y == x && from < GRADF*2)
    gl_noquery = TRUE;
  else
    gl_noquery = FALSE;
  if (look_see(x, y, &transparent))
    return TRUE;
  if (y == x)
    gl_noquery = FALSE;
  if (transparent)
    goto init_transparent;

  for (;;) {
    /* Look down the window we've found. */
    if (look_ray(y+1, from, (2 * y + 1) * GRADF / x))
	  return TRUE;
      /* Find the start of next window. */
    do {
	  if (x == max_x)
	    return FALSE;
	  /* See if this seals off the scan. (If y is zero, then it will.) */
	  from = (2 * y - 1) * GRADF / x;
	  if (from <= to)
	    return FALSE;
	  x++;

	  if (look_see(x, y, &transparent))
	    return TRUE;
	} while(!transparent);

    init_transparent:
    /* Find the end of this window of visibility. */
    do {
	  if (x == max_x)
	    /* The window is trimmed by an earlier limit. */
	    return look_ray(y+1, from, to);
	  x++;

	  if (look_see(x, y, &transparent))
	    return TRUE;
	} while(transparent);
  }
}


int8u __fastcall__ look_see(int16 x, int16 y, int8u *transparent)
{
  char *dstring, *string, query;
  register cave_type *c_ptr;
  int16 j;
  int8u m;
  int16u mptr;


  //if (x < 0 || y < 0 || y > x) {
  if (y > x) {
    sprintf(out_val1, "Illegal call to look_see(%d, %d)", x, y);
    bank_msg_print(out_val1, LOOKNUM);
  }

  if (x == 0 && y == 0)
    dstring = "You are on";
  else
    dstring = "You see";

  j = char_col + gl_fxx * x + gl_fxy * y;
  y = char_row + gl_fyx * x + gl_fyy * y;
  x = j;

  //sprintf(dbgstr, "see gl_fxx:%d gl_fxy:%d x:%d y:%d t:%d", gl_fxx, gl_fxy, x, y, *transparent);
  //bank_prt(dbgstr, 23, 0, LOOKNUM);
  //cgetc();

  if (!look_panel_contains(y, x)) {
    *transparent = FALSE;
    return FALSE;
  }

  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);
  m = c_ptr->cptr;					// save monster at this cave address -SJ

  *transparent = c_ptr->fval <= MAX_OPEN_SPACE;
  if (gl_noquery)
    return FALSE; 	/* Don't look at a direct line of sight. A hack. */

  out_val1[0] = 0;
  vbxe_bank(VBXE_MONBANK);
  if (gl_rock == 0 && m > 1 && m_list[m].ml) {			// monster here and lit? -SJ
    mptr = m_list[m].mptr;
    sprintf(out_val1, "%s %s %s. [(r)ecall]", dstring, look_is_a_vowel(c_list[mptr].name[0] ) ? "an" : "a", c_list[mptr].name);
    dstring = "It is on";
    bank_prt(out_val1, 0, 0, LOOKNUM);
    //move_cursor_relative(y, x);						// no cursor -SJ
    bank_print_symbol_highlight(y, x, LOOKNUM);

    query = cgetc();
    if (query == 'r' || query == 'R') {
	  bank_vbxe_savescreen(LOOKNUM);
	  query = bank_roff_recall(mptr, LOOKNUM);
	  bank_vbxe_restorescreen(LOOKNUM);
	}
  }

  vbxe_bank(VBXE_CAVBANK);
  if ((c_ptr->flags & CAVE_TL) || (c_ptr->flags & CAVE_PL) || (c_ptr->flags & CAVE_FM)) {
    if (c_ptr->tptr != 0) {
	  m = c_ptr->tptr;					// save tptr -SJ
	  vbxe_bank(VBXE_OBJBANK);

	  if (t_list[m].tval == TV_SECRET_DOOR)
	    goto granite;
	  if (gl_rock == 0 && t_list[m].tval != TV_INVIS_TRAP) {
	    bank_objdes(out_val2, &t_list[m], TRUE, LOOKNUM);
	    sprintf(out_val1, "%s %s ---pause---", dstring, out_val2);
	    dstring = "It is in";
	    bank_prt(out_val1, 0, 0, LOOKNUM);
	    //move_cursor_relative(y, x);			// no cursor -SJ
        bank_print_symbol_highlight(y, x, LOOKNUM);
	    query = cgetc();
	  }
	}

    vbxe_bank(VBXE_CAVBANK);
    if ((gl_rock || out_val1[0]) && c_ptr->fval >= MIN_CLOSED_SPACE) {
	  switch(c_ptr->fval) {
	    case BOUNDARY_WALL:
	    case GRANITE_WALL:
	    granite:
	      /* Granite is only interesting if it contains something. */
	      if(out_val1[0])
		    string = "a granite wall";
	      else
		    string = CNIL;	/* In case we jump here */
	      break;
	    case MAGMA_WALL:
	      string = "some dark rock";
	      break;
	    case QUARTZ_WALL:
	      string = "a quartz vein";
	      break;
	    default: string = CNIL;
	      break;
	  }

	  if (string) {
	    sprintf(out_val1, "%s %s ---pause---", dstring, string);
	    bank_prt(out_val1, 0, 0, LOOKNUM);
        bank_print_symbol_highlight(y, x, LOOKNUM);

	    query = cgetc();
	  }
	}
  }

  if (out_val1[0]) {
    bank_print_symbol(y, x, LOOKNUM);				// back to normal
    gl_nseen++;
    if (query == ESCAPE)
	  return TRUE;
  }

  return FALSE;
}

