#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>


#pragma code-name("INITBANK")
#pragma data-name("INITBANK")
#pragma rodata-name("INITBANK")


int16u player_init[MAX_CLASS][5] = {
		{ 344, 365, 123,  30, 103},     /* Warrior       */
		{ 344, 365, 123,  30, 318},     /* Mage          */
		{ 344, 365, 123,  30, 322},     /* Priest        */
		{ 344, 365, 123,  30, 318},     /* Rogue         */
		{ 344, 365, 123,  30, 318},     /* Ranger        */
		{ 344, 365, 123,  30, 322}      /* Paladin       */
};


/* Local Prototypes - in INITBANK */
void __fastcall__ init_invcopy(inven_type *to, int16u from_index);
void __fastcall__ init_inven_carry(inven_type *i_ptr);
int8u __fastcall__ init_known1_p(inven_type *i_ptr);
int8 __fastcall__ init_object_offset(inven_type *t_ptr);
void __fastcall__ init_seeds(int16u seed);
void __fastcall__ init_m_level(void);
void __fastcall__ init_t_level(void);
void __fastcall__ char_inven_init(void);
void __fastcall__ store_init(void);
void __fastcall__ magic_init(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		INITNUM

#undef  INBANK_MOD
#define INBANK_MOD			init


void __fastcall__ init_invcopy(inven_type *to, int16u from_index)
#include "inbank\invcopy.c"

int8u __fastcall__ init_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

int8 __fastcall__ init_object_offset(inven_type *t_ptr)                                                              // changed return type to int8 SJ
#include "inbank\object_offset.c"



/* Initialize, restore, and get the ball rolling. -RAK-   */
/* Considerable changes for Atari 8-bit. -SJ */
void moria_init(void)
{
  int8u generate;
  int8u result = FALSE;
  int8u new_game = FALSE;


  // Initialize variables moved into zeropage -SJ
  dun_level = 0;

  // Clear monster memory recall
  vbxe_bank(VBXE_GENBANK);
  memset(&c_recall, 0, sizeof(c_recall));

  /* Get the highscores from cartridge */
  copy_highscores_to_ram();

  /* Grab a random seed from the clock */
  init_seeds(0);

  /* Init monster and treasure levels for allocate */
  init_m_level();
  init_t_level();

  /* Init the store inventories */
  store_init();

  /* This restoration of a saved character may get ONLY the monster memory. In
   this case, get_char returns false. It may also resurrect a dead character
   (if you are the wizard). In this case, it returns true, but also sets the
   parameter "generate" to true, as it does not recover any cave details. */

  /* on the atari if the character is dead we won't load anything and return false.
     Also, there is no wizard resurrections from flashcart... -SJ */

  result = get_char(&generate);					// returns true if character not dead

  if (result) {
    generate = FALSE;							// live character restored, don't generate dungeon -SJ
    character_generated = TRUE;					// character is generated already, display stats -SJ
    bank_change_name(INITNUM);
  }
  else {
	/* Create character */
    bank_create_character();
    bank_msg_print("", INITNUM);				// pause for any messages -SJ

    char_inven_init();
    py.flags.food = 7500;
    py.flags.food_digested = 2;
    bank_init_calc_spells_prayers();

    character_generated = TRUE;
    generate = TRUE;
  }

  /* Init magic item descriptions, etc..  */
  /* We do this after the randes_seed has been restored otherwise names change each game session. -SJ */
  magic_init();

  /* Begin the game */
  bank_vbxe_clear(INITNUM);					// clear the screen
  bank_prt_stat_block(INITNUM);

  if (generate)
    bank_generate_cave(INITNUM);
  main_loop();                  			// main game loop in low memory, calls dungeon code

  return;
}


/* Init players with some belongings                    -RAK-   */
void __fastcall__ char_inven_init(void)
{
  register int8u i;
  register int16u j;
  inven_type inven_init;


  vbxe_bank(VBXE_OBJBANK);

  /* this is needed for bash to work right, it can't hurt anyway */
  for (i = 0; i < INVEN_ARRAY_SIZE; ++i)                                                                // max 34
    init_invcopy(&inventory[i], OBJ_NOTHING);

  for (i = 0; i < 5; ++i)
    {
      j = player_init[py.misc.pclass][i];                   // player_init is in INITBANK
      init_invcopy(&inven_init, j);                         // inven_init is on the stack

      /* this makes it known2 and known1 */
      //store_bought(&inven_init);                          // remove call to store_bought  SJ
      inven_init.ident |= ID_STOREBOUGHT;
      inven_init.ident |= ID_KNOWN2;

      /* must set this bit to display tohit/todam for stiletto */
      if (inven_init.tval == TV_SWORD)
		inven_init.ident |= ID_SHOW_HITDAM;
      init_inven_carry(&inven_init);
    }

  vbxe_bank(VBXE_SPLBANK);

  /* wierd place for it, but why not? */
  for (i = 0; i < 32; ++i)
    spell_order[i] = 99;
}


/* Initializes M_LEVEL array for use with PLACE_MONSTER -RAK-   */
void __fastcall__ init_m_level(void)
{
  register int16u i, k;


  vbxe_bank(VBXE_MONBANK);                              // Bank in Monster VBXE bank, no need to restore

  for (i = 0; i <= MAX_MONS_LEVEL; ++i)                 // MAX_MONS_LEVEL = 40
    m_level[i] = 0;

  k = MAX_CREATURES - WIN_MON_TOT;                      // 279 - 2 = 277
  for (i = 0; i < k; ++i)                               // 0 to 276
    m_level[c_list[i].level]++;                         // m_level is a count of all creatures of a certain level

  for (i = 1; i <= MAX_MONS_LEVEL; ++i)
    m_level[i] += m_level[i-1];                         // add the creatures below as well
}


/* Initializes T_LEVEL array for use with PLACE_OBJECT  -RAK-   */
void __fastcall__ init_t_level(void)
{
  register int16u i;
  register int8u l;
  int16u tmp[MAX_OBJ_LEVEL+1];                                  // 51 - 102 bytes


  vbxe_bank(VBXE_OBJBANK);                                      // bank in Object VBXE bank, no need to restore

  for (i = 0; i <= MAX_OBJ_LEVEL; ++i)							// 344 objects
    t_level[i] = 0;                                             // clear the list
  for (i = 0; i < MAX_DUNGEON_OBJ; ++i)
    t_level[object_list[i].level]++;                            // populate the number of objects of each level
  for (i = 1; i <= MAX_OBJ_LEVEL; ++i)
    t_level[i] += t_level[i-1];                                 // add the objects below

  /* now produce an array with object indexes sorted by level, by using
     the info in t_level, this is an O(n) sort! */
  /* this is not a stable sort, but that does not matter */
  for (i = 0; i <= MAX_OBJ_LEVEL; ++i)
    tmp[i] = 1;                                                 // clear
  for (i = 0; i < MAX_DUNGEON_OBJ; ++i) {                       // 344 objects
      l = object_list[i].level;                                 // l should only be 0-50
      sorted_objects[t_level[l] - tmp[l]] = i;
      tmp[l]++;
  }
}


/* Initializes the stores with owners                   -RAK-   */
void __fastcall__ store_init(void)
{
  register int8u i, j, k;
  register store_type *s_ptr;                                               // pointer to a location in the spell bank
  inven_type tmp_sitem;                                                     // temporary holder, so we can bank in the object list during invcopy


  vbxe_bank(VBXE_SPLBANK);                                                  // bank in spell VBXE bank where stores are, no need to restore here
  init_invcopy(&tmp_sitem, OBJ_NOTHING);                                    // Clear store inventory, restores vbxe bank - moved outside loop -SJ

  i = MAX_OWNERS / MAX_STORES;                                              // i=3 MAX_OWNERS = 18, MAX_STORES = 6
  for (j = 0; j < MAX_STORES; ++j)                                          // j=6
    {
      s_ptr = &store[j];                                                                                                                                // Store is in BSS segment -SJ
      s_ptr->owner = MAX_STORES*(randint(i)-1) + j;
      s_ptr->insult_cur = 0;
      s_ptr->store_open = 0;
      s_ptr->store_ctr  = 0;
      s_ptr->good_buy = 0;
      s_ptr->bad_buy = 0;
      for (k = 0; k < STORE_INVEN_MAX; ++k)                                     // STORE_INVEN_MAX = 24
	  {
	    memcpy(&s_ptr->store_inven[k].sitem, &tmp_sitem, sizeof(inven_type));	// copy the temp item (OBJ_NOTHING) to real location
	    s_ptr->store_inven[k].scost = 0;										// SPLBANK should be back in here
	  }
    }
}


/* Initialize all Potions, wands, staves, scrolls, etc. */
void __fastcall__ magic_init(void)
{
  register int8u i, j, h, k;                                    // was int SJ
  register char *tmp;
  vtype string;


  set_seed(randes_seed);
  vbxe_bank(VBXE_MAGBANK);                      // bank in magic bank

  /* The first 3 entries for colors are fixed, (slime & apple juice, water) */
  for (i = 3; i < MAX_COLORS; ++i) {
    j = randint(MAX_COLORS - 3) + 2;
    tmp = colors[i];
    colors[i] = colors[j];
    colors[j] = tmp;
  }
  for (i = 0; i < MAX_WOODS; ++i) {
    j = randint(MAX_WOODS) - 1;
    tmp = woods[i];
    woods[i] = woods[j];
    woods[j] = tmp;
  }
  for (i = 0; i < MAX_METALS; ++i) {
    j = randint(MAX_METALS) - 1;
    tmp = metals[i];
    metals[i] = metals[j];
    metals[j] = tmp;
  }
  for (i = 0; i < MAX_ROCKS; ++i) {
    j = randint(MAX_ROCKS) - 1;
    tmp = rocks[i];
    rocks[i] = rocks[j];
    rocks[j] = tmp;
  }
  for (i = 0; i < MAX_AMULETS; ++i) {
    j = randint(MAX_AMULETS) - 1;
    tmp = amulets[i];
    amulets[i] = amulets[j];
    amulets[j] = tmp;
  }
  for (i = 0; i < MAX_MUSH; ++i) {
    j = randint(MAX_MUSH) - 1;
    tmp = mushrooms[i];
    mushrooms[i] = mushrooms[j];
    mushrooms[j] = tmp;
  }
  for (h = 0; h < MAX_TITLES; ++h) {
    string[0] = '\0';
    k = randint(2) + 1;
    for (i = 0; i < k; ++i) {
	  for (j = randint(2); j > 0; --j)
	    strcat(string, syllables[randint(MAX_SYLLABLES) - 1]);
	  if (i < k-1)
	    strcat(string, " ");
	}
    if (string[8] == ' ')
	  string[8] = '\0';
    else
	  string[9] = '\0';
    strcpy(titles[h], string);
  }

  reset_seed();
}


/* gets a new random seed for the random number generator */
void __fastcall__ init_seeds(int16u seed)
{
  int16u clock_var;
  register int8u i;

  #define RANDOM        *(unsigned char *) 0xD20A


  if (seed == 0) {
    for(i=randint(100); i != 0; --i)					  // introduce some randomness to when we query the RANDOM register -SJ
      randint(i);										  // putting in a title screen with a press any key will help! -SJ
    clock_var = (RANDOM * 0x100L) + (RANDOM);             // get random seed from atari RANDOM -SJ
  }
  else
    clock_var = seed;

  randes_seed = (int16u) clock_var;

  clock_var += 8762;
  town_seed = (int16u) clock_var;

  clock_var += 13452;
  set_rnd_seed(clock_var);

  /* make it a little more random */
  for (i = randint(100); i != 0; --i)
    randint(clock_var);

  return;
}


/* Copied into init bank so we don't have to bank switch here.  -SJ */
/* Add an item to players inventory.  Return the        */
/* item position for a description if needed.          -RAK-   */
/* this code must be identical to the inven_check_num() code above */
void __fastcall__ init_inven_carry(inven_type *i_ptr)                                                                       // changed return type to int8u from int SJ
{
  register int8u locn, i;                                              // was int SJ
  register inven_type *t_ptr;
  register int8u typ, subt;
  int8u known1p, always_known1p;                                        // was int SJ


  typ = i_ptr->tval;
  subt = i_ptr->subval;
  known1p = init_known1_p(i_ptr);
  always_known1p = (init_object_offset(i_ptr) == -1);

  /* Now, check to see if player can carry object  */
  for (locn = 0; ; ++locn)
    {
      vbxe_bank(VBXE_OBJBANK);							// inventory is in objectbank
      t_ptr = &inventory[locn];

      if ((typ == t_ptr->tval) && (subt == t_ptr->subval) && (subt >= ITEM_SINGLE_STACK_MIN) &&
	  ((int)t_ptr->number + (int)i_ptr->number < 256) && ((subt < ITEM_GROUP_MIN) || (t_ptr->p1 == i_ptr->p1)) &&
	  /* only stack if both or neither are identified */
	  (known1p == init_known1_p(t_ptr)))
	  {
	    t_ptr->number += i_ptr->number;
	    break;
	  }
      /* For items which are always known1p, i.e. never have a 'color', insert them into the inventory in sorted order.  */
      else if ((typ == t_ptr->tval && subt < t_ptr->subval && always_known1p) || (typ > t_ptr->tval))
	  {
	    if (inven_ctr - 1 != 0) {
	      for (i = inven_ctr - 1; i >= locn; --i) {
		     memcpy(&inventory[i+1], &inventory[i], sizeof(inven_type));             // copy up one so we can insert
			 if (i == 0) break;                                                  // break out of the loop if last slot! -SJ
	      }
	    }
	    memcpy(&inventory[locn], i_ptr, sizeof(inven_type));        // insert into inventory
	    ++inven_ctr;
	    break;
	  }
    }

  inven_weight += (i_ptr->number*i_ptr->weight);				// move inven_weight into objbank?
  py.flags.status |= PY_STR_WGT;

  vbxe_restore_bank();
  return;
}



