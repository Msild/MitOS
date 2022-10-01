;
;	File: MitOS/main/ipl.nas
;	Copyright: MLworkshop
;	Author: Msild
;

; MitOS-ipl
; TAB=4

CYLS	EQU		32				; 读取扇区

		ORG		0x7c00			; 读取位置

; 标椎FAT12软盘专用代码

		JMP		entry
		DB		0x90
		DB		"MITOS   "		; 启动区名称	8字节
		DW		512				; 扇区大小		512字节
		DB		1				; 簇大小		1扇区
		DW		1				; FAT起始位置	第1扇区
		DB		2				; FAT个数		2FAT
		DW		224				; 根目录大小	224项
		DW		2880			; 磁盘大小		2880扇区
		DB		0xf0			; 磁盘种类		0xf0
		DW		9				; FAT长度		9扇区
		DW		18				; 磁道扇区数	18扇区
		DW		2				; 磁头数		2磁头
		DD		0				; 隐藏分区		0
		DD		2880			; 重写磁盘大小	2880扇区
		DB		0,0,0x29		; 意义不明		固定值
		DD		0xffffffff		; 卷标号码		0xffffffff
		DB		"MITOS      "	; 磁盘名称		11字节
		DB		"FAT12   "		; 磁盘格式名称	8字节
		RESB	18				; 空出18字节

; 程序主体

entry:
		MOV		AX,0			; 寄存器初始化
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; 显示信息

info:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msgl
putloopl:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		read
		MOV		AH,0x0e			; 显示字符函数
		MOV		BX,15			; 颜色代码
		INT		0x10			; 调用BIOS显示
		JMP		putloopl
msgl:
		DB		0x0a, 0x0a		; 换行两次
		DB		"Loading MitOS..."
		DB		0x0a			; 换行
		DB		0

; 读取软盘

read:
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 0柱面
		MOV		DH,0			; 0磁头
		MOV		CL,2			; 2扇区
		MOV		BX,18*2*CYLS-1	; 读取扇区总数
		CALL	readfast		; 高速读取

; 检查MitOS.sys
		MOV		BYTE AX,[0xc200]
		CMP		AX,0xb8
		JNE		corruption

; 运行MitOS.sys

		MOV		BYTE [0x0ff0],CYLS	; IPL实际读取数量
		JMP		0xc200

fin:
		HLT						; 暂停CPU
		JMP		fin				; 无限循环

error:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msge
putloope:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示字符函数
		MOV		BX,15			; 颜色代码
		INT		0x10			; 调用BIOS显示
		JMP		putloope
msge:
		DB		0x0a, 0x0a		; 换行两次
		DB		"Load Error."
		DB		0x0a			; 换行
		DB		0

corruption:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msgc
putloopc:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示字符函数
		MOV		BX,15			; 颜色代码
		INT		0x10			; 调用BIOS显示
		JMP		putloopc
msgc:
		DB		0x0a, 0x0a		; 换行两次
		DB		"System Corruption."
		DB		0x0a			; 换行
		DB		0

readfast:	; 使用AL一次性读取数据
;	ES:读取地址 CH:柱面, DH:磁头, CL:扇区, BX:读取扇区数

		MOV		AX,ES			; < 通过ES计算AL最大值 >
		SHL		AX,3			; AX除以32，存入AH
		AND		AH,0x7f			; AH %= 128;
		MOV		AL,128			; AL = 128 - AH;
		SUB		AL,AH

		MOV		AH,BL			; < 通过BX计算AL最大值，存入AH >
		CMP		BH,0			; if (BH != 0) { AH = 18; }
		JE		.skip1
		MOV		AH,18
.skip1:
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip2
		MOV		AL,AH
.skip2:

		MOV		AH,19			; < 通过CL计算AL最大值，存入AH >
		SUB		AH,CL			; AH = 19 - CL;
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip3
		MOV		AL,AH
.skip3:

		PUSH	BX
		MOV		SI,0			; 计算失败次数的寄存器
retry:
		MOV		AH,0x02			; AH=0x02 : 读取磁盘
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	AX
		INT		0x13			; 调用BIOS磁盘中断
		JNC		next			; 无错误跳转至next
		ADD		SI,1			; SI += 1;
		CMP		SI,5			; 将SI于5比较
		JAE		error			; SI >= 5 跳转至error
		MOV		AH,0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 重置驱动器
		POP		AX
		POP		CX
		POP		DX
		POP		ES
		JMP		retry
next:
		POP		AX
		POP		CX
		POP		DX
		POP		BX				; 将ES的内容存入BX
		SHR		BX,5			; 将BX由16字节转为512字节
		MOV		AH,0
		ADD		BX,AX			; BX += AL;
		SHL		BX,5			; 将BX由512字节转为16字节
		MOV		ES,BX			; 相当于 EX += AL * 0x20
		POP		BX
		SUB		BX,AX
		JZ		.ret
		ADD		CL,AL			; CL += AL;
		CMP		CL,18			; 将CL与18比较
		JBE		readfast		; CL <= 18 跳转至readfast
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readfast		; DH < 2 跳转至readfast
		MOV		DH,0
		ADD		CH,1
		JMP		readfast
.ret:
		RET

		RESB	0x7dfe-$		; 到0x7dfe为止填充0x00

		DB		0x55, 0xaa
