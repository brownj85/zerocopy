#include <stdio.h>
#include <sys/types.h>

#include <errno.h>
#include <string.h>

#include "zcopy_private.h"

void zcopy_private_init(struct zcopy_private *priv){
    memset(priv, 0, sizeof(*priv));
}

void zcopy_private_msgbuf_init(struct zcopy_private_msgbuf *msgbuf){
    memset(msgbuf, 0, sizeof(*msgbuf));

    msgbuf->msg.msg_control = &msgbuf->cmsg;
    msgbuf->msg.msg_controllen = sizeof(msgbuf->serr) + sizeof(msgbuf->extra);
};


void zcopy_private_seterror(struct zcopy_private *priv, int error, const char *context){
    priv->error = error;
    priv->error_context[0] = '\0';

    if (error == 0){
       return;
    }

    if (context != NULL){
        snprintf(priv->error_context, sizeof(priv->error_context), "%s", context);
    }

};
