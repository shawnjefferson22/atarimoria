
#ifndef VBXE_H
#define VBXE_H

// define some aliases for read register names
#define CORE_VERSION        VIDEO_CONTROL
#define MINOR_VERSION       XDL_ADR0
#define COLDETECT           COLCLR
#define BLT_COLLISION_CODE  BL_ADR0
#define BLITTER_BUSY        BLITTER_START
#define IRQ_STATUS          IRQ_CONTROL


typedef struct _VBXE_REGS {
  unsigned char VIDEO_CONTROL;
  unsigned char XDL_ADR0;
  unsigned char XDL_ADR1;
  unsigned char XDL_ADR2;
  unsigned char CSEL;
  unsigned char PSEL;
  unsigned char CR;
  unsigned char CG;
  unsigned char CB;
  unsigned char COLMASK;
  unsigned char COLCLR;
  unsigned char _unused0[5];
  unsigned char BL_ADR0;
  unsigned char BL_ADR1;
  unsigned char BL_ADR2;
  unsigned char BLITTER_START;
  unsigned char IRQ_CONTROL;
  unsigned char P0;
  unsigned char P1;
  unsigned char P2;
  unsigned char P3;
  unsigned char _unused1[4];
  unsigned char MEMAC_B_CONTROL;
  unsigned char MEMAC_CONTROL;
  unsigned char MEMAC_BANK_SEL;
} _VBXE_REGS;


//* XDL
//
// Order of XDL data (if required)
//
// XDLC_RPTL    (1 byte)
// XDLC_OVADR   (5 bytes)
// XDLC_OVSCRL  (2 bytes)
// XDLC_CHBASE  (1 byte)
// XDLC_MAPADR  (5 bytes)
// XDLC_MAPPAR  (4 bytes)
// XDLC_OVATT   (2 bytes)

#define XDLC_TMON       0x0001
#define XDLC_GMON       0x0002
#define XDLC_OVOFF      0x0004
#define XDLC_MAPON      0x0008
#define XDLC_MAPOFF     0x0010
#define XDLC_RPTL       0x0020
#define XDLC_OVADR      0x0040
#define XDLC_OVSCRL     0x0080
#define XDLC_CHBASE     0x0100
#define XDLC_MAPADR     0x0200
#define XDLC_MAPPAR     0x0400
#define XDLC_OVATT      0x0800
#define XDLC_HR         0x1000
#define XDLC_LR         0x2000
// 0x4000 is reserved
#define XDLC_END        0x8000


extern _VBXE_REGS *vbxe;
#pragma zpsym("vbxe")


// in vbxe.s
unsigned int __fastcall__ vbxe_detect(void);


#endif