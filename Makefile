CC?=gcc
INCLUDE?=-Iinclude
CFLAGS?=-O0
LDFLAGS?=
LIBS?=-lglfw -lvulkan

# -- Do not edit below this line --

VERSION:="$(shell git describe --abbrev=4 --dirty --always --tags)"
INCLUDE:=$(INCLUDE)
CFLAGS:=$(CFLAGS) -Wall -Wextra -DVERSION=\"$(VERSION)\" -ggdb -fstack-protector-all -fPIC
LDFLAGS:=$(LDFLAGS) -shared
LIBS:=$(LIBS)

ENGINE=librune.so
ENGINE_OBJS=src/init.o \
	    src/graphics.o \
	    src/callbacks.o \

LINK_LIST=$(LDFLAGS) \
	  $(ENGINE_OBJS) \
	  $(LIBS) \

.PHONY: all clean install install-headers
.SUFFIXES: .o .c

all: $(ENGINE)

$(ENGINE): $(ENGINE_OBJS)
	@$(CC) -o $@ $(LINK_LIST)
	@echo [LD] $@

.c.o:
	@$(CC) -MD -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@echo [CC] $@

clean:
	$(RM) $(ENGINE)
	$(RM) $(ENGINE_OBJS) *.o */*.o */*/*.o
	$(RM) $(ENGINE_OBJS:.o=.d) *.d */*.d */*/*.d

-include $(ENGINE_OBJS:.o=.d)
