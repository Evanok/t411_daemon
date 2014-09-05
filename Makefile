DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -g -DNODEBUG
endif

BINARY = t411-daemon
CFLAGS += -Werror -W -Wall -pedantic -Wformat -Wformat-security -Wextra
CFLAGS += -Wextra -Wno-long-long -Wno-variadic-macros
LDFLAGS += -lcurl
HEADERS += -I./include
CC=c99

all: $(BINARY)

install: all
	touch /etc/$(BINARY).conf
	echo "username XXXXX" > /etc/$(BINARY).conf
	echo "password XXXXX" >> /etc/$(BINARY).conf
	chmod 777 /etc/$(BINARY).conf
	cp bin/$(BINARY) /usr/bin/.

uninstall:
	rm -rf /etc/$(BINARY).conf
	rm -rf /usr/bin/$(BINARY)

$(BINARY): src/t411_daemon.o src/config.o src/message.o
	mkdir -p bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(HEADERS)
clean:
	find . -name "*.o" -print0 | xargs -0 rm -f
	rm -f bin/$(BINARY)

mrproper: clean
	rm -rf bin
	rm -rf $(BINARY)/etc

prepare-pkg: all
	mkdir -p $(BINARY)/etc/init.d
	cp bin/$(BINARY) $(BINARY)/etc/init.d/.

build-pkg: prepare-pkg
	sudo dpkg-deb --build $(BINARY)

.PHONY:
