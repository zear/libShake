ifeq ($(PLATFORM), gcw0)
  CC := /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
  STRIP := /opt/gcw0-toolchain/usr/bin/mipsel-linux-strip
  BACKEND := LINUX
endif

ifndef BACKEND
$(error Please specify BACKEND. Possible values: LINUX, OSX")
endif

LIBNAME      := libshake
SOVERSION    := 0

SRC := $(wildcard src/common/*.c)

ifeq ($(BACKEND), LINUX)
  LIBEXT := .so
  SONAME := $(LIBNAME)$(LIBEXT).$(SOVERSION)
  PREFIX ?= /usr
  LDFLAGS :=-Wl,-soname,$(SONAME)
  SRC += $(wildcard src/linux/*.c)
else ifeq ($(BACKEND), OSX)
  LIBEXT := .dylib
  SONAME := $(LIBNAME).$(SOVERSION)$(LIBEXT)
  PREFIX ?= /usr/local
  LDFLAGS := -Wl,-framework,Cocoa -framework IOKit -framework CoreFoundation -framework ForceFeedback -install_name $(SONAME)
  SRC += $(wildcard src/osx/*.c)
endif

CC ?= gcc
STRIP ?= strip
TARGET ?= $(SONAME)
SYSROOT := $(shell $(CC) --print-sysroot)
DESTDIR ?= $(SYSROOT)
CFLAGS := -fPIC
OBJ := $(SRC:.c=.o)

ifdef DEBUG
  CFLAGS += -ggdb -Wall -Werror
else
  CFLAGS += -O2
endif

.PHONY: all clean

all:	$(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -shared $(CFLAGS) $^ -o $@
ifdef DO_STRIP
	$(STRIP) $@
endif

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@ -I include

install-headers:
	cp include/*.h $(DESTDIR)$(PREFIX)/include/

install-lib:
	cp $(TARGET) $(DESTDIR)$(PREFIX)/lib/
	ln -sf $(DESTDIR)$(PREFIX)/lib/$(TARGET) $(DESTDIR)$(PREFIX)/lib/$(LIBNAME)$(LIBEXT)

install: $(TARGET) install-headers install-lib

clean:
	rm -Rf $(TARGET) $(OBJ)
