/* source/tables.c: store/attack/RNG/etc tables and variables

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"



#pragma bss-name("SPELLBANK")
#pragma data-name("SPELLBANK")
#pragma rodata-name("SPELLBANK")


store_type store[MAX_STORES];

/* Place in player structure segment */

/* Buying and selling adjustments for character race VS store   */
/* owner race                                                    */
int8u rgold_adj[MAX_RACES][MAX_RACES] = {
			 /*      Hum,  HfE,      Elf,  Hal,  Gno,  Dwa, HfO,  HfT*/
/*Human          */       {  100,  105,  105,  110,  113,  115,  120,  125},
/*Half-Elf       */       {  110,  100,  100,  105,  110,  120,  125,  130},
/*Elf            */       {  110,  105,  100,  105,  110,  120,  125,  130},
/*Halfling       */       {  115,  110,  105,   95,  105,  110,  115,  130},
/*Gnome          */       {  115,  115,  110,  105,   95,  110,  115,  130},
/*Dwarf          */       {  115,  120,  120,  110,  110,   95,  125,  135},
/*Half-Orc       */       {  115,  120,  125,  115,  115,  130,  110,  115},
/*Half-Troll     */   	  {  110,  115,  115,  110,  110,  130,  110,  110}};


/* object_list[] index of objects that may appear in the store */
int16u store_choice[MAX_STORES][STORE_CHOICES] = {
	/* General Store */
{366,365,364,84,84,365,123,366,365,350,349,348,347,346,346,345,345,345,
	344,344,344,344,344,344,344,344},
	/* Armory        */
{94,95,96,109,103,104,105,106,110,111,112,114,116,124,125,126,127,129,103,
	104,124,125,91,92,95,96},
	/* Weaponsmith   */
{29,30,34,37,45,49,57,58,59,65,67,68,73,74,75,77,79,80,81,83,29,30,80,83,
	80,83},
	/* Temple        */
{322,323,324,325,180,180,233,237,240,241,361,362,57,58,59,260,358,359,265,
	237,237,240,240,241,323,359},
	/* Alchemy shop  */
{173,174,175,351,351,352,353,354,355,356,357,206,227,230,236,252,253,352,
	353,354,355,356,359,363,359,359},
	/* Magic-User store*/
{318,141,142,153,164,167,168,140,319,320,320,321,269,270,282,286,287,292,
	293,294,295,308,269,290,319,282}
};

/* Store owners have different characteristics for pricing and haggling*/
/* Note: Store owners should be added in groups, one for each store    */
owner_type owners[MAX_OWNERS] = {
{"Erick the Honest       (Human)      General Store",
	  250,  175,  108,    4, 0, 12},
{"Mauglin the Grumpy     (Dwarf)      Armory"       ,
	32000,  200,  112,    4, 5,  5},
{"Arndal Beast-Slayer    (Half-Elf)   Weaponsmith"  ,
	10000,  185,  110,    5, 1,  8},
{"Hardblow the Humble    (Human)      Temple"       ,
	 3500,  175,  109,    6, 0, 15},
{"Ga-nat the Greedy      (Gnome)      Alchemist"    ,
	12000,  220,  115,    4, 4,  9},
{"Valeria Starshine      (Elf)        Magic Shop"   ,
	32000,  175,  110,    5, 2, 11},
{"Andy the Friendly      (Halfling)   General Store",
	  200,  170,  108,    5, 3, 15},
{"Darg-Low the Grim      (Human)      Armory"       ,
	10000,  190,  111,    4, 0,  9},
{"Oglign Dragon-Slayer   (Dwarf)      Weaponsmith"  ,
	32000,  195,  112,    4, 5,  8},
{"Gunnar the Paladin     (Human)      Temple"       ,
	 5000,  185,  110,    5, 0, 23},
{"Mauser the Chemist     (Half-Elf)   Alchemist"    ,
	10000,  190,  111,    5, 1,  8},
{"Gopher the Great!      (Gnome)      Magic Shop"   ,
	20000,  215,  113,    6, 4, 10},
{"Lyar-el the Comely     (Elf)        General Store",
	  300,  165,  107,    6, 2, 18},
{"Mauglim the Horrible   (Half-Orc)   Armory"       ,
	 3000,  200,  113,    5, 6,  9},
{"Ithyl-Mak the Beastly  (Half-Troll) Weaponsmith"  ,
	 3000,  210,  115,    6, 7,  8},
{"Delilah the Pure       (Half-Elf)   Temple"       ,
	25000,  180,  107,    6, 1, 20},
{"Wizzle the Chaotic     (Halfling)   Alchemist"    ,
	10000,  190,  110,    6, 3,  8},
{"Inglorian the Mage     (Human?)     Magic Shop"   ,
	32000,  200,  110,    7, 0, 10}
};
