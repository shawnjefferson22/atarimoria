#include <stdio.h>
#include <stdlib.h>
#include <atari.h>
#include <string.h>
#include "vbxe.h"
#include "vbxebank.h"



#pragma code-name("CODE")
#pragma data-name("DATA")


//unsigned char old_vbxe_bank;			// in zeropage -SJ


void __fastcall__ vbxe_bank(unsigned char bank) {
  old_vbxe_bank = vbxe->MEMAC_BANK_SEL;

  vbxe->MEMAC_CONTROL = 0x4B;       // $4000, CPU Enable, 32K banks
  vbxe->MEMAC_BANK_SEL = bank;
  return;
}


void vbxe_restore_bank(void) {
  vbxe->MEMAC_BANK_SEL = old_vbxe_bank;
  return;
}
