/* source/types.h: global type declarations

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifndef TYPES_H
#define TYPES_H


typedef unsigned long  int32u;
typedef long           int32;
typedef unsigned int   int16u;
typedef signed int     int16;
typedef unsigned char  int8u;
typedef signed char    int8;

/* some machines will not accept 'signed char' as a type, and some accept it
   but still treat it like an unsigned character, let's just avoid it,
   any variable which can ever hold a negative value must be 16 or 32 bits */

#define VTYPESIZ        80
#define BIGVTYPESIZ     160
typedef char vtype[VTYPESIZ];
/* note that since its output can easily exceed 80 characters, objdes must
   always be called with a bigvtype as the first paramter */
typedef char bigvtype[BIGVTYPESIZ];
typedef char stat_type[7];

/* Many of the character fields used to be fixed length, which greatly
   increased the size of the executable.  I have replaced many fixed
   length fields with variable length ones. */

/* all fields are given the smallest possbile type, and all fields are
   aligned within the structure to their natural size boundary, so that
   the structures contain no padding and are minimum size */

/* bit fields are only used where they would cause a large reduction in
   data size, they should not be used otherwise because their use
   results in larger and slower code */

typedef struct creature_type
{
  char *name;           /* Descrip of creature  */
  int32u cmove;         /* Bit field            */
  int32u spells;        /* Creature spells      */
  int16u cdefense;      /* Bit field            */
  int16u mexp;          /* Exp value for kill   */
  int8u sleep;          /* Inactive counter/10  */
  int8u aaf;            /* Area affect radius   */
  int8u ac;             /* AC                   */
  int8u speed;          /* Movement speed+10    */
  char cchar;           /* Character rep.       */
  int8u hd[2];          /* Creatures hit die    */
  int8u damage[4];      /* Type attack and damage*/
  int8u level;          /* Level of creature    */
} creature_type;

typedef struct m_attack_type    /* Monster attack and damage types */
  {
    int8u attack_type;
    int8u attack_desc;
    int8u attack_dice;
    int8u attack_sides;
  } m_attack_type;

typedef struct recall_type      /* Monster memories. -CJS- */
  {
    int32u r_cmove;								// 4
    int32u r_spells;							// 4
    int16u r_kills, r_deaths;					// 4
    int16u r_cdefense;							// 2
    int8u r_wake, r_ignore;						// 2
    int8u r_attacks[MAX_MON_NATTACK];			// 1x4
  } recall_type;								// 20 bytes

typedef struct monster_type
{
  int16 hp;             /* Hit points           */
  int8 csleep;          /* Inactive counter     */		// was int -SJ
  int8 cspeed;          /* Movement speed       */		// was int -SJ
  int16u mptr;          /* Pointer into creature*/
  /* Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256 */
  int8u fy;             /* Y Pointer into map   */
  int8u fx;             /* X Pointer into map   */
  int8u cdis;           /* Cur dis from player  */
  int8u ml;
  int8u stunned;
  int8u confused;
} monster_type;

typedef struct treasure_type
{
  char *name;           /* Object name          */
  int32u flags;         /* Special flags        */
  int8u tval;           /* Category number      */
  char tchar;           /* Character representation*/
  int16 p1;             /* Misc. use variable   */
  //int32 cost;           /* Cost of item         */
  int16 cost;			/* Cost of item			changed from int32 -SJ */
  int8u subval;         /* Sub-category number  */
  int8u number;         /* Number of items      */
  int16u weight;        /* Weight               */
  int8 tohit;           /* Plusses to hit       */
  int8 todam;           /* Plusses to damage    */
  int8 ac;              /* Normal AC            */
  int8 toac;            /* Plusses to AC        */
  int8u damage[2];      /* Damage when hits     */
  int8u level;          /* Level item first found */
} treasure_type;

/* only damage, ac, and tchar are constant; level could possibly be made
   constant by changing index instead; all are used rarely */
/* extra fields x and y for location in dungeon would simplify pusht() */
/* making inscrip a pointer and mallocing space does not work, there are
   two many places where inven_types are copied, which results in dangling
   pointers, so we use a char array for them instead */
#define INSCRIP_SIZE 13  /* notice alignment, must be 4*x + 1 */
typedef struct inven_type
{
  int16u index;         /*2 Index to object_list */
  int8u name2;          /*1 Object special name  */
  char inscrip[INSCRIP_SIZE]; /* Object inscription   */
  int32u flags;         /*4 Special flags        */
  int8u tval;           /*1 Category number      */
  int8u tchar;          /*1 Character representation*/
  int16 p1;             /*2 Misc. use variable   */								// FIXME: can be int8?
  int32 cost;           /*4 Cost of item         */
  int8u subval;         /*1 Sub-category number  */
  int8u number;         /*1 Number of items      */
  int16u weight;        /*2 Weight               */
  int8 tohit;           /*1 Plusses to hit       */                              // was int16 SJ
  int8 todam;           /*1 Plusses to damage    */
  int8 ac;              /*1 Normal AC                */
  int8 toac;            /*1 Plusses to AC        */
  int8u damage[2];      /*2 Damage when hits     */
  int8u level;          /*1 Level item first found */
  int8u ident;          /*1 Identify information */
} inven_type;  // size 27 + 13 = 40 bytes

#define PLAYER_NAME_SIZE 27

typedef struct player_type
{
  struct misc
    {
      char name[PLAYER_NAME_SIZE];      /* Name of character    */
      int8u male;       /* Sex of character     */
      int32 au;         /* Gold                 */
      int32 max_exp;    /* Max experience       */
      int32 exp;        /* Cur experience       */
      int16u exp_frac;  /* Cur exp fraction * 2^16 */
      int16u age;       /* Characters age       */
      int16u ht;        /* Height               */
      int16u wt;        /* Weight               */
      int8u  lev;       /* Level                */                      // was int16u SJ - max level = 40
      int8u  max_dlv;   /* Max level explored   */                      // was int16u SJ
      int8u  srh;       /* Chance in search     */						// should be 0-200 -SJ
      int8  fos;        /* Frenq of search      */
      int8u  bth;       /* Base to hit          */						// should be 0-200 -SJ
      int8u  bthb;      /* BTH with bows        */						// should be 0-200 -SJ
      int16u mana;      /* Mana points          */
      int16u mhp;       /* Max hit pts          */
      int8  ptohit;     /* Plusses to hit       */
      int8  ptodam;     /* Plusses to dam       */
      int8  pac;        /* Total AC             */
      int8  ptoac;      /* Magical AC           */
      int8  dis_th;     /* Display +ToHit       */
      int8  dis_td;     /* Display +ToDam       */
      int8  dis_ac;     /* Display +ToAC        */
      int8  dis_tac;    /* Display +ToTAC       */
      int8u disarm;     /* % to Disarm          */						// should be 0-200 -SJ
      int8u save;       /* Saving throw         */						// should be 0-100 -SJ
      int8  sc;         /* Social Class         */
      int8  stl;        /* Stealth factor       */
      int8u pclass;     /* # of class           */
      int8u prace;      /* # of race            */
      int8u hitdie;     /* Char hit die         */
      int8u expfact;    /* Experience factor    */
      int16 cmana;      /* Cur mana pts         */
      int16u cmana_frac; /* Cur mana fraction * 2^16 */
      int16 chp;        /* Cur hit pts          */
      int16u chp_frac;  /* Cur hit fraction * 2^16 */
      int16u frac_chr;	/* Fractional Charisma, used in Tavern -SJ */
      char history[4][60]; /* History record    */
    } misc;
  /* Stats now kept in arrays, for more efficient access. -CJS- */
  struct stats
    {
      int8u max_stat[6];        /* What is restored */
      int8u cur_stat[6];        /* What is natural */
      //int16 mod_stat[6];      /* What is modified, may be +/- */
      int8 mod_stat[6];     /* What is modified, may be +/- */
      int8u use_stat[6];        /* What is used */
    } stats;
  struct flags
    {
      int32u status;            /* Status of player    */
      int16 rest;               /* Rest counter        */		// needs to be int16 to allow more than 128 rest turns -SJ
      int16u blind;             /* Blindness counter   */
      int8u paralysis;          /* Paralysis counter   */
      int8u confused;           /* Confusion counter   */		// was int16
      int16 food;               /* Food counter        */		// was int16
      int8 food_digested;       /* Food per round      */		// was int16
      int16 protection;         /* Protection fr. evil */
      int8 speed;               /* Cur speed adjust    */		// was int16
      int8u fast;               /* Temp speed change   */		// was int16
      int8u slow;               /* Temp speed change   */		// was int16
      int8u afraid;             /* Fear                */		// was int16
      int8u poisoned;           /* Poisoned counter    */		// was int16
      int16u image;             /* Hallucinate counter */
      int8u protevil;           /* Protect VS evil     */
      int8u invuln;             /* Increases AC        */
      int8u hero;               /* Heroism             */
      int8u shero;              /* Super Heroism       */
      int8u blessed;            /* Blessed             */
      int8u resist_heat;        /* Timed heat resist   */
      int8u resist_cold;        /* Timed cold resist   */
      int8u detect_inv;         /* Timed see invisible */
      int8u word_recall;        /* Timed teleport level*/
      int8u see_infra;          /* See warm creatures  */
      int8u tim_infra;          /* Timed infra vision  */
      int8u see_inv;            /* Can see invisible   */
      int8u teleport;           /* Random teleportation*/
      int8u free_act;           /* Never paralyzed     */
      int8u slow_digest;        /* Lower food needs    */
      int8u aggravate;          /* Aggravate monsters  */
      int8u fire_resist;        /* Resistance to fire  */
      int8u cold_resist;        /* Resistance to cold  */
      int8u acid_resist;        /* Resistance to acid  */
      int8u regenerate;         /* Regenerate hit pts  */
      int8u lght_resist;        /* Resistance to light */
      int8u ffall;              /* No damage falling   */
      int8u sustain_str;        /* Keep strength       */
      int8u sustain_int;        /* Keep intelligence   */
      int8u sustain_wis;        /* Keep wisdom         */
      int8u sustain_con;        /* Keep constitution   */
      int8u sustain_dex;        /* Keep dexterity      */
      int8u sustain_chr;        /* Keep charisma       */
      int8u confuse_monster;    /* Glowing hands.    */
      int8u new_spells;         /* Number of spells can learn. */
    } flags;
} player_type;

typedef struct spell_type
{  /* spell name is stored in spell_names[] array at index i, +31 if priest */
  int8u slevel;
  int8u smana;
  int8u sfail;
  int8u sexp;   /* 1/4 of exp gained for learning spell */
} spell_type;

typedef struct race_type
{
  char  *trace;         /* Type of race                 */
  int8 str_adj;         /* adjustments                  */                              // SJ was int16
  int8 int_adj;
  int8 wis_adj;
  int8 dex_adj;
  int8 con_adj;
  int8 chr_adj;
  int8u b_age;         /* Base age of character         */
  int8u m_age;         /* Maximum age of character      */
  int8u m_b_ht;        /* base height for males         */
  int8u m_m_ht;        /* mod height for males          */
  int8u m_b_wt;        /* base weight for males         */
  int8u m_m_wt;        /* mod weight for males          */
  int8u f_b_ht;        /* base height females           */
  int8u f_m_ht;        /* mod height for females        */
  int8u f_b_wt;        /* base weight for female        */
  int8u f_m_wt;        /* mod weight for females        */
  int8 b_dis;          /* base chance to disarm         */                      // was int16
  int8 srh;                /* base chance for search    */
  int8 stl;                /* Stealth of character              */
  int8 fos;                /* frequency of auto search  */
  int8 bth;                /* adj base chance to hit    */
  int8 bthb;           /* adj base to hit with bows     */
  int8 bsav;           /* Race base for saving throw    */
  int8u bhitdie;       /* Base hit points for race      */
  int8u infra;         /* See infra-red                 */
  int8u b_exp;         /* Base experience factor        */
  int8u rtclass;       /* Bit field for class types     */
} race_type;

typedef struct class_type
{
  char *title;          /* type of class                */
  int8u adj_hd;         /* Adjust hit points            */
  int8u mdis;           /* mod disarming traps          */
  int8u msrh;           /* modifier to searching        */
  int8u mstl;           /* modifier to stealth          */
  int8u mfos;           /* modifier to freq-of-search   */
  int8u mbth;           /* modifier to base to hit      */
  int8u mbthb;          /* modifier to base to hit - bows*/
  int8u msav;           /* Class modifier to save       */
  int8 madj_str;       /* Class modifier for strength  */		// was int16
  int8 madj_int;       /* Class modifier for intelligence*/		// was int16
  int8 madj_wis;       /* Class modifier for wisdom    */		// was int16
  int8 madj_dex;       /* Class modifier for dexterity */		// was int16
  int8 madj_con;       /* Class modifier for constitution*/		// was int16
  int8 madj_chr;       /* Class modifier for charisma  */		// was int16
  int8u spell;          /* class use mage spells        */
  int8u m_exp;          /* Class experience factor      */
  int8u first_spell_lev;/* First level where class can use spells. */
} class_type;

typedef struct background_type
{
  char *info;           /* History information          */
  int8u roll;           /* Die roll needed for history  */
  int8u chart;          /* Table number                 */
  int8u next;           /* Pointer to next table        */
  int8u bonus;          /* Bonus to the Social Class+50 */
} background_type;

/* Cave light flags (instead of bitfield) -SJ */
#define CAVE_LR		0x01
#define CAVE_FM		0x02
#define CAVE_PL		0x04
#define CAVE_TL		0x08

/* Replace:
   flags lr=TRUE with flags |= CAVE_LR

   flags lr=FALSE with flags &= ~CAVE_LR

   if (cave->lr) with if (cave->flags & CAVE_LR)
*/

typedef struct cave_type
{
  int8u cptr;
  int8u tptr;
  int8u fval;
  int8u flags;
  //unsigned lr : 1;
  //unsigned fm : 1;
  //unsigned pl : 1;
  //unsigned tl : 1;
} cave_type;

typedef struct owner_type
{
  char *owner_name;
  int16 max_cost;
  int8u max_inflate;
  int8u min_inflate;
  int8u haggle_per;
  int8u owner_race;
  int8u insult_max;
} owner_type;

typedef struct inven_record
{
  int32 scost;				// size 4
  inven_type sitem;			// size 40
} inven_record; 			// 44 bytes

typedef struct store_type
{
  int32 store_open;			// 4
  int16 insult_cur;			// 2
  int8u owner;				// 1
  int8u store_ctr;			// 1
  int16u good_buy;			// 2
  int16u bad_buy;			// 2
  inven_record store_inven[STORE_INVEN_MAX];		// 24 items max in store (24x44 = 1056 bytes)
} store_type;				// 1068 bytes

typedef struct high_scores
{
  int32 points;						// 4
  //int32 birth_date;
  //int16 uid;
  int16 mhp;						// 2
  int16 chp;						// 2
  int8u dun_level;					// 1
  int8u lev;						// 1
  int8u max_dlv;					// 1
  int8u sex;						// 1
  int8u race;						// 1
  int8u class;						// 1
  char name[PLAYER_NAME_SIZE];		// 27
  char died_from[25];				// 25
} high_scores;						// 66 bytes


typedef struct quest_type
{
  int8u type;						// quest type (see constant.h)				1
  int8u status;						// quest status (see constant.h)			1
  int8u source;						// quest source								1
  int16u p1;						// misc storage (item/creature)				2
  int16u p2;						// misc storage (num monsters killed)		2
  int32u exp;						// experience for completing quest			4
  int32u gold;						// gold for completing quest				4
  int16u item;						// item for completing quest (magic?)		2
} quest_type;																//  17 bytes


#endif
