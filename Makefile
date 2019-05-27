BUILD_DIR := $(CURDIR)/build
SRC_DIR := $(CURDIR)/src
BIN_TARGET := $(BUILD_DIR)/lcss

.PHONY: exe
exe: build
	@cd $(SRC_DIR) && gcc -O3 *.c -o $(BIN_TARGET)

build:
	@mkdir -p $(BUILD_DIR)

all: exe
