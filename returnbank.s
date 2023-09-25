		.export _return_bank
		
		.segment "BCODE"
		
; handles returning to the right cartridge code bank
; void __fastcall__ return_bank(unsigned char bank);
		
_return_bank:	tax
		sta $D500,x	
		rts

