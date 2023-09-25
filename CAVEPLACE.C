/* source/generate.c: initialize/create a dungeon or town level

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("PLACE1BANK")
#pragma rodata-name("PLACE1BANK")
#pragma data-name("PLACE1BANK")
#pragma bss-name("BSS")



/* Local Prototypes */
int8u __fastcall__ place1_next_to_corr(int8u y, int8u x);
int8u __fastcall__ place1_popt(void);
void __fastcall__ place1_invcopy(inven_type *to, int16u from_index);
int8u __fastcall__ place1_next_to_walls(int8u y, int8u x);
int8u __fastcall__ place1_in_bounds(int8u y, int8u x);
int16u __fastcall__ get_obj_num(int8u level);
int8u __fastcall__ delete_object(int8u y, int8u x);
int8u __fastcall__ place1_distance(int8u y1, int8u x1, int8u y2, int8u x2);
void __fastcall__ place1_pusht(int8u x);
void __fastcall__ place1_lite_spot(int8u y, int8u x);
int8u __fastcall__ place1_test_light(int8u y, int8u x);
int8u __fastcall__ place1_loc_symbol(int8u y, int8u x);
void __fastcall__ place_gold(int8u y, int8u x);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		PLACE1NUM

#undef  INBANK_MOD
#define INBANK_MOD			place1


int8u __fastcall__ place1_popt(void)
#include "inbank\popt.c"

void __fastcall__ place1_pusht(int8u x)
#include "inbank\pusht.c"

void __fastcall__ place1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ place1_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

int8u __fastcall__ place1_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

void __fastcall__ place1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ place1_test_light(int8u y, int8u x)
#include "inbank\test_light.c"


void __fastcall__ place_open_door(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_OPEN_DOOR);
  cave_ptr->fval  = CORR_FLOOR;

  vbxe_restore_bank();
}


void __fastcall__ place_broken_door(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_OPEN_DOOR);
  cave_ptr->fval  = CORR_FLOOR;

  vbxe_bank(VBXE_OBJBANK);
  t_list[cur_pos].p1 = 1;

  vbxe_restore_bank();
}


void __fastcall__ place_closed_door(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_CLOSED_DOOR);
  cave_ptr->fval  = BLOCKED_FLOOR;

  vbxe_restore_bank();
}


void __fastcall__ place_locked_door(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_CLOSED_DOOR);
  cave_ptr->fval  = BLOCKED_FLOOR;

  vbxe_bank(VBXE_OBJBANK);
  t_list[cur_pos].p1 = randint(10) + 10;

  vbxe_restore_bank();
}


void __fastcall__ place_stuck_door(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_CLOSED_DOOR);
  cave_ptr->fval  = BLOCKED_FLOOR;

  vbxe_bank(VBXE_OBJBANK);
  t_list[cur_pos].p1 = -randint(10) - 10;

  vbxe_restore_bank();
}


void __fastcall__ place_secret_door(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_SECRET_DOOR);
  cave_ptr->fval  = BLOCKED_FLOOR;

  vbxe_restore_bank();
}


void __fastcall__ place_door(int8u y, int8u x)
{
  register int8u tmp;


  tmp = randint(3);
  if (tmp == 1) {
    if (randint(4) == 1)
	  place_broken_door(y, x);
    else
	  place_open_door(y, x);
  }
  else if (tmp == 2) {
    tmp = randint(12);
    if (tmp > 3)
	  place_closed_door(y, x);
    else if (tmp == 3)
	  place_stuck_door(y, x);
    else
	  place_locked_door(y, x);
  }
  else
    place_secret_door(y, x);
}


/* Place an up staircase at given y, x                  -RAK-   */
void __fastcall__ place_up_stairs(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cave_ptr = CAVE_ADR(y, x);
  if (cave_ptr->tptr != 0)
    delete_object(y, x);
  cur_pos = place1_popt();
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_UP_STAIR);

  vbxe_restore_bank();
}


/* Place a down staircase at given y, x                 -RAK-   */
void __fastcall__ place_down_stairs(int8u y, int8u x)
{
  register int8u cur_pos;
  register cave_type *cave_ptr;


 vbxe_bank(VBXE_CAVBANK);

  cave_ptr = CAVE_ADR(y, x);
  if (cave_ptr->tptr != 0)
    delete_object(y, x);
  cur_pos = place1_popt();
  cave_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_DOWN_STAIR);

  vbxe_restore_bank();
}


/* Places a staircase 1=up, 2=down                      -RAK-   */
void __fastcall__ place_stairs(int8u typ, int8u num, int8u walls)
{
  register cave_type *cave_ptr;
  int8u i, j, flag;
  register int8u y1, x1, y2, x2;


  for (i = 0; i < num; ++i) {
    flag = FALSE;
    do {
	  j = 0;
	  do {
	    /* Note: don't let y1/x1 be zero, and don't let y2/x2 be equal
		   to cur_height-1/cur_width-1, these values are always BOUNDARY_ROCK. */
	    y1 = randint(cur_height - 14);
	    x1 = randint(cur_width  - 14);
	    y2 = y1 + 12;
	    x2 = x1 + 12;

	    do {
		  do {
		    vbxe_bank(VBXE_CAVBANK);

		    cave_ptr = CAVE_ADR(y1, x1);
		    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->tptr == 0) && (place1_next_to_walls(y1, x1) >= walls))
			{
			  flag = TRUE;
			  if (typ == 1)
			    place_up_stairs(y1, x1);
			  else
			    place_down_stairs(y1, x1);
			}
		    ++x1;
		  } while ((x1 != x2) && (!flag));

		  x1 = x2 - 12;
		  ++y1;
		} while ((y1 != y2) && (!flag));

	    ++j;
	  } while ((!flag) && (j <= 20));					// adjust j lower, was 30 -SJ

	  --walls;
	} while (!flag);
  }
}


/* Places a treasure (Gold or Gems) at given row, column -RAK-  */
void __fastcall__ place_gold(int8u y, int8u x)
{
  register int8u i, cur_pos;
  register inven_type *t_ptr;
  register cave_type *c_ptr;


  cur_pos = place1_popt();
  i = ((randint(dun_level+2)+2) / 2) - 1;
  if (randint(OBJ_GREAT) == 1)
    i += randint(dun_level+1);
  if (i >= MAX_GOLD)
    i = MAX_GOLD - 1;

  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);
  c_ptr->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_GOLD_LIST+i);

  vbxe_bank(VBXE_OBJBANK);
  t_ptr = &t_list[cur_pos];
  t_ptr->cost += (8L * (long)randint((int)t_ptr->cost)) + randint(8);

  vbxe_bank(VBXE_CAVBANK);
  if (c_ptr->cptr == 1)
    bank_msg_print ("You feel something roll beneath your feet.", PLACE1NUM);
}


/* Checks points north, south, east, and west for a wall -RAK-  */
/* note that y,x is always gen1_in_bounds(), i.e. 0 < y < cur_height-1, and
   0 < x < cur_width-1  */
int8u __fastcall__ place1_next_to_walls(int8u y, int8u x)
{
  register int8u i;
  register cave_type *c_ptr;


  i = 0;
  c_ptr = CAVE_ADR(y-1, x);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;
  c_ptr = CAVE_ADR(y+1, x);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;
  c_ptr = CAVE_ADR(y, x-1);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;
  c_ptr = CAVE_ADR(y, x+1);
  if (c_ptr->fval >= MIN_CAVE_WALL)
    ++i;

  return(i);
}


/* Places a particular trap at location y, x            -RAK-   */
void __fastcall__ place_trap(int8u y, int8u x, int8u subval)
{
  int8u cur_pos;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  (CAVE_ADR(y, x))->tptr = cur_pos;
  place1_invcopy(&t_list[cur_pos], OBJ_TRAP_LIST + subval);

  vbxe_restore_bank();
}


/* Places an object at given row, column co-ordinate    -RAK-   */
void __fastcall__ place_object(int8u y, int8u x)
{
  register int8u cur_pos;
  register int16u tmp;
  register cave_type *c_ptr;


  cur_pos = place1_popt();

  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);
  c_ptr->tptr = cur_pos;

  vbxe_bank(VBXE_OBJBANK);                                                                // bank in objbank for next two functions!
  tmp = get_obj_num(dun_level);

  place1_invcopy(&t_list[cur_pos], sorted_objects[tmp]);  // objbank still banked in here!
  bank_magic_treasure(cur_pos, dun_level, PLACE1NUM);     // is this object magical?

  vbxe_bank(VBXE_CAVBANK);                                                              // need cave bank for this next statement
  if (c_ptr->cptr == 1)                                                                 // not during generate
    bank_msg_print ("You feel something roll beneath your feet.", PLACE1NUM); /* -CJS- */

  // this function destroys the previous object bank... too many banks being used. -SJ
}


/* Place a trap with a given displacement of point      -RAK-   */
void __fastcall__ vault_trap(int8u y, int8u x, int8u yd, int8u xd, int8u num)
{
  register int8u count, y1, x1, i;
  int8u flag;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);

  for (i = 0; i < num; ++i) {
    flag = FALSE;
    count = 0;
    do {
	  y1 = y - yd - 1 + randint(2*yd+1);
	  x1 = x - xd - 1 + randint(2*xd+1);
	  c_ptr = CAVE_ADR(y1, x1);
	  if ((c_ptr->fval != NULL_WALL) && (c_ptr->fval <= MAX_CAVE_FLOOR) && (c_ptr->tptr == 0)) {
	    place_trap(y1, x1, randint(MAX_TRAP)-1);
	    flag = TRUE;
	  }
	  ++count;
	} while ((!flag) && (count <= 5));
  }

  vbxe_restore_bank();
}


/* Creates objects nearby the coordinates given         -RAK-   */
void __fastcall__ random_object(int8u y, int8u x, int16u num)
{
  register int8u i, j, k;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  do
    {
      i = 0;
      do {
	    j = y - 3 + randint(5);
	    k = x - 4 + randint(7);
	    cave_ptr = CAVE_ADR(j, k);
	    if (place1_in_bounds(j, k) && (cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0))
	    {
	      if (randint(100) < 75)
		    place_object(j, k);
	      else
		    place_gold(j, k);
	      i = 9;
	    }
	    ++i;
	  } while(i <= 10);
      --num;
    }
  while (num != 0);

  vbxe_restore_bank();
}


/* Returns the array number of a random object          -RAK-   */
int16u __fastcall__ get_obj_num(int8u level)
{
  register int16u i, j;


  vbxe_bank(VBXE_OBJBANK);                               // already banked in!

  if (level == 0)
    i = randint(t_level[0]) - 1;
  else {
    if (level >= MAX_OBJ_LEVEL)
	  level = MAX_OBJ_LEVEL;
    else if (randint(OBJ_GREAT) == 1) {
	  level = level * MAX_OBJ_LEVEL / randint (MAX_OBJ_LEVEL) + 1;
	  if (level > MAX_OBJ_LEVEL)
	    level = MAX_OBJ_LEVEL;
	}

    /* This code has been added to make it slightly more likely to get the
	higher level objects.   Originally a uniform distribution over all
	objects less than or equal to the dungeon level.  This distribution
	makes a level n objects occur approx 2/n% of the time on level n,
	and 1/2n are 0th level. */

    if (randint(2) == 1)
	  i = randint(t_level[level]) - 1;
    else /* Choose three objects, pick the highest level. */
	{
	  i = randint(t_level[level]) - 1;
	  j = randint(t_level[level]) - 1;
	  if (i < j)
	    i = j;
	  j = randint(t_level[level]) - 1;
	  if (i < j)
	    i = j;
	  j = object_list[sorted_objects[i]].level;
	  if (j == 0)
	    i = randint(t_level[0]) - 1;
	  else
	    i = randint(t_level[j]-t_level[j-1]) - 1 + t_level[j-1];
	}
  }

  //vbxe_restore_bank();
  return(i);
}


/* Places rubble at location y, x                       -RAK-   */
void __fastcall__ place_rubble(int8u y, int8u x)
{
  register int16u cur_pos;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);

  cur_pos = place1_popt();
  cave_ptr = CAVE_ADR(y, x);
  cave_ptr->tptr = cur_pos;
  cave_ptr->fval = BLOCKED_FLOOR;
  place1_invcopy(&t_list[cur_pos], OBJ_RUBBLE);

  vbxe_restore_bank();
}


/* Checks all adjacent spots for corridors              -RAK-   */
/* note that y, x is always in_bounds(), hence no need to check that
   j, k are in_bounds(), even if they are 0 or cur_x-1 is still works */
int8u __fastcall__ place1_next_to_corr(int8u y, int8u x)
{
  register int8u k, j, i;
  register cave_type *c_ptr;

  i = 0;
  for (j = y - 1; j <= (y + 1); ++j)
    for (k = x - 1; k <= (x + 1); ++k) {
	  c_ptr = CAVE_ADR(j, k);
	  /* should fail if there is already a door present */
	  if (c_ptr->fval == CORR_FLOOR && (c_ptr->tptr == 0 || t_list[c_ptr->tptr].tval < TV_MIN_DOORS))
	    ++i;
    }

  return(i);
}


/* Deletes object from given location                   -RAK-   */
int8u __fastcall__ delete_object(int8u y, int8u x)
{
  //register int8u delete;
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(y, x);
  if (c_ptr->fval == BLOCKED_FLOOR)
    c_ptr->fval = CORR_FLOOR;
  place1_pusht(c_ptr->tptr);

  vbxe_bank(VBXE_CAVBANK);						// pusht destroys bank
  c_ptr->tptr = 0;
  c_ptr->flags &= ~CAVE_FM;

  place1_lite_spot(y, x);
  if (place1_test_light(y, x))
    return(TRUE);
  else
    return(FALSE);
}


