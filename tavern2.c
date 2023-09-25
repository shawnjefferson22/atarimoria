/* source/tavern2.c: tavern code, gossip

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
#include "vbxetext.h"


extern char text[5 * 76];				// RAM for text to be printed, 5 lines, 384 bytes
extern int8u tavern_got_gossip;			// already received gossip for today?


#pragma code-name("TAVERN2BANK")
#pragma rodata-name("TAVERN2BANK")
#pragma data-name("TAVERN2BANK")


const char *gossip_start[5] = {
"If you are going to venture into the mines, you may want to know that the ",
"You seem like a good sort. Maybe this information will be useful. The ",
"I doubt this information will keep you from dying in the mines, but the ",
"Psst! A friend told me that the ",
"My father was a great adventurer and braved the mines many times. He told me, that the "
};

const char *patron_desc[16] = {
"A rotund Dwarf",
"A beautiful Elf",
"An ugly Orc",
"A non-descript Human",
"A small Halfling",
"A sly Gnome",
"A huge Warrior",
"A slim Elf",
"A dark Half-Orc",
"A seedy Human",
"A rough Half-Troll",
"A wise Mage",
"A drunk Priest",
"A nervous Rogue",
"A scruffy Ranger",
"A haughty Paladin"
};

const char *tavern_desc_howmuch[] = {
  " not at all",
  " a bit",
  "",
  " quite",
  " very",
  " most",
  " highly",
  " extremely" };

const char *tavern_desc_spell[] = {
  "teleport short distances",
  "teleport long distances",
  "teleport its prey",
  "cause light wounds",
  "cause serious wounds",
  "paralyse its prey",
  "induce blindness",
  "confuse",
  "terrify",
  "summon a monster",
  "summon the undead",
  "slow its prey",
  "drain mana",
  "unknown 1",
  "unknown 2" };

const char *tavern_desc_breath[] = {
  "lightning",
  "poison gases",
  "acid",
  "frost",
  "fire" };

const char *tavern_desc_move[] = {
  "move invisibly",
  "open doors",
  "pass through walls",
  "kill weaker creatures",
  "pick up objects",
  "breed explosively" };

const char *tavern_desc_weakness[] = {
  "frost",
  "fire",
  "poison",
  "acid",
  "bright light",
  "rock remover" };

const char *tavern_desc_atype[] = {
  "do something undefined",
  "attack",
  "weaken",
  "confuse",
  "terrify",
  "shoot flames",
  "shoot acid",
  "freeze",
  "shoot lightning",
  "corrode",
  "blind",
  "paralyse",
  "steal money",
  "steal things",
  "poison",
  "reduce dexterity",
  "reduce constitution",
  "drain intelligence",
  "drain wisdom",
  "lower experience",
  "call for help",
  "disenchant",
  "eat your food",
  "absorb light",
  "absorb charges" };

const char *tavern_desc_amethod[] = {
  "make an undefined advance",
  "hit",
  "bite",
  "claw",
  "sting",
  "touch",
  "kick",
  "gaze",
  "breathe",
  "spit",
  "wail",
  "embrace",
  "crawl on you",
  "release spores",
  "beg",
  "slime you",
  "crush",
  "trample",
  "drool",
  "insult" };


/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ tavern2_erase_line(int8u row, int8u col);
void __fastcall__ tavern2_erase_middle(void);
int8u __fastcall__ tavern_gossip(int8u chance, int8u barkeep);
void __fastcall__ generate_gossip(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		TAVERN2NUM

#undef  INBANK_MOD
#define INBANK_MOD			tavern2


void __fastcall__ tavern2_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"


/* Erase the interaction area -SJ*/
void __fastcall__ tavern2_erase_middle(void)
{
  int8u i;

  for (i=5; i<18; ++i)
    tavern2_erase_line(i, 0);
}


/* Gossip/information from either the barkeep, or patrons of the bar -SJ */
int8u __fastcall__ tavern_gossip(int8u chance, int8u barkeep)
{
  /* Already got gossip today? */
  if (tavern_got_gossip && (randint(20) > 2))			// small chance to hear gossip again
    return(FALSE);

  /* Any gossip to hear? */
  if (chance == 0)
    chance = 1;						// at least a small chance!

  if (chance < randint(100)) {		// no gossip
    return(FALSE);
  }
  else {							// here's some gossip
    tavern2_erase_line(0, 0);		// erase top line
    tavern2_erase_middle();			// and erase middle

    if (barkeep) {					// barkeep
      bank_prt("The barkeep leans over and says:", 5, 2, TAVERN2NUM);
	}
    else {							// patron
	  strcpy(out_val1, patron_desc[randint(16)-1]);
	  strcat(out_val1, " sits next to you and says:");
	  bank_prt(out_val1, 5, 2, TAVERN2NUM);
	}

    generate_gossip();

	bank_prt("press any key", 11, 33, TAVERN2NUM);
    cgetc();

	tavern2_erase_middle();
    tavern_got_gossip = TRUE;
    return(TRUE);
  }
}


/* Generate some random gossip about the monsters in the mines -SJ */
void __fastcall__ generate_gossip(void)
{
  register creature_type *cptr;
  register int8u i;
  int16u mon;
  int32u stat, j;
  int8u type, mspeed, k;
  int8u att_type, att_how;
  char *p;


  vbxe_bank(VBXE_GENBANK);
  strcpy(text, gossip_start[randint(5)-1]);

  // monster to get info on
  mon = randint(MAX_CREATURES)-1;

  // type of information
  type = randint(7);

  vbxe_bank(VBXE_MONBANK);
  cptr = &c_list[mon];
  strcpy(out_val1, cptr->name);


  switch(type) {
    case 1:
    /* Movement Information */
    stat = cptr->cmove;
    /* the c_list speed value is 10 greater, so that it can be a int8u */
    mspeed = cptr->speed;
    if (stat & CM_ALL_MV_FLAGS) {
      k = 1;
      strcat(out_val1, " moves");
      if (stat & CM_RANDOM_MOVE) {
    	strcat(out_val1, tavern_desc_howmuch[((stat & CM_RANDOM_MOVE) >> 3)]);
    	strcat(out_val1, " erratically");
      }
      if (mspeed == 11)
  	    strcat(out_val1, " at normal speed");
      else {
  	    if (stat & CM_RANDOM_MOVE)
  	      strcat(out_val1, ", and");
  	    if (mspeed <= 10) {
  	      if (mspeed == 9)
  		    strcat(out_val1, " very");
  	      else if (mspeed < 8)
  		    strcat(out_val1, " incredibly");
  	      strcat(out_val1, " slowly");
  	    }
  	    else {
  	      if (mspeed == 13)
  		    strcat(out_val1, " very");
  	      else if (mspeed > 13)
  		    strcat(out_val1, " unbelievably");
  	      strcat(out_val1, " quickly");
  	    }
  	  }
    }
    if (stat & CM_ATTACK_ONLY) {
      if(k)
  	    strcat(out_val1, ", but");
      else {
  	    k = TRUE;
  	  }
      strcat(out_val1, " will not chase intruders");
    }
    if (stat & CM_ONLY_MAGIC) {
      if (k)
  	    strcat(out_val1, ", but");
      strcat(out_val1, " always moves and attacks by using magic");
    }
    if(k)
      strcat(out_val1, ".");

	/* learn it */
	vbxe_bank(VBXE_GENBANK);
    c_recall[mon].r_cmove = (stat & CM_ALL_MV_FLAGS);
	break;

    case 2:
  /* Spells known, if have been used against us. */
  k = TRUE;
  stat = cptr->spells;
  j = stat;
  for (i = 0; j & CS_BREATHE; ++i) {
    if (j & (CS_BR_LIGHT << i)) {
	  j &= ~(CS_BR_LIGHT << i);
	  if (k) {
	    strcat(out_val1, " can breathe ");
	    k = FALSE;
	  }
	  else if (stat & CS_BREATHE)
	    strcat(out_val1, ", ");
	  else
	    strcat(out_val1, " and ");
	  strcat(out_val1, tavern_desc_breath[i]);
	}
  }
  k = TRUE;
  for (i = 0; j & CS_SPELLS; ++i) {
    if (j & (CS_TEL_SHORT << i)) {
	  j &= ~(CS_TEL_SHORT << i);
	  if (k) {
	    if (stat & CS_BREATHE)
		  strcat(out_val1, ", and is also");
	    else
		  strcat(out_val1, " is");
	    strcat(out_val1, " magical, casting spells which ");
	    k = FALSE;
	  }
	  else if (j & CS_SPELLS)
	    strcat(out_val1, ", ");
	  else
	    strcat(out_val1, " or ");
	  strcat(out_val1, tavern_desc_spell[i]);
	}
  }
  if (stat & (CS_BREATHE|CS_SPELLS))
    strcat(out_val1, ".");
  else
    strcat(out_val1, " does not have a breath weapon or cast spells.");

  /* learn it */
  vbxe_bank(VBXE_GENBANK);
  c_recall[mon].r_spells = (stat & (CS_BREATHE|CS_SPELLS));			// don't learn frequency
  break;

  case 3:
    /* Do we know how clever they are? Special abilities. */
    k = TRUE;
    stat = cptr->cmove;
    j = stat;
    for (i = 0; j & CM_SPECIAL; ++i) {
      if (j & (CM_INVISIBLE << i)) {
  	    j &= ~(CM_INVISIBLE << i);
  	    if (k) {
  	      strcat(out_val1, " can ");
  	      k = FALSE;
  	    }
  	    else if (j & CM_SPECIAL)
  	      strcat(out_val1, ", ");
  	    else
  	      strcat(out_val1, " and ");
  	    strcat(out_val1, tavern_desc_move[i]);
  	  }
    }
    if (stat & CM_SPECIAL)
      strcat(out_val1, ".");
    else
      strcat(out_val1, " has no special abilities.");

    /* learn it */
    vbxe_bank(VBXE_GENBANK);
    c_recall[mon].r_cmove = (stat & CM_SPECIAL);
    break;

  case 4:
    /* Do we know its special weaknesses? Most cdefense flags. */
    k = TRUE;
    stat = cptr->cdefense;
    j = stat;
    for (i = 0; j & CD_WEAKNESS; ++i) {
      if (j & (CD_FROST << i)) {
	    j &= ~(CD_FROST << i);
	    if (k) {
	      strcat(out_val1, " is susceptible to ");
	      k = FALSE;
	    }
	    else if (j & CD_WEAKNESS)
	      strcat(out_val1, ", ");
	    else
	      strcat(out_val1, " and ");
	    strcat(out_val1, tavern_desc_weakness[i]);
	  }
    }
    if (stat & (CD_WEAKNESS))
      strcat(out_val1, ".");
    else
      strcat(out_val1, " has no special weakness.");

    /* learn it */
    vbxe_bank(VBXE_GENBANK);
    c_recall[mon].r_cdefense = (stat & CD_WEAKNESS);
    break;

  case 5:
    /* Do we know how aware it is? */
    i = cptr->sleep;

    if(i > 200)
	  strcat(out_val1, " prefers to ignore");
    else if(i > 95)
	  strcat(out_val1, " pays very little attention to");
    else if(i > 75)
	  strcat(out_val1, " pays little attention to");
    else if(i > 45)
	  strcat(out_val1, " tends to overlook");
    else if(i > 25)
	  strcat(out_val1, " takes quite a while to see");
    else if(i > 10)
	  strcat(out_val1, " takes a while to see");
    else if(i > 5)
	  strcat(out_val1, " is fairly observant of");
    else if(i > 3)
	  strcat(out_val1, " is observant of");
    else if(i > 1)
	  strcat(out_val1, " is very observant of");
    else if(i != 0)
	  strcat(out_val1, " is vigilant for");
    else
	  strcat(out_val1, " is ever vigilant for");
    sprintf(out_val2, " intruders, which it may notice from %d feet.", 10 * cptr->aaf);
    strcat(out_val1, out_val2);

    /* learn it */
    vbxe_bank(VBXE_GENBANK);
    c_recall[mon].r_wake = i;
    break;

  case 6:
    /* Do we know what it might carry? */
    stat = cptr->cmove;

    if (stat & (CM_CARRY_OBJ|CM_CARRY_GOLD)) {
      strcat(out_val1, " may");
      j = (stat & CM_TREASURE) >> CM_TR_SHIFT;
      if (j == 1) {
	    if ((stat & CM_TREASURE) == CM_60_RANDOM)
	      strcat(out_val1, " sometimes");
	    else
	      strcat(out_val1, " often");
	  }
      else if ((j == 2) && ((stat & CM_TREASURE) == (CM_60_RANDOM|CM_90_RANDOM)))
	    strcat(out_val1, " often");
      strcat(out_val1, " carry");
      p = " objects";
      if (j == 1)
	    p = " an object";
      else if (j == 2)
	    strcat(out_val1, " one or two");
      else {
	    sprintf(out_val2, " up to %ld", j);
	    strcat(out_val1, out_val2);
	  }
      if (stat & CM_CARRY_OBJ) {
	    strcat(out_val1, p);
	    if (stat & CM_CARRY_GOLD) {
	      strcat(out_val1, " or treasure");
	      if (j > 1)
	        strcat(out_val1, "s");
	    }
	    strcat(out_val1, ".");
	  }
      else if (j != 1)
	    strcat(out_val1, " treasures.");
      else
	    strcat(out_val1, " treasure.");
    }
    else
      strcat(out_val1, " carries no treasure.");

    /* learn it */
    vbxe_bank(VBXE_GENBANK);
    c_recall[mon].r_wake = i;
    break;

    case 7:
      k = 10;
	  do {
	    i = randint(4)-1;						// four potential attacks
	    if (cptr->damage[i] != 0)				// found a valid attack type
	      break;								// break out of endless loop
	    else
	      --k;									// just try 10 times to find a valid attack
	  } while(k);

	  if (!k) {									// no physical attacks, or failed to find one randomly
	    strcat(out_val1, " has no known attacks.");
	  }

	  att_type = monster_attacks[cptr->damage[i]].attack_type;
	  att_how = monster_attacks[cptr->damage[i]].attack_desc;

	  strcat(out_val1, " can ");

	  if (att_how > 19)
		att_how = 0;
	  strcat(out_val1, tavern_desc_amethod[att_how]);
      strcat(out_val1, " to ");
	  if (att_type > 24)
		att_type = 0;
      strcat(out_val1, tavern_desc_atype[att_type]);
	  strcat(out_val1, ".");

      /* learn it */
      vbxe_bank(VBXE_GENBANK);
      c_recall[mon].r_attacks[i] = 1;
      break;
  }

  /* Print the text */
  vbxe_bank(VBXE_GENBANK);
  strcat(text, out_val1);
  bank_pretty_print(text, 7, TAVERN2NUM);

  return;
}

