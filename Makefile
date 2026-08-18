# Toolchain
CXX ?= g++
# C++ standard
STD ?= c++17

SRC := $(wildcard ./src/*.cpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))

# Build mode (debug par défaut)
BUILD ?= release

# Flags par défaut
CXXFLAGS := -std=$(STD) -Wall -Wextra -I./include
LDFLAGS  := 
LDLIBS   := -lcurl -lcrypto -lz -ltinyxml2

ifeq ($(BUILD),release)
    CXXFLAGS += -O2 -DNDEBUG
else
    CXXFLAGS += -g -O0 -DDEBUG
endif

.PHONY: all lib crc md5 sha1 scraper

all: lib crc md5 sha1 scraper

lib : $(OBJ)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

crc :
	cd crc
	$(MAKE)


# Targets helpers
debug:
	$(MAKE) BUILD=debug all

release:
	$(MAKE) BUILD=release all

clean:
	$(RM) $(OBJ) *~
