		.export _flash_erase_bank, _flash_prog_bank
		
		.import ldaxysp, ldax0sp
		.importzp sp, tmp1

CARBASE = $A000
ZEROBANK = $D500

		.zeropage

pollsame:	.res 1
copysrc:	.res 2
copydst:	.res 2
count:		.res 2


		.segment "CODE"
		
; Erase a flashcart sector (64k on 8Mbit cart)
; Pass the starting bank of the sector to erase
; void __fastcall__ flash_erase_bank(unsigned char bank);

_flash_erase_bank:
		pha
		jsr cmd_unlock
		lda #$80
		jsr wr5555
		jsr cmd_unlock
		pla
		tax
		sta ZEROBANK,x
		lda #$30
		sta CARBASE
		jsr poll_write 
		rts


; Program a flashcart bank (8k)
; Pass the bank, copy source and count
; void __fastcall__ flash_prog_bank(unsigned char bank, void *dst, void *src, unsigned int count);

_flash_prog_bank:
		sta count			; get count from AX
		stx count+1
		jsr _dccmp			; subtract 1 from count (required to not overflow!)
		
		jsr ldax0sp			; get source address
		sta copysrc
		stx copysrc+1

		ldy #3				; get destination address
		jsr ldaxysp
		sta copydst
		stx copydst+1
		
		ldy #4				; get bank
		lda (sp),y
		sta tmp1
		tax
		sta ZEROBANK,x
		
_prg1:  	jsr cmd_unlock 
		lda #$A0 
		jsr wr5555 
		ldx tmp1
		sta ZEROBANK,x 
		ldy #$00 
		lda (copysrc),y 
		sta (copydst),y 
		jsr decctr 
		bcs _prg1
		rts 

decctr: 	inc copysrc             ;; Increment the source pointer
		bne _dcdst
		inc copysrc+1
_dcdst: 	inc copydst
		bne _dccmp
		inc copydst+1
_dccmp:		sec
		lda count
		sbc #$01
		sta count
		lda count+1
		sbc #$00
		sta count+1
		rts

		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Send unlock sequence
;;

cmd_unlock:     lda #$AA 
		jsr wr5555 
		lda #$55
		jmp wr2AAA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Shortcut to write a few commonly used 17-bit addresses
;;

wr5555: 	sta $d542 
		sta $b555
		rts 
wr2AAA: 	sta $d541 
		sta $aaaa 
		rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Poll for flash write operation to complete
;;

poll_write:     lda #$00 
		sta pollsame 
_poll_again:    lda CARBASE 
		cmp CARBASE 
		bne poll_write 
		cmp CARBASE 
		bne poll_write 
		cmp CARBASE 
		bne poll_write 
		inc pollsame 
		bne _poll_again 
		rts 
