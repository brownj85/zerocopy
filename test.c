#include "sockaddr.h"
#include "zcopy.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#define BYTES_PER_MB 1048576

void send_copy(struct zCopySocket *sock, char *test_data, size_t size){
    int ret;

    size_t sent = 0;

    while (sent < size){
        ret = send(sock->fd, &test_data[sent], size - sent, 0);
        if (ret < 0){
            perror("send");
            exit(1);
        }

        sent += ret;
    }
}
    

int main(int argc, char **argv){
    int ret;

    struct sockaddr_in6 addr;
    struct zCopySocket sock;

    size_t test_data_size = strtoll(argv[3], NULL, 10);

    ret = sockaddr_in6_init(argv[1], argv[2], &addr);
    if (ret == 1){
        printf("Invalid host: %s\n", argv[1]);
    }else if (ret == 2){
        printf("Invalid port: %s\n", argv[2]);
    }

    ret = zCopySocket_init(&sock, AF_INET6);
    if (ret){
        errno = ret;
        perror("init");
        exit(1);
    }

    ret = connect(sock.fd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1){
        perror("connect");
        exit(1);
    }

    printf("Sending %lu mb of data\n", test_data_size);

    void *test_data = malloc(test_data_size * BYTES_PER_MB);
    if (test_data == NULL){
        printf("Could not allocate space for test data: %s\n", strerror(errno));
        exit(1);
    }

    if (0){
        printf("Sending using copy\n");
        send_copy(&sock, test_data, test_data_size * BYTES_PER_MB);
        return 0;
    }
    ret = zCopySocket_send(&sock, test_data, test_data_size * BYTES_PER_MB, 0);
    if (ret != 0){
        printf("%s", zCopySocket_strerror(&sock));
        exit(1);
    }

    ret = zCopySocket_drain(&sock, -1);
    if (ret != 0){
        printf("%s\n", zCopySocket_strerror(&sock));
        exit(1);
    }

    return 0;
}
