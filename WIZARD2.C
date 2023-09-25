/* wizard.c: wizard mode functions

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


#pragma code-name("WIZARD2BANK")
#pragma data-name("WIZARD2BANK")
#pragma rodata-name("WIZARD2BANK")


/* Local Prototypes */


#undef  INBANK_RETBANK
#define INBANK_RETBANK		WIZ2NUM

#undef  INBANK_MOD
#define INBANK_MOD			wizard2



void __fastcall__ wizard_display_player(void)
{
  bank_vbxe_clear(WIZ2NUM);
  bank_prt("Player Structure", 0, 0, WIZ2NUM);

  sprintf(out_val1, "male:    %-3d                                srh:     %d", py.misc.male, py.misc.srh);
  bank_prt(out_val1, 2, 0, WIZ2NUM);
  sprintf(out_val1, "age:     %-5d                              fos:     %d", py.misc.age, py.misc.fos);
  bank_prt(out_val1, 3, 0, WIZ2NUM);
  sprintf(out_val1, "ht:      %-5d                              bth:     %d", py.misc.ht, py.misc.bth);
  bank_prt(out_val1, 4, 0, WIZ2NUM);
  sprintf(out_val1, "sc:      %-4d                               bthb:    %d", py.misc.sc, py.misc.bthb);
  bank_prt(out_val1, 5, 0, WIZ2NUM);
  sprintf(out_val1, "pclass:  %-3d                                ptohit:  %-4d dis_th:  %d", py.misc.pclass, py.misc.ptohit, py.misc.dis_th);
  bank_prt(out_val1, 6, 0, WIZ2NUM);
  sprintf(out_val1, "prace:   %-3d                                ptodam:  %-4d dis_td:  %d", py.misc.prace, py.misc.ptodam, py.misc.dis_td);
  bank_prt(out_val1, 7, 0, WIZ2NUM);
  sprintf(out_val1, "lev:     %-3d       max_dlv:    %-3d          pac:     %-4d dis_ac:  %d", py.misc.lev, py.misc.max_dlv, py.misc.pac, py.misc.dis_ac);
  bank_prt(out_val1, 8, 0, WIZ2NUM);
  sprintf(out_val1, "hitdie:  %-3d                                ptoac:   %-4d dis_tac: %d", py.misc.hitdie, py.misc.ptoac, py.misc.dis_tac);
  bank_prt(out_val1, 9, 0, WIZ2NUM);
  sprintf(out_val1, "au:      %-8ld                           disarm:  %d", py.misc.au, py.misc.disarm);
  bank_prt(out_val1, 10, 0, WIZ2NUM);
  sprintf(out_val1, "expfact: %-3d                                save:    %d", py.misc.expfact, py.misc.save);
  bank_prt(out_val1, 11, 0, WIZ2NUM);
  sprintf(out_val1, "max_exp: %-8ld                           stl:     %d", py.misc.max_exp, py.misc.stl);
  bank_prt(out_val1, 12, 0, WIZ2NUM);
  sprintf(out_val1, "exp:     %-8ld  exp_frac:   %d", py.misc.exp, py.misc.exp_frac);
  bank_prt(out_val1, 13, 0, WIZ2NUM);
  sprintf(out_val1, "mhp:     %-5d                                   Max  Cur  Mod  Use", py.misc.mhp);
  bank_prt(out_val1, 14, 0, WIZ2NUM);
  sprintf(out_val1, "chp:     %-5d     chp_frac:   %-5d        STR  %-3d  %-3d  %-4d %d", py.misc.chp, py.misc.chp_frac, py.stats.max_stat[A_STR], py.stats.cur_stat[A_STR], py.stats.mod_stat[A_STR], py.stats.use_stat[A_STR]);
  bank_prt(out_val1, 15, 0, WIZ2NUM);
  sprintf(out_val1, "mana:    %-5d                              INT  %-3d  %-3d  %-4d %d", py.misc.mana, py.stats.max_stat[A_INT], py.stats.cur_stat[A_INT], py.stats.mod_stat[A_INT], py.stats.use_stat[A_INT]);
  bank_prt(out_val1, 16, 0, WIZ2NUM);
  sprintf(out_val1, "cmana:   %-5d     cmana_frac: %-5d        WIS  %-3d  %-3d  %-4d %d", py.misc.cmana, py.misc.cmana_frac, py.stats.max_stat[A_WIS], py.stats.cur_stat[A_WIS], py.stats.mod_stat[A_WIS], py.stats.use_stat[A_WIS]);
  bank_prt(out_val1, 17, 0, WIZ2NUM);
  sprintf(out_val1, "DEX  %-3d  %-3d  %-4d %d", py.stats.max_stat[A_DEX], py.stats.cur_stat[A_DEX], py.stats.mod_stat[A_DEX], py.stats.use_stat[A_DEX]);
  bank_prt(out_val1, 18, 44, WIZ2NUM);
  sprintf(out_val1, "CON  %-3d  %-3d  %-4d %d", py.stats.max_stat[A_CON], py.stats.cur_stat[A_CON], py.stats.mod_stat[A_CON], py.stats.use_stat[A_CON]);
  bank_prt(out_val1, 19, 44, WIZ2NUM);
  sprintf(out_val1, "CHR  %-3d  %-3d  %-4d %d", py.stats.max_stat[A_CHR], py.stats.cur_stat[A_CHR], py.stats.mod_stat[A_CHR], py.stats.use_stat[A_CHR]);
  bank_prt(out_val1, 20, 44, WIZ2NUM);

  bank_prt("press any key", 23, 32, WIZ2NUM);
  cgetc();

  bank_vbxe_clear(WIZ2NUM);

  sprintf(out_val1, "status:      0x%08lx", py.flags.status);
  bank_prt(out_val1, 0, 0, WIZ2NUM);
  sprintf(out_val1, "rest:        %-4d                                 see_infra:       %d", py.flags.rest, py.flags.see_infra);
  bank_prt(out_val1, 1, 0, WIZ2NUM);
  sprintf(out_val1, "blind:       %-5d                                tim_infra:       %d", py.flags.blind, py.flags.tim_infra);
  bank_prt(out_val1, 2, 0, WIZ2NUM);
  sprintf(out_val1, "paralysis:   %-3d                                  see_inv:         %d", py.flags.paralysis, py.flags.see_inv);
  bank_prt(out_val1, 3, 0, WIZ2NUM);
  sprintf(out_val1, "confused:    %-3d                                  teleport:        %d", py.flags.confused, py.flags.teleport);
  bank_prt(out_val1, 4, 0, WIZ2NUM);
  sprintf(out_val1, "food:        %-6d      food_digested: %-3d       free_act:        %d", py.flags.food, py.flags.food_digested, py.flags.free_act);
  bank_prt(out_val1, 5, 0, WIZ2NUM);
  sprintf(out_val1, "protection:  %-6d                               slow_digest:     %d", py.flags.protection, py.flags.slow_digest);
  bank_prt(out_val1, 6, 0, WIZ2NUM);
  sprintf(out_val1, "speed:       %-4d                                 aggravate:       %d", py.flags.speed, py.flags.aggravate);
  bank_prt(out_val1, 7, 0, WIZ2NUM);
  sprintf(out_val1, "fast:        %-3d                                  fire_resist:     %d", py.flags.fast, py.flags.fire_resist);
  bank_prt(out_val1, 8, 0, WIZ2NUM);
  sprintf(out_val1, "slow:        %-3d                                  cold_resist:     %d", py.flags.slow, py.flags.cold_resist);
  bank_prt(out_val1, 9, 0, WIZ2NUM);
  sprintf(out_val1, "afraid:      %-3d                                  acid_resist:     %d", py.flags.afraid, py.flags.acid_resist);
  bank_prt(out_val1, 10, 0, WIZ2NUM);
  sprintf(out_val1, "poisoned:    %-3d                                  regenerate:      %d", py.flags.poisoned, py.flags.regenerate);
  bank_prt(out_val1, 11, 0, WIZ2NUM);
  sprintf(out_val1, "image:       %-5d                                lght_resist:     %d", py.flags.image, py.flags.lght_resist);
  bank_prt(out_val1, 12, 0, WIZ2NUM);
  sprintf(out_val1, "protevil:    %-3d                                  ffall:           %d", py.flags.protevil, py.flags.ffall);
  bank_prt(out_val1, 13, 0, WIZ2NUM);
  sprintf(out_val1, "invuln:      %-3d                                  sustain_str:     %d", py.flags.invuln, py.flags.sustain_str);
  bank_prt(out_val1, 14, 0, WIZ2NUM);
  sprintf(out_val1, "hero:        %-3d                                  sustain_int:     %d", py.flags.hero, py.flags.sustain_int);
  bank_prt(out_val1, 15, 0, WIZ2NUM);
  sprintf(out_val1, "shero:       %-3d                                  sustain_wis:     %d", py.flags.shero, py.flags.sustain_wis);
  bank_prt(out_val1, 16, 0, WIZ2NUM);
  sprintf(out_val1, "blessed:     %-3d                                  sustain_dex:     %d", py.flags.blessed, py.flags.sustain_dex);
  bank_prt(out_val1, 17, 0, WIZ2NUM);
  sprintf(out_val1, "resist_heat: %-3d                                  sustain_con:     %d", py.flags.resist_heat, py.flags.sustain_con);
  bank_prt(out_val1, 18, 0, WIZ2NUM);
  sprintf(out_val1, "resist_cold: %-3d                                  sustain_chr:     %d", py.flags.resist_cold, py.flags.sustain_chr);
  bank_prt(out_val1, 19, 0, WIZ2NUM);
  sprintf(out_val1, "detect_inv:  %-3d                                  confuse_monster: %d", py.flags.detect_inv, py.flags.confuse_monster);
  bank_prt(out_val1, 20, 0, WIZ2NUM);
  sprintf(out_val1, "new_spells:  %-3d                                  word_recall:     %d", py.flags.new_spells, py.flags.word_recall);
  bank_prt(out_val1, 21, 0, WIZ2NUM);

  bank_prt("press any key", 23, 32, WIZ2NUM);
  cgetc();
}
