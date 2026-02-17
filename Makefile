PROJECT := glennergy
VERSION := 1.0

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


#====================================================
# Directory structure: (XDG-compliant)
#====================================================

#detect if running as root or user
ifeq ($(shell id -u), 0)
# System-level paths
    PREFIX        := /usr/local
    CONF_DIR      := /etc/$(PROJECT)
    DATA_DIR      := /var/lib/$(PROJECT)
    CACHE_DIR     := /var/cache/$(PROJECT)
    LOG_DIR       := /var/log/$(PROJECT)
    STATE_DIR     := /var/lib/$(PROJECT)
    RUN_DIR       := /run/$(PROJECT)
	else
# User-level paths
    PREFIX        := $(HOME)/.local
    XDG_CONFIG    := $(or $(XDG_CONFIG_HOME),$(HOME)/.config)		#
    XDG_DATA      := $(or $(XDG_DATA_HOME),$(HOME)/.local/share)
    XDG_CACHE     := $(or $(XDG_CACHE_HOME),$(HOME)/.cache)
    XDG_STATE     := $(or $(XDG_STATE_HOME),$(HOME)/.local/state)
    
    CONF_DIR      := $(XDG_CONFIG)/$(PROJECT)
    DATA_DIR      := $(XDG_DATA)/$(PROJECT)
    CACHE_DIR     := $(XDG_CACHE)/$(PROJECT)
    LOG_DIR       := $(XDG_STATE)/$(PROJECT)
    STATE_DIR     := $(XDG_STATE)/$(PROJECT)
    RUN_DIR       := /tmp/$(PROJECT)-$(shell id -u)
endif

BIN_DIR	   := $(PREFIX)/bin

# ============================================
# Source files
# ============================================
SRC := $(shell find Libs Server -name "*.c")
OBJ := $(patsubst %.c, $(BUILD)/%.o, $(SRC))

# ============================================
# Targets
# ============================================

TARGET := glen

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "Linking $@..."	
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $@"

# ---- Rule to compile each .c file ----
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================
# Debug build
# ============================================

DEBUG_BUILD := build_debug
DEBUG_TARGET := gln_app_debug
DEBUG_FLAGS := -g -O0 -DDEBUG

debug: CFLAGS += $(DEBUG_FLAGS)
debug: BUILD := $(DEBUG_BUILD)
debug: OBJ := $(patsubst %.c, $(DEBUG_BUILD)/%.o, $(SRC))
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(OBJ)
	@echo "Linking $@(debug)..."	
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Debug build complete: $@"

$(DEBUG_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 

# ============================================
# Directory structure
# ============================================

dirs:
	@echo "Creating directory structure..."
	@echo "  Config:  $(CONF_DIR)"
	@echo "  Data:    $(DATA_DIR)"
	@echo "  Cache:   $(CACHE_DIR)"
	@echo "  Logs:    $(LOG_DIR)"
	@echo "  State:   $(STATE_DIR)"
	@echo "  Runtime: $(RUN_DIR)"
	@mkdir -p $(CONF_DIR)
	@mkdir -p $(DATA_DIR)/history_meteo
	@mkdir -p $(DATA_DIR)/history_spotpris
	@mkdir -p $(DATA_DIR)/homesystems
	@mkdir -p $(CACHE_DIR)/cache_meteo
	@mkdir -p $(CACHE_DIR)/cache_spotpris
	@mkdir -p $(LOG_DIR)
	@mkdir -p $(STATE_DIR)
	@mkdir -p $(RUN_DIR)
	@echo "Directories created"

# ============================================
# Installation
# ============================================

install: all dirs
	@echo "Installing $(PROJECT)..."
	@mkdir -p $(BIN_DIR)
	install -m 755 $(TARGET) $(BIN_DIR)/
	@echo "Installed to $(BIN_DIR)/$(TARGET)"
	@echo ""
	@echo "Installation complete!"
	@echo "  Binary:  $(BIN_DIR)/$(TARGET)"
	@echo "  Config:  $(CONF_DIR)"
	@echo "  Logs:    $(LOG_DIR)"
	@echo ""
	@echo "Run with: $(TARGET)"

uninstall:
	@echo "Uninstalling $(PROJECT)..."
	rm -f $(BIN_DIR)/$(TARGET)
	@echo "Binary removed"
	@echo ""
	@echo "Data preserved in:"
	@echo "  $(CONF_DIR)"
	@echo "  $(DATA_DIR)"
	@echo "  $(CACHE_DIR)"
	@echo "  $(LOG_DIR)"
	@echo ""
	@echo "To remove all data: rm -rf $(CONF_DIR) $(DATA_DIR) $(CACHE_DIR) $(LOG_DIR)"

# ============================================
# Development
# ============================================

run: $(TARGET) dirs
	@echo "Running $(TARGET)..."
	./$(TARGET)

test: dirs
	@echo "Running tests..."
	@if [ -f tests/test_cache.c ]; then \
		$(CC) $(CFLAGS) tests/test_cache.c cache/cache.c server/log/logger.c \
		      $(LDFLAGS) -o tests/test_cache && \
		./tests/test_cache; \
	fi

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD) $(DEBUG_BUILD)
	rm -f $(TARGET) $(DEBUG_TARGET)
	@echo "Clean complete"

# ============================================
# Help
# ============================================

help:
	@echo "$(PROJECT) Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make              Build the project"
	@echo "  make debug        Build with debug symbols"
	@echo "  make dirs         Create directory structure"
	@echo "  make install      Install to system/user directories"
	@echo "  make uninstall    Remove installed binary"
	@echo "  make run          Build and run"
	@echo "  make test         Run tests"
	@echo "  make clean        Remove build artifacts"
	@echo "  make help         Show this help"
	@echo ""
	@echo "Directory Structure:"
	@echo "  Binary:  $(BIN_DIR)"
	@echo "  Config:  $(CONF_DIR)"
	@echo "  Data:    $(DATA_DIR)"
	@echo "  Cache:   $(CACHE_DIR)"
	@echo "  Logs:    $(LOG_DIR)"
	@echo "  State:   $(STATE_DIR)"
	@echo ""
	@echo "Examples:"
	@echo "  make                # Build"
	@echo "  make dirs           # Create directories"
	@echo "  make run            # Build, create dirs, and run"
	@echo "  make install        # Install as user"
	@echo "  sudo make install   # Install system-wide"

#.DEFAULT_GOAL := all

.PHONY: all clean debug install uninstall dirs run test help
