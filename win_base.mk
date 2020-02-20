#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

DEFINES	:=	$(ENGINE_DEFINES) $(GAME_DEFINES)

ENGINEDIR := $(GAMEDIR)/MinimalismEngine
ENGINESRC := src src/util windows

ifneq ($(strip $(USE_SDL)),)
	ENGINESRC += sdl
endif

CC := gcc
CXX := g++

CFLAGS	:=	-s -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -O2 \
			-fomit-frame-pointer -ffunction-sections \
			-mwindows -D_WINDOWS -DAPP_TITLE='"$(APP_TITLE)"' \
			$(DEFINES)

CXXFLAGS := $(CFLAGS) $(INCLUDE) -fno-rtti -fno-exceptions -std=gnu++11 -static-libstdc++ -static-libgcc

LIBS	:=  $(EXTRA_LIBS) -lmingw32

ifneq ($(strip $(USE_SDL)),)
	CXXFLAGS += -D_USE_SDL
	LIBS += $(SDL_LIBS)
endif


LIBDIRS	:= 

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(GAMEDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(GAMEDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(ENGINESRC),$(ENGINEDIR)/$(dir))

export DEPSDIR	:=	$(GAMEDIR)/$(BUILD)

export CFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(GAMEDIR)/$(dir)/*.c)) \
			$(foreach dir,$(ENGINESRC),$(wildcard $(ENGINEDIR)/$(dir)/*.c))
export CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(GAMEDIR)/$(dir)/*.cpp)) \
			$(foreach dir,$(ENGINESRC),$(wildcard $(ENGINEDIR)/$(dir)/*.cpp))
export RESFILES	:=	$(foreach dir,$(META),$(wildcard $(GAMEDIR)/$(dir)/*.res))
export BINFILES	:=	$(foreach dir,$(DATA),$(wildcard $(GAMEDIR)/$(dir)/*.*))


ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

#export OFILES_SRC 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
#export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
#export OFILES 		:=	$(OFILES_BIN) $(OFILES_SOURCES)
#export HFILES		:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE		:=	$(foreach dir,$(INCLUDES),-I$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			$(foreach dir,$(ENGINESRC),-I$(ENGINEDIR)/$(dir)) \
			-I$(GAMEDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifneq ($(strip $(RES_SCRIPT)),)
	export APP_RES := $(GAMEDIR)/$(META)/$(RES_SCRIPT)
else
	export APP_RES := 
endif


.PHONY: $(BUILD) all clean clean_output

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@mkdir -p $(BUILD) $(OUTDIR)
	@windres $(APP_RES) -O coff -o $(APP_RES).res
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(ENGINEDIR)/win_base.mk

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTDIR)

clean_output:
	@rm -fr $(OUTPUT).exe

#---------------------------------------------------------------------------------
else

.PHONY: all

all: $(OUTPUT).exe

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).exe	:
	$(LD) $(CXXFLAGS) $(CFILES) $(CPPFILES) $(RESFILES) $(LIBPATHS) $(LIBS) -o $(OUTPUT).exe

#$(OFILES_SRC)	:	$(HFILES)

#$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.res	:	%.rc
#---------------------------------------------------------------------------------
	@windres $< -O coff -o $*.res


#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
