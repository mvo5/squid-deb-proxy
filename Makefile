#!/usr/bin/make

all:
	echo "nothing to do"

install:
	# dir
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy
	install -d -m 755 $(DESTDIR)/etc/apt/apt.conf.d
	install -d -m 755 $(DESTDIR)/usr/share/squid-deb-proxy-client/
	# files
	install -m 644 squid-deb-proxy.conf $(DESTDIR)/etc/squid-deb-proxy/
	install -m 644 allowed-networks-src.acl $(DESTDIR)/etc/squid-deb-proxy/
	install -m 644 mirror-dstdomain.acl $(DESTDIR)/etc/squid-deb-proxy/
	# client
	install -m 755 apt-avahi-discover $(DESTDIR)/usr/share/squid-deb-proxy-client/
	install -m 644 30autoproxy $(DESTDIR)/etc/apt/apt.conf.d
