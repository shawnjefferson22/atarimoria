/* source/generate.c: initialize/create a dungeon or town level

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



#pragma code-name("GEN2BANK")
#pragma data-name("GEN2BANK")
#pragma rodata-name("GEN2BANK")


/* Local Prototypes */
void __fastcall__ gen2_invcopy(inven_type *to, int16u from_index);
void __fastcall__ build_store(int8u store_num, int8u y, int8u x);
int8u __fastcall__ gen2_popt(void);
void __fastcall__ gen2_fill_cave(int8u fval);
void gen2_place_boundary(void);
void __fastcall__ gen2_place_stairs(int8u typ, int8u num, int8u walls);
void __fastcall__ gen2_place_up_stairs(int8u y, int8u x);
void __fastcall__ gen2_place_down_stairs(int8u y, int8u x);
int8u __fastcall__ gen2_next_to_walls(int8u y, int8u x);
void __fastcall__ gen2_new_spot(int8u *y, int8u *x);

void __fastcall__ tlink(void);
void __fastcall__ mlink(void);
void __fastcall__ blank_cave(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		GEN2NUM


int8u __fastcall__ gen2_popt(void)
#include "inbank\popt.c"

void __fastcall__ gen2_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"


/* Runs all the cave prep in this bank, moved from generate_cave. -SJ */
void prep_cave(void)
{
  tlink();
  mlink();
  blank_cave();
  return;
}


/* Blanks out entire cave                               -RAK-   */
void __fastcall__ blank_cave(void)
{
  vbxe_bank(VBXE_CAVBANK);
  memset(&cave[0][0], 0, sizeof(cave));
}


/* Link all free space in treasure list together                */
void __fastcall__ tlink(void)
{
  register int8u i;


  vbxe_bank(VBXE_OBJBANK);

  for (i = 0; i < MAX_TALLOC; ++i)
    gen2_invcopy(&t_list[i], OBJ_NOTHING);                      // t_list is in OBJBANK, but that bank is banked in so it's ok.
  tcptr = MIN_TRIX;
}


/* Link all free space in monster list together                 */
void __fastcall__ mlink(void)
{
  register int8u i;


  vbxe_bank(VBXE_MONBANK);

  for (i = 0; i < MAX_MALLOC; ++i)
      m_list[i] = blank_monster;
  mfptr = MIN_MONIX;
}


/* Town logic flow for generation of new town           */
void town_gen(void)
{
  register int8u i, j;
  int8u l, m, n;
  register cave_type *c_ptr;
  int8u rooms[8], k;


  vbxe_bank(VBXE_CAVBANK);

  set_seed(town_seed);
  if (classic_moria)
    for (i = 0; i < 6; ++i)
      rooms[i] = i;
  else
    for (i = 0; i < 8; ++i)
      rooms[i] = i;

  if (classic_moria) {
    l = 6;
    n = 3;
  }
  else {
    l = 8;
    n = 4;
  }
  for (i = 0; i < 2; ++i)
    for (j = 0; j < n; ++j) {
	  k = randint(l) - 1;
	  build_store(rooms[k], i, j);
	  for (m = k; m < l-1; ++m)
	    rooms[m] = rooms[m+1];
	  --l;
    }

  bank_tavern_init();					// init the tavern (only used in enhanced game -SJ

  gen2_fill_cave(DARK_FLOOR);

  /* make stairs before reset_seed, so that they don't move around */
  gen2_place_boundary();
  gen2_place_stairs(2, 1, 0);
  reset_seed();

  /* Set up the character co-ords, used by gen1_alloc_monster below */
  gen2_new_spot(&char_row, &char_col);
  if (0x1 & (turn / 5000)) {           /* Night */
    for (i = 0; i < cur_height; ++i) {
	  c_ptr = CAVE_ADR(i, 0);
	  for (j = 0; j < cur_width; ++j) {
	    if (c_ptr->fval != DARK_FLOOR)
		  c_ptr->flags |= CAVE_PL;
	    ++c_ptr;
	  }
	}
    bank_alloc_monster(MIN_MALLOC_TN, 3, TRUE, GEN2NUM);
  }
  else {           						/* Day */
    for (i = 0; i < cur_height; ++i) {
	  c_ptr = CAVE_ADR(i, 0);
	  for (j = 0; j < cur_width; ++j) {
	    c_ptr->flags |= CAVE_PL;
	    ++c_ptr;
	  }
	}
    bank_alloc_monster(MIN_MALLOC_TD, 3, TRUE, GEN2NUM);
  }

  bank_store_maint(GEN2NUM);
  return;
}


/* Builds a store at a row, column coordinate                   */
void __fastcall__ build_store(int8u store_num, int8u y, int8u x)
{
  int8u yval, y_height, y_depth;
  int8u xval, x_left, x_right;
  register int8u i, j;
  int8u cur_pos, tmp;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);

  yval     = y*10 + 5;
  if (classic_moria)
    xval     = x*16 + 16;
  else
    xval     = x*16 + 8;
  y_height = yval - randint(3);
  y_depth  = yval + randint(4);
  x_left   = xval - randint(6);
  x_right  = xval + randint(6);
  for (i = y_height; i <= y_depth; ++i)
    for (j = x_left; j <= x_right; ++j)
      (CAVE_ADR(i, j))->fval = BOUNDARY_WALL;

  tmp = randint(4);
  if (tmp < 3) {
    i = randint(y_depth-y_height) + y_height - 1;
    if (tmp == 1) j = x_left;
    else          j = x_right;
  }
  else {
    j = randint(x_right-x_left) + x_left - 1;
    if (tmp == 3) i = y_depth;
    else          i = y_height;
  }
  c_ptr = CAVE_ADR(i, j);
  c_ptr->fval  = CORR_FLOOR;
  cur_pos = gen2_popt();
  c_ptr->tptr = cur_pos;

  gen2_invcopy(&t_list[cur_pos], OBJ_STORE_DOOR + store_num);
}



/* Fills in empty spots with desired rock               -RAK-   */
/* Note: 9 is a temporary value.                                */
void __fastcall__ gen2_fill_cave(int8u fval)
{
  register int8u i, j;
  register cave_type *c_ptr;


  //vbxe_bank(VBXE_CAVERAM);

  /* no need to check the border of the cave */
  for (i = cur_height - 2; i > 0; --i)
    {
      //c_ptr = &cave[i][1];
      c_ptr = CAVE_ADR(i, 1);
      for (j = cur_width - 2; j > 0; --j)
	  {
	    if ((c_ptr->fval == NULL_WALL) || (c_ptr->fval == TMP1_WALL) ||
	       (c_ptr->fval == TMP2_WALL))
	      c_ptr->fval = fval;
	    ++c_ptr;
	  }
    }
}


/* Places indestructible rock around edges of dungeon   -RAK-   */
void gen2_place_boundary(void)
{
  register int8u i;
  register cave_type *top_ptr, *bottom_ptr;
  cave_type (*left_ptr)[MAX_WIDTH];
  cave_type (*right_ptr)[MAX_WIDTH];


  //vbxe_bank(VBXE_CAVERAM);                    // already banked in for fill_cave above -SJ

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
  bottom_ptr = &cave[cur_height - 1][0];

  for (i = 0; i < cur_width; ++i)
    {
      top_ptr->fval     = BOUNDARY_WALL;
      ++top_ptr;
      bottom_ptr->fval  = BOUNDARY_WALL;
      ++bottom_ptr;
    }
}


/* Places a staircase 1=up, 2=down                      -RAK-   */
void __fastcall__ gen2_place_stairs(int8u typ, int8u num, int8u walls)
{
  register cave_type *cave_ptr;
  int8u i, j, flag;
  register int8u y1, x1, y2, x2;


  // CAVERAM already banked in -SJ
  vbxe_bank(VBXE_CAVBANK);

  for (i = 0; i < num; ++i) {
    flag = FALSE;
    do {
	  j = 0;
	  do {
	    /* Note: don't let y1/x1 be zero, and don't let y2/x2 be equal
		 to cur_height-1/cur_width-1, these values are always
		 BOUNDARY_ROCK. */
	    y1 = randint(cur_height - 14);
	    x1 = randint(cur_width  - 14);
	    y2 = y1 + 12;
	    x2 = x1 + 12;
	    do {
		  do {
		    //cave_ptr = &cave[y1][x1];
		    cave_ptr = CAVE_ADR(y1, x1);
		    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->tptr == 0) && (gen2_next_to_walls(y1, x1) >= walls)) {
			  flag = TRUE;

			  if (typ == 1)
			    gen2_place_up_stairs(y1, x1);
			  else
			    gen2_place_down_stairs(y1, x1);
			}
		    ++x1;
		  } while ((x1 != x2) && (!flag));

		  x1 = x2 - 12;
		  ++y1;
		} while ((y1 != y2) && (!flag));
	    j++;
	  } while ((!flag) && (j <= 30));
	  --walls;
	} while (!flag);
  }
}


/* Checks points north, south, east, and west for a wall -RAK-  */
/* note that y,x is always gen1_in_bounds(), i.e. 0 < y < cur_height-1, and
   0 < x < cur_width-1  */
int8u __fastcall__ gen2_next_to_walls(int8u y, int8u x)
{
  register int8u i;
  register cave_type *c_ptr;


  // CAVERAM already banked in -SJ

  i = 0;
  //c_ptr = &cave[y-1][x];
  c_ptr = CAVE_ADR(y-1, x);						// CHECK: faster to just add/subtract to pointer? -SJ
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;
  //c_ptr = &cave[y+1][x];
  c_ptr = CAVE_ADR(y+1, x);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;
  //c_ptr = &cave[y][x-1];
  c_ptr = CAVE_ADR(y, x-1);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;
  //c_ptr = &cave[y][x+1];
  c_ptr = CAVE_ADR(y, x+1);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;

  return(i);
}


/* Place an up staircase at given y, x                  -RAK-   */
void __fastcall__ gen2_place_up_stairs(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  // CAVERAM already banked in -SJ
  // CHECK: don't even need this function in the town? -SJ

  cave_ptr = CAVE_ADR(y, x);
  //if (cave_ptr->tptr != 0)					// no objects in the town
  //  gen2_delete_object(y, x);
  cur_pos = gen2_popt();
  cave_ptr->tptr = cur_pos;

  // t_list will get banked in by gen2_incopy, and will restore the CAVERAM bank -SJ
  gen2_invcopy(&t_list[cur_pos], OBJ_UP_STAIR);
}


/* Place a down staircase at given y, x                 -RAK-   */
void __fastcall__ gen2_place_down_stairs(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  // CAVERAM already banked in -SJ

  cave_ptr = CAVE_ADR(y, x);
  //if (cave_ptr->tptr != 0)					// no objects in the town
  //  gen2_delete_object(y, x);
  cur_pos = gen2_popt();
  cave_ptr->tptr = cur_pos;

  // t_list will get banked in by gen2_incopy, and will restore the CAVERAM bank -SJ
  gen2_invcopy(&t_list[cur_pos], OBJ_DOWN_STAIR);
}


/* Returns random co-ordinates                          -RAK-   */
void __fastcall__ gen2_new_spot(int8u *y, int8u *x)
{
  register int8u i, j;
  register cave_type *c_ptr;


  // CAVERAM should already be banked in -SJ
  //vbxe_bank(VBXE_CAVBANK);						// make sure cave bank is in -SJ

  do {
    i = randint(cur_height - 2);
    j = randint(cur_width - 2);
    c_ptr = CAVE_ADR(i, j);
  } while (c_ptr->fval >= MIN_CLOSED_SPACE || (c_ptr->cptr != 0) || (c_ptr->tptr != 0));
  *y = i;
  *x = j;
}


