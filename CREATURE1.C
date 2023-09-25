/* source/generate.c: initialize/create a dungeon or town level

     Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

     This software may be copied and distributed for educational, research, and
     not for profit purposes provided that this copyright and statement are
     included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "vbxebank.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <conio.h>

#pragma code-name("MON1BANK")
#pragma data-name("MON1BANK")
#pragma rodata-name("MON1BANK")


/* Local Prototypes */
int8u __fastcall__ mon1_distance(int8u y1, int8u x1, int8u y2, int8u x2);
int16u __fastcall__ get_mons_num(int8u level);
int8u __fastcall__ mon1_place_monster(int8u y, int8u x, int16u z, int8u slp);
int8u __fastcall__ mon1_popm(void);
int16u __fastcall__ mon1_max_hp(int8u *array);
int16u __fastcall__ mon1_pdamroll(int8u *array);
int16u __fastcall__ mon1_damroll(int8u num, int8u sides);
int8u __fastcall__ mon1_compact_monsters(void);
void __fastcall__ mon1_lite_spot(int8u y, int8u x);
int8u __fastcall__ mon1_panel_contains(int8u y, int8u x);
int8u __fastcall__ mon1_loc_symbol(int8u y, int8u x);
int8u __fastcall__ mon1_in_bounds(int8u y, int8u x);
int16u __fastcall__ mon1_randnor(int16u mean, int16u stand);
int8u __fastcall__ mon1_los(int8u fromY, int8u fromX, int8u toY, int8u toX);
void __fastcall__ delete_monster(int8u j);
void __fastcall__ fix1_delete_monster(int8u j);
void __fastcall__ fix2_delete_monster(int8u j);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MON1NUM

#undef  INBANK_MOD
#define INBANK_MOD			mon1


//*** DEBUGGING:
void __fastcall__ mon1_put_buffer(char *out_str, int8u row, int8u col)
#include "inbank\put_buffer.c"

int16u __fastcall__ mon1_damroll(int8u num, int8u sides)
#include "inbank\damroll.c"

int16u __fastcall__ mon1_pdamroll(int8u *array)
#include "inbank\pdamroll.c"

int16u __fastcall__ mon1_randnor(int16u mean, int16u stand)
#include "inbank\randnor.c"

void __fastcall__ mon1_lite_spot(int8u y, int8u x)
#include "inbank\lite_spot.c"

int8u __fastcall__ mon1_distance(int8u y1, int8u x1, int8u y2, int8u x2)
#include "inbank\distance.c"

int8u __fastcall__ mon1_in_bounds(int8u y, int8u x)
#include "inbank\in_bounds.c"

int8u __fastcall__ mon1_panel_contains(int8u y, int8u x)
#include "inbank\panel_contains.c"

int8u __fastcall__ mon1_popm(void)
#include "inbank\popm.c"

int8u __fastcall__ mon1_los(int8u fromY, int8u fromX, int8u toY, int8u toX)
#include "inbank\los.c"


/* Allocates a random monster				-RAK-	*/
void __fastcall__ alloc_monster(int8u num, int8u dis, int8u slp)
{
  register cave_type *c_ptr;
  register int8u y, x, i;
  int16u l;


  vbxe_bank(VBXE_CAVBANK);

  for (i = 0; i < num; ++i) {				// number of monsters to allocate
    do {
	  y = randint(cur_height-2);
	  x = randint(cur_width-2);
	  c_ptr = CAVE_ADR(y, x);
	} while (c_ptr->fval >= MIN_CLOSED_SPACE || (c_ptr->cptr != 0) || (mon1_distance(y, x, char_row, char_col) <= dis));

	vbxe_bank(VBXE_MONBANK);				// the following code needs the monster bank -SJ

    l = get_mons_num(dun_level);

    /* Dragons are always created sleeping here, so as to give the player a sporting chance.*/
    if (c_list[l].cchar == 'd' || c_list[l].cchar == 'D')
	  slp = TRUE;

    /* Place_monster() should always return TRUE here.  It does not matter if it fails though.*/
    mon1_place_monster(y, x, l, slp);
  }
}


/* Places creature adjacent to given location           -RAK-   */
int8u __fastcall__ summon_monster(int8u *y, int8u *x, int8u slp)
{
  register int8u i, j, k;
  int16u l;
  int8u summon;
  register cave_type *cave_ptr;


  vbxe_bank(VBXE_CAVBANK);					// can't restore bank, too much banking

  i = 0;
  summon = FALSE;
  l = get_mons_num(dun_level + MON_SUMMON_ADJ);
  do {
    j = *y - 2 + randint(3);
    k = *x - 2 + randint(3);
    if (mon1_in_bounds(j, k)) {
	  vbxe_bank(VBXE_CAVBANK);
	  cave_ptr = CAVE_ADR(j, k);
	  if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		/* Place_monster() should always return TRUE here.  */
		if (!mon1_place_monster(j, k, l, slp))
		  return FALSE;
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	  }
	}
    ++i;
  } while (i <= 9);

  return(summon);
}


/* Places a monster at given location			-RAK-	*/
void place_win_monster(void)
{
  register int8u y, x;
  int8u cur_pos;         				// max is 125, so int8 ok -SJ
  register monster_type *mon_ptr;
  register cave_type *c_ptr;


  if (!total_winner) {
    cur_pos = mon1_popm();
    /* Check for case where could not allocate space for the win monster, this should never happen. */
    if (cur_pos == 255)
      return;							// no abort(), so just return -SJ

    vbxe_bank(VBXE_MONBANK);			// bank in monster bank-don't really need it as this is just a pointer assignment
    mon_ptr = &m_list[cur_pos];		    // pointer to free space


    vbxe_bank(VBXE_CAVBANK);				// need CAVBANK for while loop
    do {
	  y = randint(cur_height-2);
	  x = randint(cur_width-2);
	  c_ptr = CAVE_ADR(y, x);
	} while ((c_ptr->fval >= MIN_CLOSED_SPACE) || (c_ptr->cptr != 0) || (c_ptr->tptr != 0) ||
	         (mon1_distance(y, x, char_row, char_col) <= MAX_SIGHT));

    vbxe_bank(VBXE_MONBANK);			// bank in monster bank

    mon_ptr->fy = y;
    mon_ptr->fx = x;
    mon_ptr->mptr = randint(WIN_MON_TOT) - 1 + m_level[MAX_MONS_LEVEL];
    if (c_list[mon_ptr->mptr].cdefense & CD_MAX_HP)
	  mon_ptr->hp = mon1_max_hp(c_list[mon_ptr->mptr].hd);
    else
	  mon_ptr->hp = mon1_pdamroll(c_list[mon_ptr->mptr].hd);
    /* the c_list speed value is 10 greater, so that it can be a int8u */
    mon_ptr->cspeed = c_list[mon_ptr->mptr].speed - 10 + py.flags.speed;
    mon_ptr->stunned = 0;
    mon_ptr->cdis = mon1_distance(char_row, char_col,y,x);
    mon_ptr->csleep = 0;

    vbxe_bank(VBXE_CAVBANK);
    //cave[y][x].cptr = cur_pos;
    (CAVE_ADR(y, x))->cptr = cur_pos;
  }
}


/* Return a monster suitable to be placed at a given level.  This makes
   high level monsters (up to the given level) slightly more common than
   low level monsters at any given level.   -CJS- */
int16u __fastcall__ get_mons_num(int8u level)
{
  register int16u i, j, num;


  vbxe_bank(VBXE_MONBANK);

  if (level == 0)
    i = randint (m_level[0]) - 1;
  else {
    if (level > MAX_MONS_LEVEL)
	  level = MAX_MONS_LEVEL;
    if (randint(MON_NASTY) == 1) {
	  i = mon1_randnor(0, 4);			// randnor should restore vbxe bank (MONBANK)
	  //i = randint(4);
	  //level = level + abs(i) + 1;
	  level = level + i + 1;	  				// FIXME: randnor can't return negative numbers
	  if (level > MAX_MONS_LEVEL)
	    level = MAX_MONS_LEVEL;
	}
    else {
	  /* This code has been added to make it slightly more likely to
	   get the higher level monsters. Originally a uniform
	   distribution over all monsters of level less than or equal to the
	   dungeon level. This distribution makes a level n monster occur
	   approx 2/n% of the time on level n, and 1/n*n% are 1st level. */

	  num = m_level[level] - m_level[0];
	  i = randint (num) - 1;
	  j = randint (num) - 1;
	  if (j > i)
	    i = j;
	  level = c_list[i + m_level[0]].level;
	}
    i = randint(m_level[level]-m_level[level-1]) - 1 + m_level[level-1];
  }

  vbxe_restore_bank();
  return i;
}


/* Places a monster at given location			-RAK-	*/
int8u __fastcall__ mon1_place_monster(int8u y, int8u x, int16u z, int8u slp)
{
  register int8u cur_pos;
  register monster_type *mon_ptr;


  cur_pos = mon1_popm();
  if (cur_pos == 255)
    return FALSE;

  vbxe_bank(VBXE_MONBANK);			// need MONBANK here!

  mon_ptr = &m_list[cur_pos];
  mon_ptr->fy = y;
  mon_ptr->fx = x;
  mon_ptr->mptr = z;
  if (c_list[z].cdefense & CD_MAX_HP)
    mon_ptr->hp = mon1_max_hp(c_list[z].hd);
  else
    mon_ptr->hp = mon1_pdamroll(c_list[z].hd);

  /* the c_list speed value is 10 greater, so that it can be a int8u */
  mon_ptr->cspeed = c_list[z].speed - 10 + py.flags.speed;
  mon_ptr->stunned = 0;
  mon_ptr->cdis = mon1_distance(char_row, char_col, y, x);
  mon_ptr->ml = FALSE;

  if (slp) {
    if (c_list[z].sleep == 0)
	  mon_ptr->csleep = 0;
    else
	  mon_ptr->csleep = (c_list[z].sleep * 2) + randint(c_list[z].sleep*10);
  }
  else
    mon_ptr->csleep = 0;

  vbxe_bank(VBXE_CAVBANK);				// moved cave set down here -SJ
  //cave[y][x].cptr = cur_pos;
  (CAVE_ADR(y, x))->cptr = cur_pos;

  return TRUE;
}


/* Place a trap with a given displacement of point      -RAK-   */
void __fastcall__ vault_monster(int8u y, int8u x, int8u num)
{
  register int8u i;
  int8u y1, x1;


  for (i = 0; i < num; ++i) {
    y1 = y;
    x1 = x;
    summon_monster(&y1, &x1, TRUE);
  }
}


/* Gives Max hit points					-RAK-	*/
int16u __fastcall__ mon1_max_hp(int8u *array)
{
  return(array[0] * array[1]);
}


/* Compact monsters					-RAK-	*/
/* Return TRUE if any monsters were deleted, FALSE if could not delete any
   monsters.  */
int8u __fastcall__ mon1_compact_monsters(void)
{
  register int16u i;
  int8 cur_dis;
  int8u delete_any;
  register monster_type *mon_ptr;


  bank_msg_print("Compacting monsters...", MON1NUM);
  vbxe_bank(VBXE_MONBANK);

  cur_dis = 66;
  delete_any = FALSE;
  do {
    for (i = mfptr - 1; i >= MIN_MONIX; --i) {
	  mon_ptr = &m_list[i];
	  if ((cur_dis < mon_ptr->cdis) && (randint(3) == 1)) {
	    /* Never compact away the Balrog!! */
	    if (c_list[mon_ptr->mptr].cmove & CM_WIN)
		  ; /* Do nothing */

	    /* in case this is called from within creatures(), this is a
		horrible hack, the m_list/creatures() code needs to be
		rewritten */
	    else if (hack_monptr < i) {
		  delete_monster(i);
		  delete_any = TRUE;
		}
	    else
		  /* fix1_delete_monster() does not decrement mfptr, so
		   don't set delete_any if this was called */
		  fix1_delete_monster(i);
	  }
	}

    if (!delete_any) {
	  cur_dis -= 6;
	  /* Can't delete any monsters, return failure.  */
	  if (cur_dis < 0)
	    return FALSE;
	}
  } while (!delete_any);

  return TRUE;
}


/* Deletes a monster entry from the level		-RAK-	*/
void __fastcall__ delete_monster(int8u j)
{
  register monster_type *m_ptr;
  register int8u fx, fy;


  vbxe_bank(VBXE_MONBANK);		// make sure monster bank is in

  m_ptr = &m_list[j];
  fy = m_ptr->fy;				// save these values -SJ
  fx = m_ptr->fx;				//

  vbxe_bank(VBXE_CAVBANK);		// clear the monster from the cave
  (CAVE_ADR(fy, fx))->cptr = 0;
  vbxe_restore_bank();

  if (m_ptr->ml) {
    mon1_lite_spot(fy, fx);
    vbxe_bank(VBXE_MONBANK);		// lite_spot kills vbxe bank
  }

  if (j != mfptr - 1) {			// copy the last monster into the slot of deleted monster
    m_ptr = &m_list[mfptr - 1];
    fy = m_ptr->fy;
    fx = m_ptr->fx;
    memcpy(&m_list[j], &m_list[mfptr - 1], sizeof(monster_type));

    vbxe_bank(VBXE_CAVBANK);
	(CAVE_ADR(fy, fx))->cptr = j;
	vbxe_restore_bank();
  }

  --mfptr;						// decrement number of monsters
  //m_list[mfptr] = blank_monster;
  memcpy(&m_list[mfptr], &blank_monster, sizeof(monster_type));		// copy a blank monster over last monster slot
  if (mon_tot_mult > 0)
    --mon_tot_mult;
}


/* The following two procedures implement the same function as delete monster.
   However, they are used within creatures(), because deleting a monster
   while scanning the m_list causes two problems, monsters might get two
   turns, and m_ptr/monptr might be invalid after the delete_monster.
   Hence the delete is done in two steps. */
/* fix1_delete_monster does everything delete_monster does except delete
   the monster record and reduce mfptr, this is called in breathe, and
   a couple of places in creatures.c */
void __fastcall__ fix1_delete_monster(int8u j)
{
  register monster_type *m_ptr;
  register int8u fx, fy;


  vbxe_bank(VBXE_MONBANK);			// make sure monster bank is in

  m_ptr = &m_list[j];
  fy = m_ptr->fy;					// save these values -SJ
  fx = m_ptr->fx;					//

  vbxe_bank(VBXE_CAVBANK);			// clear monster from cave
  (CAVE_ADR(fy, fx))->cptr = 0;
  vbxe_restore_bank();

  /* force the hp negative to ensure that the monster is dead, for example,
     if the monster was just eaten by another, it will still have positive
     hit points */
  m_ptr->hp = -1;
  //cave[m_ptr->fy][m_ptr->fx].cptr = 0;    // moved up above -SJ
  if (m_ptr->ml)
    mon1_lite_spot(fy, fx);
  if (mon_tot_mult > 0)
    --mon_tot_mult;
}


/* fix2_delete_monster does everything in delete_monster that wasn't done
   by fix1_monster_delete above, this is only called in creatures() */
void __fastcall__ fix2_delete_monster(int8u j)
{
  //register monster_type *m_ptr;
  int8u x, y;


  vbxe_bank(VBXE_MONBANK);
  if (j != mfptr - 1) {
    //m_ptr = &m_list[mfptr - 1];
    y = m_list[mfptr -1].fy;
    x = m_list[mfptr -1].fx;

    vbxe_bank(VBXE_CAVBANK);
    (CAVE_ADR(y, x))->cptr = j;
    vbxe_restore_bank();

    //cave[m_ptr->fy][m_ptr->fx].cptr = j;

    //m_list[j] = m_list[mfptr - 1];
    memcpy(&m_list[j], &m_list[mfptr - 1], sizeof(monster_type));
  }

  //m_list[mfptr - 1] = blank_monster;
  memcpy(&m_list[mfptr - 1], &blank_monster, sizeof(monster_type));

  --mfptr;
}


/* Decreases monsters hit points and deletes monster if needed.	*/
/* (Picking on my babies.)			       -RAK-   */
int8 __fastcall__ mon_take_hit(int8u monptr, int16 dam)
{
  int32u i;
  int32 new_exp, new_exp_frac;
  register monster_type *m_ptr;
  register creature_type *c_ptr;
  int8 m_take_hit;
  int32u tmp;
  int16u mon;


  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  m_ptr->hp -= dam;

  //*** DEBUGGING
  //sprintf(dbgstr, "m:%3d/%-3d", dam, m_ptr->hp);
  //mon1_put_buffer(dbgstr, 21, 5);
  //*** DEBUGGING

  m_ptr->csleep = 0;
  if (m_ptr->hp < 0) {
    i = bank_monster_death(m_ptr->fy, m_ptr->fx, c_list[m_ptr->mptr].cmove, MON1NUM);

    if ((py.flags.blind < 1 && m_ptr->ml) || (c_list[m_ptr->mptr].cmove & CM_WIN)) {
	  mon = m_ptr->mptr;
	  vbxe_bank(VBXE_GENBANK);
	  tmp = (c_recall[mon].r_cmove & CM_TREASURE) >> CM_TR_SHIFT;
	  if (tmp > ((i & CM_TREASURE) >> CM_TR_SHIFT))
	    i = (i & ~CM_TREASURE) | (tmp << CM_TR_SHIFT);
	  c_recall[mon].r_cmove = (c_recall[mon].r_cmove & ~CM_TREASURE) | i;
	  if (c_recall[mon].r_kills < MAX_SHORT)
	    c_recall[mon].r_kills++;
	}

    vbxe_bank(VBXE_MONBANK);
    c_ptr = &c_list[m_ptr->mptr];

    bank_eval_quest_mon(m_ptr->mptr, MON1NUM);		// evaluate quest monster?

    new_exp = (((int32)c_ptr->mexp * (int32)c_ptr->level) / (int32)py.misc.lev);
    new_exp_frac = (((c_ptr->mexp * (int16u)c_ptr->level) % (int16u)py.misc.lev) * 0x10000L / (int16u)py.misc.lev) + py.misc.exp_frac;
    if (new_exp_frac >= 0x10000L) {
	  ++new_exp;
	  py.misc.exp_frac = new_exp_frac - 0x10000L;
	}
    else
	  py.misc.exp_frac = new_exp_frac;

    py.misc.exp += new_exp;
    /* can't call prt_experience() here, as that would result in "new level" message appearing before "monster dies" message */

    //m_take_hit = m_ptr->mptr;
    m_take_hit = 1;					// only matters if it's positive, doesn't need to be set to mptr which is int16 -SJ
    /* in case this is called from within creatures(), this is a horrible hack, the m_list/creatures() code needs to be rewritten */
    if (hack_monptr < monptr)
	  delete_monster(monptr);
    else
	  fix1_delete_monster(monptr);
  }
  else
    m_take_hit = -1;

  return(m_take_hit);
}


/* Updates screen when monsters move about		-RAK-	*/
void __fastcall__ update_mon(int8 monptr)
{
  int8u flag, c_flags;
  //register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  register int8u fx, fy;
  int16u mon;


  flag = FALSE;

  vbxe_bank(VBXE_MONBANK);
  m_ptr = &m_list[monptr];
  fy = m_ptr->fy;							// get monster coords
  fx = m_ptr->fx;
  mon = m_ptr->mptr;

  if ((m_ptr->cdis <= MAX_SIGHT) && !(py.flags.status & PY_BLIND) && (mon1_panel_contains(fy, fx))) {
    /* Wizard sight. */
    if (wizard)
	  flag = TRUE;
    /* Normal sight. */
    else if (mon1_los(char_row, (int8u) char_col, fy, fx)) {
	  vbxe_bank(VBXE_MONBANK);
	  r_ptr = &c_list[m_ptr->mptr];

	  vbxe_bank(VBXE_CAVBANK);					// bank in the CAVBANK
	  c_flags = (CAVE_ADR(fy, fx))->flags;		// retrieve the cave light flags
	  vbxe_restore_bank();						// restore MONBANK

	 //if (c_ptr->pl || c_ptr->tl || (find_flag && m_ptr->cdis < 2 && player_light))
	 if ((c_flags & CAVE_PL) || (c_flags & CAVE_TL) || (find_flag && m_ptr->cdis < 2 && player_light)) {
	    if (!(r_ptr->cmove & CM_INVISIBLE))
		  flag = TRUE;
	    else if (py.flags.see_inv) {
		  flag = TRUE;
		  vbxe_bank(VBXE_GENBANK);
		  c_recall[mon].r_cmove |= CM_INVISIBLE;
		  vbxe_restore_bank();
		}
	  }
	  /* Infra vision.	 */
	  else if ((py.flags.see_infra > 0) && (m_ptr->cdis <= py.flags.see_infra) && (CD_INFRA & r_ptr->cdefense)) {
	    flag = TRUE;
	    vbxe_bank(VBXE_GENBANK);
		c_recall[mon].r_cdefense |= CD_INFRA;
		vbxe_restore_bank();
	  }
	}
  }

  vbxe_bank(VBXE_MONBANK);

  /* Light it up.	 */
  if (flag) {
    if (!m_ptr->ml) {
	  bank_disturb(1, 0, MON1NUM);
	  vbxe_bank(VBXE_MONBANK);
	  m_ptr->ml = TRUE;
	  mon1_lite_spot(fy, fx);
	  screen_change = TRUE; /* notify inven_command */
	}
  }
  /* Turn it off.	 */
  else if (m_ptr->ml) {
    m_ptr->ml = FALSE;
    mon1_lite_spot(fy, fx);
    screen_change = TRUE; /* notify inven_command */
  }
}
