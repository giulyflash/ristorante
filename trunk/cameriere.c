#include "basic.h"
#include "esame.h"

pacchetto scegli_piatti(pacchetto tmp) {
	char *send = malloc(sizeof(pacchetto));
	char c[1];
	int piatto, porzioni, i = 0, tavolo, nome = tmp.nome_cameriere, error, control = 0, exit = 0;
	pacchetto p;

	while (control != 1) {
		printf("\nper quale tavolo stai prendendo l'ordine?\n");
		scanf("%d", &tavolo);
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
			if (porzioni == 0) {
				printf(
						"errore: devi ordinare almeno una porzione di ogni piatto\n");
				control = 0;
			} else {
				control = 1;
				p.ordine[i + 1] = porzioni;
				i += 2;
			}
		}

		printf("altro? s/n \n");
		read(0, &c[0], sizeof(char));

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
        if(tmp.ordine[0] != 0) {
        	my_send(2,tmp);    /* a questo punto invio alla cucina il nuovo ordine [protocollo: 2] */
        }
        menu_cameriere();
}

void modifica_ordine(pacchetto p) {

	int control=0;
        int i=0, piatto=0, porzioni=0, cont=0;
        int operazione;
        char c[1];
        stampa_ordine(p);

        printf("\n1. aggiungi piatto\n");
        printf("2. modifica porzioni\n");
        printf("3. cancella piatto\n");
        printf("4. torna al menu principale\n");
        printf("\ncosa desideri fare:\n");
        scanf("%d",&operazione);
        switch(operazione) {
        case 1:
                i=0;
                while(p.ordine[i]!='\0') {
                        cont++;
                        i++;
                }
                while(c[0] != 'n') {

                    control = 0;
                    while (control != 1){
                printf("cosa desidera\n");
                scanf("%d", &piatto);
                if ((piatto > 9) || (piatto <= 0)) {
                         printf("\nerrore: il menu ha solo 10 piatti [1-10]\n");
            	 		control = 0;
            	}
            	else {
            		control = 1;
            		p.ordine[i] = piatto;
            	}
            }
                    control = 0;
                    while (control != 1){
                        printf("quante porzioni [devi ordinare almeno una porzione]\n");
                        scanf("%d", &porzioni);
                        if (porzioni==0) {
                                 printf("errore: devi ordinare almeno una porzione di ogni piatto\n");
                    	 		control = 0;
                    	}
                    	else {
                    		control = 1;
                    		p.ordine[i + 1] = porzioni;
                    		i += 2;
                    	}
                    }
                        printf("altro? s/n \n");
                        read(0,&c[0],sizeof(char));

                }

                break;

        case 2:
    		i=0;
    		while(c[0] != 'n') {
    			printf("di quale piatto vuole modificare le porzioni?\n");
    			scanf("%d", &piatto);
    			while(p.ordine[i] != '\0') {
    				if(p.ordine[i] == piatto) {
    					printf("quante porzioni\n");
    					scanf("%d", &porzioni);
    					p.ordine[i+1] = porzioni;
    					i+=2;
    				}
    				i+=2;
    			}
    			printf("vuoi modificare altro? s/n \n");
    			read(0,&c[0],sizeof(char));
    		}
    		break;

                break;
        case 3:
                i=0;
                while(c[0] != 'n') {
                        printf("quale piatto vuoi eliminare?\n");
                        scanf("%d", &piatto);
                        while(p.ordine[i] != '\0') {
                                if(p.ordine[i] == piatto) {
                                        p.ordine[i] = 0;
                                        p.ordine[i+1] = 0;
                                        i+=2;
                                }
                                i+=2;
                        }
                        printf("vuoi eliminare altro? s/n \n");
                        read(0,&c[0],sizeof(char));
                }
                break;
        case 4:
                menu_cameriere(sockfd,p);
                break;
        default:
                printf("\nhai fatto una scelta non valida\n");
                menu_cameriere();
                break;
        }

        my_send(2,p);
        menu_cameriere();
}

void gestisci_protocollo_client(pacchetto p) {
        switch(p.protocollo) {
        case 3:
                menu_cameriere();
                break;
        case 4:
                printf("\n\t\033[31m *** tavolo servito ***\033[0m\n");
                menu_cameriere();
                break;
        case 5:
				servi_piatto(p);
                break;

        case 6:
                modifica_ordine(p) ;
                break;
        case 7:
                printf("\n\033[31m *** è pronto il piatto %s per il tavolo %d ***\033[0m\n",p.messaggio, p.tavolo);
                menu_cameriere();
                break;
        case 8:
                printf("\n\033[31m *** l' ordine per questo tavolo e' gia stato memorizato ***\033[0m\n") ;
                menu_cameriere();
                break;
        case 10:
                printf("\n\033[31m *** ordine non presente in lista ***\033[0m\n");
                break;
        case 20:
                printf("sono terminati gli ingredienti per il piatto %s \n", p.messaggio);
                break;

        }
}



void sollecita_ordine() {
	pacchetto p;
	char send[sizeof(pacchetto)];
	printf("per quale tavolo vuoi inviare un sollecito\n");
	scanf("%d", &p.tavolo);
	p.protocollo = 9;
	memcpy(send,&p,sizeof(pacchetto));
	Write(sockfd,send,sizeof(send));
	printf("invio un sollecito");
}

void esci() {
        exit(0);
}
void menu_cameriere() {
        printf("\t\n\033[34m *** applicazione di rete per la ristorazione ***\033[0m\n\n");
        printf("\t1. nuovo ordine\n");
        printf("\t2. modifica ordine\n");
        printf("\t3. sollecita ordine\n");
        printf("\t4. servi piatto\n");
        printf("\t5. esci\n");
        printf("\ncosa desideri fare:\n");
}

void servi_piatto(pacchetto p) {
	int i=0;
    while(p.ordine[i]!= '\0') {
            printf("tavolo %d in attesa di essere servito\n",p.ordine[i]);
            i++;
    }
    if(p.ordine[i]!= '\0') {
		printf("quale ordine vuoi evadere?\n");
		scanf("%d",&p.tavolo);
		my_send(4,p);
    } else {
    	printf("non ci sono tavoli da servire\n");
    	menu_cameriere();
    }
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
				printf("CUCINA SBRIGATI!!\n");
				my_send(6,p);
				menu_cameriere();
                break;
        case 4:
				my_send(5,p);
                break;
        case 5:
                esci();
                break;
        default:
                printf("\nhai effettuato una scelta non valida \n");
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
        p.nome_cameriere = 0;
        /*      struct timeval          timeout; */


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
        menu_cameriere();
        for(;;) {

                memset(input_cameriere,'\0',sizeof(input_cameriere));
                memset(rcv,'\0',sizeof(rcv));

                FD_ZERO(&rset);
                FD_SET(fileno(stdin),&rset);
                FD_SET(sockfd,&rset);

                if((n = select(maxd,&rset,NULL,NULL,NULL)) < 0)
                        err_sys("errore nella select\n");

                if(FD_ISSET(sockfd,&rset)) {
                        Read2(sockfd,rcv,sizeof(pacchetto));
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
