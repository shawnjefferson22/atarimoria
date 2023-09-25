/* source/generate.c: initialize/create a dungeon or town level

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


/* Local Prototypes */



#pragma code-name("GEN3BANK")
#pragma rodata-name("GEN3BANK")
#pragma data-name("GEN3BANK")
#pragma bss-name("BSS")



/* Builds a room at a row, column coordinate            -RAK-   */
/* Type 1 unusual rooms are several overlapping rectangular ones        */
void __fastcall__ build_type1(int8u yval, int8u xval)
{
  int8u y_height, y_depth;
  int8u x_left, x_right, limit;
  int8u floor, i0;
  register int8u i, j;
  register cave_type *c_ptr, *d_ptr;


  if (dun_level <= randint(25))
    floor = LIGHT_FLOOR;        /* Floor with light     */
  else
    floor = DARK_FLOOR;         /* Dark floor           */

  limit = 1 + randint(2);
  for (i0 = 0; i0 < limit; ++i0)
    {
      y_height = yval - randint(4);
      y_depth  = yval + randint(3);
      x_left   = xval - randint(11);
      x_right  = xval + randint(11);


      vbxe_bank(VBXE_CAVBANK);

      /* the x dim of rooms tends to be much larger than the y dim, so don't
	 bother rewriting the y loop */

      for (i = y_height; i <= y_depth; ++i)
	  {
	    //c_ptr = &cave[i][x_left];
	    c_ptr = CAVE_ADR(i, x_left);
	    for (j = x_left; j <= x_right; ++j)
	    {
	      c_ptr->fval  = floor;
	      c_ptr->flags |= CAVE_LR;
	      ++c_ptr;
	    }
	  }
      for (i = (y_height - 1); i <= (y_depth + 1); ++i)
	  {
	    //c_ptr = &cave[i][x_left-1];
	    c_ptr = CAVE_ADR(i, x_left-1);
	    if (c_ptr->fval != floor)
	    {
	      c_ptr->fval  = GRANITE_WALL;
	      c_ptr->flags |= CAVE_LR;
	    }
	    //c_ptr = &cave[i][x_right+1];
	    c_ptr = CAVE_ADR(i, x_right+1);
	    if (c_ptr->fval != floor)
	    {
	      c_ptr->fval  = GRANITE_WALL;
	      c_ptr->flags |= CAVE_LR;
	    }
	  }
      //c_ptr = &cave[y_height - 1][x_left];
      //d_ptr = &cave[y_depth + 1][x_left];
      c_ptr = CAVE_ADR(y_height-1, x_left);
      d_ptr = CAVE_ADR(y_depth+1, x_left);
      for (i = x_left; i <= x_right; ++i)
	  {
	    if (c_ptr->fval != floor)
	    {
	      c_ptr->fval  = GRANITE_WALL;
	      c_ptr->flags |= CAVE_LR;
	    }
	    ++c_ptr;
	    if (d_ptr->fval != floor)
	    {
	      d_ptr->fval  = GRANITE_WALL;
	      d_ptr->flags |= CAVE_LR;
	    }
	    ++d_ptr;
	  }
    }
}


/* Builds an unusual room at a row, column coordinate   -RAK-   */
/* Type 2 unusual rooms all have an inner room:                 */
/*   1 - Just an inner room with one door                       */
/*   2 - An inner room within an inner room                     */
/*   3 - An inner room with pillar(s)                           */
/*   4 - Inner room has a maze                                  */
/*   5 - A set of four inner rooms                              */
void __fastcall__ build_type2(int8u yval, int8u xval)
{
  register int8u i, j;
  int8u y_height, x_left;
  int8u y_depth, x_right, tmp;
  int8u floor;
  register cave_type *c_ptr, *d_ptr;


  if (dun_level <= randint(25))
    floor = LIGHT_FLOOR;        /* Floor with light     */
  else
    floor = DARK_FLOOR;         /* Dark floor           */
  y_height = yval - 4;
  y_depth  = yval + 4;
  x_left   = xval - 11;
  x_right  = xval + 11;

  vbxe_bank(VBXE_CAVBANK);

  /* the x dim of rooms tends to be much larger than the y dim, so don't bother rewriting the y loop */
  for (i = y_height; i <= y_depth; ++i) {
    c_ptr = CAVE_ADR(i, x_left);
    for (j = x_left; j <= x_right; ++j) {
	  c_ptr->fval  = floor;
	  c_ptr->flags |= CAVE_LR;
	  c_ptr++;
	}
  }

  for (i = (y_height - 1); i <= (y_depth + 1); ++i) {
    c_ptr = CAVE_ADR(i, x_left-1);
    c_ptr->fval   = GRANITE_WALL;
    c_ptr->flags |= CAVE_LR;
    c_ptr = CAVE_ADR(i, x_right+1);
    c_ptr->fval  = GRANITE_WALL;
    c_ptr->flags |= CAVE_LR;
  }

  c_ptr = CAVE_ADR(y_height-1, x_left);
  d_ptr = CAVE_ADR(y_depth+1, x_left);
  for (i = x_left; i <= x_right; ++i) {
    c_ptr->fval  = GRANITE_WALL;
    c_ptr->flags |= CAVE_LR;
    c_ptr++;
    d_ptr->fval   = GRANITE_WALL;
    d_ptr->flags |= CAVE_LR;
    d_ptr++;
  }

  /* The inner room             */
  y_height = y_height + 2;
  y_depth  = y_depth  - 2;
  x_left   = x_left   + 2;
  x_right  = x_right  - 2;

  for (i = (y_height - 1); i <= (y_depth + 1); ++i) {
    cave[i][x_left-1].fval = TMP1_WALL;
    cave[i][x_right+1].fval = TMP1_WALL;
  }
  c_ptr = CAVE_ADR(y_height-1, x_left);
  d_ptr = CAVE_ADR(y_depth+1, x_left);
  for (i = x_left; i <= x_right; ++i) {
    c_ptr->fval = TMP1_WALL;
    ++c_ptr;
    d_ptr->fval = TMP1_WALL;
    ++d_ptr;
  }

  /* Inner room variations              */
  switch(randint(5)) {
    case 1:     /* Just an inner room.  */
      tmp = randint(4);
      if (tmp < 3) {    /* Place a door */
	    if (tmp == 1) bank_place_secret_door(y_height-1, xval, GEN3NUM);
	    else          bank_place_secret_door(y_depth+1, xval, GEN3NUM);
      }
      else {
	    if (tmp == 3) bank_place_secret_door(yval, x_left-1, GEN3NUM);
	    else          bank_place_secret_door(yval, x_right+1, GEN3NUM);
      }
      bank_vault_monster(yval, xval, 1);
      break;

    case 2:     /* Treasure Vault       */
      tmp = randint(4);
      if (tmp < 3) {    /* Place a door */
	    if (tmp == 1) bank_place_secret_door(y_height-1, xval, GEN3NUM);
	    else          bank_place_secret_door(y_depth+1, xval, GEN3NUM);
      }
      else {
	    if (tmp == 3) bank_place_secret_door(yval, x_left-1, GEN3NUM);
	    else          bank_place_secret_door(yval, x_right+1, GEN3NUM);
      }

      for (i = yval-1; i <= yval+1; ++i) {
	    cave[i][xval-1].fval   = TMP1_WALL;
	    cave[i][xval+1].fval   = TMP1_WALL;
	  }
      cave[yval-1][xval].fval  = TMP1_WALL;
      cave[yval+1][xval].fval  = TMP1_WALL;

      tmp = randint(4); /* Place a door */
      if (tmp < 3)
	    bank_place_locked_door(yval-3+(tmp<<1), xval, GEN3NUM); /* 1 -> yval-1; 2 -> yval+1*/
      else
	    bank_place_locked_door(yval, xval-7+(tmp<<1), GEN3NUM);

      /* Place an object in the treasure vault  */
      tmp = randint(10);
      if (tmp > 2)
	    bank_place_object(yval, xval, GEN3NUM);				// destroys vbxe bank, but that should be ok here
      else if (tmp == 2)
	    bank_place_updown_stairs(yval, xval, DOWNSTAIRS, GEN3NUM);
      else
	    bank_place_updown_stairs(yval, xval, UPSTAIRS, GEN3NUM);

      /* Guard the treasure well                */
      bank_vault_monster(yval, xval, 2+randint(3));
      /* If the monsters don't get 'em. */
      bank_vault_trap(yval, xval, 4, 10, 2+randint(3));
      break;

    case 3:     /* Inner pillar(s).     */
      tmp = randint(4);
      if (tmp < 3) {    /* Place a door */
	    if (tmp == 1) bank_place_secret_door(y_height-1, xval, GEN3NUM);
	    else          bank_place_secret_door(y_depth+1, xval, GEN3NUM);
      }
      else {
	    if (tmp == 3) bank_place_secret_door(yval, x_left-1, GEN3NUM);
	    else          bank_place_secret_door(yval, x_right+1, GEN3NUM);
      }

      for (i = yval-1; i <= yval+1; ++i)
	  {
	    c_ptr = CAVE_ADR(i, xval-1);
	    for (j = xval-1; j <= xval+1; ++j)
	    {
	      c_ptr->fval = TMP1_WALL;
	      ++c_ptr;
	    }
	  }
      if (randint(2) == 1)
	  {
	    tmp = randint(2);
	    for (i = yval-1; i <= yval+1; ++i)
	    {
	      c_ptr = CAVE_ADR(i, xval-5-tmp);
	      for (j = xval-5-tmp; j <= xval-3-tmp; ++j)
		  {
		    c_ptr->fval = TMP1_WALL;
		    ++c_ptr;
		  }
	    }
	    for (i = yval-1; i <= yval+1; ++i)
	    {
	      c_ptr = CAVE_ADR(i, xval+3+tmp);
	      for (j = xval+3+tmp; j <= xval+5+tmp; ++j)
		{
		  c_ptr->fval  = TMP1_WALL;
		  ++c_ptr;
		}
	  }
	}

    if (randint(3) == 1)      /* Inner rooms  */
	{
	  c_ptr = CAVE_ADR(yval-1, xval-5);
	  d_ptr = CAVE_ADR(yval+1, xval-5);
	  for (i = xval-5; i <= xval+5; ++i)
	    {
	      c_ptr->fval  = TMP1_WALL;
	      ++c_ptr;
	      d_ptr->fval  = TMP1_WALL;
	      ++d_ptr;
	    }
	  cave[yval][xval-5].fval = TMP1_WALL;
	  cave[yval][xval+5].fval = TMP1_WALL;
	  bank_place_secret_door(yval-3+(randint(2)<<1), xval-3, GEN3NUM);
	  bank_place_secret_door(yval-3+(randint(2)<<1), xval+3, GEN3NUM);
	  if (randint(3) == 1)  bank_place_object(yval, xval-2, GEN3NUM);
	  if (randint(3) == 1)  bank_place_object(yval, xval+2, GEN3NUM);
	  bank_vault_monster(yval, xval-2, randint(2));
	  bank_vault_monster(yval, xval+2, randint(2));
	}
    break;

    case 4:     /* Maze inside. */
      tmp = randint(4);
      if (tmp < 3) {    /* Place a door */
	    if (tmp == 1) bank_place_secret_door(y_height-1, xval, GEN3NUM);
	    else          bank_place_secret_door(y_depth+1, xval, GEN3NUM);
      } else {
	    if (tmp == 3) bank_place_secret_door(yval, x_left-1, GEN3NUM);
	    else          bank_place_secret_door(yval, x_right+1, GEN3NUM);
      }

      for (i = y_height; i <= y_depth; ++i)
	    for (j = x_left; j <= x_right; ++j)
	      if (0x1 & (j+i))
	        cave[i][j].fval = TMP1_WALL;

      /* Monsters just love mazes.              */
      bank_vault_monster(yval, xval-5, randint(3));
      bank_vault_monster(yval, xval+5, randint(3));
      /* Traps make them entertaining.  */
      bank_vault_trap(yval, xval-3, 2, 8, randint(3));
      bank_vault_trap(yval, xval+3, 2, 8, randint(3));
      /* Mazes should have some treasure too..  */
      for (i = 0; i < 3; ++i)
	    bank_random_object(yval, xval, 1, GEN3NUM);
      break;

    case 5:     /* Four small rooms.    */
      for (i = y_height; i <= y_depth; ++i)
	    cave[i][xval].fval = TMP1_WALL;

      //c_ptr = &cave[yval][x_left];
      c_ptr = CAVE_ADR(yval, x_left);
      for (i = x_left; i <= x_right; ++i)
	  {
	    c_ptr->fval = TMP1_WALL;
	    ++c_ptr;
	  }

      if (randint(2) == 1)
	  {
	    i = randint(10);
	    bank_place_secret_door(y_height-1, xval-i, GEN3NUM);
	    bank_place_secret_door(y_height-1, xval+i, GEN3NUM);
	    bank_place_secret_door(y_depth+1, xval-i, GEN3NUM);
	    bank_place_secret_door(y_depth+1, xval+i, GEN3NUM);
	  }
      else
	  {
	    i = randint(3);
	    bank_place_secret_door(yval+i, x_left-1, GEN3NUM);
	    bank_place_secret_door(yval-i, x_left-1, GEN3NUM);
	    bank_place_secret_door(yval+i, x_right+1, GEN3NUM);
	    bank_place_secret_door(yval-i, x_right+1, GEN3NUM);
	  }

      /* Treasure in each one.          */
      bank_random_object(yval, xval, 2+randint(2), GEN3NUM);
      /* Gotta have some monsters.              */
      bank_vault_monster(yval+2, xval-4, randint(2));
      bank_vault_monster(yval+2, xval+4, randint(2));
      bank_vault_monster(yval-2, xval-4, randint(2));
      bank_vault_monster(yval-2, xval+4, randint(2));
      break;
    }
}


/* Builds a room at a row, column coordinate            -RAK-   */
/* Type 3 unusual rooms are cross shaped                                */
void __fastcall__ build_type3(int8u yval, int8u xval)
{
  int8u y_height, y_depth;
  int8u x_left, x_right;
  register int8u tmp, i, j;
  int8u floor;
  register cave_type *c_ptr;


  if (dun_level <= randint(25))
    floor = LIGHT_FLOOR;        /* Floor with light     */
  else
    floor = DARK_FLOOR;         /* Dark floor           */
  tmp = 2 + randint(2);
  y_height = yval - tmp;
  y_depth  = yval + tmp;
  x_left   = xval - 1;
  x_right  = xval + 1;

  vbxe_bank(VBXE_CAVBANK);

  for (i = y_height; i <= y_depth; ++i)
    for (j = x_left; j <= x_right; ++j)
      {
	//c_ptr = &cave[i][j];
	c_ptr = CAVE_ADR(i, j);
	c_ptr->fval = floor;
	c_ptr->flags |= CAVE_LR;
      }
  for (i = (y_height - 1); i <= (y_depth + 1); ++i)
    {
      //c_ptr = &cave[i][x_left-1];
      c_ptr = CAVE_ADR(i, x_left-1);
      c_ptr->fval  = GRANITE_WALL;
      c_ptr->flags |= CAVE_LR;
      //c_ptr = &cave[i][x_right+1];
      c_ptr = CAVE_ADR(i, x_right+1);
      c_ptr->fval  = GRANITE_WALL;
      c_ptr->flags |= CAVE_LR;
    }
  for (i = x_left; i <= x_right; ++i)
    {
      //c_ptr = &cave[y_height-1][i];
      c_ptr = CAVE_ADR(y_height-1, i);
      c_ptr->fval  = GRANITE_WALL;
      c_ptr->flags |= CAVE_LR;
      //c_ptr = &cave[y_depth+1][i];
      c_ptr = CAVE_ADR(y_depth+1, i);
      c_ptr->fval  = GRANITE_WALL;
      c_ptr->flags |= CAVE_LR;
    }
  tmp = 2 + randint(9);
  y_height = yval - 1;
  y_depth  = yval + 1;
  x_left   = xval - tmp;
  x_right  = xval + tmp;
  for (i = y_height; i <= y_depth; ++i)
    for (j = x_left; j <= x_right; ++j)
      {
	//c_ptr = &cave[i][j];
	c_ptr = CAVE_ADR(i, j);
	c_ptr->fval = floor;
	c_ptr->flags |= CAVE_LR;
      }
  for (i = (y_height - 1); i <= (y_depth + 1); ++i)
    {
      //c_ptr = &cave[i][x_left-1];
      c_ptr = CAVE_ADR(i, x_left-1);
      if (c_ptr->fval != floor)
	{
	  c_ptr->fval  = GRANITE_WALL;
	  c_ptr->flags |= CAVE_LR;
	}
      //c_ptr = &cave[i][x_right+1];
      c_ptr = CAVE_ADR(i, x_right+1);
      if (c_ptr->fval != floor)
	{
	  c_ptr->fval  = GRANITE_WALL;
	  c_ptr->flags |= CAVE_LR;
	}
    }
  for (i = x_left; i <= x_right; ++i)
    {
      //c_ptr = &cave[y_height-1][i];
      c_ptr = CAVE_ADR(y_height-1, i);
      if (c_ptr->fval != floor)
	{
	  c_ptr->fval  = GRANITE_WALL;
	  c_ptr->flags |= CAVE_LR;
	}
      //c_ptr = &cave[y_depth+1][i];
      c_ptr = CAVE_ADR(y_depth+1, i);
      if (c_ptr->fval != floor)
	{
	  c_ptr->fval  = GRANITE_WALL;
	  c_ptr->flags |= CAVE_LR;
	}
    }

  /* Special features.                  */
  switch(randint(4))
    {
    case 1:     /* Large middle pillar          */
      for (i = yval-1; i <= yval+1; ++i)
	{
	  //c_ptr = &cave[i][xval-1];
	  c_ptr = CAVE_ADR(i, xval-1);
	  for (j = xval-1; j <= xval+1; ++j)
	    {
	      c_ptr->fval = TMP1_WALL;
	      ++c_ptr;
	    }
	}
      break;

    case 2:     /* Inner treasure vault         */
      for (i = yval-1; i <= yval+1; ++i)
	{
	  cave[i][xval-1].fval   = TMP1_WALL;
	  cave[i][xval+1].fval   = TMP1_WALL;
	}
      cave[yval-1][xval].fval  = TMP1_WALL;
      cave[yval+1][xval].fval  = TMP1_WALL;

      tmp = randint(4); /* Place a door */
      if (tmp < 3)
	    bank_place_secret_door(yval-3+(tmp<<1), xval, GEN3NUM);
      else
	    bank_place_secret_door(yval, xval-7+(tmp<<1), GEN3NUM);

      /* Place a treasure in the vault          */
      bank_place_object(yval, xval, GEN3NUM);
      /* Let's guard the treasure well. */
      bank_vault_monster(yval, xval, 2+randint(2));
      /* Traps naturally                        */
      bank_vault_trap(yval, xval, 4, 4, 1+randint(3));
      break;

    case 3:
      if (randint(3) == 1)
	{
	  cave[yval-1][xval-2].fval = TMP1_WALL;
	  cave[yval+1][xval-2].fval = TMP1_WALL;
	  cave[yval-1][xval+2].fval = TMP1_WALL;
	  cave[yval+1][xval+2].fval = TMP1_WALL;
	  cave[yval-2][xval-1].fval = TMP1_WALL;
	  cave[yval-2][xval+1].fval = TMP1_WALL;
	  cave[yval+2][xval-1].fval = TMP1_WALL;
	  cave[yval+2][xval+1].fval = TMP1_WALL;
	  if (randint(3) == 1)
	    {
	      bank_place_secret_door(yval, xval-2, GEN3NUM);
	      bank_place_secret_door(yval, xval+2, GEN3NUM);
	      bank_place_secret_door(yval-2, xval, GEN3NUM);
	      bank_place_secret_door(yval+2, xval, GEN3NUM);
	    }
	}
      else if (randint(3) == 1)
	{
	  cave[yval][xval].fval = TMP1_WALL;
	  cave[yval-1][xval].fval = TMP1_WALL;
	  cave[yval+1][xval].fval = TMP1_WALL;
	  cave[yval][xval-1].fval = TMP1_WALL;
	  cave[yval][xval+1].fval = TMP1_WALL;
	}
      else if (randint(3) == 1)
	    cave[yval][xval].fval = TMP1_WALL;
      break;

    case 4:
      break;
    }
}

