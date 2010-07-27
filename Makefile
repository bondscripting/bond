#http://wiki.osdev.org/Makefile

SRCDIR = source
INCLUDEDIR = include
BLDDIR = build
BINDIR = $(BLDDIR)/bin
OBJDIR = $(BLDDIR)/obj
LIBDIR = $(BLDDIR)/lib

SRCFILE = $(SRCDIR)/%.cpp
OBJFILE = $(OBJDIR)/%.o
DEPFILE = $(OBJDIR)/%.d

SRCFILES = $(shell find $(SRCDIR) -name "*.cpp")
OBJFILES = $(patsubst $(SRCFILE),$(OBJFILE),$(SRCFILES))
DEPFILES = $(patsubst $(OBJFILE),$(DEPFILE),$(OBJFILES))
LIB = $(LIBDIR)/bond.a

AR = ar
CC = g++
MKDIR = mkdir
RM = rm
CFLAGS = -Wall -ansi -g -I$(INCLUDEDIR)

.PHONY: all clean

all: $(LIB)

$(BINDIR):
	mkdir -p $@

$(LIB): $(OBJFILES)
	@$(MKDIR) -p $(LIBDIR)
	$(AR) rcs $(LIB) $?

clean:
	$(RM) -rf $(BLDDIR)

-include $(DEPFILES)

$(OBJFILE): $(SRCFILE) Makefile
	@$(MKDIR) -p $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
