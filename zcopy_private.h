#ifndef ZCOPY_PRIVATE_H
#define ZCOPY_PRIVATE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/errqueue.h>

struct zcopy_private;
struct zcopy_private_msgbuf;

struct zcopy_private_msgbuf {
    struct msghdr msg;

    struct cmsghdr cmsg;
    struct sock_extended_err serr;
    unsigned char extra[256];
};

struct zcopy_private {
    int error;
    char error_context[256];
    uint32_t writes_queued;
};

void zcopy_private_init(struct zcopy_private *priv);
void zcopy_private_msgbuf_init(struct zcopy_private_msgbuf *msgbuf);

void zcopy_private_seterror(struct zcopy_private *priv, int error, const char *context);

#endif
