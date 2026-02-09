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

# ---- Find all source files recursively ----
SRC := $(shell find Libs Server Server/Connection Server/Log Libs/Algorithm Libs/Utils -name "*.c")

# Convert .c → build/.../.o
OBJ := $(patsubst %.c, $(BUILD)/%.o, $(SRC))

# ---- Final target ----
TARGET := glen

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---- Rule to compile each .c file ----
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@



DEBUG_BUILD := build_debug
DEBUG_TARGET := gln_app_debug
DEBUG_FLAGS := -g -O0 -DDEBUG

debug: CFLAGS += $(DEBUG_FLAGS)
debug: BUILD := $(DEBUG_BUILD)
debug: OBJ := $(patsubst %.c, $(DEBUG_BUILD)/%.o, $(SRC))
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 




# ---- Cleanup ----
clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean