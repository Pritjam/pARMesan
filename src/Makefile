CC = gcc
CC_FLAGS = -O3 -Wall -ggdb -I../lib 
CC_OPTIONS = -c
RM = rm -f
MD = gccmakedep

SRCS := archsim.c binloader.c hw_elts.c logging.c proc.c system.c vct.c
OBJS := $(SRCS:%.c=%.o)

all: parmesan clean

%.o: %.c
	${CC} ${CC_OPTIONS} ${CC_FLAGS} $<

parmesan: ${OBJS}


clean:
	$(RM) *.o

depend:
	${MD} -- ${CC_OPTIONS} ${CC_FLAGS} -- ${SRCS}