build_path := $(CURDIR)/build
src_path := $(CURDIR)/src
target := $(build_path)/lcss

.PHONY: exe
exe: build
	@cd $(src_path) && gcc -O3 *.c -o $(target)

build:
	@mkdir -p $(build_path)

all: exe
