#
#	File: Makefile
#	Copyright: MLworkshop
#	Author: Msild
#

OBJS_MAIN    = main/main.obj
OBJS_CONSOLE = console/api.obj console/arg.obj console/cmos.obj \
               console/command.obj console/console.obj console/file.obj
OBJS_DRIVE   = drive/floppy.obj
OBJS_FS      = fs/fat12.obj
OBJS_INT     = int/error.obj int/int.obj int/keyboard.obj int/mouse.obj \
               int/timer.obj
OBJS_METHOD  = method/alloca.obj method/asmfunc.obj method/crc32.obj \
               method/dsctbl.obj method/fifo.obj method/memory.obj \
               method/mtask.obj method/time.obj method/res.obj method/tek.obj
OBJS_UI      = ui/ascii.obj ui/graphic.obj ui/jpeg.obj ui/mchar.obj \
               ui/sheet.obj ui/taskbar.obj ui/window.obj ui/bmp.obj
OBJS         = $(OBJS_MAIN) $(OBJS_CONSOLE) $(OBJS_DRIVE) $(OBJS_FS) \
               $(OBJS_INT) $(OBJS_METHOD) $(OBJS_UI)

BUILDPATH = ../../Tool/build/

MAKE      = $(BUILDPATH)make.exe -r
MKBIM     = $(BUILDPATH)mkbim.exe
MKMIT     = $(BUILDPATH)mkmit.exe
EDIMG     = $(BUILDPATH)edimg.exe
IMGTOL    = $(BUILDPATH)imgtol.com
GOLIB     = $(BUILDPATH)golib00.exe
RULEFILE  = kernel.rul
ATTRIB    = attrib
COPY      = copy
DEL       = del

# 默认动作

.PHONY : default
default :
	$(MAKE) -C lib
	$(MAKE) -C main
	$(MAKE) -C console
	$(MAKE) -C drive
	$(MAKE) -C fs
	$(MAKE) -C int
	$(MAKE) -C method
	$(MAKE) -C ui
	$(MAKE) MitOS.sys

# 文件生成规则

MitOS.sys : main/boot.bin lib/stdlib.lib $(OBJS) Makefile
	$(MKBIM) @$(RULEFILE) out:main.bim stack:3136k map:main.map $(OBJS)
	$(MKMIT) main.bim main.mit 0
	$(COPY) /B main\boot.bin+main.mit MitOS.sys

# 命令

.PHONY : clean
clean :
	-$(DEL)    *.map
	-$(DEL)    *.bim
	-$(DEL)    *.mit
	$(MAKE) -C lib			clean
	$(MAKE) -C main			clean
	$(MAKE) -C console		clean
	$(MAKE) -C drive		clean
	$(MAKE) -C fs			clean
	$(MAKE) -C int			clean
	$(MAKE) -C method		clean
	$(MAKE) -C ui			clean

.PHONY : mrproper
mrproper :
	$(MAKE) clean
	-$(DEL) MitOS.sys
	$(MAKE) -C lib			mrproper
	$(MAKE) -C main			mrproper
	$(MAKE) -C console		mrproper
	$(MAKE) -C drive		mrproper
	$(MAKE) -C fs			mrproper
	$(MAKE) -C int			mrproper
	$(MAKE) -C method		mrproper
	$(MAKE) -C ui			mrproper
