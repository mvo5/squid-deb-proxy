/* $Id$ */

/***
  This file is based on an example that is part of avahi, which is copyright:
  Lennart Poettering <lennart (at) poettering (dot) de>
  Trent Lloyd <lathiat@bur.st>
  Sebastien Estienne <sebastien.estienne@gmail.com>
  Jakub Stachowski
  James Willcox <snorp@snorp.net>
  Collabora Ltd.
  Modifications for eucalyptus-udeb are copyright 2009 Canonical Ltd.
  Modifications for squid-deb-proxy-udeb are copyright 2011 Canonical Ltd.
 
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
 
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <avahi-core/core.h>
#include <avahi-core/lookup.h>
#include <avahi-core/log.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include <debian-installer.h>

static AvahiSimplePoll *simple_poll = NULL;
static AvahiServer *server = NULL;

static int debug = 0;

static void quiet_logger(AvahiLogLevel level, const char *txt) {
}

static void resolve_callback(
    AvahiSServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
    AvahiLookupResultFlags flags,
    AVAHI_GCC_UNUSED void* userdata) {

    assert(r);

    /* Called whenever a service has been resolved successfully or timed out */

    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            if (debug)
                fprintf(stderr, "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_server_errno(server)));
            break;

        case AVAHI_RESOLVER_FOUND: {
            char *human_address = avahi_malloc0(AVAHI_ADDRESS_STR_MAX);
            char *key = NULL;
            AvahiStringList *ipaddr_entry;
            char *ipaddr_key, *ipaddr_value;

            ipaddr_entry = avahi_string_list_find(txt, "ipaddr");
            if (ipaddr_entry && avahi_string_list_get_pair(ipaddr_entry, &ipaddr_key, &ipaddr_value, NULL) == 0) {
                key = avahi_strdup_printf("%s:%u", ipaddr_value, port);
                avahi_free(ipaddr_value);
                avahi_free(ipaddr_key);
            } 
            else if (avahi_address_snprint(human_address, AVAHI_ADDRESS_STR_MAX, address)) {
                if (address->proto == AVAHI_PROTO_INET6)
                    key = avahi_strdup_printf("[%s]:%u", human_address, port);
                else if (strncmp(human_address, "169.254.169.254", 15) == 0)
                    key = avahi_strdup_printf("%s:%u", name, port);
                else
                    key = avahi_strdup_printf("%s:%u", human_address, port);
            } else {
                if (debug)
                    fprintf(stderr, "(Resolver) failed to resolve %s to IP address/port\n", key);
            }
            avahi_free(human_address);

            printf("http://%s/\n", key);
        }
    }

    avahi_s_service_resolver_free(r);
}

static void browse_callback(
    AvahiSServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) {

    AvahiServer *s = userdata;
    assert(b);

    /* Called whenever a new services becomes available on the LAN or is removed from the LAN */

    switch (event) {

        case AVAHI_BROWSER_FAILURE:
            fprintf(stderr, "(Browser) %s\n", avahi_strerror(avahi_server_errno(server)));
            avahi_simple_poll_quit(simple_poll);
            return;

        case AVAHI_BROWSER_NEW:
            if (debug)
                fprintf(stderr, "(Browser) NEW: service '%s' of type '%s' in domain '%s'\n", name, type, domain);

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_s_service_resolver_new(s, interface, protocol, name, type, domain, AVAHI_PROTO_INET, 0, resolve_callback, s)))
                fprintf(stderr, "Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_server_errno(s)));

            break;

        case AVAHI_BROWSER_REMOVE:
            if (debug)
                fprintf(stderr, "(Browser) REMOVE: service '%s' of type '%s' in domain '%s'\n", name, type, domain);
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
            if (debug)
                fprintf(stderr, "(Browser) %s\n", "ALL_FOR_NOW");
            exit(0);
            break;

        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            if (debug)
                fprintf(stderr, "(Browser) %s\n", "CACHE_EXHAUSTED");
            break;
    }
}

int main(AVAHI_GCC_UNUSED int argc, AVAHI_GCC_UNUSED char*argv[]) {
    AvahiServerConfig config;
    AvahiSServiceBrowser *sb = NULL;
    int error;
    int ret = 1;

    if (getenv("SQUID_DEB_PROXY_FIND_DEBUG"))
        debug = 1;

    /* Initialize the pseudo-RNG */
    srand(time(NULL));

    if (!debug)
	avahi_set_log_function(quiet_logger);

    /* Allocate main loop object */
    if (!(simple_poll = avahi_simple_poll_new())) {
        fprintf(stderr, "Failed to create simple poll object.\n");
        goto fail;
    }

    /* Do not publish any local records */
    avahi_server_config_init(&config);
    config.publish_hinfo = 0;
    config.publish_addresses = 0;
    config.publish_workstation = 0;
    config.publish_domain = 0;

    /* Allocate a new server */
    server = avahi_server_new(avahi_simple_poll_get(simple_poll), &config, NULL, NULL, &error);

    /* Free the configuration data */
    avahi_server_config_free(&config);

    /* Check whether creating the server object succeeded */
    if (!server) {
        fprintf(stderr, "Failed to create server: %s\n", avahi_strerror(error));
        goto fail;
    }

    /* Create the service browser */
    if (!(sb = avahi_s_service_browser_new(server, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_apt_proxy._tcp", NULL, 0, browse_callback, server))) {
        fprintf(stderr, "Failed to create service browser: %s\n", avahi_strerror(avahi_server_errno(server)));
        goto fail;
    }

    /* Run the main loop */
    avahi_simple_poll_loop(simple_poll);

    ret = 0;

fail:

    /* Cleanup things */
    if (sb)
        avahi_s_service_browser_free(sb);

    if (server)
        avahi_server_free(server);

    if (simple_poll)
        avahi_simple_poll_free(simple_poll);

    return ret;
}
