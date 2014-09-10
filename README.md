Squid-deb-proxy
===============

This package provides a squid proxy config optimized for caching
deb packages on Debian and Ubuntu systems.

Quick Start
-----------

In a simple setup all you need to do is to install "squid-deb-proxy"
on your server and "squid-deb-proxy-client" on your clients. If all
clients are in the same (private) network apt will automatically
find the proxy server and use it. You can verify this by looking at
the logs in /var/log/squid-deb-proxy/access.log on your server.


Setup
-----

### Client setup

If you can't use squid-deb-proxy-client you can configure the
client manually. The sources.list should point to your prefered
mirror (no change needed usually). Then add Acquire::http::Proxy
to your apt configuration, e.g. via:

    # echo 'Acquire::http::Proxy "http://proxy-server-hostname:8000/";' >
         /etc/apt/apt.conf.d/squid-deb-proxy.conf


### Default Server Config

It should work out of the box on port 8000 for most users.

The default config will only allow fetching official Debian or
Ubuntu/Canonical packages (Ubuntu destinations on a Ubuntu server,
Debian destinations on a Debian server).

You can add more repositories either via the whitelist file
"mirror-dstdomain.acl" or by tweaking the global configuration
"squid-deb-proxy.conf" to allow fetching from any domain (see the
http_access and cache options).

By default from any request from a private (10.0.0.0/8, 172.16.0.0/12,
192.168.0.0/16) network is granted, everything else is denied.

#### Ubuntu

It will allow access to .archive.ubuntu.com, archive.canonical.com,
extras.ubuntu.com and changelogs.ubuntu.com.

#### Debian

By default it allow access to .debian.org, cdn.debian.net and
http.debian.net from any prvate network

### Customizing

To customize the settings for the allowed origins or destinations you
can edit /etc/squid-deb-proxy/*.acl.

To customize the general config, please edit the file
/etc/squid-deb-proxy/squid-deb-proxy.conf

