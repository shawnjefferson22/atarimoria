/* source/generate.c: initialize/create a dungeon or town level

     Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

     This software may be copied and distributed for educational, research, and
     not for profit purposes provided that this copyright and statement are
     included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "vbxebank.h"

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <conio.h>

#pragma code-name("MON5BANK")
#pragma data-name("MON5BANK")
#pragma rodata-name("MON5BANK")


/* Local Prototypes */
int8u __fastcall__ mon5_in_bounds(int8u y, int8u x);
void __fastcall__ mon5_lite_spot(int8u y, int8u x);
int8u __fastcall__ mon5_test_light(int8u y, int8u x);
int8u __fastcall__ mon5_damroll(int8u num, int8u sides);
int8u __fastcall__ mon5_los(int8u fromY, int8u fromX, int8u toY, int8u toX);
int16u __fastcall__ mon5_summon_object(int8u y, int8u x, int8u num, int8u typ);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MON5NUM

#undef  INBANK_MOD
#define INBANK_MOD			mon5


int8u __fastcall__ mon5_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

void __fastcall__ mon5_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ mon5_test_light(int8u y, int8u x)
#include "inbank\test_light.c"

int8u __fastcall__ mon5_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ mon5_los(int8u fromY, int8u fromX, int8u toY, int8u toX)
#include "inbank\los.c"


/* Allocates objects upon a creatures death		-RAK-	*/
/* Oh well,  another creature bites the dust.  Reward the victor*/
/* based on flags set in the main creature record		 */
/* Returns a mask of bits from the given flags which indicates what the
   monster is seen to have dropped.  This may be added to monster memory. */
int32u __fastcall__ monster_death(int8u y, int8u x, int32u flags)
{
  register int8u i, number;
  int32u dump, res;


  if (flags & CM_CARRY_OBJ)
    i = 1;
  else
    i = 0;
  if (flags & CM_CARRY_GOLD)
    i += 2;

  number = 0;
  if ((flags & CM_60_RANDOM) && (randint(100) < 60))
    ++number;
  if ((flags & CM_90_RANDOM) && (randint(100) < 90))
    ++number;
  if (flags & CM_1D2_OBJ)
    number += randint(2);
  if (flags & CM_2D2_OBJ)
    number += mon5_damroll(2, 2);
  if (flags & CM_4D2_OBJ)
    number += mon5_damroll(4, 2);
  if (number > 0)
    dump = (int32u) mon5_summon_object(y, x, number, i);
  else
    dump = 0;

  if (flags & CM_WIN) {
    total_winner = TRUE;
    bank_prt_winner(MON5NUM);
    bank_msg_print("*** CONGRATULATIONS *** You have won the game.", MON5NUM);
    bank_msg_print("You cannot save this game, but you may retire when ready.", MON5NUM);
  }

  if (dump) {
    res = 0;
    if (dump & 255)
      res |= CM_CARRY_OBJ;
    if (dump >= 256)
	  res |= CM_CARRY_GOLD;
    dump = (dump % 256) + (dump / 256);    // number of items
    res |= dump << CM_TR_SHIFT;
  }
  else
    res = 0;

  return res;
}


/* Creates objects nearby the coordinates given		-RAK-	  */
int16u __fastcall__ mon5_summon_object(int8u y, int8u x, int8u num, int8u typ)
{
  register int8u i, j, k;
  register cave_type *c_ptr;
  int16u real_typ, res;


  if (typ == 1)
    real_typ = 1; 		// typ == 1 -> objects
  else
    real_typ = 256; 	// typ == 2 -> gold
  res = 0;

  do {
    i = 0;
    do {
	  j = y - 3 + randint(5);
	  k = x - 3 + randint(5);
	  if (mon5_in_bounds(j, k) && mon5_los(y, x, j, k)) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr = CAVE_ADR(j, k);
	    if (c_ptr->fval <= MAX_OPEN_SPACE && (c_ptr->tptr == 0)) {
		  if (typ == 3) {			// typ == 3 -> 50% objects, 50% gold
            if (randint(100) < 50)
			  real_typ = 1;
		    else
			  real_typ = 256;
		  }

		  if (real_typ == 1)
		    bank_place_object(j, k, MON5NUM);
		  else
		    bank_place_gold(j, k, MON5NUM);

		  mon5_lite_spot(j, k);
		  if (mon5_test_light(j, k))
		    res += real_typ;

		  i = 20;
		}
	  }
	  ++i;
	} while (i <= 20);
    --num;
  } while (num != 0);

  return res;
}


