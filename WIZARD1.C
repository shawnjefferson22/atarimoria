/* wizard.c: wizard mode functions

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <limits.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#pragma code-name("WIZARD1BANK")
#pragma data-name("WIZARD1BANK")
#pragma rodata-name("WIZARD1BANK")


/* Local Prototypes */
void __fastcall__ wizard_erase_line(int8u row, int8u col);
int8u __fastcall__ wizard_get_check(char *prompt);
void __fastcall__ wizard_light(void);
int8u __fastcall__ wizard_cure_confusion(void);
int8u __fastcall__ wizard_cure_blindness(void);
int8u __fastcall__ wizard_cure_poison(void);
int8u __fastcall__ wizard_remove_fear(void);
int8u __fastcall__ wizard_res_stat(int8u stat);
void __fastcall__ wizard_change_speed(int8 num);
void __fastcall__ change_character(void);
void wizard_display_player(void);
void __fastcall__ wizard_create(void);
unsigned long __fastcall__ wizard_strtoul (const char* nptr);



#undef  INBANK_RETBANK
#define INBANK_RETBANK		WIZ1NUM

#undef  INBANK_MOD
#define INBANK_MOD			wizard



void __fastcall__ wizard_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

int8u __fastcall__ wizard_get_check(char *prompt)
#include "inbank\get_check.c"

int8u __fastcall__ wizard_cure_confusion(void)
#include "inbank\cure_confusion.c"

int8u __fastcall__ wizard_cure_blindness(void)
#include "inbank\cure_blindness.c"

int8u __fastcall__ wizard_cure_poison(void)
#include "inbank\cure_poison.c"

int8u __fastcall__ wizard_remove_fear(void)
#include "inbank\remove_fear.c"

int8u __fastcall__ wizard_res_stat(int8u stat)
#include "inbank\res_stat.c"

void __fastcall__ wizard_change_speed(int8 num)
#include "inbank\change_speed.c"

int8u __fastcall__ wizard_popt(void)
#include "inbank\popt.c"



/* lets anyone enter wizard mode after a disclaimer...		- JEW - */
int8u __fastcall__ enter_wiz_mode(void)
{
  int8u answer;


  if (!noscore) {
    bank_msg_print("Wizard mode is for debugging and experimenting.", WIZ1NUM);
    answer = wizard_get_check("The game will not be scored if you enter wizard mode. Are you sure?");
  }

  if (noscore || answer) {
    noscore |= 0x2;
    wizard = TRUE;
    return(TRUE);
  }

  return(FALSE);
}


/* Light up the dungeon					-RAK-	*/
void __fastcall__ wizard_light(void)
{
  register cave_type *c_ptr;
  register int8u k, l, i, j;
  int8u flag;


  vbxe_bank(VBXE_CAVBANK);
  if ((CAVE_ADR(char_row,char_col))->flags & CAVE_PL)
    flag = FALSE;
  else
    flag = TRUE;

  for (i = 0; i < cur_height; ++i)
    for (j = 0; j < cur_width; ++j)
      if ((CAVE_ADR(i,j))->fval <= MAX_CAVE_FLOOR)
		for (k = i-1; k <= i+1; ++k)
	      for (l = j-1; l <= j+1; ++l) {
	        c_ptr = CAVE_ADR(k, l);
	        if (flag)
	          c_ptr->flags |= CAVE_PL;
	        else
	          c_ptr->flags &= ~CAVE_PL;

	        if (!flag)
		      c_ptr->flags &= ~CAVE_FM;
	      }

  bank_prt_map(WIZ1NUM);
}


/* Wizard commands moved out of dungeon1 bank to here -SJ */
void __fastcall__ wizard_commands(char com_val)
{
  int8 i;
  int8u y, x;



  free_turn_flag = TRUE; // Wizard commands are free moves
  switch(com_val) {
	case CTRL('A'):	             // ^A = Cure all
	  bank_remove_curse(WIZ1NUM);
	  wizard_cure_blindness();
	  wizard_cure_confusion();
	  wizard_cure_poison();
	  wizard_remove_fear();
	  wizard_res_stat(A_STR);
	  wizard_res_stat(A_INT);
	  wizard_res_stat(A_WIS);
	  wizard_res_stat(A_CON);
	  wizard_res_stat(A_DEX);
	  wizard_res_stat(A_CHR);
      if (py.flags.slow > 1)
		py.flags.slow = 1;
	  if (py.flags.image > 1)
		py.flags.image = 1;
	  break;
	case CTRL('E'):				// ^E = wizchar
	  change_character();
	  wizard_erase_line(MSG_LINE, 0);
	  break;
    case CTRL('C'): 			// ^H print character struct
      bank_vbxe_savescreen(WIZ1NUM);
	  bank_wizard_display_player();
      bank_vbxe_restorescreen(WIZ1NUM);
	  break;
	case CTRL('F'):				// ^F = genocide
	  bank_mass_genocide(WIZ1NUM);
	  break;
	case CTRL('G'):				// ^G = treasure
	  if (command_count > 0) {
		i = command_count;
		command_count = 0;
      }
	  else
		i = 1;

	  bank_random_object(char_row, char_col, i, WIZ1NUM);
	  bank_prt_map(WIZ1NUM);
	  break;
	case CTRL('D'):				// ^D = up/down
	  if (command_count > 0) {
		if (command_count > 99)
		  i = 0;
		else
		  i = command_count;
		command_count = 0;
      }
	  else {
		bank_prt("Go to which level (0-99) ? ", 0, 0, WIZ1NUM);
		  i = -1;
		  if (bank_get_string(out_val1, 0, 27, 10, WIZ1NUM))
		    i = atoi(out_val1);
	  }
	  if (i > -1) {
		dun_level = i;
		if (dun_level > 99)
		  dun_level = 99;
		new_level_flag = TRUE;
      }
	  else
		wizard_erase_line(MSG_LINE, 0);
	  break;
	//case CTRL('O'):				// ^O = objects
	//bank_print_objects(WIZ1NUM);						// FIXME: prints random objects to a file, implement?
	//  break;
	case '\\': 					// \ wizard help
      bank_vbxe_savescreen(WIZ1NUM);
	  bank_display_wizard_help();
      bank_vbxe_restorescreen(WIZ1NUM);
	  break;
	case CTRL('I'):				// ^I = identify
	  bank_ident_spell(WIZ1NUM);
	  break;
	case '*':
	  wizard_light();
	  break;
	case ':':
	  bank_map_area(WIZ1NUM);
	  break;
	case CTRL('T'):				// ^T = teleport
	  bank_teleport(100, WIZ1NUM);
	  break;
	case '+':												// CHECK: collision with movement?
	  if (command_count > 0) {
		py.misc.exp = command_count;
		command_count = 0;
      }
	  else if (py.misc.exp == 0)
		py.misc.exp = 1;
	  else
		py.misc.exp = py.misc.exp * 2;
	  bank_prt_experience(WIZ1NUM);
	  break;
	case '&':					// & = summon
	  y = char_row;
	  x = char_col;
	  bank_summon_monster(&y, &x, TRUE, WIZ1NUM);
	  bank_creatures(FALSE, WIZ1NUM);
	  break;
	case '@':
	  wizard_create();
	  break;
	default:
	  bank_prt("Type '?' or '\\' for help.", 0, 0, WIZ1NUM);
  }

  return;
}


/* Wizard routine for gaining on stats			-RAK-	*/
void __fastcall__ change_character(void)
{
  int16 tmp_val;
  int32 tmp_lval;


  bank_prt("(3 - 118) Strength     = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	  py.stats.max_stat[A_STR] = tmp_val;
	  wizard_res_stat(A_STR);
	}
  }
  else
    return;

  bank_prt("(3 - 118) Intelligence = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	  py.stats.max_stat[A_INT] = tmp_val;
	  wizard_res_stat(A_INT);
	}
  }
  else
    return;

  bank_prt("(3 - 118) Wisdom       = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	  py.stats.max_stat[A_WIS] = tmp_val;
	  wizard_res_stat(A_WIS);
	}
  }
  else
    return;

  bank_prt("(3 - 118) Dexterity    = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	  py.stats.max_stat[A_DEX] = tmp_val;
	  wizard_res_stat(A_DEX);
	}
  }
  else
    return;

  bank_prt("(3 - 118) Constitution = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	  py.stats.max_stat[A_CON] = tmp_val;
	  wizard_res_stat(A_CON);
	}
  }
  else
    return;

  bank_prt("(3 - 118) Charisma     = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	  py.stats.max_stat[A_CHR] = tmp_val;
	  wizard_res_stat(A_CHR);
	}
  }
  else
    return;

  bank_prt("(1 - 32767) Hit points = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 5, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    //if ((tmp_val > 0) && (tmp_val <= MAX_SHORT)) {
    if (tmp_val > 0) {
	  py.misc.mhp  = tmp_val;
	  py.misc.chp  = tmp_val;
	  py.misc.chp_frac = 0;
	  bank_prt_mhp(WIZ1NUM);
	  bank_prt_chp(WIZ1NUM);
	}
  }
  else
    return;

  bank_prt("(0 - 32767) Mana       = ", 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, 25, 5, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    //if ((tmp_val > -1) && (tmp_val <= MAX_SHORT) && (*out_val1 != '\0')) {
    if ((tmp_val > -1) && (*out_val1 != '\0')) {
	  py.misc.mana  = tmp_val;
	  py.misc.cmana = tmp_val;
	  py.misc.cmana_frac = 0;
	  bank_prt_cmana(WIZ1NUM);
	}
  }
  else
    return;

  sprintf(out_val1, "Current=%ld  Gold = ", py.misc.au);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 7, WIZ1NUM)) {
    tmp_lval = atol(out_val1);
    if (tmp_lval > -1 && (*out_val1 != '\0')) {
	  py.misc.au = tmp_lval;
	  bank_prt_gold(WIZ1NUM);
	}
  }
  else
    return;

  sprintf(out_val1, "Current=%d  (0-200) Searching = ", py.misc.srh);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > -1) && (tmp_val < 201) && (*out_val1 != '\0'))
	  py.misc.srh  = tmp_val;
  }
  else
    return;

  sprintf(out_val1, "Current=%d  (-1-18) Stealth = ", py.misc.stl);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > -2) && (tmp_val < 19) && (*out_val1 != '\0'))
	  py.misc.stl  = tmp_val;
  }
  else
    return;

  sprintf(out_val1, "Current=%d  (0-200) Disarming = ", py.misc.disarm);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > -1) && (tmp_val < 201) && (*out_val1 != '\0'))
	  py.misc.disarm = tmp_val;
  }
  else
    return;

  sprintf(out_val1, "Current=%d  (0-100) Save = ", py.misc.save);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > -1) && (tmp_val < 201) && (*out_val1 != '\0'))
	  py.misc.save = tmp_val;
  }
  else
    return;

  sprintf(out_val1, "Current=%d  (0-200) Base to hit = ", py.misc.bth);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > -1) && (tmp_val < 201) && (*out_val1 != '\0'))
	  py.misc.bth  = tmp_val;
  }
  else
    return;

  sprintf(out_val1, "Current=%d  (0-200) Bows/Throwing = ", py.misc.bthb);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if ((tmp_val > -1) && (tmp_val < 201) && (*out_val1 != '\0'))
	  py.misc.bthb = tmp_val;
  }
  else
    return;

  sprintf(out_val1, "Current=%d  Weight = ", py.misc.wt);
  tmp_val = strlen(out_val1);
  bank_prt(out_val1, 0, 0, WIZ1NUM);
  if (bank_get_string(out_val1, 0, tmp_val, 3, WIZ1NUM)) {
    tmp_val = atoi(out_val1);
    if (tmp_val > -1 && (*out_val1 != '\0'))
	  py.misc.wt = tmp_val;
  }
  else
    return;

  while(bank_get_com("Alter speed? (+/-)", out_val1, WIZ1NUM)) {
    if (*out_val1 == '+')
	  wizard_change_speed(-1);
    else if (*out_val1 == '-')
	  wizard_change_speed(1);
    else
	  break;
    bank_prt_speed(WIZ1NUM);
  }
}


/* Wizard routine for creating objects			-RAK-	*/
void __fastcall__ wizard_create(void)
{
  int8 tmp8_val;
  int8u tmp8u_val;
  int16 tmp16_val;
  int16u tmp16u_val;
  int32u tmp32u_val;
  int32 tmp_lval;
  //vtype tmp_str;
  register inven_type *i_ptr;
  register cave_type *c_ptr;
  inven_type forge;
  //char pattern[4];


  bank_msg_print("Warning: This routine can cause a fatal error.", WIZ1NUM);
  i_ptr = &forge;
  i_ptr->index = OBJ_WIZARD;
  i_ptr->name2 = 0;
  //inscribe(i_ptr, "wizard item");
  strcpy(i_ptr->inscrip, "wizard item");				// in-line inscription code -SJ
  i_ptr->ident = ID_KNOWN2|ID_STOREBOUGHT;

  bank_prt("Tval   : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 3, WIZ1NUM))
    return;
  tmp8u_val = atoi(out_val1);
  i_ptr->tval = tmp8u_val;

  bank_prt("Tchar  : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 1, WIZ1NUM))
    return;
  i_ptr->tchar = out_val1[0];

  bank_prt("Subval : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 5, WIZ1NUM))
    return;
  tmp8u_val = atoi(out_val1);
  i_ptr->subval = tmp8u_val;

  bank_prt("Weight : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 5, WIZ1NUM))
    return;
  tmp16u_val = atoi(out_val1);
  i_ptr->weight = tmp16u_val;

  bank_prt("Number : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 5, WIZ1NUM))
    return;
  tmp8u_val = atoi(out_val1);
  i_ptr->number = tmp8u_val;

  bank_prt("Damage (dice): ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 15, 3, WIZ1NUM))
    return;
  tmp8u_val = atoi(out_val1);
  i_ptr->damage[0] = tmp8u_val;

  bank_prt("Damage (sides): ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 16, 3, WIZ1NUM))
    return;
  tmp8u_val = atoi(out_val1);
  i_ptr->damage[1] = tmp8u_val;

  bank_prt("+To hit: ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 3, WIZ1NUM))
    return;
  tmp8_val = atoi(out_val1);
  i_ptr->tohit = tmp8_val;

  bank_prt("+To dam: ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 3, WIZ1NUM))
    return;
  tmp8_val = atoi(out_val1);
  i_ptr->todam = tmp8_val;

  bank_prt("AC     : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 3, WIZ1NUM))
    return;
  tmp8_val = atoi(out_val1);
  i_ptr->ac = tmp8_val;

  bank_prt("+To AC : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 3, WIZ1NUM))
    return;
  tmp8_val = atoi(out_val1);
  i_ptr->toac = tmp8_val;

  bank_prt("P1     : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 5, WIZ1NUM))
    return;
  tmp16_val = atoi(out_val1);
  i_ptr->p1 = tmp16_val;

  bank_prt("Flags (In HEX): ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 16, 8, WIZ1NUM))
    return;
  /* can't be constant string, this causes problems with the GCC compiler and some scanf routines */
  //strcpy (pattern, "%lx");
  //sscanf(out_val1, pattern, &tmp_lval);
  tmp32u_val = wizard_strtoul(out_val1);
  i_ptr->flags = tmp32u_val;

  bank_prt("Cost : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 9, 8, WIZ1NUM))
    return;
  tmp_lval = atol(out_val1);
  i_ptr->cost = tmp_lval;

  bank_prt("Level : ", 0, 0, WIZ1NUM);
  if (!bank_get_string(out_val1, 0, 10, 3, WIZ1NUM))
    return;
  tmp8u_val = atoi(out_val1);
  i_ptr->level = tmp8u_val;

  if (wizard_get_check("Allocate?")) {
    /* delete object first if any, before call popt */
    vbxe_bank(VBXE_CAVBANK);
    //c_ptr = &cave[char_row][char_col];
    c_ptr = CAVE_ADR(char_row, char_col);
    if (c_ptr->tptr != 0)
	  bank_delete_object(char_row, char_col, WIZ1NUM);

	vbxe_bank(VBXE_OBJBANK);
    tmp8u_val = wizard_popt();
    t_list[tmp8u_val] = forge;

    vbxe_bank(VBXE_CAVBANK);
    c_ptr->tptr = tmp8u_val;
    bank_msg_print("Allocated.", WIZ1NUM);
  }
  else
    bank_msg_print("Aborted.", WIZ1NUM);
}


unsigned long __fastcall__ wizard_strtoul (const char* nptr)
/* Convert a string to a long unsigned int */
/* code borrowed from cc65 suite -SJ       */
{
    register const char* S          = nptr;
    unsigned long        Val        = 0;
    unsigned char        Minus      = 0;
    unsigned char        Ovf        = 0;
    unsigned             CvtCount   = 0;
    unsigned char        DigitVal;
    unsigned long        MaxVal;
    unsigned char        MaxDigit;


    /* Skip white space */
    while (isspace (*S)) {
        ++S;
    }

    /* If base is zero, we may have a 0 or 0x prefix. If base is 16, we may
    ** have a 0x prefix.
    */
    if (*S == '0' && (S[1] == 'x' || S[1] == 'X')) {
        S += 2;
    }

    /* Determine the maximum valid number and (if the number is equal to this
    ** value) the maximum valid digit.
    */
    MaxDigit = ULONG_MAX % 16;
    MaxVal   = ULONG_MAX / 16;

    /* Convert the number */
    while (1) {

        /* Convert the digit into a numeric value */
        if (isdigit(*S)) {
            DigitVal = *S - '0';
        } else if (isupper(*S)) {
            DigitVal = *S - ('A' - 10);
        } else if (islower(*S)) {
            DigitVal = *S - ('a' - 10);
        } else {
            /* Unknown character */
            break;
        }

        /* Don't accept a character that doesn't match base */
        if (DigitVal >= 16) {
            break;
        }

        /* Don't accept anything that makes the final value invalid */
        if (Val > MaxVal || (Val == MaxVal && DigitVal > MaxDigit)) {
            Ovf = 1;
        }

        /* Calculate the next value if digit is not invalid */
        if (Ovf == 0) {
            Val = (Val * 16) + DigitVal;
            ++CvtCount;
        }

        /* Next character from input */
        ++S;
    }

    /* Store the end pointer. If no conversion was performed, the value of
    ** nptr is returned in endptr.
    */
    /*if (endptr) {
        if (CvtCount > 0) {
            *endptr = (char*) S;
        } else {
            *endptr = (char*) nptr;
        }
    }*/

    /* Handle overflow */
    if (Ovf) {
        //_seterrno (ERANGE);
        return ULONG_MAX;
    }

    /* Return the result */
    return Val;
}

