/* source/generate.c: initialize/create a dungeon or town level

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>


#pragma bss-name("GENRAM")

typedef struct coords {
  int8u x, y;
} coords;

coords doorstk[100];                 			// used in build_tunnels function
coords tunstk[1000], wallstk[1000];
int8 yloc[400], xloc[400];

extern int8u doorindex;							  // moved to zeropage -SJ
extern int16u tunindex, wallindex;
#pragma zpsym("doorindex")
#pragma zpsym("tunindex")
#pragma zpsym("wallindex")

extern cave_type *btzp_c_ptr, *btzp_d_ptr;							// optimization, placed in zeropage -SJ
extern int8u btzp_i, btzp_j;
extern int8u btzp_tmp_row, btzp_tmp_col;
extern coords *btzp_tun_ptr;
extern int8u btzp_start_row, btzp_start_col;
extern int8u btzp_y, btzp_x;
#pragma zpsym("btzp_c_ptr")
#pragma zpsym("btzp_d_ptr")
#pragma zpsym("btzp_i")
#pragma zpsym("btzp_j")
#pragma zpsym("btzp_tmp_row")
#pragma zpsym("btzp_tmp_col")
#pragma zpsym("btzp_tun_ptr")
#pragma zpsym("btzp_start_row")
#pragma zpsym("btzp_start_col")
#pragma zpsym("btzp_y")
#pragma zpsym("btzp_x")

extern int8u cgzp_y1, cgzp_x1, cgzp_y2, cgzp_x2, cgzp_pick1, cgzp_pick2;		// optimization, moved to zeropage -SJ
#pragma zpsym("cgzp_y1")
#pragma zpsym("cgzp_y2")
#pragma zpsym("cgzp_x1")
#pragma zpsym("cgzp_x2")
#pragma zpsym("cgzp_pick1")
#pragma zpsym("cgzp_pick2")

extern int8u pszp_t1, pszp_t2;
#pragma zpsym("pszp_t1")
#pragma zpsym("pszp_t2")


#pragma code-name("GEN1BANK")
#pragma rodata-name("GEN1BANK")
#pragma data-name("GEN1BANK")
#pragma bss-name("BSS")


/* Local Prototypes */
int8u __fastcall__ gen1_in_bounds(int8u y, int8u x);
int8u __fastcall__ gen1_next_to_corr(int8u y, int8u x);
void __fastcall__ gen1_alloc_object(int8u __fastcall__ (*alloc_set)(int8u), int8u typ, int8u num);
int8u __fastcall__ gen1_set_room(int8u element);
int8u __fastcall__ gen1_set_corr(int8u element);
int8u __fastcall__ gen1_set_floor(int8u element);
void __fastcall__ gen1_new_spot(int8u *y, int8u *x);
void gen1_place_boundary(void);
int16u __fastcall__ gen1_randnor(int16u mean, int16u stand);
int8u __fastcall__ gen1_mmove(int8u dir, int8u *y, int8u *x);
void __fastcall__ place_streamer(int8u fval, int8u treas_chance);

//void check_cave(int8u n);						// debugging


#undef  INBANK_RETBANK
#define	INBANK_RETBANK		GEN1NUM

#undef  INBANK_MOD
#define INBANK_MOD			gen1


int16u __fastcall__ gen1_randnor(int16u mean, int16u stand)
#include "inbank\randnor.c"

int8u __fastcall__ gen1_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8u __fastcall__ gen1_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"


/* Always picks a correct direction             */
void __fastcall__ correct_dir(int8 *rdir, int8 *cdir, int8u y1, int8u x1, int8u y2, int8u x2)
{
  if (y1 < y2)
    *rdir =  1;
  else if (y1 == y2)
    *rdir =  0;
  else
    *rdir = -1;
  if (x1 < x2)
    *cdir =  1;
  else if (x1 == x2)
    *cdir =  0;
  else
    *cdir = -1;
  if ((*rdir != 0) && (*cdir != 0)) {
    if (randint (2) == 1)
	  *rdir = 0;
    else
	  *cdir = 0;
  }
}


/* Chance of wandering direction                        */
void __fastcall__ rand_dir(int8 *rdir, int8 *cdir)
{
  int8u tmp;


  tmp = randint(4);
  if (tmp < 3) {
    *cdir = 0;
    *rdir = -3 + (tmp << 1); /* tmp=1 -> *rdir=-1; tmp=2 -> *rdir=1 */
  }
  else {
    *rdir = 0;
    *cdir = -7 + (tmp << 1); /* tmp=3 -> *cdir=-1; tmp=4 -> *cdir=1 */
  }
}


/* Fills in empty spots with desired rock               -RAK-   */
/* Note: 9 is a temporary value.                                */
void __fastcall__ fill_cave(int8u fval)
{
  register int8u i, j;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);

  /* no need to check the border of the cave */
  for (i = cur_height - 2; i > 0; --i) {
    c_ptr = CAVE_ADR(i, 1);

    for (j = cur_width - 2; j > 0; --j) {
	  if ((c_ptr->fval == NULL_WALL) || (c_ptr->fval == TMP1_WALL) || (c_ptr->fval == TMP2_WALL))
	    c_ptr->fval = fval;
	  ++c_ptr;
	}
  }
}


/* Builds a room at a row, column coordinate            -RAK-   */
void __fastcall__ build_room(int8u yval, int8u xval)
{
  register cave_type *c_ptr, *d_ptr;
  register int8u i, j;
  int8u y_height, x_left, y_depth, x_right;
  int8u floor;


  if (dun_level <= randint(25))
    floor = LIGHT_FLOOR;        /* Floor with light     */
  else
    floor = DARK_FLOOR;         /* Dark floor           */

  y_height = yval - randint(4);
  y_depth  = yval + randint(3);
  x_left   = xval - randint(11);
  x_right  = xval + randint(11);

  /* the x dim of rooms tends to be much larger than the y dim, so don't
     bother rewriting the y loop */

  vbxe_bank(VBXE_CAVBANK);

  for (i = y_height; i <= y_depth; ++i) {                               // fill with floor
    //c_ptr = &cave[i][x_left];
    c_ptr = CAVE_ADR(i, x_left);
    for (j = x_left; j <= x_right; ++j) {
	  c_ptr->fval  = floor;
	  c_ptr->flags |= CAVE_LR;
	  ++c_ptr;
	}
  }

  for (i = (y_height - 1); i <= (y_depth + 1); ++i) {             // build the vertical walls
    c_ptr = CAVE_ADR(i, x_left-1);
    c_ptr->fval   = GRANITE_WALL;
    c_ptr->flags |= CAVE_LR;

    c_ptr = CAVE_ADR(i, x_right+1);
    c_ptr->fval  = GRANITE_WALL;
    c_ptr->flags |= CAVE_LR;
  }

  c_ptr = CAVE_ADR(y_height-1, x_left);							// build the horiz walls
  d_ptr = CAVE_ADR(y_depth+1, x_left);
  for (i = x_left; i <= x_right; ++i) {
    c_ptr->fval  = GRANITE_WALL;
    c_ptr->flags |= CAVE_LR;
    ++c_ptr;
    d_ptr->fval   = GRANITE_WALL;
    d_ptr->flags |= CAVE_LR;
    ++d_ptr;
  }
}


/* Constructs a tunnel between two points               */
void __fastcall__ build_tunnel(int8u row1, int8u col1, int8u row2, int8u col2)
{
  //coords tunstk[1000], wallstk[1000];                                 // 4000 bytes, can't be in stack!
  //int8u row_dir, col_dir, tunindex, wallindex;                        // also not in stack
  int8u stop_flag, door_flag;
  int16u main_loop_count;
  static int8 row_dir, col_dir;

  extern cave_type *btzp_c_ptr, *btzp_d_ptr;							// optimization, placed in zeropage -SJ
  extern int8u btzp_i, btzp_j;
  extern int8u btzp_tmp_row, btzp_tmp_col;
  extern coords *btzp_tun_ptr;
  extern int8u btzp_start_row, btzp_start_col;
  extern int8u btzp_y, btzp_x;
  #pragma zpsym("btzp_c_ptr")
  #pragma zpsym("btzp_d_ptr")
  #pragma zpsym("btzp_i")
  #pragma zpsym("btzp_j")
  #pragma zpsym("btzp_tmp_row")
  #pragma zpsym("btzp_tmp_col")
  #pragma zpsym("btzp_tun_ptr")
  #pragma zpsym("btzp_start_row")
  #pragma zpsym("btzp_start_col")
  #pragma zpsym("btzp_y")
  #pragma zpsym("btzp_x")


  /* Main procedure for Tunnel                  */
  /* Note: 9 is a temporary value               */
  stop_flag = FALSE;
  door_flag = FALSE;
  tunindex  = 0;
  wallindex = 0;
  main_loop_count = 0;
  btzp_start_row = row1;
  btzp_start_col = col1;
  correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

  do {
    /* prevent infinite loops, just in case */
    ++main_loop_count;
    //if (main_loop_count > 2000)							// CHECK: 2000 maybe too long? -SJ
    if (main_loop_count > 1000)
	  stop_flag = TRUE;

    if (randint(100) > DUN_TUN_CHG) {
	  if (randint(DUN_TUN_RND) == 1)
	    rand_dir(&row_dir, &col_dir);
	  else
	    correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
	}

    btzp_tmp_row = row1 + row_dir;
    btzp_tmp_col = col1 + col_dir;

    while (!gen1_in_bounds(btzp_tmp_row, btzp_tmp_col)) {
	  if (randint(DUN_TUN_RND) == 1)
	    rand_dir(&row_dir, &col_dir);
	  else
	    correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
	  btzp_tmp_row = row1 + row_dir;
	  btzp_tmp_col = col1 + col_dir;
	}

    vbxe_bank(VBXE_CAVBANK);
    btzp_c_ptr = CAVE_ADR(btzp_tmp_row, btzp_tmp_col);
    if (btzp_c_ptr->fval == NULL_WALL) {
	  vbxe_bank(VBXE_GENBANK);

	  row1 = btzp_tmp_row;
	  col1 = btzp_tmp_col;
	  if (tunindex < 1000) {
	    tunstk[tunindex].y = row1;
	    tunstk[tunindex].x = col1;
	    ++tunindex;
	  }
	  door_flag = FALSE;
	  vbxe_restore_bank();
	}
    else if (btzp_c_ptr->fval == TMP2_WALL)
	 ;        // do nothing
    else if (btzp_c_ptr->fval == GRANITE_WALL) {
	  vbxe_bank(VBXE_GENBANK);

	  row1 = btzp_tmp_row;
	  col1 = btzp_tmp_col;
	  if (wallindex < 1000) {
	    wallstk[wallindex].y = row1;
	    wallstk[wallindex].x = col1;
	    ++wallindex;
	  }

	  vbxe_bank(VBXE_CAVBANK);
	  for (btzp_i = row1-1; btzp_i <= row1+1; ++btzp_i)
	    for (btzp_j = col1-1; btzp_j <= col1+1; ++btzp_j)
	      if (gen1_in_bounds(btzp_i, btzp_j)) {
		    btzp_d_ptr = CAVE_ADR(btzp_i, btzp_j);
		    /* values 11 and 12 are impossible here, place_streamer is never run before build_tunnel */
		    if (btzp_d_ptr->fval == GRANITE_WALL)
		      btzp_d_ptr->fval = TMP2_WALL;
		  }
	}
    else if (btzp_c_ptr->fval == CORR_FLOOR || btzp_c_ptr->fval == BLOCKED_FLOOR) {
	  vbxe_bank(VBXE_GENBANK);

	  row1 = btzp_tmp_row;
	  col1 = btzp_tmp_col;
	  if (!door_flag) {
	    if (doorindex < 100) {
		  doorstk[doorindex].y = row1;
		  doorstk[doorindex].x = col1;
		  ++doorindex;
		}
	    door_flag = TRUE;
	  }
      vbxe_restore_bank();

	  if (randint(100) > DUN_TUN_CON) {
	    /* make sure that tunnel has gone a reasonable distance
		before stopping it, this helps prevent isolated rooms */
	    btzp_tmp_row = row1 - btzp_start_row;
	    //if (tmp_row < 0) tmp_row = -tmp_row;                        // FIXME: negative?
	    btzp_tmp_col = col1 - btzp_start_col;
	    //if (tmp_col < 0) tmp_col = -tmp_col;                        // FIXME: negative?
	    if (btzp_tmp_row > 10 || btzp_tmp_col > 10)
		  stop_flag = TRUE;
	  }
	}
    else {  /* c_ptr->fval != NULL, TMP2, GRANITE, CORR */
	  row1 = btzp_tmp_row;
	  col1 = btzp_tmp_col;
	}
  } while (((row1 != row2) || (col1 != col2)) && (!stop_flag));

  vbxe_bank(VBXE_GENBANK);
  btzp_tun_ptr = &tunstk[0];
  for (btzp_i = 0; btzp_i < tunindex; ++btzp_i) {
    vbxe_bank(VBXE_GENBANK);
    btzp_y = btzp_tun_ptr->y;
    btzp_x = btzp_tun_ptr->x;

    vbxe_bank(VBXE_CAVBANK);
    btzp_d_ptr = CAVE_ADR(btzp_y, btzp_x);
    btzp_d_ptr->fval = CORR_FLOOR;
    ++btzp_tun_ptr;
  }
  for (btzp_i = 0; btzp_i < wallindex; ++btzp_i) {
    vbxe_bank(VBXE_GENBANK);
    btzp_y = wallstk[btzp_i].y;
    btzp_x = wallstk[btzp_i].x;

    vbxe_bank(VBXE_CAVBANK);
    btzp_c_ptr = CAVE_ADR(btzp_y, btzp_x);
    if (btzp_c_ptr->fval == TMP2_WALL) {
	  if (randint(100) < DUN_TUN_PEN)
	    bank_place_door(btzp_y, btzp_x, GEN1NUM);
	  else {
		/* these have to be doorways to rooms */
		btzp_c_ptr->fval  = CORR_FLOOR;
	  }
	}
  }
}


/* Checks all adjacent spots for corridors              -RAK-   */
/* note that y, x is always in_bounds(), hence no need to check that
   j, k are in_bounds(), even if they are 0 or cur_x-1 is still works */
int8u __fastcall__ gen1_next_to_corr(int8u y, int8u x)
{
  register int8u k, j, i;
  register cave_type *c_ptr;
  cave_type c;


  i = 0;
  for (j = y - 1; j <= (y + 1); ++j)
    for (k = x - 1; k <= (x + 1); ++k) {
	  c_ptr = CAVE_ADR(j, k);			// get cave location
	  c.tptr = c_ptr->tptr;
	  c.fval = c_ptr->fval;

	  vbxe_bank(VBXE_OBJBANK);
	  /* should fail if there is already a door present */
	  if (c.fval == CORR_FLOOR && (c.tptr == 0 || t_list[c.tptr].tval < TV_MIN_DOORS))
	    ++i;
	  vbxe_restore_bank();				// cave bank will be restored -SJ
    }

  return(i);
}


int8u __fastcall__ gen1_next_to(int8u y, int8u x)
{
  int8u next;


  if (gen1_next_to_corr(y, x) > 2)
    if (((CAVE_ADR(y-1,x))->fval >= MIN_CAVE_WALL) && ((CAVE_ADR(y+1,x))->fval >= MIN_CAVE_WALL))
      next = TRUE;
    else if (((CAVE_ADR(y, x-1))->fval >= MIN_CAVE_WALL) && ((CAVE_ADR(y, x+1))->fval >= MIN_CAVE_WALL))
      next = TRUE;
    else
      next = FALSE;
  else
    next = FALSE;
  return(next);
}


/* Places door at y, x position if at least 2 walls found       */
void __fastcall__ try_door(int8u y, int8u x)
{
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);

  if ((c_ptr->fval == CORR_FLOOR) && (randint(100) > DUN_TUN_JCT) && gen1_next_to(y, x))
    bank_place_door(y, x, GEN1NUM);
}


/* Returns random co-ordinates                          -RAK-   */
void __fastcall__ gen1_new_spot(int8u *y, int8u *x)
{
  register int8u i, j;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  do {
    i = randint(cur_height - 2);
    j = randint(cur_width - 2);
    c_ptr = CAVE_ADR(i, j);
  }
  while (c_ptr->fval >= MIN_CLOSED_SPACE || (c_ptr->cptr != 0) || (c_ptr->tptr != 0));
  *y = i;
  *x = j;
}


/* Cave logic flow for generation of new dungeon                */
void cave_gen(void)
{
  static int8u room_map[4][4];                                         // max is 4x4 with atari8 see below -SJ
  register int8u i, j, k, tmp;
  int8u alloc_level;
  //int8 yloc[400], xloc[400];                          // this means yloc and xloc don't need to be so big either (max 16?)
  register int8u y, x;

  extern int8u cgzp_y1, cgzp_x1, cgzp_y2, cgzp_x2, cgzp_pick1, cgzp_pick2;		// optimization, moved to zeropage -SJ
  #pragma zpsym("cgzp_y1")
  #pragma zpsym("cgzp_y2")
  #pragma zpsym("cgzp_x1")
  #pragma zpsym("cgzp_x2")
  #pragma zpsym("cgzp_pick1")
  #pragma zpsym("cgzp_pick2")


  /* Build Rooms */
  #define row_rooms	4
  #define col_rooms	4

  memset(&room_map, FALSE, 16); 			  						// clear room map memset

  k = gen1_randnor(DUN_ROO_MEA, 2);
  for (i = 0; i < k; ++i)                                           // set some rooms
    room_map[randint(row_rooms)-1][randint(col_rooms)-1] = TRUE;

  k = 0;
  for (i = 0; i < row_rooms; ++i)                                   // 0-3
    for (j = 0; j < col_rooms; ++j)                                 // 0-3
      if (room_map[i][j] == TRUE) {
	    vbxe_bank(VBXE_GENBANK);

	    yloc[k] = i * (SCREEN_HEIGHT >> 1) + QUART_HEIGHT;  // 11+5.5 = 16 * i; 0,16,32,48
	    xloc[k] = j * (SCREEN_WIDTH >> 1) + QUART_WIDTH;    // 33+16.5 = 49 * i; 0,49,98,147

	    if (dun_level > randint(DUN_UNUSUAL)) {
	      tmp = randint(3);
	      if (tmp == 1)
	        bank_build_type1(yloc[k], xloc[k]);
	      else if (tmp == 2)
	        bank_build_type2(yloc[k], xloc[k]);
	      else
	        bank_build_type3(yloc[k], xloc[k]);
	    }
	    else
	      build_room(yloc[k], xloc[k]);
	    ++k;
	  }

  /* Build Tunnels */
  vbxe_bank(VBXE_GENBANK);
  for (i = 0; i < k; ++i)
  {
    cgzp_pick1 = randint(k) - 1;
    cgzp_pick2 = randint(k) - 1;
    cgzp_y1 = yloc[cgzp_pick1];
    cgzp_x1 = xloc[cgzp_pick1];
    yloc[cgzp_pick1] = yloc[cgzp_pick2];
    xloc[cgzp_pick1] = xloc[cgzp_pick2];
    yloc[cgzp_pick2] = cgzp_y1;
    xloc[cgzp_pick2] = cgzp_x1;
  }

  doorindex = 0;
  /* move zero entry to k, so that can call build_tunnel all k times */
  yloc[k] = yloc[0];
  xloc[k] = xloc[0];
  for (i = 0; i < k; ++i)
  {
    vbxe_bank(VBXE_GENBANK);            // for yloc and xloc!
    cgzp_y1 = yloc[i];
    cgzp_x1 = xloc[i];
    cgzp_y2 = yloc[i+1];
    cgzp_x2 = xloc[i+1];
    build_tunnel(cgzp_y2, cgzp_x2, cgzp_y1, cgzp_x1);
  }

  /* Fill the rest of the cave up */
  fill_cave(GRANITE_WALL);

  /* Place Streamers */
  for (i = 0; i < DUN_STR_MAG; ++i)
    place_streamer(MAGMA_WALL, DUN_STR_MC);

  for (i = 0; i < DUN_STR_QUA; ++i)
    place_streamer(QUARTZ_WALL, DUN_STR_QC);

  /* Place Boundary Walls */
  gen1_place_boundary();

  /* Place intersection doors   */
  for (i = 0; i < doorindex; ++i)
  {
    vbxe_bank(VBXE_GENBANK);
    y = doorstk[i].y;
    x = doorstk[i].x;

    try_door(y, x-1);
    try_door(y, x+1);
    try_door(y-1, x);
    try_door(y+1, x);
  }

  /* Place Stairs, Objects and Monsters */
  alloc_level = (dun_level/3);
  if (alloc_level < 2)
    alloc_level = 2;
  else if (alloc_level > 10)
    alloc_level = 10;
  bank_place_stairs(2, randint(2)+1, 2, GEN1NUM);							// adjusted to +1 and 2 walls
  bank_place_stairs(1, randint(2), 2, GEN1NUM);								// adjusted to 2 walls

  /* Set up the character co-ords, used by gen1_alloc_monster, place_win_monster */
  gen1_new_spot(&char_row, &char_col);

  bank_alloc_monster((randint(8)+MIN_MALLOC_LEVEL+alloc_level), 0, TRUE, GEN1NUM);

  gen1_alloc_object(gen1_set_corr, 3, randint(alloc_level));
  gen1_alloc_object(gen1_set_room, 5, gen1_randnor(TREAS_ROOM_ALLOC, 3));
  gen1_alloc_object(gen1_set_floor, 5, gen1_randnor(TREAS_ANY_ALLOC, 3));
  gen1_alloc_object(gen1_set_floor, 4, gen1_randnor(TREAS_GOLD_ALLOC, 3));
  gen1_alloc_object(gen1_set_floor, 1, randint(alloc_level));

  if (dun_level >= WIN_MON_APPEAR) bank_place_win_monster();
 }


/* Generates a random dungeon level                     -RAK-   */
void generate_cave(void)
{
  panel_row_min = 0;
  panel_row_max = 0;
  panel_col_min = 0;
  panel_col_max = 0;
  char_row = 255;                                        // CHECK: negative? was -1
  char_col = 255;                                        // CHECK: negative? was -1

  //tlink();
  //mlink();
  //blank_cave();
  bank_prep_cave();                                     // only gets called from here, in gen2bank

  if (dun_level == 0) {
      cur_height = SCREEN_HEIGHT;       						// 22
      cur_width  = SCREEN_WIDTH;        						// 66
      max_panel_rows = (cur_height/SCREEN_HEIGHT)*2 - 2;        // 0            // CHECK: could just set to zero, will never change -SJ
      max_panel_cols = (cur_width /SCREEN_WIDTH )*2 - 2;        // 0            // CHECK
      panel_row = max_panel_rows;                               // 0
      panel_col = max_panel_cols;                               // 0

      bank_town_gen();											// only called from here
  }
  else {
    cur_height = MAX_HEIGHT;
    cur_width  = MAX_WIDTH;
    max_panel_rows = (cur_height/SCREEN_HEIGHT)*2 - 2;
    max_panel_cols = (cur_width /SCREEN_WIDTH )*2 - 2;
    panel_row = max_panel_rows;
    panel_col = max_panel_cols;

	bank_eval_quest_level(dun_level, GEN1NUM);					// evaluate quest
    cave_gen();
  }
}


/* Allocates an object for tunnels and rooms            -RAK-   */
void __fastcall__ gen1_alloc_object(int8u __fastcall__ (*alloc_set)(int8u), int8u typ, int8u num)
{
  register int8u i, j, k;
  register cave_type *c_ptr;


  for (k = 0; k < num; ++k) {
    vbxe_bank(VBXE_CAVBANK);

    do {
	  i = randint(cur_height) - 1;
	  j = randint(cur_width) - 1;
      c_ptr = CAVE_ADR(i, j);
    } while ((!(*alloc_set)(c_ptr->fval)) || (c_ptr->tptr != 0) || (i == char_row && j == char_col));

    /* don't put an object beneath the player, this could cause problems
	   if player is standing under rubble, or on a trap */

    if (typ < 4) {    /* typ == 2 not used, used to be visible traps */
	  if (typ == 1) bank_place_trap(i, j, randint(MAX_TRAP)-1, GEN1NUM); /* typ == 1 */
	  else          bank_place_rubble(i, j, GEN1NUM);                    /* typ == 3 */
    } else {
	  if (typ == 4) bank_place_gold(i, j, GEN1NUM);                      /* typ == 4 */
	  else          bank_place_object(i, j, GEN1NUM);                    /* typ == 5 */
    }
  }
}


int8u __fastcall__ gen1_set_room(int8u element)
{
  if ((element == DARK_FLOOR) || (element == LIGHT_FLOOR))
    return(TRUE);
  else
    return(FALSE);
}

int8u __fastcall__ gen1_set_corr(int8u element)
{
  if (element == CORR_FLOOR || element == BLOCKED_FLOOR)
    return(TRUE);
  else
    return(FALSE);
}

int8u __fastcall__ gen1_set_floor(int8u element)
{
  if (element <= MAX_CAVE_FLOOR)
    return(TRUE);
  else
    return(FALSE);
}


/* Places indestructible rock around edges of dungeon   -RAK-   */
void gen1_place_boundary(void)
{
  register int8u i;
  register cave_type *top_ptr, *bottom_ptr;
  cave_type (*left_ptr)[MAX_WIDTH];
  cave_type (*right_ptr)[MAX_WIDTH];


  vbxe_bank(VBXE_CAVBANK);

  /* put permanent wall on leftmost row and rightmost row */
  left_ptr = (cave_type (*)[MAX_WIDTH]) &cave[0][0];
  right_ptr = (cave_type (*)[MAX_WIDTH]) &cave[0][cur_width - 1];

  for (i = 0; i < cur_height; ++i)
    {
      ((cave_type *)left_ptr)->fval     = BOUNDARY_WALL;
      ++left_ptr;
      ((cave_type *)right_ptr)->fval    = BOUNDARY_WALL;
      ++right_ptr;
    }

  /* put permanent wall on top row and bottom row */
  top_ptr = &cave[0][0];
  //bottom_ptr = &cave[cur_height - 1][0];
  bottom_ptr = CAVE_ADR(cur_height-1, 0);

  for (i = 0; i < cur_width; ++i)
    {
      top_ptr->fval     = BOUNDARY_WALL;
      ++top_ptr;
      bottom_ptr->fval  = BOUNDARY_WALL;
      ++bottom_ptr;
    }
}


/* Places "streamers" of rock through dungeon           -RAK-   */
void __fastcall__ place_streamer(int8u fval, int8u treas_chance)
{
  register int8u i, tx, ty;
  register int8u dir;
  static int8u x, y;
  register cave_type *c_ptr;

  extern int8u pszp_t1, pszp_t2;
  #pragma zpsym("pszp_t1")
  #pragma zpsym("pszp_t2")


  /* Choose starting point and direction */
  y = (cur_height / 2) + 11 - randint(23);
  x = (cur_width / 2)  + 16 - randint(33);

  dir = randint(8);     /* Number 1-4, 6-9 */
  if (dir > 4)
    dir = dir + 1;

  /* Place streamer into dungeon */
  pszp_t1 = 2*DUN_STR_RNG + 1;       /* Constants */
  pszp_t2 =   DUN_STR_RNG + 1;
  do {
    for (i = 0; i < DUN_STR_DEN; ++i) {
	  ty = y + randint(pszp_t1) - pszp_t2;
	  tx = x + randint(pszp_t1) - pszp_t2;
	  if (gen1_in_bounds(ty, tx)) {
	    //c_ptr = &cave[ty][tx];
	    vbxe_bank(VBXE_CAVBANK);					// need cavebank as place_gold may bank it out -SJ
	    c_ptr = CAVE_ADR(ty, tx);
	    if (c_ptr->fval == GRANITE_WALL) {
		  c_ptr->fval = fval;
		  if (randint(treas_chance) == 1)
		    bank_place_gold(ty, tx, GEN1NUM);
		}
	  }
	}
  } while (gen1_mmove(dir, &y, &x));
}


/*void check_cave(int8u n)
{
  int8u y, x;


  vbxe_bank(VBXE_CAVBANK);
  for(y=0; y<MAX_HEIGHT; ++y)
    for(x=0; x<MAX_WIDTH; ++x) {
      if (cave[y][x].cptr == 1) {
        sprintf(dbgstr, "Found at %d, %d %d", n, y, x);
        bank_prt(dbgstr, 0, 0, GEN1NUM);
        cgetc();
      }
	}

  vbxe_restore_bank();
  return;
}*/
