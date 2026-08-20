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

.PHONY: all lib crc md5 sha1 scraper tests

all: lib crc md5 sha1 scraper tests

lib : $(OBJ)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

crc :
	@(cd crc && $(MAKE))

md5:
	@(cd md5 && $(MAKE))

sha1:
	@(cd sha1 && $(MAKE))

scraper:
	@(cd scraper && $(MAKE))

tests:
	@(cd tests && $(MAKE))


# Targets helpers
debug:
	$(MAKE) BUILD=debug all

release:
	$(MAKE) BUILD=release all

clean:
	$(RM) $(OBJ) *~

cleanall: clean
	@(cd crc && $(MAKE) distclean)
	@(cd md5 && $(MAKE) distclean)
	@(cd sha1 && $(MAKE) distclean)
	@(cd scraper && $(MAKE) distclean)
	@(cd tests && $(MAKE) distclean)
