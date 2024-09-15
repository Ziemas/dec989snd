TARGET := 989SND_D.IRX
BUILDDIR := build
CFLAGS := -G 0 -O0 -gstabs -Iinclude
LDFLAGS := -Tconfig/undefined_syms_auto.txt -Tconfig/undefined_funcs_auto.txt -T$(TARGET).ld -Map $(BUILDDIR)/$(TARGET).map
ASFLAGS := -Iinclude -G0 -g2 -O1 -no-pad-sections
MASPSXFLAGS := --aspsx-version=2.78
CPPFLAGS := -ffreestanding -Iinclude

CC := wine tools/gcc-2.95.2/cc1.exe
CPP := cpp
MASPSX := python tools/maspsx/maspsx.py
AS := mipsel-none-elf-as
LD := mipsel-none-elf-ld
OBJCOPY := mipsel-none-elf-objcopy

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRCS := $(call rwildcard,src/,*.c)
SRCS += $(call rwildcard,asm/data/,*.s)
SRCS += $(call wildcard,asm/*.s)
#SRCS += asm/export_stub.s asm/import_stub.s 

OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.c.o,$(filter %.c,$(SRCS)))
OBJECTS += $(patsubst %.s,$(BUILDDIR)/%.s.o,$(filter %.s,$(SRCS)))

all: $(BUILDDIR)/$(TARGET)

#$(BUILDDIR)/$(TARGET): $(BUILDDIR)/$(TARGET).o
#	iopfixup -o $@ $(BUILDDIR)/$(TARGET).o

#$(BUILDDIR)/$(TARGET).o: $(OBJECTS)
#	@mkdir -p $(dir $@)
#	$(LD) -dc -r -o $@ $(OBJECTS) $(LDFLAGS)


$(BUILDDIR)/$(TARGET): $(BUILDDIR)/$(TARGET).elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) $< $@ -O binary

$(BUILDDIR)/$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(OBJECTS) $(LDFLAGS)

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) $< | $(CC) $(CFLAGS) | $(MASPSX) $(MASPSXFLAGS) | $(AS) $(ASFLAGS) -o $@

$(BUILDDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(MASPSX) $(MASPSXFLAGS) $< | $(AS) -o $@ $(ASFLAGS) 

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJECTS)
