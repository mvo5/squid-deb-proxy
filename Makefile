#!/usr/bin/make

all:
	echo "nothing to do"

install:
	# dir
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy
	# files
	install -m 644 squid-deb-proxy.conf $(DESTDIR)/etc/squid-deb-proxy/
	install -m 644 allowed-networks-src.acl $(DESTDIR)/etc/squid-deb-proxy/
	install -m 644 mirror-dstdomain.acl $(DESTDIR)/etc/squid-deb-proxy/