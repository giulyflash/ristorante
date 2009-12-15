/*
 * esame.c
 *
 *  Created on: Apr 17, 2009
 *      Author: 10011342
 */

#include "basic.h"
#include "esame.h"

/*generazione di un nuovo pacchetto*/
pacchetto *newPacchetto(void) {
	return(pacchetto*)malloc(sizeof(pacchetto));
}

/*apertura del file menu e invio al cameriere
 * viene controllato che il menu sia presente
 */
void invia_menu(int connsd) {
	FILE *fd;
	char buffer[MAXLINE],str[MAXLINE];
	printf("ti invio il menu\n");
	fd=fopen("menu","r");
	if (fd==NULL){
		perror ("Non e' possibile aprire il file");
		exit(1);
	}
	while(!feof(fd)){
		fgets(str,MAXLINE,fd);
		Write(connsd,str,strlen(str));
		memset(str,'\0',strlen(str));
	}
 	strcpy(buffer,"***fine del file***\n");
 		Write(connsd,buffer,strlen(buffer));
	fclose(fd);
}

void leggi_menu(int sockfd) {
	char echo[MAXLINE];
	int flag=0;
	while(flag!=1) {
		Read(sockfd,echo,MAXLINE);
		if(strstr(echo,"***fine del file***") != NULL)
			flag=1;
		if(flag!=1) {
			printf("%s",echo);
			memset(echo,'\0',MAXLINE);
		}
	}
}

/*gestione di stampa dell'ordine*/
void stampa_ordine(pacchetto p) {
	int i=0;
	printf("cameriere %d tavolo %d\n", p.nome_cameriere,p.tavolo);
	while(p.ordine[i]!= '\0') {
		printf("hai ordinato %d porizioni di %d\n",p.ordine[i+1],p.ordine[i]);
		i+=2;
	}
	printf("\n");
}

/*gestione dell'invio di un pacchetto*/
void my_send(int protocollo, pacchetto p) {
	char send[sizeof(pacchetto)];
	p.protocollo = protocollo;
	memcpy(send,&p,sizeof(pacchetto));
	Write(sockfd,send,sizeof(send));
}

/**/
void coda_tavoli(pacchetto p) {
	int i=0;
	printf("\t---\tin attesa dei piatti\t---\n");
    while(p.ordine[i]!= '\0') {
            printf("\n\til tavolo %d attende i piatti. . .\n",p.ordine[i]);
            i++;
    }
    printf("\n\t---\tfine lista\t\t---\n");
    menu_cameriere();
}


/*inizializzazione della dispensa*/
void init_dispensa() {
	int shmid4;
    if((shmid4 = (shmget(IPC_PRIVATE, sizeof(dispensa), 0600))) < 0) {
            err_sys("errore nell shmget");
    }
    d = (dispensa *) shmat(shmid4, 0, 0);
    d->piatto_1[0] = 20;
    d->piatto_1[1] = 7;

    d->piatto_2[0] = 20;
    d->piatto_2[1] = 20;

    d->piatto_3[0] = 20;
    d->piatto_3[1] = 18;

    d->piatto_4[0] = 20;
    d->piatto_4[1] = 15;

    d->piatto_5[0] = 20;
    d->piatto_5[1] = 35;

    d->piatto_6[0] = 20;
    d->piatto_6[1] = 25;

    d->piatto_7[0] = 20;
    d->piatto_7[1] = 50;

    d->piatto_8[0] = 20;
    d->piatto_8[1] = 5;

    d->piatto_9[0] = 20;
    d->piatto_9[1] = 8;
}

/*gestione del menu cameriere*/
void menu_cameriere() {
		printf("\n\t\t*** SeS Restaurant ***\n");
        printf("\t\n *** Applicazione di rete per la Ristorazione ***\n\n");
        printf("\t1. nuovo ordine\n");
        printf("\t2. modifica ordine\n");
        printf("\t3. invia sollecito\n");
        printf("\t4. visualizza piatti pronti\n");
        printf("\t5. servi piatto\n");
        printf("\t6. richiedi conto\n");
        printf("\t7. esci\n");
        printf("\ncosa desideri fare:\n");
}

