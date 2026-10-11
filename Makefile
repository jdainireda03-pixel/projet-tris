# Makefile compatible avec Windows CMD et MSYS2 UCRT64.
# Organisation attendue :
#   Makefile
#   src/*.c
#   include/*.h

.RECIPEPREFIX = >

CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -O2 -Iinclude -MMD -MP
LDLIBS = -lm

# MSYS2 définit MSYSTEM (par exemple UCRT64). Priorité à ce cas,
# même si OS vaut aussi Windows_NT.
ifneq ($(MSYSTEM),)
SHELL = /usr/bin/sh
EXE = .exe
MKDIR_BIN = mkdir -p bin
MKDIR_BLD = mkdir -p build
MKDIR_RES = mkdir -p resultats
RUN = ./bin/bench.exe
RM_ALL = rm -rf build bin
else ifeq ($(OS),Windows_NT)
SHELL = cmd.exe
EXE = .exe
MKDIR_BIN = if not exist bin mkdir bin
MKDIR_BLD = if not exist build mkdir build
MKDIR_RES = if not exist resultats mkdir resultats
RUN = bin\\bench.exe
RM_ALL = if exist build rmdir /S /Q build & if exist bin rmdir /S /Q bin
else
SHELL = /bin/sh
EXE =
MKDIR_BIN = mkdir -p bin
MKDIR_BLD = mkdir -p build
MKDIR_RES = mkdir -p resultats
RUN = ./bin/bench
RM_ALL = rm -rf build bin
endif

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))

all: bin/bench$(EXE)

bin/bench$(EXE): $(OBJ)
> $(MKDIR_BIN)
> $(CC) $(OBJ) -o $@ $(LDLIBS)

build/%.o: src/%.c
> $(MKDIR_BLD)
> $(CC) $(CFLAGS) -c $< -o $@

run: bin/bench$(EXE)
> $(MKDIR_RES)
> $(RUN)

clean:
> $(RM_ALL)

-include $(OBJ:.o=.d)

.PHONY: all run clean
