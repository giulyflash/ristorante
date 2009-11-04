/*
 * esame.h
 *
 *  Created on: Apr 17, 2009
 *      Author: 10011342
 */

typedef struct pacchetto{
	int protocollo;
	int nome_cameriere;
	int tavolo;
	int ordine[50];
	char messaggio[20];
	int pr;
	int servito;
	struct pacchetto *next;
}pacchetto;

pacchetto *head, *tail;

int sockfd;

pacchetto *newPacchetto(void);
void invia_menu(int connsd);
void leggi_menu(int sockfd);
void stampa_ordine(pacchetto p);
void menu_cameriere();
void my_send(int protocollo, pacchetto p);
void leggi_ordine(int connsd,pacchetto p, int i);
void gestisci_protocollo_server(pacchetto p, int connsd,int cameriere);
void gestisci_protocollo_client(pacchetto p);
pacchetto *cancella_pacchetto(pacchetto p, int connsd);
