CC?=gcc
INCLUDE?=-Iinclude
CFLAGS?=-O0
LDFLAGS?=
RENDER?=render-vulkan
PLATFORM?=linux

# -- Do not edit below this line --

VERSION:="$(shell git describe --abbrev=4 --dirty --always --tags)"
CC:=$(CC)
INCLUDE:=$(INCLUDE)
CFLAGS:=$(CFLAGS) $(INCLUDE) -Wall -Wextra -DVERSION=\"$(VERSION)\" -ggdb -fstack-protector-all -fPIC
LDFLAGS:=$(LDFLAGS) -shared

COREDIR:=core
RENDERDIR:=$(RENDER)
ENGINE:=librune.so

include $(COREDIR)/make.config $(RENDERDIR)/make.config

LDFLAGS:=$(LDFLAGS) $(RENDER_LDFLAGS) $(CORE_LDFLAGS)
LIBS:=$(LIBS) $(RENDER_LIBS) $(CORE_LIBS)
OBJS:=$(RENDER_OBJS) $(CORE_OBJS)

.PHONY: all clean install install-headers
.SUFFIXES: .o .c

all: $(ENGINE)

$(ENGINE): $(OBJS)
	@$(CC) -o $@ $(LIBS) $(LDFLAGS) $?
	@echo [LD] $@
	@objcopy --only-keep-debug $(ENGINE) $(ENGINE:.so=.sym)
	@strip -s $(ENGINE)
	@objcopy --add-gnu-debuglink=$(ENGINE:.so=.sym) $(ENGINE)
	@echo [strip] $(ENGINE)

.c.o:
	@$(CC) -MD -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@echo [CC] $@

clean:
	$(RM) $(OBJS) $(OBJS:o=.d)

-include $(OBJS:.o=.d)
