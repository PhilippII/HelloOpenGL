INCDIR = inc
SRCDIR = src
INTDIR = build/intermediates
OBJDIR = $(INTDIR)/obj
DEPDIR = $(INTDIR)/.deps
OUTDIR = build/run

CXXSFX = cxx
CXXFILES = $(wildcard $(SRCDIR)/*.$(CXXSFX))
OBJFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(OBJDIR)/%.o)
DEPFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(DEPDIR)/%.d)
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
	rm -f $(DEPDIR)/*.d $(OBJDIR)/*.o $(OUTDIR)/$(OUT)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(DEPDIR):
	mkdir -p $(DEPDIR)

# generate header dependencies:
$(DEPDIR)/%.d: $(SRCDIR)/%.$(CXXSFX) | $(DEPDIR)
	$(CXX) -MM -MT "$(OBJDIR)/$*.o $@" -MF $@ $(CFLAGS) $<
#	-MM output dependencies, but do not include headers in system directories
#	-MT "$(OBJDIR)/$*.o $@" also include the dependency file itself in the targets in addition
#		the object file (if one of the headers changes, the dependency file might be
#		out of date itself, since the changed header might itself recursively include
#		new headers)
#	-MF $@ output result to dependency file instead of standard output stream

include $(DEPFILES)

