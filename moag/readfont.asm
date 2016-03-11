	title 'readfont'

;* include g:\pc\pcmac.asm */
modbeg  macro   modname
        name    modname

L_TEXT	segment	byte public 'CODE'
        assume  cs:l_text
        endm

modend       macro
L_TEXT       ends
             end
             endm

argx1	equ	[bp+(1*2+4)]
argy1	equ	[bp+(2*2+4)]

modbeg readfont

		public	_readfont
_readfont	proc	far
		push	bp
		mov	bp,sp
		push	di
		push	si
		push	es
		mov	ax,ds
		mov	es,ax
		mov	ax,1100h
		mov	bh,16
		mov	bl,0
		mov	dx,argx1
		mov	bp,argy1
		mov	cx,1
		int	10h
		pop	es
		pop	si
		pop	di
		pop	bp
		ret
_readfont	endp


modend readfont
