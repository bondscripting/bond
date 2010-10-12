#http://wiki.osdev.org/Makefile

# Common files, folders and configuration.
BLDDIR = build
BINDIR = $(BLDDIR)/bin
OBJDIR = $(BLDDIR)/obj
LIBDIR = $(BLDDIR)/lib
OBJFILE = $(OBJDIR)/%.o
DEPFILE = $(OBJDIR)/%.d
EXEFILE = $(BINDIR)/%
AR = ar
CC = g++
MKDIR = mkdir
RM = rm

# Bond library files, folders and configuration.
SRCDIR = source
SRCFILE = $(SRCDIR)/%.cpp
INCLUDEDIR = include
SRCFILES = $(shell find $(SRCDIR) -maxdepth 1 -name "*.cpp")
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,$(OBJFILE),$(SRCFILES))
DEPFILES = $(patsubst $(OBJFILE),$(DEPFILE),$(OBJFILES))
LIB = $(LIBDIR)/bond.a
CFLAGS = -Wall -ansi -g -I$(INCLUDEDIR)

# Test Framework library files, folders and configuration.
TFSRCDIR = test/framework
TFSRCFILE = $(TFSRCDIR)/%.cpp
TFINCLUDEDIR = test
TFSRCFILES = $(shell find $(TFSRCDIR) -maxdepth 1 -name "*.cpp")
TFOBJFILES = $(patsubst $(TFSRCFILE),$(OBJFILE),$(TFSRCFILES))
TFDEPFILES = $(patsubst $(OBJFILE),$(DEPFILE),$(TFOBJFILES))
TFLIB = $(LIBDIR)/framework.a
TFCFLAGS = -Wall -g -I$(INCLUDEDIR) -I$(TFINCLUDEDIR)

 # Unit Test files, folders and configuration.
UTSRCDIR = test
UTSRCFILE = $(UTSRCDIR)/%.cpp
UTSRCFILES = $(shell find $(UTSRCDIR) -maxdepth 1 -name "*.cpp")
UTEXES = $(patsubst $(UTSRCFILE),$(EXEFILE),$(UTSRCFILES))
UTDEPFILES = $(patsubst $(EXEFILE),$(DEPFILE),$(UTEXES))
UTCFLAGS = -Wall -ansi -g -I$(INCLUDEDIR) -I$(TFINCLUDEDIR)

.PHONY: all clean

all: $(LIB)

clean:
	$(RM) -rf $(BLDDIR)

# Bond library targets.
$(LIB): $(OBJFILES)
	@$(MKDIR) -p $(LIBDIR)
	$(AR) rcs $(LIB) $?

-include $(DEPFILES)

$(OBJFILE): $(SRCFILE) Makefile
	@$(MKDIR) -p $(OBJDIR)
	$(CC) $(TFCFLAGS) -MMD -MP -c $< -o $@

# Test Framework targets.
framework: $(TFLIB)

$(TFLIB): $(TFOBJFILES)
	@$(MKDIR) -p $(LIBDIR)
	$(AR) rcs $(TFLIB) $?

-include $(TFDEPFILES)

$(OBJFILE): $(TFSRCFILE) Makefile
	@$(MKDIR) -p $(OBJDIR)
	$(CC) $(TFCFLAGS) -MMD -MP -c $< -o $@

# Unit Test targets.
test: $(UTEXES)

-include $(UTDEPFILES)

$(EXEFILE): $(UTSRCFILE) Makefile $(LIB) $(UTLIB)
	@$(MKDIR) -p $(BINDIR)
	$(CC) $(UTCFLAGS) -MMD -MP $< $(LIB) $(UTLIB) -o $@
	$@
