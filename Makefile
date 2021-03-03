##########TEMPLATE##########

TARGET=ozoom

SRCDIR=src
OBJDIR=obj
DEPDIR=dep
LIBDIR=lib

CC=gcc
EXT=.c

CCFLAGS=-g -O0 -Wall -Wextra -pedantic -D DEBUG
LDFLAGS=-I$(LIBDIR)


##########AUTO##########

SRC=$(wildcard $(SRCDIR)/*$(EXT))
OBJ=$(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP=$(OBJ:$(OBJDIR)/%.o=$(DEPDIR)/%.d)


##########TARGETS##########

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(DEPDIR)/%.d: $(SRCDIR)/%$(EXT)
	@$(CC) $(CFLAGS) $< -MM -MT $(@:$(DEPDIR)/%.d=$(OBJDIR)/%.o) >$@

-include $(DEP)

$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	$(CC) $(CCFLAGS) -o $@ -c $<


##########CLEAN##########

.PHONY: clean
clean:
	rm $(OBJ) $(DEP) $(TARGET)
