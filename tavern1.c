/* source/tavern.c: tavern code, entering, command interpreter, buying, selling

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


#pragma bss-name("BSS")
#pragma data-name("DATA")

int8u tavern_name;					// Tavern name, unique to game, initalized at town_gen
int8u tavern_atmos;					// Tavern atmosphere, unique to game, initialized at town_gen
int8u tavern_patrons_index;			// keep track of the bars patrons
int8u tavern_got_gossip;			// already received gossip for today?
int8u tavern_drunk = 0;				// drunk-o-meter

int32u tavern_day = 100000;  		// keep track of the tavern day
//int16u frac_chr = 0;			    // fractional charisma, moved in to py.misc -SJ
int8u tavern_song_num = 255;		// song number playing, 255 means none
int8u tavern_song_stanza = 255;		// song stanza, 255 means not playing


#pragma code-name("TAVERN1BANK")
#pragma rodata-name("TAVERN1BANK")
#pragma data-name("TAVERN1BANK")


#define TAVERN_NAMES	20
const char *tavern_names[20] = {
"The Toothless Orc", "The Balrog's Tongue", "The Drooling Harpy", "The Ancient Dragon", "The Broken Sword",
"Iggy's Tavern", "The Dragon's Breath", "The Pick & Axe", "The Hammer & Anvil", "The Mithril Gauntlet",
"The Wizard's Staff", "The Dancing Dragon", "The Busty Barmaid", "Durin's Forge", "The Dragon's Pillar", "The Merchant's Penny",
"The Hammered Anvil", "The Dead Dragon", "The Drunken Wizard", "Axe & Sword"
};

char *tavern_atmosphere[5] = {
"The tavern is dark and musty, with a sickly sweet aroma in the air.",
"You smell roasting meats and strong ale as you enter the tavern.",
"It seems like the floor has never been swept, and smells as if the patrons have never seen a bath!",
"The tavern is bright and inviting, the smells of cooking wafting out the open door.",
"The tavern is dimly lit, and smoky."
};

const char *tavern_patrons[5] = {
"Must be a slow day, the tavern is practically empty. Your entrance is barely noticed.",
"There are a few patrons sitting by the fire and one or two dark strangers sitting alone who eye you furtively as you enter.  There is a tension in the air.",
"Only a few tables are full, their occupants heads together in subdued conversation, who grow silent and turn to look at the door as you enter.",
"The taproom is a circus, with many loud conversations and drunken arguments brewing. You hear the loud boasting of several young men.",
"There are many townsfolk at the tavern, their conversations loud and raucous. Some local celebration perhaps.",
};

char *barkeep_thanks[4] = {
"The barkeep smiles as he takes your gold.",
"The barkeep gives you a wink as he takes your gold.",
"The barkeep thanks you as he takes your gold.",
"The barkeep gives you a drink on the house!"
};

char *round_thanks[4] = {
"You hear a few muttered thanks from the townsfolk in the Tavern.",
"A ragged cheer goes up from the patrons of the Tavern.",
"You are suddenly everyone's friend! At least until the mug is empty.",
"One or two Tavern patrons clap you on the back and drink to your health!"
};

#define NUM_SONGS		3

const int8u song_stanzas[NUM_SONGS][9] = {
  {4,  6,  6,  8,  8,  4, 10,  0,  0},
  {6,  0,  0,  0,  0,  0,  0,  0,  0},
  {10, 0,  0,  0,  0,  0,  0,  0,  0}
};

char *tavern_song0[] = {
"The world was young, the mountains green,",
"No stain yet on the Moon was seen,",
"No words were laidon stream or stone",
"When Durin woke and walked alone.",

"He named the nameless hills and dells;",
"He drank from yet untasted wells;",
"He stooped and looked in Mirrormere,",
"And saw a crown of stars appear," ,
"As gems upon a silver thread,",
"Above the shadow of his head.",

"The world was fair, the mountains tall,",
"In Elder Days before the fall",
"Of mighty kings in Nargothrond",
"And Gondolin, who now beyond",
"The Western Seas have passed away:",
"The world was fair in Durin's Day.",

"A king he was on carven throne",
"In many-pillared halls of stone",
"With golden roof and silver floor,",
"And runes of power upon the door.",
"The light of sun and star and moon",
"In shining lamps of crystal hewn",
"Undimmed by cloud or shade of night",
"There shone for ever fair and bright.",

"There hammer on the anvil smote,",
"There chisel clove, and graver wrote;",
"There forged was blade, and bound was hilt;",
"The delver mined, the mason built.",
"There beryl, pearl, and opal pale,",
"And metal wrought like fishes' mail,",
"Buckler and corslet, axe and sword,",
"And shining spears were laid in hoard.",

"Unwearied then were Durin's folk;",
"Beneath the mountains music woke:",
"The harpers harped, the minstrels sang,",
"And at the gates the trumpets rang.",

"The world is grey, the mountains old,",
"The forge's fire is ashen-cold;",
"No harp is wrung, no hammer falls",
"The darkness dwells in Durin's halls;",
"The shadow lies upon his tomb",
"In Moria, in Khazad-dum.",
"But still the sunken stars appear",
"In dark and windless Mirrormere;",
"There lies his crown in water deep,",
"Till Durin wakes again from sleep."
};

char *tavern_song1[] = {
  "Gondor! Gondor, between the Mountains and the Sea!",
  "West Wind blew there; the light upon the Silver Tree",
  "Fell like bright rain in gardens of the Kings of old.",
  "O proud walls! White towers! O winged crown and throne of gold!",
  "O Gondor, Gondor! Shall Men behold the Silver Tree,",
  "Or West Wind blow again between the Mountains and the Sea?"
};

char *tavern_song2[] = {
 "In Dwimordene, in Lorien",
 "Seldom have walked the feet of Men,",
 "Few mortal eyes have seen the light",
 "That lies there ever, long and bright.",
 "Galadriel! Galadriel!",
 "Clear is the water of your well;",
 "White is the star in your white hand;",
 "Unmarred, unstained is leaf and land",
 "In Dwimordene, in Lorien",
 "More fair than thoughts of Mortal Men."
};


/*const char *tavern_song3[] = {
 "The leaves were long, the grass was green,",
 "The hemlock-umbels tall and fair,",
 "And in the glade a light was seen",
 "Of stars in shadow shimmering.",
 "Tinúviel was dancing there",
 "To music of a pipe unseen,",
 "And light of stars was in her hair,",
 "And in her raiment glimmering.",

 "There Beren came from mountains cold,",
 "And lost he wandered under leaves,",
 "And where the Elven-river rolled",
 "He walked alone and sorrowing.",
 "He peered between the hemlock-leaves",
 "And saw in wonder flowers of gold",
 "Upon her mantle and her sleeves,",
 "And her hair like shadow following.",

 "Enchantment healed his weary feet",
 "That over hills were doomed to roam;",
 "And forth he hastened, strong and fleet,",
 "And grasped at moonbeams glistening.",
 "Through woven woods in Elvenhome",
 "She lightly fled on dancing feet,",
 "And left him lonely still to roam",
 "In the silent forest listening.",

 "He heard there oft the flying sound",
 "Of feet as light as linden-leaves,",
 "Or music welling underground,",
 "In hidden hollows quavering.",
 "Now withered lay the hemlock-sheaves,",
 "And one by one with sighing sound",
 "Whispering fell the beechen leaves",
 "In the wintry woodland wavering.",

 "He sought her ever, wandering far",
 "Where leaves of years were thickly strewn,",
 "By light of moon and ray of star",
 "In frosty heavens shivering.",
 "Her mantle glinted in the moon,",
 "As on a hilltop high and far",
 "She danced, and at her feet was strewn",
 "A mist of silver quivering.",

 "When winter passed, she came again,",
 "And her song released the sudden spring,",
 "Like rising lark, and falling rain,",
 "And melting water bubbling.",
 "He saw the elven-flowers spring",
 "About her feet, and healed again",
 "He longed by her to dance and sing",
 "Upon the grass untroubling.",

 "Again she fled, but swift he came.",
 "Tinuviel! Tinuviel!",
 "He called her by her elvish name,",
 "And there she halted listening.",
 "One moment stood she, and a spell",
 "His voice laid on her: Beren came,",
 "And doom fell on Tinuviel",
 "That in his arms lay glistening.",

 "As Beren looked into her eyes",
 "Within the shadows of her hair,",
 "The trembling starlight of the skies",
 "He saw there mirrored shimmering.",
 "Tinuviel the elven-fair,",
 "Immortal maiden elven-wise,",
 "About him cast her shadowy hair",
 "And arms like silver glimmering.",

 "Long was the way that fate them bore,",
 "O'er stony mountains cold and grey,",
 "Through halls of iron and darkling door,",
 "And woods of nightshade morrowless.",
 "The Sundering Seas between them lay,",
 "And yet at last they met once more,",
 "And long ago they passed away",
 "In the forest singing sorrowless.",
};*/

char **tavern_songs[NUM_SONGS] = {tavern_song0, tavern_song1, tavern_song2};


/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ tavern1_put_buffer(char *out_str, int8 row, int8 col);
void __fastcall__ tavern1_erase_line(int8u row, int8u col);
void __fastcall__ tavern1_erase_middle(void);
int8u __fastcall__ tavern1_chr_adj(void);
int8u __fastcall__ tavern1_inc_stat(int8u stat);
int8u __fastcall__ tavern1_ask(int32u price);
void __fastcall__ tavern_display_commands(void);
void __fastcall__ tavern1_draw_cave(void);
void __fastcall__ tavern_update_song(void);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		TAVERN1NUM

#undef  INBANK_MOD
#define INBANK_MOD			tavern1



void __fastcall__ tavern1_erase_line(int8u row, int8u col)
#include "inbank\erase_line.c"

void __fastcall__ tavern1_erase_middle(void)
#include "inbank\erase_middle.c"

void __fastcall__ tavern1_put_buffer(char *out_str, int8 row, int8 col)
#include "inbank\put_buffer.c"

int8u __fastcall__ tavern1_chr_adj(void)
#include "inbank\chr_adj.c"



/* Increases a stat by one randomized level	-RAK- */
/* Function in this bank does not print the stat! -SJ */
int8u __fastcall__ tavern1_inc_stat(int8u stat)
{
  int8u tmp_stat, gain;


  tmp_stat = py.stats.cur_stat[stat];
  if (tmp_stat < 118) {
    if (tmp_stat < 18)
	  ++tmp_stat;
    else if (tmp_stat < 116) {
	  /* stat increases by 1/6 to 1/3 of difference from max */
	  gain = ((118 - tmp_stat)/3 + 1) >> 1;
	  tmp_stat += randint(gain) + gain;
	}
    else
	  ++tmp_stat;

    py.stats.cur_stat[stat] = tmp_stat;
    if (tmp_stat > py.stats.max_stat[stat])
	  py.stats.max_stat[stat] = tmp_stat;

    bank_set_use_stat(stat, TAVERN1NUM);
    return TRUE;
  }
  else
    return FALSE;
}


/* Gets the players answer and subtracts the gold -SJ */
int8u __fastcall__ tavern1_ask(int32u price)
{
  char c;


  c = cgetc();
  if (c == 'y' || c == 'Y') {
	if (py.misc.au < price) {
  	  /* Not enough money! */
      bank_msg_print("You don't have enough gold!", TAVERN1NUM);
      bank_msg_print("", TAVERN1NUM);
      return(FALSE);
  	}
  	else {
  	  py.misc.au -= price;
  	  return(TRUE);
  	}
  }

  return(FALSE);
}


/* Displays the set of commands	-SJ	*/
void __fastcall__ tavern_display_commands(void)
{
  tavern1_put_buffer("What will it be?", 20, 10);
  tavern1_put_buffer(" b) Buy a Drink.                  r) Buy a Round.", 21, 10);
  tavern1_put_buffer(" t) Tip the Barkeep.              q) View your Quests.", 22, 10);
  tavern1_put_buffer("ESC) Exit from Building.", 23, 10);
}


/* Displays players gold -RAK-	*/
void __fastcall__ tavern1_prt_gold(void)
{
  sprintf(out_val1, "Gold Remaining : %ld", py.misc.au);			// py is in BSS -SJ
  bank_prt(out_val1, 18, 17, TAVERN1NUM);
}


/* Initialize the tavern, done during town_gen with town_seed -SJ */
void __fastcall__ tavern_init(void)
{
  tavern_name = randint(TAVERN_NAMES)-1;
  tavern_atmos = randint(5)-1;
}


/* Entering the tavern -SJ */
void __fastcall__ enter_tavern(void)
{
  char command;
  int8u ret, patrons;
  int8u time;
  int16u price;
  int32 tip;
  int8u exit_flag = FALSE;
  int8u redraw = FALSE;


  /* Determine tavern day */
  if (tavern_day != turn / 5000) {
    tavern_day = turn / 5000;				// a day/night cycle is 5000 turns according to town_gen  -SJ
    tavern_patrons_index = randint(5)-1;
    tavern_got_gossip = 0;					// reset gossip flag
    tavern_drunk = 0;
  }
  patrons = tavern_patrons_index;

  /* Display the Tavern */
  bank_vbxe_clear(TAVERN1NUM);
  strcpy(out_val1, tavern_names[tavern_name]);
  tavern1_put_buffer(out_val1, 2, 40-(strlen(out_val1)/2));
  tavern1_prt_gold();

  /* Display the atmosphere + patrons */
  vbxe_bank(VBXE_GENBANK);
  strcpy(text, tavern_atmosphere[tavern_atmos]);
  strcat(text, " ");
  strcat(text, tavern_patrons[patrons]);
  bank_pretty_print(text, 5, TAVERN1NUM);
  vbxe_restore_bank();

  /* Tavern main loop */
  time = RTCLOK1;							// get time we entered
  tavern_display_commands();
  do {
    if (redraw) {
	  tavern1_prt_gold();
	  redraw = FALSE;
	}

    /* clear the msg flag just like we do in dungeon.c */
    msg_flag = FALSE;

	bank_quest_end();						// check our quests
	redraw = TRUE;

    while (!kbhit()) {						// while idle
	  if (abs(RTCLOK1 - time) > 3) {    	// only if around 9 seconds have passed
	    tavern_update_song();				// update the song

        if (!tavern_got_gossip) {
	      bank_tavern_gossip(2, FALSE);		// gossip
          bank_quest_start(2);				// quest
		}

	    time = RTCLOK1;						// update the time
  	  }
    }

    if (bank_get_com("", &command, TAVERN1NUM)) {
      switch(command) {
		case 'b':						// buy a drink
		  price = 5;
		  price = price * (int32u) tavern1_chr_adj() / 100;

		  sprintf(out_val1, "%d gold for our finest ale. [y/n]", price);
		  bank_prt(out_val1, 0, 0, TAVERN1NUM);

		  if (tavern1_ask(price)) {		// ask for money
		    ret = bank_tavern_gossip(10, TRUE);
		    if (!ret)
		      bank_msg_print(barkeep_thanks[randint(3)-1], TAVERN1NUM);				// barkeep won't give you one on the house

	  	    tavern_drunk++;
	  	  }
	  	  else
	  	    tavern1_erase_line(0, 0);

		  redraw = TRUE;
          break;
		case 'r':						// buy a round
		  //price = (int32u) (patrons+1) * (randint(10)+3) * 5;
		  price = (int32u) (patrons+1) * 25;
		  price = price * (int32u) tavern1_chr_adj() / 100;

		  sprintf(out_val1, "It will cost %d gold to buy a round. [y/n]", price);
		  bank_prt(out_val1, 0, 0, TAVERN1NUM);

		  if (tavern1_ask(price)) {
	        py.misc.frac_chr += (int16u) (patrons+1) * (randint(20)+3);
	        if (py.misc.frac_chr > 1000) {
			  tavern1_inc_stat(A_CHR);
			  py.misc.frac_chr = 0;
			  bank_msg_print("You've risen in the estimation of the townsfolk.", TAVERN1NUM);
		    }
			else
			  bank_msg_print(round_thanks[randint(4)-1], TAVERN1NUM);

			bank_msg_print("", TAVERN1NUM);
		    bank_tavern_gossip((patrons+1)*10, FALSE);
		    bank_quest_start((patrons+1)*5);

		    tavern_drunk++;
  	      }
  	      else
  	        tavern1_erase_line(0, 0);

		  redraw = TRUE;
		  break;
		case 't':						// tip the barkeep
		  bank_prt("Tip how much? ", 0, 0, TAVERN1NUM);
		  if (bank_get_string(out_val1, 0, 14, 40, TAVERN1NUM)) {
            tavern1_erase_line(0, 0);
		    tip = atol(out_val1);
		    if (tip > 0) {
		      if (py.misc.au < tip) {
			    /* Not enough money! */
			    bank_msg_print("You don't have enough gold!", TAVERN1NUM);
                bank_msg_print("", TAVERN1NUM);
		  	  }
		  	  else {
				py.misc.au -= tip;

				/* Chances for gossip go up the more you tip */
				if (tip > 200)
				  price = 70;
				else if (tip > 150)
				  price = 60;
				else if (tip > 100)
				  price = 50;
				else
				  price = (int8u) tip / 2;
				if (price < 20)
				  price = 20;

				ret = bank_tavern_gossip((int8u)price, TRUE);
		        if (!ret)
		          bank_msg_print(barkeep_thanks[randint(4)-1], TAVERN1NUM);
		      }
		    }
	  	  }
	  	  else
	  	    tavern1_erase_line(0, 0);

		  redraw = TRUE;
		  break;

		case 'q':
		  bank_show_quests(TAVERN1NUM);
		  break;

		default:
		  break;
      }
	}
	else
	  exit_flag = TRUE;

    /* Are we drunk yet? */
    if (tavern_drunk > 7) {
	  py.flags.confused += 40;
	}
  } while (!exit_flag);

  tavern_drunk = 0;			// reset drunkeness so we don't get confused by re-entering the tavern

  /* Can't save and restore the screen because inven_command does that. */
  tavern1_draw_cave();
}


/* Draws entire screen					-RAK-	*/
void __fastcall__ tavern1_draw_cave(void)
{
  //clear_screen();
  bank_vbxe_clear(TAVERN1NUM);
  bank_prt_stat_block(TAVERN1NUM);
  bank_prt_map(TAVERN1NUM);
  bank_prt_depth(TAVERN1NUM);
}


/* Bard playing a song in the Tavern? -SJ */
void __fastcall__ tavern_update_song(void)
{
  int8u i;
  int8u start;


  /* Start playing a song? */
  if (tavern_song_num == 255) {
    if (randint(100) < 33) {
	//if (1) {									// FIXME: DEBUGGING!
	  tavern_song_num = randint(NUM_SONGS) - 1;
	  tavern_song_stanza = 0;
	}
	return;
  }

  /* Is this song over? */
  if (song_stanzas[tavern_song_num][tavern_song_stanza] == 0) {
    tavern1_erase_middle();
    tavern_song_num = 255;
    tavern_song_stanza = 255;
    return;
  }

  /* Find start of stanza */
  start = 0;
  for (i=0; i < tavern_song_stanza; ++i) {
    start += song_stanzas[tavern_song_num][i];
  }

  /* Update the song! */
  tavern1_erase_middle();
  tavern1_put_buffer("A Bard is singing:", 5, 2);

  for (i = 0; i < song_stanzas[tavern_song_num][tavern_song_stanza]; ++i) {
    tavern1_put_buffer(tavern_songs[tavern_song_num][start+i], 7+i, 4);
  }

  ++tavern_song_stanza;
  return;
}
