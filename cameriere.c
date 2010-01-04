/*
 * cameriere.c
 *
 *  Created on: Apr 17, 2009
 *      Author: Sergio Urbano, Alberto Lavezzari
 */

#include "basic.h"
#include "esame.h"

int me, pronti=0, cont=0, piatto_backup, tavolo_backup;
struct timeval          starttime,endtime;
double  tempo_medio, tempo_di_servizio, tempo_totale, medio;

pacchetto *temporaneo;

/*gestione del protocollo di comunicazione lato client*/
void gestisci_protocollo_client(pacchetto p) {
	pacchetto tmp;
        switch(p.protocollo) {
        /* messaggio di benvenuto: memorizzo il nome del cameriere all'interno di una var globale che usero' piu' avanti */
        case 1:
                me = p.nome_cameriere;
                printf("benvenuto cameriere %d \n", me);
                menu_cameriere();
                break;
        /*torna al menu principale*/
        case 3:
                menu_cameriere();
                break;
        /*segnalazione tavolo servito correttamente*/
        case 4:
                printf("\n\t\033[31m *** tavolo servito ***\033[0m\n");
                break;
        /*gestione stampa coda tavoli in attesa*/
        case 5:
        	   coda_tavoli(p);
                break;
        /*gestione modifica ordine*/
        case 6:
                modifica_ordine_client(p);
                break;
        /*notifica di piatto in preparazione*/
                /*
        case 7:
                printf("\n *** la cucina sta preparando il piatto %s per il tavolo %d ***\n",p.messaggio, p.tavolo);
                menu_cameriere();
                break;
                */
        /*segnalazione di ordine gia' presente*/
        case 8:
                printf("\n *** l' ordine per questo tavolo e' gia stato memorizato ***\n") ;
                menu_cameriere();
                break;
        /*segnalazione di errore nell'ordine*/
        case 9:
                printf("\n *** il piatto %d non e' disponibile [ordine eliminato, riprendi l'ordine] ***\n", p.esauriti) ;
                p.modificato = 1;
                tmp.tavolo = p.tavolo;
                my_send(8,p);
                menu_cameriere();
                break;
        /*segnalazione ordine non presente*/
        case 10:
                printf("\n *** ordine non presente in lista ***\n");
                menu_cameriere();
                break;
        /*segnalazione errore selezione nella selezione tavolo*/
        case 11:
				printf("hai selezionato il tavolo sbagliato\n");
				pronti=1;
				break;
		/*segnalazione di piatto pronto per il tavolo*/
        case 12:
                printf("\n *** la cucina ha terminato di preparare il piatto %d per il tavolo %d ***\n", p.pronti,p.tavolo);
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
        /*segnalazione di corretta conclusione dell'operazione*/
        case 13:
                printf("\n *** OTTIMO LAVORO!! ***\n");
                menu_cameriere();
                break;
        /*segnalazione eccessivo ritardo del cameriere*/
        case 14:
                printf("\n *** non hai servito il tuo tavolo, la cucina ha passato l'ordine a un altro cameriere ***\n");
				cont=0;
        		if ( gettimeofday(&endtime,NULL) < 0 )
        				err_sys("gettime error");
                menu_cameriere();
                break;
        /*gestione stampa del conto, controllo se il valore relativo al conto della nostra struttura e' stato modificato [ordine evaso] oppure no*/
        case 15:
        	if(p.conto == 0) {
        		printf("conto saldato\n");
        		menu_cameriere();
        	} else {
				printf("\n *** il conto e' di %d euro ***\n", p.conto);
				menu_cameriere();
        	}
                break;
        /*segnalazione esaurimento ingradienti per il piatto selezionato*/
        case 20:
                printf("sono terminati gli ingredienti per il piatto %d \n", p.esauriti);
                p.error=1;
                my_send(8,p);
                modifica_ordine_client(p);
                break;

        }
}

/*gestione input da tastiera*/
void gestisci_input(char *input_cameriere) {
		pacchetto p;
        switch(atoi(input_cameriere)) {
        /*richiama generazione di un nuovo ordine*/
        case 1:
                nuovo_ordine();
                break;
		/*richiama la modifica di un ordine*/
        case 2:
                printf("per quale tavolo vuoi modificare l'ordine?\n");
                scanf("%d",&p.tavolo);
                my_send(3,p);
                break;
		/*richiama l'invio di un sollecito alla cucina*/
        case 3:
				invia_sollecito();
                break;
		/*richiama la lista di piatti pronti per il tavolo*/
        case 4:
        	if(pronti==1)
				printf("\n\til tavolo %d attende il piatto %d\n", tavolo_backup, piatto_backup);
        	else
        		printf("\n\tnon ci sono piatti pronti\n");
				menu_cameriere();
                break;
		/*richiama la funzione per servire il piatto*/
        case 5:
                servi(p);
                break;
		/*richiama la richiesta di conto*/
        case 6:
				richiedi_conto();
                break;
		/*terminazione*/
        case 7:
                esci();
                break;
		/*easter egg: permette al cameriere di stampare le statistiche della cucina (VENGONO VISUALIZZATE SUL SERVER)*/
        case 9:
                my_send(9,p);
                menu_cameriere();
                break;
		/*segnalazione di scelta non valida*/
        default:
                printf("\nhai effettuato una scelta non valida \n");
                menu_cameriere();
                break;
        }
}




/*	gestione nuovo ordine
 *	dico alla cucina che voglio fare un nuovo ordine
 *	questa mi invia il menu
 *	lo leggo
 * 	scelgo i piatti tramite la funzione appositamente creata
 * 	imposto sollecito a 1 [default]
 * 	invio l'ordine con il protocollo relativo al nuovo ordine
 *
 * 		|PROTOCOLLO|id cameriere|id tavolo|lista_piatti|PAYLOAD|
 *
 */
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


/* gestione della scelta dei piatti
 * memorizzo le informazioni utili per creare l'ordine: id tavolo, id piatto, porzioni.
 * controlli vengono effettuati su ogni campo.
 * una volta finito di memorizzare i piatti ritorno un pacchetto cosi formato:
 *
 * 				|id cameriere|id tavolo|lista_piatti|PAYLOAD|
 *
 */
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

/*	gestione modifica ordine
 *	il cameriere puo' aggiungere un piatto o cancellarlo
 * 	memorizzo le informazioni come nel caso della scegli_piatti
 *	imposto il flag ordine modificato e invio il pacchetto con
 *	il numero di protocollo corrispondente.
 */


void modifica_ordine_client(pacchetto p) {

	int control = 0;
	int i = 0, piatto = 0, porzioni = 0, count, tav=p.tavolo,nome=p.nome_cameriere;
	int operazione;
	char c[1];
	stampa_ordine(p);
	memset(&p,'\0',sizeof(pacchetto));
	p.tavolo=tav;
	p.nome_cameriere=nome;
	p.sollecito=1;
	printf("\n1. aggiungi piatto\n");
	printf("2. cancella piatto\n");
	printf("3. torna al menu principale\n");
	printf("\ncosa desideri fare:\n");
	scanf("%d", &operazione);
	switch (operazione) {
	/*aggiunta piatto*/
	case 1:
		p.modificato = 2;
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
	/*cancellazione piatto*/
	case 2:
		p.modificato = 2;
		count = 0, i = 0;
			printf("quale piatto vuoi eliminare\n");
			scanf("%d", &piatto);
			while (p.ordine[i] != '\0') {
				if (p.ordine[i] == piatto) {
					printf("piatto eliminato\n");
						p.ordine[i] = piatto;
						p.ordine[i + 1] = 0;
						i += 2;
				}
				i += 2;
			}
		p.esauriti=count;
		break;
	/*ritorno al menu*/
	case 3:
		menu_cameriere();
		break;
	/*segnalazione di scelta non valida*/
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
	menu_cameriere();
}

/*terminazione client*/
void esci() {
        exit(0);
}

/*	gestione consegna piatto
 *	memorizzo il numero del tavolo da servire
 * 	memorizzo il piatto da servire
 * 	controllo se queste due scelte sono corrette
 * 	in caso positivo servo il piatto [calcolo i tempi di servizio e li memorizzo]
 *  invio al server i dati utili.
 */
void servi(pacchetto p) {
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

/*	gestione del sollecito verso la cucina
 *	memorizzo il tavolo
 *	invio alla cucina il sollecito mettendo come protocollo quello corrispondente
 */
void invia_sollecito() {
	pacchetto p;
	printf("per quale tavolo vuoi inviare il sollecito?\n");
	scanf("%d", &p.tavolo);
	my_send(6,p);
	printf("sollecito inviato\n");
	menu_cameriere();
}

/*	gestione richiesta conto per il tavolo
 *	scelgo il tavolo di cui stampare il conto
 * 	controllo se esiste
 *	in caso positivo visualizzo il conto
 *	in caso negativo stampo un warning
 */
void richiedi_conto() {
	pacchetto p;
	printf("per quale tavolo vuoi richiedere il conto? [devi prima aver servito tutti i piatti] \n");
	scanf("%d", &p.tavolo);
	getchar();
	if(p.tavolo==tavolo_backup) {
		printf("richiesta inviata\n");
		p.nome_cameriere=me;
		my_send(10,p);
	} else
		printf("hai digitato un id tavolo non corretto\n");
	menu_cameriere();
}

/*MAIN*/
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

        signal(SIGPIPE, SIG_IGN);

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
