DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -g -DNODEBUG
endif

COVERAGE ?= 0
ifeq ($(COVERAGE), 1)
    CFLAGS += --coverage
    LDFLAGS += --coverage
endif

BINARY = t411-daemon
CFLAGS += -Werror -W -Wall -pedantic -Wformat -Wformat-security -Wextra
CFLAGS += -Wextra -Wno-long-long -Wno-variadic-macros
CFLAGS += -std=gnu99
LDFLAGS += -lcurl
HEADERS += -I./include
CC=gcc

all: $(BINARY)

install: all
	touch /etc/$(BINARY).conf
	echo "username XXXXX" > /etc/$(BINARY).conf
	echo "password XXXXX" >> /etc/$(BINARY).conf
	echo "mail X@Y.Z" >> /etc/$(BINARY).conf
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
	find . -name "*.gcda" -print0 | xargs -0 rm -f
	find . -name "*.gcno" -print0 | xargs -0 rm -f
	rm -f bin/$(BINARY)

mrproper: clean
	rm -rf bin
	rm -rf $(BINARY)/etc
	rm -rf doc
	rm -rf gcov

prepare-pkg: all
	mkdir -p $(BINARY)/etc/init.d
	cp bin/$(BINARY) $(BINARY)/etc/init.d/.

build-pkg: prepare-pkg
	sudo dpkg-deb --build $(BINARY)

doc:
	mkdir -p doc
	doxygen -g doc/t411_doxygen.conf
	sed -i 's/My Project/t411-daemon/' doc/t411_doxygen.conf
	sed -i 's/INPUT                  =/INPUT                  = ..\/src ..\/include/' doc/t411_doxygen.conf
	sed -i 's/FILE_PATTERNS          =/FILE_PATTERNS          = *.c *.h/' doc/t411_doxygen.conf
	cd doc && doxygen -w html header.html footer.html stylesheet.css t411_doxygen.conf
	cd doc && doxygen t411_doxygen.conf

test:
	make clean && make DEBUG=1 && sh test/check.sh

gcov:
	make mrproper && make DEBUG=1 COVERAGE=1
	sh test/check.sh
	rm -rf gcov && mkdir -p gcov
	lcov --capture --directory . --output-file gcov/$(BINARY).info
	lcov --remove gcov/$(BINARY).info /usr/include/\* -o gcov/$(BINARY).info
	genhtml gcov/$(BINARY).info --output-directory gcov/result

.PHONY: doc test gcov
