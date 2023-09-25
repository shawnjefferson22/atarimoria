;
; Ullrich von Bassewitz, 1998-12-07, 2004-12-01
;
; Copy the data segment from the LOAD to the RUN location
;
; modified to be a generic segment copy function
; just set ptr1, ptr2 and set seg size in x reg and a reg
; before calling.
;
; ptr1 is source (load address)
; ptr2 is dest (run address)
; ax is size


	.export         copyseg, copysegs
	.import         __CODE_LOAD__, __CODE_RUN__, __CODE_SIZE__
	.import         __RODATA_LOAD__, __RODATA_RUN__, __RODATA_SIZE__
	.import         __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
	.importzp       ptr1, ptr2, tmp1


	.segment "INIT"

copysegs:
	lda     #<__CODE_LOAD__         ; Source pointer
	sta     ptr1
	lda     #>__CODE_LOAD__
	sta     ptr1+1

	lda     #<__CODE_RUN__          ; Target pointer
	sta     ptr2
	lda     #>__CODE_RUN__
	sta     ptr2+1

	ldx     #<~__CODE_SIZE__
	lda     #>~__CODE_SIZE__        ; Use -(__DATASIZE__+1)

	jsr copyseg

	lda     #<__RODATA_LOAD__         ; Source pointer
	sta     ptr1
	lda     #>__RODATA_LOAD__
	sta     ptr1+1

	lda     #<__RODATA_RUN__          ; Target pointer
	sta     ptr2
	lda     #>__RODATA_RUN__
	sta     ptr2+1

	ldx     #<~__RODATA_SIZE__
	lda     #>~__RODATA_SIZE__        ; Use -(__DATASIZE__+1)

	jsr copyseg

	lda     #<__DATA_LOAD__         ; Source pointer
	sta     ptr1
	lda     #>__DATA_LOAD__
	sta     ptr1+1

	lda     #<__DATA_RUN__          ; Target pointer
	sta     ptr2
	lda     #>__DATA_RUN__
	sta     ptr2+1

	ldx     #<~__DATA_SIZE__
	lda     #>~__DATA_SIZE__        ; Use -(__DATASIZE__+1)

	jsr copyseg
	rts


copyseg:
	sta     tmp1
	ldy     #$00

; Copy loop

@L1:    inx
	beq     @L3

@L2:    lda     (ptr1),y
	sta     (ptr2),y
	iny
	bne     @L1
	inc     ptr1+1
	inc     ptr2+1                  ; Bump pointers
	bne     @L1                     ; Branch always (hopefully)

; Bump the high counter byte

@L3:    inc     tmp1
	bne     @L2

; Done

	rts

