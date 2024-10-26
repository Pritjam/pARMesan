CC = gcc
CC_FLAGS = -O3 -Wall -ggdb -Ilib 
RM = rm -f


all: tidy parmesan clean

parmesan:
	(cd src && make $@)
	${CC} ${CC_FLAGS} -o parmesan src/*.o

tidy:
	$(RM) parmesan

clean:
	(cd src && make $@)
	$(RM) *.bak *.vct
	$(RM) -r output/

evaluate: tidy parmesan clean
	python3 run_tests.py --all