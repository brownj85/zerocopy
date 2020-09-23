#ifndef ZCOPY_H
#define ZCOPY_H

#ifdef ZCOPYSOCKET_PYTHON
#include <Python.h>
#endif

#include "zcopy_private.h"
#include <netinet/in.h>

struct zCopySocket {
    int fd;
    struct _zcopy_private _priv;
};

int zCopySocket_init(struct zCopySocket *sock, int socket);
int zCopySocket_drain(struct zCopySocket *sock, int timeout_ms);
int zCopySocket_send(struct zCopySocket *sock, const void *data, size_t size, int flags);

int zCopySocket_error(const struct zCopySocket *sock);
const char *zCopySocket_strerror(const struct zCopySocket *sock);

#ifdef ZCOPYSOCKET_PYTHON
int zCopySocket_get_fd(const struct zCopySocket *sock);

int zCopySocket_send_python_buffer(struct zCopySocket *sock, PyObject *obj);

size_t zCopySocket_size();
#endif

#endif
