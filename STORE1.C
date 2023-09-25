/* source/store1.c: store code, updating store inventory, pricing objects

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


#pragma code-name("STORE1BANK")
#pragma rodata-name("STORE1BANK")



/* Functions copied into this bank Prototypes */
void __fastcall__ store1_invcopy(inven_type *to, int16u from_index);
//int8u store1_popt(void);
//void __fastcall__ store1_pusht(int8u x);
int8u __fastcall__ store1_known1_p(inven_type *i_ptr);
int8u __fastcall__ store1_known2_p(inven_type *i_ptr);
int8 __fastcall__ store1_object_offset(inven_type *t_ptr);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		STORE1NUM

#undef  INBANK_MOD
#define INBANK_MOD			store1


void __fastcall__ store1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"


/* Copied into STORE1BANK so we don't have to bank switch.  -SJ */
int8u __fastcall__ store1_known2_p(inven_type *i_ptr)
{
  return (i_ptr->ident & ID_KNOWN2);
}


/* Copied into INITBANK so we don't have to bank switch. -SJ */
/* needs objectbank! */
int8 __fastcall__ store1_object_offset(inven_type *t_ptr)                                      // changed return type to int8 SJ
#include "inbank\object_offset.c"

int8u __fastcall__ store1_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"


/* Returns the value for any given object		-RAK-	*/
int32 __fastcall__ item_value(inven_type *i_ptr)
{
  register int32 value;


  vbxe_bank(VBXE_OBJBANK);						// need object bank for item lookup

  value = i_ptr->cost;
  /* don't purchase known cursed items */
  if (i_ptr->ident & ID_DAMD)
    value = 0;
  else if (((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_SWORD)) || ((i_ptr->tval >= TV_BOOTS) && (i_ptr->tval <= TV_SOFT_ARMOR))) {
	  /* Weapons and armor	*/
    if (!store1_known2_p(i_ptr))
	  value = object_list[i_ptr->index].cost;
    else if ((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_SWORD)) {
	  if (i_ptr->tohit < 0)
	    value = 0;
	  else if (i_ptr->todam < 0)
	    value = 0;
	  else if (i_ptr->toac < 0)
	    value = 0;
	  else
	    value = i_ptr->cost+(i_ptr->tohit+i_ptr->todam+i_ptr->toac)*100;
	}
    else {
	  if (i_ptr->toac < 0)
	    value = 0;
	  else
	    value = i_ptr->cost+i_ptr->toac*100;
	}
  }
  else if ((i_ptr->tval >= TV_SLING_AMMO) && (i_ptr->tval <= TV_SPIKE))
    {	/* Ammo			*/
      if (!store1_known2_p(i_ptr))
	value = object_list[i_ptr->index].cost;
      else
	{
	  if (i_ptr->tohit < 0)
	    value = 0;
	  else if (i_ptr->todam < 0)
	    value = 0;
	  else if (i_ptr->toac < 0)
	    value = 0;
	  else
	    /* use 5, because missiles generally appear in groups of 20,
	       so 20 * 5 == 100, which is comparable to weapon bonus above */
	    value = i_ptr->cost+(i_ptr->tohit+i_ptr->todam+i_ptr->toac)*5;
	}
    }
  else if ((i_ptr->tval == TV_SCROLL1) || (i_ptr->tval == TV_SCROLL2) ||
	   (i_ptr->tval == TV_POTION1) || (i_ptr->tval == TV_POTION2))
    {	/* Potions, Scrolls, and Food	*/
      if (!store1_known1_p(i_ptr))
	value = 20;
    }
  else if (i_ptr->tval == TV_FOOD)
    {
      if ((i_ptr->subval < (ITEM_SINGLE_STACK_MIN + MAX_MUSH))
	  && !store1_known1_p(i_ptr))
	value = 1;
    }
  else if ((i_ptr->tval == TV_AMULET) || (i_ptr->tval == TV_RING))
    {	/* Rings and amulets	*/
      if (!store1_known1_p(i_ptr))
	/* player does not know what type of ring/amulet this is */
	value = 45;
      else if (!store1_known2_p(i_ptr))
	/* player knows what type of ring, but does not know whether it is
	   cursed or not, if refuse to buy cursed objects here, then
	   player can use this to 'identify' cursed objects */
	value = object_list[i_ptr->index].cost;
    }
  else if ((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND))
    {	/* Wands and staffs*/
      if (!store1_known1_p(i_ptr))
	{
	  if (i_ptr->tval == TV_WAND)
	    value = 50;
	  else
	    value = 70;
	}
      else if (store1_known2_p(i_ptr))
	value = i_ptr->cost + (i_ptr->cost / 20) * i_ptr->p1;
    }
  /* picks and shovels */
  else if (i_ptr->tval == TV_DIGGING)
    {
      if (!store1_known2_p(i_ptr))
	value = object_list[i_ptr->index].cost;
      else
	{
	  if (i_ptr->p1 < 0)
	    value = 0;
	  else
	    {
	      /* some digging tools start with non-zero p1 values, so only
		 multiply the plusses by 100, make sure result is positive */
	      value = i_ptr->cost + (i_ptr->p1 - object_list[i_ptr->index].p1) * 100;
	      if (value < 0)
		value = 0;
	    }
	}
    }
  /* multiply value by number of items if it is a group stack item */
  if (i_ptr->subval > ITEM_GROUP_MIN) /* do not include torches here */
    value = value * i_ptr->number;

  vbxe_restore_bank();
  return(value);
}


/* Asking price for an item				-RAK-	*/
int32 __fastcall__ sell_price(int8u snum, int32 *max_sell, int32 *min_sell, inven_type *item)
{
  register int32 i;
  register store_type *s_ptr;
  int32 cost;										// need to copy some of item's attrs to main ram -SJ


  s_ptr = &store[snum];				// store is in SPLBANK
  i = item_value(item);				// item is in OBJBANK

  /* Copy item from Object bank to stack */
  vbxe_bank(VBXE_OBJBANK);								// object bank might already be banked in? maybe not...
  cost = item->cost;									// just need the item cost

  vbxe_bank(VBXE_SPLBANK);								// need spellbank for rest of function

  /* check item->cost in case it is cursed, check i in case it is damaged */
  /*if ((item->cost > 0) && (i > 0)) {
    i = i * rgold_adj[owners[s_ptr->owner].owner_race][py.misc.prace] / 100;
    if (i < 1)  i = 1;
    *max_sell = i * owners[s_ptr->owner].max_inflate / 100;
    *min_sell = i * owners[s_ptr->owner].min_inflate / 100;
    if (min_sell > max_sell) min_sell = max_sell;
    return(i);*/

  /* check item->cost in case it is cursed, check i in case it is damaged */
  /* new code to use item on stack -SJ */
  if ((cost > 0) && (i > 0)) {
    i = i * (int32) rgold_adj[owners[s_ptr->owner].owner_race][py.misc.prace] / 100;
    if (i < 1)  i = 1;
    *max_sell = i * (int32) owners[s_ptr->owner].max_inflate / 100;
    *min_sell = i * (int32) owners[s_ptr->owner].min_inflate / 100;
    if (min_sell > max_sell) min_sell = max_sell;

    return(i);
  }
  else
    /* don't let the item get into the store inventory */
    return(0);
}


/* Check to see if he will be carrying too many objects	-RAK-	*/
int8u store_check_num(inven_type *t_ptr, int8u store_num)
{
  register int8u store_check, i;
  register store_type *s_ptr;
  register inven_type *i_ptr;
  inven_type item;										// need to copy some of item's attrs to main ram -SJ


  vbxe_bank(VBXE_OBJBANK);								// copy item from OBJBANK to stack
  memcpy(&item, t_ptr, sizeof(inven_type));
  vbxe_bank(VBXE_SPLBANK);								// need spellbank for if statement

  store_check = FALSE;
  s_ptr = &store[store_num];							// store is in spellbank
  if (s_ptr->store_ctr < STORE_INVEN_MAX)
    store_check = TRUE;
  else {
    if (item.subval >= ITEM_SINGLE_STACK_MIN)			// new code, since SPLBANK is active only
      for (i = 0; i < s_ptr->store_ctr; ++i) {
	   i_ptr = &s_ptr->store_inven[i].sitem;

	   /* note: items with subval of gte ITEM_SINGLE_STACK_MAX only stack if their subvals match */

	   if (i_ptr->tval == item.tval && i_ptr->subval == item.subval && ((int)i_ptr->number + (int)item.number < 256)
	       && (item.subval < ITEM_GROUP_MIN || (i_ptr->p1 == item.p1)))
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


/* Insert INVEN_MAX at given location	*/
static void insert_store(int8u store_num, int8u pos, int32 icost, inven_type *i_ptr)
{
  register int8 i;
  register store_type *s_ptr;
  //inven_type item;								// to copy item to stack


  //vbxe_bank(VBXE_OBJBANK);						// bank in OBJBANK for treasure t_list
  //memcpy(&item, i_ptr, sizeof(inven_type));		// copy to local stack
  //vbxe_bank(VBXE_SPLBANK);						// bank in SPLBANK for store structure

  s_ptr = &store[store_num];
  vbxe_bank(VBXE_SPLBANK);						// bank in SPLBANK for store structure

  if (s_ptr->store_ctr != 0) {
    for (i = s_ptr->store_ctr-1; i >= (int8) pos; --i) {
      //s_ptr->store_inven[i+1] = s_ptr->store_inven[i];									// cc65 doesn't copy structs like this
      memcpy(&s_ptr->store_inven[i+1], &s_ptr->store_inven[i], sizeof(inven_record));		// need to do a memcpy
    }
  }

  memcpy(&s_ptr->store_inven[pos].sitem, i_ptr, sizeof(inven_type));

  s_ptr->store_inven[pos].scost = -icost;
  s_ptr->store_ctr++;

  return;
}


/* Add the item in INVEN_MAX to stores inventory.	-RAK-	*/
void store_carry(int8u store_num, int8 *ipos, inven_type *t_ptr)				// ipos was int
{
  int8u item_num, item_val, flag;
  register int8u typ, subt;
  int32 icost, dummy;
  register inven_type *i_ptr;
  register store_type *s_ptr;
  inven_type item, sitem;									// keep items from objbank/splbank on stack -SJ


  *ipos = -1;
  //*ipos = 0;												// was -1, doesn't seem to be really used -SJ
  if (sell_price(store_num, &icost, &dummy, t_ptr) > 0) {	// destroys VBXE bank!

    vbxe_bank(VBXE_OBJBANK);
	memcpy(&item, t_ptr, sizeof(inven_type));				// copy item to stack
	vbxe_bank(VBXE_SPLBANK);								// need spellbank for next section

    s_ptr = &store[store_num];								// pointer to store (in SPLBANK)
    item_val = 0;
    flag = FALSE;
    item_num = item.number;									// copy all this stuff to stack item
    typ  = item.tval;
    subt = item.subval;

    do {
	  vbxe_bank(VBXE_SPLBANK);								// need spellbank for next section

	  i_ptr = &s_ptr->store_inven[item_val].sitem;
	  if (typ == i_ptr->tval) {
	    if (subt == i_ptr->subval && /* Adds to other item	*/
		  //subt >= ITEM_SINGLE_STACK_MIN && (subt < ITEM_GROUP_MIN || i_ptr->p1 == t_ptr->p1)) {
		  subt >= ITEM_SINGLE_STACK_MIN && (subt < ITEM_GROUP_MIN || i_ptr->p1 == item.p1)) {
		  *ipos = item_val;
		  i_ptr->number += item_num;

		  /* must set new scost for group items, do this only for items
		     strictly greater than group_min, not for torches, this
		     must be recalculated for entire group */
		  if (subt > ITEM_GROUP_MIN) {
		     memcpy(&sitem, i_ptr, sizeof(inven_type));						// i_ptr is in SPLBANK! (in store structure), copy to stack
		     //(void) sell_price (store_num, &icost, &dummy, i_ptr);		// destroys vbxe bank -SJ
		     sell_price(store_num, &icost, &dummy, &sitem);			        // destroys vbxe bank -SJ

		      vbxe_bank(VBXE_SPLBANK);										// bank SPLBANK back in
		      s_ptr->store_inven[item_val].scost = -icost;					// adjust cost
		  }
		  /* must let group objects (except torches) stack over 24
		     since there may be more than 24 in the group */
		  else if (i_ptr->number > 24)
		    i_ptr->number = 24;
		  flag = TRUE;
		}
	  }
	  else if (typ > i_ptr->tval) {		/* Insert into list		*/
	    insert_store(store_num, item_val, icost, &item);					// destroys vbxe bank -SJ

	    flag = TRUE;
	    *ipos = item_val;
	  }

	  vbxe_bank(VBXE_SPLBANK);
	  ++item_val;
	} while ((item_val < s_ptr->store_ctr) && (!flag));

    if (!flag) {	/* Becomes last item in list	*/
	  insert_store(store_num, s_ptr->store_ctr, icost, &item);				// destroys vbxe bank -SJ
      vbxe_bank(VBXE_SPLBANK);
	  *ipos = s_ptr->store_ctr - 1;
	}
  }

  return;
}


/* Destroy an item in the stores inventory.  Note that if	*/
/* "one_of" is false, an entire slot is destroyed	-RAK-	*/
void store_destroy(int8u store_num, int8u item_val, int8u one_of)
{
  register int8u j, number;
  register store_type *s_ptr;
  register inven_type *i_ptr;
  inven_type item;


  // SPLBANK already banked in! -SJ
  s_ptr = &store[store_num];
  i_ptr = &s_ptr->store_inven[item_val].sitem;

  /* for single stackable objects, only destroy one half on average,
     this will help ensure that general store and alchemist have
     reasonable selection of objects */
  if ((i_ptr->subval >= ITEM_SINGLE_STACK_MIN) && (i_ptr->subval <= ITEM_SINGLE_STACK_MAX))
  {
    if (one_of)
	  number = 1;
    else
	  number = randint(i_ptr->number);
  }
  else
    number = i_ptr->number;

  if (number != i_ptr->number)
    i_ptr->number -= number;
  else
  {
    for (j = item_val; j < s_ptr->store_ctr-1; ++j)										// shuffle items down
	  //s_ptr->store_inven[j] = s_ptr->store_inven[j+1];								// cc65 doesn't copy structs like this
	  memcpy(&s_ptr->store_inven[j], &s_ptr->store_inven[j+1], sizeof(inven_record));	// need to memcpy

    //store1_invcopy(&s_ptr->store_inven[s_ptr->store_ctr-1].sitem, OBJ_NOTHING);		// store is in SPLBANK, object_list is in OBJBANK!

    //memcpy(&item, &s_ptr->store_inven[s_ptr->store_ctr-1].sitem, sizeof(inven_type));  // FIXME: don't need to do this copy?
    store1_invcopy(&item, OBJ_NOTHING);													// copy OBJ_NOTHING object from object_list
    memcpy(&s_ptr->store_inven[s_ptr->store_ctr-1].sitem, &item, sizeof(inven_type));

    s_ptr->store_inven[s_ptr->store_ctr-1].scost = 0;
    s_ptr->store_ctr--;
  }

  return;
}


/* Creates an item and inserts it into store's inven	-RAK-	*/
static void store_create(int8u store_num)
{
  register int8u tries;
  int16u i;
  int8u cur_pos;
  int8 dummy;
  register store_type *s_ptr;
  register inven_type *t_ptr;
  int32 cost;


  tries = 0;
  //cur_pos = store1_popt();	// don't really need this function, replaced with code below -SJ  FIXME: what if tlist is full?
  tcptr += 1;					// temporarily use the last t_list entry
  cur_pos = tcptr;				// cur_pos

  vbxe_bank(VBXE_SPLBANK);
  s_ptr = &store[store_num];											// store is in spellbank

  do {
    i = store_choice[store_num][randint(STORE_CHOICES)-1];				// this is in spellbank -SJ

    store1_invcopy(&t_list[cur_pos], i);								// invcopy banks in objbank
    bank_magic_treasure(cur_pos, OBJ_TOWN_LEVEL, STORE1NUM);			// bank in magic treasure function

    vbxe_bank(VBXE_OBJBANK);											// magic treasure might destroy bank?
    t_ptr = &t_list[cur_pos];

    if (store_check_num(t_ptr, store_num)) {						    // destroys VBXE bank!
	  vbxe_bank(VBXE_OBJBANK);											// bank objbank back in
	  cost = t_ptr->cost;												// save cost, need to bank out objbank
	  vbxe_bank(VBXE_SPLBANK);											// need spellbank for owners and s_ptr

	  //if ((t_ptr->cost > 0) &&	/* Item must be good	*/
	  //    (t_ptr->cost < owners[s_ptr->owner].max_cost))				// owners is in spellbank -SJ
	  if ((cost > 0) &&	/* Item must be good	*/
	      (cost < owners[s_ptr->owner].max_cost))						// owners is in spellbank -SJ
	  {
	    /* equivalent to calling ident_spell(), except will not change the object_ident array */
 	    vbxe_bank(VBXE_OBJBANK);					   				    // bank objbank back in
        t_ptr->ident |= ID_STOREBOUGHT;
        t_ptr->ident |= ID_KNOWN2;

	    store_carry(store_num, &dummy, t_ptr);						    // t_ptr is in the OBJBANK (in the temporary tlist node)
	    tries = 10;
	  }
	}

    ++tries;
  } while (tries <= 3);

  //store1_pusht(cur_pos);						  // don't really need this function, replaced with code below -SJ
  store1_invcopy(&t_list[tcptr], OBJ_NOTHING);	  // set to nothing, invcopy sets proper bank here
  cur_pos -= 1;									  // remove last t_list entry

  return;
}


/* Initialize and up-keep the store's inventory.		-RAK-	*/
void store_maint(void)
{
  register int8u i;
  register int8 j;
  register store_type *s_ptr;


  bank_msg_print("Running store maintenance...", STORE1NUM);

  for (i = 0; i < MAX_STORES; ++i) {
    vbxe_bank(VBXE_SPLBANK);				// inside loop, store_destroy and store_create may kill vbxe bank -SJ
    s_ptr = &store[i];						// s_ptr set to current store


    s_ptr->insult_cur = 0;
    if (s_ptr->store_ctr >= STORE_MIN_INVEN)				// MIN is 10
	{
	  j = randint(STORE_TURN_AROUND);						// TURNAROUND (currently 9)

	  if (s_ptr->store_ctr >= STORE_MAX_INVEN)				// MAX is 18
	    j += 1 + s_ptr->store_ctr - STORE_MAX_INVEN;

      while (--j >= 0) {
	    vbxe_bank(VBXE_SPLBANK);							// needed for store_ctr
	    store_destroy(i, randint(s_ptr->store_ctr)-1, FALSE);
      }
	}

    vbxe_bank(VBXE_SPLBANK);				// inside loop, store_destroy and store_create may kill vbxe bank -SJ
    if (s_ptr->store_ctr <= STORE_MAX_INVEN)				// MAX is 18
	{
	  j = randint(STORE_TURN_AROUND);

	  if (s_ptr->store_ctr < STORE_MIN_INVEN)				// MIN is 10
	    j += STORE_MIN_INVEN - s_ptr->store_ctr;

	  while (--j >= 0)
	    store_create(i);
	}
  }

  bank_msg_print("", STORE1NUM);
  return;
}


