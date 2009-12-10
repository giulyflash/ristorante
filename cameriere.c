#include "basic.h"
#include "esame.h"

int me, pronti=0, cont=0, piatto_backup, tavolo_backup;
struct timeval          starttime,endtime;
double  tempo_medio, tempo_di_servizio, tempo_totale, medio;

pacchetto *temporaneo;

void gestisci_protocollo_client(pacchetto p) {
	pacchetto tmp;
        switch(p.protocollo) {
        case 1:
                me = p.nome_cameriere;
                printf("benvenuto cameriere %d \n", me);
                menu_cameriere();
                break;
        case 3:
                menu_cameriere();
                break;
        case 4:
                printf("\n\t\033[31m *** tavolo servito ***\033[0m\n");
                break;
        case 5:
        	   coda_tavoli(p);
                break;

        case 6:
                modifica_ordine(p);
                break;
        case 7:
                printf("\n\033[31m *** la cucina sta preparando il piatto %s per il tavolo %d ***\033[0m\n",p.messaggio, p.tavolo);
                menu_cameriere();
                break;
        case 8:
                printf("\n\033[31m *** l' ordine per questo tavolo e' gia stato memorizato ***\033[0m\n") ;
                menu_cameriere();
                break;
        case 9:
                printf("\n\033[31m *** il piatto %d non e' disponibile [ordine eliminato, riprendi l'ordine] ***\033[0m\n", p.esauriti) ;
                p.modificato = 1;
                tmp.tavolo = p.tavolo;
                my_send(8,p);
                menu_cameriere();
                break;
        case 10:
                printf("\n\033[31m *** ordine non presente in lista ***\033[0m\n");
                menu_cameriere();
                break;
        case 11:
				printf("hai selezionato il tavolo sbagliato\n");
				pronti=1;
				break;
        case 12:
                printf("\n\033[31m *** la cucina ha terminato di preparare il piatto %d per il tavolo %d ***\033[0m\n", p.pronti,p.tavolo);
                cont++;
                pronti=1;
                if(cont==1) {
                	piatto_backup = p.pronti;
                	tavolo_backup = p.tavolo;
                if ( gettimeofday(&starttime, NULL) < 0 )
                        err_sys("gettime error");
                }
                if(p.esauriti==0) {
                	*temporaneo = p;
                }
                menu_cameriere();
                break;
        case 13:
                printf("\n\033[31m *** OTTIMO LAVORO!! ***\033[0m\n");
                menu_cameriere();
                break;
        case 14:
                printf("\n\033[31m *** non hai servito il tuo tavolo, la cucina ha passato l'ordine a un altro cameriere ***\033[0m\n");
                menu_cameriere();
                break;

        case 15:
        	if(p.conto == 0) {
        		printf("conto saldato\n");
        		menu_cameriere();
        	} else {
				printf("\n\033[31m *** il conto e' di %d euro ***\033[0m\n", p.conto);
				menu_cameriere();
        	}
                break;


        case 20:
                printf("sono terminati gli ingredienti per il piatto %d \n", p.esauriti);
                p.error=1;
                my_send(8,p);
                modifica_ordine(p);
                break;

        }
}

pacchetto scegli_piatti(pacchetto tmp) {
	char c[1];
	int piatto, porzioni, i = 0, tavolo, nome = tmp.nome_cameriere, control = 0;
	pacchetto p;

	while (control != 1) {
		printf("\nper quale tavolo stai prendendo l'ordine?\n");
		scanf("%d", &tavolo);
		getchar();
		if ((tavolo > 20) || (tavolo == 0)) {
			control = 0;
			printf("errore\n");
		} else {
			control = 1;
		}
	}
	while ((c[0] != 'n')) {
		control = 0;
		while (control != 1) {
			printf("cosa desidera\n");
			scanf("%d", &piatto);
			getchar();
			if ((piatto > 9) || (piatto <= 0)) {
				printf("\nerrore: il menu ha solo 10 piatti [1-10]\n");
				control = 0;
			} else {
				control = 1;
				p.ordine[i] = piatto;
			}
		}

		control = 0;
		while (control != 1) {
			fflush(stdin);
			porzioni = 0;
			printf("quante porzioni [devi ordinare almeno una porzione]\n");
			scanf("%d", &porzioni);
			getchar();
			if (porzioni == 0) {
				printf("errore: devi ordinare almeno una porzione di ogni piatto\n");
				control = 0;
			} else {
				control = 1;
				p.ordine[i + 1] = porzioni;
				i += 2;
			}

		}
		if(control==1) {
		printf("altro? s/n \n");
		read(0, &c[0], sizeof(char));
		}

	}

	memset(&c, '\0', sizeof(c));
	p.tavolo = tavolo;
	p.nome_cameriere = nome;
	p.ordine[i] = '\0';

	return (p);
}

void nuovo_ordine() {
        pacchetto p, tmp;
        my_send(1,p);   /* dico alla cucina che devo fare un nuovo ordine [protocollo: 1] */
        leggi_menu(sockfd);     /* la cucina mi invia il menu e lo stampo a video */
        tmp = scegli_piatti(p);
        tmp.sollecito=1;
        if(tmp.ordine[0] != 0) {
        	my_send(2,tmp);    /* a questo punto invio alla cucina il nuovo ordine [protocollo: 2] */
        }
        menu_cameriere();
}

void modifica_ordine(pacchetto p) {

	int control = 0;
	int i = 0, piatto = 0, porzioni = 0, count;
	int operazione;
	char c[1];
	stampa_ordine(p);

	printf("\n1. aggiungi piatto\n");
	printf("2. cancella piatto\n");
	printf("3. torna al menu principale\n");
	printf("\ncosa desideri fare:\n");
	scanf("%d", &operazione);
	switch (operazione) {
	case 1:
		p.modificato = 1;
		count = 0, i = 0;
		while (p.ordine[i] != '\0') {
			count++;
			i++;
		}
		while (c[0] != 'n') {
			control = 0;
			while (control != 1) {
				printf("cosa desidera\n");
				scanf("%d", &piatto);
				getchar();
				if ((piatto > 9) || (piatto <= 0)) {
					printf("\nerrore: il menu ha solo 10 piatti [1-10]\n");
					control = 0;
				} else {
					control = 1;
					p.ordine[i] = piatto;
				}
			}
			control = 0;

			while (control != 1) {
				printf("quante porzioni [devi ordinare almeno una porzione]\n");
				scanf("%d", &porzioni);
				getchar();
				if (porzioni == 0) {
					printf("errore: devi ordinare almeno una porzione di ogni piatto\n");
					control = 0;

				} else {
					control = 1;
					p.ordine[i + 1] = porzioni;
					i += 2;
					count+=2;
				}
			}
			printf("altro? s/n \n");
			read(0, &c[0], sizeof(char));

		}
		p.esauriti = count;
		break;

	case 2:
		p.modificato = 2;
		count = 0, i = 0;
		printf("%d\n",count);
			printf("quale piatto vuoi eliminare\n");
			scanf("%d", &piatto);
			while (p.ordine[i] != '\0') {
				if (p.ordine[i] == piatto) {
					printf("trovato\n");
						p.ordine[i] = piatto;
						p.ordine[i + 1] = 0;
						printf("p.ordine[i + 1] = %d\n", p.ordine[i + 1]);
						i += 2;
				}
				i += 2;
			}
		p.esauriti=count;
		break;
	case 3:
		menu_cameriere();
		break;
	default:
		printf("\nhai fatto una scelta non valida\n");
		menu_cameriere();
		break;
	}
	if(p.error==1) {
		p.ordine[1]=0;
		p.modificato=0;
	}
	my_send(2, p);
}

void esci() {
        exit(0);
}
void servi_piatto(pacchetto p) {
	pacchetto tmp;
	int tavolo;
	int piatto;
	printf("quale tavolo vuoi servire?\n");
    scanf("%d",&tavolo);
    printf("quale piatto?\n");
    scanf("%d",&piatto);
    if((tavolo==tavolo_backup) && (piatto==piatto_backup)) {
    if(pronti==1) {
		if ( gettimeofday(&endtime,NULL) < 0 )
				err_sys("gettime error");
		tempo_di_servizio = (endtime.tv_sec+(endtime.tv_usec/1000000.0)) - (starttime.tv_sec+(starttime.tv_usec/1000000.0));
		tempo_totale += tempo_di_servizio;
		printf("l'ordine e' stato evaso in %fl secondi\n", tempo_di_servizio);
		printf("il tempo totale di evasione degli ordini e' di %f secondi\n", tempo_totale);
		tmp.tavolo=tavolo;
		tmp.tempi[0] = tempo_di_servizio;
		tmp.tempi[1] = tempo_totale;
		my_send(7,tmp);
    	p.servito = 1;
    	p.esauriti=piatto;
    	p.nome_cameriere = me;
    	my_send(4,p);
    	cont=0;
    	pronti=0;
    }
    }
   else
  	printf("\ni piatti non sono ancora pronti [aspetta la notifica della cucina], o hai digitato male l'id tavolo\n");
    menu_cameriere();
}

void invia_sollecito() {
	pacchetto p;
	printf("per quale tavolo vuoi inviare il sollecito?\n");
	scanf("%d", &p.tavolo);
	my_send(6,p);
	printf("sollecito inviato\n");
	menu_cameriere();
}

void richiedi_conto() {
	pacchetto p;
	printf("per quale tavolo vuoi richiedere il conto? [devi prima aver servito tutti i piatti] \n");
	scanf("%d", &p.tavolo);
	getchar();
	if(p.tavolo==tavolo_backup) {
		printf("inviato\n");
		p.nome_cameriere=me;
		my_send(10,p);
	} else
		printf("hai digitato un id tavolo non corretto\n");
	menu_cameriere();
}

void gestisci_input(char *input_cameriere) {
		pacchetto p;
        switch(atoi(input_cameriere)) {
        case 1:
                nuovo_ordine();
                break;
        case 2:
                printf("per quale tavolo vuoi modificare l'ordine?\n");
                scanf("%d",&p.tavolo);
                my_send(3,p);
                break;
        case 3:
				invia_sollecito();
                break;
        case 4:
        	if(pronti==1)
				printf("\n\til tavolo %d attende il piatto %d\n", tavolo_backup, piatto_backup);
        	else
        		printf("\n\tnon ci sono piatti pronti\n");
				menu_cameriere();
                break;
        case 5:
                servi_piatto(p);
                break;
        case 6:
				richiedi_conto();
                break;
        case 7:
                esci();
                break;

        case 9:
                my_send(9,p);
                menu_cameriere();
                break;
        default:
                printf("\nhai effettuato una scelta non valida \n");
                menu_cameriere();
                break;
        }
}

int main(int argc, char **argv) {

        int port, maxd, n;
        char buff[MAXLINE], rcv[sizeof(pacchetto)], input_cameriere[sizeof(pacchetto)];
        struct hostent *he;
        struct sockaddr_in servaddr, localaddr, peeraddr;
        socklen_t localaddr_len = sizeof(localaddr), peeraddr_len = sizeof(peeraddr);
        fd_set rset;
        pacchetto p;
        int control;
        struct timeval timeout;
        p.nome_cameriere = 0;


        if(argc!=3)
                err_quit("Usage: %s <hostname | IP address> <Port>\n",argv[0]);

        port=atoi(argv[2]);

        if( (port > 12000) || (port < 10000) )
                err_sys("Hai scelto una porta non consentita");

        memset(&servaddr,'\0',sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);

        he = Gethostbyname(argv[1]);

        memcpy(&servaddr.sin_addr.s_addr, he->h_addr, he->h_length);

        sockfd = Socket(AF_INET, SOCK_STREAM,0);

        Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

        Getsockname(sockfd,(struct sockaddr *)&localaddr,&localaddr_len);

        Getpeername(sockfd,(struct sockaddr *)&peeraddr,&peeraddr_len);

        system("clear");

        printf("\nclient locale --> *** indirizzo IP: %s PORTA: %d ***\n",inet_ntop(AF_INET,&localaddr.sin_addr,buff,sizeof(buff)),ntohs(localaddr.sin_port));
        printf("server remoto --> *** indirizzo IP: %s PORTA: %d ***\n",inet_ntop(AF_INET,&peeraddr.sin_addr,buff,sizeof(buff)),ntohs(peeraddr.sin_port));

        Signal(SIGPIPE, SIG_IGN);

        maxd = (MAX(0,sockfd))+1;
        timeout.tv_sec = 6000;
        timeout.tv_usec = 0;
        for(;;) {


                memset(input_cameriere,'\0',sizeof(input_cameriere));
                memset(rcv,'\0',sizeof(rcv));

                FD_ZERO(&rset);
                FD_SET(fileno(stdin),&rset);
                FD_SET(sockfd,&rset);

                if((n = select(maxd,&rset,NULL,NULL,&timeout)) < 0)
                        err_sys("errore nella select\n");
                if(n==0) {
                	printf("\n\tnon ci sono clienti, vai a fare un giro. . .\n\n");
                    Close(sockfd);
                    exit(0);
                }

                if(FD_ISSET(sockfd,&rset)) {
                       control = Read2(sockfd,rcv,sizeof(pacchetto));
                        if(control==0) {
                        	printf("cucina chiusa\n");
                        	break;
                        }
                        memcpy(&p,rcv,sizeof(pacchetto));
                        gestisci_protocollo_client(p);
                }
                if(FD_ISSET(fileno(stdin),&rset)) {
                        fgets(input_cameriere,sizeof(input_cameriere),stdin);
                        if(strlen(input_cameriere) <= 1)
                                continue;
                        gestisci_input(input_cameriere);
                }
        }
        Close(sockfd);
        exit(0);
}
