/* source/objdes.c: store code, entering, command interpreter, buying, selling

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


#pragma code-name("OBJDESBANK")
#pragma rodata-name("OBJDESBANK")


/* Prototypes of functions moved into this cartridge bank */
void __fastcall__ objdes_insert_str(char *object_str, char *mtc_str, char *insert);
int8u __fastcall__ objdes_is_a_vowel(char ch);
int8u __fastcall__ objdes_known1_p(inven_type *i_ptr);
int8 __fastcall__ objdes_object_offset(inven_type *t_ptr);


#undef  INBANK_RETBANK
#define INBANK_RETBANK		OBJDESNUM

#undef  INBANK_MOD
#define INBANK_MOD			objdes


/* Inserts a string into a string				*/
void __fastcall__ objdes_insert_str(char *object_str, char *mtc_str, char *insert)
{
  int8u obj_len;
  char *bound, *pc;
  register int8u i, mtc_len;
  register char *temp_obj, *temp_mtc;
  char out_val[80];


  mtc_len = strlen(mtc_str);
  obj_len = strlen(object_str);
  bound = object_str + obj_len - mtc_len;
  for (pc = object_str; pc <= bound; ++pc) {
    temp_obj = pc;
    temp_mtc = mtc_str;
    for (i = 0; i < mtc_len; ++i)
	  if (*temp_obj++ != *temp_mtc++)
	    break;
      if (i == mtc_len)
	    break;
  }

  if (pc <= bound) {
    strncpy(out_val, object_str, (pc-object_str));
    out_val[pc-object_str] = '\0';
    if (insert)
	  strcat(out_val, insert);
    strcat(out_val, (char *)(pc+mtc_len));
    strcpy(object_str, out_val);
  }

  return;
}


int8u __fastcall__ objdes_is_a_vowel(char ch)
#include "inbank\is_a_vowel.c"

int8u __fastcall__ objdes_known1_p(inven_type *i_ptr)
#include "inbank\known1_p.c"

int8 __fastcall__ objdes_object_offset(inven_type *t_ptr)                                       // changed return type to int8 SJ
#include "inbank\object_offset.c"


/* defines for p1_use, determine how the p1 field is printed */
#define IGNORED   0
#define CHARGES   1
#define PLUSSES   2
#define LIGHT     3
#define FLAGS     4
#define Z_PLUSSES 5

/* Returns a description of item for inventory			*/
/* pref indicates that there should be an article added (prefix) */
/* note that since out_val can easily exceed 80 characters, objdes must
   always be called with a bigvtype as the first paramter */
void __fastcall__ objdes(char *out_val, inven_type *i_ptr, int8u pref)
{
  /* base name, modifier string*/
  register char *basenm;
  static char modstr[50];						// was char *modstr
  static bigvtype tmp_val;						// store in BSS (keep seperate from out_val1, out_val2 that we are using elsewhere -SJ)
  static vtype tmp_str;							// store in BSS (keep seperate from out_val1, out_val2 that we are using elsewhere -SJ)
  static char damstr[15];						// 15 characters should be enough -SJ
  register int8u p1_use;
  int8u modify, append_name;
  register int8 tmp;
  register int16 indexx;
  inven_type item;



  // copy item to stack - should still be in the right VBXE bank
  memcpy(&item, i_ptr, sizeof(inven_type));

  //sprintf(dbgstr, "objdes i:%d id:%X", item.index, item.ident);
  //bank_prt(dbgstr, 1, 1, OBJDESNUM);
  //cgetc();


  // clear the static temp variables (just in case)
  strcpy(tmp_val, "");
  strcpy(tmp_str, "");
  strcpy(damstr, "");
  strcpy(out_val, "");		// clear out_val (since it's a static now) -SJ

  vbxe_bank(VBXE_OBJBANK);										// required for object_list

  indexx = item.subval & (ITEM_SINGLE_STACK_MIN - 1);			// changed all i_ptr-> to item.
  basenm = object_list[item.index].name;						// pointer to object_list item name (in VBXE object bank)

  //sprintf(dbgstr, "objdes i:%d %p %s, %p %s", item.index, basenm, basenm, out_val, out_val);
  //bank_prt(dbgstr, 1, 1, OBJDESNUM);
  //cgetc();

  modstr[0] = '\0';
  damstr[0] = '\0';
  p1_use = IGNORED;
  modify = (objdes_known1_p(&item) ? FALSE : TRUE);
  append_name = FALSE;

  switch(item.tval)
  {
    case  TV_MISC:
    case  TV_CHEST:
      break;
    case  TV_SLING_AMMO:
    case  TV_BOLT:
    case  TV_ARROW:
      sprintf(damstr, " (%dd%d)", item.damage[0], item.damage[1]);
      break;
    case  TV_LIGHT:
      p1_use = LIGHT;
      break;
    case  TV_SPIKE:
      break;
    case  TV_BOW:
      if (item.p1 == 1 || item.p1 == 2)
	    tmp = 2;
      else if (item.p1 == 3 || item.p1 == 5)
	    tmp = 3;
      else if (item.p1 == 4 || item.p1 == 6)
	    tmp = 4;
      else
	    tmp = -1;
      sprintf (damstr, " (x%d)", tmp);
      break;
    case  TV_HAFTED:
    case  TV_POLEARM:
    case  TV_SWORD:
      sprintf(damstr, " (%dd%d)", item.damage[0], item.damage[1]);
      p1_use = FLAGS;
      break;
    case  TV_DIGGING:
      p1_use = Z_PLUSSES;
      sprintf(damstr, " (%dd%d)", item.damage[0], item.damage[1]);
      break;
    case  TV_BOOTS:
    case  TV_GLOVES:
    case  TV_CLOAK:
    case  TV_HELM:
    case  TV_SHIELD:
    case  TV_HARD_ARMOR:
    case  TV_SOFT_ARMOR:
      break;
    case  TV_AMULET:
      if (modify) {
	    basenm = "& %s Amulet";

	    vbxe_bank(VBXE_MAGBANK);
	    strcpy(modstr, amulets[indexx]);
	    vbxe_restore_bank();
	  }
      else
	  {
	    basenm = "& Amulet";
	    append_name = TRUE;
	  }
      p1_use = PLUSSES;
      break;
    case  TV_RING:
      if (modify) {
	    basenm = "& %s Ring";
	    //modstr = rocks[indexx];
	    vbxe_bank(VBXE_MAGBANK);
	    strcpy(modstr, rocks[indexx]);
	    vbxe_restore_bank();
	  }
      else
	  {
	    basenm = "& Ring";
	    append_name = TRUE;
	  }
      p1_use = PLUSSES;
      break;
    case  TV_STAFF:
      if (modify) {
	    basenm = "& %s Staff";
	    //modstr = woods[indexx];
	    vbxe_bank(VBXE_MAGBANK);
		strcpy(modstr, woods[indexx]);
		vbxe_restore_bank();
	  }
      else
	  {
	    basenm = "& Staff";
	    append_name = TRUE;
	  }
      p1_use = CHARGES;
      break;
    case  TV_WAND:
      if (modify) {
	    basenm = "& %s Wand";
	    //modstr = metals[indexx];
	    vbxe_bank(VBXE_MAGBANK);
		strcpy(modstr, metals[indexx]);
		vbxe_restore_bank();
	  }
      else
	  {
	    basenm = "& Wand";
	    append_name = TRUE;
	  }
      p1_use = CHARGES;
      break;
    case  TV_SCROLL1:
    case  TV_SCROLL2:
      if (modify)
	  {
	    basenm =  "& Scroll~ titled \"%s\"";
	    //modstr = titles[indexx];
	    vbxe_bank(VBXE_MAGBANK);
		strcpy(modstr, titles[indexx]);
		vbxe_restore_bank();
	  }
      else
	  {
	    basenm = "& Scroll~";
	    append_name = TRUE;
	  }
      break;
    case  TV_POTION1:
    case  TV_POTION2:
      if (modify)
	  {
	    basenm = "& %s Potion~";
	    //modstr = colors[indexx];
	    vbxe_bank(VBXE_MAGBANK);
		strcpy(modstr, colors[indexx]);
		vbxe_restore_bank();
	  }
      else
	  {
	    basenm = "& Potion~";
	    append_name = TRUE;
	  }
      break;
    case  TV_FLASK:
      break;
    case  TV_FOOD:
      if (modify) {
	    if (indexx <= 15)
	      basenm = "& %s Mushroom~";
	    else if (indexx <= 20)
	      basenm = "& Hairy %s Mold~";
	    if (indexx <= 20) {
	      //modstr = mushrooms[indexx];
	      vbxe_bank(VBXE_MAGBANK);
		  strcpy(modstr, mushrooms[indexx]);
		  vbxe_restore_bank();
	    }
	  }
      else
	  {
	    append_name = TRUE;
	    if (indexx <= 15)
	      basenm = "& Mushroom~";
	    else if (indexx <= 20)
	      basenm = "& Hairy Mold~";
	    else
	      /* Ordinary food does not have a name appended.  */
	      append_name = FALSE;
	  }
      break;
    case  TV_MAGIC_BOOK:
      //modstr = basenm;
      strcpy(modstr, basenm);
      basenm = "& Book~ of Magic Spells %s";
      break;
    case  TV_PRAYER_BOOK:
      //modstr = basenm;
      strcpy(modstr, basenm);
      basenm = "& Holy Book~ of Prayers %s";
      break;
    case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
    case TV_SECRET_DOOR:
    case TV_RUBBLE:
      break;
    case TV_GOLD:
    case TV_INVIS_TRAP:
    case TV_VIS_TRAP:
    case TV_UP_STAIR:
    case TV_DOWN_STAIR:
      strcpy(out_val, object_list[item.index].name);			// object_list should be banked in already -SJ
      strcat(out_val, ".");
      return;
    case TV_STORE_DOOR:
      sprintf(out_val, "the entrance to the %s.", object_list[item.index].name);
      return;
    default:
      strcpy(out_val, "Error in objdes()");
      return;
  }

  vbxe_bank(VBXE_OBJBANK);

  //if (modstr != CNIL)
  if (modstr[0] != '\0')
    sprintf(tmp_val, basenm, modstr);
  else
    strcpy(tmp_val, basenm);

  if (append_name) {
      strcat(tmp_val, " of ");
      //vbxe_bank(VBXE_OBJBANK);
      strcat(tmp_val, object_list[item.index].name);
  }

  if (item.number != 1) {
    objdes_insert_str(tmp_val, "ch~", "ches");
    objdes_insert_str(tmp_val, "~", "s");
  }
  else
    objdes_insert_str(tmp_val, "~", CNIL);

  if (!pref) {
    if (!strncmp("some", tmp_val, 4))
	  strcpy(out_val, &tmp_val[5]);
    else if (tmp_val[0] == '&')
	/* eliminate the '& ' at the beginning */
	  strcpy(out_val, &tmp_val[2]);
    else
	  strcpy(out_val, tmp_val);
  }
  else {
    //if (item.name2 != SN_NULL && known2_p(i_ptr)) {			    // item.ident & ID_KNOWN2
    if (item.name2 != SN_NULL && (item.ident & ID_KNOWN2)) {		// replaced known2_p -SJ
	  strcat(tmp_val, " ");

	  vbxe_bank(VBXE_MAGBANK);
	  strcat(tmp_val, special_names[item.name2]);					// special_names is in MAGBANK
	  vbxe_restore_bank();

	}
    if (damstr[0] != '\0')
	  strcat(tmp_val, damstr);
    if (item.ident & ID_KNOWN2) {										// replaced known2_p(i_ptr) -SJ
	  /* originally used %+d, but several machines don't support it */
	  if (item.ident & ID_SHOW_HITDAM)
	    sprintf(tmp_str, " (%c%d,%c%d)", (item.tohit < 0) ? '-' : '+', abs(item.tohit), (item.todam < 0) ? '-' : '+', abs(item.todam));
	  else if (item.tohit != 0)
	    sprintf(tmp_str, " (%c%d)", (item.tohit < 0) ? '-' : '+', abs(item.tohit));
	  else if (item.todam != 0)
	    sprintf(tmp_str, " (%c%d)", (item.todam < 0) ? '-' : '+', abs(item.todam));
	  else
	    tmp_str[0] = '\0';
	  strcat(tmp_val, tmp_str);
	}
    /* Crowns have a zero base AC, so make a special test for them. */
    if (item.ac != 0 || (item.tval == TV_HELM)) {
	  sprintf(tmp_str, " [%d", item.ac);
	  strcat(tmp_val, tmp_str);
	  if (item.ident & ID_KNOWN2) {									// replaced known2_p(i_ptr) -SJ
	      /* originally used %+d, but several machines don't support it */
	      sprintf(tmp_str, ",%c%d", (item.toac < 0) ? '-' : '+', abs(item.toac));
	      strcat(tmp_val, tmp_str);
	  }
	  strcat(tmp_val, "]");
	}
    else if ((item.toac != 0) && (item.ident & ID_KNOWN2)) {		// replaced known2_p(i_ptr) -SJ
	  /* originally used %+d, but several machines don't support it */
	  sprintf(tmp_str, " [%c%d]", (item.toac < 0) ? '-' : '+', abs(item.toac));
	  strcat(tmp_val, tmp_str);
	}

    /* override defaults, check for p1 flags in the ident field */
    if (item.ident & ID_NOSHOW_P1)
	  p1_use = IGNORED;
    else if (item.ident & ID_SHOW_P1)
	  p1_use = Z_PLUSSES;
    tmp_str[0] = '\0';
    if (p1_use == LIGHT)
	  sprintf(tmp_str, " with %d turns of light", item.p1);
    else if (p1_use == IGNORED)
	  ;
    else if (item.ident & ID_KNOWN2) {								// replaced known2_p(i_ptr) -SJ
	  if (p1_use == Z_PLUSSES)
	    /* originally used %+d, but several machines don't support it */
	    sprintf(tmp_str, " (%c%d)", (item.p1 < 0) ? '-' : '+', abs(item.p1));
	  else if (p1_use == CHARGES)
	    sprintf(tmp_str, " (%d charges)", item.p1);
	  else if (item.p1 != 0) {
	    if (p1_use == PLUSSES)
	      sprintf(tmp_str, " (%c%d)", (item.p1 < 0) ? '-' : '+', abs(item.p1));
	    else if (p1_use == FLAGS) {
		  if (item.flags & TR_STR)
		    sprintf(tmp_str, " (%c%d to STR)", (item.p1 < 0) ? '-' : '+',abs(item.p1));
		  else if (item.flags & TR_STEALTH)
		    sprintf(tmp_str, " (%c%d to stealth)", (item.p1 < 0) ? '-' : '+',abs(item.p1));
		}
	  }
	}
    strcat(tmp_val, tmp_str);

    /* ampersand is always the first character */
    if (tmp_val[0] == '&') {
	  /* use &tmp_val[1], so that & does not appear in output */
	  if (item.number > 1)
	    sprintf(out_val, "%d%s", (int)item.number, &tmp_val[1]);
	  else if (item.number < 1)
	    sprintf(out_val, "%s%s", "no more", &tmp_val[1]);
	  else if (objdes_is_a_vowel(tmp_val[2]))
	    sprintf(out_val, "an%s", &tmp_val[1]);
	  else
	    sprintf(out_val, "a%s", &tmp_val[1]);
	}
    /* handle 'no more' case specially */
    else if (item.number < 1) {
	  /* check for "some" at start */
	  if (!strncmp("some", tmp_val, 4))
	    sprintf(out_val, "no more %s", &tmp_val[5]);
	  /* here if no article */
	  else
	    sprintf(out_val, "no more %s", tmp_val);
	}
    else
	  strcpy(out_val, tmp_val);

    tmp_str[0] = '\0';
    if ((indexx = objdes_object_offset(&item)) >= 0) {
	  indexx = (indexx <<= 6) + (item.subval & (ITEM_SINGLE_STACK_MIN - 1));
	  /* don't print tried string for store bought items */
	  if ((object_ident[indexx] & OD_TRIED) && !(item.ident & ID_STOREBOUGHT))		// replaced store_bought_p(i_ptr) -SJ
	    strcat(tmp_str, "tried ");
	}

    if (item.ident & (ID_MAGIK|ID_EMPTY|ID_DAMD)) {
	  if (item.ident & ID_MAGIK)
	    strcat(tmp_str, "magik ");
	  if (item.ident & ID_EMPTY)
	    strcat(tmp_str, "empty ");
	  if (item.ident & ID_DAMD)
	    strcat(tmp_str, "damned ");
	}

    if (item.inscrip[0] != '\0')
	  strcat(tmp_str, item.inscrip);
    else if ((indexx = strlen(tmp_str)) > 0)
	  /* remove the extra blank at the end */
	  tmp_str[indexx-1] = '\0';

    if (tmp_str[0]) {
	  sprintf(tmp_val, " {%s}", tmp_str);
	  strcat(out_val, tmp_val);
	}

    strcat(out_val, ".");
  }

  //sprintf(dbgstr, "objdes i:%d %s, %s %s", item.index, item.ident, basenm, out_val, damstr);
  //bank_prt(dbgstr, 23, 1, OBJDESNUM);
  //cgetc();

  return;
}


