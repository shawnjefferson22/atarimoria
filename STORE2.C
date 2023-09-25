/* source/store2.c: store code, entering, command interpreter, buying, selling

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

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


#pragma code-name("STORE2BANK")
#pragma rodata-name("STORE2BANK")


/* Variables */
extern int16 last_store_inc;
//bigvtype out_val1, out_val2;				// moved here from functions below (placed in BSS in lowram)


/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ store2_put_buffer(char *out_str, int8 row, int8 col);
void __fastcall__ store2_put_buffer_color(char *out_str, int8u row, int8u col, int8u color);
void __fastcall__ store2_erase_line(int8u row, int8u col);
int8u __fastcall__ store2_chr_adj(void);
int8 __fastcall__ store2_object_offset(inven_type *t_ptr);
void __fastcall__ store2_take_one_item (inven_type *s_ptr, inven_type *i_ptr);
int8u __fastcall__ store2_inven_check_num (inven_type *t_ptr);
int8 __fastcall__ store2_object_offset(inven_type *t_ptr);
int8u __fastcall__ store2_inven_carry(inven_type *i_ptr);
int8u __fastcall__ store2_get_store_item(int8 *com_val, char *pmt, int8u i, int8u j);
int8u __fastcall__ store2_increase_insults(int8u store_num);
void __fastcall__ store2_decrease_insults(int8u store_num);
int8u __fastcall__ store_buy(int8u storenum, int8u element);
int8u __fastcall__ store2_store_check_num(inven_type *t_ptr, int8u store_num);
void __fastcall__ store2_unsample(inven_type *i_ptr);
void __fastcall__ store2_known2(inven_type *i_ptr);
int8u __fastcall__ store_purchase(int8u store_num, int8u *cur_top);
int8u __fastcall__ store_sell(int8u store_num, int8u *cur_top);
void __fastcall__ draw_cave(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		STORE2NUM

#undef  INBANK_MOD
#define INBANK_MOD			store2


void __fastcall__ store2_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

void __fastcall__ store2_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

void __fastcall__ store2_put_buffer_color(char *out_str, int8u row, int8u col, int8u color)
#include "inbank\put_buffer_color.c"

void __fastcall__ store2_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ store2_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"

int8 __fastcall__ store2_object_offset(inven_type *t_ptr)                                      // changed return type to int8 SJ
#include "inbank\object_offset.c"

int8u __fastcall__ store2_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

int8u __fastcall__ store2_chr_adj(void)
#include "inbank\chr_adj.c"


/* Displays the set of commands				-RAK-	*/
void __fastcall__ display_commands(void)
{
  store2_put_buffer_color("You may:", 20, 0, 0x0F);
  store2_put_buffer_color(" p) Purchase an item.           b) Browse store's inventory.", 21, 0, 0x0F);
  store2_put_buffer_color(" s) Sell an item.               i/e/t/w/x) Inventory/Equipment Lists.", 22, 0, 0x0F);
  store2_put_buffer_color("ESC) Exit from Building.", 23, 0, 0x0F);
}


/* Displays a store's inventory				-RAK-	*/
void __fastcall__ display_inventory(int8u store_num, int8u start)
{
  register store_type *s_ptr;
  register inven_type *i_ptr;
  register int8u i, j, stop;
  //bigvtype out_val1, out_val2;
  int32 x, value;


  vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK

  s_ptr = &store[store_num];
  i = (start % 12);
  stop = ((start / 12) + 1) * 12;
  if (stop > s_ptr->store_ctr)	stop = s_ptr->store_ctr;
  while (start < stop)
    {
      vbxe_bank(VBXE_SPLBANK);										// stores are in SPLBANK
      i_ptr = &s_ptr->store_inven[start].sitem;
      x = i_ptr->number;
      if ((i_ptr->subval >= ITEM_SINGLE_STACK_MIN) && (i_ptr->subval <= ITEM_SINGLE_STACK_MAX))
	    i_ptr->number = 1;

      bank_objdes(out_val1, i_ptr, TRUE, STORE2NUM);				// destroys VBXE bank
	  vbxe_bank(VBXE_SPLBANK);

      i_ptr->number = x;
      sprintf(out_val2, "%c) %s", 'a'+i, out_val1);
      bank_prt(out_val2, i+5, 0, STORE2NUM);

	  vbxe_bank(VBXE_SPLBANK);
      x = s_ptr->store_inven[start].scost;
      if (x <= 0)
	  {
	    value = -x;
	    value = value * (int32u) store2_chr_adj() / 100;
	    if (value <= 0)
	      value = 1;
	    sprintf(out_val2, "%9ld", value);
	  }
      else
	    sprintf(out_val2,"%9ld [Fixed]", x);
        bank_prt(out_val2, i+5, 59, STORE2NUM);
        ++i;
        ++start;
      }
      if (i < 12)
        for (j = 0; j < (11 - i + 1); ++j)
          store2_erase_line (j+i+5, 0); /* clear remaining lines */
      if (s_ptr->store_ctr > 12)
        store2_put_buffer("- cont. -", 17, 60);
      else
        store2_erase_line(17, 60);
}


/* Re-displays only a single cost			-RAK-	*/
void __fastcall__ display_cost(int8u store_num, int8u pos)
{
  register store_type *s_ptr;
  int8u i;
  register int32 j;
  //vtype out_val;						// using out_val1 instead in BSS -SJ


  vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK

  s_ptr = &store[store_num];
  i = (pos % 12);
  if (s_ptr->store_inven[pos].scost < 0)
  {
    j = - s_ptr->store_inven[pos].scost;
    j = j * (int32) store2_chr_adj() / 100;
    sprintf(out_val1, "%ld", j);		// was out_val -SJ
  }
  else
    sprintf(out_val1, "%9ld [Fixed]", s_ptr->store_inven[pos].scost);

  bank_prt(out_val1, i+5, 59, STORE2NUM);
}


/* Displays players gold					-RAK-	*/
void __fastcall__ store_prt_gold(void)
{
  //vtype out_val;						// using out_val1 instead -SJ

  sprintf(out_val1, "Gold Remaining : %ld", py.misc.au);			// py is in BSS -SJ
  bank_prt(out_val1, 18, 17, STORE2NUM);
}


/* Displays store					-RAK-	*/
void __fastcall__ display_store(int8u store_num, int8u cur_top)
{
  register store_type *s_ptr;


  bank_vbxe_clear(STORE2NUM);										// clear the screen

  vbxe_bank(VBXE_SPLBANK);											// stores are in SPLBANK
  s_ptr = &store[store_num];

  store2_put_buffer_color(owners[s_ptr->owner].owner_name, 3, 9, 0x0F);			// owners are in SPLBANK
  store2_put_buffer_color("Item", 4, 3, 0x0F);
  store2_put_buffer_color("Asking Price", 4, 60, 0x0F);
  store_prt_gold();
  display_commands();
  display_inventory(store_num, cur_top);
}


/* Entering a store					-RAK-	*/
void __fastcall__ enter_store(int8u store_num)
{
  register int8u tmp_chr;
  int8u cur_top;
  char command;
  register int8u exit_flag;
  register store_type *s_ptr;


  vbxe_bank(VBXE_SPLBANK);											// store is SPLBANK

  s_ptr = &store[store_num];
  if (s_ptr->store_open < turn) {
    exit_flag = FALSE;
    cur_top = 0;
    display_store(store_num, cur_top);
    do {
	  /* clear the msg flag just like we do in dungeon.c */
	  msg_flag = FALSE;
	  if (bank_get_com("", &command, STORE2NUM)) {					// CHECK: changed from CNIL -SJ
	    switch(command) {
		  case 'b':
		    if (cur_top == 0)
		      if (s_ptr->store_ctr > 12) {
			    cur_top = 12;
			    display_inventory(store_num, cur_top);
		      }
		      else
		        bank_msg_print("Entire inventory is shown.", STORE2NUM);
		    else {
		      cur_top = 0;
		      display_inventory(store_num, cur_top);
		    }
		    break;
		  case 'E': case 'e':	 /* Equipment List	*/
		  case 'I': case 'i':	 /* Inventory		*/
		  case 'T': case 't':	 /* Take off		*/
		  case 'W': case 'w':	 /* Wear			*/
		  case 'X': case 'x':	 /* Switch weapon		*/
		    tmp_chr = py.stats.use_stat[A_CHR];
		    do {
		      bank_inven_command(command, STORE2NUM);
		      command = doing_inven;
		    } while (command);
		    /* redisplay store prices if charisma changes */
		    if (tmp_chr != py.stats.use_stat[A_CHR])
		      display_inventory(store_num, cur_top);
		    free_turn_flag = FALSE;	/* No free moves here. -CJS- */
		    break;
		  case 'p':
		    exit_flag = store_purchase(store_num, &cur_top);
		    break;
		  case 's':
		    exit_flag = store_sell(store_num, &cur_top);
		    break;
		  default:
		    //bell();
		    break;
		}
	  }
	  else
	    exit_flag = TRUE;
	} while (!exit_flag);

    /* Can't save and restore the screen because inven_command does that. */
    draw_cave();
  }
  else
    bank_msg_print("The doors are locked.", STORE2NUM);
}


/* Copies the object in the second argument over the first argument.
   However, the second always gets a number of one except for ammo etc. */
void __fastcall__ store2_take_one_item (inven_type *s_ptr, inven_type *i_ptr)
{

  // SPLBANK should already be banked in, other argument is on the stack
  //*s_ptr = *i_ptr;
  memcpy(s_ptr, i_ptr, sizeof(inven_type));														// can't copy structs this way in cc65

  if ((s_ptr->number > 1) && (s_ptr->subval >= ITEM_SINGLE_STACK_MIN)
      && (s_ptr->subval <= ITEM_SINGLE_STACK_MAX))
    s_ptr->number = 1;
}


/* this code must be identical to the inven_carry() code below */
int8u __fastcall__ store2_inven_check_num (inven_type *t_ptr)
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
	  && (store2_known1_p(&inventory[i]) == store2_known1_p(t_ptr)))
	    ret = TRUE;
  }

  vbxe_restore_bank();
  return(ret);
}


/* Copied into init bank so we don't have to bank switch here.  -SJ */
/* Add an item to players inventory.  Return the        */
/* item position for a description if needed.          -RAK-   */
/* this code must be identical to the inven_check_num() code above */
int8u __fastcall__ store2_inven_carry(inven_type *i_ptr)
{
  register int8u locn, i;                                              // was int SJ
  register inven_type *t_ptr;
  register int8u typ, subt;
  int8u known1p, always_known1p;                                        // was int SJ


  typ = i_ptr->tval;
  subt = i_ptr->subval;
  known1p = store2_known1_p(i_ptr);
  always_known1p = (store2_object_offset(i_ptr) == -1);

  vbxe_bank(VBXE_OBJBANK);

  /* Now, check to see if player can carry object  */
  for (locn = 0; ; ++locn) {
    t_ptr = &inventory[locn];

    if ((typ == t_ptr->tval) && (subt == t_ptr->subval) && (subt >= ITEM_SINGLE_STACK_MIN) &&
	   ((int)t_ptr->number + (int)i_ptr->number < 256) && ((subt < ITEM_GROUP_MIN) || (t_ptr->p1 == i_ptr->p1)) &&
	   /* only stack if both or neither are identified */
	   (known1p == store2_known1_p(t_ptr))) {
	  t_ptr->number += i_ptr->number;
	  break;
	}
    /* For items which are always known1p, i.e. never have a 'color', insert them into the inventory in sorted order.  */
    else if ((typ == t_ptr->tval && subt < t_ptr->subval && always_known1p) || (typ > t_ptr->tval)) {
	  if (inven_ctr - 1 != 0) {
	    for (i = inven_ctr - 1; i >= locn; --i) {
		  memcpy(&inventory[i+1], &inventory[i], sizeof(inven_type));         // copy up one so we can insert
		  if (i == 0) break;                                                  // break out of the loop if last slot! -SJ
	    }
	  }

	  memcpy(&inventory[locn], i_ptr, sizeof(inven_type));        			// insert into inventory
	  ++inven_ctr;
	  break;
	}
  }

  inven_weight += i_ptr->number*i_ptr->weight;
  py.flags.status |= PY_STR_WGT;


  vbxe_restore_bank();
  return(locn);    // CHECK: return?
}


/* Get the ID of a store item and return it's value	-RAK-	*/
int8u __fastcall__ store2_get_store_item(int8 *com_val, char *pmt, int8u i, int8u j)
{
  char command;
  //vtype out_val;						// using out_val1 instead -SJ
  register int8u flag;


  *com_val = -1;
  flag = FALSE;
  sprintf(out_val1, "(Items %c-%c, ESC to exit) %s", i+'a', j+'a', pmt);

  while (bank_get_com(out_val1, &command, STORE2NUM))
  {
    command -= 'a';
    if (command >= i && command <= j)
	{
	  flag = TRUE;
	  *com_val = command;
	  break;
	}

	//bell();
  }

  store2_erase_line(MSG_LINE, 0);
  return(flag);
}


/* Increase the insult counter and get angry if too many -RAK-	*/
int8u __fastcall__ store2_increase_insults(int8u store_num)
{
  register int8u increase;
  register store_type *s_ptr;


  vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK

  increase = FALSE;
  s_ptr = &store[store_num];
  s_ptr->insult_cur++;
  if (s_ptr->insult_cur > owners[s_ptr->owner].insult_max)
  {
    bank_prt_comment4(STORE2NUM);
    s_ptr->insult_cur = 0;
    s_ptr->good_buy = 0;
    s_ptr->bad_buy = 0;
    s_ptr->store_open = turn + 2500 + randint(2500);
    increase = TRUE;
  }

  return(increase);
}


/* Decrease insults					-RAK-	*/
void __fastcall__ store2_decrease_insults(int8u store_num)
{
  register store_type *s_ptr;


  vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK

  s_ptr = &store[store_num];
  if (s_ptr->insult_cur != 0)
    s_ptr->insult_cur--;
}


/* Buy an item from a store				-RAK-	*/
int8u __fastcall__ store_purchase(int8u store_num, int8u *cur_top)
{
  int32 price;
  register int8u i, choice;
  //bigvtype out_val, tmp_str;			// using out_val1 and out_val2 (tmp_str) instead -SJ
  register store_type *s_ptr;
  inven_type sell_obj;
  register inven_record *r_ptr;
  int8 item_val;
  int8u item_new, purchase;


  vbxe_bank(VBXE_SPLBANK);				// store structure is in SPLBANK

  purchase = FALSE;
  s_ptr = &store[store_num];
  /* i == number of objects shown on screen	*/
  if (*cur_top == 12)
    i = s_ptr->store_ctr - 1 - 12;
  else if (s_ptr->store_ctr > 11)
    i = 11;
  else
    i = s_ptr->store_ctr - 1;
  if (s_ptr->store_ctr < 1)
    bank_msg_print("I am currently out of stock.", STORE2NUM);
  /* Get the item number to be bought		*/
  else if (store2_get_store_item(&item_val, "Which item are you interested in? ", 0, i))
  {
    item_val = item_val + *cur_top;	/* TRUE item_val	*/
    vbxe_bank(VBXE_SPLBANK);															// make sure spellbank is in

    store2_take_one_item(&sell_obj, &s_ptr->store_inven[item_val].sitem);			    // sell_obj is on stack, store item is in SPLBANK

    if (store2_inven_check_num(&sell_obj))
	{
	  if (s_ptr->store_inven[item_val].scost > 0)
	  {
	    vbxe_bank(VBXE_SPLBANK);
	    price = s_ptr->store_inven[item_val].scost;
	    choice = 0;
	  }
	  else {
	   choice = bank_purchase_haggle(store_num, &price, &sell_obj, STORE2NUM);
	  }

	  vbxe_bank(VBXE_SPLBANK);
	  if (choice == 0)
	  {
	    if (py.misc.au >= price)
	    {
		  bank_prt_comment1(STORE2NUM);
		  store2_decrease_insults(store_num);
		  py.misc.au -= price;
		  item_new = store2_inven_carry(&sell_obj);

		  vbxe_bank(VBXE_SPLBANK);

		  i = s_ptr->store_ctr;
		  bank_store_destroy(store_num, item_val, TRUE, STORE2NUM);				// bank in store_destroy

		  vbxe_bank(VBXE_OBJBANK);
		  bank_objdes(out_val2, &inventory[item_new], TRUE, STORE2NUM);			// bank in objdes

		  sprintf(out_val1, "You have %s (%c)", out_val2, item_new+'a');
		  bank_prt(out_val1, 0, 0, STORE2NUM);
		  bank_check_strength(STORE2NUM);										// bank in check_strength function

		  vbxe_bank(VBXE_SPLBANK);

		  if (*cur_top >= s_ptr->store_ctr)
		  {
		    *cur_top = 0;
		    display_inventory(store_num, *cur_top);
		  }
		  else
		  {
		    r_ptr = &s_ptr->store_inven[item_val];
		    if (i == s_ptr->store_ctr)
			{
			  if (r_ptr->scost < 0)
			  {
			    r_ptr->scost = price;
			    display_cost(store_num, item_val);
			  }
			}
		    else
			  display_inventory(store_num, item_val);
		  }
		  store_prt_gold();
		}
	    else
		{
		  if (store2_increase_insults(store_num))
		    purchase = TRUE;
		  else
		  {
		    bank_prt_comment1(STORE2NUM);
		    bank_msg_print("Liar!  You have not the gold!", STORE2NUM);
		  }
		}
	  }
	  else if (choice == 2)
	    purchase = TRUE;

	  /* Less intuitive, but looks better here than in purchase_haggle. */
	  display_commands();
	  store2_erase_line (1, 0);
	}
    else
	  bank_prt("You cannot carry that many different items.", 0, 0, STORE2NUM);
  }

  return(purchase);
}


/* Sell an item to the store				-RAK-	*/
int8u __fastcall__ store_sell(int8u store_num, int8u *cur_top)
{
  int8u counter, item_val, first_item;
  int32 price;
  //bigvtype out_val, tmp_str;					// using out_val1 and out_val2 (tmp_str) instead -SJ
  inven_type sold_obj;
  register int8u sell, choice, flag;
  int8u mask[INVEN_WIELD];						// array of items and if we can buy them
  int8 last_item, item_pos;
  inven_type item;								// temp item for copying between banks


  sell = FALSE;
  first_item = inven_ctr;
  last_item = -1;

  vbxe_bank(VBXE_OBJBANK);												// inventory is in OBJBANK
  for (counter = 0; counter < inven_ctr; ++counter) {
    flag = store_buy(store_num, inventory[counter].tval);
    mask[counter] = flag;
    if (flag) {
	  if (counter < first_item)
	    first_item = counter;
	  if ((int8) counter > last_item)
	    last_item = counter;
	} /* end of if (flag) */
  } /* end of for (counter) */

  if (last_item == -1)
    bank_msg_print("You have nothing to sell to this store!", STORE2NUM);
  else if (bank_get_item(&item_val, "Which one? ", first_item, last_item, (char *) mask, "I do not buy such items.", STORE2NUM))
  {
    vbxe_bank(VBXE_OBJBANK);
    memcpy(&item, &inventory[item_val], sizeof(inven_type));					// copy item out of OBJBANK

    store2_take_one_item(&sold_obj, &item);										// both items are on the stack
    bank_objdes(out_val2, &sold_obj, TRUE, STORE2NUM);
    sprintf(out_val1, "Selling %s (%c)", out_val2, item_val+'a');
    bank_msg_print(out_val1, STORE2NUM);

    vbxe_bank(VBXE_SPLBANK);
    if (store2_store_check_num(&sold_obj, store_num))
	{
	  choice = bank_sell_haggle(store_num, &price, &sold_obj, STORE2NUM);

	  if (choice == 0) {
	    bank_prt_comment1(STORE2NUM);
	    store2_decrease_insults(store_num);
	    py.misc.au += price;
	    /* identify object in inventory to set object_ident */
	    bank_identify(&item_val, STORE2NUM);
	    /* retake sold_obj so that it will be identified */
	    store2_take_one_item(&sold_obj, &item);
	    /* call known2 for store item, so charges/pluses are known */
	    store2_known2(&sold_obj);
	    bank_inven_destroy(item_val, STORE2NUM);
	    bank_objdes(out_val2, &sold_obj, TRUE, STORE2NUM);
	    sprintf(out_val1, "You've sold %s", out_val2);
	    bank_msg_print(out_val1, STORE2NUM);
	    bank_store_carry(store_num, &item_pos, &sold_obj, STORE2NUM);
	    bank_check_strength(STORE2NUM);
	    if (item_pos >= 0)
		{
		  if (item_pos < 12)
		    if (*cur_top < 12)
		      display_inventory(store_num, item_pos);
		    else {
			  *cur_top = 0;
			  display_inventory(store_num, *cur_top);
		    }
		  else if (*cur_top > 11)
		    display_inventory(store_num, item_pos);
		  else {
		    *cur_top = 12;
		    display_inventory(store_num, *cur_top);
		  }
		}
	    store_prt_gold();
	  }
	  else if (choice == 2)
	    sell = TRUE;
	  else if (choice == 3) {
	      bank_msg_print("How dare you!", STORE2NUM);
	      bank_msg_print("I will not buy that!", STORE2NUM);
	      sell = store2_increase_insults(store_num);
	  }

	  /* Less intuitive, but looks better here than in sell_haggle. */
	  store2_erase_line (1, 0);
	  display_commands();
	}
    else
	  bank_msg_print("I have not the room in my store to keep it.", STORE2NUM);
  }

  return(sell);
}


/* Check to see if he will be carrying too many objects	-RAK-	*/
int8u __fastcall__ store2_store_check_num(inven_type *t_ptr, int8u store_num)
{
  register int8u store_check, i;
  register store_type *s_ptr;
  register inven_type *i_ptr;


  //vbxe_bank(VBXE_OBJBANK);								// copy item from OBJBANK to stack
  //memcpy(&item, t_ptr, sizeof(inven_type));
  vbxe_bank(VBXE_SPLBANK);								// need spellbank for if statement

  store_check = FALSE;
  s_ptr = &store[store_num];							// store is in spellbank
  if (s_ptr->store_ctr < STORE_INVEN_MAX)
    store_check = TRUE;
  else {
    if (t_ptr->subval >= ITEM_SINGLE_STACK_MIN)			// new code, since SPLBANK is active only
      for (i = 0; i < s_ptr->store_ctr; ++i) {
	   i_ptr = &s_ptr->store_inven[i].sitem;

	   /* note: items with subval of gte ITEM_SINGLE_STACK_MAX only stack if their subvals match */

	   if (i_ptr->tval == t_ptr->tval && i_ptr->subval == t_ptr->subval && ((int)i_ptr->number + (int)t_ptr->number < 256)
	       && (t_ptr->subval < ITEM_GROUP_MIN || (i_ptr->p1 == t_ptr->p1)))
	     store_check = TRUE;
      }

  /*  if (t_ptr->subval >= ITEM_SINGLE_STACK_MIN)			// t_ptr is in objbank!
    for (i = 0; i < s_ptr->store_ctr; ++i) {
	   i_ptr = &s_ptr->store_inven[i].sitem;
	   // note: items with subval of gte ITEM_SINGLE_STACK_MAX only stack if their subvals match
	   if (i_ptr->tval == t_ptr->tval && i_ptr->subval == t_ptr->subval
	       && ((int)i_ptr->number + (int)t_ptr->number < 256)
	       && (t_ptr->subval < ITEM_GROUP_MIN
		   || (i_ptr->p1 == t_ptr->p1)))


	    store_check = TRUE;
      }*/
  }

  return(store_check);
}


/* re-implemented store_buy as one function -SJ */
int8u __fastcall__ store_buy(int8u storenum, int8u element)
{
  switch(storenum) {
    case 0:		// general store
      switch(element)
	      {
	      case TV_DIGGING: case TV_BOOTS: case TV_CLOAK: case TV_FOOD:
	      case TV_FLASK: case TV_LIGHT: case TV_SPIKE:
	        return(TRUE);
      }
      break;

    case 1:		// armory
      switch(element)
	      {
	      case TV_BOOTS: case TV_GLOVES: case TV_HELM: case TV_SHIELD:
	      case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
	        return(TRUE);
	   }
	   break;

	 case 2:	// weaponsmith
       switch(element)
       {
         case TV_SLING_AMMO: case TV_BOLT: case TV_ARROW: case TV_BOW:
         case TV_HAFTED: case TV_POLEARM: case TV_SWORD:
           return(TRUE);
       }
       break;

     case 3:	// temple
       switch(element)
	       {
	       case TV_HAFTED: case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1:
	       case TV_POTION2: case TV_PRAYER_BOOK:
	         return(TRUE);
       }
       break;

     case 4:	// alchemist
       switch(element)
	       {
	       case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1: case TV_POTION2:
	         return(TRUE);
       }
       break;

     case 5:	// magic shop
       switch(element)
	       {
	       case TV_AMULET: case TV_RING: case TV_STAFF: case TV_WAND:
	       case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1: case TV_POTION2:
	       case TV_MAGIC_BOOK:
	         return(TRUE);
       }
       break;
  }

  return(FALSE);
}


/* Draws entire screen					-RAK-	*/
void __fastcall__ draw_cave(void)
{
  //clear_screen();
  bank_vbxe_clear(STORE2NUM);
  bank_prt_stat_block(STORE2NUM);
  bank_prt_map(STORE2NUM);
  bank_prt_depth(STORE2NUM);
}
