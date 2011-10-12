######################################################################

PROGRAMS          = test-spot test-alsa
test_spot_SOURCES = test-spot.cpp alsa.cpp
test_spot_HEADERS = alsa.h
test_alsa_SOURCES = test-alsa.cpp alsa.cpp
test_alsa_HEADERS = alsa.h

SPOTIFY_CXXFLAGS = $(shell pkg-config --cflags libspotify)
SPOTIFY_LDFLAGS  = $(shell pkg-config --libs-only-L libspotify) $(shell pkg-config --libs-only-other libspotify)
SPOTIFY_LIBS     = $(shell pkg-config --libs-only-l libspotify)

ALSA_CXXFLAGS    = $(shell pkg-config --cflags alsa)
ALSA_LDFLAGS     = $(shell pkg-config --libs-only-L alsa) $(shell pkg-config --libs-only-other alsa)
ALSA_LIBS        = $(shell pkg-config --libs-only-l alsa)

######################################################################

CXXFLAGS  = -Wall -Wextra -Weffc++ -std=c++0x -ggdb3 -pipe
LDFLAGS   = -ggdb3 -pipe
LIBS      =

CXXFLAGS += $(SPOTIFY_CXXFLAGS) $(ALSA_CXXFLAGS)
LDFLAGS  += $(SPOTIFY_LDFLAGS) $(ALSA_LDFLAGS)
LIBS     += $(SPOTIFY_LIBS) $(ALSA_LIBS)

CXX       = g++
LD        = g++

test_spot_OBJECTS = $(test_spot_SOURCES:%.cpp=%.o)
test_alsa_OBJECTS = $(test_alsa_SOURCES:%.cpp=%.o)

######################################################################

.PHONY: default
default: all

.PHONY: all
all: build_programs

.PHONY: build_programs
build_programs: pre_build_programs $(foreach p,$(subst -,_,$(PROGRAMS)),build_$(p)) post_build_programs

.PHONY: pre_build_programs
pre_build_programs:

.PHONY: post_build_programs
post_build_programs:

######################################################################

.PHONY: build_test_spot
build_test_spot: pre_build_test_spot test-spot post_build_test_spot

.PHONY: pre_build_test_spot
pre_build_test_spot: tmp

.PHONY: post_build_test_spot
post_build_test_spot:

tmp:
	-mkdir -p tmp

test-spot: $(test_spot_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

######################################################################

.PHONY: build_test_alsa
build_test_alsa: pre_build_test_alsa test-alsa post_build_test_alsa

.PHONY: pre_build_test_alsa
pre_build_test_alsa:

.PHONY: post_build_test_alsa
post_build_test_alsa:

test-alsa: $(test_alsa_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

######################################################################

.cpp.o:
	$(CXX) $(CXXFLAGS) $^ -c -o $@

######################################################################

.PHONY: someclean
someclean:
	-rm -f *~

.PHONY: clean
clean: someclean
	-rm -f $(test_spot_OBJECTS) $(test_alsa_OBJECTS)

.PHONY: realclean
realclean: clean
	-rm -f $(PROGRAMS)

######################################################################
