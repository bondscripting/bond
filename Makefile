#http://wiki.osdev.org/Makefile

SRCDIR = source
BLDDIR = build
LIBDIR = lib

SRCFILES = $(shell find $(SRCDIR) -name "*.cpp")
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,$(BLDDIR)/%.o,$(SRCFILES))
DEPFILES = $(patsubst %.o,%.d,$(OBJFILES))
LIB = $(LIBDIR)/bond.a

SRCFILE = $(SRCDIR)/%.cpp
OBJFILE = $(BLDDIR)/%.o
DEPFILE = $(BLDDIR)/%.d

AR = ar
CC = g++
RM = rm
CFLAGS = -Wall -g -Iinclude

.PHONY: all clean

all: $(LIB)

$(LIB): $(LIBDIR) $(OBJFILES)
	$(AR) rcs $(LIB) $?

$(BLDDIR):
	mkdir -p $@

$(LIBDIR):
	mkdir -p $@

clean:
	$(RM) -f $(LIB) $(OBJFILES) $(DEPFILES)

-include $(DEPFILES)

$(OBJFILE): $(SRCFILE) $(BLDDIR) Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
