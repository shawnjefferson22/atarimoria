/* source/help.c: identify a symbol

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"



#pragma code-name("HELP1BANK")
#pragma rodata-name("HELP1BANK")
#pragma data-name("HELP1BANK")


static char *sym_desc[128] = {
	CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, 	// 15
	CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, CNIL, 	// 31
    "  - An open pit.", 						// SPACE
    "! - A potion.",							// !
	"\" - An amulet, periapt, or necklace.",	// \
	"# - A stone wall.",						// #
	"$ - Treasure.",							// $
	CNIL,										// %
	"& - Treasure chest.",						// &
	"' - An open door.",						// '
	"( - Soft armor.",							// (
	") - A shield.",							// )
	"* - Gems.",								// *
	"+ - A closed door.",						// +
	", - Food or mushroom patch.",				// ,
	"- - A wand",								// -
	". - Floor.",								// .
	"/ - A pole weapon.",						// /
	"0 - Not used.",							// 0
	"1 - Entrance to General Store.",			// 1
	"2 - Entrance to Armory.",					// 2
	"3 - Entrance to Weaponsmith.",				// 3
	"4 - Entrance to Temple.",					// 4
	"5 - Entrance to Alchemy shop.",			// 5
	"6 - Entrance to Magic-Users store.",		// 6
	"7 - Not used.",							// 7
	"8 - Not used.",							// 8
	"9 - Not used.",							// 9
	": - Rubble.",								// :
	"; - A loose rock.",						// ;
	"< - An up staircase.",						// <
	"= - A ring.",								// =
	"> - A down staircase.",					// >
	"? - A scroll.",							// ?
	CNIL,										// @
	"A - Giant Ant Lion.",						// A
	"B - The Balrog.",							// B
	"C - Gelatinous Cube.",						// C
	"D - An Ancient Dragon (Beware).",			// D
	"E - Elemental.",							// E
	"F - Giant Fly.",							// F
	"G - Ghost.",								// G
	"H - Hobgoblin.",							// H
	"I - Invisible Stalker.",					// I
	"J - Jelly.",								// J
	"K - Killer Beetle.",						// K
	"L - Lich.",								// L
	"M - Mummy.",								// M
	"N - Not used.",							// N
	"O - Ooze.",								// O
	"P - Giant humanoid.",						// P
	"Q - Quylthulg (Pulsing Flesh Mound).",		// Q
	"R - Reptile.",								// R
	"S - Giant Scorpion.",						// S
	"T - Troll.",								// T
	"U - Umber Hulk.",							// U
	"V - Vampire.",								// V
	"W - Wight or Wraith.",						// W
	"X - Xorn.",								// X
	"Y - Yeti.",								// Y
	"Z - Not used.",							// Z
	"[ - Hard armor.",							// [
	"\\ - A hafted weapon.",					// \
	"] - Misc. armor.",							// ]
	"^ - A trap.",								// ^
	"_ - A staff.",								// _
	"` - Not used.",							// `
	"a - Giant Ant.",							// a
	"b - Giant Bat.",							// b
	"c - Giant Centipede.",						// c
	"d - Dragon.",								// d
	"e - Floating Eye.",						// e
	"f - Giant Frog.",							// f
	"g - Golem.",								// g
	"h - Harpy.",								// h
	"i - Icky Thing.",							// i
	"j - Jackal.",								// j
	"k - Kobold.",								// k
	"l - Giant Louse.",							// l
	"m - Mold.",								// m
	"n - Naga.",								// n
	"o - Orc or Ogre.",							// o
	"p - Person (Humanoid).",					// p
	"q - Quasit.",								// q
	"r - Rodent.",								// r
	"s - Skeleton.",							// s
	"t - Giant Tick.",							// t
	"u - Not used.",							// u
	"v - Not used.",							// v
	"w - Worm or Worm Mass.",					// w
	"x - Not used.",							// x
	"y - Yeek.",								// y
	"z - Zombie.",								// z
	"{ - Arrow, bolt, or bullet.",				// {
	"| - A sword or dagger.",					// |
	"} - Bow, crossbow, or sling.",				// }
	"~ - Miscellaneous item.",					// ~
	"\t - Not Used"								// TAB
};


static char *help_screen1[23] = {
"Command summary: (@ is optional count, \x1F is direction, ESC aborts)",
"  c \x1F     Close a door.             |   C       Character player description.",
"  d       Drop an item.             | @ D \x1F     Disarm a trap/chest.",
"  e       Equipment list.           |   E       Eat some food.",
"@ f \x1F     Force (bash) item or mons.|   F       Fill lamp with oil.",
"  i       Inventory list.           |   G       Gain new magic spells.",
"  m       magic spell casting.      |   M       Map, show reduced size.",
"@ o \x1F     Open a door/chest.        |   P       Peruse a book.",
"  p       Pray.                     |   Q       Quit the game.",
"  q       Quaff a potion.           | @ R       Rest (Count or *=restore).",
"  r       Read a scroll.            | @ S \x1F     Spike a door.",
"@ s       Search for traps or doors.|   T       Take off an item.",
"  t       Throw an item.            |   V       View scores.",
"  v       Version info              |   W       Where: locate self.",
"  w       Wear/Wield an item.       |   X       Exchange weapon.",
"  x \x1F     Examine surroundings      |   Z       Zap a staff.",
"  z       Zap a wand.               |   #       Search Mode.",
"  =       Set options.              |   <       Go up an up-staircase.",
"  /       Identify a character.     |   >       Go down a down-staircase.",
"@ CTRL-P  Previous message review   |   [       Inscribe an object.",
"@ - \x1F     Move without pickup.      |   ?       Type this page.",
"@ CTRL  \x1F Tunnel in a direction.    |   CTRL-X  Save character and exit.",
"@ SHIFT \x1F Run in direction.         | @ \x1F       For movement."
};

static char *help_screen2[22] = {
"Directions:     y  k  u",
"                h  .  l  [. is stay]",
"                b  j  n",
"To give a count to a command, type the number in digits, then the command.",
"A count of 0 defaults to a count of 99. Counts only work with some commands,",
"and will be terminated by the same things that end a rest or a run. In",
"particular, typing any character during the execution of a counted command",
"will terminate the command. Counted searches or tunnels will terminate on",
"success, or if you are attacked. A count with control-P will specify the",
"number of previous messages to display.",
"",
"Control commands may be entered with a single key stroke, or with two key",
"strokes by typing ^ and then a letter.",
"",
"Type ESCAPE to abort the look command at any point.",
"",
"Some commands will prompt for a spell, or an inventory item. Selection is",
"by an alphabetic character - entering a capital causes a desription to be",
"printed, and the selection may be aborted.",
"",
"Typing 'R*' will make you rest until both your mana and your hp reach their",
"maximum values.",
};


static char *wizard_help[14] = {
"*  - Wizard light.",
":  - Map area.",
"\\  - Wizard Help.",
"^A - Remove Curse and Cure all maladies.",
"^D - Down/Up n levels.",
"^E - Change character.",
"^F - Delete monsters.",
"^G - Allocate treasures.",
"^I - Identify.",
"^T - Teleport player.",
"^W - Wizard mode on/off.",
"@  - Create any object *CAN CAUSE FATAL ERROR*",
"+  - Gain experience.",
"&  - Summon monster.",
};


/* Local Prototypes */
void __fastcall__ help_erase_line(int8u row, int8u col);
void __fastcall__ help_put_buffer(char *out_str, int8u row, int8u col);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		HELP1NUM


void __fastcall__ help_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

/* Dump IO to buffer                                    -RAK-   */
void __fastcall__ help_put_buffer(char *out_str, int8u row, int8u col)
#include "inbank\put_buffer.c"


void __fastcall__ ident_char(void)
{
  char command, query;
  register int16u i;
  register int8u n;


  if (bank_get_com("Enter character to be identified :", &command, HELP1NUM)) {
    if ((command < 32) || (command > 127))		// non-printable character
      return;

	switch(command) {
	  case '%':
	    if (highlight_seams == TRUE)
	      bank_prt("% - A magma or quartz vein.", 0, 0, HELP1NUM);
	    else
	      bank_prt("% - Not used.", 0, 0, HELP1NUM);
	    break;

      case '@':
        bank_prt(py.misc.name, 0, 0, HELP1NUM);
        break;

      default:
        bank_prt(sym_desc[command], 0, 0, HELP1NUM);
        break;
	}
  }

  /* Allow access to monster memory. -CJS- */
  n = 0;
  for (i = 0 ; i < MAX_CREATURES; ++i) {
    vbxe_bank(VBXE_MONBANK);
    if ((c_list[i].cchar == command) && bank_bool_roff_recall(i)) {
	  if (n == 0) {
	    help_put_buffer("You recall those details? [y/n]", 0, 40);
	    query = cgetc();
	    if (query != 'y' && query != 'Y')
	      break;
	    help_erase_line(0, 40);
	    bank_vbxe_savescreen(HELP1NUM);
	  }
	  n++;
	  query = bank_roff_recall(i, HELP1NUM);
	  bank_vbxe_restorescreen(HELP1NUM);
	  if (query == ESCAPE)
	    break;
    }
  }
}


void __fastcall__ display_help(void)
{
  register int8u i;


  for(i=0; i<23; ++i)
	bank_prt(help_screen1[i], i, 0, HELP1NUM);

  bank_prt("[Press any key to continue.]", 23, 23, HELP1NUM);
  cgetc();

  bank_vbxe_clear(HELP1NUM);

  for(i=0; i<22; ++i)
	bank_prt(help_screen2[i], i, 0, HELP1NUM);

  bank_prt("[Press any key to continue.]", 23, 23, HELP1NUM);
  cgetc();
}


void __fastcall__ display_wizard_help(void)
{
  register int8u i;


  bank_vbxe_clear(HELP1NUM);
  for(i=0; i<14; ++i)
	bank_prt(wizard_help[i], i, 0, HELP1NUM);

  bank_prt("[Press any key to continue.]", 23, 23, HELP1NUM);
  cgetc();
}


