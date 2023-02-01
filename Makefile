all: clean build

build:
	@gcc -g -I lib lib/*.h src/*.c -o parmesan

test:
	@cd tests; make -s clean
	@cd tests; make -s all

clean:
	@rm -f parmesan
	