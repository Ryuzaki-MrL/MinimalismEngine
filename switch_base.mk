#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

APP_VERSION		:=	$(strip $(APP_MAJOR)).$(strip $(APP_MINOR)).$(strip $(APP_PATCH))

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

DEFINES	:=	$(ENGINE_DEFINES) $(GAME_DEFINES)

ENGINEDIR := $(GAMEDIR)/MinimalismEngine
ENGINESRC := src src/util switch

ifneq ($(strip $(USE_SDL)),)
	ENGINESRC += sdl
endif

CFLAGS	:=	-g -Wall -Wno-unused-parameter -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	-D__SWITCH__ $(INCLUDE)

ifneq ($(strip $(USE_SDL)),)
	CFLAGS += -D_USE_SDL `sdl2-config --cflags` `aarch64-none-elf-pkg-config --cflags $(SDL_LIBS)`
endif

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:=	$(EXTRA_LIBS)

ifneq ($(strip $(USE_SDL)),)
	LIBS += `sdl2-config --libs` `aarch64-none-elf-pkg-config --libs $(SDL_LIBS)`
else
	LIBS += -lnx
endif

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(GAMEDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(GAMEDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(ENGINESRC),$(ENGINEDIR)/$(dir))

export DEPSDIR	:=	$(GAMEDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CFILES		+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CPPFILES	+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			$(foreach dir,$(ENGINESRC),-I$(ENGINEDIR)/$(dir)) \
			-I$(GAMEDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifneq ($(strip $(CONFIG_JSON)),)
	export APP_JSON := $(GAMEDIR)/$(META)/$(CONFIG_JSON)
else
	export APP_JSON := 
endif

ifneq ($(strip $(ICON)),)
	export APP_ICON := $(GAMEDIR)/$(META)/$(ICON)
else
	export APP_ICON := 
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(OUTPUT).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(GAMEDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all clean_output

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@mkdir -p $(BUILD) $(GFXBUILD) $(OUTDIR)
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(ENGINEDIR)/switch_base.mk

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTDIR)

clean_output:
	@rm -fr $(OUTDIR)

#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
