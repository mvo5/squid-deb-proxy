#!/usr/bin/make

all:
	echo "nothing to do"

install:
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy
	install -m 644 squid-deb-proxy.conf $(DESTDIR)/etc/squid-deb-proxy/squid-deb-proxy.conf
