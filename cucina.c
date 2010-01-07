/*
 * cucina.c
 *
 *  Created on: Apr 17, 2009
 *      Author: Sergio Urbano, Alberto Lavezzari
 */

#include "basic.h"
#include "esame.h"

int numero_ordine=0, camerieri, max, *sollecito, *incasso, *client, k=0,listensd;
double  *tempo_medio, *tempo_di_servizio, *tempo_totale, tempo_tot,*info;

pacchetto *lista_camerieri[4];
int shmid_t;
int *piatti_pronti;

void handler(int signum) {
	int pid,status;
	printf("signum=%d\n",signum);
	switch (signum) {
		case SIGCHLD:
			while((pid=waitpid(-1,&status,WNOHANG))>0) {
				printf("Processo [%d] terminato.\n",pid);
				if (WIFSIGNALED(status)) {
					printf("Errore. Processo [%d] e' terminato a causa del segnale %d.\n",pid,WTERMSIG(status));
				}
			}
		break;
		case SIGINT:
			printf("rilevato segnale SIGINT\n");
		break;

	}
}
/* gestione inserimento di un nuovo pacchetto in lista*/
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

/*gestione cancellazione di un pacchetto dalla lista*/
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

/*gestione di stampa della lista ordini*/
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

/*gestione di stampa dei piatti pronti in attesa di essere serviti*/
void lista_piatti_attesa(pacchetto p) {
	int i=0;
	while(i!=10) {
		if(piatti_pronti[i]!=0) {
			printf("tavolo %d piatto %d pronto\n", p.tavolo, piatti_pronti[i]);
		}
		i++;
	}
}

/*gestione dei piatti pronti per essere serviti
 * viene effettuato un controllo se il cameriere serve il piatto
 * se dopo 3 solleciti il cameriere non ha ancora servito, il piatto viene passato ad un altro cameriere
 * quando il piatto viene servito viene inviata una comunicazione di operazione a buon fine
 * e viene incrementato l'incasso di giornata
*/
void servi_piatto(pacchetto p, int cameriere, int i, int tot) {
	int j=3, cont=0, q;
	pacchetto tmp, *tmp2;
	char send[sizeof(pacchetto)];
	tmp2 = &p;
	p.pronti=i;
    while(j!=0) {
    	if(piatti_pronti[cameriere]!=0) {
                    tmp.protocollo = 12;
                    tmp.pronti=i;
                    tmp.tavolo=p.tavolo;
                    memcpy(send, &tmp, sizeof(pacchetto));
                    printf("fd dentro servi_piatto %d\n", client[cameriere]);
                    Write(client[cameriere], send, sizeof(send));
                    printf("cameriere %d devi servire il piatto %d al tavolo %d\n", cameriere, i, p.tavolo);
                    printf("STRUTTURA CLIENT DENTRO FORK\n");
                    for (q=1; q<10; q++){
                         	printf ("client[%d]=%d\n", q, client[q]);
                    }
                    cont++;
                    sleep(25);
            }
            j--;
    }
   	if(piatti_pronti[cameriere]!=0) {
            printf("piatto servito\n");
    }

    if(cont>2) {
            printf("Il cameriere non ha servito i piatti nel tempo prestabilito [inviate 3 notifiche], passo l'ordine a un altro cameriere\n");
            tmp.protocollo = 14;
            memcpy(send, &tmp, sizeof(pacchetto));
            Write(client[cameriere], send, sizeof(send));
            if(cameriere<3) {
					cameriere+=1;
                    piatti_pronti[p.nome_cameriere]='\0';
                    lista_camerieri[cameriere] = lista_camerieri[p.nome_cameriere];
                    piatti_pronti[cameriere]=1;
                    servi_piatto(*tmp2, cameriere, i,tot);
            } else {
                    lista_camerieri[1] = lista_camerieri[cameriere];
                    cameriere=1;
                    piatti_pronti[1]=1;
                    servi_piatto(*tmp2,1,i,tot);
            }
    } else {
            printf("ottimo lavoro\n");
            *incasso += tot;
            lista_camerieri[cameriere]->conto+=tot;
            tmp.protocollo = 13;
            memcpy(send, &tmp, sizeof(pacchetto));
            printf("fd dentro servi_piatto %d\n", client[cameriere]);
            Write(client[cameriere], send, sizeof(send));
    }

}

/*gestione di preparazione dei piatti
 * viene effettuato un controllo sull'ordine
 * se il piatto risulta non modificato viene preparato normalmente
 * se il flag segnala che e' stato modificato la sua preparazione viene scartata, viene segnalata
 * la sua eliminazione al cameriere e si procede con gli altri elementi dell'ordine
 * nella preparazione viene effettuato un controllo sulle porzioni disponibili in dispensa
 * se l'ordine e' soddisfacibile le porzioni vengono decrementate e
 * si procede al calcolo del tempo di preparazione e del costo degli n piatti in preparazione
 * se l'ordine non e' soddisfacibile viene segnalato l'esaurimento del piatto i al cameriere
 */
void prepara_piatti(pacchetto p) {
	int time=0;
	int i = 0, tot = 0;
	pid_t pid;
	int shmid[10];
	char send[sizeof(pacchetto)];

    if(p.modificato==0) {
        if((shmid[p.tavolo] = (shmget(IPC_PRIVATE, sizeof(pacchetto), 0600))) < 0) {
                err_sys("errore nell shmget");
        }
        lista_camerieri[p.nome_cameriere] = (pacchetto *) shmat(shmid[p.tavolo], 0, 0);
        *lista_camerieri[p.nome_cameriere] = p;
    }
    *lista_camerieri[p.nome_cameriere] = p;
    if((p.modificato==0)||(p.modificato==2)) {

    	if ((pid = fork()) == 0) {
    		Close(listensd);
    		sleep(30);
/*    		Signal(SIGPIPE, SIG_IGN); */
			while(lista_camerieri[p.nome_cameriere]->ordine[i] != '\0') {
					switch (lista_camerieri[p.nome_cameriere]->ordine[i]) {
						case 1:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_1[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_1[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_1[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_1[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_1[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
								tot -= d->piatto_1[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
							}
						break;

						case 2:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_2[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_2[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_2[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_2[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_2[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_2[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;

						case 3:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_3[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_3[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_3[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_3[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_3[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_3[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;
						case 4:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_4[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_4[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_4[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_4[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_4[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_4[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;

						case 5:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_5[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_5[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_5[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_5[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_5[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_5[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;

						case 6:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_6[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_6[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_6[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_6[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_6[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_6[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;

						case 7:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_7[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_7[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_7[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_7[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_7[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_7[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;

						case 8:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_8[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_8[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_8[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_8[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_8[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_8[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;
						case 9:
							if(lista_camerieri[p.nome_cameriere]->ordine[i+1]!=0) {
								if ((d->piatto_9[0] -= lista_camerieri[p.nome_cameriere]->ordine[i+1]) > 0) {
									lista_camerieri[p.nome_cameriere]->error=0;
									printf("piatto %d in lavorazione. . .In dispensa sono presenti %d porzioni del piatto %d\n", lista_camerieri[p.nome_cameriere]->ordine[i],d->piatto_9[0], lista_camerieri[p.nome_cameriere]->ordine[i]);
									tot = d->piatto_9[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								} else {
									p.esauriti=lista_camerieri[p.nome_cameriere]->ordine[i];
									p.protocollo=20;
									p.ordine[i]=0;
									memcpy(send, &p, sizeof(pacchetto));
									Write(client[p.nome_cameriere], send, sizeof(send));
									printf("gli ingredienti per il piatto %d sono esauriti\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
									lista_camerieri[p.nome_cameriere]->ordine[i]=0;
									d->piatto_9[0] += lista_camerieri[p.nome_cameriere]->ordine[i+1];
									tot -= d->piatto_9[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
									lista_camerieri[p.nome_cameriere]->error = 2;
								}
							} else {
								lista_camerieri[p.nome_cameriere]->error = 1;
								tot -= d->piatto_9[1] * lista_camerieri[p.nome_cameriere]->ordine[i+1];
								printf("piatto %d eliminato\n", lista_camerieri[p.nome_cameriere]->ordine[i]);
							}
						break;

					}
					if(lista_camerieri[p.nome_cameriere]->error!=1) {
						if(lista_camerieri[p.nome_cameriere]->error==2)
							exit(1);
						time = ((lista_camerieri[p.nome_cameriere]->ordine[i]*lista_camerieri[p.nome_cameriere]->ordine[i+1]));
						printf("cucino il piatto in %d secondi. . .\n",(time)/lista_camerieri[p.nome_cameriere]->sollecito);
						time=time/lista_camerieri[p.nome_cameriere]->sollecito;
						sleep(time);
						printf("piatto pronto, comincio a notificarlo al cameriere\n");
						piatti_pronti[p.nome_cameriere] = 1;
                        servi_piatto(p,p.nome_cameriere,lista_camerieri[p.nome_cameriere]->ordine[i], tot);
					}
                    lista_camerieri[p.nome_cameriere]->error = 0;
					i+=2;

				}
    exit(1);
    }
    }

}

/*gestione di lettura dell'ordine ricevuto*/
void leggi_ordine(pacchetto p, int i) {
	int trovato = 0;
	char send[sizeof(pacchetto)];
	pacchetto *tmp = newPacchetto();
	pacchetto *tmp2;
	memset(send, '\0', sizeof(pacchetto));
	memset(tmp, '\0', sizeof(tmp));
	p.nome_cameriere = i;
	memcpy(tmp, &p, sizeof(p));

	tmp2 = head;

	while ((tmp2 != NULL) && (trovato == 0)) {
		if ((tmp2->tavolo == p.tavolo)&& (p.modificato!=1)) {
			memcpy(&p, tmp2, sizeof(pacchetto));
			trovato = 1;
		}
		tmp2 = tmp2->next;
	}

	if ((trovato == 1)) {
		p.protocollo = 8;
		printf("ordine scartato [gia memorizzato un ordine con quell'id tavolo]\n");
		memcpy(send, &p, sizeof(pacchetto));
		Write(connsd, send, sizeof(send));
	} else {
		printf("ordine inserito in lista [ordine nuovo, o modificato]\n");
		numero_ordine++;
		inserisci_pacchetto(tmp);
		stampa_lista(head);
		prepara_piatti(p);
	}
}


/*gestione della modifica dell'ordine lato server*/
void modifica_ordine_server(pacchetto p) {
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
                numero_ordine--;
        } else {
                p.protocollo = 3;
        }

        memset(send,'\0',sizeof(pacchetto));
        memcpy(send,&p,sizeof(pacchetto));
        Write(connsd,send,sizeof(send));
        }
        head = cancella_pacchetto(p);
}

/*gestione dell'evasione ordine*/
void evadi_ordine(pacchetto p) {
	piatti_pronti[p.nome_cameriere]='\0';
}

/*gestione stampa delle statistiche della cucina*/
void stampa_statistiche() {

	int j=0;
	printf("\n\n\t\t *** STATISTICHE ***\n\n");
	while(info[j]!=0) {
		printf(" *** il tavolo %d e' stato servito in %fl secondi ***\n", (int)info[j], info[j+1]);
		j+=2;
	}
	printf(" *** il tempo totale di evasione degli ordini e' %fl ***\n",tempo_tot);
	printf(" *** il tempo medio di evasione degli ordini e' %fl ***\n",(tempo_tot / numero_ordine));
	printf(" *** Il ristorante ha incassato %d euro ***\n", *incasso);
}

void memorizza_info(pacchetto p) {
	tempo_tot+=p.tempi[0];
	info[k] = p.tavolo;
	info[k+1] = p.tempi[0];
	k+=2;

}

/*gestionde del sollecito*/
void gestisci_sollecito(pacchetto p) {
	pacchetto *tmp;
	int trovato=0;
	tmp = head;

	while ((tmp != NULL) && (trovato == 0)) {
		if (tmp->tavolo == p.tavolo) {
			memcpy(&p, tmp, sizeof(pacchetto));
			trovato = 1;
			printf("CUCINA SBRIGATI!!\n");
			*lista_camerieri[p.nome_cameriere] = *tmp;
			lista_camerieri[p.nome_cameriere]->sollecito+=1;
		}
		tmp = tmp->next;
	}
	if(trovato!=1) {
		printf("tavolo non presente in lista\n");
	}
}

/*gestione di invio del conto al tavolo richiedente*/
void invia_conto(pacchetto p) {
	char send[sizeof(pacchetto)];
	pacchetto tmp, *tmp2;
	int trovato=0;
	tmp2=head;
	while ((tmp2 != NULL) && (trovato == 0)) {
		if (tmp2->tavolo == p.tavolo) {
			memcpy(&p, tmp2, sizeof(pacchetto));
			trovato = 1;
		}
		tmp2 = tmp2->next;
	}
	if(trovato==1) {
		printf("il conto e' di %d euro\n", lista_camerieri[p.nome_cameriere]->conto);
		head = cancella_pacchetto(p);
		tmp.protocollo=15;
		tmp.conto = lista_camerieri[p.nome_cameriere]->conto;
		memcpy(send, &tmp, sizeof(pacchetto));
		Write(client[p.nome_cameriere], send, sizeof(send));
		lista_camerieri[p.nome_cameriere]->conto=0;
	} else {
		printf("impossibile eseguire l'operazione");
	}
}

/*gestione del protocollo di comunicazione lato server*/
void gestisci_protocollo_server(pacchetto p, int cameriere) {
	char send[sizeof(pacchetto)];
        switch(p.protocollo) {
        /*invia il menu al cameriere*/
        case 1:
                invia_menu(connsd);
                break;
        /*legge l'ordine consegnato dal cameriere*/
        case 2:
                leggi_ordine(p,cameriere);
                break;
        /*richiama la modifica di un ordine*/
        case 3:
				if(head!=NULL)
					modifica_ordine_server(p);
				else
					p.protocollo=10;
					memcpy(send,&p,sizeof(pacchetto));
					Write(connsd,send,sizeof(send));
                break;
        /*richiama la gestione di evasione ordini*/
        case 4:
                evadi_ordine(p);
                break;
        /*richiama la lista dei piatti in attesa*/
        case 5:
				lista_piatti_attesa(p);
                break;
        /*richiama la gestione di un sollecito*/
        case 6:
				gestisci_sollecito(p);
                break;
        case 7:
                memorizza_info(p);
                break;
        /*richiama la cancellazione di un ordine*/
        case 8:
				printf("il tavolo %d puo' ora completare l'ordine\n", p.tavolo);
                head = cancella_pacchetto(p);
                break;
		/*richiama la stampa delle statistiche della cucina*/
        case 9:
                stampa_statistiche();
                break;
		/*richiama l'invio del conto ad un tavolo*/
        case 10:
				invia_conto(p);
				break;
        }
}

/*gestione inizializzazione delle aree di memoria condivisa
 *	client 				zona condivisa per gli identificatori dei client connessi
 *  tempo_medio 		zona condivisa per il tempo medio di servizio
 *  tempo_di_servizio	zona condivisa per il tempo di servizio di un singolo ordine
 *  tempo_totale		zona condivisa per il tempo totale di servizio del totale degli ordini
 *  incasso				zona condivisa per l'incasso totale del ristorante
 *  piatti_pronti		zona condivisa per la lista dei piatti pronti in attesa
 */
void shared() {
	int                             shmid1, shmid2,shmid3,shmid4,shmid5,shmid6, shmid8;

    if((shmid1 = (shmget(IPC_PRIVATE, sizeof(int), 0777))) < 0) {
            err_sys("errore nell shmget");
    }

    if((shmid2 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
            err_sys("errore nell shmget");
    }
    if((shmid3 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
            err_sys("errore nell shmget");
    }
    if((shmid4 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
            err_sys("errore nell shmget");
    }

    if((shmid5 = (shmget(IPC_PRIVATE, sizeof(int), 0600))) < 0) {
            err_sys("errore nell shmget");
    }

    if((shmid6 = (shmget(IPC_PRIVATE, sizeof(int)*50, 0600))) < 0) {
            err_sys("errore nell shmget");
    }
    if((shmid8 = (shmget(IPC_PRIVATE, sizeof(double), 0600))) < 0) {
            err_sys("errore nell shmget");
    }


/*
    if((shmid7 = (shmget(IPC_PRIVATE, sizeof(struct sockaddr_in), 0600))) < 0) {
                err_sys("errore nell shmget");
        }
    cliaddr = (struct sockaddr_in *) shmat(shmid7, 0, 0);
 */


    client = (int *) shmat(shmid1, 0, 0);
    tempo_medio = (double *) shmat(shmid2, 0, 0);
    tempo_di_servizio = (double *) shmat(shmid3, 0, 0);
    tempo_totale = (double *) shmat(shmid4, 0, 0);
    incasso = (int *) shmat(shmid5, 0, 0);
    piatti_pronti = (int *) shmat(shmid6, 0, 0);
    info = (double *) shmat(shmid8, 0, 0);
}


/*MAIN*/
int main(int argc, char **argv) {

        int                             /*listensd, connsd,*/clilen,port,ready,/*client[FD_SETSIZE],*/i,maxi,maxd,n, dimensione,q;
        char                            rcv[MAXLINE],data[MAXLINE], send[MAXLINE];
        socklen_t                       servaddr_len,cliaddr_len;
        fd_set                          rset,allset;
        time_t                          ticks;
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

        shared();

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


        init_dispensa();

    	signal(SIGPIPE, SIG_IGN);


        for(;;) {

                rset = allset;
                timeout.tv_sec = 6000;
                timeout.tv_usec = 0;

                /* eseguo la select e quando il socket e' attivo faccio accept */
                if((ready = select(maxd+1,&rset,NULL,NULL,&timeout)) < 0)
                        err_sys("errore nella select");

                if(ready==0) {
                    ticks = time(NULL);
                    snprintf(data,sizeof(data),"%.24s\r\n", ctime(&ticks));
                    printf("\nchiusura cucina %.24s\n", ctime(&ticks));
                	stampa_statistiche();
                    Close(connsd);
                    exit(0);
                }

                /* se ci sono gestisco richieste di connessione */
                if(FD_ISSET(listensd,&rset)) {

                        cliaddr_len = sizeof(cliaddr);

						/*connsd=Accept(listensd, (struct sockaddr *) &*cliaddr, (socklen_t *)&clilen);*/
                        connsd=Accept(listensd, (struct sockaddr *) &cliaddr, (socklen_t *)&clilen);
                        Getsockname(listensd,(struct sockaddr *)&servaddr,&servaddr_len);
                        Getpeername(connsd,(struct sockaddr *)&cliaddr,&cliaddr_len);

                        printf("\nserver --> *** indirizzo IP: %s\tPORTA: %d ***\n",inet_ntoa(servaddr.sin_addr),ntohs(servaddr.sin_port));
                        printf("connessione accettata\n");
                        printf("client --> *** indirizzo IP: %s\tPORTA: %d ***\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

                        /* memorizzo la connessione dei client */
                        for(i=1;i<FD_SETSIZE;i++) {
                                if(client[i] < 0) {
                                        client[i] = connsd;
                                        printf("\n\t dentro il padre connsd = %d\n",client[i]);
                                        printf("\ncameriere [%d] connesso!\n\n", i);
                                        p.nome_cameriere = i;
                                        p.protocollo=1;
                                        memcpy(send,&p,sizeof(pacchetto));
                                        Write(client[i],send,sizeof(send));
                                        printf("STRUTTURA CLIENT CONNESSIONE ACCETTATA\n");
                                        camerieri++;
                                        for (q=1; q<10; q++){
                                               	printf ("client[%d]=%d\n", q, client[q]);
                                        }
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
                                        printf("STRUTTURA CLIENT CONNESSIONE CADUTA\n");
                                        for (q=1; q<10; q++){
                                               	printf ("client[%d]=%d\n", q, client[q]);
                                        }
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
