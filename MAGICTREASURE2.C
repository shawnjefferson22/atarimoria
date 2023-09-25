/* source/generate.c: initialize/create a dungeon or town level

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include <string.h>


#pragma code-name("MAGICT2BANK")
#pragma rodata-name("MAGICT2BANK")
#pragma data-name("MAGICT2BANK")


/* Local Prototypes */
int8u __fastcall__ mt2_magik(int8u chance);
int8u __fastcall__ mt2_bonus(int16u base, int16u max_std, int8u level);
int16u __fastcall__ magict2_randnor(int16u mean, int16u stand);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MAGICT2NUM


int16u __fastcall__ magict2_randnor(int16u mean, int16u stand)
#include "inbank\randnor.c"


/* Chance of treasure having magic abilities		-RAK-	*/
/* Chance increases with each dungeon level			 */
void __fastcall__ magic_treasure_2(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed)
{
  register int i;
  int16u tmp;


  /* some objects appear multiple times in the object_list with different
     levels, this is to make the object occur more often, however, for
     consistency, must set the level of these duplicates to be the same
     as the object with the lowest level */

  /* Depending on treasure type, it can have certain magical properties*/
  switch (t_ptr->tval)
    {
    case TV_RING: /* Rings	      */
      switch(t_ptr->subval)
	  {
	    case 0: case 1: case 2: case 3:
	      if (mt2_magik(cursed))
	      {
	        t_ptr->p1 = -mt2_bonus(1, 20, level);
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      else
	      {
	        t_ptr->p1 = mt2_bonus(1, 10, level);
	        t_ptr->cost += t_ptr->p1*100;
	      }
	      break;
	    case 4:
	      if (mt2_magik(cursed))
	      {
	        t_ptr->p1 = -randint(3);
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      else
	       t_ptr->p1 = 1;
	      break;
	    case 5:
	      t_ptr->p1 = 5 * mt2_bonus(1, 20, level);
	      t_ptr->cost += t_ptr->p1*50;
	      if (mt2_magik (cursed))
	      {
	        t_ptr->p1 = -t_ptr->p1;
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      break;
	    case 19:     /* Increase damage	      */
	      t_ptr->todam += mt2_bonus(1, 20, level);
	      t_ptr->cost += t_ptr->todam*100;
	      if (mt2_magik(cursed))
	      {
	        t_ptr->todam = -t_ptr->todam;
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      break;
	    case 20:     /* Increase To-Hit	      */
	      t_ptr->tohit += mt2_bonus(1, 20, level);
	      t_ptr->cost += t_ptr->tohit*100;
	      if (mt2_magik(cursed))
	      {
	        t_ptr->tohit = -t_ptr->tohit;
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      break;
	    case 21:     /* Protection	      */
	      t_ptr->toac += mt2_bonus(1, 20, level);
	      t_ptr->cost += t_ptr->toac*100;
	      if (mt2_magik(cursed))
	      {
	        t_ptr->toac = -t_ptr->toac;
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      break;
	    case 24: case 25: case 26:
	    case 27: case 28: case 29:
	      t_ptr->ident |= ID_NOSHOW_P1;
	      break;
	    case 30:     /* Slaying	      */
	      t_ptr->ident |= ID_SHOW_HITDAM;
	      t_ptr->todam += mt2_bonus(1, 25, level);
	      t_ptr->tohit += mt2_bonus(1, 25, level);
	      t_ptr->cost += (t_ptr->tohit+t_ptr->todam)*100;
	      if (mt2_magik(cursed))
	      {
	        t_ptr->tohit = -t_ptr->tohit;
	        t_ptr->todam = -t_ptr->todam;
	        t_ptr->flags |= TR_CURSED;
	        t_ptr->cost = -t_ptr->cost;
	      }
	      break;
	    default:
	      break;
	  }
      break;

    case TV_AMULET: /* Amulets	      */
      if (t_ptr->subval < 2)
	  {
	  if (mt2_magik(cursed))
	  {
	      t_ptr->p1 = -mt2_bonus(1, 20, level);
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	    else
	    {
	      t_ptr->p1 = mt2_bonus(1, 10, level);
	      t_ptr->cost += t_ptr->p1*100;
	    }
	  }
      else if (t_ptr->subval == 2)
	  {
	    t_ptr->p1 = 5 * mt2_bonus(1, 25, level);
	    if (mt2_magik(cursed))
	    {
	      t_ptr->p1 = -t_ptr->p1;
	      t_ptr->cost = -t_ptr->cost;
	      t_ptr->flags |= TR_CURSED;
	    }
	    else
	      t_ptr->cost += 50*t_ptr->p1;
	  }
      else if (t_ptr->subval == 8)
	  {
	    /* amulet of the magi is never cursed */
	    t_ptr->p1 = 5 * mt2_bonus(1, 25, level);
	    t_ptr->cost += 20*t_ptr->p1;
	  }
      break;

      /* Subval should be even for store, odd for dungeon*/
      /* Dungeon found ones will be partially charged	 */
    case TV_LIGHT:
      if ((t_ptr->subval % 2) == 1)
	  {
	    t_ptr->p1 = randint(t_ptr->p1);
	    t_ptr->subval -= 1;
	  }
      break;

    case TV_WAND:
      switch(t_ptr->subval)
	  {
	    case 0:	  t_ptr->p1 = randint(10) +	 6; break;
	    case 1:	  t_ptr->p1 = randint(8)  +	 6; break;
	    case 2:	  t_ptr->p1 = randint(5)  +	 6; break;
	    case 3:	  t_ptr->p1 = randint(8)  +	 6; break;
	    case 4:	  t_ptr->p1 = randint(4)  +	 3; break;
	    case 5:	  t_ptr->p1 = randint(8)  +	 6; break;
	    case 6:	  t_ptr->p1 = randint(20) +	 12; break;
	    case 7:	  t_ptr->p1 = randint(20) +	 12; break;
	    case 8:	  t_ptr->p1 = randint(10) +	 6; break;
	    case 9:	  t_ptr->p1 = randint(12) +	 6; break;
	    case 10:  t_ptr->p1 = randint(10) +	 12; break;
	    case 11:  t_ptr->p1 = randint(3)  +	 3; break;
	    case 12:  t_ptr->p1 = randint(8)  +	 6; break;
	    case 13:  t_ptr->p1 = randint(10) +	 6; break;
	    case 14:  t_ptr->p1 = randint(5)  +	 3; break;
	    case 15:  t_ptr->p1 = randint(5)  +	 3; break;
	    case 16:  t_ptr->p1 = randint(5)  +	 6; break;
	    case 17:  t_ptr->p1 = randint(5)  +	 4; break;
	    case 18:  t_ptr->p1 = randint(8)  +	 4; break;
	    case 19:  t_ptr->p1 = randint(6)  +	 2; break;
	    case 20:  t_ptr->p1 = randint(4)  +	 2; break;
	    case 21:  t_ptr->p1 = randint(8)  +	 6; break;
	    case 22:  t_ptr->p1 = randint(5)  +	 2; break;
	    case 23:  t_ptr->p1 = randint(12) + 12; break;
	    default:
	     break;
	  }
      break;

    case TV_STAFF:
      switch(t_ptr->subval)
	  {
	    case 0:	  t_ptr->p1 = randint(20) +	 12; break;
	    case 1:	  t_ptr->p1 = randint(8)  +	 6; break;
	    case 2:	  t_ptr->p1 = randint(5)  +	 6; break;
	    case 3:	  t_ptr->p1 = randint(20) +	 12; break;
	    case 4:	  t_ptr->p1 = randint(15) +	 6; break;
	    case 5:	  t_ptr->p1 = randint(4)  +	 5; break;
	    case 6:	  t_ptr->p1 = randint(5)  +	 3; break;
	    case 7:	  t_ptr->p1 = randint(3)  +	 1;
	      t_ptr->level = 10;
	      break;
	    case 8:	  t_ptr->p1 = randint(3)  +	 1; break;
	    case 9:	  t_ptr->p1 = randint(5)  +	 6; break;
	    case 10:  t_ptr->p1 = randint(10) +	 12; break;
	    case 11:  t_ptr->p1 = randint(5)  +	 6; break;
	    case 12:  t_ptr->p1 = randint(5)  +	 6; break;
	    case 13:  t_ptr->p1 = randint(5)  +	 6; break;
	    case 14:  t_ptr->p1 = randint(10) +	 12; break;
	    case 15:  t_ptr->p1 = randint(3)  +	 4; break;
	    case 16:  t_ptr->p1 = randint(5)  +	 6; break;
	    case 17:  t_ptr->p1 = randint(5)  +	 6; break;
	    case 18:  t_ptr->p1 = randint(3)  +	 4; break;
	    case 19:  t_ptr->p1 = randint(10) +	 12; break;
	    case 20:  t_ptr->p1 = randint(3)  +	 4; break;
	    case 21:  t_ptr->p1 = randint(3)  +	 4; break;
	    case 22:  t_ptr->p1 = randint(10) + 6;
	      t_ptr->level = 5;
	      break;
	    default:
	      break;
	  }
      break;

    case TV_CLOAK:
      if (mt2_magik(chance))
	  {
	    if (mt2_magik(special))
	    {
	      if (randint(2) == 1)
		  {
		    t_ptr->name2 = SN_PROTECTION;
		    t_ptr->toac += mt2_bonus(2, 40, level);
		    t_ptr->cost += 250;
		  }
	      else
		  {
		    t_ptr->toac += mt2_bonus(1, 20, level);
		    t_ptr->ident |= ID_SHOW_P1;
		    t_ptr->p1 = randint(3);
		    t_ptr->flags |= TR_STEALTH;
		    t_ptr->name2 = SN_STEALTH;
		    t_ptr->cost += 500;
		  }
	    }
	    else
	      t_ptr->toac += mt2_bonus(1, 20, level);
	  }
      else if (mt2_magik(cursed))
	  {
	    tmp = randint(3);
	    if (tmp == 1)
	    {
	      t_ptr->flags |= TR_AGGRAVATE;
	      t_ptr->name2 = SN_IRRITATION;
	      t_ptr->toac  -= mt2_bonus(1, 10, level);
	      t_ptr->ident |= ID_SHOW_HITDAM;
	      t_ptr->tohit -= mt2_bonus(1, 10, level);
	      t_ptr->todam -= mt2_bonus(1, 10, level);
	      t_ptr->cost =  0;
	    }
	    else if (tmp == 2)
	    {
	      t_ptr->name2 = SN_VULNERABILITY;
	      t_ptr->toac -= mt2_bonus(10, 100, level+50);
	      t_ptr->cost = 0;
	    }
	    else
	    {
	      t_ptr->name2 = SN_ENVELOPING;
	      t_ptr->toac  -= mt2_bonus(1, 10, level);
	      t_ptr->ident |= ID_SHOW_HITDAM;
	      t_ptr->tohit -= mt2_bonus(2, 40, level+10);
	      t_ptr->todam -= mt2_bonus(2, 40, level+10);
	      t_ptr->cost = 0;
	    }
	    t_ptr->flags |= TR_CURSED;
	  }
      break;

    case TV_CHEST:
      switch(randint(level+4))
	  {
	    case 1:
	      t_ptr->flags = 0;
	      t_ptr->name2 = SN_EMPTY;
	      break;
	    case 2:
	      t_ptr->flags |= CH_LOCKED;
	      t_ptr->name2 = SN_LOCKED;
	      break;
	    case 3: case 4:
	      t_ptr->flags |= (CH_LOSE_STR|CH_LOCKED);
	      t_ptr->name2 = SN_POISON_NEEDLE;
	      break;
	    case 5: case 6:
	      t_ptr->flags |= (CH_POISON|CH_LOCKED);
	      t_ptr->name2 = SN_POISON_NEEDLE;
	      break;
	    case 7: case 8: case 9:
	      t_ptr->flags |= (CH_PARALYSED|CH_LOCKED);
	      t_ptr->name2 = SN_GAS_TRAP;
	      break;
	    case 10: case 11:
	      t_ptr->flags |= (CH_EXPLODE|CH_LOCKED);
	      t_ptr->name2 = SN_EXPLOSION_DEVICE;
	      break;
	    case 12: case 13: case 14:
	      t_ptr->flags |= (CH_SUMMON|CH_LOCKED);
	      t_ptr->name2 = SN_SUMMONING_RUNES;
	      break;
	    case 15: case 16: case 17:
	      t_ptr->flags |= (CH_PARALYSED|CH_POISON|CH_LOSE_STR|CH_LOCKED);
	      t_ptr->name2 = SN_MULTIPLE_TRAPS;
	      break;
	    default:
	      t_ptr->flags |= (CH_SUMMON|CH_EXPLODE|CH_LOCKED);
	      t_ptr->name2 = SN_MULTIPLE_TRAPS;
	      break;
	  }
      break;

    case TV_SLING_AMMO: case TV_SPIKE:
    case TV_BOLT: case TV_ARROW:
      if (t_ptr->tval == TV_SLING_AMMO || t_ptr->tval == TV_BOLT || t_ptr->tval == TV_ARROW)
	  {
	    /* always show tohit/todam values if identified */
	    t_ptr->ident |= ID_SHOW_HITDAM;

	    if (mt2_magik(chance))
	    {
	      t_ptr->tohit += mt2_bonus(1, 35, level);
	      t_ptr->todam += mt2_bonus(1, 35, level);
	      /* see comment for weapons */
	      if (mt2_magik(3*special/2))
		    switch(randint(10))
		    {
		      case 1: case 2: case 3:
		        t_ptr->name2 = SN_SLAYING;
		        t_ptr->tohit += 5;
		        t_ptr->todam += 5;
		        t_ptr->cost += 20;
		        break;
		      case 4: case 5:
		        t_ptr->flags |= TR_FLAME_TONGUE;
		        t_ptr->tohit += 2;
		        t_ptr->todam += 4;
		        t_ptr->name2 = SN_FIRE;
		        t_ptr->cost += 25;
		        break;
		      case 6: case 7:
		        t_ptr->flags |= TR_SLAY_EVIL;
		        t_ptr->tohit += 3;
		        t_ptr->todam += 3;
		        t_ptr->name2 = SN_SLAY_EVIL;
		        t_ptr->cost += 25;
		        break;
		      case 8: case 9:
		        t_ptr->flags |= TR_SLAY_ANIMAL;
		        t_ptr->tohit += 2;
		        t_ptr->todam += 2;
		        t_ptr->name2 = SN_SLAY_ANIMAL;
		        t_ptr->cost += 30;
		        break;
		      case 10:
		        t_ptr->flags |= TR_SLAY_DRAGON;
		        t_ptr->tohit += 3;
		        t_ptr->todam += 3;
		        t_ptr->name2 = SN_DRAGON_SLAYING;
		        t_ptr->cost += 35;
		        break;
		    }
	    }
	    else if (mt2_magik(cursed))
	    {
	      t_ptr->tohit -= mt2_bonus(5, 55, level);
	      t_ptr->todam -= mt2_bonus(5, 55, level);
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = 0;
	    }
	  }

      t_ptr->number = 0;
      for (i = 0; i < 7; ++i)
	    t_ptr->number += randint(6);
      if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;						// CHECK: missile_ctr negative?
      else
	    ++missile_ctr;
      t_ptr->p1 = missile_ctr;
      break;

    case TV_FOOD:
      /* make sure all food rations have the same level */
      if (t_ptr->subval == 90)
	    t_ptr->level = 0;
      /* give all elvish waybread the same level */
      else if (t_ptr->subval == 92)
	    t_ptr->level = 6;
      break;

    case TV_SCROLL1:
      /* give all identify scrolls the same level */
      if (t_ptr->subval == 67)
	    t_ptr->level = 1;
      /* scroll of light */
      else if (t_ptr->subval == 69)
	    t_ptr->level = 0;
      /* scroll of trap detection */
      else if (t_ptr->subval == 80)
	    t_ptr->level = 5;
      /* scroll of door/stair location */
      else if (t_ptr->subval == 81)
	    t_ptr->level = 5;
      break;

    case TV_POTION1:  /* potions */
      /* cure light */
      if (t_ptr->subval == 76)
	    t_ptr->level = 0;
      break;

      default:
        break;
  }
}


/* Enchant a bonus based on degree desired -RAK- */
int8u __fastcall__ mt2_bonus(int16u base, int16u max_std, int8u level)
{
  register int16u x, stand_dev, tmp;


  stand_dev = (OBJ_STD_ADJ * level / 100) + OBJ_STD_MIN;
  /* Check for level > max_std since that may have generated an overflow.  */
  if (stand_dev > max_std || level > max_std)
    stand_dev = max_std;
  /* abs may be a macro, don't call it with randnor as a parameter */
  tmp = magict2_randnor(0, stand_dev);
  //x = (abs(tmp) / 10) + base;				// CHECK: don't need abs, as can't return a negative number
  x = (tmp / 10) + base;     				// CHECK: don't need abs, as can't return a negative number
  if (x < base)
    return(base);
  else
    return(x);
}


/* Boolean : is object enchanted	  -RAK- */
int8u __fastcall__ mt2_magik(int8u chance)
{
  if (randint(100) <= chance)
    return(TRUE);
  else
    return(FALSE);
}


