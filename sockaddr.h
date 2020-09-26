#ifndef SOCKADDR_H
#define SOCKADDR_H

#include <netinet/in.h>


int sockaddr_in6_init(const char *host, const char *port, struct sockaddr_in6 *ret);


#endif
