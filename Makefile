#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	frodo
BUILD		:=	build
SOURCES		:=	Src
DATA		:=	data  
INCLUDES	:=

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

PCFLAGS = -DPRECISE_CPU_CYCLES=1 -DPRECISE_CIA_CYCLES=1 -DPC_IS_POINTER=0
CFLAGS	= -g -Wall $(MACHDEP) $(INCLUDE) -I$(DEVKITPRO)/SDL/include -U__unix -DHAVE_SDL
CXXFLAGS	=	$(CFLAGS)

LDFLAGS	=	-L$(DEVKITPRO)/SDL/lib -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lSDL_image -lSDL_ttf -ljpeg -lpng -lz -lSDL -lfreetype -lfat -lwiiuse -lbte -logc -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:= char_to_kc.c gcaudio.c
CPPFILES	:= Display.cpp main.cpp Prefs.cpp SID.cpp REU.cpp IEC.cpp 1541fs.cpp \
               1541d64.cpp 1541t64.cpp 1541job.cpp SAM.cpp C64.cpp CPUC64.cpp VIC.cpp \
               CIA.cpp CPU1541.cpp menu.cpp
sFILES		:=	
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol Src/sysconfig.h Src/*.o \
          FrodoSC FrodoPC dist

dist: $(BUILD)
	install -d $@/apps/frodo
	install -d $@/apps/frodo/images
	install -d $@/apps/frodo/saves
	cp $(TARGET).dol $@/apps/frodo/boot.dol
	cp meta.xml $@/apps/frodo/
	cp icon.png $@/apps/frodo/
	cp FreeMono.ttf $@/apps/frodo/
	cp "Kernal ROM" $@/apps/frodo/Kernal_ROM
	cp "Char ROM" $@/apps/frodo/Char_ROM
	cp "1541 ROM" $@/apps/frodo/1541_ROM
	cp "Basic ROM" $@/apps/frodo/Basic_ROM
	cp frodorc $@/apps/frodo/
	cd $@ && tar -czf ../frodo-wii-bin.tar.gz *

#---------------------------------------------------------------------------------
run:
	wiiload $(TARGET).dol


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: sysconfig.h $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	:	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

sysconfig.h: sysconfig.h.Wii
	cp $< $@

Display.cpp: Display_SDL.i

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
