#ifndef ZCOPY_PRIVATE_H
#define ZCOPY_PRIVATE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/errqueue.h>

struct _zcopy_private;
struct _zcopy_private_msgbuf;

struct _zcopy_private_msgbuf {
    struct msghdr msg;

    struct cmsghdr cmsg;
    struct sock_extended_err serr;
    unsigned char extra[256];
};

struct _zcopy_private {
    uint32_t writes_queued;
    const void *buffer_queued;

    int error;
    char error_context[256];
};

void zcopy_private_init(struct _zcopy_private *priv);
void zcopy_private_msgbuf_init(struct _zcopy_private_msgbuf *msgbuf);

void zcopy_private_seterror(struct _zcopy_private *priv, int error, const char *context);

#endif
