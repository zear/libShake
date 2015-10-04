ifeq ($(PLATFORM), gcw0)
  CC         := /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
  STRIP      := /opt/gcw0-toolchain/usr/bin/mipsel-linux-strip
endif

CC           ?= gcc
STRIP        ?= strip
LIBNAME      := libshake.so
SOVERSION    := 0
SONAME       := $(LIBNAME).$(SOVERSION)
TARGET       ?= $(SONAME)
SYSROOT      := $(shell $(CC) --print-sysroot)
DESTDIR      ?= $(SYSROOT)
PREFIX       ?= /usr
CFLAGS       := -fPIC
SRCDIR       := src
OBJDIR       := obj
SRC          := $(wildcard $(SRCDIR)/*.c)
OBJ          := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

ifdef DEBUG
  CFLAGS += -ggdb -Wall -Werror
else
  CFLAGS += -O2
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -Wl,-soname,$(SONAME) -shared $(CFLAGS) $^ -o $@
ifdef DO_STRIP
	$(STRIP) $@
endif

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@ -I include

$(OBJDIR):
	mkdir -p $@

install-headers:
	cp include/*.h $(DESTDIR)$(PREFIX)/include/

install-lib:
	cp $(TARGET) $(DESTDIR)$(PREFIX)/lib/
	ln -sf $(DESTDIR)$(PREFIX)/lib/$(TARGET) $(DESTDIR)$(PREFIX)/lib/$(LIBNAME)

install: $(TARGET) install-headers install-lib

clean:
	rm -Rf $(TARGET) $(OBJDIR)
