#include	"wrapper.h"

/* miei wrapper i/o */


ssize_t
Write(int fd, void *ptr, size_t nbytes) {
	ssize_t n;
  	n=write(fd, ptr, nbytes);
	if ( n == 0) {
		if(errno==EINTR)
			err_sys("EINTR: write error\n");
		if(errno==EPIPE)
			err_sys("EPIPE: chiusura anomala\n");
	}
	if( n < 0){
		perror("cameriere disperso\n");
	}
	return(n);
}

ssize_t
Write2(int fd, void *ptr, size_t nbytes) {
	ssize_t n;
  	n=write(fd, ptr, nbytes);
	if ( n == 0) {
		if(errno==EINTR)
			err_sys("EINTR: write error\n");
		if(errno==EPIPE)
			err_sys("EPIPE: chiusura anomala\n");
	}
	if( n < 0){
		printf("cameriere disperso\n");
	}
	return(n);
}

ssize_t
Read(int fd, void *vptr, size_t maxlen) {
	ssize_t		n;
	n = readline(fd, vptr, maxlen);
	if (n == 0) {
			err_sys("EOF sul socket");
	}
	if (n < 0) {
			err_sys("read error");
	}
	return(n);
}

ssize_t
Read2(int fd, void *vptr, size_t maxlen) {
	ssize_t		n;
	n = read(fd, vptr, maxlen);
	if (n < 0) {
			err_sys("read error");
	}
	return(n);
}

/* miei wrapper socket */

int
Socket(int family, int type, int protocol) {
	int		n;
	n = socket(family, type, protocol);
	if (n < 0)
		err_sys("socket error");
	return(n);
}

void
Listen(int fd, int backlog) {
	if(listen(fd, backlog) < 0)
		err_sys("listen error");
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (bind(fd, sa, salen) < 0)
		err_sys("bind error");
}

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
	int		n;
	n = accept(fd, sa, salenptr);
	if ( n < 0)
			err_sys("accept error");
	return(n);
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (connect(fd, sa, salen) < 0)
		err_sys("connect error");
}

void
Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr) {
	if (getpeername(fd, sa, salenptr) < 0)
		err_sys("getpeername error");
}

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr) {
	if (getsockname(fd, sa, salenptr) < 0)
		err_sys("getsockname error");
}

struct hostent *
Gethostbyname(char *nome){
	struct hostent		*he;
	he = gethostbyname(nome);
	if (he == NULL) {
 		herror("gethostbyname");
 		exit(0);
 	}
 	return(he);
}

/* miei wrapper c */

pid_t
Fork(void) {
	pid_t	pid;
	pid = fork();
	if ( pid < 0) {
		err_sys("fork error");
	}
	return(pid);
}

void
Close(int fd) {
	int n, flag=0;
	while(flag!=1) {
		n = close(fd);
		if (n < 0) {
			if(errno==EINTR) {
				flag=0;
				err_sys("close error [EINTR: chiusura anomala]");
			} else {
				err_sys("close error");
			}
		} else {
			flag = 1;
		}
	}
}

void
Close2(int fd) {
	int n;
	n = close(fd);
	if(n < 0) {
		if(errno==EINTR)
			err_sys("close error");
	}
}
