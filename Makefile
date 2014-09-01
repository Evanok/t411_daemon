DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -g -DNODEBUG
endif

BINARY = t411_daemon
CFLAGS += -Werror -W -Wall -pedantic -Wformat -Wformat-security -Wextra
CFLAGS += -Wextra -Wno-long-long -Wno-variadic-macros
LDFLAGS += -lcurl
HEADERS += -I./include
CC=c99

all: $(BINARY)

$(BINARY): src/t411_daemon.o src/config.o src/message.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(HEADERS)
clean:
	find . -name "*.o" -print0 | xargs -0 rm -f

mrproper: clean
	rm -f $(BINARY)

.PHONY:
