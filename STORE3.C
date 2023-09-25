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


#pragma code-name("STORE3BANK")
#pragma rodata-name("STORE3BANK")


/* Variables */
extern int16 last_store_inc;



/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ store3_erase_line(int8u row, int8u col);
void __fastcall__ store3_put_buffer(char *out_str, int8u row, int8u col);
void __fastcall__ store3_insert_lnum(char *object_str, char *mtc_str, int32 number, int8u show_sign);
int8u __fastcall__ store3_chr_adj(void);

void __fastcall__ store3_prt_comment2(int32u offer, int32u asking, int8u final);
void __fastcall__ store3_prt_comment4(void);
void __fastcall__ store3_prt_comment5(void);
void __fastcall__ store3_prt_comment6(void);
int32 __fastcall__ store3_sell_price(int8u snum, int32 *max_sell, int32 *min_sell, inven_type *item);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		STORE3NUM


void __fastcall__ store3_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

void __fastcall__ store3_put_buffer(char *out_str, int8u row, int8u col)
#include "inbank\put_buffer.c"

int8u __fastcall__ store3_chr_adj(void)
#include "inbank\chr_adj.c"


void __fastcall__ store3_insert_lnum(char *object_str, char *mtc_str, int32 number, int8u show_sign)
{
  register int8u mlen;
  vtype str1, str2;
  register char *string, *tmp_str;
  register int8u flag;


  flag = 1;
  mlen = strlen(mtc_str);
  tmp_str = object_str;
  do {
    //string = index(tmp_str, mtc_str[0]);
    string = strchr(tmp_str, mtc_str[0]);				// using strchr instead -SJ
    if (string == 0)
	  flag = 0;
    else {
	  flag = strncmp(string, mtc_str, mlen);
	  if (flag)
	    tmp_str = string+1;
	}
  } while (flag);

  if (string) {
      strncpy(str1, object_str, string - object_str);
      str1[string - object_str] = '\0';
      strcpy(str2, string + mlen);

      if ((number >= 0) && (show_sign))
	    sprintf(object_str, "%s+%ld%s", str1, number, str2);
      else
	    sprintf(object_str, "%s%ld%s", str1, number, str2);
  }

  return;
}


/* Comments vary.					-RAK-	*/
/* Comment one : Finished haggling				*/
void __fastcall__ store3_prt_comment1(void)
{
  vbxe_bank(VBXE_SPLBANK);										// comments are in SPLBANK
  bank_msg_print(comment1[randint(14)-1], STORE3NUM);
  vbxe_restore_bank();
}


/* %A1 is offer, %A2 is asking.		*/
void __fastcall__ store3_prt_comment2(int32u offer, int32u asking, int8u final)
{
  if (final > 0)
    strcpy(out_val1, comment2a[randint(3)-1]);
  else
    strcpy(out_val1, comment2b[randint(16)-1]);

  store3_insert_lnum(out_val1, "%A1", offer, FALSE);
  store3_insert_lnum(out_val1, "%A2", asking, FALSE);
  bank_msg_print(out_val1, STORE3NUM);
}


void __fastcall__ store3_prt_comment3(int32u offer, int32u asking, int8u final)
{
  if (final > 0)
    strcpy (out_val1, comment3a[randint(3)-1]);
  else
    strcpy (out_val1, comment3b[randint(15)-1]);

  store3_insert_lnum(out_val1, "%A1", offer, FALSE);
  store3_insert_lnum(out_val1, "%A2", asking, FALSE);
  bank_msg_print(out_val1, STORE3NUM);
}


/* Kick 'da bum out.					-RAK-	*/
void __fastcall__ store3_prt_comment4(void)
{
  register int8u tmp;

  vbxe_bank(VBXE_SPLBANK);
  tmp = randint(5) - 1;
  bank_msg_print(comment4a[tmp], STORE3NUM);
  bank_msg_print(comment4b[tmp], STORE3NUM);
  vbxe_restore_bank();
}


void __fastcall__ store3_prt_comment5(void)
{
  bank_msg_print(comment5[randint(10)-1], STORE3NUM);
}


void __fastcall__ store3_prt_comment6(void)
{
  bank_msg_print(comment6[randint(5)-1], STORE3NUM);
}


/* eliminate need to bargain if player has haggled well in the past   -DJB- */
int8u noneedtobargain(int8u store_num, int32u minprice)
{
  register int8u flagnoneed;
  register store_type *s_ptr;


  //vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK  should already be in splbank

  s_ptr = &store[store_num];
  flagnoneed = ((s_ptr->good_buy == MAX_SHORT) || (s_ptr->good_buy > 3 * s_ptr->bad_buy + 5 + minprice/50));
  return(flagnoneed);
}


/* update the bargin info					-DJB- */
void updatebargain(int8u store_num, int32u price, int32u minprice)
{
  register store_type *s_ptr;


  //vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK  - should already be in splbank

  s_ptr = &store[store_num];
  if (minprice > 9)
    if (price == minprice)
    {
	  if (s_ptr->good_buy < MAX_SHORT)
	    s_ptr->good_buy++;
    }
    else
    {
	  if (s_ptr->bad_buy < MAX_SHORT)
	    s_ptr->bad_buy++;
    }
}


/* Increase the insult counter and get angry if too many -RAK-	*/
int8u increase_insults(int8u store_num)
{
  register int8u increase;
  register store_type *s_ptr;


  //vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK  should already be in splbank

  increase = FALSE;
  s_ptr = &store[store_num];
  s_ptr->insult_cur++;
  if (s_ptr->insult_cur > owners[s_ptr->owner].insult_max)
  {
    store3_prt_comment4();
    s_ptr->insult_cur = 0;
    s_ptr->good_buy = 0;
    s_ptr->bad_buy = 0;
    s_ptr->store_open = turn + 2500 + randint(2500);
    increase = TRUE;
  }

  return(increase);
}


/* Decrease insults					-RAK-	*/
void decrease_insults(int8u store_num)
{
  register store_type *s_ptr;


  vbxe_bank(VBXE_SPLBANK);				// stores are in SPLBANK

  s_ptr = &store[store_num];
  if (s_ptr->insult_cur != 0)
    s_ptr->insult_cur--;
}


/* Have insulted while haggling				-RAK-	*/
int8u haggle_insults(int8u store_num)
{
  register int8u haggle;


  haggle = FALSE;
  if (increase_insults(store_num))
    haggle = TRUE;
  else {
    store3_prt_comment5();
    bank_msg_print("", STORE3NUM); /* keep insult separate from rest of haggle */
  }

  return(haggle);
}


/* Displays the set of commands				-RAK-	*/
void __fastcall__ haggle_commands(int8u typ)
{
  if (!typ)
    bank_prt("Specify an asking-price in gold pieces.", 21, 0, STORE3NUM);
  else
    bank_prt("Specify an offer in gold pieces.", 21, 0, STORE3NUM);
  bank_prt("ESC) Quit Haggling.", 22, 0, STORE3NUM);
  store3_erase_line(23, 0);	/* clear last line */
}


int8u get_haggle(char *comment, int32 *new_offer, int8u num_offer)
{
  int32 i;
  //vtype out_val, default_offer;			// using out_val1 and out_val2 (default_offer)
  register int8u flag, clen;
  register int8u orig_clen;
  register char *p;
  register int8u increment;


  flag = TRUE;
  increment = FALSE;
  clen = strlen(comment);
  orig_clen = clen;
  if (num_offer == 0)
    last_store_inc = 0;
  i = 0;
  do {
    bank_prt(comment, 0, 0, STORE3NUM);
    if (num_offer && last_store_inc != 0)
	{
	  //sprintf (default_offer, "[%c%d] ", (last_store_inc < 0) ? '-' : '+', abs(last_store_inc));
	  //prt(default_offer, 0, orig_clen);
	  //clen = orig_clen + strlen (default_offer);
	  sprintf (out_val2, "[%c%d] ", (last_store_inc < 0) ? '-' : '+', abs(last_store_inc));
	  bank_prt(out_val2, 0, orig_clen, STORE3NUM);
	  clen = orig_clen + strlen (out_val2);
	}
    if (!bank_get_string(out_val1, 0, clen, 40, STORE3NUM))
	  flag = FALSE;
    for (p = out_val1; *p == ' '; ++p)
	  ;
    if (*p == '+' || *p == '-')
	  increment = TRUE;
    if (num_offer && increment)
	{
	  i = atol(out_val1);
	  /* Don't accept a zero here.  Turn off increment if it was zero
	     because a zero will not exit.  This can be zero if the user
	     did not type a number after the +/- sign.  */
	  if (i == 0)
	    increment = FALSE;
	  else
	    last_store_inc = i;
	}
    else if (num_offer && *out_val1 == '\0')
	{
	  i = last_store_inc;
	  increment = TRUE;
	}
    else
	  i = atol(out_val1);

    /* don't allow incremental haggling, if player has not made an offer yet */
    if (flag && num_offer == 0 && increment)
	{
	  bank_msg_print("You haven't even made your first offer yet!", STORE3NUM);
	  i = 0;
	  increment = FALSE;
	}
  }
  while (flag && (i == 0));

  if (flag) {
    if (increment)
	  *new_offer += i;
    else
	  *new_offer = i;
  }
  else
    store3_erase_line (0, 0);
  return(flag);
}


int8u receive_offer(int8u store_num, char *comment, int32 *new_offer, int32 last_offer, int8u num_offer, int8 factor)
{
  register int8u flag;
  register int8u receive;


  receive = 0;
  flag = FALSE;
  do {
    if (get_haggle(comment, new_offer, num_offer))
	{
	  if (*new_offer*(int)factor >= last_offer*(int)factor)
	    flag = TRUE;
	  else if (haggle_insults(store_num))
	  {
	    receive = 2;
	    flag = TRUE;
	  }
	  else
	    /* new_offer rejected, reset new_offer so that incremental
	       haggling works correctly */
	    *new_offer = last_offer;
	}
      else
	{
	  receive = 1;
	  flag = TRUE;
	}
  } while (!flag);

  return(receive);
}


/* Haggling routine					-RAK-	*/
int8u __fastcall__ purchase_haggle(int8u store_num, int32 *price, inven_type *item)
{
  int32 max_sell, min_sell, max_buy;
  int32 cost, cur_ask, final_ask, min_offer;
  int32 last_offer, new_offer;
  int32 x1, x2, x3;
  int32 min_per, max_per;
  register int8u flag, loop_flag;
  char *comment;
  //vtype out_val;							// using out_val1 instead -SJ
  int8u purchase, num_offer, final_flag, didnt_haggle;
  register store_type *s_ptr;
  register owner_type *o_ptr;


  vbxe_bank(VBXE_SPLBANK);					// need SPLBANK for store structure

  flag = FALSE;
  purchase = 0;
  *price = 0;
  final_flag = 0;
  didnt_haggle = FALSE;

  s_ptr = &store[store_num];
  o_ptr = &owners[s_ptr->owner];

  cost = bank_sell_price(store_num, &max_sell, &min_sell, item, STORE3NUM);
  vbxe_bank(VBXE_SPLBANK);

  max_sell = max_sell * (int32) store3_chr_adj() / 100;
  if (max_sell <= 0)  max_sell = 1;
  min_sell = min_sell * (int32) store3_chr_adj() / 100;
  if (min_sell <= 0)  min_sell = 1;
  /* cast max_inflate to signed so that subtraction works correctly */
  max_buy  = cost * (200 - (int)o_ptr->max_inflate) / 100;
  if (max_buy <= 0) max_buy = 1;
  min_per  = o_ptr->haggle_per;
  max_per  = min_per * 3;

  haggle_commands(1);

  cur_ask   = max_sell;
  final_ask = min_sell;
  min_offer = max_buy;
  last_offer = min_offer;
  new_offer = 0;
  num_offer = 0; /* this prevents incremental haggling on first try */
  comment = "Asking";

  /* go right to final price if player has bargained well */
  if (noneedtobargain(store_num, final_ask))
  {
    bank_msg_print("After a long bargaining session, you agree upon the price.", STORE3NUM);
    cur_ask = min_sell;
    comment = "Final offer";
    didnt_haggle = TRUE;

    /* Set up automatic increment, so that a return will accept the final price.  */
    last_store_inc = min_sell;
    num_offer = 1;
  }

  do {
    do {
	  loop_flag = TRUE;
	  sprintf(out_val1, "%s :  %ld", comment, cur_ask);
	  store3_put_buffer(out_val1, 1, 0);

	  purchase = receive_offer(store_num, "What do you offer? ", &new_offer, last_offer, num_offer, 1);
	  if (purchase != 0)
	    flag = TRUE;
	  else {
	    if (new_offer > cur_ask) {
		  store3_prt_comment6();
		  /* rejected, reset new_offer for incremental haggling */
		  new_offer = last_offer;

		  /* If the automatic increment is large enough to overflow,
		     then the player must have made a mistake.  Clear it
		     because it is useless.  */
		  if (last_offer + last_store_inc > cur_ask)
		    last_store_inc = 0;
		}
	    else if (new_offer == cur_ask) {
		  flag = TRUE;
		  *price = new_offer;
		}
	    else
		  loop_flag = FALSE;
	  }
	} while (!flag && loop_flag);

    if (!flag) {
	  x1 = (new_offer - last_offer) * 100 / (cur_ask - last_offer);
	  if (x1 < min_per) {
	    flag = haggle_insults(store_num);
	    if (flag) purchase = 2;
	  }
	  else if (x1 > max_per) {
	    x1 = x1 * 75 / 100;
	    if (x1 < max_per) x1 = max_per;
	  }
	  x2 = x1 + randint(5) - 3;
	  x3 = ((cur_ask - new_offer) * x2 / 100) + 1;
	  /* don't let the price go up */
	  if (x3 < 0)
	    x3 = 0;
	  cur_ask -= x3;
	  if (cur_ask < final_ask) {
	    cur_ask = final_ask;
	    comment = "Final Offer";
	    /* Set the automatic haggle increment so that RET will give a new_offer equal to the final_ask price.  */
	    last_store_inc = final_ask - new_offer;
	    ++final_flag;
	    if (final_flag > 3) {
		  if (increase_insults(store_num))
		    purchase = 2;
		  else
		    purchase = 1;
		  flag = TRUE;
		}
	  }
	  else if (new_offer >= cur_ask) {
	    flag = TRUE;
	    *price = new_offer;
	  }
	  if (!flag) {
	    last_offer = new_offer;
	    ++num_offer; /* enable incremental haggling */
	    store3_erase_line (1, 0);
	    sprintf(out_val1, "Your last offer : %ld", last_offer);
	    store3_put_buffer(out_val1, 1, 39);
	    store3_prt_comment2(last_offer, cur_ask, final_flag);

	    /* If the current increment would take you over the store's price, then decrease it to an exact match.  */
	    if (cur_ask - last_offer < last_store_inc)
		  last_store_inc = cur_ask - last_offer;
	  }
	}
  } while (!flag);

  /* update bargaining info */
  if ((purchase == 0) && (!didnt_haggle))
    updatebargain(store_num, *price, final_ask);

  return(purchase);
}


/* Haggling routine					-RAK-	*/
int8u __fastcall__ sell_haggle(int8u store_num, int32 *price, inven_type *item)
{
  int32 max_sell, max_buy, min_buy;
  int32 cost, cur_ask, final_ask, min_offer;
  int32 last_offer, new_offer;
  int32 max_gold;
  int32 x1, x2, x3;
  int32 min_per, max_per;
  register int8u flag, loop_flag;
  char *comment;
  //vtype out_val;						// using out_val1 instead -SJ
  register store_type *s_ptr;
  register owner_type *o_ptr;
  int8u sell, num_offer, final_flag, didnt_haggle;


  vbxe_bank(VBXE_SPLBANK);				// for store structure

  flag = FALSE;
  sell = 0;
  *price = 0;
  final_flag = 0;
  didnt_haggle = FALSE;
  s_ptr = &store[store_num];

  cost = bank_item_value(item, STORE3NUM);
  if (cost < 1) {
    sell = 3;
    flag = TRUE;
  }
  else {
    o_ptr = &owners[s_ptr->owner];
    cost = cost * (200 - (int) store3_chr_adj()) / 100;
    cost = cost * (200 - (int) rgold_adj[o_ptr->owner_race][py.misc.prace]) / 100;
    if (cost < 1)  cost = 1;
    max_sell = cost * (int) o_ptr->max_inflate / 100;
    /* cast max_inflate to signed so that subtraction works correctly */
    max_buy  = cost * (200 - (int) o_ptr->max_inflate) / 100;
    min_buy  = cost * (200 - (int) o_ptr->min_inflate) / 100;
    if (min_buy < 1) min_buy = 1;
    if (max_buy < 1) max_buy = 1;
    if (min_buy < max_buy)  min_buy = max_buy;
    min_per  = o_ptr->haggle_per;
    max_per  = min_per * 3;
    max_gold = o_ptr->max_cost;
  }

  //sprintf(dbgstr, "cost: %ld, buy: %ld %ld mp: %ld max: %ld", cost, min_buy, max_buy, min_per, max_gold);
  //bank_prt(dbgstr, 19, 0, STORE3NUM);
  //cgetc();


  if (!flag) {
    haggle_commands(0);
    num_offer = 0; /* this prevents incremental haggling on first try */
    if (max_buy > max_gold) {
	  final_flag = 1;
	  comment = "Final Offer";
	  /* Disable the automatic haggle increment on RET.  */
	  last_store_inc = 0;
	  cur_ask   = max_gold;
	  final_ask = max_gold;
	  bank_msg_print("I am sorry, but I have not the money to afford such a fine item.", STORE3NUM);
	  didnt_haggle = TRUE;
    }
    else {
	  cur_ask   = max_buy;
	  final_ask = min_buy;
	  if (final_ask > max_gold)
	    final_ask = max_gold;
	  comment = "Offer";

	  /* go right to final price if player has bargained well */
      if (noneedtobargain(store_num, final_ask)) {
	    bank_msg_print("After a long bargaining session, you agree upon the price.", STORE3NUM);
        cur_ask = final_ask;
	    comment = "Final offer";
	    didnt_haggle = TRUE;

	    /* Set up automatic increment, so that a return will accept the final price.  */
	    last_store_inc = final_ask;
	    num_offer = 1;
      }
	}

    min_offer = max_sell;
    last_offer = min_offer;
    new_offer = 0;
    if (cur_ask < 1) cur_ask = 1;
    do {
	  do {
	    loop_flag = TRUE;
	    sprintf(out_val1, "%s :  %ld", comment, cur_ask);
	    store3_put_buffer(out_val1, 1, 0);
	    sell = receive_offer(store_num, "What price do you ask? ", &new_offer, last_offer, num_offer, -1);

	    //sprintf(dbgstr, "sell: %d, no:%ld lo:%ld no:%d", sell, new_offer, last_offer, num_offer);
		//bank_prt(dbgstr, 18, 0, STORE3NUM);
        //cgetc();

	    if (sell != 0)
		  flag   = TRUE;
	    else {
		  if (new_offer < cur_ask) {
		    store3_prt_comment6();
		    /* rejected, reset new_offer for incremental haggling */
		    new_offer = last_offer;

		    /* If the automatic increment is large enough to overflow, then the player must have made a mistake.
			 Clear it because it is useless.  */
		    if (last_offer + last_store_inc < cur_ask)
			  last_store_inc = 0;
		  }
		  else if (new_offer == cur_ask) {
		      flag = TRUE;
		      *price = new_offer;
		  }
		  else
		    loop_flag = FALSE;
		}
	  } while (!flag && loop_flag);

	  if (!flag) {
	    x1 = (last_offer - new_offer) * 100 / (last_offer - cur_ask);
	    if (x1 < min_per) {
		  flag = haggle_insults(store_num);
		  if (flag)  sell = 2;
		}
	    else if (x1 > max_per) {
		  x1 = x1 * 75 / 100;
		  if (x1 < max_per)  x1 = max_per;
		}

	    x2 = x1 + randint(5) - 3;
	    x3 = ((new_offer - cur_ask) * x2 / 100) + 1;
	    /* don't let the price go down */
	    if (x3 < 0)
		  x3 = 0;
	    cur_ask += x3;
	    if (cur_ask > final_ask) {
		  cur_ask = final_ask;
		  comment = "Final Offer";
		  /* Set the automatic haggle increment so that RET will give a new_offer equal to the final_ask price.  */
		  last_store_inc = final_ask - new_offer;
		  ++final_flag;
		  if (final_flag > 3) {
		    if (increase_insults(store_num))
	          sell = 2;
		    else
		      sell = 1;
		    flag = TRUE;
		  }
	   }
	   else if (new_offer <= cur_ask) {
		 flag = TRUE;
		 *price = new_offer;
       }
	   if (!flag) {
		 last_offer = new_offer;
		 ++num_offer; /* enable incremental haggling */
		 store3_erase_line(1, 0);
		 sprintf(out_val1, "Your last bid %ld", last_offer);
		 store3_put_buffer(out_val1, 1, 39);
		 store3_prt_comment3(cur_ask, last_offer, final_flag);

		 /* If the current decrement would take you under the store's price, then increase it to an exact match.  */
		 if (cur_ask - last_offer > last_store_inc)
		   last_store_inc = cur_ask - last_offer;
		}
	  }
	} while (!flag);
  }

  /* update bargaining info */
  if ((sell == 0) && (!didnt_haggle))
    updatebargain(store_num, *price, final_ask);

  return(sell);
}
