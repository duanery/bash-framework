SCRIPT := $(wildcard *.sh)
HEADER := $(wildcard *.h) $(wildcard *.cls)
BIN := inout
OTHER := Readme.txt Makefile cache

install: $(SCRIPT) $(HEADER) $(BIN)
	@chmod +x $(SCRIPT) $(BIN)
	@chmod -x $(HEADER)
	@mkdir -p cache

$(BIN):%:%.c
	@echo CC $@
	@gcc -s -o $@ $^

