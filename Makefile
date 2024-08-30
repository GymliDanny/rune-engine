CC?=gcc
LD?=ld
INCLUDE?=-Iinclude
CFLAGS?=-O0
LDFLAGS?=
LIBS?=
PREFIX?=/usr/local/

HEADLESS?=0
RENDER_API?=vulkan
NETWORK?=1

# -- Do not edit below this line --

MAJOR:=0
MINOR:=55
REV:=
VERSION:="$(MAJOR).$(MINOR)$(REV)"

LIBRARY:=librune.so.$(MAJOR).$(MINOR)
SONAME:=librune.so.$(MAJOR)
SYMFILE:=librune.sym

LIBDIR:=$(DESTDIR)$(PREFIX)lib/
BINDIR:=$(DESTDIR)$(PREFIX)bin/
INCLUDEDIR:=$(DESTDIR)$(PREFIX)include/rune/

CC:=$(CC)
INCLUDE:=$(INCLUDE)
CFLAGS:=$(CFLAGS) $(INCLUDE) -Wall -Wextra -DVERSION=\"$(VERSION)\" -ggdb -fstack-protector-all -fPIC
LDFLAGS:=$(LDFLAGS) -shared

.PHONY: all check clean install
.SUFFIXES: .o .c

OBJS:=core/callbacks.o \
      core/init.o \
      core/logging.o \

-include graphics/make.config
-include sound/make.config
-include network/make.config

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	@$(LD) -soname $(SONAME) -o $@ $(LIBS) $(LDFLAGS) $?
	@echo [LD] $@
	@objcopy --only-keep-debug $(LIBRARY) $(SYMFILE)
	@objcopy --add-gnu-debuglink=$(SYMFILE) $(LIBRARY)
	@strip -s $(LIBRARY)
	@echo [strip] $(LIBRARY)

.c.o:
	@$(CC) -MD -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@echo [CC] $@

check:

clean:
	$(RM) $(OBJS) $(OBJS:.o=.d) $(LIBRARY) $(SYMFILE) tags TAGS

install:
	install -d $(LIBDIR)
	install -d $(BINDIR)
	install -d $(INCLUDEDIR)
	install -d $(INCLUDEDIR)$(RENDER_API)
	install -p -m 644 include/rune/*.h $(INCLUDEDIR)
	install -p -m 644 include/rune/$(RENDER_API)/*.h $(INCLUDEDIR)$(RENDER_API)
	install -p -m 755 $(LIBRARY) $(LIBDIR)
	ln -s $(LIBDIR)$(LIBRARY) $(LIBDIR)$(SONAME)

tags:
	ctags -R --kinds-c=+pLl --fields=+S include/ core/ render-vulkan/

-include $(OBJS:.o=.d)
