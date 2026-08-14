# Makefile pour scraper.cpp
# Utilisation :
#   make            -> build (mode debug par défaut)
#   make BUILD=release  -> build en release (optimisé)
#   make debug/release  -> cible raccourcie
#   make run        -> exécute ./scraper
#   make clean      -> supprime .o
#   make distclean  -> supprime binaire aussi

# Toolchain
CXX ?= g++
# C++ standard
STD ?= c++17

# Nom du binaire (modifiez si besoin)
BIN := scraper crc

# Detecte automatiquement tous les .cpp dans le répertoire
# SRC := $(wildcard *.cpp)
SRC-SCRAPER := $(filter-out crc.cpp,$(wildcard *.cpp))
SRC-CRC := $(filter-out scraper.cpp,$(wildcard *.cpp))
OBJ-SCRAPER := $(patsubst %.cpp,%.o,$(SRC-SCRAPER))
OBJ-CRC := $(patsubst %.cpp,%.o,$(SRC-CRC))

# Build mode (debug par défaut)
BUILD ?= debug

# Flags par défaut
CXXFLAGS := -std=$(STD) -Wall -Wextra -DSSDEVID="\"$(SSDEVID)\"" -DSSDEVPASSWD="\"$(SSDEVPASSWD)\"" -DSSDEVPASSWDDEBUG="\"$(SSDEVPASSWDDEBUG)\"" -DSSID="\"$(SSID)\"" -DSSPASSWD="\"$(SSPASSWD)\""
LDFLAGS  := 
LDLIBS   := -lcurl -lcrypto -lz -ltinyxml2

ifeq ($(BUILD),release)
    CXXFLAGS += -O2 -DNDEBUG
else
    CXXFLAGS += -g -O0 -DDEBUG
endif

# Ajoutez ici des flags supplémentaires si nécessaire, par ex :
# CXXFLAGS += -fsanitize=address     # pour debugging
# LDLIBS  += -lpthread                # si votre code utilise pthreads

.PHONY: all debug release clean distclean run info

all: $(BIN)

# Link
# $(BIN): $(OBJ)
# 	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

scraper: $(OBJ-SCRAPER)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

crc: $(OBJ-CRC)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Targets helpers
debug:
	$(MAKE) BUILD=debug all

release:
	$(MAKE) BUILD=release all

run: $(BIN)
	./$(BIN) $$SSDEVID $$SSDEVPASSWD $$SSID $$SSPASSWD ./GBA

clean:
	$(RM) $(OBJ-SCRAPER) $(OBJ-CRC) *~

distclean: clean
	$(RM) $(BIN)

info:
	@echo "CXX      = $(CXX)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "LDLIBS   = $(LDLIBS)"
	@echo "SRC-SCRAPER      = $(SRC-SCRAPER)"
	@echo "SRC-CRC      = $(SRC-CRC)"
	@echo "OBJ-SCRAPER      = $(OBJ-SCRAPER)"
	@echo "OBJ-CRC      = $(OBJ-CRC)"
	@echo "BIN      = $(BIN)"
