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


#pragma code-name("MAGICT1BANK")
#pragma rodata-name("MAGICT1BANK")
#pragma data-name("MAGICT1BANK")


/* Local Prototypes */
int8u __fastcall__ mt1_magik(int8u chance);
int8u __fastcall__ mt1_bonus(int16u base, int16u max_std, int8u level);
int16u __fastcall__ magict1_randnor(int16u mean, int16u stand);
void __fastcall__ magic_treasure_1(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		MAGICT1NUM


int16u __fastcall__ magict1_randnor(int16u mean, int16u stand)
#include "inbank\randnor.c"



/* Chance of treasure having magic abilities		-RAK-	*/
/* Chance increases with each dungeon level			 */
void __fastcall__ magic_treasure(int8u x, int8u level)
{
  register inven_type *t_ptr;
  register int8u chance, special, cursed;


  vbxe_bank(VBXE_OBJBANK);

  chance = OBJ_BASE_MAGIC + level;
  if (chance > OBJ_BASE_MAX)
    chance = OBJ_BASE_MAX;
  special = chance / OBJ_DIV_SPECIAL;
  cursed  = (10 * chance) / OBJ_DIV_CURSED;					// CHECK: intermediate calculation can be int?
  t_ptr = &t_list[x];


  magic_treasure_1(t_ptr, level, special, chance, cursed);
  bank_magic_treasure_2(t_ptr, level, special, chance, cursed);
}


/* Chance of treasure having magic abilities		-RAK-	*/
/* Chance increases with each dungeon level			        */
/* Split the function in half, due to code size -SJ         */
void __fastcall__ magic_treasure_1(inven_type *t_ptr, int8u level, int8u special, int8u chance, int8u cursed)
{
  register int16u tmp;


  /* some objects appear multiple times in the object_list with different
     levels, this is to make the object occur more often, however, for
     consistency, must set the level of these duplicates to be the same
     as the object with the lowest level */

  /* Depending on treasure type, it can have certain magical properties*/
  switch (t_ptr->tval)
    {
    case TV_SHIELD: case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
      if (mt1_magik(chance))
	  {
	    t_ptr->toac += mt1_bonus(1, 30, level);
	    if (mt1_magik(special))
	      switch(randint(9))
	      {
	        case 1:
		      t_ptr->flags |= (TR_RES_LIGHT|TR_RES_COLD|TR_RES_ACID|TR_RES_FIRE);
		      t_ptr->name2 = SN_R;
		      t_ptr->toac += 5;
		      t_ptr->cost += 2500;
		      break;
	        case 2:	 /* Resist Acid	  */
		      t_ptr->flags |= TR_RES_ACID;
		      t_ptr->name2 = SN_RA;
		      t_ptr->cost += 1000;
		      break;
	        case 3: case 4:	 /* Resist Fire	  */
		      t_ptr->flags |= TR_RES_FIRE;
		      t_ptr->name2 = SN_RF;
		      t_ptr->cost += 600;
		      break;
	        case 5: case 6:	/* Resist Cold	 */
		      t_ptr->flags |= TR_RES_COLD;
		      t_ptr->name2 = SN_RC;
		      t_ptr->cost += 600;
		      break;
	        case 7: case 8: case 9:  /* Resist Lightning*/
		      t_ptr->flags |= TR_RES_LIGHT;
		      t_ptr->name2 = SN_RL;
		      t_ptr->cost += 500;
		      break;
	      }
	  }
      else if (mt1_magik(cursed))
	  {
	    t_ptr->toac -= mt1_bonus(1, 40, level);
	    t_ptr->cost = 0;
	    t_ptr->flags |= TR_CURSED;
	  }
      break;

    case TV_HAFTED: case TV_POLEARM: case TV_SWORD:
      /* always show tohit/todam values if identified */
      t_ptr->ident |= ID_SHOW_HITDAM;
      if (mt1_magik(chance))
	  {
	    t_ptr->tohit += mt1_bonus(0, 40, level);
	    /* Magical damage bonus now proportional to weapon base damage */
	    tmp = t_ptr->damage[0] * t_ptr->damage[1];
	    t_ptr->todam += mt1_bonus(0, 4*tmp, tmp*level/10);
	    /* the 3*special/2 is needed because weapons are not as common as
	    before change to treasure distribution, this helps keep same
	    number of ego weapons same as before, see also missiles */
	    if (mt1_magik(3*special/2))
	      switch(randint(16))
	      {
	        case 1:	/* Holy Avenger	 */
		      t_ptr->flags |= (TR_SEE_INVIS|TR_SUST_STAT|TR_SLAY_UNDEAD|TR_SLAY_EVIL|TR_STR);
		      t_ptr->tohit += 5;
		      t_ptr->todam += 5;
		      t_ptr->toac  += randint(4);
		      /* the value in p1 is used for strength increase */
		      /* p1 is also used for sustain stat */
		      t_ptr->p1    = randint(4);
		      t_ptr->name2 = SN_HA;
		      t_ptr->cost += t_ptr->p1*500;
		      t_ptr->cost += 10000;
		      break;
	        case 2:	/* Defender	 */
		      t_ptr->flags |= (TR_FFALL|TR_RES_LIGHT|TR_SEE_INVIS|TR_FREE_ACT|TR_RES_COLD|TR_RES_ACID|TR_RES_FIRE|TR_REGEN|TR_STEALTH);
		      t_ptr->tohit += 3;
		      t_ptr->todam += 3;
		      t_ptr->toac  += 5 + randint(5);
		      t_ptr->name2 = SN_DF;
		      /* the value in p1 is used for stealth */
		      t_ptr->p1    = randint(3);
		      t_ptr->cost += t_ptr->p1*500;
		      t_ptr->cost += 7500;
		      break;
	        case 3: case 4:	 /* Slay Animal  */
		      t_ptr->flags |= TR_SLAY_ANIMAL;
		      t_ptr->tohit += 3;
		      t_ptr->todam += 3;
		      t_ptr->name2 = SN_SA;
		      t_ptr->cost += 5000;
		      break;
	        case 5: case 6:	/* Slay Dragon	 */
		      t_ptr->flags |= TR_SLAY_DRAGON;
		      t_ptr->tohit += 3;
		      t_ptr->todam += 3;
		      t_ptr->name2 = SN_SD;
		      t_ptr->cost += 4000;
		      break;
	        case 7: case 8:	  /* Slay Evil	   */
		      t_ptr->flags |= TR_SLAY_EVIL;
		      t_ptr->tohit += 3;
		      t_ptr->todam += 3;
		      t_ptr->name2 = SN_SE;
		      t_ptr->cost += 4000;
		      break;
	        case 9: case 10:	 /* Slay Undead	  */
		      t_ptr->flags |= (TR_SEE_INVIS|TR_SLAY_UNDEAD);
		      t_ptr->tohit += 2;
		      t_ptr->todam += 2;
		      t_ptr->name2 = SN_SU;
		      t_ptr->cost += 3000;
		      break;
	        case 11: case 12: case 13:   /* Flame Tongue  */
		      t_ptr->flags |= TR_FLAME_TONGUE;
		      t_ptr->tohit++;
		      t_ptr->todam += 3;
		      t_ptr->name2 = SN_FT;
		      t_ptr->cost += 2000;
		      break;
	        case 14: case 15: case 16:   /* Frost Brand   */
		      t_ptr->flags |= TR_FROST_BRAND;
		      t_ptr->tohit++;
		      t_ptr->todam++;
		      t_ptr->name2 = SN_FB;
		      t_ptr->cost += 1200;
		      break;
	       }
	  }
      else if (mt1_magik(cursed))
	  {
	    t_ptr->tohit -= mt1_bonus(1, 55, level);
	    /* Magical damage bonus now proportional to weapon base damage */
	    tmp = t_ptr->damage[0] * t_ptr->damage[1];
	    t_ptr->todam -= mt1_bonus(1, 11*tmp/2, tmp*level/10);
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	  }
      break;

    case TV_BOW:
      /* always show tohit/todam values if identified */
      t_ptr->ident |= ID_SHOW_HITDAM;
      if (mt1_magik(chance))
	  {
	    t_ptr->tohit += mt1_bonus(1, 30, level);
	    t_ptr->todam += mt1_bonus(1, 20, level); /* add damage. -CJS- */
	  }
      else if (mt1_magik(cursed))
	  {
	    t_ptr->tohit -= mt1_bonus(1, 50, level);
	    t_ptr->todam -= mt1_bonus(1, 30, level); /* add damage. -CJS- */
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	  }
      break;

    case TV_DIGGING:
      /* always show tohit/todam values if identified */
      t_ptr->ident |= ID_SHOW_HITDAM;
      if (mt1_magik(chance))
	  {
	    tmp = randint(3);
	    if (tmp < 3)
	      t_ptr->p1 += mt1_bonus(0, 25, level);
	    else
	    {
	      /* a cursed digging tool */
	      t_ptr->p1 = -mt1_bonus(1, 30, level);
	      t_ptr->cost = 0;
	      t_ptr->flags |= TR_CURSED;
	    }
	  }
      break;

    case TV_GLOVES:
      if (mt1_magik(chance))
	  {
	    t_ptr->toac += mt1_bonus(1, 20, level);
	    if (mt1_magik(special))
	    {
	      if (randint(2) == 1)
		  {
		    t_ptr->flags |= TR_FREE_ACT;
		    t_ptr->name2 = SN_FREE_ACTION;
		    t_ptr->cost += 1000;
		  }
	      else
		  {
		    t_ptr->ident |= ID_SHOW_HITDAM;
		    t_ptr->tohit += 1 + randint(3);
		    t_ptr->todam += 1 + randint(3);
		    t_ptr->name2 = SN_SLAYING;
		    t_ptr->cost += (t_ptr->tohit+t_ptr->todam)*250;
		  }
	    }
	  }
      else if (mt1_magik(cursed))
	  {
	    if (mt1_magik(special))
	    {
	      if (randint(2) == 1)
		  {
		    t_ptr->flags |= TR_DEX;
		    t_ptr->name2 = SN_CLUMSINESS;
		  }
	      else
		  {
		    t_ptr->flags |= TR_STR;
		    t_ptr->name2 = SN_WEAKNESS;
		  }
	      t_ptr->ident |= ID_SHOW_P1;
	      t_ptr->p1   = -mt1_bonus(1, 10, level);
	    }
	    t_ptr->toac -= mt1_bonus(1, 40, level);
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	  }
      break;

    case TV_BOOTS:
      if (mt1_magik(chance))
	  {
	    t_ptr->toac += mt1_bonus(1, 20, level);
	    if (mt1_magik(special))
	    {
	      tmp = randint(12);
	      if (tmp > 5)
		  {
		    t_ptr->flags |= TR_FFALL;
		    t_ptr->name2 = SN_SLOW_DESCENT;
		    t_ptr->cost += 250;
		  }
	      else if (tmp == 1)
		  {
		    t_ptr->flags |= TR_SPEED;
		    t_ptr->name2 = SN_SPEED;
		    t_ptr->ident |= ID_SHOW_P1;
		    t_ptr->p1 = 1;
		    t_ptr->cost += 5000;
		  }
	      else /* 2 - 5 */
		  {
		    t_ptr->flags |= TR_STEALTH;
		    t_ptr->ident |= ID_SHOW_P1;
		    t_ptr->p1 = randint(3);
		    t_ptr->name2 = SN_STEALTH;
		    t_ptr->cost += 500;
		  }
	    }
	  }
      else if (mt1_magik(cursed))
	  {
	    tmp = randint(3);
	    if (tmp == 1)
	    {
	      t_ptr->flags |= TR_SPEED;
	      t_ptr->name2 = SN_SLOWNESS;
	      t_ptr->ident |= ID_SHOW_P1;
	      t_ptr->p1 = -1;
	    }
	    else if (tmp == 2)
	    {
	      t_ptr->flags |= TR_AGGRAVATE;
	      t_ptr->name2 = SN_NOISE;
	    }
	    else
	    {
	      t_ptr->name2 = SN_GREAT_MASS;
	      t_ptr->weight = t_ptr->weight * 5;
	    }
	    t_ptr->cost = 0;
	    t_ptr->toac -= mt1_bonus(2, 45, level);
	    t_ptr->flags |= TR_CURSED;
	  }
      break;

    case TV_HELM:  /* Helms */
      if ((t_ptr->subval >= 6) && (t_ptr->subval <= 8))
	  {
	    /* give crowns a higher chance for magic */
	    chance += (int) (t_ptr->cost / 100);
	    special += special;
	  }
      if (mt1_magik(chance))
	  {
	    t_ptr->toac += mt1_bonus(1, 20, level);
	    if (mt1_magik(special))
	    {
	      if (t_ptr->subval < 6)
		  {
		    tmp = randint(3);
		    t_ptr->ident |= ID_SHOW_P1;
		    if (tmp == 1)
		    {
		      t_ptr->p1 = randint(2);
		      t_ptr->flags |= TR_INT;
		      t_ptr->name2 = SN_INTELLIGENCE;
		      t_ptr->cost += t_ptr->p1*500;
		    }
		    else if (tmp == 2)
		    {
		      t_ptr->p1 = randint(2);
		      t_ptr->flags |= TR_WIS;
		      t_ptr->name2 = SN_WISDOM;
		      t_ptr->cost += t_ptr->p1*500;
		    }
		    else
		    {
		      t_ptr->p1 = 1 + randint(4);
		      t_ptr->flags |= TR_INFRA;
		      t_ptr->name2 = SN_INFRAVISION;
		      t_ptr->cost += t_ptr->p1*250;
		    }
		  }
	      else
		  {
		    switch(randint(6))
		    {
		      case 1:
		        t_ptr->ident |= ID_SHOW_P1;
		        t_ptr->p1 = randint(3);
		        t_ptr->flags |= (TR_FREE_ACT|TR_CON|TR_DEX|TR_STR);
		        t_ptr->name2 = SN_MIGHT;
		        t_ptr->cost += 1000 + t_ptr->p1*500;
		        break;
		      case 2:
		        t_ptr->ident |= ID_SHOW_P1;
		        t_ptr->p1 = randint(3);
		        t_ptr->flags |= (TR_CHR|TR_WIS);
		        t_ptr->name2 = SN_LORDLINESS;
		        t_ptr->cost += 1000 + t_ptr->p1*500;
		        break;
		      case 3:
		        t_ptr->ident |= ID_SHOW_P1;
		        t_ptr->p1 = randint(3);
		        t_ptr->flags |= (TR_RES_LIGHT|TR_RES_COLD|TR_RES_ACID|TR_RES_FIRE|TR_INT);
		        t_ptr->name2 = SN_MAGI;
		        t_ptr->cost += 3000 + t_ptr->p1*500;
		        break;
		      case 4:
		        t_ptr->ident |= ID_SHOW_P1;
		        t_ptr->p1 = randint(3);
		        t_ptr->flags |= TR_CHR;
		        t_ptr->name2 = SN_BEAUTY;
		        t_ptr->cost += 750;
		        break;
		      case 5:
		        t_ptr->ident |= ID_SHOW_P1;
		        t_ptr->p1 = 5*(1 + randint(4));
  		        t_ptr->flags |= (TR_SEE_INVIS|TR_SEARCH);
    		    t_ptr->name2 = SN_SEEING;
		        t_ptr->cost += 1000 + t_ptr->p1*100;
		        break;
		      case 6:
		        t_ptr->flags |= TR_REGEN;
		        t_ptr->name2 = SN_REGENERATION;
		        t_ptr->cost += 1500;
		        break;
		    }
		  }
	    }
	  }
      else if (mt1_magik(cursed))
	  {
	    t_ptr->toac -= mt1_bonus(1, 45, level);
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	    if (mt1_magik(special))
	      switch(randint(7))
	      {
	        case 1:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = -randint (5);
		      t_ptr->flags |= TR_INT;
		      t_ptr->name2 = SN_STUPIDITY;
		      break;
	        case 2:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = -randint (5);
		      t_ptr->flags |= TR_WIS;
		      t_ptr->name2 = SN_DULLNESS;
		      break;
	        case 3:
		      t_ptr->flags |= TR_BLIND;
		      t_ptr->name2 = SN_BLINDNESS;
		      break;
	        case 4:
		      t_ptr->flags |= TR_TIMID;
		      t_ptr->name2 = SN_TIMIDNESS;
		      break;
	        case 5:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = -randint (5);
		      t_ptr->flags |= TR_STR;
		      t_ptr->name2 = SN_WEAKNESS;
		      break;
	        case 6:
		      t_ptr->flags |= TR_TELEPORT;
		      t_ptr->name2 = SN_TELEPORTATION;
		      break;
	        case 7:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = -randint (5);
		      t_ptr->flags |= TR_CHR;
		      t_ptr->name2 = SN_UGLINESS;
		      break;
	      }
	  }
      break;

    default:
      break;
  }
}




/* Enchant a bonus based on degree desired -RAK- */
int8u __fastcall__ mt1_bonus(int16u base, int16u max_std, int8u level)
{
  register int16u x, stand_dev, tmp;


  stand_dev = (OBJ_STD_ADJ * level / 100) + OBJ_STD_MIN;
  /* Check for level > max_std since that may have generated an overflow.  */
  if (stand_dev > max_std || level > max_std)
    stand_dev = max_std;
  /* abs may be a macro, don't call it with randnor as a parameter */
  tmp = magict1_randnor(0, stand_dev);
  //x = (abs(tmp) / 10) + base;				// CHECK: don't need abs, as can't return a negative number
  x = (tmp / 10) + base;     				// CHECK: don't need abs, as can't return a negative number
  if (x < base)
    return(base);
  else
    return(x);
}


/* Boolean : is object enchanted	  -RAK- */
int8u __fastcall__ mt1_magik(int8u chance)
{
  if (randint(100) <= chance)
    return(TRUE);
  else
    return(FALSE);
}


