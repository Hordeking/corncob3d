
;    Copyright Ad Lib Inc., 1990

;    This file is part of the Ad Lib Programmer's Manual product and is
;    subject to copyright laws.  As such, you may make copies of this file
;    only for the purpose of having backup copies.  This file may not be
;    redistributed in any form whatsoever.

;    If you find yourself in possession of this file without having received
;    it directly from Ad Lib Inc., then you are in violation of copyright
;    laws, which is a form of theft.


; OUTCHIP.ASM

; Adlib Inc, 20-avr-89

.MODEL LARGE

; @codesize is predefined by MASM: 0 = small, compact; 1 = med, large, huge
IF  @codesize
   CPSIZE  EQU 4
ELSE
   CPSIZE  EQU 2
ENDIF

extrn   _genAddr: word        ; address of sound chip.

; ========================= DATA SEGMENT =========================
.DATA



;========================== CODE SEGMENT ============================
.CODE


; void SndOutput (addr, dataVal)
;	int addr, dataVal;
;
;	Set the register at address 'addr' to the value 'data'.
;	Take care of data register to data register write delay.

public _SndOutput

_SndOutput proc

SO_F struc
	dw	(?)	; old bp
	db	CPSIZE DUP (?)	; return addr
addr	dw	(?)	; register adress in sound chip
dataVal	dw	(?)	; data to be written
SO_F ends

; AX & DX lost.

	push	bp
	mov	bp, sp

	mov dx, _genAddr            ; get address register of sound chip
	mov ax, [bp].addr
	out	dx, al

; 12 cyles delay ( 3.6 Mhz )
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx

	inc	dx		; get data reg. of sound chip
	mov ax, [bp].dataVal
	out	dx, al
	dec dx

; 84 cyles delay ( 3.6 Mhz )
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx

	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx

	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx

	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx
	in	al, dx

	pop	bp
	ret

_SndOutput endp

	end


