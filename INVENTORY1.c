/* Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <atari.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "vbxetext.h"


#pragma code-name("INVEN1BANK")
#pragma rodata-name("INVEN1BANK")


/* Prototypes of functions copied into this bank */
void __fastcall__ inven1_put_buffer(char *out_str, int8 row, int8 col);
void __fastcall__ inven1_erase_line(int8u row, int8u col);
int8u __fastcall__ inven1_get_check(char *prompt);
void __fastcall__ inven1_invcopy(inven_type *to, int16u from_index);
int8 __fastcall__ inven1_object_offset(inven_type *t_ptr);
int8u __fastcall__ inven1_known1_p(inven_type *i_ptr);
void __fastcall__ inven1_known1(inven_type *i_ptr);
void __fastcall__ inven_destroy(int8u item_val);
int16u __fastcall__ inven1_weight_limit(void);
void __fastcall__ inven1_show_inven(int8u r1, int8u r2, int8u weight, char *mask);
void __fastcall__ inven1_show_equip(int8u weight);
void __fastcall__ inven1_takeoff(int8u item_val, int8 posn);
int8u __fastcall__ inventory1_isupper(char c);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		INVEN1NUM

#undef  INBANK_MOD
#define INBANK_MOD			inven1


void __fastcall__ inven1_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

int8u __fastcall__ inven1_popt(void)
#include "inbank\popt.c"



/* Implemented in cartridge bank to get out of main RAM -SJ */
int8u __fastcall__ inventory1_isupper(char c)
{
  if ((c > 64) && (c < 91))
    return(TRUE);
  else
    return(FALSE);
}


void __fastcall__ inven1_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

int8u __fastcall__ inven1_get_check(char *prompt)
#include "inbank\get_check.c"

void __fastcall__ inven1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8 __fastcall__ inven1_object_offset(inven_type *t_ptr)                                      // changed return type to int8 SJ
#include "inbank\object_offset.c"

void __fastcall__ inven1_known1(inven_type *i_ptr)
#include "inbank\known1.c"

int8u __fastcall__ inven1_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"


/* Displays equipment items from r1 to end	-RAK-	*/
/* Keep display as far right as possible. -CJS- */
void __fastcall__ inven1_show_equip(int8u weight)
{
  register int8u i, line;
  int8u total_weight;
  register char *prt1;
  register inven_type *i_ptr;


  line = 0;
  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; ++i) /* Range of equipment */
  {
    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[i];
    if (i_ptr->tval != TV_NOTHING) {
	  switch(i)	     /* Get position	      */
	  {
	    case INVEN_WIELD:
	      if (py.stats.use_stat[A_STR]*15 < i_ptr->weight)
		    prt1 = "Just lifting";
	      else
		    prt1 = "Wielding";
	      break;
	    case INVEN_HEAD:
	      prt1 = "On head"; break;
	    case INVEN_NECK:
	      prt1 = "Around neck"; break;
	    case INVEN_BODY:
	      prt1 = "On body"; break;
	    case INVEN_ARM:
	      prt1 = "On arm"; break;
	    case INVEN_HANDS:
	      prt1 = "On hands"; break;
	    case INVEN_RIGHT:
	      prt1 = "On right hand"; break;
	    case INVEN_LEFT:
	      prt1 = "On left hand"; break;
	    case INVEN_FEET:
	      prt1 = "On feet"; break;
	    case INVEN_OUTER:
	      prt1 = "About body"; break;
	    case INVEN_LIGHT:
	      prt1 = "Light source"; break;
	    case INVEN_AUX:
	      prt1 = "Spare weapon"; break;
	    default:
	      prt1 = "Unknown value"; break;
	  }

	  bank_objdes(out_val1, i_ptr, TRUE, INVEN1NUM);
	  sprintf(out_val2, " %c) %-14s: %s  ", line+'a', prt1, out_val1);

      bank_prt(out_val2, line+1, 0, INVEN1NUM);
	  if (weight) {
	    vbxe_bank(VBXE_OBJBANK);
	    total_weight = i_ptr->weight*i_ptr->number;
	    sprintf(out_val2, "%3d.%d lb", (total_weight) / 10, (total_weight) % 10);
	    bank_prt(out_val2, line+1, 71, INVEN1NUM);
	  }
	  ++line;
	}
  }

  return;
}


/* Displays inventory items from r1 to r2	-RAK-	*/
/* Designed to keep the display as far to the right as possible.  The  -CJS-
   parameter col gives a column at which to start, but if the display does
   not fit, it may be moved left.  The return value is the left edge used. */
/* If mask is non-zero, then only display those items which have a non-zero
   entry in the mask array.  */
/* Re-write of this function to just assume screen is cleared already, and
   left justify the display. -SJ */
void __fastcall__ inven1_show_inven(int8u r1, int8u r2, int8u weight, char *mask)
{
  register int8u i;
  int8u total_weight, current_line;


  current_line = 1;
  vbxe_bank(VBXE_OBJBANK);

  for (i = r1; i <= r2; ++i) { 				// Print the items
    if (mask == CNIL || mask[i]) {
	  bank_objdes(out_val1, &inventory[i], TRUE, INVEN1NUM);
	  sprintf(out_val2, " %c) %s  ", 'a'+i, out_val1);
	  bank_prt(out_val2, current_line, 0, INVEN1NUM);

	  vbxe_bank(VBXE_OBJBANK);				// if objdes destroys VBXE bank CHECK
	  if (weight) {
	    total_weight = inventory[i].weight*inventory[i].number;
	    sprintf(out_val1, "%3d.%d lb", (total_weight) / 10, (total_weight) % 10);
	    bank_prt(out_val1, current_line, 71, INVEN1NUM);
	  }
	  ++current_line;
	}
  }

  //inven1_erase_line(current_line, 0);		// erase one extra line -SJ
  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", current_line, 0, INVEN1NUM);

  return;
}


/* Used to verify if this really is the item we wish to	 -CJS-
   wear or read. */
int8u __fastcall__ inven1_verify(char *prompt, int8u item)
{
  //bigvtype out_str, object;				// let's try using out_val1 and out_val2


  vbxe_bank(VBXE_OBJBANK);

  bank_objdes(out_val1, &inventory[item], TRUE, INVEN1NUM);
  out_val1[strlen(out_val1)-1] = '?'; 			/* change the period to a question mark */
  sprintf(out_val2, "%s %s", prompt, out_val1);

  return inven1_get_check(out_val2);
}


/* Get the ID of an item and return the CTR value of it	-RAK-	*/
int8u __fastcall__ get_item(int8u *com_val, char *pmt, int8u i, int8u j, char *mask, char *message)
{
  //vtype out_val;						// use out_val1 instead?
  char which;
  int8u test_flag, item;
  int8u full, redraw;
  int8 i_scr;
  register int8u m;


  item = FALSE;
  redraw = FALSE;
  *com_val = 0;
  i_scr = 1;

  if (j > INVEN_WIELD) {
    full = TRUE;
    if (inven_ctr == 0) {
	  i_scr = 0;
	  j = equip_ctr - 1;
	}
    else
	  j = inven_ctr - 1;
  }
  else
    full = FALSE;

  if (inven_ctr > 0 || (full && equip_ctr > 0)) {
    do {
	  if (redraw) {
	    if (i_scr > 0) {
		  inven1_show_inven(i, j, FALSE, mask);
	    }
	    else
		  inven1_show_equip(FALSE);
	  }

	  if (full)
	    sprintf(out_val1, "(%s: %c-%c,%s / for %s, or ESC) %s", (i_scr > 0 ? "Inven" : "Equip"), i+'a', j+'a',
	            (redraw ? "" : " * to see,"), (i_scr > 0 ? "Equip" : "Inven"), pmt);
	  else
	    sprintf(out_val1, "(Items %c-%c,%s ESC to exit) %s", i+'a', j+'a', (redraw ? "" : " * for inventory list,"), pmt);

	  test_flag = FALSE;
	  bank_prt(out_val1, 0, 0, INVEN1NUM);

	  do {
	    which = cgetc();
	    switch(which) {
		  case ESCAPE:
		    test_flag = TRUE;
		    free_turn_flag = TRUE;
		    i_scr = -1;
		    break;
		  case '/':
		    if (full) {
		      if (i_scr > 0) {
			    if (equip_ctr == 0) {
			      bank_prt("But you're not using anything -more-",0,0, INVEN1NUM);
			      cgetc();
			    }
			    else {
			      i_scr = 0;
			      test_flag = TRUE;
			      if (redraw) {
				    j = equip_ctr;
				    while (j < inven_ctr) {
				      ++j;
				      inven1_erase_line(j, 0);
				    }
				}

			    j = equip_ctr - 1;
			  }
			  bank_prt(out_val1, 0, 0, INVEN1NUM);
			}
		    else {
			  if (inven_ctr == 0) {
			    bank_prt("But you're not carrying anything -more-",0,0, INVEN1NUM);
			    cgetc();
			  }
			  else {
			    i_scr = 1;
			    test_flag = TRUE;
			    if (redraw) {
				  j = inven_ctr;
				  while (j < equip_ctr) {
				    ++j;
				    inven1_erase_line(j, 0);
				  }
				}

			    j = inven_ctr - 1;
			  }
			}
		  }
		  break;
		case '*':
		  if (!redraw) {
		    test_flag = TRUE;
		    bank_vbxe_savescreen(INVEN1NUM);
		    redraw = TRUE;
		  }
		  break;
		default:
		  //if ((which >= '0') && (which <= '9') && (i_scr != 0)) {
		  if ((which >= '0') && (which <= '9')) {
		    /* look for item whose inscription matches "which" */
		    for (m = i; (m < INVEN_WIELD) && ((inventory[m].inscrip[0] != which) || (inventory[m].inscrip[1] != '\0')); ++m);
		    if (m < INVEN_WIELD)
		      *com_val = m;
		    else
		      *com_val = 255;			// was -1 -SJ
		  }
		  else
		    if (inventory1_isupper(which))
		      *com_val = which - 'A';
		    else
		      *com_val = which - 'a';

		  if ((*com_val >= i) && (*com_val <= j) && (mask == CNIL || mask[*com_val])) {
		    if (i_scr == 0) {
			  i = 21;
			  j = *com_val;
			  //do {
		      while (j != 0) {
				vbxe_bank(VBXE_OBJBANK);
			    while (inventory[++i].tval == TV_NOTHING);
			    --j;
			  }
			  //while (j >= 0);
			  *com_val = i;
			}
		    if (inventory1_isupper(which) && !inven1_verify("Try", *com_val)) {
			  test_flag = TRUE;
			  free_turn_flag = TRUE;
			  i_scr = -1;
			  break;
			}

		    test_flag = TRUE;
		    item = TRUE;
		    i_scr = -1;
		  }
		  else if (strlen(message) > 0) {								// can't use CNIL -SJ
		    bank_msg_print (message, INVEN1NUM);
		    /* Set test_flag to force redraw of the question.  */
		    test_flag = TRUE;
		  }
		  break;
		}
	  } while (!test_flag);
	} while (i_scr >= 0);

    if (redraw)
	  bank_vbxe_restorescreen(INVEN1NUM);

    inven1_erase_line(MSG_LINE, 0);
  }
  else
    bank_prt("You are not carrying anything.", 0, 0, INVEN1NUM);

  return(item);
}


/* Destroy an item in the inventory			-RAK-	*/
void __fastcall__ inven_destroy(int8u item_val)
{
  register int8u j;
  register inven_type *i_ptr;


  vbxe_bank(VBXE_OBJBANK);

  i_ptr = &inventory[item_val];
  if ((i_ptr->number > 1) && (i_ptr->subval <= ITEM_SINGLE_STACK_MAX)) {
    i_ptr->number--;
    inven_weight -= i_ptr->weight;
  }
  else {
    inven_weight -= i_ptr->weight*i_ptr->number;
    for (j = item_val; j < inven_ctr-1; ++j)
	  memcpy(&inventory[j], &inventory[j+1], sizeof(inven_type));
    inven1_invcopy(&inventory[inven_ctr-1], OBJ_NOTHING);
    --inven_ctr;
  }

  py.flags.status |= PY_STR_WGT;
  vbxe_restore_bank();
}


/* Append an additional comment to an object description.	-CJS- */
/*void __fastcall__ inven1_add_inscribe(inven_type *i_ptr, int8u type)
{
  i_ptr->ident |= type;
}*/  // just in-line this code! -SJ


/* Somethings been identified					*/
/* extra complexity by CJS so that it can merge store/dungeon objects
   when appropriate */
void __fastcall__ identify(int8u *item)
{
  register int8u i;
  int8u x1, x2;
  int8u j;
  register inven_type *i_ptr, *t_ptr;


  vbxe_bank(VBXE_OBJBANK);
  i_ptr = &inventory[*item];

  if (i_ptr->flags & TR_CURSED)
    //add_inscribe(i_ptr, ID_DAMD);						// replace function call with in-line code -SJ
    i_ptr->ident |= ID_DAMD;

  if (!inven1_known1_p(i_ptr)) {
    inven1_known1(i_ptr);
    x1 = i_ptr->tval;
    x2 = i_ptr->subval;
    if (x2 < ITEM_SINGLE_STACK_MIN || x2 >= ITEM_GROUP_MIN)
	/* no merging possible */
	  ;
    else
	  for (i = 0; i < inven_ctr; ++i) {
	    t_ptr = &inventory[i];
	    if (t_ptr->tval == x1 && t_ptr->subval == x2 && i != *item && ((int)t_ptr->number + (int)i_ptr->number < 256)) {
		  /* make *item the smaller number */
		  if (*item > i) {
		    j = *item; *item = i; i = j;
		  }

	      bank_msg_print("You combine similar objects from the shop and dungeon.", INVEN1NUM);

		  inventory[*item].number += inventory[i].number;
		  --inven_ctr;
		  for (j = i; j < inven_ctr; ++j)
		    memcpy(&inventory[j], &inventory[j+1], sizeof(inven_type));

		  inven1_invcopy(&inventory[j], OBJ_NOTHING);
	   }
    }
  }

  vbxe_restore_bank();
}


/* Remove item from equipment list		-RAK-	*/
void __fastcall__ inven1_takeoff(int8u item_val, int8 posn)
{
  register char *p;
  //bigvtype out_val, prt2;
  register inven_type *t_ptr;


  --equip_ctr;

  vbxe_bank(VBXE_OBJBANK);
  t_ptr = &inventory[item_val];
  inven_weight -= t_ptr->weight*t_ptr->number;
  py.flags.status |= PY_STR_WGT;

  if (item_val == INVEN_WIELD || item_val == INVEN_AUX)
    p = "Was wielding ";
  else if (item_val == INVEN_LIGHT)
    p = "Light source was ";
  else
    p = "Was wearing ";

  bank_objdes(out_val2, t_ptr, TRUE, INVEN1NUM);

  if (posn >= 0)
    sprintf(out_val1, "%s%s (%c)", p, out_val2, 'a'+posn);
  else
    sprintf(out_val1, "%s%s", p, out_val2);
  bank_msg_print(out_val1, INVEN1NUM);

  if (item_val != INVEN_AUX)	  /* For secondary weapon  */
    bank_py_bonuses(t_ptr, -1, INVEN1NUM);

  inven1_invcopy(t_ptr, OBJ_NOTHING);
}


/* Drops an item from inventory to given location	-RAK-	*/
void __fastcall__ inven_drop(int8u item_val, int8u drop_all)
{
  register int8u i;
  register inven_type *i_ptr;
  register cave_type *c_ptr;
  //vtype prt2;
  //bigvtype prt1;



  vbxe_bank(VBXE_OBJBANK);						// inventory is in OBJBANK
  i = inven1_popt();
  i_ptr = &inventory[item_val];
  memcpy(&t_list[i], i_ptr, sizeof(inven_type));

  vbxe_bank(VBXE_CAVBANK);						// need CAVBANK for this
  c_ptr = CAVE_ADR(char_row, char_col);
  //if (cave[char_row][char_col].tptr != 0)
  if (c_ptr->tptr != 0)
    bank_delete_object(char_row, char_col, INVEN1NUM);

  vbxe_bank(VBXE_CAVBANK);						// make sure cavebank is back in!
  //cave[char_row][char_col].tptr = i;
  c_ptr->tptr = i;

  vbxe_bank(VBXE_OBJBANK);

  if (item_val >= INVEN_WIELD)
    inven1_takeoff(item_val, -1);
  else {
    if (drop_all || i_ptr->number == 1) {
	  inven_weight -= i_ptr->weight*i_ptr->number;
	  --inven_ctr;
	  while (item_val < inven_ctr) {
	      memcpy(&inventory[item_val], &inventory[item_val+1], sizeof(inven_type));
	      ++item_val;
	  }
	  inven1_invcopy(&inventory[inven_ctr], OBJ_NOTHING);
	}
    else {
	  t_list[i].number = 1;
	  inven_weight -= i_ptr->weight;
	  i_ptr->number--;
	}
    bank_objdes(out_val1, &t_list[i], TRUE, INVEN1NUM);
    sprintf(out_val2, "Dropped %s", out_val1);
    bank_msg_print(out_val2, INVEN1NUM);
  }

  py.flags.status |= PY_STR_WGT;
}

