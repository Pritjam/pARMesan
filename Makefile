define newline


endef

all: clean build

build:
	@gcc -O3 -I lib lib/*.h src/*.c -o parmesan

debug: 
	@gcc -g -I lib lib/*.h src/*.c -o parmesan
clean:
	@rm -f parmesan *.bak
	@rm -rf output/

evaluate: clean build
	@python3 run_tests.py --all