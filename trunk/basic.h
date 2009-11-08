/* Header file di base per tutti gli esempi del corso */
/* Include header file di sistema attinenti alla programmazione con i socket
   e dichiarazioni e definizioni comuni a tutti gli esempi */

#ifndef	__basic_h
#define	__basic_h

/* include dei principali  header file di sistema */
#include <sys/shm.h>
#include	<sys/types.h>	/* tipi di dati di sistema */
#include	<sys/socket.h>	/* dichiarazioni funzioni base su socket */
#include	<sys/time.h>	/* timeval{} per la select() */
#include	<time.h>	/* timespec{} per la pselect() */
#include	<netinet/in.h>	/* definizioni degli indirizzi dei socket */
#include	<arpa/inet.h>	/* funzioni inet(3) */
#include	<errno.h>       /* definizione di errno */
#include	<fcntl.h>	/* utilizzato per I/O nonbloccante */
#include	<netdb.h>	/* definizioni di macro e costanti attienti il network */
#include	<signal.h>      /* definizioni di handler di segnali */
#include	<stdio.h>	/* funzioni di I/O standard */
#include	<stdlib.h>	/* funzioni della libreria standard */
#include	<string.h>	/* funzioni ANSI C per manipolazione di stringhe */
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>	/* usato per iovec{} e readv/writev */
#include	<unistd.h>	/* usato da sysconf */
#include	<sys/wait.h>	/* usato da wait() */
#include	<sys/un.h>	/* utilizzato per socket Unix domain */
#include        <sys/select.h>  /* definizione di select() */
#include        <poll.h>        /* definizione di poll() */
#include        <strings.h>     /* funzioni non ANSI C su stringhe */
#include        <sys/ioctl.h>   /* usato per ioctl */
#include <sys/shm.h>


/* ------------------------------------------------------------------------- */

/* dichiarazioni delle funzioni di gestione degli errori */

void err_sys(const char *, ...);  /* errore fatale in una system call */
void err_quit(const char *, ...); /* errore fatale non in una system call */
void err_dump(const char *, ...); /* errore fatale in una system call con dump */
void err_ret(const char *, ...);  /* errore non fatale in una system call */
void err_msg(const char *, ...);  /* errore non fatale non in una system call */

/* ------------------------------------------------------------------------- */

/* dichiarazioni delle funzioni di lettura e scrittura su uno stream socket */

ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readn(int fd, void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxline);
ssize_t my_read(int fd, char *ptr);

/* ------------------------------------------------------------------------- */

/* definizioni di costanti e macro di uso comune */

#define MIN(a,b)        ((a) < (b) ? (a) : (b))
#define MAX(a,b)        ((a) > (b) ? (a) : (b))

#define MAXLINE 	1024
#define MAXSOCKADDR 	128
#define BUFFSIZE 	8192

#define LISTENQ 	5            /* Dimensione del backlog della listen() */
#define SERV_PORT 	9877         /* TCP and UDP client-servers */
#define SERV_PORT_STR 	"9877"       /* TCP and UDP client-servers */
#define UNIXSTR_PATH 	"/tmp/unix.str" /* Unix domain stream cli-serv */
#define UNIXDG_PATH  	"/tmp/unix.dg"  /* Unix domain datagram cli-serv */

#ifndef SHUT_RD
#define SHUT_RD         0       /* shutdown in lettura */
#define SHUT_WR         1       /* shutdown in scrittura */
#define SHUT_RDWR       2       /* shutdown in lettura e scrittura */
#endif

typedef struct sockaddr SA;



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

//typedef void (*sighandler_t)(int);
/*
sighandler_t
Signal(int signum, sighandler_t handler);
*/

#endif
