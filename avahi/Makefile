CFLAGS := $(shell pkg-config --cflags avahi-core) -g -Wall
LDFLAGS := $(shell pkg-config --libs avahi-core) -ldebian-installer

all: find_squid_deb_proxy

find_squid_deb_proxy: find_squid_deb_proxy.c
	$(CC) $(CFLAGS) find_squid_deb_proxy.c -o find_squid_deb_proxy $(LDFLAGS)

clean:
	rm -f find_squid_deb_proxy
