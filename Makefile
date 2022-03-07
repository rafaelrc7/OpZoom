##########TEMPLATE##########

TARGET=ozoom

SRCDIR=src
OBJDIR=obj
DEPDIR=dep
LIBDIR=lib

CC=gcc
EXT=.c

CCFLAGS=-g -O0 -Wall -Wextra -pedantic
LDFLAGS=-I$(LIBDIR) -llua


##########AUTO##########

SRC=$(wildcard $(SRCDIR)/*$(EXT))
OBJ=$(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP=$(OBJ:$(OBJDIR)/%.o=$(DEPDIR)/%.d)


##########TARGETS##########

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(DEPDIR)/%.d: $(SRCDIR)/%$(EXT) | $(DEPDIR)
	@$(CC) $(CFLAGS) $< -MM -MT $(@:$(DEPDIR)/%.d=$(OBJDIR)/%.o) >$@

-include $(DEP)

$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) | $(OBJDIR)
	$(CC) $(CCFLAGS) -o $@ -c $<

$(SRCDIR) $(OBJDIR) $(DEPDIR):
	mkdir -p $@

##########CLEAN##########

.PHONY: clean
clean:
	rm -r $(OBJDIR) $(DEPDIR) $(TARGET)
