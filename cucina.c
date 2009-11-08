#include "basic.h"
#include "esame.h"


int numero_ordine=0, camerieri, max, *sollecito, *incasso, client[FD_SETSIZE];
double  *tempo_medio, *tempo_di_servizio, *tempo_totale, *medio;
pacchetto *temporaneo;
dispensa *d;

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
                printf("-----------------------------------------\n");
                printf("il cameriere %d comunica che\n\n", p->nome_cameriere);
                i = 0;
                while(p->ordine[i]!= '\0') {
                        printf("il tavolo %d ha ordinato %d porzioni di %d\n",p->tavolo, p->ordine[i+1],p->ordine[i]);
                        i+=2;
                }
                printf("-----------------------------------------\n");
                p = p->next;
        }
        printf("\t*** fine lista ***\n");
}

pacchetto stampa_lista2(pacchetto *p) {
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

int stampa_lista3(pacchetto *p, int cameriere) {
        pacchetto tmp;
    char send[sizeof(pacchetto)];

        int i,j, ritorno=0;
        printf("\n\ti piatti sono pronti. . .\n");
        j=0;
        while(p != NULL) {
        	if(p->nome_cameriere == cameriere) {
				ritorno++;
        	}
                p = p->next;
        }
        tmp.protocollo = 12;
        memcpy(send,&tmp,sizeof(pacchetto));
        Write2(client[cameriere],send,sizeof(send));
        if(ritorno!=0)
        	return 1;
        else
        	return 0;
}
void passa_ordine(pacchetto *p, int old, int new) {
    while(p != NULL) {
    	if(p->nome_cameriere == old) {
    		printf("la cucina passa l'ordine dal cameriere %d al cameriere %d\n", p->nome_cameriere, new);
    		p->nome_cameriere=new;
    	}
    	p = p->next;
    }
}
void *tbody1(void *arg){
    pacchetto tmp;
    char send[sizeof(pacchetto)];
	int i=3, notifica=0;
	pthread_t t1;
	while(i!=0) {
		sleep(60);
		notifica += stampa_lista3(head,(int)arg);
		i--;
	}
	if(notifica<3) {
        tmp.protocollo = 13;
        memcpy(send,&tmp,sizeof(pacchetto));
        Write2(client[(int)arg],send,sizeof(send));
		printf("OTTIMO LAVORO!!\n");

	}
	else {
        tmp.protocollo = 14;
        memcpy(send,&tmp,sizeof(pacchetto));
        Write2(client[(int)arg],send,sizeof(send));
		printf("cameriere non ci siamo\n");
		max++;
		if(max<2) {
		passa_ordine(head,(int)arg,(int)arg+1);
		pthread_create(&t1, NULL, tbody1, (arg+1));
		} else {
			passa_ordine(head,(int)arg,1);
			pthread_create(&t1, NULL, tbody1, 1);
			max = 0;
		}

	}
        pthread_exit(NULL);
}



void prepara_piatti(pacchetto p) {
        char numPiatto[MAXLINE];

        int qt, piatto, error;
        double tempo;
        int i=0, tot=0, j;
        pid_t pid;
        char send[sizeof(pacchetto)];
        struct timeval          starttime,endtime;

        if((pid=fork()) == 0) {
              sleep(15);
                if ( gettimeofday(&starttime, NULL) < 0 )
                        err_sys("gettime error");
                printf("\n *** Il cuoco [un processo figlio con pid %d] comincia a preparare il piatto ***\n", getpid());
                j=0;
                while((p.ordine[i]!= '\0') && (p.servito != 1)){
                        error = 0;
                        j++;
                        switch(p.ordine[i]) {
                        case 1:
                                piatto = 1;
                                tot += d->piatto_1[1];
                                printf("il piatto costa %d euro\n", d->piatto_1[1]);
                                if((d->piatto_1[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_1[0] += p.ordine[i+1];
                                        incasso -= d->piatto_1[0];
                                        error = 1;
                                }
                                break;
                        case 2:
                                piatto = 2;
                                if((d->piatto_2[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_2[0] += p.ordine[i+1];
                                        incasso -= d->piatto_2[0];
                                        error = 1;
                                }
                                break;
                        case 3:
                                piatto = 3;
                                if((d->piatto_3[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_3[0] += p.ordine[i+1];
                                        incasso -= d->piatto_3[0];
                                        error = 1;
                                }
                                break;
                        case 4:
                                piatto = 4;
                                if((d->piatto_4[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_4[0] += p.ordine[i+1];
                                        incasso -= d->piatto_4[0];
                                        error = 1;
                                }
                                break;
                        case 5:
                                piatto = 5;
                                if((d->piatto_5[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_5[0] += p.ordine[i+1];
                                        incasso -= d->piatto_5[0];
                                        error = 1;
                                }
                                break;
                        case 6:
                                piatto = 6;
                                if((d->piatto_6[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_6[0] += p.ordine[i+1];
                                        incasso -= d->piatto_6[0];
                                        error = 1;
                                }
                                break;
                        case 7:
                                piatto = 7;
                                if((d->piatto_7[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_7[0] += p.ordine[i+1];
                                        incasso -= d->piatto_7[0];
                                        error = 1;
                                }
                                break;
                        case 8:
                                piatto = 8;
                                if((d->piatto_8[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_8[0] += p.ordine[i+1];
                                        incasso -= d->piatto_8[0];
                                        error = 1;
                                }
                                break;
                        case 9:
                                piatto = 9;
                                if((d->piatto_9[0] -= p.ordine[i+1]) > 0) {
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", p.ordine[i], p.tavolo);
                                printf("In dispensa sono presenti %d porzioni del piatto %d\n", d->piatto_1[0], p.ordine[i]);
                                } else {
                                        printf("ingredienti esauriti\n");
                                        d->piatto_9[0] += p.ordine[i+1];
                                        incasso -= d->piatto_9[0];
                                        error = 1;
                                }
                                break;
                        }
                        if(error != 1) {
                        	printf("sollecito %d\n", *sollecito);
        					qt = p.ordine[i+1];
        					tempo = ((qt*piatto)/(*sollecito));
        					printf("%f\n", tempo);
        					sleep(tempo);
							tot = tot * qt;
							*incasso += tot;

							if ( gettimeofday(&endtime,NULL) < 0 )
									err_sys("gettime error");
							*tempo_di_servizio = (endtime.tv_sec+(endtime.tv_usec/1000000.0)) - (starttime.tv_sec+(starttime.tv_usec/1000000.0));

							snprintf(numPiatto, sizeof(numPiatto),"%d", p.ordine[i]);
							p.protocollo = 7;
							strcpy(p.messaggio,numPiatto);
							memcpy(send,&p,sizeof(pacchetto));
							Write(connsd,send,sizeof(send));
							printf("\n *** sto preparando il piatto %d per il tavolo %d ***\n",p.ordine[i], p.tavolo);
							i+=2;
                        } else {
                            p.protocollo = 9;
                      //      d->piatto_1[0] += p.ordine[i+1];
                      //      incasso -= d->piatto_1[0];
                            strcpy(p.messaggio,numPiatto);
                            memcpy(send,&p,sizeof(pacchetto));
                            Write(connsd,send,sizeof(send));
                            i+=2;
                        }

                }
                *tempo_totale += *tempo_di_servizio;
				printf("\n *** Il tempo di preparazione di questo ordine e' di %fl secondi *** \n",*tempo_di_servizio);
				printf(" *** Il tempo totale di preparazione dei piatti e' di %f secondi ***\n",*tempo_totale);
				printf(" *** Il tempo medio di preparazione dei piatti e' di %f secondi ***\n",(*tempo_totale / numero_ordine));
				printf(" *** il cuoco [%d] ha terminato ***\n", getpid());
				printf(" *** il conto e' di %d euro *** \n", tot);
                exit(1);
        }
        pthread_t t1;

        printf("\nordine in lavorazione. . .\n");
        pthread_create(&t1, NULL, tbody1, p.nome_cameriere);



}

void leggi_ordine2(pacchetto p, int i) {
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
                        head = cancella_pacchetto(p);
                }
                tmp = tmp->next;
        }

        if(trovato == 1) {
                tmp->protocollo = 5;
            memcpy(send,&tmp,sizeof(pacchetto));
            Write(connsd,send,sizeof(send));
        } else {
        numero_ordine++;
        stampa_lista(head);
        prepara_piatti(p);
        }
}

void leggi_ordine(pacchetto p, int i) {
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
    if((trovato == 1) && (p.pr ==1)) {
    	head = cancella_pacchetto(p);
        printf(" *** da inserire ***\n");
        numero_ordine++;
        inserisci_pacchetto(tmp);
        stampa_lista(head);
        prepara_piatti(p);
    }
    if((trovato == 1)){
            p.protocollo = 8;
            printf("gia inserito\n");
            memcpy(send,&p,sizeof(pacchetto));
            Write(connsd,send,sizeof(send));
    } else {
        printf("da inserire\n");
        numero_ordine++;
        inserisci_pacchetto(tmp);
        stampa_lista(head);
        prepara_piatti(p);
    }


}

pacchetto *cancella_pacchetto(pacchetto p) {
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

void modifica_ordine(pacchetto p) {
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
        Write(connsd,send,sizeof(send));
        }
        head = cancella_pacchetto(p);

}

void evadi_ordine(pacchetto p) {

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
        printf(" *** Tavolo %d servito ***\n", p.tavolo);
        head = cancella_pacchetto(p);

        printf(" *** Fino a ora il ristorante ha incassato %d euro ***\n", *incasso);
        Write(connsd,send,sizeof(send));
        }

}

/*

void *tbody2(pacchetto p){

int trovato=0;
char send[sizeof(pacchetto)];
	pacchetto *tmp;
    if (head == NULL) {
    	p.protocollo = 10;
            memset(send,'\0',sizeof(pacchetto));
            memcpy(send,&p,sizeof(pacchetto));
            Write(connsd,send,sizeof(send));
    }
    else    {
    while((head != NULL) && (trovato == 0)) {
            if(head->tavolo == p.tavolo) {
                    trovato = 1;
                    head->sollecito++;
            }
            head = head->next;
    }
    }
	pthread_exit(NULL);
}


void sollecita_ordine(pacchetto p) {
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
            p.protocollo = 11;
    } else {
            p.protocollo = 3;
    }

    memset(send,'\0',sizeof(pacchetto));
    memcpy(send,&p,sizeof(pacchetto));
    Write(connsd,send,sizeof(send));
    }
    head = cancella_pacchetto(p);

}

void lista_da_servire() {
        stampa_lista(head);
}
*/
void gestisci_protocollo_server(pacchetto p, int cameriere) {
	pthread_t t1;
        switch(p.protocollo) {
        case 1:
                invia_menu(connsd);
                break;
        case 2:
                leggi_ordine(p,cameriere);
                break;
        case 3:
                modifica_ordine(p);
                break;
        case 4:
                evadi_ordine(p);
                break;
        case 5:
                stampa_lista2(head);
                break;
        case 6:
				printf("CUCINA SBRIGATI!!\n");
				*sollecito+=1;
                break;
        }
}

void handler(int signum) {
	int pid,status;
	fprintf(stdout,"signum=%d\n",signum);
	switch (signum) {
		case SIGCHLD:
			while((pid=waitpid(-1,&status,WNOHANG))>0) {
				fprintf(stdout,"Processo [%d] terminato.\n",pid);
				if (WIFSIGNALED(status)) {
					fprintf(stdout,"Errore. Processo [%d] e' terminato a causa del segnale %d.\n",pid,WTERMSIG(status));
					// se nel figlio avviene un errore come buffer overflow e viene generato il segnale SIGSEGV questo non viene segnalato dal padre
					// attraverso questo if(WIFSIGNALED(status)) si riesce a capire se il figlio e' terminato in maniera prematura x un errore.
				}
			}
		break;
		case SIGINT:
			fprintf(stdout,"*** figlio *** rilevato segnale SIGINT, la connessione attiva continua a essere gestita\n");
		break;

	}
}
int main(int argc, char **argv) {

        int                             listensd, /*connsd,*/clilen,port,ready,/*client[FD_SETSIZE],*/i,maxi,maxd,n, dimensione;
        char                            rcv[MAXLINE],data[MAXLINE];
        socklen_t                       servaddr_len,cliaddr_len;
        fd_set                          rset,allset;
        time_t                          ticks;
        int                             shmid, shmid2, shmid3, shmid4, shmid5, shmid6, shmid7, shmid8, shmid9;
        struct sockaddr_in              servaddr, cliaddr;
        struct timeval                  timeout;
        pacchetto p;
        head = NULL;
        tail = NULL;
        dimensione = sizeof(pacchetto);


        if(argc!=2)
                err_quit("Usage: %s <Port>\n",argv[0]);

        port = atoi( argv[1] );

        if( (port > 12000) || (port < 10000) )
                err_sys("Hai scelto una porta non consentita");

        memset(&servaddr,'\0',sizeof(servaddr));
        servaddr.sin_family                     = AF_INET;
        servaddr.sin_addr.s_addr        = htonl(INADDR_ANY);
        servaddr.sin_port                       = htons(port);


        /* inizializzo i campi della struttura socket */
        listensd = Socket(AF_INET, SOCK_STREAM,0);

        /* eseguo bind e listen */
        Bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        Listen(listensd,LISTENQ);

        /* inizializzo l'array dei client */

/*      client[0] = listensd; */
        maxi = -1;
        for(i=1;i<FD_SETSIZE;i++)
                client[i] = -1;

        /* inizializzo il set dei socket */
        maxd = listensd;
        FD_ZERO(&allset);
        FD_SET(listensd,&allset);
        clilen=sizeof(struct sockaddr_in);

        system("clear");
        ticks = time(NULL);
        snprintf(data,sizeof(data),"%.24s\r\n", ctime(&ticks));
        printf("apertura cucina %.24s\n", ctime(&ticks));


        if((shmid = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        if((shmid2 = (shmget(IPC_PRIVATE, sizeof(int)*20, 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        if((shmid3 = (shmget(IPC_PRIVATE, sizeof(pacchetto), 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        if((shmid4 = (shmget(IPC_PRIVATE, sizeof(dispensa), 0600))) < 0) {
                err_sys("errore nell shmget");
        }


        if((shmid5 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
                err_sys("errore nell shmget");
        }
        if((shmid6 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
                err_sys("errore nell shmget");
        }
        if((shmid7 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
                err_sys("errore nell shmget");
        }

        if((shmid8 = (shmget(IPC_PRIVATE, sizeof(int), 0600))) < 0) {
                err_sys("errore nell shmget");
        }
        if((shmid9 = (shmget(IPC_PRIVATE, sizeof(int), 0600))) < 0) {
                err_sys("errore nell shmget");
        }


        medio = (double *) shmat(shmid, 0, 0);

        tempo_medio = (double *) shmat(shmid5, 0, 0);
        tempo_di_servizio = (double *) shmat(shmid6, 0, 0);
        tempo_totale = (double *) shmat(shmid7, 0, 0);
        incasso = (int *) shmat(shmid8, 0, 0);
        sollecito = (int *) shmat(shmid9, 0, 0);
        *sollecito+=1;

//      head = (pacchetto *) shmat(shmid2, 0, 0);

//      tail = (pacchetto *) shmat(shmid2, 0, 0);

//        *l_piatti = (int *) shmat(shmid2, 0, 0);

        temporaneo = (pacchetto *) shmat(shmid3, 0, 0);


        d = (dispensa *) shmat(shmid4, 0, 0);

        d->piatto_1[0] = 100;
        d->piatto_1[1] = 7;

        d->piatto_2[0] = 100;
        d->piatto_2[1] = 20;

        d->piatto_3[0] = 100;
        d->piatto_3[1] = 18;

        d->piatto_4[0] = 100;
        d->piatto_4[1] = 15;

        d->piatto_5[0] = 100;
        d->piatto_5[1] = 35;

        d->piatto_6[0] = 100;
        d->piatto_6[1] = 25;

        d->piatto_7[0] = 100;
        d->piatto_7[1] = 50;

        d->piatto_8[0] = 100;
        d->piatto_8[1] = 5;

        d->piatto_9[0] = 100;
        d->piatto_9[1] = 8;

        for(;;) {
        //	signal(SIGINT, handler);
                rset = allset;
                timeout.tv_sec = 3;

                /* eseguo la select e quando il socket e' attivo faccio accept */
                if((ready = select(maxd+1,&rset,NULL,NULL,&timeout)) < 0)
                        err_sys("errore nella select");


                /* se ci sono gestisco richieste di connessione */
                if(FD_ISSET(listensd,&rset)) {

                        cliaddr_len = sizeof(cliaddr);

                        connsd=Accept(listensd, (struct sockaddr *) &cliaddr, &clilen);
                        Getsockname(connsd,(struct sockaddr *)&servaddr,&servaddr_len);
                        Getpeername(connsd,(struct sockaddr *)&cliaddr,&cliaddr_len);

                        printf("\nserver --> *** indirizzo IP: %s\tPORTA: %d ***\n",inet_ntoa(servaddr.sin_addr),ntohs(servaddr.sin_port));
                        printf("connessione accettata\n");
                        printf("client --> *** indirizzo IP: %s\tPORTA: %d ***\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

                        /* memorizzo la connessione dei client */
                        for(i=1;i<FD_SETSIZE;i++) {
                                if(client[i] < 0) {
                                        client[i] = connsd;
                                        printf("\ncameriere [%d] connesso!\n\n", i);
                                        camerieri++;
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


                /* scorro la lista dei client ed eseguo le richieste */
                for(i=1;i<=maxi;i++) {
                        if((connsd = client[i]) < 0)
                                continue;
                        if(FD_ISSET(connsd, &rset)) {
                                memset(rcv,'\0',MAXLINE);
                                if((n = read(connsd, rcv, dimensione)) == 0) {
                                        Close(connsd);
                                        camerieri--;
                                        printf("la connessione con il cameriere [%d] e' caduta\n", i);
                                        client[i] = -1;
                                        FD_CLR(connsd, &allset);
                                } else {
                                                memcpy(&p,rcv,sizeof(pacchetto));
                                                gestisci_protocollo_server(p,i);
                                }
                        }
                        if(--ready <= 0)
                                continue;
                }
        }
        Close(listensd);
        exit(0);
}
