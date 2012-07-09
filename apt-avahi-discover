#!/usr/bin/python
#
# use avahi to find a _apt_proxy._tcp provider and return
# a http proxy string suitable for apt

import socket
from subprocess	import Popen, PIPE


def is_ipv6(a):
    return ':' in a


def get_proxy_host_port_from_avahi():
    service = '_apt_proxy._tcp'

    # Obtain all of the services addresses from avahi, pulling the IPv6
    # addresses to the top.
    addr4 = []
    addr6 = []
    p = Popen(['avahi-browse', '-kprt', service], stdout=PIPE)
    for line in p.stdout:
        if line.startswith('='):
            bits = line.split(';')
            if is_ipv6(bits[7]):
                addr6.append([bits[7], bits[8]])
            else:
                addr4.append([bits[7], bits[8]])

    # Run through the offered addresses and see if we we have a bound local
    # address for it.
    for (ip, port) in addr6 + addr4:
        try:
            res = socket.getaddrinfo(ip, port, 0, 0, 0, socket.AI_ADDRCONFIG)
            if res:
                return (ip, port)
        except socket.gaierror:
            pass
    # nothing found
    return None


if __name__ == "__main__":
    # Dump the approved address out in an appropriate format.
    address = get_proxy_host_port_from_avahi()
    if address:
        (ip, port) = address
        if is_ipv6(ip):
            print "http://[%s]:%s/" % (ip, port)
        else:
            print "http://%s:%s/" % (ip, port)
