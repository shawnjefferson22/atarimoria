#ifndef VBXETEXT_H
#define VBXETEXT_H


void __fastcall__ vbxe_init(void);
void __fastcall__ vbxe_cputsxy(unsigned char x, unsigned char y, char *str, unsigned char atr);
void __fastcall__ vbxe_cputcxy(unsigned char x, unsigned char y, char c, unsigned char atr);
void __fastcall__ vbxe_clear(void);
void __fastcall__ vbxe_clearxy(unsigned char x, unsigned char y, unsigned char len);
void __fastcall__ vbxe_cleartoeol(unsigned char x, unsigned char y);
void __fastcall__ vbxe_cleartobot(unsigned char y);
void __fastcall__ vbxe_savescreen(void);
void __fastcall__ vbxe_restorescreen(void);


#endif
