/* source/dungeon.c: the main command interpreter, updating player status

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>



/* Put code, rodata and data in Dungeon1 bank */
#pragma code-name("DUNGEON7BANK")
#pragma rodata-name("DUNGEON7BANK")
#pragma data-name("DUNGEON7BANK")


/* Prototypes of functions copied into this bank */
//void __fastcall__ dungeon7_erase_line(int8u row, int8u col);
int8u __fastcall__ dungeon7_mmove(int8u dir, int8u *y, int8u *x);
int8u __fastcall__ dungeon7_get_com(char *prompt, char *command);
char __fastcall__ dungeon7_map_roguedir(char comval);
int8u __fastcall__ dungeon7_get_dir(char *prompt, int8u *dir);
int8u __fastcall__ dungeon7_panel_contains(int8u y, int8u x);
void __fastcall__ dungeon7_lite_spot(int8u y, int8u x);
int16u __fastcall__ dungeon7_pdamroll(int8u *array);
int16u __fastcall__ dungeon7_damroll(int8u num, int8u sides);
void __fastcall__ inven_throw(int8u item_val, inven_type *t_ptr);
void __fastcall__ facts(inven_type *i_ptr, int16u *tbth, int8 *tpth, int8u *tdam, int8u *tdis);
void __fastcall__ drop_throw(int8u y, int8u x, inven_type *t_ptr);
int8u __fastcall__ dungeon7_popt(void);
int8u __fastcall__ dungeon7_in_bounds(int8u y, int8u x);
int8u __fastcall__ dungeon7_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type);
void __fastcall__ dungeon7_disturb(int8u s, int8u l);
void __fastcall__ dungeon7_rest_off(void);
void __fastcall__ dungeon7_search_off(void);
void __fastcall__ dungeon7_change_speed(int8 num);
int8u __fastcall__ dungeon7_get_panel(int8u y, int8u x, int8u force);
int8u __fastcall__ dungeon7_no_light(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		DUN7NUM

#undef	INBANK_MOD
#define INBANK_MOD			dungeon7


int8u __fastcall__ dungeon7_mmove(int8u dir, int8u *y, int8u *x)
#include "inbank\mmove.c"

int8u __fastcall__ dungeon7_get_com(char *prompt, char *command)
#include "inbank\get_com.c"

char __fastcall__ dungeon7_map_roguedir(char comval)
#include "inbank\map_roguedir.c"

int8u __fastcall__ dungeon7_get_dir(char *prompt, int8u *dir)
#include "inbank\get_dir.c"

int8u __fastcall__ dungeon7_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ dungeon7_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

void __fastcall__ dungeon7_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int16u __fastcall__ dungeon7_pdamroll(int8u *array)
#include "inbank\pdamroll.c"

int16u __fastcall__ dungeon7_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int8u __fastcall__ dungeon7_popt(void)
#include "inbank\popt.c"

int8u __fastcall__ dungeon7_test_hit(int16u bth, int8u level, int8 pth, int8u ac, int8u attack_type)
#include "inbank\test_hit.c"

void __fastcall__ dungeon7_disturb(int8u s, int8u l)
#include "inbank\disturb.c"

void __fastcall__ dungeon7_search_off(void)
#include "inbank\search_off.c"

void __fastcall__ dungeon7_rest_off(void)
#include "inbank\rest_off.c"

void __fastcall__ dungeon7_change_speed(int8 num)
#include "inbank\change_speed.c"

int8u __fastcall__ dungeon7_no_light(void)
#include "inbank\no_light.c"


/* Throw an object across the dungeon.		-RAK-	*/
/* Note: Flasks of oil do fire damage				 */
/* Note: Extra damage and chance of hitting when missiles are used*/
/*	 with correct weapon.  I.E.  wield bow and throw arrow.	 */
void __fastcall__ throw_object(void)
{
  int8u item_val, tdam, tdis;
  int16u tbth;
  int8 tpth;
  int8u y, x, oldy, oldx, cur_dis, dir;
  int8u flag, visible, cptr;
  //bigvtype out_val, tmp_str;
  inven_type throw_obj;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  int8 i;
  char tchar;


  if (inven_ctr == 0) {
    bank_msg_print("But you are not carrying anything.", DUN7NUM);
    free_turn_flag = TRUE;
  }
  else if (bank_get_item(&item_val, "Fire/Throw which one?", 0, inven_ctr-2, "", "", DUN7NUM)) {
    if (dungeon7_get_dir(CNIL, &dir)) {
	  bank_desc_remain(item_val, DUN7NUM);
	  if (py.flags.confused > 0) {
	    bank_msg_print("You are confused.", DUN7NUM);
	    do {
		  dir = randint(9);
		} while (dir == 5);
	  }
	  inven_throw(item_val, &throw_obj);
	  facts(&throw_obj, &tbth, &tpth, &tdam, &tdis);
	  tchar = throw_obj.tchar;
	  flag = FALSE;
	  y = char_row;
	  x = char_col;
	  oldy = char_row;
	  oldx = char_col;
	  cur_dis = 0;
	  do {
	    dungeon7_mmove(dir, &y, &x);
	    ++cur_dis;
	    dungeon7_lite_spot(oldy, oldx);
	    if (cur_dis > tdis) flag = TRUE;
        vbxe_bank(VBXE_CAVBANK);
		c_ptr = CAVE_ADR(y, x);
		cptr = c_ptr->cptr;

	    if ((c_ptr->fval <= MAX_OPEN_SPACE) && (!flag)) {
		  if (c_ptr->cptr > 1) {
		    flag = TRUE;
		    vbxe_bank(VBXE_MONBANK);
		    m_ptr = &m_list[cptr];
		    tbth = tbth - cur_dis;
		    /* if monster not lit, make it much more difficult to
			   hit, subtract off most bonuses, and reduce bthb
			   depending on distance */
		    if (!m_ptr->ml) {
		      vbxe_bank(VBXE_SPLBANK);
			  tbth = (tbth / (cur_dis+2)) - (py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTHB] / 2) - (tpth * (BTH_PLUS_ADJ-1));
			  vbxe_bank(VBXE_MONBANK);
		    }

		    if (dungeon7_test_hit(tbth, py.misc.lev, tpth, c_list[m_ptr->mptr].ac, CLA_BTHB)) {
			  bank_objdes(out_val1, &throw_obj, FALSE, DUN7NUM);
			  vbxe_bank(VBXE_MONBANK);
			  /* Does the player know what he's fighting?	   */
			  if (!m_ptr->ml) {
			    sprintf(out_val2, "You hear a cry as the %s finds a mark.", out_val1);
			    visible = FALSE;
			  }
			  else {
			    sprintf(out_val2, "The %s hits the %s.", out_val1, c_list[m_ptr->mptr].name);
			    visible = TRUE;
			  }
			  bank_msg_print(out_val2, DUN7NUM);
			  tdam = bank_tot_dam(&throw_obj, tdam, m_ptr->mptr, DUN7NUM);
			  tdam = bank_critical_blow(throw_obj.weight, tpth, tdam, CLA_BTHB, DUN7NUM);
			  //if (tdam < 0) tdam = 0;														// CHECK: cant be less than 0 -SJ
			  i = bank_mon_take_hit(cptr, tdam, DUN7NUM);

			  vbxe_bank(VBXE_MONBANK);
			  if (i >= 0) {
			    if (!visible)
		          bank_msg_print("You have killed something!", DUN7NUM);
		        else {
		    	  sprintf(out_val2,"You have killed the %s.", c_list[m_ptr->mptr].name);
				  bank_msg_print(out_val2, DUN7NUM);
				}
			    bank_prt_experience(DUN7NUM);
			  }
			}
		    else
			  drop_throw(oldy, oldx, &throw_obj);
		  }
		  else {	/* do not test c_ptr->fm here */
		    if (dungeon7_panel_contains(y, x) && (py.flags.blind < 1) && ((c_ptr->flags & CAVE_TL) || (c_ptr->flags & CAVE_PL))) {
			  bank_print(tchar, y, x, DUN7NUM);
			}
		  }
		}
	    else {
		  flag = TRUE;
		  drop_throw(oldy, oldx, &throw_obj);
		}
	    oldy = y;
	    oldx = x;

	  //cave[char_row][char_col].cptr = 1;					// FIXME testing
	  bank_print_self(char_row, char_col, DUN7NUM);			// FIXME testing
	  //cgetc();

	  } while (!flag);
	}
  }

  //bank_print_symbol(char_row, char_col, DUN7NUM);				// FIXME: testing
}


void __fastcall__ inven_throw(int8u item_val, inven_type *t_ptr)
{
  register inven_type *i_ptr;


  vbxe_bank(VBXE_OBJBANK);
  i_ptr = &inventory[item_val];
  memcpy(t_ptr, i_ptr, sizeof(inven_type));

  if (i_ptr->number > 1) {
    t_ptr->number = 1;
    i_ptr->number--;
    inven_weight -= i_ptr->weight;
      py.flags.status |= PY_STR_WGT;
  }
  else
    bank_inven_destroy(item_val, DUN7NUM);
}


/* Obtain the hit and damage bonuses and the maximum distance for a thrown missile. */
void __fastcall__ facts(inven_type *i_ptr, int16u *tbth, int8 *tpth, int8u *tdam, int8u *tdis)
{
  register int16u tmp_weight;


  if (i_ptr->weight < 1)
    tmp_weight = 1;
  else
    tmp_weight = i_ptr->weight;

  /* Throwing objects			*/
  *tdam = dungeon7_pdamroll(i_ptr->damage) + i_ptr->todam;
  *tbth = py.misc.bthb * 75 / 100;
  *tpth = py.misc.ptohit + i_ptr->tohit;

  vbxe_bank(VBXE_OBJBANK);

  /* Add this back later if the correct throwing device. -CJS- */
  if (inventory[INVEN_WIELD].tval != TV_NOTHING)
    *tpth -= inventory[INVEN_WIELD].tohit;

  *tdis = (((py.stats.use_stat[A_STR]+20)*10)/tmp_weight);
  if (*tdis > 10)  *tdis = 10;

  /* multiply damage bonuses instead of adding, when have proper
     missile/weapon combo, this makes them much more useful */

  /* Using Bows,  slings,  or crossbows	*/
  if (inventory[INVEN_WIELD].tval == TV_BOW)
    switch(inventory[INVEN_WIELD].p1) {
      case 1:
	    if (i_ptr->tval == TV_SLING_AMMO) {		/* Sling and ammo */
	      *tbth = py.misc.bthb;
	      *tpth += 2 * inventory[INVEN_WIELD].tohit;
	      *tdam += inventory[INVEN_WIELD].todam;
	      *tdam = *tdam * 2;
	      *tdis = 20;
	    }
	    break;
      case 2:
	    if (i_ptr->tval == TV_ARROW) {      	/* Short Bow and Arrow	*/
	      *tbth = py.misc.bthb;
	      *tpth += 2 * inventory[INVEN_WIELD].tohit;
	      *tdam += inventory[INVEN_WIELD].todam;
	      *tdam = *tdam * 2;
	      *tdis = 25;
	    }
	    break;
      case 3:
	    if (i_ptr->tval == TV_ARROW) {	      /* Long Bow and Arrow	*/
	      *tbth = py.misc.bthb;
	      *tpth += 2 * inventory[INVEN_WIELD].tohit;
	      *tdam += inventory[INVEN_WIELD].todam;
	      *tdam = *tdam * 3;
	      *tdis = 30;
	    }
	    break;
      case 4:
	    if (i_ptr->tval == TV_ARROW) {	      /* Composite Bow and Arrow*/
	      *tbth = py.misc.bthb;
	      *tpth += 2 * inventory[INVEN_WIELD].tohit;
	      *tdam += inventory[INVEN_WIELD].todam;
	      *tdam = *tdam * 4;
	      *tdis = 35;
	    }
	    break;
      case 5:
	    if (i_ptr->tval == TV_BOLT) { 	     /* Light Crossbow and Bolt*/
	      *tbth = py.misc.bthb;
	      *tpth += 2 * inventory[INVEN_WIELD].tohit;
	      *tdam += inventory[INVEN_WIELD].todam;
	      *tdam = *tdam * 3;
	      *tdis = 25;
	    }
	    break;
      case 6:
	    if (i_ptr->tval == TV_BOLT) {  	    /* Heavy Crossbow and Bolt*/
	      *tbth = py.misc.bthb;
	      *tpth += 2 * inventory[INVEN_WIELD].tohit;
	      *tdam += inventory[INVEN_WIELD].todam;
	      *tdam = *tdam * 4;
	      *tdis = 35;
	    }
	    break;
    }
}


void __fastcall__ drop_throw(int8u y, int8u x, inven_type *t_ptr)
{
  register int8u i, j, k;
  int8u flag, cur_pos;
  //bigvtype out_val, tmp_str;
  register cave_type *c_ptr;


  flag = FALSE;
  i = y;
  j = x;
  k = 0;
  if (randint(10) > 1) {
    do {
	  if (dungeon7_in_bounds(i, j)) {
		vbxe_bank(VBXE_CAVBANK);
		c_ptr = CAVE_ADR(i, j);
	    if (c_ptr->fval <= MAX_OPEN_SPACE && (c_ptr->tptr == 0))
		  flag = TRUE;
	  }
	  if (!flag) {
	    i = y + randint(3) - 2;
	    j = x + randint(3) - 2;
	    ++k;
	  }
	} while ((!flag) && (k <= 9));
  }
  if (flag) {
    cur_pos = dungeon7_popt();

    vbxe_bank(VBXE_CAVBANK);
    (CAVE_ADR(i, j))->tptr = cur_pos;

    vbxe_bank(VBXE_OBJBANK);
    memcpy(&t_list[cur_pos], t_ptr, sizeof(inven_type));
    dungeon7_lite_spot(i, j);
  }
  else {
    bank_objdes(out_val1, t_ptr, FALSE, DUN7NUM);
    sprintf(out_val2, "The %s disappears.", out_val1);
    bank_msg_print(out_val2, DUN7NUM);
  }
}


static struct opt_desc { char *o_prompt; int8u *o_var; } options[] = {
  { "Running: cut known corners",				&find_cut },
  { "Running: examine potential corners",		&find_examine },
  { "Running: print self during run",			&find_prself },
  { "Running: stop when map sector changes",	&find_bound },
  { "Running: run through open doors",			&find_ignore_doors },
  { "Prompt to pick up objects",				&prompt_carry_flag },
  { "Show weights in inventory",				&show_weight_flag },
  { "Highlight and notice mineral seams",		&highlight_seams },
  { "Display rest/repeat counts",				&display_counts },
  { 0, 0 } };


/* Set or unset various boolean options.		-CJS- */
void __fastcall__ set_options(void)
{
  register int8u i, max;
  char c;


  bank_prt("  ESC when finished, press letter to toggle option", 0, 0, DUN7NUM);

  for (max = 0; options[max].o_prompt != 0; ++max) {
    sprintf(out_val1, "%c) %-38s: %s", 'a'+max, options[max].o_prompt, (*options[max].o_var ? "yes" : "no "));
    bank_prt(out_val1, max+1, 0, DUN7NUM);
  }

  //dungeon7_erase_line(max+1, 0);
  //bank_vbxe_cleartoeol(0, max+1, DUN7NUM);
  bank_prt("\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12"
           "\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12", max+1, 0, DUN7NUM);

  for(;;) {
    c = cgetc();
    if (c == ESCAPE)
      return;

    if ((c >= 'a') && (c < 'a'+max)) {
	  i = c - 'a';
	  if (*options[i].o_var == FALSE) {
	    *options[i].o_var = TRUE;
	    bank_prt("yes", i+1, 43, DUN7NUM);
      }
      else {
	    *options[i].o_var = FALSE;
	    bank_prt("no ", i+1, 43, DUN7NUM);
      }
    }
  }
}


void __fastcall__ where_locate(void) {
  register int8u cy, cx, p_y, p_x;
  int16 y, x;
  int8u dir_val;


  if ((py.flags.blind > 0) || dungeon7_no_light())
	bank_msg_print("You can't see your map.", DUN7NUM);
  else {
    y = char_row;
	x = char_col;

	if (dungeon7_get_panel(y, x, TRUE))
	  bank_prt_map(DUN7NUM);

	cy = panel_row;
	cx = panel_col;

	for(;;) {
	  p_y = panel_row;
	  p_x = panel_col;
	  if (p_y == cy && p_x == cx)
	    out_val1[0] = '\0';
	  else
	    sprintf(out_val1, "%s%s of", p_y < cy ? " North" : p_y > cy ? " South" : "", p_x < cx ? " West" : p_x > cx ? " East" : "");
	  sprintf(out_val2, "Map sector [%d,%d], which is%s your sector. Look which direction?", p_y, p_x, out_val1);
	  if (!dungeon7_get_dir(out_val2, &dir_val))
	    break;

		//								      -CJS-
		// Should really use the move function, but what the hell. This
		// is nicer, as it moves exactly to the same place in another
		// section. The direction calculation is not intuitive. Sorry.
		//
      for(;;){
		x += (int16)((dir_val-1)%3 - 1) * SCREEN_WIDTH/2;
		y -= (int16)((dir_val-1)/3 - 1) * SCREEN_HEIGHT/2;
		if (x < 0 || y < 0 || x >= (int16) cur_width || y >= (int16) cur_width) {
		  bank_msg_print("You've gone past the end of your map.", DUN7NUM);
		  x -= (int16)((dir_val-1)%3 - 1) * SCREEN_WIDTH/2;
		  y += (int16)((dir_val-1)/3 - 1) * SCREEN_HEIGHT/2;
		  break;
		}
		if (dungeon7_get_panel((int8u) y, (int8u) x, TRUE)) {
		  bank_prt_map(DUN7NUM);
		  break;
		}
	  }
	}
	// Move to a new panel - but only if really necessary. //
	if (dungeon7_get_panel(char_row, char_col, FALSE))
	  bank_prt_map(DUN7NUM);
  }
}


/* Given an row (y) and col (x), this routine detects  -RAK-	*/
/* when a move off the screen has occurred and figures new borders.
   Force forcses the panel bounds to be recalculated, useful for 'W'here. */
int8u __fastcall__ dungeon7_get_panel(int8u y, int8u x, int8u force)
{
  register int8 prow, pcol;
  register int8u panel;


  prow = panel_row;
  pcol = panel_col;
  if (force || ((int8)y < panel_row_min + 2) || ((int8)y > panel_row_max - 2)) {
    prow = (((int8)y - SCREEN_HEIGHT/4)/(SCREEN_HEIGHT/2));
    if (prow > max_panel_rows)
	  prow = max_panel_rows;
    else if (prow < 0)
	  prow = 0;
  }

  if (force || ((int16)x < panel_col_min + 3) || ((int16)x > panel_col_max - 3)) {
    pcol = (((int16)x - SCREEN_WIDTH/4)/(SCREEN_WIDTH/2));
    if (pcol > max_panel_cols)
	  pcol = max_panel_cols;
    else if (pcol < 0)
	  pcol = 0;
  }

  if ((prow != panel_row) || (pcol != panel_col)) {
    panel_row = prow;
    panel_col = pcol;
    //panel_bounds();										// panel_bounds code below -SJ

    panel_row_min = panel_row*(SCREEN_HEIGHT/2);
    panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
    panel_row_prt = (int8)panel_row_min - 1;
    panel_col_min = panel_col*(SCREEN_WIDTH/2);
    panel_col_max = panel_col_min + SCREEN_WIDTH - 1;
    panel_col_prt = (int16)panel_col_min - 13;

    panel = TRUE;
    // stop movement if any //
    if (find_bound)
	  bank_end_find(DUN7NUM);
  }
  else
    panel = FALSE;

  return(panel);
}

