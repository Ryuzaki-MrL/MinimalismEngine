#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

DEFINES	:=	$(ENGINE_DEFINES) $(GAME_DEFINES)

ENGINEDIR := $(GAMEDIR)/MinimalismEngine
ENGINESRC := src src/util 3ds

CFLAGS	:=	-g -Wall -Wextra -Wno-unused-parameter -Wno-psabi -O2 -mword-relocations \
			-fomit-frame-pointer -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= $(EXTRA_LIBS) -lcitro2d -lcitro3d -lctru -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CTRULIB)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(GAMEDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(GAMEDIR)

export GFXBUILD	:=	$(GAMEDIR)/$(GFXOUT)

export VPATH	:=	$(foreach dir,$(SOURCES),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(GRAPHICS),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(ENGINESRC),$(ENGINEDIR)/$(dir))

export DEPSDIR	:=	$(GAMEDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CFILES		+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CPPFILES	+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.s)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
PICAFILES	+=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.v.pica)))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
SHLISTFILES	:=	$(foreach dir,$(ENGINESRC),$(notdir $(wildcard $(ENGINEDIR)/$(dir)/*.shlist)))
GFXFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.t3s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

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

export T3XFILES		:=	$(GFXFILES:.t3s=.t3x)

export OFILES_SOURCES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES)) \
			$(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o) \
			$(if $(filter $(BUILD),$(GFXBUILD)),$(addsuffix .o,$(T3XFILES)))

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES	:=	$(PICAFILES:.v.pica=_shbin.h) $(SHLISTFILES:.shlist=_shbin.h) \
			$(addsuffix .h,$(subst .,_,$(BINFILES))) \
			$(GFXFILES:.t3s=.h)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(GAMEDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			$(foreach dir,$(ENGINESRC),-I$(ENGINEDIR)/$(dir)) \
			-I$(GAMEDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS	:=	$(if $(NO_SMDH),,$(OUTPUT).smdh)

ifneq ($(strip $(ICON)),)
	export APP_ICON := $(GAMEDIR)/$(META)/$(ICON)
else
	export APP_ICON := 
endif

export BANNER_AUDIO	:=	$(META)/banner.wav
export BANNER_IMAGE	:=	$(META)/banner.png
export APP_RSF		:=	$(META)/app.rsf

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(OUTPUT).smdh
endif

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(GAMEDIR)/$(ROMFS)
endif

.PHONY: all clean clean_output

#---------------------------------------------------------------------------------
all:
	@mkdir -p $(BUILD) $(GFXBUILD) $(OUTDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(ENGINEDIR)/3ds_base.mk $(OUTPUT).3dsx
	@bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -i "$(APP_ICON)" -f "$(ICON_FLAGS)" -o $(BUILD)/icon.icn
	@bannertool makebanner -i "$(BANNER_IMAGE)" -a "$(BANNER_AUDIO)" -o $(BUILD)/banner.bnr
	@makerom -f cia -o $(OUTPUT).cia -target t -exefslogo -elf "$(OUTPUT).elf" -rsf "$(APP_RSF)" -ver "$$(($(APP_MAJOR)*1024+$(APP_MINOR)*16+$(APP_PATCH)))" -banner "$(BUILD)/banner.bnr" -icon "$(BUILD)/icon.icn" -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(PRODUCT_CODE)" -DAPP_UNIQUE_ID="$(UNIQUE_ID)" -DAPP_ROMFS="$(ROMFS)" -logo "$(LOGO)"
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTDIR)

clean_output:
	@rm -fr $(OUTDIR)

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(_3DSXDEPS)

$(OFILES_SOURCES) : $(HFILES)

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.t3x.o	%_t3x.h :	%.t3x
#---------------------------------------------------------------------------------
	@$(bin2o)

#---------------------------------------------------------------------------------
# rules for assembling GPU shaders
#---------------------------------------------------------------------------------
define shader-as
	$(eval CURBIN := $*.shbin)
	$(eval DEPSFILE := $(DEPSDIR)/$*.shbin.d)
	echo "$(CURBIN).o: $< $1" > $(DEPSFILE)
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(CURBIN) | tr . _)`.h
	picasso -o $(CURBIN) $1
	bin2s $(CURBIN) | $(AS) -o $*.shbin.o
endef

%.shbin.o %_shbin.h : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

%.shbin.o %_shbin.h : %.v.pica
	@echo $(notdir $<)
	@$(call shader-as,$<)

%.shbin.o %_shbin.h : %.shlist
	@echo $(notdir $<)
	@$(call shader-as,$(foreach file,$(shell cat $<),$(dir $<)$(file)))

#---------------------------------------------------------------------------------
%.t3x	%.h	:	%.t3s
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@tex3ds -i $< -H $*.h -d $*.d -o $(GFXBUILD)/$*.t3x

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
