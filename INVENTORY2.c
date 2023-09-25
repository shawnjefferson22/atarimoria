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


#pragma code-name("INVEN2BANK")
#pragma rodata-name("INVEN2BANK")


/* Prototypes of functions copied into this bank */
void __fastcall__ inven2_put_buffer(char *out_str, int8 row, int8 col);
void __fastcall__ inven2_erase_line(int8u row, int8u col);
int8u __fastcall__ inven2_get_check(char *prompt);
int8u __fastcall__ inven2_get_com(char *prompt, char *command);
int8 __fastcall__ inven2_object_offset(inven_type *t_ptr);
int8u __fastcall__ inven2_known1_p(inven_type *i_ptr);
void __fastcall__ inven2_known1(inven_type *i_ptr);
int16u __fastcall__ inven2_weight_limit(void);
int8u __fastcall__ inventory2_isupper(char c);
char __fastcall__ inventory2_tolower(char c);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		INVEN2NUM

#undef  INBANK_MOD
#define INBANK_MOD			inven2


void __fastcall__ inven2_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"


/* Implemented in cartridge bank to get out of main RAM -SJ */
int8u __fastcall__ inventory2_isupper(char c)
{
  if ((c > 64) && (c < 91))
    return(TRUE);
  else
    return(FALSE);
}


char __fastcall__ inventory2_tolower(char c)
{
  if ((c > 64) && (c < 91))
    return(c + 32);
}


void __fastcall__ inven2_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

int8u __fastcall__ inven2_get_check(char *prompt)
#include "inbank\get_check.c"

int8u __fastcall__ inven2_get_com(char *prompt, char *command)
#include "inbank\get_com.c"

int8 __fastcall__ inven2_object_offset(inven_type *t_ptr)                                      // changed return type to int8 SJ
#include "inbank\object_offset.c"

void __fastcall__ inven2_known1(inven_type *i_ptr)
#include "inbank\known1.c"

int8u __fastcall__ inven2_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"


/* Used to verify if this really is the item we wish to	 -CJS-
   wear or read. */
int8u __fastcall__ inven2_verify(char *prompt, int8u item)
{
  //bigvtype out_str, object;				// let's try using out_val1 and out_val2


  vbxe_bank(VBXE_OBJBANK);
  //bank_objdes(object, &inventory[item], TRUE, INVEN2NUM);
  //object[strlen(object)-1] = '?'; 			/* change the period to a question mark */
  //sprintf(out_str, "%s %s", prompt, object);

  bank_objdes(out_val1, &inventory[item], TRUE, INVEN2NUM);
  out_val1[strlen(out_val1)-1] = '?'; 			/* change the period to a question mark */
  sprintf(out_val2, "%s %s", prompt, out_val1);

  return inven2_get_check(out_val2);
}


/* this code must be identical to the inven_carry() code below */
int8u __fastcall__ inven2_inven_check_num (inven_type *t_ptr)
{
  register int8u i;
  int8u ret;


  vbxe_bank(VBXE_OBJBANK);

  ret = FALSE;
  if (inven_ctr < INVEN_WIELD)
    ret = TRUE;
  else if (t_ptr->subval >= ITEM_SINGLE_STACK_MIN) {
	ret = FALSE;
    for (i = 0; i < inven_ctr; ++i)
      if (inventory[i].tval == t_ptr->tval && inventory[i].subval == t_ptr->subval &&      // inventory is in OBJBANK
	  /* make sure the number field doesn't overflow */
	  ((int)inventory[i].number + (int)t_ptr->number < 256) &&
	  /* they always stack (subval < 192), or else they have same p1 */
	  ((t_ptr->subval < ITEM_GROUP_MIN) || (inventory[i].p1 == t_ptr->p1))
	  /* only stack if both or neither are identified */
	  && (inven2_known1_p(&inventory[i]) == inven2_known1_p(t_ptr)))
	    ret = TRUE;
  }

  vbxe_restore_bank();
  return(ret);
}


/* Computes current weight limit			-RAK-	*/
int16u __fastcall__ inven2_weight_limit(void)
{
  register int16u weight_cap;


  weight_cap = py.stats.use_stat[A_STR] * PLAYER_WEIGHT_CAP + py.misc.wt;
  if (weight_cap > 3000)  weight_cap = 3000;
  return(weight_cap);
}


/* Displays inventory items from r1 to r2	-RAK-	*/
/* Designed to keep the display as far to the right as possible.  The  -CJS-
   parameter col gives a column at which to start, but if the display does
   not fit, it may be moved left.  The return value is the left edge used. */
/* If mask is non-zero, then only display those items which have a non-zero
   entry in the mask array.  */
/* Re-write of this function to just assume screen is cleared already, and
   left justify the display.  -SJ */
void __fastcall__ inven2_show_inven(int8u r1, int8u r2, int8u weight, char *mask)
{
  register int8u i;
  int8u total_weight, current_line;


  current_line = 1;
  for (i = r1; i <= r2; ++i) { 								// Print the items
    if (mask == CNIL || mask[i]) {
	  vbxe_bank(VBXE_OBJBANK);
	  bank_objdes(out_val1, &inventory[i], TRUE, INVEN2NUM);
	  sprintf(out_val2, " %c) %s  ", 'a'+i, out_val1);
	  bank_prt(out_val2, current_line, 0, INVEN2NUM);

	  vbxe_bank(VBXE_OBJBANK);								// if objdes destroys VBXE bank -SJ
	  if (weight) {
	    total_weight = inventory[i].weight*inventory[i].number;
	    sprintf(out_val1, "%3d.%d lb", (total_weight) / 10, (total_weight) % 10);
	    bank_prt(out_val1, current_line, 71, INVEN2NUM);
	  }
	  ++current_line;
	}
  }

  //inven2_erase_line(current_line, 0);						// erase one more line
  return;
}


/* All inventory commands (wear, exchange, take off, drop, inventory and
   equipment) are handled in an alternative command input mode, which accepts
   any of the inventory commands.

   It is intended that this function be called several times in succession,
   as some commands take up a turn, and the rest of moria must proceed in the
   interim. A global variable is provided, doing_inven, which is normally
   zero; however if on return from inven_command it is expected that
   inven_command should be called *again*, (being still in inventory command
   input mode), then doing_inven is set to the inventory command character
   which should be used in the next call to inven_command.

   On return, the screen is restored, but not flushed. Provided no flush of
   the screen takes place before the next call to inven_command, the inventory
   command screen is silently redisplayed, and no actual output takes place at
   all. If the screen is flushed before a subsequent call, then the player is
   prompted to see if we should continue. This allows the player to see any
   changes that take place on the screen during inventory command input.

  The global variable, screen_change, is cleared by inven_command, and set
  when the screen is flushed. This is the means by which inven_command tell
  if the screen has been flushed.

  The display of inventory items is kept to the right of the screen to
  minimize the work done to restore the screen afterwards.		-CJS-*/

/* Inventory command screen states. */
#define BLANK_SCR	0
#define EQUIP_SCR	1
#define INVEN_SCR	2
#define WEAR_SCR	3
#define HELP_SCR	4
#define WRONG_SCR	5

/* Keep track of the state of the inventory screen. */
int8u scr_state, scr_base;
int8u wear_low, wear_high;


/* Draw the inventory screen. */
void __fastcall__ inven_screen(int8u new_scr)
{
  register int8u line;


  if (new_scr != scr_state) {
      scr_state = new_scr;
      switch(new_scr) {
	    case BLANK_SCR:
	      line = 0;
	      break;
	    case HELP_SCR:
	      for(line=1; line<8; ++line)				// clear the screen for the help
	        inven2_erase_line(line, 0);

	      bank_prt("  ESC: exit", 1, 1, INVEN2NUM);
	      bank_prt("  w  : wear or wield object", 2, 1, INVEN2NUM);
	      bank_prt("  t  : take off item", 3, 1, INVEN2NUM);
	      bank_prt("  d  : drop object", 4, 1, INVEN2NUM);
	  	  bank_prt("  x  : exchange weapons", 5, 1, INVEN2NUM);
	  	  bank_prt("  i  : inventory of pack", 6, 1, INVEN2NUM);
	      bank_prt("  e  : list used equipment", 7, 1, INVEN2NUM);
	      line = 7;
	      break;
	    case INVEN_SCR:
	      inven2_show_inven(0, inven_ctr - 2, show_weight_flag, CNIL);		// changed to -2 -SJ inven_ctr is next available
	      line = inven_ctr;
	      break;
	   case WEAR_SCR:
	     inven2_show_inven(wear_low, wear_high, show_weight_flag, CNIL);
	     line = wear_high - wear_low + 1;
	     line++;					// FIXME: debugging
	     break;
	   case EQUIP_SCR:
	     bank_show_equip(show_weight_flag, INVEN2NUM);
	     line = equip_ctr;
	     line++;					// FIXME: debugging
	     break;
	 }

     if (line >= scr_base) {
	   scr_base = line + 1;
       //inven2_erase_line(scr_base-1, 0);								// changed from scr_left to 0 -SJ
	 }
     else {
	  /*while (++line <= scr_base)
	    inven2_erase_line(line, 0);*/
	   while (line < scr_base) {
		 inven2_erase_line(line, 0);
		 ++line;
       }
	 }

     bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	          "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	          "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	          "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", scr_base-1, 0, INVEN2NUM);
  }
}


/* This does all the work. */
void __fastcall__ inven_command(char command)
{
  register int8u slot;
  int8 tmp, item;
  int8u tmp2, selecting, from, to;
  char *prompt, *swap, *disp, *string;
  char which, query;
  //bigvtype prt1, prt2;					// use out_val1, out_val2 instead!
  register inven_type *i_ptr;
  inven_type tmp_obj;
  int16u tptr;								// tptr from cave


  /* Grab the tptr from the players cave location for use later */
  /* so we don't have to bank in CAVERAM later -SJ              */
  vbxe_bank(VBXE_CAVBANK);
  tptr = cave[char_row][char_col].tptr;
  vbxe_bank(VBXE_OBJBANK);

  free_turn_flag = TRUE;
  bank_vbxe_savescreen(INVEN2NUM);
  /* Take up where we left off after a previous inventory command. -CJS- */
  if (doing_inven) {
    /* If the screen has been flushed, we need to redraw. If the command is
	 a simple ' ' to recover the screen, just quit. Otherwise, check and
	 see what the user wants. */
    if (screen_change) {
	  if (command == ' ' || !inven2_get_check("Continuing with inventory command?")) {
	    doing_inven = FALSE;
	    return;
	  }
	  //scr_left = 50;				move over to left side of screen to save memory -SJ
	  scr_base = 0;
	}
	tmp = scr_state;
    scr_state = WRONG_SCR;
    inven_screen(tmp);
  }
  else {
    //scr_left = 50;
    scr_base = 0;
    /* this forces exit of inven_command() if selecting is not set true */
    scr_state = BLANK_SCR;
  }

  do {
    if (inventory2_isupper(command))
	  command = inventory2_tolower(command);

      /* Simple command getting and screen selection. */
      selecting = FALSE;
      switch(command) {
	    case 'i':	   /* Inventory	    */
	      if (inven_ctr == 0)
	        bank_msg_print("You are not carrying anything.", INVEN2NUM);
	      else
	        inven_screen(INVEN_SCR);
	      break;
	    case 'e':	  /* Equipment	   */
	      if (equip_ctr == 0)
	        bank_msg_print("You are not using any equipment.", INVEN2NUM);
	      else
	        inven_screen(EQUIP_SCR);
	      break;
	    case 't':	  /* Take off	   */
	      if (equip_ctr == 0)
	        bank_msg_print("You are not using any equipment.", INVEN2NUM);
	      /* don't print message restarting inven command after taking off something, it is confusing */
	      else if (inven_ctr >= INVEN_WIELD && !doing_inven)
	        bank_msg_print("You will have to drop something first.", INVEN2NUM);
	      else {
	        if (scr_state != BLANK_SCR)
	          inven_screen(EQUIP_SCR);
	        selecting = TRUE;
	      }
	      break;
	    case 'd':		/* Drop */
	      if (inven_ctr == 0 && equip_ctr == 0)
	        bank_msg_print("But you're not carrying anything.", INVEN2NUM);
	      //else if (cave[char_row][char_col].tptr != 0)
	      else if (tptr != 0)
	        bank_msg_print("There's no room to drop anything here.", INVEN2NUM);
	      else {
	        selecting = TRUE;
	        if ((scr_state == EQUIP_SCR && equip_ctr > 0) || inven_ctr == 0) {
		      if (scr_state != BLANK_SCR)
		        inven_screen(EQUIP_SCR);
		      command = 'r';	/* Remove - or take off and drop. */
		    }
	        else if (scr_state != BLANK_SCR)
		      inven_screen(INVEN_SCR);
	      }
	      break;
	    case 'w':	  /* Wear/wield	   */
	      vbxe_bank(VBXE_OBJBANK);
	      for (wear_low = 0; wear_low < inven_ctr && inventory[wear_low].tval > TV_MAX_WEAR; ++wear_low)
	        ;
	      for(wear_high = wear_low; wear_high < inven_ctr && inventory[wear_high].tval >=TV_MIN_WEAR; ++wear_high)
	        ;
	      wear_high--;
	      if (wear_low > wear_high)
	        bank_msg_print("You have nothing to wear or wield.", INVEN2NUM);
	      else {
	        if (scr_state != BLANK_SCR && scr_state != INVEN_SCR)
		      inven_screen(WEAR_SCR);
	        selecting = TRUE;
	      }
	      break;
	    case 'x':
	      if (inventory[INVEN_WIELD].tval == TV_NOTHING && inventory[INVEN_AUX].tval == TV_NOTHING)
	        bank_msg_print("But you are wielding no weapons.", INVEN2NUM);
	      else if (TR_CURSED & inventory[INVEN_WIELD].flags) {
	        bank_objdes(out_val1, &inventory[INVEN_WIELD], FALSE, INVEN2NUM);
	        sprintf(out_val2, "The %s you are wielding appears to be cursed.", out_val1);
	        bank_msg_print(out_val2, INVEN2NUM);
	      }
	      else {
	        free_turn_flag = FALSE;
	        memcpy(&tmp_obj, &inventory[INVEN_AUX], sizeof(inven_type));				//tmp_obj = inventory[INVEN_AUX];
	        memcpy(&inventory[INVEN_AUX], &inventory[INVEN_WIELD], sizeof(inven_type));	//inventory[INVEN_AUX] = inventory[INVEN_WIELD];
	        memcpy(&inventory[INVEN_WIELD], &tmp_obj, sizeof(inven_type));				// inventory[INVEN_WIELD] = tmp_obj;
	        if (scr_state == EQUIP_SCR)
		      bank_show_equip(show_weight_flag, INVEN2NUM);

		    // copy item to tmp_obj for banked calls below, assumes tmp_obj isn't used here -SJ
		    memcpy(&tmp_obj, &inventory[INVEN_AUX], sizeof(inven_type));
	        //bank_py_bonuses(&inventory[INVEN_AUX], -1, INVEN2NUM);	/* Subtract bonuses */
	        bank_py_bonuses(&tmp_obj, -1, INVEN2NUM);	   				/* Subtract bonuses */
		    memcpy(&tmp_obj, &inventory[INVEN_WIELD], sizeof(inven_type));
	        //bank_py_bonuses(&inventory[INVEN_WIELD], 1, INVEN2NUM);	/* Add bonuses    */
	        bank_py_bonuses(&tmp_obj, 1, INVEN2NUM);	   				/* Add bonuses    */

	        vbxe_bank(VBXE_OBJBANK);									// make sure OBJBANK is in -SJ
	        if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
		      strcpy(out_val1, "Primary weapon   : ");
		      bank_objdes(out_val2, &inventory[INVEN_WIELD], TRUE, INVEN2NUM);
		      bank_msg_print(strcat(out_val1, out_val2), INVEN2NUM);
		    }
	        else
		      bank_msg_print("No primary weapon.", INVEN2NUM);
	          /* this is a new weapon, so clear the heavy flag */
	          weapon_heavy = FALSE;
	          bank_check_strength(INVEN2NUM);
	      }
	      break;
	    case ' ':	/* Dummy command to return again to main prompt. */
	      break;
	    case '?':
	      inven_screen(HELP_SCR);
	      break;
	    default:
	      /* Nonsense command					   */
	      //bell();
	      break;
	  }

      /* Clear the doing_inven flag here, instead of at beginning, so that
	     can use it to control when messages above appear. */
      doing_inven = 0;

      /* Keep looking for objects to drop/wear/take off/throw off */
      which = 'z';
      while (selecting && free_turn_flag) {
	    swap = "";
	    if (command == 'w') {
	      from = wear_low;
	      to = wear_high;
	      prompt = "Wear/Wield";
	    }
	    else {
	      from = 0;
	      if (command == 'd') {
		    to = inven_ctr - 2;					// CHECK: inven_ctr is next, so -2?
		    prompt = "Drop";
		    if (equip_ctr > 0)
		      swap = ", / for Equip";
		  }
	      else {
		    to = equip_ctr - 1;
		    if (command == 't')
		      prompt = "Take off";
		    else	/* command == 'r' */
		    {
		      prompt = "Throw off";
		      if (inven_ctr > 0)
			    swap = ", / for Inven";
		    }
		  }
	    }
	    if (from > to)
	      selecting = FALSE;
	    else {
	      if (scr_state == BLANK_SCR)
		    disp = ", * to list";
	      else
		    disp = "";
	      sprintf(out_val1, "(%c-%c%s%s, space to break, ESC to exit) %s which one?", from+'a', to+'a', disp, swap, prompt);

	      /* Abort everything. */
	      if (!inven2_get_com(out_val1, &which)) {
		    selecting = FALSE;
		    which = ESCAPE;
		  }
	      /* Draw the screen and maybe exit to main prompt. */
	      else if (which == ' ' || which == '*') {
		    if (command == 't' || command == 'r')
		      inven_screen(EQUIP_SCR);
		    else if (command == 'w' && scr_state != INVEN_SCR)
		      inven_screen(WEAR_SCR);
		    else
		      inven_screen(INVEN_SCR);
		    if (which == ' ')
		      selecting = FALSE;
		  }
	      /* Swap screens (for drop) */
	      else if (which == '/' && swap[0]) {
		    if (command == 'd')
		      command = 'r';
		    else
		      command = 'd';
		    if (scr_state == EQUIP_SCR)
		      inven_screen(INVEN_SCR);
		    else if (scr_state == INVEN_SCR)
		      inven_screen(EQUIP_SCR);
		  }
	      else if ((which < from + 'a' || which > to + 'a') && (which < from + 'A' || which > to + 'A'))
		    //bell();
		    ;
	      else  /* Found an item! */
		  {
		    vbxe_bank(VBXE_OBJBANK);				// make sure the OBJBANK is in -SJ
		    if (inventory2_isupper(which))
		      item = which - 'A';
		    else
		      item = which - 'a';
		    if (command == 'r' || command == 't')  {
		      /* Get its place in the equipment list. */
		      tmp = item;
		      item = 21;
		      do {
			    ++item;
			    if (inventory[item].tval != TV_NOTHING)
			      --tmp;
			  } while (tmp >= 0);
		      if (inventory2_isupper(which) && !inven2_verify(prompt, item))
			    item = -1;
		      else if (TR_CURSED & inventory[item].flags) {
			    bank_msg_print("Hmmm, it seems to be cursed.", INVEN2NUM);
			    item = -1;
			  }
		      else if (command == 't' && !inven2_inven_check_num(&inventory[item])) {
			    //if (cave[char_row][char_col].tptr != 0) {
			    if (tptr != 0) {
			      bank_msg_print("You can't carry it.", INVEN2NUM);
			      item = -1;
			    }
			    else if (inven2_get_check("You can't carry it.  Drop it?"))
			      command = 'r';
			    else
			      item = -1;
			  }
		      if (item >= 0) {
			    if (command == 'r') {
			      bank_inven_drop(item, TRUE, INVEN2NUM);
			      /* As a safety measure, set the player's inven weight to 0, when the last object is dropped */
			      if (inven_ctr == 0 && equip_ctr == 0)
				    inven_weight = 0;
			    }
			    else {
			      vbxe_bank(VBXE_OBJBANK);									// make sure OBJBANK is in -SJ
			      slot = bank_inven_carry(&inventory[item], INVEN2NUM);		// should have OBJBANK in -SJ
			      bank_takeoff(item, slot, INVEN2NUM);
			    }
			    bank_check_strength(INVEN2NUM);
			    free_turn_flag = FALSE;
			    if (command == 'r')
			      selecting = FALSE;
			  }
		    }
		    else if (command == 'w') {
			  vbxe_bank(VBXE_OBJBANK);										// make sure OBJBANK is in -SJ
		      /* Wearing. Go to a bit of trouble over replacing existing equipment. */
		      if (inventory2_isupper(which) && !inven2_verify(prompt, item))
			    item = -1;
		      else
		        switch(inventory[item].tval) { /* Slot for equipment	   */
			      case TV_SLING_AMMO: case TV_BOLT: case TV_ARROW:
			      case TV_BOW: case TV_HAFTED: case TV_POLEARM:
			      case TV_SWORD: case TV_DIGGING: case TV_SPIKE:
			        slot = INVEN_WIELD; break;
			      case TV_LIGHT: slot = INVEN_LIGHT; break;
			      case TV_BOOTS: slot = INVEN_FEET; break;
			      case TV_GLOVES: slot = INVEN_HANDS; break;
			      case TV_CLOAK: slot = INVEN_OUTER; break;
			      case TV_HELM: slot = INVEN_HEAD; break;
			      case TV_SHIELD: slot = INVEN_ARM; break;
			      case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
			        slot = INVEN_BODY; break;
			      case TV_AMULET: slot = INVEN_NECK; break;
			      case TV_RING:
			        if (inventory[INVEN_RIGHT].tval == TV_NOTHING)
			          slot = INVEN_RIGHT;
			        else if (inventory[INVEN_LEFT].tval == TV_NOTHING)
			          slot = INVEN_LEFT;
			       else {
			        slot = 0;
			        /* Rings. Give some choice over where they go. */
			        do {
				      if (!inven2_get_com("Put ring on which hand (l/r/L/R)?", &query)) {
				        item = -1;
				        slot = -1;
				      }
				      else if (query == 'l' || query == 'L')
				        slot = INVEN_LEFT;
				      else if (query == 'r' || query == 'R')
				        slot = INVEN_RIGHT;
				      //else {
				      //  if (query == 'L')
				  	  //    slot = INVEN_LEFT;
				      //  else if (query == 'R')
					  //    slot = INVEN_RIGHT;
				      //else
					    //bell();
				      if (slot && !inven2_verify("Replace", slot))
					    slot = 0;
				      //}
				    } while(slot == 0);
			      } // switch
			    break;
			  default:
		        bank_msg_print("IMPOSSIBLE: I don't see how you can use that.", INVEN2NUM);
			    item = -1;
			    break;
			}
		    if (item >= 0 && inventory[slot].tval != TV_NOTHING) {
			  if (TR_CURSED & inventory[slot].flags) {
			    bank_objdes(out_val1, &inventory[slot], FALSE, INVEN2NUM);
			    sprintf(out_val2, "The %s you are ", out_val1);
			      if (slot == INVEN_HEAD)
				    strcat(out_val2, "wielding ");
			      else
				    strcat(out_val2, "wearing ");
			    bank_msg_print(strcat(out_val2, "appears to be cursed."), INVEN2NUM);
			    item = -1;
			  }
			  else if (inventory[item].subval == ITEM_GROUP_MIN && inventory[item].number > 1 &&
				   !inven2_inven_check_num(&inventory[slot])) {
			      /* this can happen if try to wield a torch, and have more than one in your inventory */
			    bank_msg_print("You will have to drop something first.", INVEN2NUM);
			    item = -1;
			  }
			}
		    if (item >= 0) {
			  /* OK. Wear it. */
			  free_turn_flag = FALSE;

			  /* first remove new item from inventory */
			  vbxe_bank(VBXE_OBJBANK);										// inventory in objectbank
			  memcpy(&tmp_obj, &inventory[item], sizeof(inven_type));       //tmp_obj = inventory[item];
			  i_ptr = &tmp_obj;

			  --wear_high;
			  /* Fix for torches	   */
			  if (i_ptr->number > 1 && i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
			    i_ptr->number = 1;
			    ++wear_high;
			  }
			  inven_weight += i_ptr->weight*i_ptr->number;
			  bank_inven_destroy(item, INVEN2NUM);	/* Subtracts weight */

			  /* second, add old item to inv and remove from
			     equipment list, if necessary */
			  vbxe_bank(VBXE_OBJBANK);										// make sure OBJBANK is in -SJ
			  i_ptr = &inventory[slot];
			  if (i_ptr->tval != TV_NOTHING) {
			    tmp2 = inven_ctr;
			    tmp = bank_inven_carry(i_ptr, INVEN2NUM);
			    /* if item removed did not stack with anything in
				   inventory, then increment wear_high */
			    if (inven_ctr != tmp2)
				  ++wear_high;
			    bank_takeoff(slot, tmp, INVEN2NUM);
			  }

			  /* third, wear new item */
			  memcpy(i_ptr, &tmp_obj, sizeof(inven_type));		// *i_ptr = tmp_obj;
			  ++equip_ctr;
			  bank_py_bonuses(i_ptr, 1, INVEN2NUM);				// tmp_obj is on stack, so call is ok -SJ
			  if (slot == INVEN_WIELD)
			    string = "You are wielding";
			  else if (slot == INVEN_LIGHT)
			    string = "Your light source is";
			  else
			    string = "You are wearing";

			  bank_objdes(out_val2, i_ptr, TRUE, INVEN2NUM);	// i_ptr should be on the stack -SJ
			  /* Get the right equipment letter. */
			  tmp = INVEN_WIELD;
			  item = 0;

			  vbxe_bank(VBXE_OBJBANK);							// make sure objbank is in after objdes call

			  while (tmp != slot)
			    if (inventory[tmp++].tval != TV_NOTHING)
			      ++item;

			  sprintf(out_val1, "%s %s (%c)", string, out_val2, 'a'+item);
			  bank_msg_print(out_val1, INVEN2NUM);

			  /* this is a new weapon, so clear the heavy flag */
			  if (slot == INVEN_WIELD)
			    weapon_heavy = FALSE;
			  bank_check_strength(INVEN2NUM);

			  if (i_ptr->flags & TR_CURSED) {
			    bank_msg_print("Oops! It feels deathly cold!", INVEN2NUM);
			    i_ptr->ident |= ID_DAMD;   //add_inscribe(i_ptr, ID_DAMD);  inlined this code -SJ
			    /* To force a cost of 0, even if unidentified. */
			    i_ptr->cost = -1;
			  }
			}
		  }
		  else /* command == 'd' */
		  {
		    vbxe_bank(VBXE_OBJBANK);							// make sure objbank is in -SJ
		    if (inventory[item].number > 1) {
			  bank_objdes(out_val1, &inventory[item], TRUE, INVEN2NUM);
			  out_val1[strlen(out_val1)-1] = '?';
			  sprintf(out_val2, "Drop all %s [y/n]", out_val1);
			  out_val1[strlen(out_val1)-1] = '.';
			  bank_prt(out_val2, 0, 0, INVEN2NUM);
			  //query = inkey();
			  query = cgetc();
			  if (query != 'y' && query != 'n') {
			    //if (query != ESCAPE)
				  //bell();
			    inven2_erase_line(MSG_LINE, 0);
			    item = -1;
			  }
			}
		    else if (inventory2_isupper(which) && !inven2_verify(prompt, item))
			  item = -1;
		    else
			  query = 'y';

		    if (item >= 0) {
			  free_turn_flag = FALSE;    /* Player turn   */
			  bank_inven_drop(item, query == 'y', INVEN2NUM);
			  bank_check_strength(INVEN2NUM);
			}
		    selecting = FALSE;
		    /* As a safety measure, set the player's inven weight to 0, when the last object is dropped.  */
		    if (inven_ctr == 0 && equip_ctr == 0)
			  inven_weight = 0;
		  }
		  if (free_turn_flag == FALSE && scr_state == BLANK_SCR)
		    selecting = FALSE;
		}
	  }
	}

    if (which == ESCAPE || scr_state == BLANK_SCR)
	  command = ESCAPE;
    else if (!free_turn_flag) {
	  /* Save state for recovery if they want to call us again next turn.*/
	  if (selecting)
	    doing_inven = command;
	  else
	    doing_inven = ' ';	/* A dummy command to recover screen. */
	  /* flush last message before clearing screen_change and exiting */

	  bank_msg_print("", INVEN2NUM);

	  screen_change = FALSE;/* This lets us know if the world changes */
	  command = ESCAPE;
	}
    else {
	  /* Put an appropriate header. */
	  if (scr_state == INVEN_SCR){
	    if (!show_weight_flag || inven_ctr == 0)
		  sprintf(out_val1, "You are carrying %d.%d pounds. In your pack there is %s", inven_weight / 10, inven_weight % 10,
			      (inven_ctr == 0 ? "nothing." : "-"));
	    else
		  sprintf (out_val1, "You are carrying %d.%d pounds. Your capacity is %d.%d pounds. %s", inven_weight / 10, inven_weight % 10,
				   inven2_weight_limit () / 10, inven2_weight_limit () % 10, "In your pack is -");
	      bank_prt(out_val1, 0, 0, INVEN2NUM);
	  }
	  else if (scr_state == WEAR_SCR) {
	    if (wear_high < wear_low)
	      bank_prt("You have nothing you could wield.", 0, 0, INVEN2NUM);
	    else
		  bank_prt("You could wield -", 0, 0, INVEN2NUM);
	  }
	  else if (scr_state == EQUIP_SCR) {
	    if (equip_ctr == 0)
		  bank_prt("You are not using anything.", 0, 0, INVEN2NUM);
	    else
		  bank_prt("You are using -", 0, 0, INVEN2NUM);
	  }
	  else
	    bank_prt("Allowed commands:", 0, 0, INVEN2NUM);

	  //inven2_erase_line(scr_base-1, 0);								// erase the whole line -SJ
	  //inven2_put_buffer("e/i/t/w/x/d/?/ESC:", scr_base-1, 0);		// changed to 0 from 60 to put on left side -SJ
	  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", scr_base-1, 0, INVEN2NUM);
	  inven2_put_buffer("e/i/t/w/x/d/?/ESC", scr_base-1, 62);

	  command = cgetc();
	  //inven2_erase_line(scr_base-1, 0);								// erase command prompt
	  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
	           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", scr_base-1, 0, INVEN2NUM);
	}
  } while (command != ESCAPE);

  if (scr_state != BLANK_SCR)
    bank_vbxe_restorescreen(INVEN2NUM);
  bank_calc_bonuses(INVEN2NUM);
}

