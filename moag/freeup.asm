	title 'freeup'

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

modbeg freeup

		public	_freeup
_freeup	proc	far
		push	bp
		mov	bp,sp
		push	di
		push	si
		push	es
		xor	dx,dx	; zero bytes
fuagain:
		push	dx
		mov	ah,52h	; unknown function call to DOS to get
		int	21h	; 'list of lists'
		mov	ax,es:[bx-2]	; This should be a ptr to 1st memblk 
		pop	dx
fuloop:
		mov	es,ax
		mov	al,es:[0]	; check for valid mem block
		cmp	al,4dh
		jz	niceblock
		cmp	al,5ah
		jz	lastblock
		mov	ax,-1
furet:
		pop	es
		pop	si
		pop	di
		pop	bp
		ret
niceblock:
		mov	ax,es:[1]	; get owner of block
		mov	bx,es:[3]	; get size of block
		cmp	ax,argx1	; compare to desired owner
		jnz	notown
		add	dx,bx
		mov	ax,es
		inc	ax
		mov	es,ax		; must point to data for de-alloc
		mov	ah,49h
		int	21h		; free the ram
		mov	ax,-1		; error flag
		jc	furet
		jmp	fuagain		; restart scanning the mem chain
notown:
		mov	ax,es
		add	ax,bx		; advance to next block in chain
		inc	ax		; skip over this mcb info
		loop	fuloop
lastblock:
		mov	ax,es:[1]	; get owner of block
		mov	bx,es:[3]	; get size of block
		cmp	ax,argx1	; compare to desired owner
		jnz	notown1
		add	dx,bx
		mov	ax,es
		inc	ax
		mov	es,ax		; must point to data for de-alloc
		mov	ah,49h
		int	21h		; free the ram
		mov	ax,-1		; error flag
		jc	furet
		jmp	fuagain		; restart scanning the mem chain
notown1:
		mov	ax,dx		; return result to c
		jmp	furet
		
_freeup	endp


modend freeup
