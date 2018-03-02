	title 'vga13'

;* include g:\pc\pcmac.asm */
include tcmac.asm
modbeg vga13




;         mov    register,word ptr [bp+(number*2+4)]
;.dataseg
;vertincr	dw	0
;incr1		dw	0
;incr2		dw	0
;routine		dw	0
;rflag		db	0
;argx2		dw	0
;argx1		dw	0
;argy1		dw	0
;argy2		dw	0
;n		db	15
;vboff		dw	0
;.ends
argx1	equ	[bp+(1*2+4)]
argy1	equ	[bp+(2*2+4)]
argx2	equ	[bp+(3*2+4)]
argy2	equ	[bp+(4*2+4)]
n	equ	[bp+(5*2+4)]
dirtyo	equ	[bp+(6*2+4)]
dirtys	equ	[bp+(7*2+4)]
vertincr	equ	[bp-6]
incr1		equ	[bp-8]
incr2		equ	[bp-10]
routine		equ	[bp-12]

procbeg	txmode
	mov	ax,3h		; use mode 10h
	int	10h
procend	txmode

procbeg	grmode
	mov	ax,0013h
	int	10h
procend	grmode

;            WRTGLINE(xsize,startoffset, dataptr );
; xsize is size in bytes of image line to draw
; startoffset is the offset into the video buffer to begin the write
; dataptr is a near pointer to the raw 8-bit screen data


procbeg	wrtgline
;	jmp	donoth
	mov	cx,argx1
	mov	ax,0a000h
	mov	es,ax
	mov	si,argx2
	mov	di,argy1
	rep	movsb
donoth:
procend	wrtgline

modend vga13
