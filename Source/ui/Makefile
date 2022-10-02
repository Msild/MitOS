#
#	File: Source/ui/Makefile
#	Copyright: MLworkshop
#	Author: Msild
#

TARGET    = graphic.obj jpeg.obj sheet.obj taskbar.obj window.obj bmp.obj

BUILDPATH =
INCPATH   = ../include/

MAKE      = $(BUILDPATH)make.exe -r
NASK      = $(BUILDPATH)nask.exe
NASM       = $(BUILDPATH)nasm.exe -fcoff
MCC       = $(BUILDPATH)mcc.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK  = $(BUILDPATH)gas2nask.exe -a
MAKEFONT  = $(BUILDPATH)makefont.exe
BIN2OBJ   = $(BUILDPATH)bin2obj.exe
ATTRIB    = attrib
COPY      = copy
DEL       = del

# 默认动作

.PHONY : default
default :
	$(MAKE) $(TARGET)
	$(MAKE) ascii.obj
	$(MAKE) mchar.obj

# 文件生成规则

ascii.bin : ascii.txt Makefile
	$(MAKEFONT) ascii.txt ascii.bin

ascii.obj : ascii.bin Makefile
	$(BIN2OBJ) ascii.bin ascii.obj _ascii
	
mchar.bin : mchar.txt Makefile
	$(MAKEFONT) mchar.txt mchar.bin

mchar.obj : mchar.bin Makefile
	$(BIN2OBJ) mchar.bin mchar.obj _mchar

# 一般规则

%.gas : %.c ../MitOS.h Makefile
	$(MCC) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

%.obj : %.asm Makefile
	$(NASM) $*.asm -o$*.obj -l$*.lst

# 命令

.PHONY : clean
clean :
	-$(DEL)    *.lst
	-$(DEL)    *.bin

.PHONY : mrproper
mrproper :
	$(MAKE) clean
	-$(DEL) *.obj
