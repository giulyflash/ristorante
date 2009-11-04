#include	"basic.h"


ssize_t
Write(int fd, void *ptr, size_t nbytes);

ssize_t
Read(int fd, void *ptr, size_t nbytes);

ssize_t
Read2(int fd, void *vptr, size_t maxlen);

int
Socket(int family, int type, int protocol);

void
Listen(int fd, int backlog);

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen);

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen);

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr);

void
Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);

struct hostent *
Gethostbyname(char *nome);

pid_t
Fork(void);

void
Close(int fd);

typedef void (*sighandler_t)(int);

sighandler_t
Signal(int signum, sighandler_t handler);

