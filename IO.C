/* source/io.c: terminal I/O code, uses the curses package

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <conio.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "vbxetext.h"


#pragma bss-name("BSS")

cave_type loc_c;										// temp cave for use in loc_symbol functions -SJ


#pragma code-name("PRINTBANK")
#pragma rodata-name("PRINTBANK")
#pragma data-name("PRINTBANK")

/* Color lookup table for map symbols */
char color_lookup[128] = { 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,   // 15
						   0x03, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,   // 31
						//  SPC   !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
						   0x0F, 0x0F, 0x0F, 0x06, 0x1E, 0x08, 0x0F, 0x17, 0x0F, 0x0F, 0x37, 0x17, 0x0F, 0x0F, 0x03, 0x0F,   // 47
						//  0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
						   0x0F, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x1B, 0x0F, 0x1B, 0x0F,   // 63
						//  @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
						   0x0F, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,   // 79
						//  P     Q     R     S     T     U     V     W     X     Y     Z	  [     \     ]     ^     _
						   0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,   // 95
						//  DIA   a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
						   0x0F, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,   // 111
						//  p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~    SEAM
						   0x31, 0x31, 0x31, 0x0E, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x0F, 0x0F, 0x0F, 0x0F, 0x04 }; // 127


/* Local prototype to keep from banking */
int8u __fastcall__ screen_map_loc_symbol(int8u y, int8u x);
void __fastcall__ msg_print(char *str_buff);
int8u __fastcall__ io_isprint(char c);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		PRINTNUM


/* Dump IO to buffer                                    -RAK-   */
void __fastcall__ put_buffer(char *out_str, int8u row, int8u col)
{
  vtype tmp_str;


  /* truncate the string, to make sure that it won't go past right edge of
     screen */
  if (col > 79)
    col = 79;
  strncpy (tmp_str, out_str, 79 - col);
  tmp_str[79 - col] = '\0';

  vbxe_cputsxy(col, row, tmp_str, 0x0F);
}


/* Dump IO to buffer -RAK-    */
/* Allows to choose color -SJ */
/*void __fastcall__ put_buffer_color(char *out_str, int8u row, int8u col, int8u color)
{
  vtype tmp_str;


  // truncate the string, to make sure that it won't go past right edge of screen
  if (col > 79)
    col = 79;
  strncpy (tmp_str, out_str, 79 - col);
  tmp_str[79 - col] = '\0';

  vbxe_cputsxy(col, row, tmp_str, color);
}*/


/* Implemented this in the cartridge bank instead of main ram -SJ */
int8u __fastcall__ io_isprint(char c)
{
  switch(c) {
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
      return(FALSE);
    default:
      return(TRUE);
  }
}


/* Returns a single character input from the terminal.  This silently -CJS-
   consumes ^R to redraw the screen and reset the terminal, so that this
   operation can always be performed at any input prompt.  inkey() never
   returns ^R.  */
char __fastcall__ inkey(void)
{
  char i;

  command_count = 0;  /* Just to be safe -CJS- */
  i = cgetc();
  return i;
}


/* Clears given line of text                            -RAK-   */
void __fastcall__ erase_line(int8u row, int8u col)
{
  if (row == MSG_LINE && msg_flag)
    msg_print("");

  vbxe_cleartoeol(col, row);
}


/* Clears screen */
/*void __fastcall__ clear_screen(void)
{
  if (msg_flag)
    msg_print("");

  vbxe_clear();
}*/


void __fastcall__ clear_from(int8u row)
{
  vbxe_cleartobot(row);
}


/* Outputs a char to a given interpolated y, x position -RAK-   */
/* sign bit of a character used to indicate standout mode. -CJS */
void __fastcall__ print(char ch, int8u row, int8u col)
{
  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;

  vbxe_cputcxy(col, row, ch, color_lookup[ch]);
}


/* Print a message so as not to interrupt a counted command. -CJS- */
void __fastcall__ count_msg_print(char *p)
{
  int8u i;


  i = command_count;
  msg_print(p);
  command_count = i;
}


/* Outputs a line to a given y, x position -RAK-   */
void __fastcall__ prt(char *str_buff, int8u row, int8u col)
{
  vtype tmp_str;

  // clear any waiting messages
  if (row == MSG_LINE && msg_flag)
    msg_print("");

  // clear the line starting from col
  vbxe_cleartoeol(col, row);

  // copied put_buffer code here for speed -SJ
  //put_buffer(str_buff, row, col);

  /* truncate the string, to make sure that it won't go past right edge of screen */
  if (col > 79)
    col = 79;
  strncpy (tmp_str, str_buff, 79 - col);
  tmp_str[79 - col] = '\0';

  vbxe_cputsxy(col, row, tmp_str, 0x0F);
}


/* Outputs a line to a given y, x position -RAK-   */
/* Allows to pick color -SJ */
void __fastcall__ prt_color(char *str_buff, int8u row, int8u col, int8u color)
{
  vtype tmp_str;

  // clear any waiting messages
  if (row == MSG_LINE && msg_flag)
    msg_print("");

  // clear the line starting from col
  vbxe_cleartoeol(col, row);

  // copied put_buffer code here for speed -SJ
  //put_buffer(str_buff, row, col);

  /* truncate the string, to make sure that it won't go past right edge of screen */
  if (col > 79)
    col = 79;
  strncpy (tmp_str, str_buff, 79 - col);
  tmp_str[79 - col] = '\0';

  vbxe_cputsxy(col, row, tmp_str, color);
}


/* Outputs message to top line of screen         */
/* These messages are kept for later reference.  */
/* Have to rewrite slightly, since str_buff will never be NULL, but might point to an empty string -SJ */
/* An empty string pauses with -more- until the user presses a key.  -SJ */
void __fastcall__ msg_print(char *str_buff)
{
  register int8u old_len, new_len;
  char in_char;
  int8u combine_messages = FALSE;


  new_len = strlen(str_buff);
  if (msg_flag) {
    old_len = strlen(old_msg[last_msg]) + 1;
     /* If the new message and the old message are short enough, we want
	 display them together on the same line.  So we don't flush the old
	 message in this case.  */

    if (!new_len || (new_len + old_len + 2 >= 73)) {		// test for zero len, not null -SJ
	  if (old_len > 73)
	    old_len = 73;
	  put_buffer(" -more-", MSG_LINE, old_len);
	  do {
		while(!kbhit()) {				// if no key is pressed 		-SJ
		  if (STRIG0 == 0) {			// if joystick button pressed
		    in_char = ' ';				// pretend we hit the spacebar
		    break;
	  	  }
		}

		if (kbhit())					// the user pressed a key		-SJ
		  in_char = cgetc();			// get it

	  } while ((in_char != ' ') && (in_char != ESCAPE) && (in_char != '\n') && (in_char != '\r'));
	}
    else
	  combine_messages = TRUE;
  }

  if (!combine_messages)
    vbxe_cleartoeol(0, MSG_LINE);

  /* Make the null string a special case.  -CJS- */
  if (new_len) {											// test for zero len, not null -SJ
    command_count = 0;
    msg_flag = TRUE;

    /* If the new message and the old message are short enough, display them on the same line.  */
    if (combine_messages) {
	  put_buffer(str_buff, MSG_LINE, old_len + 2);
	  strcat(old_msg[last_msg], "  ");
	  strcat(old_msg[last_msg], str_buff);
	}
    else {
	  put_buffer(str_buff, MSG_LINE, 0);
	  ++last_msg;
	  if (last_msg >= MAX_SAVE_MSG)
	    last_msg = 0;
	  strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
	  old_msg[last_msg][VTYPESIZ - 1] = '\0';
	}
  }
  else
    msg_flag = FALSE;
}


/* Used to verify a choice - user gets the chance to abort choice.  -CJS- */
//int8u __fastcall__ get_check(char *prompt)
//#include "inbank\get_check.c"


/* Prompts (optional) and returns ord value of input char       */
/* Function returns false if <ESCAPE> is input  */
int8u __fastcall__ get_com(char *prompt, char *command)
{
  int8u res;

  if (strlen(prompt) > 0)
    prt(prompt, 0, 0);
  //*command = inkey();
  *command = cgetc();
  if (*command == ESCAPE)
    res = FALSE;
  else
    res = TRUE;
  erase_line(MSG_LINE, 0);
  return(res);
}


/* Gets a string terminated by <RETURN>         */
/* Function returns false if <ESCAPE> is input  */
int8u __fastcall__ get_string(char *in_str, int8u row, int8u column, int8u slen)
{
  register int8u start_col, end_col;
  register char i;
  register char *p;
  register int8u flag, aborted;


  aborted = FALSE;
  flag  = FALSE;

  //(void) move(row, column);                                   // this code clears a portion of the screen
  //for (i = slen; i > 0; i--)
  //  (void) addch(' ');
  //(void) move(row, column);

  vbxe_clearxy(column, row, slen);

  start_col = column;
  end_col = column + slen - 1;
  if (end_col > 79)
    {
      slen = 80 - column;
      end_col = 79;
    }
  p = in_str;
  do
    {
      i = inkey();
      switch(i)
	  {
	    case ESCAPE:                                                                        // this should be 0x27 on Atari
	      aborted = TRUE;
	      break;
	    case RETURN:
	      flag = TRUE;
	      break;
	    case DELETE:                                                                        // delete should work on Atari
	      if (column > start_col)
	      {
		--column;
		vbxe_clearxy(column, row, 1);
		//put_buffer(" ", row, column);
		//move_cursor(row, column);
		*--p = '\0';
	      }
	      break;
	    default:
	      if (!io_isprint(i) || column > end_col)
		 //bell();
		    ;
	      else
	      {
		    vbxe_cputcxy(column, row, i, 0x1F);					// output in yellow
		*p++ = i;
		column++;
	      }
	    break;
	  }
    }
  while ((!flag) && (!aborted));

  if (aborted)
    return(FALSE);

  /* Remove trailing blanks     */
  while (p > in_str && p[-1] == ' ')
    p--;

  *p = '\0';
  return(TRUE);
}


/* Returns symbol for given row, column			-RAK-	*/
int8u __fastcall__ screen_map_loc_symbol(int8u y, int8u x)
{
  register cave_type *cave_ptr;			// in VBXE CAVBANK


  /* Save the cave data locally */
  vbxe_bank(VBXE_CAVBANK);

  cave_ptr = CAVE_ADR(y, x);
  loc_c.cptr = cave_ptr->cptr;		// copy the cave data locally
  loc_c.tptr = cave_ptr->tptr;		// memcpy faster?
  loc_c.fval = cave_ptr->fval;
  loc_c.flags = cave_ptr->flags;

  /* Check monsters and player first, need to appear on top of other items */
  vbxe_bank(VBXE_MONBANK);
  if ((loc_c.cptr == 1) && (!find_flag || find_prself))
    return SYM_PLAYER;
  else if (py.flags.status & PY_BLIND)
    return ' ';
  else if ((py.flags.image > 0) && (randint (12) == 1))
    return randint (95) + 31;
  else if ((loc_c.cptr > 1) && (m_list[loc_c.cptr].ml))
    return c_list[m_list[loc_c.cptr].mptr].cchar;
  else if (!(loc_c.flags & CAVE_PL) && !(loc_c.flags & CAVE_TL) && !(loc_c.flags & CAVE_FM))  	// only print those parts that are lit
    return ' ';

  /* Check objects first then floor and walls */
  vbxe_bank(VBXE_OBJBANK);
  if ((loc_c.tptr != 0) && (t_list[loc_c.tptr].tval != TV_INVIS_TRAP))
    return t_list[loc_c.tptr].tchar;
  else if (loc_c.fval <= MAX_CAVE_FLOOR)
    return SYM_FLOOR;
  else if (loc_c.fval == GRANITE_WALL || loc_c.fval == BOUNDARY_WALL || highlight_seams == FALSE)
    return SYM_WALL;
  else
    return SYM_SEAM;
}


/* Prints the map of the dungeon			-RAK-	*/
void __fastcall__ prt_map(void)
{
  register int8u i, j, k;
  register char tmp_char;
  register int8u color;


  k = 0;
  color = 0;
  for (i = panel_row_min; i <= panel_row_max; ++i)  	// Top to bottom
  {
    ++k;
    //erase_line(k, 13);
    vbxe_cleartoeol(13, k);								// call vbxe function directly, saves a jsr -SJ
    for (j = panel_col_min; j <= panel_col_max; ++j)	// Left to right
	{
	  tmp_char = screen_map_loc_symbol(i, j);
	  if (tmp_char != ' ') {
		color = color_lookup[tmp_char];
		//color += 1;						// debugging colors
	    //print(tmp_char, i, j);
	    vbxe_cputcxy((j - panel_col_min + 13), k, tmp_char, color);
      }
	}
  }
}


/* Outputs a char to a given interpolated y, x position -RAK-   */
/* sign bit of a character used to indicate standout mode. -CJS */
/* Combined the two functions together here. -SJ				*/
void __fastcall__ print_symbol(int8u row, int8u col)
{
  register cave_type *cave_ptr;		// in VBXE CAVBANK
  int8u color;						// color to display character
  char ch;							// character to display


  vbxe_bank(VBXE_CAVBANK);
  cave_ptr = CAVE_ADR(row, col);
  loc_c.cptr = cave_ptr->cptr;			// copy the cave data locally
  loc_c.tptr = cave_ptr->tptr;			// memcpy faster? -SJ
  loc_c.fval = cave_ptr->fval;
  loc_c.flags = cave_ptr->flags;

  vbxe_bank(VBXE_MONBANK);			// bank in the monster list
  if ((loc_c.cptr == 1) && (!find_flag || find_prself))
    ch = SYM_PLAYER;
  else if (py.flags.status & PY_BLIND)
    ch = ' ';
  else if ((py.flags.image > 0) && (randint (12) == 1))
    ch = randint (95) + 31;
  else if ((loc_c.cptr > 1) && (m_list[loc_c.cptr].ml))
    ch = c_list[m_list[loc_c.cptr].mptr].cchar;
  else if (!(loc_c.flags & CAVE_PL) && !(loc_c.flags & CAVE_TL) && !(loc_c.flags & CAVE_FM))  	// only print those parts that are lit
    ch = ' ';
  else {								// only look for objects if nothing hits above -SJ
    vbxe_bank(VBXE_OBJBANK);			// bank in the object list
    if ((loc_c.tptr != 0) && (t_list[loc_c.tptr].tval != TV_INVIS_TRAP))
      ch = t_list[loc_c.tptr].tchar;
    else if (loc_c.fval <= MAX_CAVE_FLOOR)
      ch = SYM_FLOOR;
    else if (loc_c.fval == GRANITE_WALL || loc_c.fval == BOUNDARY_WALL || highlight_seams == FALSE)
      ch = SYM_WALL;
    else
      ch = SYM_SEAM;
  }

  color = color_lookup[ch];			// determine color to print
  row -= panel_row_prt;				// Real co-ords convert to screen positions
  col -= panel_col_prt;

  vbxe_cputcxy(col, row, ch, color);
}


/* Outputs a char to a given interpolated y, x position -RAK-   */
/* highlights the symbol, using inverse video -SJ				*/
/* Combined the two functions together here. -SJ				*/
void __fastcall__ print_symbol_highlight(int8u row, int8u col)
{
  register cave_type *cave_ptr;		// in VBXE CAVBANK
  int8u color;						// color to display character
  char ch;							// character to display


  vbxe_bank(VBXE_CAVBANK);
  cave_ptr = CAVE_ADR(row, col);
  loc_c.cptr = cave_ptr->cptr;			// copy the cave data locally
  loc_c.tptr = cave_ptr->tptr;			// memcpy faster? -SJ
  loc_c.fval = cave_ptr->fval;
  loc_c.flags = cave_ptr->flags;

  vbxe_bank(VBXE_MONBANK);			// bank in the monster list
  if ((loc_c.cptr == 1) && (!find_flag || find_prself))
    ch = SYM_PLAYER;
  else if (py.flags.status & PY_BLIND)
    ch = ' ';
  else if ((py.flags.image > 0) && (randint (12) == 1))
    ch = randint (95) + 31;
  else if ((loc_c.cptr > 1) && (m_list[loc_c.cptr].ml))
    ch = c_list[m_list[loc_c.cptr].mptr].cchar;
  else if (!(loc_c.flags & CAVE_PL) && !(loc_c.flags & CAVE_TL) && !(loc_c.flags & CAVE_FM))  	// only print those parts that are lit
    ch = ' ';
  else {								// only look for objects if nothing hits above -SJ
    vbxe_bank(VBXE_OBJBANK);			// bank in the object list
    if ((loc_c.tptr != 0) && (t_list[loc_c.tptr].tval != TV_INVIS_TRAP))
      ch = t_list[loc_c.tptr].tchar;
    else if (loc_c.fval <= MAX_CAVE_FLOOR)
      ch = SYM_FLOOR;
    else if (loc_c.fval == GRANITE_WALL || loc_c.fval == BOUNDARY_WALL || highlight_seams == FALSE)
      ch = SYM_WALL;
    else
      ch = SYM_SEAM;
  }

  //color = color_lookup[ch];			// determine color to print
  //color += 0x80;     				// highlight character
  color = 0x80;

  row -= panel_row_prt;				// Real co-ords convert to screen positions
  col -= panel_col_prt;

  /*for (color = 0; color < 0x80; ++color) {
    vbxe_cputcxy(0, 0, ch, color);
    vbxe_cputcxy(1, 0, ch, color + 0x80);
    vbxe_cputcxy(2, 0, ch, color | 0x80);
    cgetc();
  }*/

  vbxe_cputcxy(col, row, ch, color);
}


/* Outputs a char to a given interpolated y, x position -RAK-   */
/* sign bit of a character used to indicate standout mode. -CJS */
void __fastcall__ print_self(int8u row, int8u col)
{
  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;

  vbxe_cputcxy(col, row, SYM_PLAYER, color_lookup[SYM_PLAYER]);
}


/* definitions used by screen_map() */
/* index into border character array */
#define TL 0    /* top left */
#define TR 1
#define BL 2
#define BR 3
#define HE 4    /* horizontal edge */
#define VE 5

/* character set to use */
//#   define CH(x)      (screen_border[0][x])
#define CH(x)   (screen_border[x])

const int8u screen_border[6] = { 0x11, 0x05, 0x1A, 0x03, 0x12, 0x7C };

  /* Display highest priority object in the RATIO by RATIO area */
#define RATIO 3


#pragma bss-name("CAVERAM")

char map[66];						// MAP will be 66x22 (cave is 132x44), so 66 chars in one line
int8u priority[128];				// 128 characters in ASCII character set


void __fastcall__ screen_map(void)
{
  register int8u i, j;
  int8u color;
  char tmp;
  register int8u row, col;


  vbxe_bank(VBXE_CAVBANK);

  /* Setup the priority array */
  memset(&priority, 0, 128);			// clear the priority array

  priority['<'] 		 = 20;			// set some priorites
  priority['>'] 		 = 20;
  priority[SYM_PLAYER]   = 100;
  priority[SYM_WALL] 	 = 10;
  priority[SYM_FLOOR]  	 = 5;
  priority['+']			 = 2;
  priority['\''] 		 = 2;
  priority[' '] 		 = 0;

  vbxe_savescreen();					// save
  vbxe_clear();							// and clear the screen

  /* Draw the top border */
  vbxe_cputcxy(6, 0, CH(TL), 0x0F);
  for (i = 0; i < 66; ++i) {
    vbxe_cputcxy(7+i, 0, CH(HE), 0x0F);
  }
  vbxe_cputcxy(72, 0, CH(TR), 0x0F);

  /* Scan the map and find priority items */
  row = 1;
  for (i = 0; i < MAX_HEIGHT; i+=2) {
    memset(&map, ' ', 66);							// clear the map priority string
    for (j = 0; j < MAX_WIDTH; ++j) {				// first row
	  col = j / 2;
	  tmp = screen_map_loc_symbol(i, j);			// find the character at this location
	  vbxe_bank(VBXE_CAVBANK);						// bring the cave ram bank in again
	  if (priority[map[col]] < priority[tmp])
	    map[col] = tmp;
	}

    for (j = 0; j < MAX_WIDTH; ++j) {				// second row
	  col = j / 2;
	  tmp = screen_map_loc_symbol(i+1, j);
	  vbxe_bank(VBXE_CAVBANK);						// bring the cave ram bank in again
	  if (priority[map[col]] < priority[tmp])
	    map[col] = tmp;
	}

    /* Print the row */
    vbxe_cputcxy(6, row, CH(VE), 0x0F);
    for(j=0; j < 66; ++j) {
      if (map[j] != ' ') {
        color = color_lookup[map[j]];					// determine color to print
        vbxe_cputcxy(7+j, row, map[j], color);
  	  }
	}
    vbxe_cputcxy(72, row, CH(VE), 0x0F);

	++row;
  }

  /* Draw the bottom border */
  vbxe_cputcxy(6, row, CH(BL), 0x0F);
  for (i = 0; i < 66; ++i) {
    vbxe_cputcxy(7+i, row, CH(HE), 0x0F);
  }
  vbxe_cputcxy(72, row, CH(BR), 0x0F);
  vbxe_cputsxy(29, row, "Hit any key to continue", 0x0F);

  cgetc();
  vbxe_restorescreen();
}


/* Prints a block of text starting at line passed in, with word wrap,
 * between columns START_COL and END_COL defined below.  Text is expected
 * to be in the GEN_BANK.  -SJ */
void __fastcall__ pretty_print(char *text, int8u line)
{
  register int16u s, i, k;
  int16u len;
  int8u l;

  #define START_COL		2
  #define END_COL		76


  /* Get the working text */
  vbxe_bank(VBXE_GENBANK);
  len = strlen(text);

  l = line;
  i = 0;
  k = 0;
  s = 0;
  while (i < len) {
    out_val1[s] = text[i];								// copy to line buffer
    ++s;    											// increment line buffer position
    if (s > END_COL) {									// filled this line?
      for(k=i; k>0; --k) {								// scan backward for space
        if (text[k] == ' ') {							// found a space
		  out_val1[END_COL-(i-k)] = '\0';				// truncate out string at space
		  i = k+1;										// reset back to k, +1 to ignore space
		  //bank_prt(out_val1, l, START_COL, TAVERN1NUM);	// print this line
		  vbxe_cputsxy(START_COL, l, out_val1, 0x0F);	// call vbxe cputsxy in this bank
		  ++l;											// next line
		  s = 0;										// reset outval position
		  break;										// break out of look back loop
		}
      }
    }
    else												// out string not full yet
      ++i;												// increment source pos
  }

  out_val1[s] = '\0';									// truncate last line
  //bank_prt(out_val1, l, START_COL, TAVERN1NUM);			// print last line
  vbxe_cputsxy(START_COL, l, out_val1, 0x0F);			// call vbxe cputsxy in this bank

  vbxe_restore_bank();
  return;
}
