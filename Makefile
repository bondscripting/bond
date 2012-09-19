#http://wiki.osdev.org/Makefile

# Common files, folders and configuration.
BLDDIR := build
LIBDIR := $(BLDDIR)/lib
AR := ar
CC := g++
CHECK := cppcheck
ETAGS := etags
MKDIR := mkdir
RM := rm
BASE_CFLAGS = -Wall -O3

# Bond library files, folders and configuration.
INCLUDEDIR := include
SRCDIR := source
SRCFILE := $(SRCDIR)/%.cpp
OBJDIR := $(BLDDIR)/obj/bond
OBJFILE := $(OBJDIR)/%.o
OBJDEPFILE := $(OBJDIR)/%.d
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCFILE),$(OBJFILE),$(SRCFILES))
DEPFILES := $(patsubst $(SRCFILE),$(OBJDEPFILE),$(SRCFILES))
LIB := $(LIBDIR)/bond.a
CFLAGS := $(BASE_CFLAGS) -I$(INCLUDEDIR) -Isource

# Tool files, folders and configuration.
TOOL := %
TOOL_SRCDIR := tools
TOOL_SRCFILE := $(TOOL_SRCDIR)/%.cpp
TOOL_EXEDIR := $(BLDDIR)/bin
TOOL_EXEFILE := $(TOOL_EXEDIR)/%
TOOL_SRCFILES := $(wildcard $(TOOL_SRCDIR)/*.cpp)
TOOL_EXEFILES := $(patsubst $(TOOL_SRCFILE),$(TOOL_EXEFILE),$(TOOL_SRCFILES))
TOOL_EXEDEPFILE := $(TOOL_EXEDIR)/%.d
TOOL_DEPFILES := $(patsubst $(TOOL_SRCFILE),$(TOOL_EXEDEPFILE),$(TOOL_SRCFILES))
TOOLS := $(patsubst $(TOOL_SRCFILE),$(TOOL),$(TOOL_SRCFILES))
TOOL_CFLAGS := $(BASE_CFLAGS) -I$(INCLUDEDIR) -Itools

# Test Framework library files, folders and configuration.
TF_SRCDIR := test/framework
TF_SRCFILE := $(TF_SRCDIR)/%.cpp
TF_OBJDIR := $(BLDDIR)/obj/testframework
TF_OBJFILE := $(TF_OBJDIR)/%.o
TF_OBJDEPFILE := $(TF_OBJDIR)/%.d
TF_SRCFILES := $(wildcard $(TF_SRCDIR)/*.cpp)
TF_OBJFILES := $(patsubst $(TF_SRCFILE),$(TF_OBJFILE),$(TF_SRCFILES))
TF_DEPFILES := $(patsubst $(TF_SRCFILE),$(TF_OBJDEPFILE),$(TF_SRCFILES))
TF_LIB := $(LIBDIR)/framework.a
TF_CFLAGS := $(BASE_CFLAGS) -I$(INCLUDEDIR) -Itest

# Unit Test files, folders and configuration.
UT_TEST := %
UT_SRCDIR := test
UT_SRCFILE := $(UT_SRCDIR)/%.cpp
UT_EXEDIR := $(BLDDIR)/testbin
UT_EXEFILE := $(UT_EXEDIR)/%
UT_SRCFILES := $(wildcard $(UT_SRCDIR)/*.cpp)
UT_EXEFILES := $(patsubst $(UT_SRCFILE),$(UT_EXEFILE),$(UT_SRCFILES))
UT_EXEDEPFILE := $(UT_EXEDIR)/%.d
UT_DEPFILES := $(patsubst $(UT_SRCFILE),$(UT_EXEDEPFILE),$(UT_SRCFILES))
UT_TESTS := $(patsubst $(UT_SRCFILE),$(UT_TEST),$(UT_SRCFILES))
UT_CFLAGS := $(BASE_CFLAGS) -I$(INCLUDEDIR) -Itest


.PHONY: all check clean deepclean test tools tags

# Not sure why executables are considered to be generated by implicit rules when the rules are
# listed below, but they are. The consequence is that make deletes the executables after the rules
# that generate them run unless they are marked as precious.
.PRECIOUS: $(UT_EXEFILES) $(TOOL_EXEFILES)

all: $(LIB)

clean:
	$(RM) -rf $(BLDDIR)

deepclean:
	$(RM) -rf $(BLDDIR) TAGS vs/*/*.sdf vs/*/*.suo vs/*/*.user vs/*/ipch

tags :
	rm -f TAGS
	$(ETAGS) --language-force=C++ -R $(PWD)/$(INCLUDEDIR) $(PWD)/$(SRCDIR) $(PWD)/$(TOOL_SRCDIR) $(PWD)/$(UT_SRCDIR)

# Bond library targets.
$(LIB): $(OBJFILES)
	@$(MKDIR) -p $(LIBDIR)
	$(AR) rcs $(LIB) $?

-include $(DEPFILES)

$(OBJFILE): $(SRCFILE) Makefile
	@$(MKDIR) -p $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

check: $(SRCFILES)
	$(CHECK) -DBOGUS_CONFIG --enable=all -I $(INCLUDEDIR) -I source $^

# Tool targets.
tools: $(TOOLS)

$(TOOL): $(TOOL_EXEFILE) ;

-include $(TOOL_DEPFILES)

$(TOOL_EXEFILE): $(TOOL_SRCFILE) Makefile $(LIB)
	@$(MKDIR) -p $(TOOL_EXEDIR)
	$(CC) $(TOOL_CFLAGS) -MMD -MP $< $(LIB) -o $@

# Test Framework targets.
framework: $(TF_LIB)

$(TF_LIB): $(TF_OBJFILES)
	@$(MKDIR) -p $(LIBDIR)
	$(AR) rcs $(TF_LIB) $?

-include $(TF_DEPFILES)

$(TF_OBJFILE): $(TF_SRCFILE) Makefile
	@$(MKDIR) -p $(TF_OBJDIR)
	$(CC) $(TF_CFLAGS) -MMD -MP -c $< -o $@

# Unit Test targets.
test: $(UT_TESTS)

$(UT_TEST): $(UT_EXEFILE)
	$<

-include $(UT_DEPFILES)

$(UT_EXEFILE): $(UT_SRCFILE) Makefile $(LIB) $(TF_LIB)
	@$(MKDIR) -p $(UT_EXEDIR)
	$(CC) $(UT_CFLAGS) -MMD -MP $< $(TF_LIB) $(LIB) -o $@
