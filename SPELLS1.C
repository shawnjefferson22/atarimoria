/* source/spells.c: player/creature spells, breaths, wands, scrolls, etc. code

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


#pragma code-name("SPELLS1BANK")
#pragma data-name("SPELLS1BANK")
#pragma rodata-name("SPELLS1BANK")


/* Local Prototypes */
int8u __fastcall__ spells1_panel_contains(int8u y, int8u x);
int8u __fastcall__ spells1_in_bounds(int8u y, int8u x);
void __fastcall__ spells1_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr);
void __fastcall__ spells1_lower_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr);
int8u __fastcall__ spells1_popt(void);
void __fastcall__ spells1_invcopy(inven_type *to, int16u from_index);
int8u __fastcall__ spells1_test_light(int8u y, int8u x);
void __fastcall__ spells1_lite_spot(int8u y, int8u x);
int8 __fastcall__ spells1_object_offset(inven_type *t_ptr);
void __fastcall__ spells1_unsample(inven_type *i_ptr);
void __fastcall__ spells1_known2(inven_type *i_ptr);
void __fastcall__ spells1_change_trap(int8u y, int8u x);
char* __fastcall__ spells1_describe_use(int8u i);
int8u __fastcall__ spells1_find_range(int8u item1, int8u item2, int8u *j, int8u *k);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		SPELLS1NUM

#undef  INBANK_MOD
#define INBANK_MOD			spells1


int8u __fastcall__ spells1_find_range(int8u item1, int8u item2, int8u *j, int8u *k)
#include "inbank\find_range.c"

int8u __fastcall__ spells1_popt(void)
#include "inbank\popt.c"

int8u __fastcall__ spells1_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ spells1_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

void __fastcall__ spells1_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
#include "inbank\monster_name.c"

void __fastcall__ spells1_lower_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
#include "inbank\lower_monster_name.c"

void __fastcall__ spells1_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ spells1_test_light(int8u y, int8u x)
#include "inbank\test_light.c"

void __fastcall__ spells1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8 __fastcall__ spells1_object_offset(inven_type *t_ptr)
#include "inbank\object_offset.c"

void __fastcall__ spells1_known2(inven_type *i_ptr)
#include "inbank\known2.c"

void __fastcall__ spells1_unsample(inven_type *i_ptr)
#include "inbank\unsample.c"

void __fastcall__ spells1_change_trap(int8u y, int8u x)
#include "inbank\change_trap.c"

char* __fastcall__ spells1_describe_use(int8u i)
#include "inbank\describe_use.c"


/* Sleep creatures adjacent to player			-RAK-	*/
int8u __fastcall__ sleep_monsters1(int8u y, int8u x)
{
  register int8u i, j;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int8u sleep;
  int8u c;
  int16u mon;


  sleep = FALSE;
  for (i = y-1; i <= y+1; ++i)
    for (j = x-1; j <= x+1; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  c = (CAVE_ADR(i,j))->cptr;

	  if (c > 1) {
		vbxe_bank(VBXE_MONBANK);
	    m_ptr = &m_list[c];
	    r_ptr = &c_list[m_ptr->mptr];
	    mon = m_ptr->mptr;

	    spells1_monster_name(out_val1, m_ptr, r_ptr);
	    if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
		  if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
			vbxe_bank(VBXE_GENBANK);
		    c_recall[mon].r_cdefense |= CD_NO_SLEEP;
	  	  }
		  sprintf(out_val2, "%s is unaffected.", out_val1);
		  bank_msg_print(out_val2, SPELLS1NUM);
	    }
	    else {
		  sleep = TRUE;
		  m_ptr->csleep = 500;
		  sprintf(out_val2, "%s falls asleep.", out_val1);
		  bank_msg_print(out_val2, SPELLS1NUM);
	    }
	  }
    }

  return(sleep);
}


/* Detect any treasure on the current panel		-RAK-	*/
int8u __fastcall__ detect_treasure(void)
{
  register int8u i, j;
  register cave_type *c_ptr;
  int8u detect;
  int8u t;							// temporary storage of tptr


  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; ++i)
    for (j = panel_col_min; j <= panel_col_max; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(i,j);  //&cave[i][j];
	  t = c_ptr->tptr;			// save tptr

	  vbxe_bank(VBXE_OBJBANK);
	  if ((t != 0) && (t_list[t].tval == TV_GOLD) && !spells1_test_light(i, j)) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr->flags |= CAVE_FM;	//fm = TRUE;
	    spells1_lite_spot(i, j);
	    detect = TRUE;
	  }
    }

  return(detect);
}


/* Detect all objects on the current panel		-RAK-	*/
int8u __fastcall__ detect_object(void)
{
  register int8u i, j;
  register cave_type *c_ptr;
  int8u detect;
  int8u t;


  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; ++i)
    for (j = panel_col_min; j <= panel_col_max; ++j) {
      vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(i,j);
	  t = c_ptr->tptr;

	  vbxe_bank(VBXE_OBJBANK);
	  if ((t != 0) && (t_list[t].tval < TV_MAX_OBJECT) && !spells1_test_light(i, j)) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr->flags |= CAVE_FM;			// set true
	    spells1_lite_spot(i, j);
	    detect = TRUE;
	  }
    }

  return(detect);
}


/* Locates and displays traps on current panel		-RAK-	*/
int8u __fastcall__ detect_trap(void)
{
  register int8u i, j;
  int8u detect, t;
  register cave_type *c_ptr;
  register inven_type *t_ptr;


  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; ++i)
    for (j = panel_col_min; j <= panel_col_max; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(i,j);
	  t = c_ptr->tptr;

	  if (t != 0) {
		vbxe_bank(VBXE_OBJBANK);
	    if (t_list[t].tval == TV_INVIS_TRAP) {
	      vbxe_bank(VBXE_CAVBANK);
	      c_ptr->flags |= CAVE_FM;		// set true
	      spells1_change_trap(i, j);
	      detect = TRUE;
	    }
	    else if (t_list[t].tval == TV_CHEST) {
	      t_ptr = &t_list[t];
	      spells1_known2(t_ptr);
	    }
      }
	}

  return(detect);
}


/* Locates and displays all secret doors on current panel -RAK-	*/
int8u __fastcall__ detect_sdoor(void)
{
  register int8u i, j;
  register cave_type *c_ptr;
  int8u detect, t, fl;


  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; ++i)
    for (j = panel_col_min; j <= panel_col_max; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  c_ptr = CAVE_ADR(i,j);
	  t = c_ptr->tptr;
	  fl = c_ptr->flags;

	  if (t != 0) {
	    /* Secret doors  */
	    vbxe_bank(VBXE_OBJBANK);
	    if (t_list[t].tval == TV_SECRET_DOOR) {
	      vbxe_bank(VBXE_CAVBANK);
	      c_ptr->flags |= CAVE_FM;			// set true
	      spells1_change_trap(i, j);
	      detect = TRUE;
	    }
	    /* Staircases	 */
	    else if (((t_list[t].tval == TV_UP_STAIR) || (t_list[t].tval == TV_DOWN_STAIR)) && !(fl & CAVE_FM)) {
	      c_ptr->flags |= CAVE_FM;          // set true
	      spells1_lite_spot(i, j);
	      detect = TRUE;
	    }
      }
	}
  return(detect);
}


/* Locates and displays all invisible creatures on current panel -RAK-*/
int8u __fastcall__ detect_invisible(void)
{
  register int8u i;
  int8u flag;
  register monster_type *m_ptr;


  flag = FALSE;
  vbxe_bank(VBXE_MONBANK);

  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    m_ptr = &m_list[i];
    if (spells1_panel_contains(m_ptr->fy, m_ptr->fx) && (CM_INVISIBLE & c_list[m_ptr->mptr].cmove)) {
	  m_ptr->ml = TRUE;
	  /* works correctly even if hallucinating */
	  bank_print(c_list[m_ptr->mptr].cchar, m_ptr->fy, m_ptr->fx, SPELLS1NUM);
	  flag = TRUE;
	}
  }

  if (flag) {
    bank_msg_print("You sense the presence of invisible creatures!", SPELLS1NUM);
    bank_msg_print("", SPELLS1NUM);
    /* must unlight every monster just lighted */
    bank_creatures(FALSE, SPELLS1NUM);
  }

  vbxe_restore_bank();
  return(flag);
}


/* Light an area: 1.  If corridor  light immediate area              -RAK-*/
/*		          2.  If room  light entire room plus immediate area.     */
int8u __fastcall__ light_area(int8u y, int8u x)
{
  register int8u i, j;
  int8u light;


  if (py.flags.blind < 1)
    bank_msg_print("You are surrounded by a white light.", SPELLS1NUM);

  light = TRUE;
  vbxe_bank(VBXE_CAVBANK);

  //if (cave[y][x].lr && (dun_level > 0))
  if (((CAVE_ADR(y,x))->flags & CAVE_LR) && (dun_level > 0))
    bank_light_room(y, x, SPELLS1NUM);

  /* Must always light immediate area, because one might be standing on
     the edge of a room, or next to a destroyed area, etc.  */
  for (i = y-1; i <= y+1; ++i)
    for (j = x-1; j <=  x+1; ++j) {
	  vbxe_bank(VBXE_CAVBANK);
	  //cave[i][j].pl = TRUE;
	  (CAVE_ADR(i,j))->flags |= CAVE_PL;		// set pl true
	  spells1_lite_spot(i, j);
    }

  return(light);
}


/* Darken an area, opposite of light area		-RAK-	*/
int8u __fastcall__ unlight_area(int8u y, int8u x)
{
  register int8u i, j;
  int8u tmp1, tmp2, unlight;
  int8u start_row, start_col, end_row, end_col;
  register cave_type *c_ptr;


  unlight = FALSE;
  vbxe_bank(VBXE_CAVBANK);

  if (((CAVE_ADR(y,x))->flags & CAVE_LR) && (dun_level > 0)) {
    tmp1 = (SCREEN_HEIGHT/2);
    tmp2 = (SCREEN_WIDTH /2);
    start_row = (y/tmp1)*tmp1 + 1;
    start_col = (x/tmp2)*tmp2 + 1;
    end_row = start_row + tmp1 - 1;
    end_col = start_col + tmp2 - 1;
    for (i = start_row; i <= end_row; ++i) {
	  for (j = start_col; j <= end_col; ++j) {
	    vbxe_bank(VBXE_CAVBANK);
	    c_ptr = CAVE_ADR(i,j);
	    if ((c_ptr->flags & CAVE_LR) && c_ptr->fval <= MAX_CAVE_FLOOR) {
		  c_ptr->flags &= ~CAVE_PL;		// set pl = FALSE;
		  c_ptr->fval = DARK_FLOOR;
		  spells1_lite_spot(i, j);
		  if (!spells1_test_light(i, j))
		    unlight = TRUE;
		}
	  }
	}
  }
  else
    for (i = y-1; i <= y+1; ++i)
      for (j = x-1; j <= x+1; ++j) {
	    c_ptr = CAVE_ADR(i,j);
	    if ((c_ptr->fval == CORR_FLOOR) && (c_ptr->flags & CAVE_PL)) {
	      /* pl could have been set by star-lite wand, etc */
	      c_ptr->flags &= ~CAVE_PL;			//set pl = FALSE;
	      unlight = TRUE;
	    }
	  }

  if (unlight && py.flags.blind <= 0)
    bank_msg_print("Darkness surrounds you.", SPELLS1NUM);

  return(unlight);
}


/* Map the current area plus some			-RAK-	*/
void __fastcall__ map_area(void)
{
  register cave_type *c_ptr;
  register int8u i7, i8, n, m;
  int8u i, j, k, l;
  int8u t, fval;


  i = panel_row_min - randint(10);
  j = panel_row_max + randint(10);
  k = panel_col_min - randint(20);
  l = panel_col_max + randint(20);

  vbxe_bank(VBXE_CAVBANK);

  for (m = i; m <= j; ++m)
    for (n = k; n <= l; ++n)
      if (spells1_in_bounds(m, n) && ((CAVE_ADR(m,n))->fval <= MAX_CAVE_FLOOR))
	    for (i7 = m-1; i7 <= m+1; ++i7)
	      for (i8 = n-1; i8 <= n+1; ++i8) {
	        vbxe_bank(VBXE_CAVBANK);
	        c_ptr = CAVE_ADR(i7, i8);     	//&cave[i7][i8];
	        t = c_ptr->tptr;				// save tptr
	        fval = c_ptr->fval;				// save fval

	        vbxe_bank(VBXE_OBJBANK);
	        if (fval >= MIN_CAVE_WALL) {
			  vbxe_bank(VBXE_CAVBANK);
		      c_ptr->flags |= CAVE_PL;		// set pl = TRUE;
			}
	        else if ((t != 0) && (t_list[t].tval >= TV_MIN_VISIBLE) && (t_list[t].tval <= TV_MAX_VISIBLE)) {
			  vbxe_bank(VBXE_CAVBANK);
		      c_ptr->flags |= CAVE_FM; 		// set fm = TRUE;
			}
	      }

  bank_prt_map(SPELLS1NUM);
}


/* Identify an object					-RAK-	*/
int8u __fastcall__ ident_spell(void)
{
  int8u item_val;
  //bigvtype out_val, tmp_str;
  int8u ident;
  register inven_type *i_ptr;


  ident = FALSE;
  vbxe_bank(VBXE_OBJBANK);

  //if (bank_get_item(&item_val, "Item you wish identified?", 0, INVEN_ARRAY_SIZE, CNIL, "", SPELLS1NUM)) {
  if (bank_get_item(&item_val, "Item you wish identified?", 0, inven_ctr-2, CNIL, "", SPELLS1NUM)) {
    ident = TRUE;
    bank_identify(&item_val, SPELLS1NUM);
    i_ptr = &inventory[item_val];
    spells1_known2(i_ptr);
    bank_objdes(out_val1, i_ptr, TRUE, SPELLS1NUM);
    if (item_val >= INVEN_WIELD) {
	  bank_calc_bonuses(SPELLS1NUM);
	  sprintf(out_val2, "%s: %s", spells1_describe_use(item_val), out_val1);
	}
    else
	  sprintf(out_val2, "%c %s", item_val+97, out_val1);

    bank_msg_print(out_val2, SPELLS1NUM);
  }

  return(ident);
}


/* Removes curses from items in inventory		-RAK-	*/
int8u __fastcall__ remove_curse(void)
{
  register int8u i;
  int8u result;
  register inven_type *i_ptr;


  result = FALSE;
  vbxe_bank(VBXE_OBJBANK);

  for (i = INVEN_WIELD; i <= INVEN_OUTER; ++i) {
    i_ptr = &inventory[i];
    if (TR_CURSED & i_ptr->flags) {
	  i_ptr->flags &= ~TR_CURSED;
	  bank_calc_bonuses(SPELLS1NUM);
	  result = TRUE;
	}
  }

  return(result);
}


/* Leave a glyph of warding. Creatures will not pass over! -RAK-*/
void __fastcall__ warding_glyph(void)
{
  int8u i;
  register cave_type *c_ptr;

  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(char_row, char_col);

  if (c_ptr->tptr == 0) {
    i = spells1_popt();
    c_ptr->tptr = i;
    spells1_invcopy(&t_list[i], OBJ_SCARE_MON);
  }
}


/* Evil creatures don't like this.		       -RAK-   */
int8u __fastcall__ protect_evil(void)
{
  int8u res;


  if (py.flags.protevil == 0)
    res = TRUE;
  else
    res = FALSE;

  py.flags.protevil += randint(25) + 3*py.misc.lev;
  return(res);
}


/* Create some high quality mush for the player.	-RAK-	*/
void __fastcall__ create_food(void)
{
  register cave_type *c_ptr;


  vbxe_bank(VBXE_CAVBANK);
  c_ptr = CAVE_ADR(char_row, char_col);

  if (c_ptr->tptr != 0) {
    /* take no action here, don't want to destroy object under player */
    bank_msg_print("There is already an object under you.", SPELLS1NUM);
    /* set free_turn_flag so that scroll/spell points won't be used */
    free_turn_flag = TRUE;
  }
  else {
    bank_place_object(char_row, char_col, SPELLS1NUM);

    vbxe_bank(VBXE_CAVBANK);				// need to lookup the tptr again -SJ
    spells1_invcopy(&t_list[c_ptr->tptr], OBJ_MUSH);
  }
}


/* Destroys any adjacent door(s)/trap(s)		-RAK-	*/
int8u __fastcall__ td_destroy(void)
{
  register int8u i, j, destroy;
  register cave_type *c_ptr;
  int8u tptr;


  vbxe_bank(VBXE_CAVBANK);
  destroy = FALSE;

  for (i = char_row-1; i <= char_row+1; ++i)
    for (j = char_col-1; j <= char_col+1; ++j) {
	  c_ptr = CAVE_ADR(i,j);
	  tptr = c_ptr->tptr;							// save tptr from cave location -SJ

	  if (tptr != 0) {
		vbxe_bank(VBXE_OBJBANK);
	    if (((t_list[tptr].tval >= TV_INVIS_TRAP) && (t_list[tptr].tval <= TV_CLOSED_DOOR) &&
		     (t_list[tptr].tval != TV_RUBBLE)) || (t_list[tptr].tval == TV_SECRET_DOOR)) {
		  if (bank_delete_object(i, j, SPELLS1NUM))
		    destroy = TRUE;
	    }
	    else if (t_list[tptr].tval == TV_CHEST) {
		  /* destroy traps on chest and unlock */
		  t_list[tptr].flags &= ~(CH_TRAPPED|CH_LOCKED);
		  t_list[tptr].name2 = SN_UNLOCKED;
		  bank_msg_print ("You have disarmed the chest.", SPELLS1NUM);
		  spells1_known2(&t_list[tptr]);
		  destroy = TRUE;
	    }
	  }
    }

  return(destroy);
}


/* Recharge a wand, staff, or rod.  Sometimes the item breaks. -RAK-*/
int8u __fastcall__ recharge(int8u num)
{
  int8u i, j, item_val, res;
  int16 x;
  register inven_type *i_ptr;


  res = FALSE;
  if (!spells1_find_range(TV_STAFF, TV_WAND, &i, &j))
    bank_msg_print("You have nothing to recharge.", SPELLS1NUM);
  else if (bank_get_item(&item_val, "Recharge which item?", i, j, CNIL, "", SPELLS1NUM)) {
    vbxe_bank(VBXE_OBJBANK);
    i_ptr = &inventory[item_val];
    res = TRUE;
    /* recharge I = recharge(20) = 1/6 failure for empty 10th level wand   */
    /* recharge II = recharge(60) = 1/10 failure for empty 10th level wand */
    /* make it harder to recharge high level, and highly charged wands, note that
       i can be negative, so check its value before trying to call randint().  */
    x = (int16)num + 50 - (int16)i_ptr->level - i_ptr->p1;
    if (x < 19)
	  x = 1;	/* Automatic failure.  */
    else
	  x = randint(x/10);

    if (x == 1) {
	  bank_msg_print("There is a bright flash of light.", SPELLS1NUM);
	  bank_inven_destroy(item_val, SPELLS1NUM);
	}
    else {
	  num = (num/(i_ptr->level+2)) + 1;
	  i_ptr->p1 += 2 + randint(num);
	  //if (known2_p(i_ptr))					item.ident & ID_KNOWN2
	  if (i_ptr->ident & ID_KNOWN2)				// in-line known2_p  -SJ
	    //clear_known2(i_ptr);
	    i_ptr->ident &= ~ID_KNOWN2;				// in-line clear_known2 -SJ
	  //clear_empty(i_ptr);
	  i_ptr->ident &= ~ID_EMPTY;				// in-line clear empty -SJ
	}
  }
  return(res);
}


/* Delete all creatures within max_sight distance	-RAK-	*/
/* NOTE : Winning creatures cannot be genocided			 */
int8u __fastcall__ mass_genocide(void)
{
  register int8u i;
  int8u result;
  register monster_type *m_ptr;
  register creature_type *r_ptr;


  result = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; --i) {
    vbxe_bank(VBXE_MONBANK);

    m_ptr = &m_list[i];
    r_ptr = &c_list[m_ptr->mptr];
    if ((m_ptr->cdis <= MAX_SIGHT) && ((r_ptr->cmove & CM_WIN) == 0)) {
	  bank_delete_monster(i, SPELLS1NUM);
	  result = TRUE;
	}
  }

  return(result);
}


/* Delete all creatures of a given type from level.	-RAK-	*/
/* This does not keep creatures of type from appearing later.	 */
/* NOTE : Winning creatures can not be genocided. */
int8u __fastcall__ genocide(void)
{
  register int8u i;
  int8u killed;
  char typ;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  //vtype out_val;


  killed = FALSE;
  if (bank_get_com("Which type of creature do you wish exterminated?", &typ, SPELLS1NUM))
    for (i = mfptr - 1; i >= MIN_MONIX; --i) {
	  vbxe_bank(VBXE_MONBANK);
	  m_ptr = &m_list[i];
	  r_ptr = &c_list[m_ptr->mptr];

	  if (typ == c_list[m_ptr->mptr].cchar)
	    if ((r_ptr->cmove & CM_WIN) == 0) {
	      bank_delete_monster(i, SPELLS1NUM);
	      killed = TRUE;
	    }
	    else {
	      /* genocide is a powerful spell, so we will let the player
		     know the names of the creatures he did not destroy,
		     this message makes no sense otherwise */
	      sprintf(out_val1, "The %s is unaffected.", r_ptr->name);
	      bank_msg_print(out_val1, SPELLS1NUM);
	    }
    }

  return(killed);
}
