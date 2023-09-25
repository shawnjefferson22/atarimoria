#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <string.h>
#include <conio.h>
#include "vbxetext.h"
#include "vbxebank.h"
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


#define SDMCTL *(unsigned char *)	0x22F     	// Antic DMA control shadow
#define NOCLIK *(unsigned char *)	0x2DB    	// keyclick control
#define KEYREP *(unsigned char *)	0x2DA		// key repeat rate

extern unsigned char _BCODE_RUN__;
extern void _BCODE_SIZE__;
extern unsigned char _BCODE1_RUN__;
extern void _BCODE1_SIZE__;
extern unsigned char _BCODE2_RUN__;
extern void _BCODE2_SIZE__;



#pragma code-name("INIT")
#pragma rodata-name("PRINTBANK")                                // put the RO text into the print bank


void __fastcall__ intro(void);
void __fastcall__ copy_banking_code(void);
void __fastcall__ copy_vbxe_banks(void);
void __fastcall__ bank_moria_init(void);
int8u __fastcall__ check_char(void);

char title[] = "Atari Moria v5.5 beta4";

void main(void) {
  int8u i;

  SDMCTL = 0;                                                   // Turn off ANTIC DMA
  NOCLIK = 255;													// Turn off keyclick
  KEYREP = 4;													// Keyrepeat rate

  vbxe_init();                                                  // Initialize VBXE (copies charset from OS)
  old_vbxe_bank = VBXE_SPLBANK;                                 // VBXE bank to bring back

  IntHandlerInit();												// Turn off OSRAM and init Interrupt Handler
  copy_banking_code();											// copy the banking functions

  bank_vbxe_cputsxy(0, 0, title, 0x0F, STARTUPNUM);
  bank_vbxe_cputsxy(0, 2, "Copying data to VBXE banks...", 0x0F, STARTUPNUM);
  copy_vbxe_banks();                                            // Copy data from cart to VBXE banks

  intro();

  vbxe_bank(VBXE_CAVBANK);										// build cave row lookup table
  for (i=0; i<MAX_HEIGHT; ++i)
    crow_lookup[i] = (int16u) &cave + ((int16u)i * (MAX_WIDTH * sizeof(cave_type)));

  bank_moria_init();                            				// This is in another bank, call a function in low memory to call it
  return;
}


void __fastcall__ intro(void)
{
  char ch;


  bank_vbxe_clear(STARTUPNUM);

  bank_vbxe_cputsxy(29, 0, title, 0x0F, STARTUPNUM);
  bank_vbxe_cputsxy(3, 8, "The Dwarves tell no tale; but even as mithril was the foundation of their", 0x0D, STARTUPNUM);
  bank_vbxe_cputsxy(3, 9, "wealth, so also it was their destruction: they delved too greedily and too", 0x0C, STARTUPNUM);
  bank_vbxe_cputsxy(8, 10, "deep, and disturbed that from which they fled, Durin's Bane.", 0x0B, STARTUPNUM);

  if (!check_char()) {
    bank_vbxe_cputsxy(20, 22, "Press Any Key to Enter the Mines of Moria",  0x0F, STARTUPNUM);
    bank_vbxe_cputsxy(27, 23, "or Press [c] for Classic Moria", 0x0F, STARTUPNUM);

    ch = cgetc();
    if (ch == 'c' || ch == 'C')
      classic_moria = TRUE;
    else
      classic_moria = FALSE;
  }
  else {
    bank_vbxe_cputsxy(18, 22, "Press Any Key to Resume Playing your Character", 0x0F, STARTUPNUM);
    cgetc();
  }

  return;
}


/* This code needs to be in low memory to copy memory into banking areas */
#pragma code-name("CODE")


void __fastcall__ copy_banking_code(void)
{
  // copying banking code into low memory
  BANKCODE0BANK;
  memcpy(&_BCODE_RUN__, (unsigned char *) 0xA000, (unsigned int) &_BCODE_SIZE__);

  // copying banking code into memory under OS
  BANKCODE1BANK;
  memcpy(&_BCODE1_RUN__, (unsigned char *) 0xA000, (unsigned int) &_BCODE1_SIZE__);
  //STARTUPBANK;

  BANKCODE2BANK;
  memcpy(&_BCODE2_RUN__, (unsigned char *) 0xA000, (unsigned int) &_BCODE2_SIZE__);
  STARTUPBANK;
}


void __fastcall__ copy_vbxe_banks(void) {
  // Copy monster bank
  vbxe_bank(VBXE_MONBANK);
  asm("sta $D501");
  memcpy((unsigned char *) 0x4000, (unsigned char *) 0xA000, 0x2000);
  asm("sta $D502");
  memcpy((unsigned char *) 0x6000, (unsigned char *) 0xA000, 0x2000);

  // Copy object bank
  vbxe_bank(VBXE_OBJBANK);
  asm("sta $D503");
  memcpy((unsigned char *) 0x4000, (unsigned char *) 0xA000, 0x2000);
  asm("sta $D504");
  memcpy((unsigned char *) 0x6000, (unsigned char *) 0xA000, 0x2000);
  asm("sta $D505");
  memcpy((unsigned char *) 0x8000, (unsigned char *) 0xA000, 0x2000);

  // Copy spell/player bank
  vbxe_bank(VBXE_SPLBANK);
  asm("sta $D506");
  memcpy((unsigned char *) 0x4000, (unsigned char *) 0xA000, 0x2000);
  asm("sta $D507");
  memcpy((unsigned char *) 0x6000, (unsigned char *) 0xA000, 0x2000);
  asm("sta $D508");
  memcpy((unsigned char *) 0x8000, (unsigned char *) 0xA000, 0x2000);

  // Copy magic bank
  vbxe_bank(VBXE_MAGBANK);
  asm("sta $D509");
  memcpy((unsigned char *) 0x4000, (unsigned char *) 0xA000, 0x2000);

  STARTUPBANK;
}


/* Check if the character is dead or alive -SJ */
int8u __fastcall__ check_char(void)
{
  CHARSAVE0BANK;

  /* Is Character dead? */
  if (save_death) {
    STARTUPBANK;
    return(FALSE);
  }
  else {
	STARTUPBANK;
	return(TRUE);
  }
}

