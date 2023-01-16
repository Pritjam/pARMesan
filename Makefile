all: clean build

build:
	gcc -g -I lib lib/*.h src/*.c -o parmesan

clean:
	rm -f parmesan