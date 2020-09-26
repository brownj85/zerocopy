#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>

#include "zcopy.h"
#include "zcopy_private.h"

int zCopySocket_init(struct zCopySocket *sock, int domain){
    int ret;

    memset(sock, 0, sizeof(*sock));
    zcopy_private_init(&sock->_priv);
    
    if (domain != AF_INET6 && domain != AF_INET){
        return EAFNOSUPPORT;
    }

    sock->fd = socket(domain, SOCK_STREAM, 0);
    if (sock->fd == -1){
        return errno;
    }
    
    ret = 1;
    ret = setsockopt(sock->fd, SOL_SOCKET, SO_ZEROCOPY, &ret, sizeof(ret));
    if (ret < 0){
        if (errno == 524){
            return ENOTSUP; //Linux kernel uses not-standard error codes
        }
        return errno;
        close(sock->fd);
    }

    return 0;

}

int zCopySocket_send(struct zCopySocket *sock, const void *buf, size_t len, int flags){
    ssize_t tmp;

    tmp = 0;

    tmp = send(sock->fd, buf, len, MSG_ZEROCOPY | flags);
    if (tmp < 0){
        perror("send");
        zcopy_private_seterror(&sock->_priv, -errno, "send() call failed");
        return -1;
    }

    sock->_priv.writes_queued += 1;

    return 0;
}

static int handle_message(struct zCopySocket *sock){
    ssize_t tmp;
    
    struct zcopy_private_msgbuf msgbuf;
    struct msghdr *msg;
    struct cmsghdr *cmsg;
    struct sock_extended_err *serr;

    zcopy_private_msgbuf_init(&msgbuf);

    msg = &msgbuf.msg;

    tmp = recvmsg(sock->fd, msg, MSG_ERRQUEUE);
    if (tmp < 0){
        zcopy_private_seterror(&sock->_priv, -errno, "zCopySocket_drain(): recvmsg()");
        return -1;
    }

    cmsg = CMSG_FIRSTHDR(msg);
    serr = (void *) CMSG_DATA(cmsg);

    if (serr->ee_errno != 0){
        zcopy_private_seterror(&sock->_priv, -serr->ee_errno, "zCopySocket_drain(): Received error from socket message queue");
        return -1;
    }else if (serr->ee_origin != SO_EE_ORIGIN_ZEROCOPY){
        return 1;
    }

    return 0;
};

int zCopySocket_drain(struct zCopySocket *sock, int timeout_ms){
    int ret = 0;

    if (!sock->_priv.writes_queued){
        return 0;
    }

    struct pollfd pfd;
    pfd.fd = sock->fd;
    pfd.events = 0;

    ret = poll(&pfd, 1, timeout_ms);
    if (ret < 0){
        zcopy_private_seterror(&sock->_priv, -errno, "zCopySocket_drain(): poll()");
        return -1;
    }else if (ret == 0){
        return 1;
    }

    ret = handle_message(sock);

    if (ret == 0){
        sock->_priv.writes_queued = 0;
        zcopy_private_seterror(&sock->_priv, 0, NULL);
    }

    return ret;
}

int zCopySocket_error(const struct zCopySocket *sock){
    return sock->_priv.error;
}

const char *zCopySocket_strerror(const struct zCopySocket *sock){
    return sock->_priv.error_context;
}
