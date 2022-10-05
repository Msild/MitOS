#
#	File: Source/main/Makefile
#	Copyright: MLworkshop
#	Author: Msild
#

BUILDPATH =
INCPATH   = ../include/

MAKE      = $(BUILDPATH)make.exe -r
NASK      = $(BUILDPATH)nask.exe
NASM      = $(BUILDPATH)nasm.exe -fcoff
MCC       = $(BUILDPATH)mcc.exe -I$(INCPATH) -Os -Wall -quiet
CONVNAS   = $(BUILDPATH)convnas.exe -a
ATTRIB    = attrib
COPY      = copy
DEL       = del

# 默认动作

.PHONY : default
default :
	$(MAKE) ipl.bin
	$(MAKE) boot.bin
	$(MAKE) main.obj

# 文件生成规则

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

boot.bin : boot.nas Makefile
	$(NASK) boot.nas boot.bin boot.lst

# 一般规则

%.gas : %.c ../MitOS.h Makefile
	$(MCC) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(CONVNAS) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

%.obj : %.asm Makefile
	$(NASM) $*.asm -o$*.obj -l$*.lst

# 命令

.PHONY : clean
clean :
	-$(DEL)    *.lst

.PHONY : mrproper
mrproper :
	$(MAKE) clean
	-$(DEL) ipl.bin
	-$(DEL) boot.bin
	-$(DEL) main.obj
