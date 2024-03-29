#!/usr/bin/make

all:
	echo "nothing to do"

check:
	flake8 apt-avahi-discover

install:
	# dir
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy/allowed-networks-src.acl.d
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy/mirror-dstdomain.acl.d
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy/pkg-blacklist.d
	install -d -m 755 $(DESTDIR)/etc/squid-deb-proxy/autogenerated
	install -m 644 README.autogenerated $(DESTDIR)/etc/squid-deb-proxy/autogenerated
	install -d -m 755 $(DESTDIR)/etc/apt/apt.conf.d
	install -d -m 755 $(DESTDIR)/usr/share/squid-deb-proxy-client/
	# files
	install -m 644 squid-deb-proxy.conf $(DESTDIR)/etc/squid-deb-proxy/
	install -m 644 allowed-networks-src.acl $(DESTDIR)/etc/squid-deb-proxy/
	install -m 644 mirror-dstdomain.acl $(DESTDIR)/etc/squid-deb-proxy/
	# client
	install -m 755 apt-avahi-discover $(DESTDIR)/usr/share/squid-deb-proxy-client/
	install -m 644 30autoproxy $(DESTDIR)/usr/share/squid-deb-proxy-client/
