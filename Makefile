DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -g -DNODEBUG
endif

PACKAGE_NAME = t411-daemon
BINARY = t411_daemon
CFLAGS += -Werror -W -Wall -pedantic -Wformat -Wformat-security -Wextra
CFLAGS += -Wextra -Wno-long-long -Wno-variadic-macros
LDFLAGS += -lcurl
HEADERS += -I./include
CC=c99

all: $(BINARY)

$(BINARY): src/t411_daemon.o src/config.o src/message.o
	mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(HEADERS)
clean:
	find . -name "*.o" -print0 | xargs -0 rm -f
	rm -f bin/$(BINARY)

mrproper: clean
	rm -rf bin
	rm -rf $(PACKAGE_NAME)/etc

prepare-pkg: all
	mkdir -p $(PACKAGE_NAME)/etc/init.d
	cp bin/$(BINARY) $(PACKAGE_NAME)/etc/init.d/.

build-pkg: prepare-pkg
	sudo dpkg-deb --build $(PACKAGE_NAME)

.PHONY:
