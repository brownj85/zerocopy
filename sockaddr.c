#include "sockaddr.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static int set_port(const char *port, struct sockaddr_in6 *addr)
{
    int ret;
    
    errno = 0;
    ret = strtol(port, NULL, 10);

    if (errno != 0){
        return -1;
    }

    if (ret < 0 || ret > 65535){
        return -1;
    }

    addr->sin6_port = htons(ret);

    return 0;
}


static int set_host(const char *host, struct sockaddr_in6 *addr)
{
    int ret;
    char map_buf[40];

    if (host == NULL){
        struct in6_addr tmp = IN6ADDR_ANY_INIT;

        addr->sin6_addr = tmp;
        return 0;
    }

    ret = inet_pton(AF_INET6, host, &addr->sin6_addr);
    if (ret == 0){
        snprintf(map_buf, sizeof(map_buf), "::FFFF:%s", host);
        ret = inet_pton(AF_INET6, map_buf, &addr->sin6_addr);
    }

    if (ret == 0){
        return -1;
    }

    return 0;
}

int sockaddr_in6_init(const char *host, const char *port, struct sockaddr_in6 *addr)
{
    int ret;
    memset(addr, 0, sizeof(*addr));
    
    addr->sin6_family = AF_INET6;

    ret = set_host(host, addr);
    if (ret == -1){
        return 1;
    }

    ret = set_port(port, addr);
    if (ret == -1){
        return 2;
    }

    return 0;
}
