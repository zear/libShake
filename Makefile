ifeq ($(PLATFORM), gcw0)
  CC         := /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
  STRIP      := /opt/gcw0-toolchain/usr/bin/mipsel-linux-strip
endif

CC           ?= gcc
STRIP        ?= strip
TARGET       ?= libshake.so
SYSROOT      := $(shell $(CC) --print-sysroot)
CFLAGS       := -fPIC
SRCDIR       := src
OBJDIR       := obj
SRC          := $(wildcard $(SRCDIR)/*.c)
OBJ          := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SOVERSION    := $(TARGET).0

ifdef DEBUG
  CFLAGS += -ggdb -Wall -Werror
else
  CFLAGS += -O2
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -Wl,-soname,$(SOVERSION) -shared $(CFLAGS) $^ -o $@
ifdef DO_STRIP
	$(STRIP) $@
endif

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@ -I include

$(OBJDIR):
	mkdir -p $@

clean:
	rm -Rf $(TARGET) $(OBJDIR)

install: $(TARGET)
	cp include/*.h $(SYSROOT)/usr/include/
	cp $(TARGET) $(SYSROOT)/usr/lib/
