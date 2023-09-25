;
; Christian Groessler, November-2002
;
; get a char from the keyboard
; char cgetc(void)
;

        .include "atari.inc"
        .export _cgetc
	.importzp ptr1


_cgetc:
	lda PORTB		; save portb state
	sta portbsav
	ora #1			; turn on OS
	sta PORTB		

	jsr call_cgetc		; do the actual call (ues the RTS trick) so need to get back here to disable OS again

	ldx portbsav		; key is in A register, so don't mess with it
	stx PORTB		; restore portb state
        rts

call_cgetc:
        lda     #12
        sta     ICAX1Z          ; fix problems with direct call to KEYBDV

@1:     lda     KEYBDV+5
        pha
        lda     KEYBDV+4
        pha

	ldx	#0
	rts			; shouldn't even get here, os routine should RTS back to _cgetc


	.segment "BSS"

portbsav:	.res	1

