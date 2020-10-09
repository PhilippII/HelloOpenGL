# TODO: header-dependencies for imgui source files also?

INCDIR = inc
SRCDIR = src
CONFIGDIR.release = build/release
CONFIGDIR.debug = build/debug
ifeq ($(CONFIG), release)
CONFIGDIR = $(CONFIGDIR.release)
else
CONFIGDIR = $(CONFIGDIR.debug)
endif
INTDIR = $(CONFIGDIR)/intermediates
OBJDIR = $(INTDIR)/obj
DEPDIR = $(INTDIR)/.deps
OUTDIR = $(CONFIGDIR)/run

VENDORDIR = 3rd_party

GSLDIR = $(VENDORDIR)/GSL-master
STBDIR = $(VENDORDIR)/stb_image
STBOBJ.release = $(STBDIR)/build/release/intermediates/obj/stb_image.o
STBOBJ.debug = $(STBDIR)/build/debug/intermediates/obj/stb_image.o
ifeq ($(CONFIG), release)
STBOBJ = $(STBOBJ.release)
else
STBOBJ = $(STBOBJ.debug)
endif


IMGUIDIR = $(VENDORDIR)/dear_imgui/imgui
IMGUIBINDDIR = $(IMGUIDIR)/bindings
IMGUIOBJDIR = $(OBJDIR)/imgui

IMGUICXXFILES = $(wildcard $(IMGUIDIR)/*.cpp)
IMGUIBINDCXXFILES = $(wildcard $(IMGUIBINDDIR)/*.cpp)
IMGUIOBJFILES = $(IMGUICXXFILES:$(IMGUIDIR)/%.cpp=$(IMGUIOBJDIR)/%.o)
IMGUIOBJFILES += $(IMGUIBINDCXXFILES:$(IMGUIBINDDIR)/%.cpp=$(IMGUIOBJDIR)/%.o)


CXXSFX = cxx
CXXFILES = $(wildcard $(SRCDIR)/*.$(CXXSFX))
OBJFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(OBJDIR)/%.o)
DEPFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(DEPDIR)/%.d)
OUT = main

CXX = g++
CXXFLAGS = -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -Wwrite-strings -Wold-style-cast
CXXFLAGS += -g -std=c++17 -pedantic-errors -I$(INCDIR) 
CXXFLAGS += -I$(STBDIR)/inc -I$(VENDORDIR)/glm -I$(IMGUIDIR) -I$(IMGUIBINDDIR)
ifeq ($(CONFIG), release)
CXXFLAGS += -O2 -DNDEBUG
else
# CXXFLAGS += -DDEBUG
CXXFLAGS += -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
# CXXFLAGS += -D_GLIBCXX_CONCEPT_CHECKS # also use this?
endif
LDFLAGS =
LDLIBS =

#include Guidlines Support Library:
CXXFLAGS += -I$(GSLDIR)/include
# LDLIBS += nothing because it is basically a header only library

# glfw3:
CXXFLAGS += $(shell pkg-config --cflags glfw3)
LDLIBS += $(shell pkg-config --libs glfw3)

# glew:
CXXFLAGS += $(shell pkg-config --cflags glew)
LDLIBS += $(shell pkg-config --libs glew)

# gl (not needed anymore in combination with glew):
# CXXFLAGS += $(shell pkg-config --cflags gl)
# LDLIBS += $(shell pkg-config --libs gl)

all: $(OUTDIR)/$(OUT)

run: $(OUTDIR)/$(OUT)
	./$<

.PHONY: all run clean

$(OUTDIR)/$(OUT): $(OBJFILES) $(STBOBJ) $(IMGUIOBJFILES) | $(OUTDIR)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(CXXSFX) | $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(IMGUIOBJDIR)/%.o: $(IMGUIDIR)/%.cpp | $(IMGUIOBJDIR)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(IMGUIOBJDIR)/%.o: $(IMGUIBINDDIR)/%.cpp | $(IMGUIOBJDIR)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f -r $(CONFIGDIR.debug) $(CONFIGDIR.release)
#	rm -f $(DEPDIR)/*.d $(OBJDIR)/*.o $(OUTDIR)/$(OUT)

$(STBOBJ): FORCE
	$(MAKE) -C $(STBDIR)

FORCE:

$(OBJDIR):
	mkdir -p $@

$(IMGUIOBJDIR):
	mkdir -p $@

$(OUTDIR):
	mkdir -p $@

$(DEPDIR):
	mkdir -p $@

# generate header dependencies:
$(DEPDIR)/%.d: $(SRCDIR)/%.$(CXXSFX) | $(DEPDIR)
	$(CXX) -MM -MP -MT "$(OBJDIR)/$*.o $@" -MF $@ $(CXXFLAGS) $<
#	-MM output dependencies, but do not include headers in system directories
#	-MP add a phony target for each header in order to avoid errors if a header does not
#		exist anymore
#	-MT "$(OBJDIR)/$*.o $@" also include the dependency file itself in the targets in addition
#		the object file (if one of the headers changes, the dependency file might be
#		out of date itself, since the changed header might itself recursively include
#		new headers)
#	-MF $@ output result to dependency file instead of standard output stream

include $(DEPFILES)

