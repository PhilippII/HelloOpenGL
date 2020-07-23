INCDIR = inc
SRCDIR = src
OBJDIR = build/intermediates
OUTDIR = build/run

CXXSFX = cxx
CXXFILES = $(wildcard $(SRCDIR)/*.$(CXXSFX))
OBJFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(OBJDIR)/%.o)
OUT = main

CXX = g++
CFLAGS = -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -Wwrite-strings -Wold-style-cast
CFLAGS += -g -std=c++17 -pedantic-errors -I$(INCDIR)
LDFLAGS =
LDLIBS =

# glfw3:
CFLAGS += $(shell pkg-config --static --cflags glfw3)
LDLIBS += $(shell pkg-config --static --libs glfw3)

# gl (TODO: remove this when using glew or glad or similar):
CFLAGS += $(shell pkg-config --cflags gl)
LDLIBS += $(shell pkg-config --libs gl)

all: $(OUTDIR)/$(OUT)

.PHONY: all clean

$(OUTDIR)/$(OUT): $(OBJFILES) | $(OUTDIR)
	$(CXX) $(LDFLAGS) -o $@ $(OBJFILES) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(CXXSFX) | $(OBJDIR)
	$(CXX) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o $(OUTDIR)/$(OUT)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)









