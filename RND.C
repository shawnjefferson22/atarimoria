/* source/rnd.c: random number generator

   Copyright (c) 1989-92 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include <atari.h>
#include <stdlib.h>


/* Put into lowcode */
#pragma code-name("CODE")
#pragma rodata-name("RODATA")
#pragma data-name("DATA")
#pragma bss-name("BSS")


int16u rnd_seed;					/* 16 bit seed */
int16u old_seed;					/* holds the previous rnd state */



int16u __fastcall__ get_rnd_seed(void)
{
  return rnd_seed;
}


void __fastcall__ set_rnd_seed(int16u seedval)
{
  rnd_seed = seedval;
  srand(seedval);
}


/* returns a pseudo-random number from set 1, 2, ..., RNG_M - 1 */
/*int32 __fastcall__ rnd(void)
{
  long ret;

  ret = rand();
  ret += ((int32u)rand() << 16);

  return(ret);
}*/


/* change to different random number generator state */
void __fastcall__ set_seed(int16u seed)
{
  old_seed = get_rnd_seed();

  /* want reproducible state here */
  set_rnd_seed(seed);
}


/* restore the normal random generator state */
void __fastcall__ reset_seed(void)
{
  set_rnd_seed(old_seed);
}


/* Generates a random integer x where 1<=X<=MAXVAL	-RAK-	*/
int16u __fastcall__ randint(int16u maxval)
{
  int16u randval;

  randval = rand();
  return ((int16u)(randval % maxval) + 1);
}


