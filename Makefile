define newline


endef

all: clean build

build:
	@gcc -g -I lib lib/*.h src/*.c -o parmesan

clean:
	@rm -f parmesan *.bak

tests = $(wildcard tests/bin/*)
run_all_tests = $(foreach file, $(tests), ./parmesan $(file) $(newline) )
evaluate: clean build
	$(run_all_tests)