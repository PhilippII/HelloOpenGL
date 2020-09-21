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

STB_IMAGE_DIR = 3rd_party/stb_image
STB_IMAGE_OBJ_DIR = $(OBJDIR)/3rd_party/stb_image

CXXSFX = cxx
CXXFILES = $(wildcard $(SRCDIR)/*.$(CXXSFX))
OBJFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(OBJDIR)/%.o)
DEPFILES = $(CXXFILES:$(SRCDIR)/%.$(CXXSFX)=$(DEPDIR)/%.d)
OUT = main

CXX = g++
CXXFLAGS = -Wall -Wextra -Wconversion -Wshadow -Wcast-qual -Wwrite-strings -Wold-style-cast
CXXFLAGS += -g -std=c++17 -pedantic-errors -I$(INCDIR) -I$(STB_IMAGE_DIR)
CXXFLAGS_STB = -g -std=c++17 -pedantic-errors
ifeq ($(CONFIG), release)
CXXFLAGS += -O2 -DNDEBUG
CXXFLAGS_STB += -O2 -DNDEBUG
else
# CXXFLAGS += -DDEBUG
# CXXFLAGS_STB
endif
LDFLAGS =
LDLIBS =

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

$(OUTDIR)/$(OUT): $(OBJFILES) $(STB_IMAGE_OBJ_DIR)/stb_image.o | $(OUTDIR)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(CXXSFX) | $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(STB_IMAGE_OBJ_DIR)/stb_image.o: $(STB_IMAGE_DIR)/stb_image.$(CXXSFX) | $(STB_IMAGE_OBJ_DIR)
	$(CXX) -c $(CXXFLAGS_STB) -o $@ $<

clean:
	rm -f -r $(CONFIGDIR.debug) $(CONFIGDIR.release)
#	rm -f $(DEPDIR)/*.d $(OBJDIR)/*.o $(OUTDIR)/$(OUT)

$(OBJDIR):
	mkdir -p $@

$(OUTDIR):
	mkdir -p $@

$(DEPDIR):
	mkdir -p $@

$(STB_IMAGE_OBJ_DIR):
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

