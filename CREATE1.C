/* source/create.c: create a player character

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <conio.h>


#pragma code-name("CREATE1BANK")
#pragma rodata-name("CREATE1BANK")
#pragma data-name("CREATE1BANK")


/* Local prototypes, duplicated in CREATEBANK */
void get_all_stats(void);
void get_stats(void);
void get_name(void);

int8 __fastcall__ create1_todam_adj(void);
int8 __fastcall__ create1_tohit_adj(void);
int8 __fastcall__ create1_toac_adj(void);
int8 __fastcall__ create1_con_adj(void);
int8 __fastcall__ create1_todis_adj(void);
void __fastcall__ create1_change_stat(int8u stat, int8 amount);
void __fastcall__ create1_put_buffer(char *out_str, int8 row, int8 col);
int16u __fastcall__ create1_randnor(int16u mean, int16u stand);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		CREATE1NUM


void __fastcall__ create1_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

void __fastcall__ create1_change_stat(int8u stat, int8 amount)
#include "inbank\change_stat.c"

int8 __fastcall__ create1_todam_adj(void)
#include "inbank\todam_adj.c"

int8 __fastcall__ create1_tohit_adj(void)
#include "inbank\tohit_adj.c"

int8 __fastcall__ create1_con_adj(void)
#include "inbank\con_adj.c"

int8 __fastcall__ create1_toac_adj(void)
#include "inbank\toac_adj.c"

int8 __fastcall__ create1_todis_adj(void)
#include "inbank\todis_adj.c"

int16u __fastcall__ create1_randnor(int16u mean, int16u stand)
#include "inbank\randnor.c"


/* Allows player to select a race                       -JWT-   */
void choose_race(void)
{
  register int8u j, k;
  int8u l, m, exit_flag;
  char s;
  char tmp_str[80];
  register player_type *p_ptr;
  register race_type *r_ptr;


  j = 0;
  k = 0;
  l = 2;
  m = 21;

  bank_clear_from(20, CREATE1NUM);

  create1_put_buffer("Choose a race:", 20, 2);

  do {
      vbxe_bank(VBXE_SPLBANK);

      (void) sprintf(tmp_str, "%c) %s", k+'a', race[j].trace);
      create1_put_buffer(tmp_str, m, l);

      ++k;
      l += 15;
      if (l > 70)
	  {
	    l = 2;
	    ++m;
	  }
      ++j;
  }
  while (j < MAX_RACES);
  exit_flag = FALSE;
  do
    {
      s = cgetc();
      j = s - 'a';
      if (j < MAX_RACES)
        exit_flag = TRUE;
    }
  while (!exit_flag);

  vbxe_bank(VBXE_SPLBANK);

  p_ptr = &py;
  r_ptr = &race[j];
  p_ptr->misc.prace  = j;
  create1_put_buffer(r_ptr->trace, 3, 15);
}


/* Will print the history of a character                        -JWT-   */
void print_history(void)
{
  register int i;


  create1_put_buffer("Character Background", 14, 27);
  for (i = 0; i < 4; ++i)
    bank_prt(py.misc.history[i], i+15, 10, CREATE1NUM);
}


/* Get the racial history, determines social class      -RAK-   */
/* Assumptions: Each race has init history beginning at         */
/*              (race-1)*3+1                                    */
/*              All history parts are in ascending order        */
void get_history(void)
{
  int8u hist_ptr, cur_ptr, test_roll, flag;                                             // changed all to int8u -SJ
  register int8u start_pos, end_pos, cur_len;
  int8u line_ctr, new_start, social_class;
  char history_block[240];
  register background_type *b_ptr;


  /* Get a block of history text                                */
  hist_ptr = py.misc.prace*3 + 1;
  history_block[0] = '\0';
  social_class = randint(4);
  cur_ptr = 0;
  do
    {
      flag = FALSE;
      do
	{
	  if (background[cur_ptr].chart == hist_ptr)
	    {
	      test_roll = randint(100);
	      while (test_roll > background[cur_ptr].roll)
		    cur_ptr++;
	      b_ptr = &background[cur_ptr];
	      (void) strcat(history_block, b_ptr->info);
	      social_class += b_ptr->bonus - 50;
	      if (hist_ptr > b_ptr->next)
		    cur_ptr = 0;
	      hist_ptr = b_ptr->next;
	      flag = TRUE;
	    }
	  else
	    ++cur_ptr;
	}
      while (!flag);
    }
  while (hist_ptr >= 1);

  /* clear the previous history strings */
  for (hist_ptr = 0; hist_ptr < 4; ++hist_ptr)
    py.misc.history[hist_ptr][0] = '\0';

  /* Process block of history text for pretty output    */
  start_pos = 0;
  end_pos   = strlen(history_block) - 1;
  line_ctr  = 0;
  flag = FALSE;
  while (history_block[end_pos] == ' ')
    end_pos--;
  do
    {
      while (history_block[start_pos] == ' ')
	start_pos++;
      cur_len = end_pos - start_pos + 1;
      if (cur_len > 60)
	{
	  cur_len = 60;
	  while (history_block[start_pos+cur_len-1] != ' ')
	    cur_len--;
	  new_start = start_pos + cur_len;
	  while (history_block[start_pos+cur_len-1] == ' ')
	    cur_len--;
	}
      else
	flag = TRUE;
      (void) strncpy(py.misc.history[line_ctr], &history_block[start_pos],
		     cur_len);
      py.misc.history[line_ctr][cur_len] = '\0';
      line_ctr++;
      start_pos = new_start;
    }
  while (!flag);

  /* Compute social class for player                    */
  if (social_class > 100)
    social_class = 100;
  else if (social_class < 1)
    social_class = 1;
  py.misc.sc = social_class;
}


/* Gets the character's sex                             -JWT-   */
void get_sex(void)
{
  register int8u exit_flag;
  char c;


  exit_flag = FALSE;
  bank_clear_from(20, CREATE1NUM);

  create1_put_buffer("Choose a sex:", 20, 2);
  create1_put_buffer("m) Male       f) Female", 21, 2);
  do
    {
      //move_cursor (20, 29);
      /* speed not important here */
      //c = inkey();
      c = cgetc();
      if (c == 'f' || c == 'F')
	  {
	    py.misc.male = FALSE;
	    create1_put_buffer("Female", 4, 15);
	    exit_flag = TRUE;
	  }
      else if (c == 'm' || c == 'M')
	  {
	    py.misc.male = TRUE;
	    create1_put_buffer("Male", 4, 15);
	    exit_flag = TRUE;
	  }
    }
  while (!exit_flag);
}


/* Computes character's age, height, and weight         -JWT-   */
void get_ahw(void)
{
  register int8u i;                                                                                     // changed to int8u -SJ


  i = py.misc.prace;
  py.misc.age = race[i].b_age + randint((int)race[i].m_age);
  if (py.misc.male)
    {
      py.misc.ht = create1_randnor((int)race[i].m_b_ht, (int)race[i].m_m_ht);
      py.misc.wt = create1_randnor((int)race[i].m_b_wt, (int)race[i].m_m_wt);
    }
  else
    {
      py.misc.ht = create1_randnor((int)race[i].f_b_ht, (int)race[i].f_m_ht);
      py.misc.wt = create1_randnor((int)race[i].f_b_wt, (int)race[i].f_m_wt);
    }
  py.misc.disarm = race[i].b_dis + create1_todis_adj();
}


/* Gets a character class                               -JWT-   */
void get_class(void)
{
  register int8u i, j;                                                                  // changed all to int8 -SJ
  int8u k, l, m;
  int16u min_value, max_value;
  int8u cl[MAX_CLASS], exit_flag;
  register struct misc *m_ptr;
  register player_type *p_ptr;
  class_type *c_ptr;
  char tmp_str[80], s;
  int8u mask;								// changed to int8u, only 6 classes!


  vbxe_bank(VBXE_SPLBANK);

  for (j = 0; j < MAX_CLASS; ++j)
    cl[j] = 0;
  i = py.misc.prace;
  j = 0;
  k = 0;
  l = 2;
  m = 21;
  mask = 0x1;

  bank_clear_from(20, CREATE1NUM);
  create1_put_buffer("Choose a class:", 20, 2);

  do
    {
      if (race[i].rtclass & mask)
	  {
	    sprintf(tmp_str, "%c) %s", k+'a', class[j].title);
	    create1_put_buffer(tmp_str, m, l);
	    cl[k] = j;
	    l += 15;
	    if (l > 70)
	    {
	      l = 2;
	      ++m;
	    }
	    ++k;
	  }
      ++j;
      mask <<= 1;
    }
  while (j < MAX_CLASS);

  py.misc.pclass = 0;
  exit_flag = FALSE;
  do
    {
      s = cgetc();
      j = s - 'a';
      if (j < k)
	{
	  py.misc.pclass = cl[j];
	  c_ptr = &class[py.misc.pclass];
	  exit_flag = TRUE;

      bank_clear_from(20, CREATE1NUM);
	  create1_put_buffer(c_ptr->title, 5, 15);

	  /* Adjust the stats for the class adjustment          -RAK-   */
	  p_ptr = &py;
	  create1_change_stat(A_STR, c_ptr->madj_str);
	  create1_change_stat(A_INT, c_ptr->madj_int);
	  create1_change_stat(A_WIS, c_ptr->madj_wis);
	  create1_change_stat(A_DEX, c_ptr->madj_dex);
	  create1_change_stat(A_CON, c_ptr->madj_con);
	  create1_change_stat(A_CHR, c_ptr->madj_chr);
	  for(i = 0; i < 6; ++i) {
	    p_ptr->stats.cur_stat[i] = p_ptr->stats.max_stat[i];
	    bank_set_use_stat(i, CREATE1NUM);
	  }

	  p_ptr->misc.ptodam = create1_todam_adj();     /* Real values          */
	  p_ptr->misc.ptohit = create1_tohit_adj();
	  p_ptr->misc.ptoac  = create1_toac_adj();
	  p_ptr->misc.pac    = 0;
	  p_ptr->misc.dis_td = p_ptr->misc.ptodam; /* Displayed values  */
	  p_ptr->misc.dis_th = p_ptr->misc.ptohit;
	  p_ptr->misc.dis_tac= p_ptr->misc.ptoac;
	  p_ptr->misc.dis_ac = p_ptr->misc.pac + p_ptr->misc.dis_tac;

	  /* now set misc stats, do this after setting stats because
	     of con_adj() for hitpoints */
	  m_ptr = &py.misc;
	  m_ptr->hitdie += c_ptr->adj_hd;
	  m_ptr->mhp = create1_con_adj() + m_ptr->hitdie;
	  m_ptr->chp = m_ptr->mhp;
	  m_ptr->chp_frac = 0;

	  /* initialize hit_points array */
	  /* put bounds on total possible hp, only succeed if it is within
	     1/8 of average value */
	  min_value = (MAX_PLAYER_LEVEL*3/8 * (m_ptr->hitdie-1)) + MAX_PLAYER_LEVEL;
	  max_value = (MAX_PLAYER_LEVEL*5/8 * (m_ptr->hitdie-1)) + MAX_PLAYER_LEVEL;
	  player_hp[0] = m_ptr->hitdie;

	  do {
	    for (i = 1; i < MAX_PLAYER_LEVEL; ++i) {
		  player_hp[i] = randint(m_ptr->hitdie);
		  player_hp[i] += player_hp[i-1];
		}
  	  } while ((player_hp[MAX_PLAYER_LEVEL-1] < min_value) || (player_hp[MAX_PLAYER_LEVEL-1] > max_value));

	  m_ptr->bth += c_ptr->mbth;
	  m_ptr->bthb += c_ptr->mbthb;  /*RAK*/
	  m_ptr->srh += c_ptr->msrh;
	  m_ptr->disarm += c_ptr->mdis;
	  m_ptr->fos += c_ptr->mfos;
	  m_ptr->stl += c_ptr->mstl;
	  m_ptr->save += c_ptr->msav;
	  m_ptr->expfact += c_ptr->m_exp;
	}
  } while (!exit_flag);
}


/* Given a stat value, return a monetary value, which affects the amount
   of gold a player has. */
int16u monval(int8u i)
{
  return 5 * ((int16u)i - 10);
}


static void get_money(void)
{
  register int16u tmp, gold;


  tmp = monval(py.stats.max_stat[A_STR]) + monval(py.stats.max_stat[A_INT])
      + monval(py.stats.max_stat[A_WIS]) + monval(py.stats.max_stat[A_CON])
      + monval(py.stats.max_stat[A_DEX]);

  gold = py.misc.sc*6 + randint (25) + 325;     		/* Social Class adj */
  gold -= tmp;                                  		/* Stat adj */
  gold += monval(py.stats.max_stat[A_CHR]);             /* Charisma adj */
  if (!py.misc.male)
    gold += 50;                 /* She charmed the banker into it! -CJS- */
  if (gold < 80)
    gold = 80;                  /* Minimum */
  py.misc.au = gold;
}


/* ---------- M A I N  for Character Creation Routine ---------- */
/*                                                      -JWT-   */
void create_character(void)
{
  register int8u exit_flag = 1;
  char c;


  vbxe_bank(VBXE_SPLBANK);				// default to the player data bank
  memset(&py, 0, sizeof(py));			// clear character (so restored dead character doesn't show garbage.)

  bank_put_character();					// bank in put_character code

  choose_race();
  get_sex();

  /* here we start a loop giving a player a choice of characters -RGM- */
  bank_get_all_stats();
  get_history();
  get_ahw();
  print_history();
  bank_put_misc1();
  bank_put_stats();

  bank_clear_from(20, CREATE1NUM);
  create1_put_buffer("Hit space to reroll or ESC to accept characteristics:", 20, 2);

  do {
    c = cgetc();
    if (c == ESCAPE)
      exit_flag = 0;
    else if (c == ' ') {
	  bank_get_all_stats();

	  get_history();
	  get_ahw();
	  print_history();
      bank_put_misc1();
      bank_put_stats();
    }
  }               /* done with stats generation */
  while (exit_flag == 1);

  get_class();
  get_money();
  bank_put_stats();
  bank_put_misc2();
  bank_put_misc3();
  get_name();
}


/* Gets a name for the character                        -JWT-   */
void get_name(void)
{
  #define BLANK_LENGTH    24
  char blank_string[BLANK_LENGTH];
  int8u ret;


  create1_put_buffer("Enter your player's name  [press <RETURN> when finished]", 21, 2);
  create1_put_buffer(&blank_string[BLANK_LENGTH-23], 2, 15);

  ret = bank_get_string(py.misc.name, 2, 15, 23, CREATE1NUM);

  //if (!get_string(py.misc.name, 2, 15, 23) || py.misc.name[0] == 0)
  if (!ret || py.misc.name[0] == 0) {
    create1_put_buffer(py.misc.name, 2, 15);
  }

  bank_clear_from(20, CREATE1NUM);
}


/* Changes the name of the character                    -JWT-   */
void change_name(void)
{
  char c;
  int8u flag;


  flag = FALSE;
  bank_display_char(CREATE1NUM);
  do {
    bank_prt( "<c>hange character name, ESC to exit.", 21, 2, CREATE1NUM);
    c = cgetc();
    switch(c) {
	  case 'c':
	    get_name();
	    flag = TRUE;
	    break;
	  case ESCAPE: case ' ':
	  case RETURN:
	    flag = TRUE;
	    break;
	  default:
	    break;
	}
  } while (!flag);
}

