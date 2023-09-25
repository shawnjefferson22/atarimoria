		.export _IntHandlerInit

		.include        "atari.inc"
		
;VDSLST = $0200
;VVBLKI = $0222
;VIMIRQ = $0216
;NMIRES = $D40F
;NMIST  = $D40F
;NMIEN  = $D40E
;PORTB  = $D301


		.segment "BSS"

pbsave:		.res 1

			
		.segment "CODE"	

IntHandler:
		bit NMIST
		bpl NotDLI
		jmp (VDSLST)
NotDLI:
		pha ; return address and processor status register are already on stack (3 bytes)
		txa ; we've added a and x
		pha
		lda #>NMI ; and our return address
		pha
		lda #<NMI
		pha
		tsx
		lda $0105,x ; get processor status register as originally pushed by CPU
		pha ; push psr
		cld ; doesn't really matter about saved accumulator here
		pha ; push a, x, and y
		txa
		pha
		tya
		pha
		lda PORTB
		sta pbsave
		ora #1
		sta PORTB
		sta NMIRES
		jmp (VVBLKI)

IRQHandler:
		pha
		lda #>IRQ
		pha
		lda #<IRQ
		pha
		php
		lda PORTB
		sta pbsave
		ora #1
		sta PORTB
		jmp (VIMIRQ)

NMI:
		tsx
		lda $0103,x ; get saved processor status register
		and #$10 ; check for BRK flag
		beq NotBRK
		lda $0104,x
		sec
		sbc #2 ; drop it by 2 (back to missed BRK)
		sta $0104,x
		lda $0105,x ; get MSB of return address
		sbc #0
		sta $0105,x ; after RTI, missed BRK will now be processed
NotBRK:
		pla ; get x which we pushed earlier
		tax
IRQ:
		lda pbsave
		sta PORTB
		pla ; get a
		rti


		.segment "INIT"

_IntHandlerInit:
		lda #0
		sta NMIEN
		sei
		lda PORTB
		and #$fe ; switch out ROM
		;sta banktab ; save this value
		sta PORTB
		lda #< IntHandler
		sta $fffa
		lda #> IntHandler
		sta $fffb
		lda #< IRQHandler
		sta $fffe
		lda #> IRQHandler
		sta $ffff
		lda #96
		sta NMIEN
		cli
		rts

