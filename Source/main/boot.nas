;
;	File: Source/main/boot.nas
;	Copyright: MLworkshop
;	Author: Msild
;

; MitOS boot asm
; TAB=4

[INSTRSET "i486p"]

VBEMODE	EQU		0x112

;  画面模式号    画面分辨率    画面颜色数
;	0x100		640*400			8
;	0x101		640*480			8
;	0x102		640*480			16
;	0x103		800*600			8
;	0x104		1024*768		16
;	0x105		1024*768		8
;	0x106		1280*1024		16
;	0x107		1280*1024		8
;	0x10f		320*200			24
;	0x112		640*480			24
;	0x115		800*600			24
;	0x118		1024*768		24
;	0x11b		1280*1024		24

MAIN	EQU		0x00280000		; main 加载目标
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

; 清屏

clear:
		MOV		AH,0x06
		MOV		AL,0x00
		MOV		CH,0x00
		MOV		CL,0x00
		MOV		DH,0x00
		MOV		DL,0x00
		MOV		BH,0x07
		INT		0x10

; 确认VBE存在

scrnVBE:
		XOR		AX,AX
		MOV		DS,AX
		MOV		ES,AX
		MOV		DI,0x0800
		MOV		AX,0x4f00		; 使用 VBE 标准
		INT		0x10
		CMP		AX,0x004f		; AX=0x004f 表示 VBE 存在
		JNE		scrn320
		MOV		AX,[ES:DI+4]	; 检查 VBE 版本
		CMP		AX,0x0200		; VBE 版本需在 2.0 及以上
		JB		scrn320

; 获取画面模式信息

		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AL,0x004f		; 是否支持此画面模式
		JNE		scrn320

; 画面模式确认

		; CMP		BYTE [ES:DI+0x19],8
		; JNE		scrn320
		; CMP		BYTE [ES:DI+0x1b],4
		; JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320			; 模式属性 bit7 为 0，放弃

; 画面模式切换

		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10

; 记录画面模式
		MOV		AX,[ES:DI+0x19]
		MOV		[VMODE],AX
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

; 通过 BIOS 获取键盘 LED 状态

keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PIC 关闭一切中断
; 进行 PIC 初始化后 CLI

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 暂停 CPU
		OUT		0xa1,AL

		CLI						; 禁止 CPU 级别中断

; 让 CPU 访问 1MiB 以上的内存空间，设定 A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; 切换到保护模式

		LGDT	[GDTR0]			; 设置临时 GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设 bit31 为 0，禁止分页
		OR		EAX,0x00000001	; 设 bit0 为 1，切换到保护模式
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  可读写的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; main 的传送

		MOV		ESI,main	; 传送源
		MOV		EDI,MAIN		; 传送目的地
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
		SUB		ECX,512/4		; 减去 IPL
		CALL	memcpy

; asmhead 完毕，剩余由 main 完成

; main 启动

		MOV		EBX,MAIN
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
		JNZ		waitkbdout		; AND 非 0，跳转至 waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; SUB 非 0，跳转至 memcpy
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
main:
