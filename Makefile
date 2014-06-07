# Makefile suitable for POSIXy platforms having GCC.

# Folders
BLDDIR := build
LIBDIR := $(BLDDIR)/lib

# File name manipulations.
DEP_SUFFIX := .d
OBJ_SUFFIX := .o
LIB_PREFIX :=
LIB_SUFFIX := .a
EXE_SUFFIX :=

# Tools
AR := ar
CC := g++
CHECK := cppcheck
ETAGS := etags
MKDIR := mkdir
RM := rm

# Command line options
BASE_CFLAGS := -Wall -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wold-style-cast -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused -O3
INCLUDE_CFLAGS := -I
DEP_CFLAGS := -MMD -MP
COMP_CFLAGS := -c
OUT_CFLAGS := -o

-include Makefile.common
