#ifndef HOOTCP_H
#define HOOTCP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

#define MAX_HOST_ADDR_LEN 32

typedef enum { FALSE = 0, TRUE = 1 } BOOL;


extern int htcp_listen(int port, int queue);

extern int htcp_accept(int fd, struct sockaddr *sockAddr, socklen_t *addrLen);

extern int htcp_connect(const char *host, int port);

extern int htcp_close(int fd);

extern int htcp_send(int fd, char *buf, int len);

extern int htcp_recv(int fd, char *buf, int len);

#endif
