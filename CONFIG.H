/* source/config.h: configuration definitions

   Copyright (c) 1989-92 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifndef CONFIG_H
#define CONFIG_H


#define CONFIG_H_INCLUDED
#ifdef CONSTANT_H_INCLUDED
//Constant.h should always be included after config.h, because it uses
//some of the system defines set up here.
#endif

/* This sets the default user interface.  */
/* To use the original key bindings (keypad for movement) set ROGUE_LIKE
   to FALSE; to use the rogue-like key bindings (vi style movement)
   set ROGUE_LIKE to TRUE.  */
/* If you change this, you only need to recompile main.c.  */
#define ROGUE_LIKE FALSE


#endif
