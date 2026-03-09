
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L 
LDFLAGS := -lcurl -ljansson -lpthread
BUILD   := build

# Add include directories
CFLAGS  += -ILibs \
           -ILibs/Utils \
           -IServer \
		   -IServer/Connection \
		   -IServer/Log \
		   -ILibs/Algorithm \


# ============================================
# Source files
# ============================================
SRC := $(shell find Libs Server -name "*.c")
OBJ := $(patsubst %.c, $(BUILD)/%.o, $(SRC))

# ---- Final target ----
PREFIX ?= /usr/local/bin
#TARGET := Glennergy-Main

# 1) Server
SRC_SERVER := $(shell find Server Libs -name "*.c" ! -name "main_algorithm.c" ! -path "Cache/*")
OBJ_SERVER := $(patsubst %.c,$(BUILD)/%.o,$(SRC_SERVER))
TARGET_SERVER := Glennergy-Server

# 2) Algorithm
SRC_ALGO := $(shell find Libs/Algorithm -name "*.c") Libs/Sockets.c Server/Log/Logger.c
OBJ_ALGO := $(patsubst %.c,$(BUILD)/%.o,$(SRC_ALGO))
TARGET_ALGO := Glennergy-Algorithm

# 3) Cache (InputCache)
SRC_CACHE := $(shell find Cache -name "*.c") Libs/Sockets.c Server/Log/Logger.c Server/SignalHandler.c Libs/Pipes.c Libs/Homesystem.c
OBJ_CACHE := $(patsubst %.c,$(BUILD)/%.o,$(SRC_CACHE))
TARGET_CACHE := Glennergy-Cache

INSTALLDIR = $(PREFIX)

all: $(TARGET_SERVER) $(TARGET_ALGO) $(TARGET_CACHE)

$(TARGET_SERVER): $(OBJ_SERVER)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $@"

$(TARGET_ALGO): $(OBJ_ALGO)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Algorithm build complete: $@"

$(TARGET_CACHE): $(OBJ_CACHE)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Cache build complete: $@"

#all: $(TARGET)
#
#$(TARGET): $(OBJ)#
#	@echo "Linking $@..."	
#	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
#	@echo "Build complete: $@"
#
# ---- Rule to compile each .c file ----
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================
# Debug build
# ============================================
DEBUG_BUILD := build_debug
DEBUG_FLAGS := -g -O0 -DDEBUG

debug: CFLAGS += $(DEBUG_FLAGS)
debug: BUILD := $(DEBUG_BUILD)
debug: all_debug

all_debug: $(DEBUG_TARGET_SERVER) $(DEBUG_TARGET_ALGO) $(DEBUG_TARGET_CACHE)

DEBUG_TARGET_SERVER := Glennergy-Server-debug
DEBUG_TARGET_ALGO   := Glennergy-Algorithm-debug
DEBUG_TARGET_CACHE  := Glennergy-Cache-debug

DEBUG_OBJ_SERVER := $(patsubst %.c,$(DEBUG_BUILD)/%.o,$(SRC_SERVER))
DEBUG_OBJ_ALGO   := $(patsubst %.c,$(DEBUG_BUILD)/%.o,$(SRC_ALGO))
DEBUG_OBJ_CACHE  := $(patsubst %.c,$(DEBUG_BUILD)/%.o,$(SRC_CACHE))

$(DEBUG_TARGET_SERVER): $(DEBUG_OBJ_SERVER)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_TARGET_ALGO): $(DEBUG_OBJ_ALGO)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_TARGET_CACHE): $(DEBUG_OBJ_CACHE)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET_SERVER) $(TARGET_ALGO) $(TARGET_CACHE)
	install -d $(DESTDIR)$(INSTALLDIR)
	install -m 755 $(TARGET_SERVER) $(DESTDIR)$(INSTALLDIR)
	install -m 755 $(TARGET_ALGO) $(DESTDIR)$(INSTALLDIR)
	install -m 755 $(TARGET_CACHE) $(DESTDIR)$(INSTALLDIR)

uninstall:
	rm -f $(DESTDIR)$(INSTALLDIR)/$(TARGET_SERVER)
	rm -f $(DESTDIR)$(INSTALLDIR)/$(TARGET_ALGO)
	rm -f $(DESTDIR)$(INSTALLDIR)/$(TARGET_CACHE)
#install:
#	# Create directory
#	install -d $(DESTDIR)$(INSTALLDIR)
#
#	# Install binary
#	install -m 755 $(TARGET) $(DESTDIR)$(INSTALLDIR)

#uninstall:
#	rm -rf $(DESTDIR)$(INSTALLDIR)/$(TARGET)


# ---- Cleanup ----
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD) $(DEBUG_BUILD)
	rm -f $(TARGET_SERVER) $(TARGET_ALGO) $(TARGET_CACHE)
	rm -f $(DEBUG_TARGET_SERVER) $(DEBUG_TARGET_ALGO) $(DEBUG_TARGET_CACHE)
	@echo "Clean complete"
#clean:
#	@echo "Cleaning build files..."
#	rm -rf $(BUILD) $(DEBUG_BUILD)
#	rm -f $(TARGET) $(DEBUG_TARGET)
#	@echo "Clean complete"

# ============================================
# Help
# ============================================



.PHONY: all clean debug install uninstall dirs run test help
