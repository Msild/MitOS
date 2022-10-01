;
;	File: App/picture/bmp.asm
;	Copyright: MLworkshop
;	Author: Msild
;

section .text
[bits 32]


	struc BMP
		
.fType:		resw 1	
.fSize:		resd 1	
		resd 1	
.fOffBits:	resd 1	
		
.iSize:		resd 1	
.iWidth:	resd 1	
.iHeight:	resd 1	
.iPlanes:	resw 1	
.iBitCount:	resw 1	
.iCompression:	resd 1	
.iSizeImage:	resd 1	
.iXPPM:		resd 1	
.iYPPM:		resd 1
.iClrUsed:	resd 1	
.iClrImportant:	resd 1	
	endstruc

	struc BMPOS2
		
.fType:		resw 1	
.fSize:		resd 1	
		resd 1	
.fOffBits:	resd 1	
		
.iSize:		resd 1	
.iWidth:	resw 1	
.iHeight:	resw 1	
.iPlanes:	resw 1	
.iBitCount:	resw 1	
	endstruc



	struc CQuad
.b:	resb 1
.g:	resb 1
.r:	resb 1
	resb 1	
	endstruc

[absolute 0]	
bmpinfo:
.regs:		resd 4
.reteip:	resd 1
.env:		resd 1
.info:		resd 1
.size:		resd 1
.module:	resd 1
[absolute 0]
info:
.type:		resd 1	
		resd 1	
.width:		resd 1
.height:	resd 1
[section .text]
[global  _bmp_info]
_bmp_info:
	push ebx
	push ebp
	push esi
	push edi
	mov esi, [esp+bmpinfo.module]
	mov eax, [esp+bmpinfo.size]
	call bmpHeader
	test edi, edi
	jz .ret
	mov esi, [esp+bmpinfo.info]
	mov [esi+info.width], eax
	mov [esi+info.height], ecx
	mov [esi+info.type], edi	
	dec edi
	mov [esi+info.type+4], edi	
	inc edi
.ret:	mov eax, edi
	pop edi
	pop esi
	pop ebp
	pop ebx
	ret




bmpHeader:
	lea edx, [esi+eax]		
	xor edi, edi
	push edx
	xor edx, edx

	cmp eax, byte BMP.iSize+4
	jbe ..@ret			
	cmp word[esi],'BM'
	je .notMAC
	sub esi, byte -128
	add eax, byte -128
	pop ebx
	push eax
	cmp eax, byte BMP.iSize+4
..@ret:	jbe .ret
	cmp word[esi], 'BM'
	jne .ret
.notMAC:
	
	mov ecx, [esi +BMP.iSize]
	cmp ecx, byte 12			
	jne .MS
	  cmp eax, byte BMPOS2_size
	  jbe .ret				
	  lea ebx, [esi+ecx+14]			
	  movzx eax, word[esi+BMPOS2.iWidth]	
	  movzx ecx, word[esi+BMPOS2.iHeight]	
	  movzx ebp, word[esi+BMPOS2.iBitCount]	
	  mov dl, 3				
	jmp short .endif
.MS:	  cmp eax, byte BMP_size
	  jbe .ret				
	  lea ebx, [esi+ecx+14]
	  sub ecx,byte 40
	  jne .ret				
	  cmp ecx, [esi+BMP.iCompression]
	  jne .ret				
	  mov   eax, [esi+BMP.iWidth]		
	  mov   ecx, [esi+BMP.iHeight]		
	  movzx ebp, word[esi +BMP.iBitCount]	
	  mov dl, 4				
.endif:
	add esi, [esi +BMP.fOffBits]

	
	
	push edx
	push eax
	mul ebp				
	add eax, byte 7
	shr eax, 3			
	mov edx, eax
	add eax, byte 3			
	and al, -4			
	sub edx, eax			
	push edx
	mul ecx
	pop edx
	add esi, eax
	add esi, edx			
	cmp esi, [esp+8]		
	pop eax
	ja .ret2
	sub esi, edx			
	inc edi				
.ret2:	pop edx
.ret:	add esp, byte 4
	ret





[absolute 0]
decode:
.regs:		resd 4
.reteip:	resd 1
.env:		resd 1
.size:		resd 1
.module:	resd 1
.outputType:	resd 1
.dest:		resd 1
.skip:		resd 1
[section .text]
[global _bmp_decode]
_bmp_decode:
	push ebx
	push ebp
	push esi
	push edi
	mov esi, [esp+decode.module]
	mov eax, [esp+decode.size]
	call bmpHeader
	
	
	test edi,edi
	jz .error
	mov edi, [esp+decode.dest]
	push dword[esp+decode.outputType]
	push dword[esp+4+decode.skip]
	push ecx			
	push eax
	push edx
	mul ebp
	add eax, byte 31
	shr eax, 3
	and al, -4
	push eax
	mov edx, ebp
	mov ebp, esp
	call bmp2beta			
	add esp, byte bb.size
	mov eax, ecx
	test ecx, ecx
	jz .ret
.error:	push byte 1
	pop eax
.ret:	pop edi
	pop esi
	pop ebp
	pop ebx
	ret

[absolute -4*2]
bb:
.col0:		resd 1	
.reteip:	resd 1
.sw:		resd 1	
.paletteSize:	resd 1	
.w:		resd 1	
.h:		resd 1
.s:		resd 1
.t:		resd 1
.size: equ $-$$
[section .text]


bmp2beta:
	mov al, [ebp+bb.t]
	and al, 0x7f
	cmp al, 2
	je near buf16
	cmp al, 4
	je buf32
	mov ecx, esp		
	ret



buf32:
	dec edx
	je near .bpp1
	sub edx, byte 4-1
	je .bpp4
	sub edx, byte 8-4
	je .bpp8
	sub edx, byte 24-8
	je .bpp24
	mov ecx, esp		
	ret



.bpp24:
	
	
	
	

	.do24.1:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do24.2:
	    mov al, [esi]
	     mov [edi+3], dl
	    mov [edi], al
	     mov al, [esi+1]
	    mov [edi+1], al
	     mov al, [esi+2]
	    mov [edi+2], al
	     add esi, byte 3
	    add edi, byte 4
	     dec ecx
	  jnz .do24.2
	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do24.1
	ret




.bpp8:
	
	

	
	mov dl, 255
	mov eax, [ebp+bb.paletteSize]
	sub ebx, eax
	shl eax, 8
	add ebx, eax		
	.do8.1:
	  mov eax, [ebx]
	  sub ebx, [ebp+bb.paletteSize]
	  and eax, 0x00ffffff
	  dec edx
	  push eax
	jns .do8.1

	.do8.2:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do8.3:
	    xor eax, eax
	     add edi, byte 4
	    mov al, [esi]
	     inc esi
	    
	    mov eax, [esp+eax*4+8]
	     dec ecx
	    mov [edi-4], eax
	  jnz .do8.3
	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do8.2
	add esp, 256*4		
	ret




.bpp4:
	
	

	
	mov dl, 16
	mov eax, [ebp+bb.paletteSize]
	sub ebx, eax
	shl eax, 4
	add ebx, eax			
	.do4.1:
	  mov eax, [ebx]
	  sub ebx, [ebp+bb.paletteSize]
	  and eax, 0x00ffffff
	  dec edx
	  push eax
	jnz .do4.1

	.do4.2:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do4.3:
	    xor edx, edx
	     mov al, [esi]
	    mov dl, al
	     inc esi
	    shr dl, 4
	     and eax, byte 15
	    add edi, byte 4
	     dec ecx
	    mov edx, [esp+edx*4+8]
	     mov eax, [esp+eax*4+8]
	    mov [edi-4], edx
	     jz .wend
	    mov [edi], eax
	     add edi, byte 4
	    dec ecx
	  jnz .do4.3
.wend:	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do4.2
	add esp, 16*4		
	ret




.bpp1:
	
	

	
	mov eax, [ebx]
	add ebx, [ebp+bb.paletteSize]
	and eax, 0x00ffffff
	mov ebx, [ebx]
	and ebx, 0x00ffffff
	xor ebx, eax
	
	push eax

	.do1.1:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do1.2:
	    mov dl, [esi]
	     inc esi
	    push esi
	     mov esi, 8
	    .do1.3:
	      add edi, byte 4
	       add dl, dl
	      sbb eax, eax
	      and eax, ebx
	      xor eax, [ebp+bb.col0]
	       dec ecx
	      mov [edi-4], eax
	       jz .wend1bpp
	      dec esi
	    jnz .do1.3
	    pop esi
	  jmp short .do1.2
.wend1bpp:pop ecx
	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do1.1
	pop eax
	ret




buf16:
	dec edx
	je near .bpp1
	sub edx, byte 4-1
	je near .bpp4
	sub edx, byte 8-4
	je .bpp8
	sub edx, byte 24-8
	je .bpp24
	mov ecx, esp
	ret



.bpp24:
	
	

	.do24.1:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do24.2:
	    mov al, [esi+2]
	    shl eax, 16
	    mov ax, [esi]
	     add esi, byte 3
	    
	    
	    
	    shr ah, 2		
	     inc edi
	    shr eax, 3		
	    shl ax, 5		
	     inc edi
	    shr eax, 5		
	     dec ecx
	    mov [edi-2], ax
	  jnz .do24.2
	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do24.1
	ret




.bpp8:
	
	

	
	mov dl, 255
	mov eax, [ebp+bb.paletteSize]
	sub ebx, eax
	shl eax, 8
	add ebx, eax		
	.do8.1:
	  mov eax, [ebx]
	  sub ebx, [ebp+bb.paletteSize]
	  call .paletteConv
	  dec edx
	  push eax
	jns .do8.1

	.do8.2:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do8.3:
	    xor eax, eax
	     add edi, byte 2
	    mov al, [esi]
	     inc esi
	    
	    mov eax, [esp+eax*4+8]
	     dec ecx
	    mov [edi-2], ax
	  jnz .do8.3
	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do8.2
	add esp, 256*4		
	ret




.bpp4:
	
	

	
	mov dl, 16
	mov eax, [ebp+bb.paletteSize]
	sub ebx, eax
	shl eax, 4
	add ebx, eax			
	.do4.1:
	  mov eax, [ebx]
	  sub ebx, [ebp+bb.paletteSize]
	  call .paletteConv
	  dec edx
	  push eax
	jnz .do4.1

	.do4.2:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do4.3:
	    xor edx, edx
	     mov al, [esi]
	    mov dl, al
	     inc esi
	    shr dl, 4
	     and eax, byte 15
	    add edi, byte 2
	     dec ecx
	    mov edx, [esp+edx*4+8]
	     mov eax, [esp+eax*4+8]
	    mov [edi-2], dx
	     jz .wend
	    mov [edi], ax
	     add edi, byte 2
	    dec ecx
	  jnz .do4.3
.wend:	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do4.2
	add esp, 16*4		
	ret




.bpp1:
	
	

	
	mov eax, [ebx]
	add ebx, [ebp+bb.paletteSize]
	call .paletteConv
	push eax
	mov eax, [ebx]
	call .paletteConv
	pop ebx
	xchg eax, ebx
	xor ebx, eax
	push eax

	.do1.1:
	  sub esi, [ebp+bb.sw]	
	  push ecx
	  push esi
	  mov ecx, [ebp+bb.w]
	  .do1.2:
	    mov dl, [esi]
	     inc esi
	    push esi
	     mov esi, 8
	    .do1.3:
	      add dl, dl
	       inc edi
	      sbb eax, eax
	       inc edi
	      and eax, ebx
	      xor eax, [ebp+bb.col0]
	       dec ecx
	      mov [edi-2], ax
	       jz .wend1bpp
	      dec esi
	    jnz .do1.3
	    pop esi
	  jmp short .do1.2
.wend1bpp:
	  pop ecx
	  pop esi
	  pop ecx
	  add edi, [ebp+bb.s]
	  dec ecx
	jnz .do1.1
	pop eax
	ret

.paletteConv:
	shr ah, 2
	shr eax, 3
	shl ax, 5
	shr eax, 5
	ret

