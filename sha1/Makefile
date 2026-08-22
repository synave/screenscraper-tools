# Makefile pour cherche_doublon.cpp
# Utilisation :
#   make            -> build (mode debug par défaut)
#   make BUILD=release  -> build en release (optimisé)
#   make debug/release  -> cible raccourcie
#   make run        -> exécute ./cherche_doublon
#   make clean      -> supprime .o
#   make distclean  -> supprime binaire aussi

# Toolchain
CXX ?= g++
# C++ standard
STD ?= c++17

# Nom du binaire (modifiez si besoin)
BIN := sha1

# Detecte automatiquement tous les .cpp dans le répertoire
SRC := sha1.cpp ../src/Rom.cpp ../src/Cryptage.cpp ../src/ScreenScraper.cpp ../src/Requete.cpp
OBJ := $(patsubst %.cpp,%.o,$(SRC))

# Build mode (debug par défaut)
BUILD ?= release

# Flags par défaut
CXXFLAGS := -std=$(STD) -Wall -Wextra -I../include
LDFLAGS  := 
LDLIBS   := -lcurl -lcrypto -lz -ltinyxml2

ifeq ($(BUILD),release)
    CXXFLAGS += -O2 -DNDEBUG
else
    CXXFLAGS += -g -O0 -DDEBUG
endif


.PHONY: all debug release clean distclean run info

all: $(BIN)

# Link
$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Targets helpers
debug:
	$(MAKE) BUILD=debug all

release:
	$(MAKE) BUILD=release all

clean:
	$(RM) $(OBJ) *~

distclean: clean
	$(RM) $(BIN)

info:
	@echo "CXX      = $(CXX)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "LDLIBS   = $(LDLIBS)"
	@echo "SRC      = $(SRC)"
	@echo "OBJ      = $(OBJ)"
	@echo "BIN      = $(BIN)"
