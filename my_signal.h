#include <signal.h>
#include <sys/wait.h>

typedef void Sigfunc(int); 

Sigfunc*
signal(int signo, Sigfunc *handler);

void
gestisci_zombie (int segnale);


typedef void (*sighandler_t)(int);

sighandler_t
Signal(int signum, sighandler_t handler);
