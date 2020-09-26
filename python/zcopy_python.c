#ifdef ZCOPYSOCKET_PYTHON
#include "zcopy.h"
#include "zcopy_private.h"


int zCopySocket_get_fd(const struct zCopySocket *sock){
    return sock->fd;
}

size_t zCopySocket_size(){
    return sizeof(struct zCopySocket);
}

int zCopySocket_send_python_buffer(struct zCopySocket *sock, PyObject *obj){
    int ret;
    Py_buffer view;

    ret = PyObject_CheckBuffer(obj);
    if (ret != 1){
        zcopy_private_seterror(&sock->_priv, 1, "zCopySocket_send_python_buffer(): provided Python object does not support buffer protocol");
        return -1;
    }
    
    ret = PyObject_GetBuffer(obj, &view, PyBUF_ANY_CONTIGUOUS);
    if (ret == -1){
        zcopy_private_seterror(&sock->_priv, 1, "zCopySocket_send_python_buffer(): PyObject_GetBuffer() failed");
        return -1;
    }

    ret = zCopySocket_send(sock, view.buf, view.len, 0);
    if (ret == -1){
        PyBuffer_Release(&view);
        return -1;
    }

    ret = zCopySocket_drain(sock, -1);
    
    PyBuffer_Release(&view);

    return ret;
}

#endif
