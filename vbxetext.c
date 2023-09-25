#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <conio.h>
#include <string.h>
#include "vbxe.h"
#include "vbxetext.h"
#include "vbxebank.h"


#define SAVMSC	*(unsigned int *) 0x58			// atari screen address
#define SDMCTL *(unsigned char *) 0x22F     	// Antic DMA control shadow

#define bank 0x4000								// vbxe memory bank location


#pragma code-name("BANK7F")
#pragma data-name("BANK7F")
#pragma rodata-name("BANK7F")


extern unsigned char vbxe_save_bank;			// track old vbxe banks seperately from vbxe_bank code
#pragma zpsym("vbxe_save_bank")

extern char *vbxe_scrnptr;
#pragma zpsym("vbxe_scrnptr");


/* VBXE REQUIRED text in internal screen codes */
const unsigned char vbxe_missing_text[13] = {54, 34, 56, 37, 0, 50, 37, 49, 53, 41, 50, 37, 36};

/* VBXE XDL */
#define XDLC    (XDLC_TMON+XDLC_RPTL+XDLC_OVADR+XDLC_CHBASE+XDLC_OVATT+XDLC_END)

const char xdl[] = {(XDLC % 256), (XDLC / 256), 215, 0x20, 0x0E, 0x00, 160, 0, 1, 1, 255 };

/* Characters to redefine */
const unsigned char paren1[8] = {0x0E, 0x18, 0x18, 0x60, 0x18, 0x18, 0x0E, 0x00};
const unsigned char paren2[8] = {0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00};
const unsigned char tilde[8]  = {0x00, 0x34, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char block[8]  = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};



void __fastcall__ vbxe_init(void) {
  vbxe = (_VBXE_REGS *) vbxe_detect();
  if (!vbxe) {
	memcpy((void *)SAVMSC, &vbxe_missing_text, 13);			// VBXE REQUIRED
	SDMCTL = 0x22;											// turn DMA back on
    while(1);												// lockup
  }

  // clear screen bank, bank 1
  vbxe->VIDEO_CONTROL = 0x00;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)
  memset((unsigned char *) 0x4000, 0, 0x1000);

  // font and XDL bank
  vbxe->MEMAC_BANK_SEL = 0x80;      // bank zero

  // copy font to $0800 VBXE memory, bank 0
  memcpy((unsigned char *)0x4800, (unsigned char*)0xE000, 1024); //

  // redefine some chars
  memcpy((unsigned char *) 0x4800+(8*80), &block, 8);
  memcpy((unsigned char *) 0x4800+(8*123), &paren1, 8);
  memcpy((unsigned char *) 0x4800+(8*125), &paren2, 8);
  memcpy((unsigned char *) 0x4800+(8*126), &tilde, 8);
  memcpy((unsigned char *) 0x4800+(8*127), &block, 8);

  memset((unsigned char *) 0x4E20, 0, 0x1E0);				// FIXME: why clearing this? -SJ

  // copy XDL to VBXE memory, bank 0
  memcpy((unsigned char *)0x4000, xdl, 11);

  // turn off vbxe bank access
  vbxe->MEMAC_BANK_SEL = 0x0;

  // set XDL address
  vbxe->XDL_ADR0 = 0x00;
  vbxe->XDL_ADR1 = 0x00;
  vbxe->XDL_ADR2 = 0x00;

  // turn on XDL processing
  vbxe->VIDEO_CONTROL = 0x01;

  return;
}


#pragma code-name("PRINTBANK")
#pragma rodata-name("PRINTBANK")


const unsigned int vrow_lookup[24] = {0x4000, 0x40A0, 0x4140, 0x41E0, 0x4280, 0x4320, 0x43C0, 0x4460, 0x4500, 0x45A0, 0x4640, 0x46E0,
									  0x4780, 0x4820, 0x48C0, 0x4960, 0x4A00, 0x4AA0, 0x4B40, 0x4BE0, 0x4C80, 0x4D20, 0x4DC0, 0x4E60};



void __fastcall__ vbxe_cputsxy(unsigned char x, unsigned char y, char *str, unsigned char atr) {
  register unsigned char i, s, c;


  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;	// save old vbxe bank
  vbxe->MEMAC_CONTROL = 0x48;       		// 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;				// bank 0x81 (text screen bank)


  //vbxe_scrnptr = (unsigned char *) (vrow_lookup[y] + (x*2));
  vbxe_scrnptr = (char *) (vrow_lookup[y] + (x*2));
  s = strlen(str);

  /*for(j=0,i=0; i<s; ++i,j+=2) {
    c = str[i];
    if (c > 95)
      vbxe_scrnptr[j] = c;
    else if (c > 31)
      vbxe_scrnptr[j] = c - 32;
    else
      vbxe_scrnptr[j] = c + 64;

    vbxe_scrnptr[j+1] = atr;
  }*/

  for(i=0; i<s; ++i) {
    c = str[0];
    if (c > 95)
      vbxe_scrnptr[0] = c;
    else if (c > 31)
      vbxe_scrnptr[0] = c - 32;
    else
      vbxe_scrnptr[0] = c + 64;

    vbxe_scrnptr[1] = atr;
    vbxe_scrnptr += 2;
    str++;
  }

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;
  return;
}


void __fastcall__ vbxe_cputcxy(unsigned char x, unsigned char y, char c, unsigned char atr) {
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;
  //vbxe_scrnptr = (unsigned char *) (vrow_lookup[y] + (x*2));
  vbxe_scrnptr = (char *) (vrow_lookup[y] + (x*2));


  if (c > 95)
    vbxe_scrnptr[0] = c;
  else if (c > 31)
    vbxe_scrnptr[0] = c - 32;
  else
    vbxe_scrnptr[0] = c + 64;

  vbxe_scrnptr[1] = atr;

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}


void __fastcall__ vbxe_clear(void) {
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)

  memset((unsigned char *) 0x4000, 0, 0xF00);

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}


void __fastcall__ vbxe_clearxy(unsigned char x, unsigned char y, unsigned char len)
{
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)

  memset((unsigned char *)(vrow_lookup[y]+(x*2)), 0, len*2);

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}


void __fastcall__ vbxe_cleartoeol(unsigned char x, unsigned char y)
{
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)

  memset((unsigned char *)(vrow_lookup[y]+(x*2)), 0, (80-x)*2);

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}


void __fastcall__ vbxe_cleartobot(unsigned char y)
{
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)

  memset((unsigned char *) vrow_lookup[y], 0, (24-y)*160);

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}


void __fastcall__ vbxe_savescreen(void) {
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;

  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)

  memcpy((unsigned char *) 0x5000, (unsigned char *) 0x4000, 0x1000);			// $5000 is in main ram!

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}


void __fastcall__ vbxe_restorescreen(void) {
  vbxe_save_bank = vbxe->MEMAC_BANK_SEL;
  vbxe->MEMAC_CONTROL = 0x48;       // 4k banks, CPU only, $4000
  vbxe->MEMAC_BANK_SEL = 0x81;      // bank 1 ($1000)

  memcpy((unsigned char *) 0x4000, (unsigned char *) 0x5000, 0x1000);			// $5000 is in main ram!

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = vbxe_save_bank;

  return;
}
