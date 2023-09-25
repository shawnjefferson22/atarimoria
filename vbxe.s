	    .export _vbxe_detect, _vbxe


	    .zeropage

_vbxe:      .res    2

	    .segment "INIT"

_vbxe_detect:
	    ldx #$D6        ; try $D640 first
	    ldy #$40
	    lda #0
	    sta $D640
	    lda $D640
	    cmp #$10        ; $10 = FX core
	    beq fxcore
	    inx             ; try $D740
	    sta $D740
	    lda $D740
	    cmp #$10        ; $10 = FX core
	    beq fxcore

	    lda #0
	    ldx #0          ; no fx core
	    rts

fxcore:     tya             ; move low byte into a
	    rts

