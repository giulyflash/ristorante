#include "basic.h"
#include "esame.h"
#include <sys/shm.h>

int numero_ordine=0;

double *medio;
int *l_piatti[20];
pacchetto *temporaneo;

void inserisci_pacchetto(pacchetto *p){
        if (head == NULL) {
                head = p;
                tail = p;
        }
        else {
                tail->next = p;
                tail = tail->next;
                tail->next = NULL;
        }

}

void stampa_lista(pacchetto *p) {
        int i;
        printf("\n\t*** lista ordini ***\n");
        while(p != NULL) {
                printf("--------------------------------\n");
                printf("cameriere %d tavolo %d\n\n", p->nome_cameriere,p->tavolo);
                i = 0;
                while(p->ordine[i]!= '\0') {
                        printf("hai ordinato %d porizioni di %d\n",p->ordine[i+1],p->ordine[i]);
                        i+=2;
                }
                printf("--------------------------------\n");
                p = p->next;
        }
        printf("\t*** fine lista ***\n");
}

pacchetto stampa_lista2(pacchetto *p, int connsd) {
	pacchetto tmp;
    char send[sizeof(pacchetto)];

        int i,j;
        printf("\n\t*** lista ordini ***\n");
        j=0;
        while(p != NULL) {
            printf("--------------------------------\n");
                printf("tavolo %d in attesa di essere servito\n", p->tavolo);
                i = 0;
                tmp.ordine[j] = p->tavolo;
                j++;
                printf("--------------------------------\n");
                p = p->next;
        }
        tmp.ordine[j] = '\0';
    	tmp.protocollo = 5;
        memcpy(send,&tmp,sizeof(pacchetto));
        Write(connsd,send,sizeof(send));
        return tmp;
}

void prepara_piatti(int connsd, pacchetto p) {
        char numPiatto[MAXLINE];

        int qt,tempo, piatto;
        int i=0;
        pid_t pid;
        double tempo_medio = 0;
        char send[sizeof(pacchetto)];
        struct timeval          starttime,endtime;
        double  tempo_di_servizio = 0, tempo_totale = 0;
        if((pid=fork()) == 0) {
//              sleep(20);
                if ( gettimeofday(&starttime, NULL) < 0 )
                        err_sys("gettime error");
                printf("\n *** Il cuoco [un processo figlio con pid %d] comincia a preparare il piatto ***\n", getpid());
                while((p.ordine[i]!= '\0') && (p.servito != 1)){

                        switch(p.ordine[i]) {
                        case 1:
                                piatto = 1;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 2:
                                piatto = 2;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 3:
                                piatto = 3;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 4:
                                piatto = 4;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 5:
                                piatto = 5;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 6:
                                piatto = 6;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 7:
                                piatto = 7;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 8:
                                piatto = 8;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        case 9:
                                piatto = 9;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                break;
                        }
                        memset(numPiatto,'\0',sizeof(numPiatto));
                        qt = temporaneo->ordine[i+1];
                        tempo = qt*piatto*10;
                        sleep(tempo);
                        if ( gettimeofday(&endtime,NULL) < 0 )
                                err_sys("gettime error");
                        tempo_totale += (tempo_di_servizio = (endtime.tv_sec+(endtime.tv_usec/1000000.0)) - (starttime.tv_sec+(starttime.tv_usec/1000000.0)));
                        printf("\nIl tempo di preparazione del piatto e' di %fl secondi\n",tempo_di_servizio);
                        snprintf(numPiatto, sizeof(numPiatto),"%d", p.ordine[i]);
                        p.protocollo = 7;
                        strcpy(p.messaggio,numPiatto);
                        memcpy(send,&p,sizeof(pacchetto));
                        Write(connsd,send,sizeof(send));
                        printf("\n *** e' pronto il piatto %d per il tavolo %d ***\n",p.ordine[i], p.tavolo);
                        i+=2;
                }
                tempo_medio += tempo_totale;
                *medio += tempo_totale;
                printf("\n *** Il tempo di evasione dell' ordine e' di %f secondi ***\n",tempo_totale);
                printf("\n *** Il tempo medio di evasione degli ordini e' di %f secondi ***\n",(*medio / numero_ordine));
                printf("\n *** processo con pid [%d] terminato ***\n", getpid());
                exit(1);
        }
}

void leggi_ordine2(int connsd,pacchetto p, int i) {
        char send[sizeof(pacchetto)];
        pacchetto *tmp = newPacchetto();
        memset(send,'\0',sizeof(pacchetto));
        memset(tmp,'\0',sizeof(tmp));
        p.nome_cameriere = i;
        memcpy(tmp,&p,sizeof(p));

        int trovato = 0;
        tmp = head;
        while((tmp != NULL) && (trovato == 0)) {
                if(tmp->tavolo == p.tavolo) {
                        memcpy(&p,tmp,sizeof(pacchetto));
                        trovato = 1;
                }
                tmp = tmp->next;
        }

        if(trovato == 1) {
        	tmp->protocollo = 5;
            memcpy(send,&tmp,sizeof(pacchetto));
            Write(connsd,send,sizeof(send));
        } else {
        numero_ordine++;
        inserisci_pacchetto(tmp);
        stampa_lista(head);
        prepara_piatti(connsd,p);
        }

 //       memcpy(send,&p,sizeof(pacchetto));
 //       Write(connsd,send,sizeof(send)); non sono sicuro che serva a qualcosa.
}

void leggi_ordine(int connsd,pacchetto p, int i) {
	int trovato = 0;
	char send[sizeof(pacchetto)];
	pacchetto *tmp = newPacchetto();
	pacchetto *tmp2;
	memset(send,'\0',sizeof(pacchetto));
	memset(tmp,'\0',sizeof(tmp));
	p.nome_cameriere = i;
	memcpy(tmp,&p,sizeof(p));

    tmp2 = head;
    while((tmp2 != NULL) && (trovato == 0)) {
            if(tmp2->tavolo == p.tavolo) {
                    memcpy(&p,tmp2,sizeof(pacchetto));
                    trovato = 1;
            }
            tmp2 = tmp2->next;
    }
    if(trovato == 1) {
            p.protocollo = 8;
            printf("gia inserito\n");
            memcpy(send,&p,sizeof(pacchetto));
            Write(connsd,send,sizeof(send));
    } else {
    	printf("da inserire\n");
    	numero_ordine++;
    	inserisci_pacchetto(tmp);
    	stampa_lista(head);
    	prepara_piatti(connsd,p);
    }


}

pacchetto *cancella_pacchetto(pacchetto p, int connsd) {
        pacchetto *tmp,*tmp2;
        char send[sizeof(pacchetto)];
        tmp2 = head;
        if ((p.nome_cameriere == tmp2->nome_cameriere) && (p.tavolo == tmp2->tavolo)) {
                if(tmp2->next != NULL) {
                        return(tmp2->next);
                } else {
                        head = NULL;
                        tail = NULL;
                        return(head);
                }
        }
        while(tmp2->next != NULL) {
                if((p.nome_cameriere == tmp2->next->nome_cameriere) && (p.tavolo == tmp2->next->tavolo)) {
                        if(tmp2->next->next == NULL) {
                                free(tmp2->next);
                                tmp2->next = NULL;
                                tail = tmp2;
                                return(head);
                        }
                        tmp = tmp2->next->next;
                        free(tmp2->next);
                        tmp2->next = tmp;
                        return(head);
                }
                tmp2 = tmp2->next;
        }

        p.protocollo = 10;
        memcpy(send,&p,sizeof(pacchetto));
        Write(connsd,send,sizeof(send));

        return(head);
}

void modifica_ordine(int connsd,pacchetto p) {
        char send[sizeof(pacchetto)];
        pacchetto *tmp;
        int trovato = 0;
        if (head == NULL) {
                p.protocollo = 10;
                memset(send,'\0',sizeof(pacchetto));
                memcpy(send,&p,sizeof(pacchetto));
                Write(connsd,send,sizeof(send));
        }
        else    {
        tmp = head;
        while((tmp != NULL) && (trovato == 0)) {
                if(tmp->tavolo == p.tavolo) {
                        memcpy(&p,tmp,sizeof(pacchetto));
                        trovato = 1;
                }
                tmp = tmp->next;
        }
        if(trovato == 1) {
                p.protocollo = 6;
        } else {
                p.protocollo = 3;
        }

        memset(send,'\0',sizeof(pacchetto));
        memcpy(send,&p,sizeof(pacchetto));
        head = cancella_pacchetto(p,connsd);
        Write(connsd,send,sizeof(send));
        }
}

void evadi_ordine(pacchetto p, int connsd) {

        char send[sizeof(pacchetto)];
        pacchetto *tmp;
        int trovato = 0;
        if (head == NULL) {
//                printf ("errore: lista vuota\n");
                p.protocollo = 10;
                memset(send,'\0',sizeof(pacchetto));
                memcpy(send,&p,sizeof(pacchetto));
                Write(connsd,send,sizeof(send));
        }
        else    {
        tmp = head;
        while((tmp != NULL) && (trovato == 0)) {
                if(tmp->tavolo == p.tavolo) {
                        memcpy(&p,tmp,sizeof(pacchetto));
                        trovato = 1;
                        tmp->servito = 1;
                }
                tmp = tmp->next;
        }
        if(trovato == 1) {
                p.protocollo = 4;
        } else {
                p.protocollo = 3;
        }
        memset(send,'\0',sizeof(pacchetto));
        memcpy(send,&p,sizeof(pacchetto));
        head = cancella_pacchetto(p,connsd);
        Write(connsd,send,sizeof(send));
        }

}

void sollecita_ordine(int connsd, pacchetto p) {
	int trovato;
	char send[sizeof(pacchetto)];
	pacchetto *tmp;
	tmp = head;
	if (head == NULL) {
//		printf ("errore: lista vuota\n");
		p.protocollo = 10;
		memset(send,'\0',sizeof(pacchetto));
		memcpy(send,&p,sizeof(pacchetto));
		Write(connsd,send,sizeof(send));
	} else {
	pacchetto *tmp;
	tmp = head;
	while((tmp != NULL) && (trovato == 0)) {
		if(tmp->tavolo == p.tavolo) {
			memcpy(&p,tmp,sizeof(pacchetto));
			trovato = 1;
		}
		tmp = tmp->next;
	}
	if(trovato == 1) {
		p.pr++;
		p.protocollo = 11;
	} else {
		p.protocollo = 3;
	}
	}
	memset(send,'\0',sizeof(pacchetto));
	memcpy(send,&p,sizeof(pacchetto));
	Write(connsd,send,sizeof(send));
}

void lista_da_servire(int connsd) {
	stampa_lista(head);
}

void gestisci_protocollo_server(pacchetto p, int connsd,int cameriere) {
	pacchetto tmp2;
        switch(p.protocollo) {
        case 1:
                invia_menu(connsd);
                break;
        case 2:
                leggi_ordine(connsd,p,cameriere);
                break;
        case 3:
                modifica_ordine(connsd,p);
                break;
        case 4:
                evadi_ordine(p, connsd);
                break;
        case 5:
				stampa_lista2(head,connsd);
                break;
        case 6:
				printf("CUCINA SBRIGATI ALTRIMENTI TI SPACCO!!\n");
                break;
        }
}



int main(int argc, char **argv) {

        int                             listensd, connsd,clilen,port,ready,client[FD_SETSIZE],i,maxi,maxd,n;
        char                            rcv[MAXLINE],send[MAXLINE], data[MAXLINE];
        socklen_t                       servaddr_len,cliaddr_len;
        fd_set                          rset,allset;
        time_t                          ticks;
        int shmid, shmid2, shmid3;
        struct sockaddr_in      servaddr, cliaddr;
        struct timeval          timeout;
        int dimensione = sizeof(pacchetto);
        pacchetto p;
        head = NULL;
        tail = NULL;

        if(argc!=2)
                err_quit("Usage: %s <Port>\n",argv[0]);

        port = atoi( argv[1] );

        if( (port > 12000) || (port < 10000) )
                err_sys("Hai scelto una porta non consentita");

        memset(&servaddr,'\0',sizeof(servaddr));
        servaddr.sin_family                     = AF_INET;
        servaddr.sin_addr.s_addr        = htonl(INADDR_ANY);
        servaddr.sin_port                       = htons(port);


        listensd = Socket(AF_INET, SOCK_STREAM,0);
        Bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        Listen(listensd,LISTENQ);

        maxd = listensd;
/*      client[0] = listensd; */
        maxi = -1;
        for(i=1;i<FD_SETSIZE;i++)
                client[i] = -1;

        FD_ZERO(&allset);
        FD_SET(listensd,&allset);
        clilen=sizeof(struct sockaddr_in);

        system("clear");
        ticks = time(NULL);
        snprintf(data,sizeof(data),"%.24s\r\n", ctime(&ticks));
        printf("cucina aperta il %.24s\n", ctime(&ticks));


        if((shmid = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        if((shmid2 = (shmget(IPC_PRIVATE, sizeof(int)*20, 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        if((shmid3 = (shmget(IPC_PRIVATE, sizeof(pacchetto), 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        medio = (double *) shmat(shmid, 0, 0);

//      head = (pacchetto *) shmat(shmid2, 0, 0);

//      tail = (pacchetto *) shmat(shmid2, 0, 0);

        *l_piatti = (int *) shmat(shmid2, 0, 0);

        temporaneo = (pacchetto *) shmat(shmid3, 0, 0);

        for(;;) {

                rset = allset;
                timeout.tv_sec = 3;
                if((ready = select(maxd+1,&rset,NULL,NULL,&timeout)) < 0)
                        err_sys("errore nella select");


                if(FD_ISSET(listensd,&rset)) {

                        cliaddr_len = sizeof(cliaddr);

                        connsd=Accept(listensd, (struct sockaddr *) &cliaddr, &clilen);
                        Getsockname(connsd,(struct sockaddr *)&servaddr,&servaddr_len);
                        Getpeername(connsd,(struct sockaddr *)&cliaddr,&cliaddr_len);

                        printf("\nserver --> *** indirizzo IP: %s\tPORTA: %d ***\n",inet_ntoa(servaddr.sin_addr),ntohs(servaddr.sin_port));
                        printf("connessione accettata\n");
                        printf("client --> *** indirizzo IP: %s\tPORTA: %d ***\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

                        for(i=1;i<FD_SETSIZE;i++) {
                                if(client[i] < 0) {
                                        client[i] = connsd;
                                        printf("\ncameriere [%d] connesso!\n\n", i);
                                        break;
                                }
                        }

                        if(i==FD_SETSIZE)
                                err_quit("troppi client");

                        FD_SET(connsd,&allset);

                        if(connsd > maxd)
                                maxd = connsd;
                        if(i > maxi)
                                maxi = i;
                        if(--ready <= 0)
                                continue;
                }

                for(i=1;i<=maxi;i++) {
                        if((connsd = client[i]) < 0)
                                continue;
                        if(FD_ISSET(connsd, &rset)) {
                                memset(rcv,'\0',MAXLINE);
                                if((n = read(connsd, rcv, dimensione)) == 0) {
                                        Close(connsd);
                                        printf("la connessione con il cameriere [%d] e' caduta\n", i);
                                        client[i] = -1;
                                        FD_CLR(connsd, &allset);
                                } else {
                                                memcpy(&p,rcv,sizeof(pacchetto));
                                                gestisci_protocollo_server(p,connsd,i);
                                }
                        }
                        if(--ready <= 0)
                                continue;
                }
        }
        Close(listensd);
        exit(0);
}

