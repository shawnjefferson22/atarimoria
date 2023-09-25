;
; Startup code for cc65 (ATARI version)
; Modifed for cartridge, Shawn Jefferson 08/15/2008
;
; Contributing authors:
;       Mark Keates
;       Freddy Offenga
;       Christian Groessler
;
; This must be the *first* file on the linker command line
;

	.export         _exit
	.export         __STARTUP__ : absolute = 1     ; mark as startup
	.constructor    initsp, 26

	.import         initlib, donelib, callmain
	.import         zerobss, pushax
	.import         copysegs
	.import         _main
	.import         __STARTUP_LOAD__, __BSS_LOAD__
	.import         __CODE_LOAD__
	.import         __RESERVED_MEMORY__
	.import         __STACK_START__, __STACK_SIZE__
.ifdef  DYNAMIC_DD
	.import         __getdefdev
.endif

	.include        "zeropage.inc"
	.include        "atari.inc"
	.include        "_file.inc"

;CONSOL = $D10F


; ------------------------------------------------------------------------
; EXE header
; Not needed for cartridge

;        .segment "EXEHDR"
;      .word   $FFFF
;        .word   __LOWCODE_LOAD__
;        .word   __BSS_LOAD__ - 1

; ------------------------------------------------------------------------
; Actual code

	.segment        "STARTUP"

	rts     ; fix for SpartaDOS / OS/A+
		; they first call the entry point from AUTOSTRT and
		; then the load addess (this rts here).
		; We point AUTOSTRT directly after the rts.

; Real entry point:

; Check for cartridge bypass

		lda CONSOL
		and #$04		
		bne @1
		rts

; Copy the library segements to RAM

@1:
	 jsr    copysegs

; Clear the BSS data

	jsr     zerobss

; Call module constructors

	jsr     initlib

; set left margin to 0

	lda     #0
	sta     LMARGN

; set keyb to upper/lowercase mode

	sta     SHFLOK

; Initialize conio stuff

	lda     #$FF
	sta     CH

; Push arguments and call main

	jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

END:     jmp END            ; loop forever

; *** end of main startup code

; setup sp

.segment        "INIT"

initsp:
	lda     #<(__STACK_START__ + __STACK_SIZE__ - 1)
	sta     sp
	lda     #>(__STACK_START__ + __STACK_SIZE__ - 1)
	sta     sp+1

	rts


; Cartridge vectors at $BFFA
;
	.segment "VECTORS"

	; Start Address
	.word __STARTUP_LOAD__+1
	; Cart present
	.byte $00
	; Init & Start
	;.byte $04
	.byte $05
	; Init Address
	.word __STARTUP_LOAD__
