TARGET := 989SND_D.IRX
BUILDDIR := build
CFLAGS := -G 0 -O0 -gstabs -Iinclude -Wa,-g
LDFLAGS := -Tconfig/undefined_syms_auto.txt -Tconfig/undefined_funcs_auto.txt -T$(TARGET).ld -Map $(BUILDDIR)/$(TARGET).map
ASFLAGS := -Iinclude -G0

CC := iop-gcc
AS := ./Ps2IopAs.exe
MODERN_AS := mipsel-none-elf-as
LD := mipsel-none-elf-ld

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRCS := $(call rwildcard,src/,*.c)
SRCS += $(call rwildcard,asm/data/,*.s)
SRCS += $(call wildcard,asm/*.s)
#SRCS += asm/export_stub.s asm/import_stub.s 

OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.c.o,$(filter %.c,$(SRCS)))
OBJECTS += $(patsubst %.s,$(BUILDDIR)/%.s.o,$(filter %.s,$(SRCS)))

all: fixup $(BUILDDIR)/$(TARGET)

# This causes make to always rebuild, but whatever
fixup:
	find . -name '*.s' | xargs sed -i 's/break[[:space:]]*0,[[:space:]]*7/break 7/'

#$(BUILDDIR)/$(TARGET): $(BUILDDIR)/$(TARGET).o
#	iopfixup -o $@ $(BUILDDIR)/$(TARGET).o

#$(BUILDDIR)/$(TARGET).o: $(OBJECTS)
#	@mkdir -p $(dir $@)
#	$(LD) -dc -r -o $@ $(OBJECTS) $(LDFLAGS)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(OBJECTS) $(LDFLAGS)

$(BUILDDIR)/%.c.o: %.c.s
	@mkdir -p $(dir $@)
	$(AS) -o $@ $< $(ASFLAGS)

$(BUILDDIR)/%.c.s: %.c
	@mkdir -p $(dir $@)
	$(CC) -S -o $@ $< $(CFLAGS)

$(BUILDDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(MODERN_AS) -o $@ $< $(ASFLAGS) 

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJECTS)
