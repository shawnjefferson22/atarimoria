/* UNIX Moria Version 5.x
   source/main.c: initialization, main() function and main loop
   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */


/* Original copyright message follows. */

/* Moria Version 4.8    COPYRIGHT (c) Robert Alan Koeneke               */
/*                                                                       */
/*       I lovingly dedicate this game to hackers and adventurers        */
/*       everywhere...                                                   */
/*                                                                       */
/*                                                                       */
/*       Designer and Programmer : Robert Alan Koeneke                   */
/*                                 University of Oklahoma                */
/*                                                                       */
/*       Assistant Programmers   : Jimmey Wayne Todd                     */
/*                                 University of Oklahoma                */
/*                                                                       */
/*                                 Gary D. McAdoo                        */
/*                                 University of Oklahoma                */
/*                                                                       */
/*       UNIX Port               : James E. Wilson                       */
/*                                 UC Berkeley                           */
/*                                 wilson@kithrup.com                    */
/*                                                                       */
/*       MSDOS Port              : Don Kneller                           */
/*                                 1349 - 10th ave                       */
/*                                 San Francisco, CA 94122               */
/*                                 kneller@cgl.ucsf.EDU                  */
/*                                 ...ucbvax!ucsfcgl!kneller             */
/*                                 kneller@ucsf-cgl.BITNET               */
/*                                                                       */
/*       BRUCE Moria             : Christopher Stuart                    */
/*                                 Monash University                     */
/*                                 Melbourne, Victoria, AUSTRALIA        */
/*                                 cjs@moncsbruce.oz                     */
/*                                                                       */
/*       Amiga Port              : Corey Gehman                          */
/*                                 Clemson University                    */
/*                                 cg377170@eng.clemson.edu              */
/*                                                                       */
/*       Version 5.5             : David Grabiner                        */
/*                                 Harvard University                    */
/*                                 grabiner@math.harvard.edu             */
/*                                                                       */
/*       Moria may be copied and modified freely as long as the above    */
/*       credits are retained.  No one who-so-ever may sell or market    */
/*       this software in any form without the expressed written consent */
/*       of the author Robert Alan Koeneke.                              */
/*                                                                       */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "interbank.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>



/* Put this code in low memory, always there */
#pragma code-name("CODE")


/* The main game loop.  This will be kept in memory, the rest of main+init will be banked out. */
void main_loop(void)
{
  /* Loop till dead, or exit */
  while(!death)
  {
	DUN1BANK;								// bank in dungeon1, we are in main memory here
    dungeon();	                            // Dungeon logic

    if (!death) {
	  GEN1BANK;								// bank in gen1, we are in main memory here
      generate_cave();		                // New level
	}
  }

  save_char();				// FIXME! exit_game should do the save. maybe?
  DEATH1BANK;				// save_char sets DUN1BANK, so change that for next call
  exit_game();          	/* Character gets buried. Doesn't Return -SJ*/

  return;
}
