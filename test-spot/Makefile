######################################################################

PROGRAM = test-spot
SOURCES = test-spot.cpp
HEADERS =

SPOTIFY_CXXFLAGS = $(shell pkg-config --cflags libspotify)
SPOTIFY_LDFLAGS  = $(shell pkg-config --libs-only-L libspotify) $(shell pkg-config --libs-only-other libspotify)
SPOTIFY_LIBS     = $(shell pkg-config --libs-only-l libspotify)

######################################################################

CXXFLAGS  = -Wall -Wextra -Weffc++ -std=c++0x -ggdb3 -pipe
LDFLAGS   = -ggdb3 -pipe
LIBS      =

CXXFLAGS += $(SPOTIFY_CXXFLAGS)
LDFLAGS  += $(SPOTIFY_LDFLAGS)
LIBS     += $(SPOTIFY_LIBS)

CXX       = g++
LD        = g++

OBJECTS   = $(SOURCES:%.cpp=%.o)

######################################################################

.PHONY: default
default: all

.PHONY: all
all: build_program

.PHONY: build_program
build_program: pre_build $(PROGRAM) post_build

$(PROGRAM): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

.PHONY: pre_build
pre_build: tmp

.PHONY: post_build
post_build:

.cpp.o:
	$(CXX) $(CXXFLAGS) $^ -c -o $@

tmp:
	-mkdir -p tmp

######################################################################

.PHONY: someclean
someclean:
	-rm -f *~

.PHONY: clean
clean: someclean
	-rm -f $(OBJECTS)

.PHONY: realclean
realclean: clean
	-rm -f $(PROGRAM)

######################################################################
