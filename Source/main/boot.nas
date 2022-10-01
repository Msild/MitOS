;
;	File: Source/main/boot.nas
;	Copyright: MLworkshop
;	Author: Msild
;

; MitOS boot asm
; TAB=4

[INSTRSET "i486p"]

VBEMODE	EQU		0x100			; 640*400 8位彩色
; 画面模式阅览
;	0x100 :   640*400 8位彩色
;	0x101 :   640*480 8位彩色
;	0x103 :   800*600 8位彩色
;	0x105 :  1024*768 8位彩色
;	0x107 : 1280*1024 8位彩色

BOTPAK	EQU		0x00280000		; bootpack加载目标
DSKCAC	EQU		0x00100000		; 磁盘缓存地址
DSKCAC0	EQU		0x00008000		; 磁盘缓存地址（实时模式）

; BOOT_INFO信息
CYLS	EQU		0x0ff0			; 引导扇区设定
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 颜色数信息
SCRNX	EQU		0x0ff4			; 分辨率 X
SCRNY	EQU		0x0ff6			; 分辨率 Y
VRAM	EQU		0x0ff8			; 图像缓冲区起始地址

		ORG		0xc200			; 程序加载位置

; 显示信息

info:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		scrnVBE
		MOV		AH,0x0e			; 显示字符函数
		MOV		BX,15			; 颜色代码
		INT		0x10			; 调用BIOS显示
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; 换行两次
		DB		"Start MitOS..."
		DB		0x0a			; 换行
		DB		0

; 确认VBE存在

scrnVBE:
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 检查VBE版本

		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320

; 获取画面模式信息

		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 画面模式信息确认

		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320			; 模式属性bit7为0，放弃

; 画面模式切换

		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; 记录画面模式
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGA 320*200 8位彩色
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 记录画面模式
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 通过BIOS获取键盘LED状态

keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PIC关闭一切中断
; 进行PIC初始化后CLI

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 暂停CPU
		OUT		0xa1,AL

		CLI						; 禁止CPU级别中断

; 让CPU访问1MB以上的内存空间，设定A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; 切换到保护模式

		LGDT	[GDTR0]			; 设置临时GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设bit31为0，禁止分页
		OR		EAX,0x00000001	; 设bit0为1，切换到保护模式
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  可读写的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的传送

		MOV		ESI,bootpack	; 传送源
		MOV		EDI,BOTPAK		; 传送目的地
		MOV		ECX,512*1024/4
		CALL	memcpy

; 磁盘数据传送的本来的位置

; 从启动区开始

		MOV		ESI,0x7c00		; 传送源
		MOV		EDI,DSKCAC		; 传送目的地
		MOV		ECX,512/4
		CALL	memcpy

; 剩余全部

		MOV		ESI,DSKCAC0+512	; 传送源
		MOV		EDI,DSKCAC+512	; 传送目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 柱面到字节 / 4
		SUB		ECX,512/4		; 减去IPL
		CALL	memcpy

; asmhead完毕，剩余由bootpack完成

; bootpack启动

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 无需传送
		MOV		ESI,[EBX+20]	; 传送源
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 传送目的地
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 栈初始值
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		IN		 AL,0x60 		; 空读
		JNZ		waitkbdout		; AND非0，跳转至waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; SUB非0，跳转至memcpy
		RET

		ALIGNB	16
GDT0:
		RESB	8
		DW		0xffff,0x0000,0x9200,0x00cf	; 可读写的段 32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可执行的段 32bit

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
